/*!
* @file     pulldatahandler.cpp
* @author   Agiliad
* @brief    This file contains functionality related to pulling bag, when bag will be ready from scanner for processing.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/bagdata/pulldatahandler.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       PullDataHandler
* @parma    QThread *thread
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class PullDataHandler.
*/
PullDataHandler::PullDataHandler(QThread *thread, QObject* parent):BagDataAcqHandler(thread, parent)
{
    m_sal = ScannerAccessInterface::getInstance();
    DEBUG_LOG("Get Scanner Access layer instance and set host and ip address.");

    connect(m_sal, &ScannerAccessInterface::bagReceived, this, &PullDataHandler::bagDataReceived);
}

/*!
* @fn       startBagRecv
* @param    None
* @return   Errors::RESULT
* @brief    start bag data receive.
*/
Errors::RESULT PullDataHandler::startBagRecv()
{
    Errors::RESULT res = Errors::S_OK;
    if ((m_bagqueue != NULL) && (m_sal != NULL))
    {
        DEBUG_LOG("Getting bags of state BagData::INITIALISED");
        boost::shared_ptr<BagData> data = m_bagqueue->getBagbyState(BagData::INITIALISED);
        if(data != NULL){
            INFO_LOG("Start receiving bags");
            m_sal->setBagdata(data);
            res = m_sal->Start();
        }
    }
    return res;
}

/*!
* @fn       stopToReceiveBag
* @param    None
* @return   None
* @brief    notify scannerAccessInterface to stop receiving current bag as network is disconnected.
*/
void PullDataHandler::stopToReceiveBag()
{
    DEBUG_LOG("stopToReceiveBag() Received network status is UNREGISTERED ");
    if (m_sal != NULL) {
        m_sal->closeReceiver(true);
    }
    m_bagDownloadInprogress = false;
}

/*!
* @fn       bagDataReceived
* @param    boost::shared_ptr<BagData> bagdata
* @param    bool status
* @param    QString message
* @return   None
* @brief    This function will call on bag data received
*/
void PullDataHandler::bagDataReceived(boost::shared_ptr<BagData> bagdata,
                                      bool status, QString message)
{
    if (bagdata.get() != NULL)
    {
        m_bagDownloadInprogress = false;
        analogic::nss::NssBag *tempLiveBag = bagdata->getLivebag();
        if (tempLiveBag != NULL)
        {
            if (status)
            {
                DEBUG_LOG("Notifying bag is recevied successfully having bagid: "
                          << tempLiveBag->get_transfer_bag_identifier());
                if (bagdata.get()->getArchiveSaveMode() == true)
                {
                    emit saveRemoteArchiveBag(bagdata);
                }
            }
            else
            {
                DEBUG_LOG("Notifying bag is not recevied having bagid: "
                          << tempLiveBag->get_transfer_bag_identifier());
                // notify qml abot error
                emit notifyBagReceivedFail(Errors::E_FAIL, message, QMLEnums::ModelEnum::
                                           BAGRECEIVED_STATUS_MODEL);
            }
        }
        else
        {
            if(!status)
            {
                emit notifyBagReceivedFail(Errors::E_FAIL, message, QMLEnums::ModelEnum::
                                           BAGRECEIVED_STATUS_MODEL);
            }
            else
            {
                ERROR_LOG("Received success from scanner but live bag is NULL.");
            }
        }
    }
}

/*!
* @fn       ~PullDataHandler
* @param    None
* @return   None
* @brief    Destructor for class PullDataHandler.
*/
PullDataHandler::~PullDataHandler()
{
    ScannerAccessInterface::destroyInstance();
    m_sal = NULL;
}

/*!
* @fn       onUpdateState();
* @param    QMLEnums::AppState state
* @return   void
* @brief    call on update state.
*/
void PullDataHandler::onUpdateState(QMLEnums::AppState state)
{
    if ( state == QMLEnums::SET_TO_OFFLINE )
    {
    }
}
}  // end of namespace ws
}  // end of namespace analogic
