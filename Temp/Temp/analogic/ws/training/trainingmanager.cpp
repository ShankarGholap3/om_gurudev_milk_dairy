/*!
* @file     trainingmanager.cpp
* @author   Agiliad
* @brief    This file contains interface, functionality for training workstation.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include <analogic/ws/training/trainingmanager.h>
#include <analogic/ws/training/traininghandler.h>
#include <analogic/ws/nsshandler.h>

#include <analogic/ws/rerun/rerunconfig.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
   * @fn       TrainingManager
   * @param    None
   * @return   None
   * @brief    Constructor for class TrainingManager.
   */
TrainingManager::TrainingManager()
{
  TRACE_LOG("");
  INFO_LOG("Creating instance of training manager");

  // intialize Rerun configuration
  INFO_LOG("Creating instance of workstation config and initialize Rerun configuration");
  m_workstation_config_handle.reset(new RerunConfig());
  THROW_IF_FAILED((m_workstation_config_handle.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  if(RerunConfig::isRemovableStorage())
  {
    m_usbStorageService = new UsbStorageSrv;
  } else {
    m_usbStorageService = NULL;
  }
  THROW_IF_FAILED((m_workstation_config_handle.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
}
/*!
   * @fn       TrainingManager
   * @param    None
   * @return   None
   * @brief    Destructor for class TrainingManager.
   */
TrainingManager::~TrainingManager()
{

}
/*!
* @fn       onCommandButtonEvent();
* @param    QMLEnums::ModelEnum model
* @param    QString data
* @return   None
* @brief    This function is responsible to process model data.
*/
void TrainingManager::onCommandButtonEvent(QMLEnums::ModelEnum model, QString data)
{
  TRACE_LOG("");

  if (QMLEnums::ModelEnum::NETWORK_AUTHENTICATION_MODEL == model && data != "logout")
  {
    QJsonDocument jsondoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jobj =  jsondoc.object();
    QVariantMap mapobj =  jobj.toVariantMap();
    QString username = QVariant(mapobj["username"]).toString();
    QString password = QVariant(mapobj["password"]).toString();
    //  QString pin = QVariant(mapobj["pin"]).toString();

    if(getNWFaultHandle()->getNetworkState() == QMLEnums::NetworkState::REGISTERED)
    {
      INFO_LOG("Request to authenticate user on Nss");
      authenticate(username, password);//, pin);
    }
    else if(getNWFaultHandle()->getNetworkState() != QMLEnums::NetworkState::REGISTERED)
    {
      int hr = Errors::CONNECTION_ERROR;
      INFO_LOG("trainingmanager emit authstatuschanged for NETWORK_AUTHENTICATION_MODEL")

          emit authstatuschanged(hr, "", QMLEnums::
                                 NETWORK_AUTHENTICATION_MODEL);

    }
  }
  else if (QMLEnums::ModelEnum::NETWORK_CHANGE_PASSWORD_MODEL == model)
  {
    QJsonDocument jsondoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jobj =  jsondoc.object();
    QVariantMap mapobj =  jobj.toVariantMap();
    QString username = QVariant(mapobj["username"]).toString();
    QString oldAuthMethodDetail = QVariant(mapobj["old_auth_method_detail"]).toString();
    QString newAuthMethodDetail = QVariant(mapobj["new_auth_method_detail"]).toString();
    QMLEnums::AuthMethodType authMethodType = static_cast<QMLEnums::AuthMethodType>(QVariant(mapobj["auth_method_to_change"]).toInt());

    if(getNWFaultHandle()->getNetworkState() == QMLEnums::NetworkState::REGISTERED)
    {
      INFO_LOG("Request to change password for a user on Nss");
      changePassword(
            username.toStdString(), oldAuthMethodDetail.toStdString(), newAuthMethodDetail.toStdString(), authMethodType);
    }
    else
    {
      int hr = Errors::CONNECTION_ERROR;
      emit authstatuschanged(hr, "", QMLEnums::
                             NETWORK_CHANGE_PASSWORD_MODEL);
    }
  }
  else if (data == "logout")
  {
    INFO_LOG("Request to Logout user");

    deAuthenticate();
  }
  else if (QMLEnums::ModelEnum::WORKSTATION_CONFIG_MODEL == model)
  {
    updateXMLFileData(model, data);
    emit notifyWSConfigUpdated();
  }
}
/*!
* @fn       onUpdateState();
* @param    QMLEnums::AppState state
* @return   void
* @brief    call on update state.
*/
void TrainingManager::onUpdateState(QMLEnums::AppState state)
{

}
/*!
* @fn       init
* @param    None
* @return   None
* @brief    Calls NssHandler init for intialization of nss and connection agent.
*/
void TrainingManager::init()
{
  TRACE_LOG("");
  WorkstationManager::init();
  m_rerunviewerHandler.reset(new RerunViewerHandler());

  connect(this, &TrainingManager::networkStatus,
          this, &TrainingManager::onNetworkStatus);

  THROW_IF_FAILED((m_rerunviewerHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_rerunviewerHandler->init();

  m_nss_handler.reset( new TrainingHandler());
  connect(this->getNWFaultHandle().get(), &NetworkFaultHandler::updateNetwork,
          this, &TrainingManager::onNetworkUpdate, Qt::QueuedConnection);

  NSSAgentAccessInterface* nssInterface = NSSAgentAccessInterface::getInstance();
  if(nssInterface != NULL)
  {
    AgentListener* osragentlistner = nssInterface->getAgentListener();
    if (osragentlistner)
    {
      connect(osragentlistner, &AgentListener::networkStatusChanged,
              this, &TrainingManager::networkStatus, Qt::QueuedConnection);

    }
    connect(nssInterface,
            &NSSAgentAccessInterface::displayMsg,
            this,
            &WorkstationManager::authstatuschanged,
            Qt::QueuedConnection);
  }

  emit networkStatus(QMLEnums::NetworkState::UNREGISTERED);

  m_rerundataacqHandler.reset(new RerunDataAcqHandler());
  THROW_IF_FAILED((m_rerunviewerHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  m_rerundataacqHandler->initRerunDataAcqHandler();

  connect(this, SIGNAL(deAuthenticate()),this,SLOT(onDeAuthenticate()));
  Semaphore::getInstance()->release(3);
}

/*!
* @fn      onNetworkStatus
* @param    QMLEnums::NetworkState
* @return   None
* @brief    notifies about status of network.
*/
void TrainingManager::onNetworkStatus(QMLEnums::NetworkState status)
{
  INFO_LOG("TrainingManager: Received network status: "<<status);
  getNWFaultHandle()->networkStatus(status);

#if defined WORKSTATION || defined RERUN
  if (status == QMLEnums::NetworkState::UNREGISTERED) {
#ifdef WORKSTATION
    m_bagdata_acq_handle->stopToReceiveBag();
#endif
    emit stateChanged(QMLEnums::SET_TO_OFFLINE);
  }
#endif
}
void TrainingManager::onNetworkUpdate()
{
  INFO_LOG("Recreate network connection on Network Update.");
  if (NULL != m_nss_handler)
  {
    int ret = m_nss_handler->init();
    if (Errors::S_OK == ret)
    {
      INFO_LOG( "Initialized OSR Handler");
    }
    else
    {
      ERROR_LOG("Failed OSR Handler initalization");
      return;
    }
    // create osr agent
    ret = createOsrAgent();
    if (Errors::S_OK == ret)
    {
      INFO_LOG("Created OSR agent emmiting networkStatus");
      emit networkStatus(QMLEnums::NetworkState::REGISTERED);
    }
    else
    {
      ERROR_LOG("Failed OSR agent creation");
    }
  }
  else
  {
    INFO_LOG("m_nss_handler is NULL.");
  }
}
/*!
* @fn       createOsrAgent
* @param    None
* @return   int - Agent creation status (0/1 - success /failure).
* @brief    Creates Osr agent.
*/
int TrainingManager::createOsrAgent()
{
  TRACE_LOG("");
  // 1. Read properties from configuration file
  analogic::nss::OsrAgentStaticProperties properties = RerunConfig::getOsrAgentProperties();

  // 2. Pass those properties to osrhandler for creating osr agent
  return m_nss_handler->createWSAgent(properties);
}

/*!
* @fn       mock_init
* @param    RerunHandler pointer
* @return   None
* @brief    Mocking init method to run test suites.
*/
void TrainingManager::mock_init()
{
  m_rerunviewerHandler.reset(new RerunViewerHandler());
  THROW_IF_FAILED((m_rerunviewerHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_rerunviewerHandler->init();

  m_rerundataacqHandler.reset(new RerunDataAcqHandler());
  THROW_IF_FAILED((m_rerunviewerHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  connect(this, SIGNAL(deAuthenticate()),this,SLOT(onDeAuthenticate()));
  Semaphore::getInstance()->release(3);
}

/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    Slot calls on exit if application.
*/
void TrainingManager::onExit()
{
  TRACE_LOG("");
  DEBUG_LOG("Sending exit");
  m_nss_handler.reset();
  m_workstation_config_handle.reset();
  WorkstationManager::onExit();
}

/*!
* @fn       getRerunViewerHandler
* @param    None
* @return   boost::shared_ptr<RerunViewerHandler> - archive handler
* @brief    Gets local RerunViewer handler
*/
boost::shared_ptr<RerunViewerHandler> TrainingManager::getRerunViewerHandler()
{
  return m_rerunviewerHandler;
}

/*!
* @fn       getRerunDataAcqHandler
* @param    None
* @return   boost::shared_ptr<RerunDataAcqHandler> - data aqc handler
* @brief    Gets local RerunDatAcq handler
*/
boost::shared_ptr<RerunDataAcqHandler> TrainingManager::getRerunDataAcqHandler()
{
  return m_rerundataacqHandler;
}

/*!
* @fn       onDeAuthenticate
* @param    None
* @return   None
* @brief    Slot calls on deauthentication
*/
void TrainingManager::onDeAuthenticate()
{
  INFO_LOG("Deauthenticate user");
  if (m_usbStorageService != NULL)
  {
    m_usbStorageService->setLoggedIn(false);
    m_usbStorageService->unMountUsb();
  }
  UsbFileSystemModel*  usbmodel = m_rerunviewerHandler->getUsbFileSystemModel();

  usbmodel->setRootPath("");
}
/*!
* @fn       authenticate
* @param    QString username
* @param    QString password
* @return   QMLEnums::WSAuthenticationResult
* @brief    Authenticate user
*/
QMLEnums::WSAuthenticationResult  TrainingManager::authenticate(QString username, QString password)
{
  QString msg;
  INFO_LOG("QMLEnums::WSAuthenticationResult  TrainingManager :: ");
  QMLEnums::WSAuthenticationResult ret = QMLEnums::IO_ERROR;
  if (m_nss_handler != NULL)
  {
    // authenticate user

    std::map<analogic::nss::AuthMethodType, std::string> auth_methods;
    if(!password.isEmpty())
    {
      auth_methods.insert(pair<analogic::nss::AuthMethodType, std::string>(analogic::nss::AuthMethodType::AUTH_METHOD_PASSWORD, password.toStdString()));
    }
    ret = m_nss_handler->authenticate(username, auth_methods);

    emit authstatuschanged(static_cast<int>(ret), "", QMLEnums::
                           NETWORK_AUTHENTICATION_MODEL);

    if(QMLEnums::WSAuthenticationResult::SUCCESS == ret)
    {
      INFO_LOG("User authentication Success");
      m_username = username;
      m_password = password;
      m_rerunviewerHandler->setUserName(m_username);
      if (m_usbStorageService != NULL)
      {
        m_usbStorageService->setLoggedIn(true);
        m_usbStorageService->retriveStorageMountInfo();
      }
      WorkstationConfig::getInstance()->setDataPath(UsbStorageSrv::getUsbfilePath().c_str());

    }
    else
    {
      ERROR_LOG("Failed User authentication ");
    }
    return ret;
  }
  else
  {
    ERROR_LOG("Nss Handler instance is NULL");
    emit authstatuschanged(static_cast<int>(ret), "", QMLEnums::
                           NETWORK_AUTHENTICATION_MODEL);
    return ret;
  }
}

}   // end of namespace ws
}   // end of namespace analogic
