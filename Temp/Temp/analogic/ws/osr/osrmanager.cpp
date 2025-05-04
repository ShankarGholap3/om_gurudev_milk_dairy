/*!
* @file     osrmanager.cpp
* @author   Agiliad
* @brief    This file contains functions related to OsrManager which communicates with nss service to authenticate user and get live bag data.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include <analogic/ws/osr/osrmanager.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/archive/localarchivebaglisthandler.h>
#include <analogic/ws/common/archive/remotearchivebaglisthandler.h>

namespace analogic
{

namespace ws
{

/*!
* @fn       OsrManager
* @param    None
* @return   None
* @brief    Constructor for class OsrManager.
*/
OsrManager::OsrManager()
{
    TRACE_LOG("");
    // intialize OSR configuration
    INFO_LOG( "Creating instance of workstation config and initialize OSR configuration");
    m_workstation_config_handle.reset(new OsrConfig());
    THROW_IF_FAILED((m_workstation_config_handle.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
}

/*!
 * @fn       OsrManager
 * @param    None
 * @return   None
 * @brief    Destructor for class OsrManager.
 */
OsrManager::~OsrManager()
{
    TRACE_LOG("");
}
/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    Slot calls on exit if application.
*/
void OsrManager::onExit()
{
    TRACE_LOG("");
    DEBUG_LOG("Sending exit");
    m_nss_handler.reset();
    m_workstation_config_handle.reset();
    m_localArchiveBagLHndlr.reset();
    m_remoteArchiveBagLHndlr.reset();
    WorkstationManager::onExit();
}

/*!
* @fn       init
* @param    None
* @return   void
* @brief    Calls NssHandler init for intialization of nss and connection agent.
*/
void OsrManager::init()
{
    TRACE_LOG("");

    WorkstationManager::init();
    INFO_LOG( "Creating instance of OsrHandler");
    m_nss_handler.reset( new OsrHandler());

    //Creating instance of Archive BagList Handler
    m_localArchiveBagLHndlr.reset(new LocalArchiveBagListHandler());
    m_remoteArchiveBagLHndlr.reset(new RemoteArchiveBagListHandler());

    Semaphore::getInstance()->release(1);
    // notify bagdata acq handler about the bag arrival.
    connect(m_nss_handler.get(),
            SIGNAL(bagNotification(analogic::nss::
                                   NssBag*, QMLEnums::ArchiveFilterSrchLoc, std::string , QMLEnums::BagsListAndBagDataCommands)),
            m_bagdata_acq_handle.get(),
            SLOT(onBagNotification(
                     analogic::nss::NssBag*, QMLEnums::ArchiveFilterSrchLoc, std::string,
                     QMLEnums::BagsListAndBagDataCommands)),
            Qt::QueuedConnection);


    //notify to change screen or not to BAG_SCREENING screen
    connect(m_localArchiveBagLHndlr.get(), SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)),
            m_nss_handler.get(), SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::DirectConnection);

    // notify bagListHandler about the bagList arrival
    connect(m_nss_handler.get(), &NssHandler::bagListUpdated,
            this, &OsrManager::bagListCommand, Qt::DirectConnection);

    connect(m_remoteArchiveBagLHndlr.get(), &RemoteArchiveBagListHandler::
            getBagDataFromServer, m_nss_handler.get(), &NssHandler
            ::getBagDataFromServer, Qt::DirectConnection);

    // notify the status of bag archival
    connect(m_localArchiveBagLHndlr.get(), SIGNAL(bagArchivedStatus(int,QString,
                                                                    int)), this, SIGNAL(authstatuschanged(int,QString,int)));

    connect(this, SIGNAL(deAuthenticate()), m_nss_handler.get(), SLOT(
                onDeAuthenticate()), Qt::QueuedConnection);

    connect(this, &OsrManager::networkStatus,
            this, &OsrManager::onNetworkStatus);

    connect(this->getNWFaultHandle().get(), &NetworkFaultHandler::updateNetwork,
            this, &OsrManager::onNetworkUpdate, Qt::QueuedConnection);


    connect(this->getScannerFaultHandle().get(), &ScannerFaultHandler::updateScannerNetwork,
            this, &OsrManager::onScannerNetworkUpdate, Qt::QueuedConnection);

    connect(m_bagdata_acq_handle.get(), &BagDataAcqHandler::saveRemoteArchiveBag,
            m_remoteArchiveBagLHndlr.get(),&RemoteArchiveBagListHandler::onSaveRemoteArchiveBag, Qt::QueuedConnection);

    NSSAgentAccessInterface* nssInterface = NSSAgentAccessInterface::getInstance();
    if(nssInterface != NULL)
    {
        AgentListener* osragentlistner = nssInterface->getAgentListener();
        if (osragentlistner)
        {
            connect(osragentlistner, &AgentListener::networkStatusChanged,
                    this, &OsrManager::networkStatus, Qt::QueuedConnection);

        }
        // message received
        connect(nssInterface,
                &NSSAgentAccessInterface::displayMsg,
                this,
                &WorkstationManager::authstatuschanged,
                Qt::QueuedConnection);

        connect(nssInterface,
                &NSSAgentAccessInterface::requestBagPickupsig,
                this,
                &WorkstationManager::requestBagPickupsig,
                Qt::QueuedConnection);

        connect(this,
                &WorkstationManager::notifyBagIsPickedUpsig,
                nssInterface,
                &NSSAgentAccessInterface::onNotifyBagIsPickedUp,
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
void OsrManager::onNetworkStatus(QMLEnums::NetworkState status)
{
    DEBUG_LOG("WorkstationManager: Received network status: "<<status);
    getNWFaultHandle()->networkStatus(status);
    if (status == QMLEnums::NetworkState::UNREGISTERED) {
        m_bagdata_acq_handle->stopToReceiveBag();

        emit stateChanged(QMLEnums::SET_TO_OFFLINE);
    }
}

/*!
* @fn       onNetworkUpdate
* @param    None
* @return   None
* @brief    This slot gets call if networks gets disconnected.
*/
void OsrManager::onNetworkUpdate()
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
            DEBUG_LOG("Created OSR agent");
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
* @fn       onScannerNetworkUpdate
* @param    None
* @return   None
* @brief    This slot gets call if scanner network gets updated
*/
void OsrManager::onScannerNetworkUpdate()
{
    DEBUG_LOG("Recreate scanner operation agent in case of disconnection.");
    CreateScannerOperationAgent();
}

/*!
* @fn       createOsrAgent
* @param    None
* @return   int - Agent creation status (0/1 - success /failure).
* @brief    Creates Osr agent.
*/
int OsrManager::createOsrAgent()
{
    TRACE_LOG("");
    // 1. Read properties from configuration file
    analogic::nss::OsrAgentStaticProperties properties = OsrConfig::getOsrAgentProperties();

    // 2. Pass those properties to osrhandler for creating osr agent
    return m_nss_handler->createWSAgent(properties);
}

/*!
* @fn       RegistrationChanged
* @param    analogic::nss::OsrAgent* agent
* @return   None
* @brief    Gives Registration Changed status of the workstation to the nss service.
*/
void OsrManager::RegistrationChanged(analogic::nss::OsrAgent* agent)
{
    TRACE_LOG("");
    if (agent == NULL)
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
void OsrManager::onCommandButtonEvent(QMLEnums::ModelEnum model, QString data)
{
    TRACE_LOG("");
    if (QMLEnums::ModelEnum::NETWORK_AUTHENTICATION_MODEL == model && data != "logout")
    {

        QJsonDocument jsondoc = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jobj =  jsondoc.object();
        QVariantMap mapobj =  jobj.toVariantMap();
        QString username = QVariant(mapobj["username"]).toString();
        QString password = QVariant(mapobj["password"]).toString();
        QString pin = QVariant(mapobj["pin"]).toString();

        if(getNWFaultHandle()->getNetworkState() == QMLEnums::NetworkState::REGISTERED)
        {
            INFO_LOG("Request to authenticate user on Nss");
            authenticate(username, password, pin);
        }
        else if(getNWFaultHandle()->getNetworkState() != QMLEnums::NetworkState::REGISTERED)
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
void OsrManager::onUpdateState(QMLEnums::AppState state)
{
    DEBUG_LOG("Update App State to: "<< state);
    m_nss_handler->onUpdateState(state);
}

/*!
* @fn       bagListCommand
* @param    QMLEnums::ArchiveFilterSrchLoc
* @param    QMLEnums::BagsListAndBagDataCommands command
* @param    QVariant data
* @return   None
* @brief    Command to process.
*/
void OsrManager::bagListCommand(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                                QMLEnums::BagsListAndBagDataCommands command,
                                QVariant data)
{
    DEBUG_LOG("start of WorkstationManager::bagListCommand");
    if(command <= QMLEnums::BagsListAndBagDataCommands::BLBDC_START ||
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
        if (command == QMLEnums::BagsListAndBagDataCommands::BLBDC_FETCH_BAGS_LIST) {
            m_nss_handler.get()->fetchRemoteArchiveList(data);
        }
        else {

            m_remoteArchiveBagLHndlr->bagListCommand(command, data, results, srchLoc);
            emit bagListCommandResults(srchLoc, command, results);
        }
        break;
    default:
        break;
    }
}
}  // end of namespace ws
}  // end of namespace analogic
