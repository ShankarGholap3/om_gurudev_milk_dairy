/*!
* @file     bagdataacqhandler.cpp
* @author   Agiliad
* @brief    This file contains interface, responsible for managing bag data. It receives bag data,sends it for processing and updates its status.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/bagdata/bagdataacqhandler.h>
#include <analogic/ws/ulm/uilayoutmanager.h>


namespace analogic
{

namespace ws
{

/*!
* @fn       BagDataAcqHandler
* @param    QThread* thread
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class BagDataAcqHandler.
*/
BagDataAcqHandler::BagDataAcqHandler(QThread* thread, QObject *parent):
  QObject(parent), m_Thread(thread)
{
  TRACE_LOG("");
  m_bagqueue = NULL;
  m_bagDownloadInprogress = false;

  THROW_IF_FAILED((m_Thread == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  // move current object to thread
  this->moveToThread(m_Thread);

  m_Thread->start();
  m_pUpdateTimer  = NULL;
  connect (m_Thread, SIGNAL(started()), this, SLOT(onThreadStarted()),
           Qt::QueuedConnection);
  connect (this, &BagDataAcqHandler::exit, this, &BagDataAcqHandler::onExit,
           Qt::QueuedConnection);
}

/*!
* @fn       onThreadStarted
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void BagDataAcqHandler::onThreadStarted()
{
  TRACE_LOG("");
  m_bagqueue = BagsDataPool::getInstance();
  DEBUG_LOG("BagQueue handle used: " << m_bagqueue);
  // creating new qtimer instance
  m_pUpdateTimer = new QTimer(this);
  THROW_IF_FAILED((m_pUpdateTimer == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  // setting timer interval
  m_pUpdateTimer->setInterval(1000);
  // connecting timer events to proper slot
  connect(m_pUpdateTimer, SIGNAL(timeout()), this,
          SLOT(getBagData()) ,
          Qt::QueuedConnection);
  m_pUpdateTimer->start();
  Semaphore::getInstance()->release(1);
}

/*!
* @fn       getBagData
* @param    None
* @return   None
* @brief    get bag data.
*/
void BagDataAcqHandler::getBagData()
{
  TRACE_LOG("");
  if (m_bagqueue != NULL)
  {
    if(!m_bagDownloadInprogress && (m_bagqueue->getBagbyState(BagData::INITIALISED)!= NULL))
    {
      m_bagDownloadInprogress = true;
      Errors::RESULT result = startBagRecv();
      Q_UNUSED(result);
      m_bagDownloadInprogress = false;
    }
    bool bagqueempty = m_bagqueue->isBagdataqueueEmpty();
    QMLEnums::AppState current_state = UILayoutManager::getUILayoutManagerInstance()->getAppState();
    if (bagqueempty && current_state != QMLEnums::AppState::OFFLINE)
    {
      DEBUG_LOG("Bagdataqueue is empty and app state is not OFFLINE");
      emit BagDataAcqHandler::stateChanged(QMLEnums::AppState::OFFLINE);
    }
  }
  else
  {
    ERROR_LOG("Bagqueue is NULL");
  }
}

/*!
* @fn       ~BagDataAcqHandler
* @param    None
* @return   None
* @brief    Destructor for class BagDataAcqHandler.
*/
BagDataAcqHandler::~BagDataAcqHandler()
{
  TRACE_LOG("");
}

/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    calls on exit
*/
void BagDataAcqHandler::onExit()
{
  TRACE_LOG("");

  if (m_pUpdateTimer != NULL)
  {
    m_pUpdateTimer->stop();

    SAFE_DELETE(m_pUpdateTimer);
  }
  if (NULL != m_bagqueue)
  {
    BagsDataPool::destroyInstance();
    m_bagqueue = NULL;
  }
  Semaphore::getInstance()->release(1);
}

/*!
* @fn       onBagNotification
* @param    analogic::nss::NssBag*
* @param    QMLEnums::ArchiveFilterSrchLoc loc
* @param    std::string data
* @param    QMLEnums::BagsListAndBagDataCommands mode
* @return   None
* @brief    slot called on bag data received
*/
void BagDataAcqHandler::onBagNotification(analogic::nss::NssBag * nss_bag,
                                          QMLEnums::ArchiveFilterSrchLoc loc,
                                          std::string data ,  QMLEnums::BagsListAndBagDataCommands mode)
{
  TRACE_LOG("");
  if (m_bagqueue != NULL && nss_bag != NULL)
  {

    if ((loc == QMLEnums::BHS_SERVER) || (WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::SEARCH_WORKSTATION ) || (WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::SUPERVISOR_WORKSTATION))
    {
        boost::shared_ptr<BagData> bagdata = m_bagqueue->getBagbyState
            (BagData::INITIALISED | BagData::CONSTRUCTION | BagData::READYTORENDER | BagData::RENDERING);
        if(NULL != bagdata)
        {
             DEBUG_LOG("Ignoring bag review request from BHS or user selected bag as one Bag is alreay rendering or getting rendered bagId:" << data);
             return;
        }
    }
    boost::shared_ptr<BagData> bagdata = m_bagqueue->getBagbyState
        (BagData::DISPOSED | BagData::ERROR | BagData::EMPTY);
    if(NULL != bagdata)
    {
      DEBUG_LOG("Bagid of bag received: " << nss_bag->get_transfer_bag_identifier());
      bagdata->setBagid(nss_bag->get_transfer_bag_identifier());
      bagdata->setLivebag(nss_bag);
      bagdata->setState(BagData::INITIALISED);
      bagdata->setBagReqLoc(loc);
      if(loc == QMLEnums::SEARCH_SERVER)
      {
        bagdata->setSearchReason(data);
      }
      else if(loc == QMLEnums::BHS_SERVER)
      {
        bagdata->setBHSBagId(data);
      }
      else if (loc == QMLEnums::REMOTE_SERVER)
      {
          if (mode == QMLEnums::BagsListAndBagDataCommands::BLBDC_ARCHIVE_BAG_TO_USB)
          {
              bagdata->setArchiveSaveMode(true);
          }
      }
      getBagData();
    }
  }
}
}  // end of namespace ws
}  // end of namespace analogic
