/*!
* @file     supervisorhandler.cpp
* @author   Agiliad
* @brief    This file contains class declaration for supervisor workstation connections.
* @date     Jul, 11 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include "supervisorhandler.h"

namespace analogic
{
namespace ws
{
/*!
* @fn       SupervisorHandler
* @param    None
* @return   None
* @brief    Constructor for class SupervisorHandler.
*/
SupervisorHandler::SupervisorHandler()
{
  TRACE_LOG("");
  THROW_IF_FAILED(m_nssAccesslayer == nullptr?Errors::E_POINTER : Errors::S_OK);
  m_nssAccesslayer->createSupervisorAgentListener();
  if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::SUPERVISOR_WORKSTATION)
  {
    analogic::ws::BhsBagListenerInterface* listner = m_nssAccesslayer->getBHSBagListnerInterface();
    THROW_IF_FAILED((listner == nullptr ? Errors::E_POINTER : Errors::S_OK));
    connect(listner, &analogic::ws::BhsBagListenerInterface::sigReviewBag, this,
            &SupervisorHandler::onReviewBag, Qt::DirectConnection);
  }
}

/*!
* @fn       SupervisorHandler
* @param    NSSAgentAccessInterface*
* @return   None
* @brief    Constructor for class SupervisorHandler, needed for mocking nss in unit testing.
*/
SupervisorHandler::SupervisorHandler(NSSAgentAccessInterface* nssaccess):NssHandler(nssaccess)
{
  TRACE_LOG("")
}

/*!
* @fn       ~SupervisorHandler
* @param    None
* @return   None
* @brief    Destructor for class SupervisorHandler.
*/
SupervisorHandler::~SupervisorHandler()
{

}

/*!
* @fn       onUpdateState();
* @param    QMLEnums::AppState state
* @return   void
* @brief    call on update state.
*/
void SupervisorHandler::onUpdateState(QMLEnums::AppState state)
{
  Q_UNUSED(state);
  TRACE_LOG("")
}

/*!
* @fn       getSearchBagList
* @param    void
* @return   void
* @brief    Gets Search BagList.
*/
// Search List not available in Supervisor Agent
//void SupervisorHandler::getSearchBagList()
//{
//  m_nssAccesslayer->searchListUpdated();
//}

/*!
* @fn       onReviewBag
* @param    std::string &bhsid
* @return   None
* @brief    slot gets called on review bag from BHS
*/
void SupervisorHandler::onReviewBag(const std::string &bhsid)
{
  INFO_LOG("SupervisorHandler::onReviewBag: Got request to review bag: " << bhsid);
  if((BagsDataPool::getInstance() == NULL))
  {
    ERROR_LOG("BHS::BagsDataPool is null. Ignoring bag review request from BHS.");
    return;
  }
  if(!m_nssAccesslayer->isUserLoggedIn())
  {
    ERROR_LOG("BHS::User is not logged in. We can not accept bag review request from BHS.");
    return;
  }

  bool isFreeBaginQueue = false;

  boost::shared_ptr<BagData> bag = BagsDataPool::getInstance()->getBagbyState(BagData::INITIALISED | BagData::CONSTRUCTION |
                                                                              BagData::READYTORENDER);
  if(bag != NULL)
  {
    ERROR_LOG("BHS::Can not accept bag request from BHS as Bag is getting load.");
    return;
  }
  else
  {
    isFreeBaginQueue = true;
  }

  boost::shared_ptr<BagData> retainbag = BagsDataPool::getInstance()->getBagbyState(BagData::RENDERING);
  if(retainbag != NULL)
  {
    if(retainbag->getBHSBagId() != bhsid)
    {
      INFO_LOG("BHS::Retaining currently displayed bag with bag id: "<<retainbag->getBHSBagId()<<". Display new bag with bhs id: "<<bhsid);
      emit bagRetain();
    }
    else
    {
      INFO_LOG("BHS::Ignoring Bag review request from BHS as currently displayed bag id: "<<retainbag->getBagid()<<" and requested bag id: "<<bhsid<< " are same.");
      return;
    }
  }

  while ((BagsDataPool::getInstance()->getBagbyState(BagData::DISPOSED | BagData::ERROR | BagData::EMPTY)) == NULL)
  {

  };

  QMLEnums::ScreenEnum screen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  if(((screen == QMLEnums::SEARCH_SCREEN) ||
       (screen == QMLEnums::SEARCH_BAG_SCREEN) ) && isFreeBaginQueue)
  {
    emit getBagDataFromServer(QMLEnums::BHS_SERVER,bhsid,QMLEnums::BLBDC_SHOW_BAG);
  }
  else
  {
    ERROR_LOG("BagReview from BHS is not allowed for the screens other than search.");
  }
}

}  // end of namespace ws
}  // end of namespace analogic
