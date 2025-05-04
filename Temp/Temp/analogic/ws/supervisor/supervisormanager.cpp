/*!
* @file     supervisormanager.cpp
* @author   Agiliad
* @brief    This file contains interface, responsible for receiving the data from NAL, and providing it to the UI.
* @date     Jul, 11 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/search/searcharchivebaglisthandler.h>
#include <analogic/ws/common/archive/localarchivebaglisthandler.h>
#include <analogic/ws/common/archive/remotearchivebaglisthandler.h>
#include "supervisormanager.h"


namespace analogic
{
namespace ws
{
/*!
* @fn       SupervisorManager
* @param    None
* @return   None
* @brief    Constructor for class SupervisorManager.
*/
SupervisorManager::SupervisorManager()
{
  // intialize Supervisor configuration
  TRACE_LOG("");
  INFO_LOG("Creating instance of workstation config and initialize Supervisor"
           " configuration");
  m_workstation_config_handle.reset(new SupervisorConfig());
  THROW_IF_FAILED((m_workstation_config_handle.use_count() == 0)?Errors::
                                                                 E_OUTOFMEMORY:Errors::S_OK);
}

/*!
* @fn       ~SupervisorManager
* @param    None
* @return   None
* @brief    Destructor for class SupervisorManager.
*/
SupervisorManager::~SupervisorManager()
{
  TRACE_LOG("");
  INFO_LOG("Sending exit");
}

/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    Slot calls on exit if application.
*/
void SupervisorManager::onExit()
{
  TRACE_LOG("");
  INFO_LOG("Sending exit");
  m_nss_handler.reset();
  m_workstation_config_handle.reset();
  m_localArchiveBagLHndlr.reset();
  m_remoteArchiveBagLHndlr.reset();
  m_searchArchiveBagListHandler.reset();
  WorkstationManager::onExit();
}

void SupervisorManager::init()
{
  TRACE_LOG("");
  WorkstationManager::init();
  INFO_LOG("Creating instance of SupervisorHandler");
  m_nss_handler.reset(new SupervisorHandler());

  //Creating instance of Archive BagList Handler
  m_localArchiveBagLHndlr.reset(new LocalArchiveBagListHandler());
  m_remoteArchiveBagLHndlr.reset(new RemoteArchiveBagListHandler());

  // Creating SearchArchiveBagListHandler
  m_searchArchiveBagListHandler.reset(new SearchArchiveBagListHandler());

  Semaphore::getInstance()->release(1);
  // notify bagdata acq handler about the bag arrival.
  connect(m_nss_handler.get(), SIGNAL(bagNotification(analogic::nss::
                                                      NssBag*, QMLEnums::ArchiveFilterSrchLoc, std::string,QMLEnums::BagsListAndBagDataCommands )), m_bagdata_acq_handle.get(), SLOT(onBagNotification(
                                                                                                                                                                                       analogic::nss::NssBag*,QMLEnums::ArchiveFilterSrchLoc, std::string,QMLEnums::BagsListAndBagDataCommands)), Qt::QueuedConnection);

  // notify bagListHandler about the bagList arrival
  connect(m_nss_handler.get(), &NssHandler::bagListUpdated,
          this, &SupervisorManager::bagListCommand, Qt::QueuedConnection);

  connect(m_searchArchiveBagListHandler.get(), &SearchArchiveBagListHandler::
          getBagDataFromServer, m_nss_handler.get(), &NssHandler
          ::getBagDataFromServer, Qt::DirectConnection);

  connect(m_remoteArchiveBagLHndlr.get(), &RemoteArchiveBagListHandler::
          getBagDataFromServer, m_nss_handler.get(), &NssHandler
          ::getBagDataFromServer, Qt::DirectConnection);

  // notify the status of bag archival
  connect(m_localArchiveBagLHndlr.get(), SIGNAL(bagArchivedStatus(int,QString,
                                                                  int)), this, SIGNAL(authstatuschanged(int,QString,int)));

  connect(this, SIGNAL(deAuthenticate()),
          m_nss_handler.get(), SLOT(onDeAuthenticate()),
          Qt::QueuedConnection);

  connect(this, &SupervisorManager::networkStatus,
          this, &SupervisorManager::onNetworkStatus);

  connect(this->getNWFaultHandle().get(), &NetworkFaultHandler::updateNetwork,
          this, &SupervisorManager::onNetworkUpdate, Qt::QueuedConnection);
  connect(this->getScannerFaultHandle().get(), &ScannerFaultHandler::updateScannerNetwork,
          this, &SupervisorManager::onScannerNetworkUpdate, Qt::QueuedConnection);

  connect(m_bagdata_acq_handle.get(), &BagDataAcqHandler::saveRemoteArchiveBag,
          m_remoteArchiveBagLHndlr.get(),&RemoteArchiveBagListHandler::onSaveRemoteArchiveBag, Qt::QueuedConnection);
  NSSAgentAccessInterface* nssInterface = NSSAgentAccessInterface::getInstance();
  if (nssInterface != nullptr)
  {
    AgentListener* supervisoragentlistner = nssInterface->getAgentListener();
    if (supervisoragentlistner)
    {
      connect(supervisoragentlistner, &AgentListener::networkStatusChanged,
              this, &SupervisorManager::networkStatus, Qt::QueuedConnection);
    }
    // message received
    connect(nssInterface,
            &NSSAgentAccessInterface::displayMsg,
            this,
            &WorkstationManager::authstatuschanged,
            Qt::QueuedConnection);
  }
  emit networkStatus(QMLEnums::NetworkState::UNREGISTERED);
}

/*!
* @fn      onNetworkStatus
* @param    QMLEnums::NetworkState
* @return   None
* @brief    notifies about status of network.
*/
void SupervisorManager::onNetworkStatus(QMLEnums::NetworkState status)
{
  TRACE_LOG("");
  getNWFaultHandle()->networkStatus(status);
  if (status == QMLEnums::NetworkState::UNREGISTERED) {
    m_bagdata_acq_handle->stopToReceiveBag();
  }
}

/*!
* @fn       onNetworkUpdate
* @param    None
* @return   None
* @brief    This slot gets call if networks gets disconnected.
*/
void SupervisorManager::onNetworkUpdate()
{
  INFO_LOG("Recreate network connection on Network Update.");
  if (nullptr != m_nss_handler)
  {
    int ret = m_nss_handler->init();
    if (Errors::S_OK == ret)
    {
      INFO_LOG("Initialized Supervisor Handler");
    }
    else
    {
      ERROR_LOG("Failed Supervisor Handler initalization");
      return;
    }
    // create SUPERVISOR agent
    ret = createSupervisorAgent();
    if (Errors::S_OK == ret)
    {
      INFO_LOG("Created SUPERVISOR agent");
      emit networkStatus(QMLEnums::NetworkState::REGISTERED);
    }
    else
    {
      ERROR_LOG("Failed SUPERVISOR agent creation");
    }
  }
  else
  {
    INFO_LOG("m_nss_handler is NULL .");
  }
}

/*!
* @fn       onScannerNetworkUpdate
* @param    None
* @return   None
* @brief    This slot gets call if scanner network gets updated
*/
void SupervisorManager::onScannerNetworkUpdate()
{
  INFO_LOG("Recreate scanner operation agent in case of disconnection.");
  CreateScannerOperationAgent();
}

/*!
* @fn       RegistrationChanged
* @param    analogic::nss::SupervisorAgent* agent
* @return   None
* @brief    Gives Registration Changed status of the workstation to the nss service.
*/
void SupervisorManager::RegistrationChanged(analogic::nss::SupervisorAgent* agent)
{
  TRACE_LOG("");
  if (agent == nullptr)
  {
    ERROR_LOG("Agent is NULL");
  }
}

/*!
* @fn       onCommandButtonEvent();
* @param    QMLEnums::ModelEnum model
* @param    QString data
* @return   None
* @brief    This function is responsible to process model data.
*/
void SupervisorManager::onCommandButtonEvent(QMLEnums::ModelEnum model, QString data)
{
  TRACE_LOG("");
  if (QMLEnums::ModelEnum::NETWORK_AUTHENTICATION_MODEL == model && data !=
      "logout")
  {
    QJsonDocument jsondoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jobj =  jsondoc.object();
    QVariantMap mapobj =  jobj.toVariantMap();
    QString username = QVariant(mapobj["username"]).toString();
    QString password = QVariant(mapobj["password"]).toString();
    QString pin = QVariant(mapobj["pin"]).toString();

    if (getNWFaultHandle()->getNetworkState() == QMLEnums::NetworkState::REGISTERED)
    {
      INFO_LOG("Request to authenticate user on Nss");
      QMLEnums::WSAuthenticationResult res = authenticate(username, password, pin);
      if (QMLEnums::WSAuthenticationResult::SUCCESS == res)
      {
        //  searchlist not available in supervisor_agent
//        dynamic_cast<SupervisorHandler*>(m_nss_handler.get())->
//            getSearchBagList();
      }
    }
    else if (getNWFaultHandle()->getNetworkState() != QMLEnums::NetworkState::REGISTERED)
    {
      int hr = Errors::CONNECTION_ERROR;
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

    if (getNWFaultHandle()->getNetworkState() == QMLEnums::NetworkState::REGISTERED)
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

  else if (QMLEnums::ModelEnum::WORKSTATION_CONFIG_MODEL == model)
  {
    updateXMLFileData(model, data);
    emit notifyWSConfigUpdated();
  }
  else if (data == "logout")
  {
    INFO_LOG("Request to Logout user");
    deAuthenticate();
  }
}
/*!
* @fn       onUpdateState();
* @param    QMLEnums::AppState state
* @return   void
* @brief    call on update state.
*/
void SupervisorManager::onUpdateState(QMLEnums::AppState state)
{
  TRACE_LOG("");
  INFO_LOG("Update App State to: "<< state);
  m_nss_handler->onUpdateState(state);
}

/*!
* @fn       createSupervisorAgent
* @param    None
* @return   int - Agent creation status (0/1 - success/failure).
* @brief    Creates Supervisor agent.
*/
int SupervisorManager::createSupervisorAgent()
{
  analogic::nss::SupervisorAgentStaticProperties properties = SupervisorConfig::
      getSupervisorAgentProperties();

  return m_nss_handler->createWSAgent(properties);
}

/*!
* @fn       bagListCommand
* @param    QMLEnums::ArchiveFilterSrchLoc
* @param    QMLEnums::BagsListAndBagDataCommands command
* @param    QVariant data
* @return   None
* @brief    Command to process.
*/
void SupervisorManager::bagListCommand(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                                       QMLEnums::BagsListAndBagDataCommands command,
                                       QVariant data)
{
  TRACE_LOG("");
  if (command <= QMLEnums::BagsListAndBagDataCommands::BLBDC_START ||
     command >= QMLEnums::BagsListAndBagDataCommands::BLBDC_END)
  {
    ERROR_LOG("Incorrect command: " << command);
    return;
  }

  QVariantList results;
  switch (srchLoc) {
  case QMLEnums::ArchiveFilterSrchLoc::LOCAL_SERVER:
    m_localArchiveBagLHndlr->bagListCommand(command, data, results, srchLoc);
    emit bagListCommandResults(srchLoc, command, results);
    break;
  case QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER:
    // Check if the data is modal then request is from qml,
    // else if the data is already a List of BagInfo then it is from nss
    // Not handled in Supervisor_workstation as Archive screen not present in this
    if (command == QMLEnums::BagsListAndBagDataCommands::BLBDC_FETCH_BAGS_LIST) {
      m_nss_handler.get()->fetchRemoteArchiveList(data);
    }
    else {
      m_remoteArchiveBagLHndlr->bagListCommand(command, data, results, srchLoc);
      emit bagListCommandResults(srchLoc, command, results);
    }
    break;
  case QMLEnums::ArchiveFilterSrchLoc::SEARCH_SERVER:
    m_searchArchiveBagListHandler->bagListCommand(command, data, results, srchLoc);
    emit bagListCommandResults(srchLoc, command, results);
    break;
  default:
    break;
  }
}

}  // end of namespace ws
}  // end of namespace analogic
