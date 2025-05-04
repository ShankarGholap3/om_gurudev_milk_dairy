/*!
* @file     workstationmanager.h
* @author   Agiliad
* @brief    This file contains interface to manage workstation.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_WORKSTATIONMANAGER_H_
#define ANALOGIC_WS_WORKSTATIONMANAGER_H_

#include <boost/shared_ptr.hpp>

#include <QObject>
#include <Logger.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/nsshandler.h>
#ifdef WORKSTATION
#include <analogic/ws/common/bagdata/pulldatahandler.h>
#include <analogic/ws/common/bagdata/bagdataacqhandler.h>
#include <analogic/ws/common/faults/scannerfaulthandler.h>
#include <analogic/ws/common/faults/networkfaulthandler.h>
#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>
#endif

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   WorkstationManager
 * \brief   This class contains variable and function related to
 *          managing  interface for workstations based on configuration.
 */
class WorkstationManager : public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       WorkstationManager();
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    WorkstationManager();

    /*!
    * @fn       ~WorkstationManager
    * @param    None
    * @return   None
    * @brief    Destructor responsible for deinitialization of members, memory and resources.
    */
    virtual ~WorkstationManager();
#ifdef WORKSTATION
    /*!
    * @fn       getBagDataAcqHnd
    * @param    None
    * @return   boost::shared_ptr<BagDataAcqHandler>  - bag data acq handle
    * @brief    Gets bag data acqusition handle.
    */
    boost::shared_ptr<BagDataAcqHandler> getBagDataAcqHnd();

    /*!
    * @fn       getLivecontrollerHnd
    * @param    None
    * @return   boost::shared_ptr<NssHandler> - live controller handler
    * @brief    Gets live controller handler
    */
    virtual boost::shared_ptr<NssHandler> getLiveControllerHnd();
    /*!
    * @fn       getScannerFaultHandle
    * @param    None
    * @return   boost::shared_ptr<ScannerFaultHandler>
    * @brief    Gets scanner fault handler
    */
    boost::shared_ptr<ScannerFaultHandler> getScannerFaultHandle() const;

#endif

    /*!
    * @fn       CreateScannerOperationAgent
    * @param    None
    * @return   int- Agent creation status (S_OK/E_POINTER - success/failure).
    * @brief    Creates Osr/ Search Scanner Operation agent.
    */
    int CreateScannerOperationAgent();

    /*!
       * @fn       getNWFaultHandle
       * @param    None
       * @return   boost::shared_ptr<NetworkFaultHandler>
       * @brief    Gets network fault handler
       */
     boost::shared_ptr<NetworkFaultHandler> getNWFaultHandle() const;

    /*!
    * @fn       setBagDataHandlerThread
    * @param    boost::shared_ptr<QThread>
    * @return   None
    * @brief    set bag data thread
    */
    void setBagDataHandlerThread(boost::shared_ptr<QThread> thread);
    /*!
    * @fn       getUsername
    * @param    None
    * @return   QString username
    * @brief    Gets name of currently logged in user.
    */
    QString getUsername();

signals:
    /*!
    * @fn       networkStatus
    * @param    QMLEnums::NetworkState
    * @return   None
    * @brief    notifies about status of network from networkFaultHandler.
    */
    void networkStatus(QMLEnums::NetworkState status);

    /*!
    * @fn       exit
    * @param    None
    * @return   None
    * @brief    signals about exit of application.
    */
    void exit();
    /*!
     * @fn      authstatuschanged
     * @param   int - errorcode
     * @param   QString - status
     * @param   int - model
     * @return  void
     * @brief   signal for authentication status changed
     */
    void authstatuschanged(int hr, QString status, int model);

    /*!
    * @fn       stateChanged();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    void stateChanged(QMLEnums::AppState state);

    /*!
    * @fn       registScannerInDiscovered
    * @param    int
    * @return   None
    * @brief    This function is responsible to notify qml about registered
    *           scanner found in Discovered Scanner.
    */
    void registScannerInDiscovered(int ret);

    /*!
    * @fn       deAuthenticate
    * @param    None
    * @return   None
    * @brief    signal to propagate call for Deauthenticate user
    */
    void deAuthenticate();

    /*!
    * @fn       bagListCommandResults
    * @param    QMLEnums::ArchiveFilterSrchLoc srchLoc
    * @param    QVariant results
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @return   None
    * @brief    Send back the results of the command to caller.
    */
    void bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                               QMLEnums::BagsListAndBagDataCommands command, QVariant results);

    /*!
     * @fn       updateModels(QVariantMap data , QString sName) ;
     * @param    QVariantList - model data
     * @param    QMLEnums::ModelEnum - model name
     * @return   void
     * @brief    update model.
     */
    void updateModels(QVariantList data , QMLEnums::ModelEnum sName);

    /*!
    * @fn       requestBagPickupsig
    * @param    std::string id_assigning_authority
    * @param    const std::string primary bag  ID
    * @param    const std::string rfid
    * @param    const QString pickup_type
    * @param    QMLEnums::BhsDiverterDecision
    * @param    const std::string diverter decision string
    * @return   None
    * @brief    Signal to notify back is ready for pick up.
    */
    void requestBagPickupsig(const std::string& id_assigning_authority, const std::string &primary_bag_id,
                             const std::string &rfid, const std::string& pickup_type, QMLEnums::BhsDiverterDecision
                             diverter_decision_code, const std::string &diverter_decision_string);

    /*!
    * @fn       notifyBagIsPickedUpsig
    * @param    QMLEnums::WSBasicCommandResult
    * @return   None
    * @brief    Signal To Notify Bag Is Picked Up.
    */
    void notifyBagIsPickedUpsig(QMLEnums::WSBasicCommandResult commandResult);

    /*!
     * @fn       notifyWSConfigUpdated
     * @param    None
     * @return   void
     * @brief    Signal to notify config parameter are updated.
     */
    void notifyWSConfigUpdated();

public slots:    

    /*!
    * @fn       onCommandButtonEvent();
    * @param    QMLEnums::ModelEnum model
    * @param    QString data
    * @return   None
    * @brief    This function is responsible to process model data.
    */
    virtual void onCommandButtonEvent(QMLEnums::ModelEnum, QString) = 0;

    /*!
    * @fn       onExit
    * @param    None
    * @return   None
    * @brief    Slot calls on exit if application.
    */
    virtual void onExit();

    /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    virtual void onUpdateState(QMLEnums::AppState state) = 0;

    /*!
    * @fn       init
    * @param    None
    * @return   None
    * @brief    Intializes nss and connection agent.
    */
    virtual void init();

#ifdef WORKSTATION
    /*!
    * @fn      onNetworkStatus
    * @param    QMLEnums::NetworkState
    * @return   None
    * @brief    notifies about status of network.
    */
    virtual void onNetworkStatus(QMLEnums::NetworkState status) = 0;

    /*!
    * @fn       bagListCommand
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @param    QVariant data
    * @return   None
    * @brief    Command to process.
    */
    virtual void bagListCommand(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                                QMLEnums::BagsListAndBagDataCommands command, QVariant data) = 0;

    /*!
     * @fn       setAuthenticatedUserDetailsToScanner
     * @param    None
     * @return   void
     * @brief    Sets Authenticated user details to scanner agent.
     */
    void setAuthenticatedUserDetailsToScanner();

#endif
    /*!
    * @fn       changePassword
    * @param    const std::string& - username
    * @param    const std::string& - oldAuthMethodDetail
    * @param    const std::string& - newAuthMethodDetail
    * @param    QMLEnums::AuthMethodType - authMethodType
    * @return   QMLEnums::WSBasicCommandResult - result
    * @brief    Changes password of the user to new password.
    */
    virtual QMLEnums::WSBasicCommandResult changePassword(const std::string& username,
                                                          const std::string& oldAuthMethodDetail, const std::string& newAuthMethodDetailchan, QMLEnums::AuthMethodType authMethodType);


protected:
    /*!
    * @fn       validateAndParseXml
    * @param    None
    * @return   None
    * @brief    Parse xml configuration.
    */
    void validateAndParseXml();

    /*!
    * @fn       updateXMLFileData
    * @param    QMLEnums::modelname
    * @param    QString &data
    * @return   bool
    * @brief    common function for update xml
    */
    bool updateXMLFileData(QMLEnums::ModelEnum modelname, QString &data);

    /*!
    * @fn       authenticate
    * @param    QString username
    * @param    QString password
    * @return   QMLEnums::WSAuthenticationResult
    * @brief    Authenticate user using connection and creates agent
    *           according to WS-Type
    */
    virtual QMLEnums::WSAuthenticationResult  authenticate(
            QString username, QString password, QString pin);

protected:
    QString                                 m_configXMLData;                //!< received xml data
    QString                                 m_configXSDData;                //!< received xsd data
    QString                                 m_username;                     //!< user name
    QString                                 m_password;                     //!< Password

    boost::shared_ptr<NssHandler>           m_nss_handler;                  //!< handle to nss according to WS_Type

#ifdef WORKSTATION
    //  boost::shared_ptr<ReportHandler>        m_report_handler;               //!< handle to report handler
    boost::shared_ptr<BagDataAcqHandler>    m_bagdata_acq_handle;           //!< handle to bagdata acquire
#endif
    boost::shared_ptr<WorkstationConfig>    m_workstation_config_handle;    //!< handle to workstation config

private:
    boost::shared_ptr<QThread>                m_bagdata_hdl_thread;         //!< handle to thread for bag
#ifdef WORKSTATION
    boost::shared_ptr<ScannerFaultHandler>    m_scannerfault_handle;        //!< handle to scanner fault   
#endif
#if defined WORKSTATION || defined RERUN
    boost::shared_ptr<NetworkFaultHandler>    m_networkfault_handle;        //!< handle to network fault
#endif
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_WORKSTATIONMANAGER_H_

