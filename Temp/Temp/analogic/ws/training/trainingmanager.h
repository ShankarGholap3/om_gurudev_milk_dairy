/*!
* @file     trainingmanager.h
* @author   Agiliad
* @brief    This file contains interface, functionality for training workstation.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_TRAINING_TRAININGMANAGER_H_
#define ANALOGIC_WS_TRAINING_TRAININGMANAGER_H_

#include <boost/scoped_ptr.hpp>
#include <analogic/ws/workstationmanager.h>
#include <analogic/ws/rerun/rerunviewerhandler.h>
#include <analogic/ws/rerun/rerundataacqhandler.h>
#include <analogic/ws/common/utility/usbstorageservice.h>
#include <analogic/ws/common/utility/customfiledialog.h>
#include <analogic/ws/rerun/rerunconfig.h>
#include <analogic/ws/common/utility/connectemulator.h>
#include <analogic/ws/nsshandler.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   TrainingManager
 * \brief   This class contains functionality related to Training Workstation
 */
class TrainingManager: public WorkstationManager
{
     Q_OBJECT
public:
    /*!
       * @fn       TrainingManager
       * @param    None
       * @return   None
       * @brief    Constructor for class TrainingManager.
       */
    TrainingManager();
    /*!
       * @fn       TrainingManager
       * @param    None
       * @return   None
       * @brief    Destructor for class TrainingManager.
       */
    ~TrainingManager();
    /*!
    * @fn       getRerunViewerHandler
    * @param    None
    * @return   boost::shared_ptr<RerunViewerHandler> - archive handler
    * @brief    Gets local RerunViewer handler
    */
    virtual boost::shared_ptr<RerunViewerHandler> getRerunViewerHandler();

    /*!
    * @fn       getRerunDataAcqHandler
    * @param    None
    * @return   boost::shared_ptr<RerunDataAcqHandler> - data aqc handler
    * @brief    Gets local RerunDatAcq handler
    */
    virtual boost::shared_ptr<RerunDataAcqHandler> getRerunDataAcqHandler();

    /*!
    * @fn       mock_init
    * @param    RerunHandler pointer
    * @return   None
    * @brief    Mocking init method to run test suites.
    */
    void mock_init();

public slots:
    /*!
    * @fn       onCommandButtonEvent();
    * @param    QMLEnums::ModelEnum model
    * @param    QString data
    * @return   None
    * @brief    This function is responsible to process model data.
    */
    void onCommandButtonEvent(QMLEnums::ModelEnum model, QString data);
    /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    void onUpdateState(QMLEnums::AppState state);
    /*!
    * @fn       init
    * @param    None
    * @return   None
    * @brief    Calls NssHandler init for intialization of nss and connection agent.
    */

    /*!
        * @fn      onNetworkStatus
        * @param    QMLEnums::NetworkState
        * @return   None
        * @brief    notifies about status of network.
        */
        void onNetworkStatus(QMLEnums::NetworkState status);

        /*!
        * @fn       onNetworkUpdate
        * @param    None
        * @return   None
        * @brief    This slot gets call if networks gets disconnected.
        */
        void onNetworkUpdate();

    void init();
    /*!
    * @fn       onExit
    * @param    None
    * @return   None
    * @brief    Slot calls on exit if application.
    */
    void onExit();
    /*!
    * @fn       onDeAuthenticate
    * @param    None
    * @return   None
    * @brief    Slot calls on deauthentication
    */
    void onDeAuthenticate();
signals:

    /*!
    * @fn       deAuthenticate
    * @param    None
    * @return   None
    * @brief    signal to propagate call for Deauthenticate user
    */
    void deAuthenticate();
private:
    /*!
       * @fn       createOsrAgent
       * @param    None
       * @return   int - Agent creation status (0/1 - success/failure).
       * @brief    Creates Osr agent.
       */
       int createOsrAgent();

    /*!
    * @fn       authenticate
    * @param    QString username
    * @param    QString password
    * @return   QMLEnums::WSAuthenticationResult
    * @brief    Authenticate user
    */
    QMLEnums::WSAuthenticationResult  authenticate(QString username, QString password);
    QString                                 m_username;            //!< user name
    QString                                 m_password;            //!< Password
    QStringList                            *m_detectionAlgList;    //!< available detection algorithm list
    boost::shared_ptr<RerunViewerHandler>   m_rerunviewerHandler;  //!< handle to ReviewViewHandler
    boost::shared_ptr<RerunDataAcqHandler>  m_rerundataacqHandler; //!< handle to Review Data Acq Handler
    UsbStorageSrv                          *m_usbStorageService;   //!< handle to usb service
};
}    // end of namespace ws
}    // end of namespace analogic
#endif // ANALOGIC_WS_TRAINING_TRAININGMANAGER_H_
