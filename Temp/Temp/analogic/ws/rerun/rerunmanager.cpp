/*!
* @file     rerunmanager.cpp
* @author   Agiliad
* @brief    This file contains interface, responsible rerun emulator.
* @date     March, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/rerun/rerunmanager.h>
namespace analogic
{
namespace ws
{

/*!
   * @fn       RerunManager
   * @param    None
   * @return   None
   * @brief    Constructor for class RerunManager.
   */
RerunManager::RerunManager()
{
    TRACE_LOG("");
    INFO_LOG("Creating instance of rerun handler");
    m_detectionAlgList = new QStringList;
    // intialize Rerun configuration
    INFO_LOG("Creating instance of workstation config and initialize Rerun configuration");
    m_workstation_config_handle.reset(new RerunConfig());
    THROW_IF_FAILED((m_workstation_config_handle.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

}

/*!
   * @fn       RerunManager
   * @param    None
   * @return   None
   * @brief    Destructor for class RerunManager.
   */
RerunManager::~RerunManager()
{
    TRACE_LOG("");
    INFO_LOG("deleting instance of Rerun handler");
    m_rerunviewerHandler.reset();
    SAFE_DELETE(m_detectionAlgList);
}

/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    Slot calls on exit if application.
*/
void RerunManager::onExit()
{
    DEBUG_LOG("Sending exit");
    m_rerunHandler.get()->getConnectEmulator()->release_rerun();
    m_rerunviewerHandler.reset();
    m_rerunHandler.reset();
    Semaphore::getInstance()->release(3);
    //WorkstationManager::onExit();
}

/*!
* @fn       authenticate
* @param    QString username
* @param    QString password
* @return   QMLEnums::WSAuthenticationResult
* @brief    Authenticate user
*/
QMLEnums::WSAuthenticationResult  RerunManager::authenticate(QString username, QString password)
{
    QString msg;
    QMLEnums::WSAuthenticationResult ret;
    if (username.compare("emulator")==0 && (password.compare("emulator"))==0)
    {
        msg = tr("Login to EMulator success");
        ret = QMLEnums::SUCCESS;
        if(m_rerunHandler != NULL && (m_rerunHandler.get()->getUsbStorageService() != NULL))
        {
            m_rerunHandler.get()->getUsbStorageService()->setLoggedIn(true);
            m_rerunHandler.get()->getUsbStorageService()->retriveStorageMountInfo();
            m_rerunviewerHandler.get()->setOperatorName(username);
        }
    }
    else
    {
        msg = tr("Login to EMulator failed");
        ret = QMLEnums::INVALID_LOGIN;
    }
    emit authstatuschanged((int)ret, msg, QMLEnums::
                           RERUN_EMULATOR_AUTHENTICATION_MODEL);
    return ret;
}

/*!
* @fn       onCommandButtonEvent();
* @param    QMLEnums::ModelEnum model
* @param    QString data
* @return   None
* @brief    This function is responsible to process model data.
*/
void RerunManager::onCommandButtonEvent(QMLEnums::ModelEnum model, QString data)
{
    TRACE_LOG("");
    if (QMLEnums::ModelEnum::RERUN_EMULATOR_AUTHENTICATION_MODEL == model && data != "logout")    {
        QStringList credentialStringList = data.split(",");
        QString username = credentialStringList.at(0);
        QString password = credentialStringList.at(1);

        INFO_LOG("Request to authenticate user on Emulator");
        QMLEnums::WSAuthenticationResult res = authenticate(username, password);
        INFO_LOG("Result of authenticate user on Emulator" << (int)res);
    }
    else if (data == "logout")
    {
        INFO_LOG("Request to Logout user");

        deAuthenticate();
    }
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    Calls NssHandler init for intialization of nss and connection agent.
*/
void RerunManager::init()
{
    TRACE_LOG("");
    //WorkstationManager::init();

    m_rerunHandler.reset(new RerunHandler());
    THROW_IF_FAILED((m_rerunHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    // int res = m_rerunHandler.get()->init();
    m_rerunHandler.get()->init();
    m_rerunviewerHandler.reset(new RerunViewerHandler());
    THROW_IF_FAILED((m_rerunviewerHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    m_rerunviewerHandler->init();

    m_rerundataacqHandler.reset(new RerunDataAcqHandler());
    THROW_IF_FAILED((m_rerunviewerHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    m_rerundataacqHandler->initRerunDataAcqHandler();

    connect(this, SIGNAL(deAuthenticate()),this,SLOT(onDeAuthenticate()));
    Semaphore::getInstance()->release(3);
}

/*!
* @fn       mock_init
* @param    RerunHandler pointer
* @return   None
* @brief    Mocking init method to run test suites.
*/
void RerunManager::mock_init(RerunHandler *pRerunHandler)
{
    m_rerunHandler.reset(pRerunHandler);
    m_rerunHandler.get()->init();
    m_rerunviewerHandler.reset(new RerunViewerHandler());
    THROW_IF_FAILED((m_rerunviewerHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    m_rerunviewerHandler->init();

    m_rerundataacqHandler.reset(new RerunDataAcqHandler());
    THROW_IF_FAILED((m_rerunviewerHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    connect(this, SIGNAL(deAuthenticate()),this,SLOT(onDeAuthenticate()));
    Semaphore::getInstance()->release(3);
}

/*!
* @fn       onUpdateState();
* @param    QMLEnums::AppState state
* @return   void
* @brief    call on update state.
*/
void RerunManager::onUpdateState(QMLEnums::AppState state)
{
    TRACE_LOG("");
    DEBUG_LOG("Updated App State to: "<< state);
}

/*!
* @fn       getRerunViewerHandler
* @param    None
* @return   boost::shared_ptr<RerunViewerHandler> - archive handler
* @brief    Gets local RerunViewer handler
*/
boost::shared_ptr<RerunViewerHandler> RerunManager::getRerunViewerHandler()
{
    return m_rerunviewerHandler;
}

/*!
* @fn       getRerunDataAcqHandler
* @param    None
* @return   boost::shared_ptr<RerunDataAcqHandler> - data aqc handler
* @brief    Gets local RerunDatAcq handler
*/
boost::shared_ptr<RerunDataAcqHandler> RerunManager::getRerunDataAcqHandler()
{
    return m_rerundataacqHandler;
}

/*!
* @fn       onNetworkUpdate
* @param    None
* @return   None
* @brief    This slot gets call if networks gets disconnected.
*/
void RerunManager::onNetworkUpdate()
{

}

/*!
* @fn       onRerunCommand
* @param    QMLEnums::RERUN_COMMANDS cmd
* @param    RerunTaskConfig taskConfig
* @return   None
* @brief    slot to execute rerun detection
*/
void RerunManager::onRerunCommand(QMLEnums::RERUN_COMMANDS cmd, RerunTaskConfig taskConfig)
{
    if (cmd == QMLEnums::RERUN_GET_ALGORITHM)
    {
        INFO_LOG("Getting algorithm list");
        m_rerunHandler.get()->getConnectEmulator()->getAlgorithmsList_rerun();
    }
    else if (cmd == QMLEnums::RERUN_START)
    {
        INFO_LOG("start rerun task");
        m_rerunHandler.get()->setTaskConfig(taskConfig);
        m_rerunHandler.get()->startRerunTask();
    }
    else if (cmd == QMLEnums::RERUN_CANCEL)
    {
        INFO_LOG("cancel rerun task");
        m_rerunHandler.get()->cancelRerunTask();
    }
    else if (cmd == QMLEnums::RERUN_RELEASE)
    {
        INFO_LOG("release rerun");
        m_rerunHandler.get()->getConnectEmulator()->release_rerun();
    }
}

/*!
* @fn       updateAlgorithmList
* @param    None
* @return   None
* @brief    get available algorithmsv from  rerun libs api and update the list.
*/
void RerunManager::updateAlgorithmList()
{

}

/*!
* @fn       onDeAuthenticate
* @param    None
* @return   None
* @brief    Slot calls on deauthentication
*/
void RerunManager::onDeAuthenticate()
{
    m_rerunHandler.get()->onDeAuthenticate();

}
}  // end of namespace ws
}  // end of namespace analogic
