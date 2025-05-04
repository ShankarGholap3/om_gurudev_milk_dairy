/*!
* @file     bagdataacqhandler.h
* @author   Agiliad
* @brief    This file contains interface for managing bag data. It receives bag data,sends it for processing and updates its status.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_BAGDATA_BAGDATAACQHANDLER_H_
#define ANALOGIC_WS_COMMON_BAGDATA_BAGDATAACQHANDLER_H_

#include <QTimer>
#include <QObject>
#include <QThread>
#include <Logger.h>
#include <semaphore.h>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{

/*!
 * \class   BagDataAcqHandler
 * \brief   This class contains variable and function related to
 *          bag data acquisition handler.
 */
class BagDataAcqHandler:public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       BagDataAcqHandler
    * @param    QThread* - thread
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class BagDataAcqHandler.
    */
    explicit BagDataAcqHandler(QThread *thread = NULL, QObject *parent = NULL);

    /*!
    * @fn       ~BagDataAcqHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class BagDataAcqHandler.
    */
    virtual ~BagDataAcqHandler();

    /*!
    * @fn       startBagRecv
    * @param    None
    * @return   Errors::RESULT
    * @brief    start bag recv from bag data server. This is interface function of abstract class.
    *           This function needs to defined in all derived class.
    */
    virtual Errors::RESULT startBagRecv() = 0;

    /*!
    * @fn       getBagDataacqHdlrThread
    * @param    None
    * @return   QThread* thread handle
    * @brief    Returns handle to BagDataAcqHandler thread.
    */
    QThread* getBagDataacqHdlrThread();

    /*!
    * @fn       stopToReceiveBag
    * @param    None
    * @return   None
    * @brief    notify scannerAccessInterface to stop receiving current bag as network is disconnected.
    */
    virtual void stopToReceiveBag()=0;

signals:
    /*!
    * @fn       exit
    * @param    None
    * @return   None
    * @brief    signals exit
    */
    void exit();

    /*!
    * @fn       stateChanged
    * @param    QMLEnums::AppState
    * @return   None
    * @brief    signal state changed
    */
    void stateChanged(QMLEnums::AppState);

    /*!
    * @fn       notifyBagReceivedFail
    * @param    int hr
    * @param    QString - status
    * @param    int - model
    * @return   None
    * @brief    notify workstationmanager about the error behind bag is not
    *           received.
    */
    void notifyBagReceivedFail(int hr, QString status, int model);

    /*!
    * @fn       saveRemoteArchiveBag
    * @param    boost::shared_ptr<BagData> bagdata
    * @return   None
    * @brief    signal save remote bag ready
    */
    void saveRemoteArchiveBag(boost::shared_ptr<BagData> bagdata);

public slots:
    /*!
    * @fn       onExit
    * @param    None
    * @return   None
    * @brief    calls on exit
    */
    void onExit();

    /*!
    * @fn       onBagNotification
    * @param    analogic::nss::NssBag*
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @param    std::string data
    * @param    QMLEnums::BagsListAndBagDataCommands mode
    * @return   None
    * @brief    slot called on bag data received
    */
    void onBagNotification(analogic::nss::NssBag *, QMLEnums::ArchiveFilterSrchLoc, std::string data ,QMLEnums::BagsListAndBagDataCommands mode);
    /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    virtual void onUpdateState(QMLEnums::AppState state) = 0;
    /*!
    * @fn       onThreadStarted
    * @param    None
    * @return   void
    * @brief    on thread start this function will initialize models.
    */
    void onThreadStarted();
    /*!
    * @fn       getBagData
    * @param    None
    * @return   void
    * @brief    get Bag data
    */
    void getBagData();


protected:
    bool                               m_bagDownloadInprogress;
    QTimer*                            m_pUpdateTimer;  //!< handle for timer
    QThread*                           m_Thread;        //!< handle for thread
    BagsDataPool*                      m_bagqueue;
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_BAGDATA_BAGDATAACQHANDLER_H_

