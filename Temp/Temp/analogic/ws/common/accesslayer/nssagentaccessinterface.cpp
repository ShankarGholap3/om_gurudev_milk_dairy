/*!
* @file     nssagentaccessinterface.cpp
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <algorithm>

#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>
#include <analogic/ws/osr/osragentlistenerinterface.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/search/searchagentlistenerinterface.h>
#include <analogic/ws/common/bagdata/baginfo.h>
#include <qtranslator.h>

namespace analogic
{
namespace ws
{
NSSAgentAccessInterface* NSSAgentAccessInterface::m_nal = NULL;
/*!
* @fn       NSSAgentAccessInterface();
* @param    QObject* - parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
NSSAgentAccessInterface::NSSAgentAccessInterface(QObject* parent):QObject(parent)
{
  TRACE_LOG("");
  m_auth_agent = NULL;
  m_connection_agent = NULL;
  m_osr_agent = NULL;
  m_search_agent = NULL;
  m_scannerAgent = NULL;
  m_agentListener = NULL;
  m_archiveAgentListener = NULL;
  m_last_status_event_time_ = 0;
  m_lastOsrReadyStateSent = false;
  m_nssEventData = new NssEventData();
  m_nssEventData->insertNssEventData();
  m_logsink=new analogic::ws::wsLoggingSink();
  m_supervisor_agent = nullptr;
  analogic::nss::NssLog::SetLoggingSink(m_logsink,
                                        analogic::nss::NssLogLevel(Logger::severity_level));
  connect(this, &NSSAgentAccessInterface::deAuthenticate, this,
          &NSSAgentAccessInterface::onDeAuthenticate, Qt::QueuedConnection);
  connect(this, &NSSAgentAccessInterface::Disconnect, this,
          &NSSAgentAccessInterface::onDisconnect, Qt::QueuedConnection);
  connect(this, &NSSAgentAccessInterface::exit, this,
          &NSSAgentAccessInterface::onExit, Qt::QueuedConnection);
  m_nssaddress.clear();
  m_NSSLiveControllerconnected = false;

  m_wsType = WorkstationConfig::getInstance()->getWorkstationNameEnum();

  if (m_wsType == QMLEnums::SEARCH_WORKSTATION  || m_wsType == QMLEnums::SUPERVISOR_WORKSTATION)
  {
    m_bhsType =  WorkstationConfig::getInstance()->getBhsType();
    m_bhsBinType = WorkstationConfig::getInstance()->getBhsBinType();
    m_BHSBagListnerInterface = new analogic::ws::BhsBagListenerInterface();

    INFO_LOG("m_bhsType: " << m_bhsType);

    if (strcmp(m_bhsType.c_str(), "SCARABEE_SL") == 0)
    {
      INFO_LOG("NSSAgentAccessInterface, BHS: SCARABEE SL");
      m_scarabeeConfig = new analogic::ancp00::ScarabeeConfig();
      m_scarabeeConfig->lm_soap_client_endpoint = WorkstationConfig::getInstance()->getSoapClientEndPoint();
      m_scarabeeConfig->sws_soap_server_port = WorkstationConfig::getInstance()->getSoapServerPort();
      m_scarabeeBHSInterface =  new analogic::ancp00::ScarabeeBhsImpl(m_scarabeeConfig);
      m_BHSBagInterface = m_scarabeeBHSInterface->CreateBagInterface(m_BHSBagListnerInterface);
    }
    else if ( (strncmp(m_bhsType.c_str(), "STANDALONE", 10) == 0)
              || (strcmp(m_bhsType.c_str(), "VANDERLANDE_LM") == 0)
              )
    {
      if (strcmp(m_bhsType.c_str(), "VANDERLANDE_LM") == 0)
      {
        INFO_LOG("NSSAgentAccessInterface, BHS: VANDERLANDE_LM");
      }
      {
        INFO_LOG("NSSAgentAccessInterface, BHS: STANDALONE (I, II, III, etc.)");
      }
      m_bhsWorkstationInterfaceConfig = new analogic::ancp00::BhsWorkstationInterfaceConfig();
      // For these variants, the defaults should be used and not the editable workstation.xml parameters (Al P.)
      //            m_bhsWorkstationInterfaceConfig->ip_address_ = WorkstationConfig::getSwsBhsServerIpAddr();
      //            m_bhsWorkstationInterfaceConfig->port_ = WorkstationConfig::getSwsBhsServerPort();
      m_bhsWorkstationInterface = new analogic::ancp00::BhsWorkstationInterfaceServerImpl(m_bhsWorkstationInterfaceConfig);
      m_BHSBagInterface = m_bhsWorkstationInterface->CreateBagInterface(m_BHSBagListnerInterface);
      // startup RFID reader with bin type argument
      std::string tmp = "Starting -b " + m_bhsBinType;
      INFO_LOG(tmp.c_str());
      std::string sys_arg = "./OmronRfidTagReader.sh -b " + m_bhsBinType;
      system(sys_arg.c_str());
    }
    else if ( (strcmp(m_bhsType.c_str(), "HERBERT_TRS") == 0)  ||
              (strcmp(m_bhsType.c_str(), "RAPISCAN_TRS") == 0)
              )
    {
      INFO_LOG("NSSAgentAccessInterface, BHS: RAPISCAN/HERBERT TRS");
      m_bhsWorkstationInterfaceConfig = new analogic::ancp00::BhsWorkstationInterfaceConfig();
      // For this variant, the defaults should be used and not the editable workstation.xml parameters (Al P.)
      //            m_bhsWorkstationInterfaceConfig->ip_address_ = WorkstationConfig::getSwsBhsServerIpAddr();
      //            m_bhsWorkstationInterfaceConfig->port_ = WorkstationConfig::getSwsBhsServerPort();
      m_bhsWorkstationInterface = new analogic::ancp00::BhsWorkstationInterfaceServerImpl(m_bhsWorkstationInterfaceConfig);
      m_BHSBagInterface = m_bhsWorkstationInterface->CreateBagInterface(m_BHSBagListnerInterface);
      // startup RFID reader
      INFO_LOG("Starting ./RapiscanRfidTagReader.sh");
      system("./RapiscanRfidTagReader.sh");
    }
    else if (strcmp(m_bhsType.c_str(), "MACH_TRS") == 0)
    {
      INFO_LOG("NSSAgentAccessInterface, BHS: MACH TRS");
      m_bhsWorkstationInterfaceConfig = new analogic::ancp00::BhsWorkstationInterfaceConfig();
      m_bhsWorkstationInterface = new analogic::ancp00::BhsWorkstationInterfaceServerImpl(m_bhsWorkstationInterfaceConfig);
      m_BHSBagInterface = m_bhsWorkstationInterface->CreateBagInterface(m_BHSBagListnerInterface);
      // startup RFID reader with bin type argument
      std::string tmp = "Starting OmronRfidTagReader.sh -b " + m_bhsBinType;
      INFO_LOG(tmp.c_str());
      std::string sys_arg = "./OmronRfidTagReader.sh -b " + m_bhsBinType;
      system(sys_arg.c_str());
    }
    else
    {
      INFO_LOG("NSSAgentAccessInterface, No BHS");
    }
  }
  else
  {
    m_scarabeeConfig = NULL;
    m_scarabeeBHSInterface=NULL;
    m_bhsWorkstationInterfaceConfig = NULL;
    m_bhsWorkstationInterface=NULL;
    m_BHSBagListnerInterface=NULL;
    m_BHSBagInterface=NULL;
  }
  //m_auth_result.result_type == analogic::nss::AuthenticationResult::INVALID_LOGIN;
}

/*!
* @fn       getBHSBagListnerInterface
* @param    void
* @return   analogic::ancp00::BhsBagInterface*
* @brief    get BHS bag listner
*/
analogic::ws::BhsBagListenerInterface* NSSAgentAccessInterface::getBHSBagListnerInterface()
{
  return m_BHSBagListnerInterface;
}

/*!
 * @fn getBHSBagInterface
 * @param void
 * @return analogic::ancp00::BhsBagInterface*
 * @brief get BHS interface directly
 */
analogic::ancp00::ExternalBhsBagInterface* NSSAgentAccessInterface::getBHSBagInterface()
{
  return m_BHSBagInterface;
}

/*!
* @fn       mapNssAuthenticationResultToWS
* @param    analogic::nss::AuthenticationResult authResult
* @return   QMLEnums::WSAuthenticationResult
* @brief    returns QMLEnums::WSAuthenticationResult mapped to analogic::nss::AuthenticationResult.
*/
QMLEnums::WSAuthenticationResult NSSAgentAccessInterface::mapNssAuthenticationResultToWS(nss::AuthenticationResult authResult)
{
  TRACE_LOG("");
  QMLEnums::WSAuthenticationResult wsauthresult = QMLEnums::WSAuthenticationResult::IO_ERROR;
  switch(authResult.result_type) {
  case analogic::nss::AuthenticationResult::SUCCESS:
    wsauthresult = QMLEnums::WSAuthenticationResult::SUCCESS;
    WorkstationConfig::getInstance()->setOperatorCompany(QString::fromStdString(authResult.credential.company));
    break;
  case analogic::nss::AuthenticationResult::INVALID_LOGIN:
    wsauthresult = QMLEnums::WSAuthenticationResult::INVALID_LOGIN;
    break;
  case analogic::nss::AuthenticationResult::CONCURRENT_LOGIN_ERROR:
    wsauthresult = QMLEnums::WSAuthenticationResult::CONCURRENT_LOGIN_ERROR;
    break;
  case analogic::nss::AuthenticationResult::AUTHENTICATION_METHOD_EXPIRED:
    wsauthresult = QMLEnums::WSAuthenticationResult::AUTHENTICATION_METHOD_EXPIRED;
    break;
  case analogic::nss::AuthenticationResult::LOCKED_OUT:
    wsauthresult = QMLEnums::WSAuthenticationResult::LOCKED_OUT;
    break;
  case analogic::nss::AuthenticationResult::INACTIVE:
    wsauthresult = QMLEnums::WSAuthenticationResult::INACTIVE;
    break;
  case analogic::nss::AuthenticationResult::IO_ERROR:
    wsauthresult = QMLEnums::WSAuthenticationResult::IO_ERROR;
    break;
  default:
    break;
  }
  return wsauthresult;
}

/*!
* @fn       mapNssExpiredAuthMethodToWs
* @param    analogic::nss::AuthMethodType authMethod
* @return   QMLEnums::AuthMethodType
* @brief    returns QMLEnums::AuthMethodType mapped to analogic::nss::AuthMethodType.
*/
QMLEnums::AuthMethodType NSSAgentAccessInterface::mapNssExpiredAuthMethodToWs(AuthMethodType authMethod)
{
  TRACE_LOG("");
  QMLEnums::AuthMethodType authResult = QMLEnums::AUTH_METHOD_UNDEFINED;
  switch(authMethod)
  {
  case analogic::nss::AUTH_METHOD_CARD:
    authResult = QMLEnums::AUTH_METHOD_CARD;
    break;
  case analogic::nss::AUTH_METHOD_PASSWORD:
    authResult = QMLEnums::AUTH_METHOD_PASSWORD;
    break;
  case analogic::nss::AUTH_METHOD_PIN:
    authResult = QMLEnums::AUTH_METHOD_PIN;
    break;
  case analogic::nss::AUTH_METHOD_PIV:
    authResult = QMLEnums::AUTH_METHOD_PIV;
    break;
  case analogic::nss::AUTH_METHOD_UNDEFINED:
    authResult = QMLEnums::AUTH_METHOD_UNDEFINED;
  }
  return authResult;
}

/*!
* @fn       mapNssAuthenticationResultToWS
* @param    analogic::nss::ChangeAuthMethodResult::Type authResult
* @return   QMLEnums::WSBasicCommandResult
* @brief    returns QMLEnums::WSAuthenticationResult mapped to analogic::nss::ChangeAuthMethodResult::Type.
*/
QMLEnums::WSBasicCommandResult NSSAgentAccessInterface::mapNssBasicCommandResultToWS(nss::ChangeAuthMethodResult::Type result)
{
  TRACE_LOG("");
  QMLEnums::WSBasicCommandResult wsauthresult = QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_IO_ERROR;
  switch(result) {
  case analogic::nss::ChangeAuthMethodResult::SUCCESS:
    wsauthresult = QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_SUCCESS;
    break;
  case analogic::nss::ChangeAuthMethodResult::IO_ERROR:
    wsauthresult = QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_IO_ERROR;
    break;
  case analogic::nss::ChangeAuthMethodResult::INVALID_CREDENTIAL:
    wsauthresult = QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_INVALID_CREDENTIAL;
    break;
  case analogic::nss::ChangeAuthMethodResult::INVALID_AUTH_METHOD:
    wsauthresult = QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_INVALID_AUTH_METHOD;
    break;
  case analogic::nss::ChangeAuthMethodResult::DETAIL_RULE_VIOLATION:
    wsauthresult = QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_DETAIL_RULE_VIOLATION;
    break;
  case analogic::nss::ChangeAuthMethodResult::INVALID_OLD_DETAIL:
    wsauthresult = QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_INVALID_OLD_DETAIL;
    break;
  default:
    break;
  }
  return wsauthresult;
}

/*!
* @fn       mapWsBasicCommandResultToNSS
* @param    QMLEnums::WSBasicCommandResult
* @return   analogic::nss::BasicCommandResult
* @brief    returns analogic::nss::BasicCommandResult mapped to QMLEnums::WSBasicCommandResult.
*/
BasicCommandResult NSSAgentAccessInterface::mapWsBasicCommandResultToNSS(QMLEnums::WSBasicCommandResult result)
{
  TRACE_LOG("");
  analogic::nss::BasicCommandResult wsauthresult = analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_IO_ERROR;
  switch(result) {
  case QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_SUCCESS:
    wsauthresult = analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_SUCCESS;
    break;
  case QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_IO_ERROR:
    wsauthresult = analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_IO_ERROR;
    break;
  case QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_INVALID_REGISTRATION:
    wsauthresult = analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_INVALID_REGISTRATION;
    break;
  case QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_INVALID_PERMISSION:
    wsauthresult = analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_INVALID_PERMISSION;
    break;
  case QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_INVALID_ARGUMENTS:
    wsauthresult = analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_INVALID_ARGUMENTS;
    break;
  case QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_INVALID_STATE:
    wsauthresult = analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_INVALID_STATE;
    break;
  case QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_EXECUTION_ERROR:
    wsauthresult = analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_EXECUTION_ERROR;
    break;
  default:
    break;
  }
  return wsauthresult;
}

/*!
* @fn       mapBhsDivertedDecisionToWS
* @param    analogic::nss::BhsDiverterDecision diverterDecision
* @return   QMLEnums::BhsDiverterDecision
* @brief    returns QMLEnums::BhsDiverterDecision mapped to analogic::nss::BhsDiverterDecision.
*/
QMLEnums::BhsDiverterDecision NSSAgentAccessInterface::mapBhsDivertedDecisionToWS(BhsDiverterDecision diverterDecision)
{
  TRACE_LOG("");
  QMLEnums::BhsDiverterDecision bhsDecision = QMLEnums::BhsDiverterDecision::BHS_DIVERTER_DECISION_NONE;
  switch(diverterDecision) {
  case analogic::nss::BhsDiverterDecision::BHS_DIVERTER_DECISION_ACCEPT:
    bhsDecision = QMLEnums::BhsDiverterDecision::BHS_DIVERTER_DECISION_ACCEPT;
    break;
  case analogic::nss::BhsDiverterDecision::BHS_DIVERTER_DECISION_REJECT:
    bhsDecision = QMLEnums::BhsDiverterDecision::BHS_DIVERTER_DECISION_REJECT;
    break;
  }
  return bhsDecision;
}

NSSAgentAccessInterface* NSSAgentAccessInterface::getInstance()
{
  ANALOGIC_NSS_VERIFY_VERSION;
  TRACE_LOG("");
  if(!m_nal)
  {
    INFO_LOG("Creating NSSAgentAccessInterface instance");
    m_nal = new NSSAgentAccessInterface();
    INFO_LOG("NSS Agent library version: " << analogic::nss::
             GetAgentLibraryVersion();)
  }
  return m_nal;
}

/*!
* @fn       ~NSSAgentAccessInterface
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
NSSAgentAccessInterface::~NSSAgentAccessInterface()
{
  TRACE_LOG("");

  //TO DO: while deleting m_scarabeeBHSInterface, it got crashed here. For now we have marked it as NULL. We will check it later
  //SAFE_DELETE(m_scarabeeBHSInterface);
  m_scarabeeBHSInterface = NULL;
  SAFE_DELETE(m_scarabeeConfig);
  SAFE_DELETE(m_bhsWorkstationInterfaceConfig);
  SAFE_DELETE(m_bhsWorkstationInterface);
  SAFE_DELETE(m_BHSBagListnerInterface);
  SAFE_DELETE(m_BHSBagInterface);
  SAFE_DELETE(m_logsink);
  SAFE_DELETE(m_bhsWorkstationInterfaceConfig);

  emit exit();
}

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    Destroys NSSAgentAccessInterface.
*/
void NSSAgentAccessInterface::destroyInstance()
{
  TRACE_LOG("");
  if(m_nal)
  {
    m_nal->setOsrReadyState(false);
    m_nal->deAuthenticate();
    m_nal->destroyAgentListener();
    m_nal->destroyScannerAgent();
    m_nal->destroyWSAgent();
    m_nal->destroyAuthenticationAgent();
    m_nal->destroyConnection();

    SAFE_DELETE(m_nal)

  }
}

/*!
* @fn       createConnection
* @param    const std::string address
* @return   int  (S_OK/E_POINTER) connection success/failure.
* @brief    Creates connection agent and connects to nss.
*/
int NSSAgentAccessInterface::createConnection(const std::string& nss_address)
{
  TRACE_LOG("");
  try{
    if(!nss_address.empty())
    {
      m_nssaddress = nss_address.c_str();
      DEBUG_LOG("NSS ip address :" <<m_nssaddress.toStdString());
    }
    if((m_connection_agent !=NULL))
    {
      DEBUG_LOG("Destroy Existing Connection Agent.");
      destroyScannerAgent();
      destroyAuthenticationAgent();
      destroyWSAgent();
      destroyConnection();
    }
    if (NULL == m_connection_agent)
    {
      // Create and intialize connection agent to communicate with nss library
      m_connection_agent = analogic::nss::ConnectionAgent::CreateConnectionAgent();
      if (NULL == m_connection_agent)
      {
        ERROR_LOG("Failed Connection agent creation");
        return Errors::E_POINTER;
      }
      INFO_LOG("Connection agent created");
    }
    else
    {
      DEBUG_LOG("Connection agent is already created");
    }
  }catch(...)
  {
    ERROR_LOG("NSS create connection agent failed");
    return Errors::E_FAIL;
  }
  try{
    if((m_connection_agent != NULL))
    {
      bool created = m_connection_agent->Connect(m_nssaddress.toStdString(),
                                                 "{transport: ssl, username: agent, password: a1cUk8b*gent, sasl-mechanism: PLAIN}", NULL);
      if(created)
      {
        INFO_LOG("Connected to network : " <<  m_nssaddress.toStdString());
        m_NSSLiveControllerconnected = true;
        return Errors::S_OK;
      }
      else
      {
        ERROR_LOG("Network Connection failed : "<< m_nssaddress.toStdString());
        return Errors::E_FAIL;
      }
    }
    else if((m_connection_agent != NULL))
    {
      INFO_LOG("Already Connected to network");
      return Errors::S_OK;
    }
  }catch(...)
  {
    ERROR_LOG("Network Connection failed");
    return Errors::E_FAIL;
  }
  return Errors::S_OK;
}

/*!
* @fn       destroyConnection
* @param    void
* @return   void
* @brief    destroy connection agent and disconnects to nss.
*/
void NSSAgentAccessInterface::destroyConnection()
{
  TRACE_LOG("");
  onDisconnect();
  SAFE_DELETE(m_connection_agent);
}

/*!
* @fn       authenticate
* @param    const std::string & - username
* @param    const std::string & - password
* @return   QMLEnums::WSAuthenticationResult result
* @brief    Authenticate user on the connection.
*/
analogic::nss::AuthenticationResult NSSAgentAccessInterface::
authenticate(const std::string &username, const std::map<analogic::nss::AuthMethodType, std::string>& auth_methods)
{
  TRACE_LOG("");
  try
  {
    // Authenticate user
    int ret = Errors::S_OK;
    if((m_connection_agent == NULL))
    {
      DEBUG_LOG("Request to create connection");
      ret = createConnection(m_nssaddress.toStdString());
      if (static_cast<int>(Errors::S_OK != ret ))
      {
        ERROR_LOG("Failed to create connection agent and connection with Nss.");
      }
    }
    if( NULL == m_auth_agent)
    {
      DEBUG_LOG("Request to create authentication agent.");
      createAuthenticationAgent();
    }
    if ((NULL != m_connection_agent) && (static_cast<int>(Errors::S_OK == ret )) &&
        (NULL != m_auth_agent) && m_NSSLiveControllerconnected)
    {
      m_auth_agent->Authenticate(username, auth_methods, &m_auth_result);
      INFO_LOG("Authentication." << "result =" << m_auth_result.result_type);

      if ((m_auth_result.result_type != analogic::nss::AuthenticationResult::SUCCESS) &&
          m_auth_result.result_type != analogic::nss::AuthenticationResult::AUTHENTICATION_METHOD_EXPIRED) {
        ERROR_LOG("Authentication Failed for User:" << username);
      }
      else if(m_auth_result.result_type == analogic::nss::AuthenticationResult::AUTHENTICATION_METHOD_EXPIRED)
      {
        ERROR_LOG("Authentication Method Expired for User:" << username);
        m_expiredAuthMethod = mapNssExpiredAuthMethodToWs(m_auth_result.result_associated_auth_method);
      }
      else if(m_auth_result.result_type == analogic::nss::AuthenticationResult::SUCCESS)
      {
        qint64 mSecsSinceEpoch = static_cast<qint64>(m_auth_result.auth_method_expiration_dates[analogic::nss::AUTH_METHOD_PASSWORD]);
        m_passwordExpirationDays = static_cast<int>(ceil(static_cast<double>(mSecsSinceEpoch - QDateTime::currentMSecsSinceEpoch())/(1000*60*60*24)));
        m_passwordExpirationDays -= 1;
        DEBUG_LOG("Password Expires in :" << m_passwordExpirationDays << " days.");
      }
    }
    else
    {
      ERROR_LOG("Authentication Failed. Connection Agent or Authetication agent is NULL or Network Connection failed");
    }
  }
  catch(...)
  {
    ERROR_LOG("Network Authentication failed");
  }
  return m_auth_result;
}

/*!
* @fn       changePassword
* @param    const std::string& - username
* @param    const std::string& - oldAuthMethodDetail
* @param    const std::string& - newAuthMethodDetail
* @param    QMLEnums::AuthMethodType - authMethodType
* @return   QMLEnums::WSBasicCommandResult - result
* @brief    Changes password of the user to new password.
*/
QMLEnums::WSBasicCommandResult NSSAgentAccessInterface::changePassword(
    const std::string &username, const std::string &oldAuthMethodDetail, const std::string &newAuthMethodDetail, QMLEnums::AuthMethodType authMethodType)
{
  TRACE_LOG("");
  QMLEnums::WSBasicCommandResult changPwdResult = QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_IO_ERROR;
  analogic::nss::ChangeAuthMethodResult::Type result;
  analogic::nss::AuthMethodType nssAuthMethodType = static_cast<analogic::nss::AuthMethodType>(authMethodType);
  try
  {
    // Change password of the user
    int ret = Errors::S_OK;
    if((m_connection_agent == NULL))
    {
      DEBUG_LOG("Request to create connection");
      ret = createConnection(m_nssaddress.toStdString());
      if (static_cast<int>(Errors::S_OK != ret ))
      {
        ERROR_LOG("Failed to create connection agent and connection with Nss.");
      }
    }
    if( NULL == m_auth_agent)
    {
      DEBUG_LOG("Request to create authentication agent.");
      createAuthenticationAgent();
    }
    if ((NULL != m_connection_agent) && (static_cast<int>(Errors::S_OK == ret )) &&
        (NULL != m_auth_agent) && m_NSSLiveControllerconnected)
    {
      m_auth_agent->ChangeAuthMethod(m_auth_result.credential.credential_id, nssAuthMethodType
                                     , oldAuthMethodDetail, newAuthMethodDetail, &m_change_auth_method_result);
      result = m_change_auth_method_result.change_auth_method_result_type;
      INFO_LOG("Change Password. " << "result =" << result);

      if (result != analogic::nss::ChangeAuthMethodResult::SUCCESS) {
        ERROR_LOG("Change Password Failed for User:" << username);
      }
    }
    else
    {
      ERROR_LOG("Change Password. Connection Agent or Authetication agent is NULL or Network Connection failed");
    }

    changPwdResult = mapNssBasicCommandResultToWS(result);
  }
  catch(...)
  {
    ERROR_LOG("Network Change Password failed");
  }
  return changPwdResult;
}

/*!
* @fn       onDeAuthenticate
* @param    None
* @return   None
* @brief    Deauthenticate user on the connection.
*/
void NSSAgentAccessInterface::onDeAuthenticate()
{
  try
  {
    if (m_auth_agent)
    {
      DEBUG_LOG("Deauthenticate agent.");
      m_auth_agent->Deauthenticate(m_auth_result.credential.credential_id);
      INFO_LOG("Deauthentication result: "<< m_auth_result.result_type);
      m_auth_result.result_type = analogic::nss::AuthenticationResult::INVALID_LOGIN;
    }
  }
  catch(...)
  {
    ERROR_LOG("Error in Deauthentication");
  }
}

/*!
* @fn       onDisconnect
* @param    None
* @return   None
* @brief    disconnect workstation on the exit of application.
*/
void NSSAgentAccessInterface::onDisconnect()
{
  try
  {
    if ((NULL != m_connection_agent))
    {
      INFO_LOG("Disconnect from Network");
      m_connection_agent->Disconnect();
    }
  }
  catch(...)
  {
    ERROR_LOG("network disconnect error");
  }
  m_NSSLiveControllerconnected = false;
}

/*!
* @fn       createAuthenticationAgent
* @param    None
* @return   int  (S_OK/E_POINTER) Agent creation success/failure.
* @brief    Creates Authentication agent to authenticate user on the connection.
*/
int NSSAgentAccessInterface::createAuthenticationAgent()
{
  try
  {
    destroyAuthenticationAgent();
    if ((NULL != m_connection_agent)
        && (m_auth_agent == NULL) && m_NSSLiveControllerconnected)
    {
      // Create Authenticatation agent
      m_auth_agent = m_connection_agent->CreateAuthenticationAgent();
      if (m_auth_agent == NULL) {
        ERROR_LOG("Failed to create Authentication Agent");
        return Errors::E_POINTER;
      }
      DEBUG_LOG("Created Authentication Agent");
      return Errors::S_OK;
    }
  }
  catch(...)
  {
    ERROR_LOG("Authentication agent creation fail");
    return Errors::E_FAIL;
  }
  return Errors::S_OK;
}
/*!
* @fn       destroyAuthenticationAgen
* @param    void
* @return   void
* @brief    destroy authentication agent.
*/
void NSSAgentAccessInterface::destroyAuthenticationAgent()
{
  TRACE_LOG("");
  SAFE_DELETE(m_auth_agent);
}

/*!
* @fn       destroyWSAgent
* @param    void
* @return   void
* @brief    destroy WS agent.
*/
void NSSAgentAccessInterface::destroyWSAgent()
{
  TRACE_LOG("");
  if ((m_wsType == QMLEnums::OSR_WORKSTATION ) && (m_osr_agent != NULL) )
  {
    SAFE_DELETE(m_osr_agent);
  }
  else if ((m_wsType == QMLEnums::SEARCH_WORKSTATION) && (m_search_agent != NULL) )
  {
    //SAFE_DELETE(m_search_agent);
    m_search_agent=NULL;
  }
  else if ((m_wsType == QMLEnums::SUPERVISOR_WORKSTATION) && (m_supervisor_agent != nullptr) )
  {
    m_supervisor_agent = nullptr;
  }
}

void NSSAgentAccessInterface::destroyAgentListener()
{
  SAFE_DELETE(m_agentListener);
  SAFE_DELETE(m_archiveAgentListener);
}
/*!
* @fn       createWSAgent
* @param    analogic::nss::OsrAgentStaticProperties static_properties
* @return   Agent creation status (S_OK/E_POINTER - success/failure).
* @brief    Creates Osr agent.
*/
int NSSAgentAccessInterface::createWSAgent(
    analogic::nss::OsrAgentStaticProperties static_properties)
{
  TRACE_LOG("");
  try
  {
    destroyWSAgent();
    if((NULL != m_connection_agent)
       && (m_osr_agent == NULL) && m_NSSLiveControllerconnected)
    {
      DEBUG_LOG("Sending request to create Osr Agent...");
      // Create OSR agent and set credentials
      m_osr_agent = m_connection_agent->CreateOsrAgent(
            &static_properties,(dynamic_cast<analogic::nss::
                                OsrAgentListener*>(m_agentListener)), &m_registration_result);
      if (m_osr_agent == NULL) {
        ERROR_LOG (" Failed to create OSR Agent with registration "
                   "result: "<< m_registration_result.result);
        return Errors::E_POINTER;
      }
      DEBUG_LOG("OSR Agent created");
      return Errors::S_OK;
    }
    else if((m_connection_agent != NULL) &&
            m_osr_agent)
    {
      DEBUG_LOG("OSR Agent is already created");
      return Errors::S_OK;
    }
    else
    {
      DEBUG_LOG("OSR agent creation fail");
      return Errors::E_FAIL;
    }
  }
  catch(...)
  {
    ERROR_LOG("OSR agent creation fail");
    return Errors::E_FAIL;
  }
}

/*!
* @fn       setOsrReadyState
* @param    bool - readystate
* @return   OSR state change (S_OK/E_FAIL - success/failure).
* @brief    Sets Osr state.
*/
int NSSAgentAccessInterface::setOsrReadyState(bool readystate)
{
  try
  {
    if((NULL != m_osr_agent) && m_NSSLiveControllerconnected)
    {
      INFO_LOG("Set Osr Ready state to: "<< readystate);
      m_lastOsrReadyStateSent = readystate;
      return m_osr_agent->SetReadyToReceive(readystate);
    }
    else
    {
      DEBUG_LOG("Couldnot set Osr Ready state");
      m_lastOsrReadyStateSent = readystate;
      return Errors::E_FAIL;
    }

  }
  catch(...)
  {
    ERROR_LOG("set OSR ready state fail");
    return Errors::E_FAIL;
  }
}

/*!
* @fn       getOsrReadyState
* @param    none
* @return   bool readystate
* @brief    Returns the last OSR ready state we've set.
*/
bool NSSAgentAccessInterface::getOsrReadyState()
{
  DEBUG_LOG("Get osr ready state.");
  return m_lastOsrReadyStateSent;
}

/*!
* @fn       setWSStatus
* @param    const std::string& - status
* @return   None
* @brief    Sets WS state.
*/
void NSSAgentAccessInterface::setWSStatus(const std::string&  status)
{
  INFO_LOG("Set WS Status to :" << status);
}

/*!
* @fn       onBagFinishUpdates
* @param    analogic::nss::NssBag bag
* @return   None
* @brief    notifies nss Bag updatation is finished.
*/
void NSSAgentAccessInterface::onBagFinishUpdates(analogic::nss::NssBag* bag)
{
  TRACE_LOG("");
  if (( NULL != bag) && m_NSSLiveControllerconnected && (NULL != m_auth_agent))
  {
    INFO_LOG("Bag Flow Event:: Notify Bag update finished"<<bag->get_transfer_bag_identifier());
    bag->Finished(analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_SUCCESS);
    SAFE_DELETE(bag);
  }
  else
  {
    ERROR_LOG("Bag Data is NULL. Can't send finished signal.");
  }
}
/*!
* @fn       destroySCannerAgent
* @param    void
* @return   void
* @brief    destroy scanner agent.
*/
void NSSAgentAccessInterface::destroyScannerAgent()
{
  INFO_LOG("Destroy Scanner Agent");
  SAFE_DELETE(m_scannerAgent);
}

/*!
* @fn       getScannerStatus
* @param    None
* @return   analogic::nss::ScannerAgentStatus* - scannerAgentStatus
* @brief    Returns the current status information received from the scanner.
*/
analogic::nss::ScannerAgentStatus* NSSAgentAccessInterface::getScannerStatus()
{
  try
  {
    TRACE_LOG("");
    if((m_scannerAgent) && m_NSSLiveControllerconnected)
    {
      INFO_LOG("Getting Scanner status");
      return m_scannerAgent->get_status();
    }
  }
  catch(...)
  {
    ERROR_LOG("Error while getting scanner status");
  }
  ERROR_LOG("Error while getting scanner status");
  return  NULL;
}

/*!
* @fn       setScannerState
* @param    QMLEnums::ScannerState state to be set
* @return   int - State changed (S_OK/E_FAIL - success/failure).
* @brief    Request to change the scanner state to Scanning or Standby depending upon argument.
*/
int NSSAgentAccessInterface::setScannerState(QMLEnums::ScannerState state)
{
  TRACE_LOG("");
  DEBUG_LOG("Setting scanner state to: "<<state);
  analogic::nss::BasicCommandResult result;
  try
  {
    if ((m_scannerAgent != NULL) && m_NSSLiveControllerconnected)
    {
      switch (state)
      {
      case QMLEnums::IDLE:

        break;
      case QMLEnums::STANDBY:
        result = m_scannerAgent->SetToStandby();
        break;
      case QMLEnums::SCAN:
        result = m_scannerAgent->SetToScan();
        break;
      }
    }
    else
    {
      ERROR_LOG("Scanner Agent is not created");
    }
  }
  catch(...)
  {
    ERROR_LOG("Set Scanner state failed");
  }
  return result;
}

/*!
* @fn       StatusChanged
* @param    analogic::nss::ScannerOperationAgent* agent
* @return   None
* @brief    Method from analogic::nss::ScannerOperationAgentListener.
*/
void NSSAgentAccessInterface::StatusChanged(nss::ScannerOperationAgent *agent) {
  TRACE_LOG("");

  m_last_status_event_time_++;
  ScannerAgentStatusHandler status;

  if(( NULL != agent) && (agent->is_valid()) && m_NSSLiveControllerconnected)
  {

    status.setscannerstatus(*agent->get_status());
  }
  emit scannerFaultReceived(status);
}

/*!
* @fn       createOsrAgentListener
* @param    None
* @return   None
* @brief    creates analogic::nss::OsrAgentListener.
*/
void NSSAgentAccessInterface::createOsrAgentListener()
{
  TRACE_LOG("");
  m_agentListener = new OSRAgentListenerInterface();
#ifdef WORKSTATION
  //Connect signals from OSR/ SEARCH AgentListener
  connect(m_agentListener, &AgentListener::requestBagDecision, this,
          &NSSAgentAccessInterface::requestBagDecision,
          Qt::QueuedConnection);

  connect(m_agentListener, &AgentListener::requestBagPickupsig, this,
          &NSSAgentAccessInterface::onRequestBagPickup,
          Qt::DirectConnection);

  connect(this, &NSSAgentAccessInterface::notifyBagIsPickedUpsig, m_agentListener,
          &AgentListener::onNotifyBagIsPickedUp,
          Qt::DirectConnection);

  DEBUG_LOG("Create ArchiveAgentListenerInterface");
  m_archiveAgentListener = new ArchiveAgentListenerInterface();
  if (m_archiveAgentListener != NULL)
  {
    connect(m_archiveAgentListener, &ArchiveAgentListenerInterface::requestBagDecision,
            this, &NSSAgentAccessInterface::requestBagDecision,
            Qt::QueuedConnection);

    connect(m_agentListener, &SearchAgentListenerInterface::eventReceivedsig, this,
            &NSSAgentAccessInterface::onEventReceived,
            Qt::QueuedConnection);
  }
#endif
}
/*!
* @fn       getAgentListener
* @param    AgentListener*
* @return   None
* @brief    gets OSRAgentListenerInterface
*/
AgentListener* NSSAgentAccessInterface::getAgentListener()
{
  return m_agentListener;
}

/*!
* @fn       createSearchAgentListener
* @param    None
* @return   None
* @brief    creates analogic::nss::SearchAgentListener.
*/
void NSSAgentAccessInterface::createSearchAgentListener()
{
  m_agentListener = new SearchAgentListenerInterface();
  if (m_agentListener != NULL)
  {
    //Connect signals from OSR/ SEARCH AgentListener
    connect(m_agentListener, &AgentListener::requestBagDecision, this,
            &NSSAgentAccessInterface::requestBagDecision,
            Qt::QueuedConnection);
    connect(dynamic_cast<SearchAgentListenerInterface*>(m_agentListener),
            &SearchAgentListenerInterface::searchListChanged, this,
            &NSSAgentAccessInterface::searchListUpdated, Qt::QueuedConnection);
    connect(dynamic_cast<SearchAgentListenerInterface*>(m_agentListener),
            &SearchAgentListenerInterface::eventReceivedsig, this,
            &NSSAgentAccessInterface::onEventReceived,
            Qt::QueuedConnection);
  }

}

/*!
* @fn       onEventReceived
* @param    analogic::nss::NssEvent event_code
* @param    const std::vector<std::string>&
* @return   None
* @brief    Event received from nss
*/
void NSSAgentAccessInterface::onEventReceived(analogic::nss::NssEvent event_code, const std::vector<std::string>& data)
{
  INFO_LOG("NSSAgentAccessInterface onEventReceived : "<<event_code);
  if (event_code == analogic::nss::NssEvent::NSS_EVENT_STOP_THE_CHECK)
  {
    INFO_LOG("Received NssEvent NSS_EVENT_STOP_THE_CHECK with : "<< data.size());
    QString bagid;
    QString location;
    QString rfid;

    if(data.size() == 3)
    {
      bagid = data[0].c_str();
      location = data[1].c_str();
      rfid = data[2].c_str();
      rfid.remove(QRegExp("^0+"));
      INFO_LOG("Received NssEvent : "<< bagid.toStdString() <<"  "<<location.toStdString()<<"  "<<rfid.toStdString());
    }
    else{
      WARN_LOG("Received NssEvent NSS_EVENT_STOP_THE_CHECK wihtout proper data : " <<  data.size());
      for (unsigned long i=0 ; i < data.size() ; i++)
        WARN_LOG("Improper data received as : "<<data[i]);
    }
    emit displayMsg(Errors::FILE_OPERATION_SUCCESS,
                    WorkstationConfig::getInstance()->getHighThreatUpperText()
                    + QString(" ")
                    + WorkstationConfig::getInstance()->getHighThreatLowerText()
                    + tr(" bag: ") + bagid +tr(" RFID: ") + rfid + tr(" Located at: ") + location + tr(" on the decision conveyor has been suspected by the PVS operator.") ,
                    QMLEnums::QML_MESSAGE_MODEL);
  }
  else if (event_code == analogic::nss::NssEvent::NSS_EVENT_SERVER_SYNC_IO_ERROR)
  {
    INFO_LOG("Received NssEvent NSS Server Down : "<<event_code);
    emit displayMsg(Errors::FILE_OPERATION_SUCCESS,
                    tr("Received NSS Event : NSS Server Down") ,
                    QMLEnums::QML_MESSAGE_MODEL);
  }
}

/*!
* @fn       onRequestBagPickup
* @param    std::string id_assigning_authority
* @param    const std::string primary bag  ID
* @param    const std::string rfid
* @param    const QString pickup_type
* @param    analogic::nss::BhsDiverterDecision
* @param    const std::string diverter decision string
* @return   None
* @brief    Slot called when bag is ready for picked up.
*/
void NSSAgentAccessInterface::onRequestBagPickup(const std::string& id_assigning_authority, const string &primary_bag_id, const string &rfid,
                                                 const std::string& pickup_type, BhsDiverterDecision diverter_decision_code,
                                                 const string &diverter_decision_string)
{
  QMLEnums::BhsDiverterDecision wsDiverterDecisionCode = mapBhsDivertedDecisionToWS(diverter_decision_code);
  INFO_LOG("onRequestBagPickup : | "<<id_assigning_authority << " | " \
           <<primary_bag_id << " | " \
           <<rfid << " | " \
           <<pickup_type << " | " \
           <<diverter_decision_code << " | " \
           <<diverter_decision_string << " | " );

  emit requestBagPickupsig(id_assigning_authority, primary_bag_id, rfid, pickup_type, wsDiverterDecisionCode,
                           diverter_decision_string);
}

/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    notifies about exit.
*/
void NSSAgentAccessInterface::onExit()
{
  TRACE_LOG("");
  onDeAuthenticate();
  onDisconnect();
  destroyInstance();
}

/*!
* @fn       searchListUpdated
* @param    None
* @return   None
* @brief    notifies nssHandler when searchList updated.
*/
void NSSAgentAccessInterface::searchListUpdated()
{

  if ((m_search_agent != NULL) && m_NSSLiveControllerconnected)
  {
    std::vector<analogic::nss::SearchListEntry> vec =
        m_search_agent->GetSearchList();
    DEBUG_LOG("Search list size"<<vec.size());

    QList<BagInfo>  m_bagInfoList;
    foreach (analogic::nss::SearchListEntry var, vec) {
      BagInfo temp;
      temp.nss_bag_id = var.nss_bag_id;
      temp.displayable_bag_id = var.displayable_bag_id;
      temp.machine_alarm_decision = var.machine_alarm_decision;
      temp.user_id = var.user_id;
      temp.user_alarm_decision = var.user_alarm_decision;
      temp.displayable_current_search_station_id =
          var.displayable_current_search_station_id;
      temp.has_obvious_threat = var.has_obvious_threat;
      temp.thumbnail_url = var.thumbnail_url;
      temp.machine_id = var.machine_id;
      temp.bag_timestamp = var.bag_timestamp;
      temp.reason_for_search = var.reason_for_search;
      QDateTime datetime;
      datetime.setMSecsSinceEpoch(var.bag_timestamp);
      temp.m_timeStampStr = datetime.toString("yyyy-MM-dd hh:mm:ss");
      temp.rfid = var.rfid;
      temp.m_location = QString::fromStdString(var.location);
      m_bagInfoList.append(temp);
      INFO_LOG("nss_bag_id:"<<temp.nss_bag_id<<" displayable_bag_id:"<<temp.displayable_bag_id
               <<" Machine_id: "<<var.machine_id << "rfid: "<<temp.rfid<<"temp.m_location::"<<temp.m_location.toStdString()<<"temp.reason_for_search"<<var.reason_for_search);
    }
    QVariant var;
    var.setValue(m_bagInfoList);
    emit onBagListUpdated(QMLEnums::ArchiveFilterSrchLoc::SEARCH_SERVER,
                          QMLEnums::BagsListAndBagDataCommands::BLBDC_SHOW_BAGS_LIST, var);
  }
}

/*!
* @fn       getBagDataFromServer
* @param    QMLEnums::ArchiveFilterSrchLoc
* @param    std::string nss_bag_id
* @param    QMLEnums::BagsListAndBagDataCommands mode
* @return   None
* @brief    requests bag from server through agent and created a BagData object.
*/
void NSSAgentAccessInterface::getBagDataFromServer(
    QMLEnums::ArchiveFilterSrchLoc srchLoc, std::string nss_bag_id,QMLEnums::BagsListAndBagDataCommands mode)
{
  QString erroMsg;
  int hr = Errors::E_FAIL;
  bool toBeChanged = false;
  try
  {
    INFO_LOG("Get BagData from server. Location:" << srchLoc << ", BagID:" << nss_bag_id);
    if (srchLoc == QMLEnums::ArchiveFilterSrchLoc::SEARCH_SERVER)
    {
      if ((m_search_agent != NULL) && m_NSSLiveControllerconnected)
      {
        std::vector<analogic::nss::SearchListEntry> vec =
            m_search_agent->GetSearchList();

        unsigned int i = 0;
        for (; i < vec.size(); ++i) {
          if (vec.at(i).nss_bag_id == nss_bag_id)
          {
            // Note: disabeled due to issue with NSS server - B. Davidson 10-9-2017

            // if (vec.at(i).displayable_current_search_station_id == "") {
            // Requested Bag is free
            analogic::nss::SearchBag* searchBag = m_search_agent->
                RequestSearchBagByNssBagId(nss_bag_id);

            if (searchBag != NULL) {

              //-----------------------------
              // ARO-ADDED
              int count = searchBag->get_extended_property_count();
              for (int c = 0; c < count; c++)
              {
                const analogic::nss::Property& extProp  = searchBag->get_extended_property(c);
                if (extProp.key.compare("reason_for_search") == 0)
                {
                  DEBUG_LOG("Reason for Search:" << extProp.value);
                  vec.at(i).reason_for_search = extProp.value;
                }
              }
              //-----------------------------
              hr = Errors::S_OK;
              emit requestBagDecision(searchBag, QMLEnums::ArchiveFilterSrchLoc::SEARCH_SERVER,vec.at(i).reason_for_search,mode);
              toBeChanged = true;
            }
            else {
              erroMsg = tr("No response to start request for bag: ") + QString::fromStdString(nss_bag_id);
            }

            // }
            // else {
            // erroMsg = "Requested bag: "+QString::fromStdString(nss_bag_id)+" is already opened on another search station.";
            // }
            break;
          }

        }
        if (i == vec.size()) {
          erroMsg = tr("Selected BagId is not in current search list range");
        }
      }
      else
      {
        erroMsg = tr("Agent is not connected");
      }
    }
    else if(srchLoc == QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER)
    {

      analogic::nss::RecallClient* recall_client = NULL;
      if ((m_wsType == QMLEnums::OSR_WORKSTATION ) && (m_osr_agent != NULL) )
      {
        recall_client = m_osr_agent->GetRecallClient();
      }
      else if ((m_wsType == QMLEnums::SEARCH_WORKSTATION) && (m_search_agent != NULL) )
      {
        recall_client = m_search_agent->GetRecallClient();
      }
      else if (( m_wsType == QMLEnums::SUPERVISOR_WORKSTATION) && (m_supervisor_agent != nullptr) )
      {
        recall_client = m_supervisor_agent->GetRecallClient();
      }
      else
      {
        return;
      }
      if(recall_client != NULL)
      {
        INFO_LOG("Requesting bag to recall Server"<<nss_bag_id);
        analogic::nss::RecallBag* bag = recall_client->RequestRecallBag(nss_bag_id);

        if (bag == NULL) {
          erroMsg = tr("Recall bag [") + QString::fromStdString(nss_bag_id) + tr ("] could not be found.");
          emit recallBagDownloadFailed(nss_bag_id);
        }
        else if (bag->get_server_transfer_option() == NULL) {
          erroMsg = tr("Recall bag [") + QString::fromStdString(nss_bag_id) +
              tr("] was found but is not available at this time.");
          emit recallBagDownloadFailed(nss_bag_id);
        }
        else {
          if (mode == QMLEnums::BLBDC_SHOW_BAG)
          {
            toBeChanged = true;
          }
          hr = Errors::S_OK;
          emit requestBagDecision(bag,QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER,"",mode);
        }

      }
    }
    else if (srchLoc == QMLEnums::ArchiveFilterSrchLoc::BHS_SERVER)
    {
      if ((m_search_agent != NULL) && m_NSSLiveControllerconnected)
      {
        INFO_LOG("Requesting bag to Search Server from BHS"<<nss_bag_id.c_str());
        analogic::nss::SearchBag* searchBag = m_search_agent->
            RequestSearchBagByIdPair(nss_bag_id, "bhs");
        if (searchBag != NULL) {
          DEBUG_LOG("Received requested bag"<<nss_bag_id.c_str());
          hr = Errors::S_OK;
          emit requestBagDecision(searchBag, QMLEnums::ArchiveFilterSrchLoc::BHS_SERVER, nss_bag_id,mode);
          toBeChanged = true;
        }
        else {
          erroMsg = tr("No response to start request for bag: ") + QString::fromStdString(nss_bag_id);
          ERROR_LOG(erroMsg.toStdString().c_str());
        }
      }
    }
  }
  catch(...)
  {
    toBeChanged=false;
    erroMsg = tr("Exception in recall_client. Recall bag [") + QString::fromStdString(nss_bag_id) + tr("] could not be found.");
    ERROR_LOG(erroMsg.toStdString().c_str());
  }
  DEBUG_LOG("Notify to cahnge screen to bag screen"<<toBeChanged);
  if(!toBeChanged)
  {
    // Send Message in qml of the screen toBeChanged
    emit notifyToChangeOrNotScreen(toBeChanged, erroMsg, hr);
  }
}
#ifdef WORKSTATION
/*!
* @fn       fetchRemoteArchiveList
* @param    QVariant searchFields
* @return   None
* @brief    fetches RemoteArchive list
*/
void NSSAgentAccessInterface::fetchRemoteArchiveList(QVariant searchFields)
{
  try
  {
    DEBUG_LOG("Requesting bag to recall Server");

    analogic::nss::RecallClient* recall_client = NULL;
    if ((m_wsType == QMLEnums::OSR_WORKSTATION ) && (m_osr_agent != NULL) )
    {
      recall_client = m_osr_agent->GetRecallClient();
    }
    else if ((m_wsType == QMLEnums::SEARCH_WORKSTATION) && (m_search_agent != NULL) )
    {
      recall_client = m_search_agent->GetRecallClient();
    }
    else if ((m_wsType == QMLEnums::SUPERVISOR_WORKSTATION) && (m_supervisor_agent != nullptr) )
    {
      recall_client = m_supervisor_agent->GetRecallClient();
    }
    else
    {
      return;
    }

    if (recall_client != NULL)
    {
      QString filterparam = searchFields.toString();
      QJsonParseError jsonError;
      QJsonDocument jsondoc = QJsonDocument::fromJson(filterparam.toUtf8(), &jsonError);
      if (jsonError.error == QJsonParseError::NoError)
      {
        QJsonObject jobj =  jsondoc.object();
        QVariantMap mapobj =  jobj.toVariantMap();
        analogic::nss::RecallQuery query;
        query.SetStartDate(mapobj["startDate"].toDateTime().toMSecsSinceEpoch()/1000);
        query.SetEndDate(mapobj["endDate"].toDateTime().toMSecsSinceEpoch()/1000);

        if (mapobj["machineidBoxIndex"].toInt() == 0)
        {
          query.ClearMachineId();
        }
        else
        {
          query.SetMachineId(mapobj["machineidBoxText"].toString().toStdString());
        }

        if(mapobj["machineDescBoxIndex"].toInt() != 0)
        {
          query.SetMachineDecision(mapAlarmDescToDicosValue(true,
                                                            static_cast<QMLEnums::ArchiveFilterMachineDecision>
                                                            (mapobj["machineDescBoxIndex"].toInt())));
        }

        query.SetBagId(mapobj["bagidentered"].toString().toStdString());
        if (mapobj["useridBoxIndex"].toInt() == 0)
        {
          query.ClearUserId();
        }
        else
        {
          query.SetUserId(mapobj["useridBoxText"].toString().toStdString());
        }

        if(mapobj["userDecBoxIndex"].toInt() != 0)
        {
          query.SetUserDecision(mapAlarmDescToDicosValue(false,
                                                         static_cast<QMLEnums::ArchiveFilterUserDecision>(
                                                           mapobj["userDecBoxIndex"].toInt())));
        }


        std::string queryLogMsg = "Sending Bag query " +
            std::to_string(query.GetStartDate())  + ", " +
            std::to_string(query.GetEndDate())    + ", " +
            query.GetMachineId()                  + ", " +
            std::to_string(query.GetMachineDecision()) + ", " +
            query.GetBagId()                      + ", " +
            query.GetUserId()                     + ", " +
            std::to_string(query.GetUserDecision());
        INFO_LOG(queryLogMsg);

        /*
                      INFO_LOG("Sending Bag query"<<query.start_date<<","
                      <<query.end_date<<","
                      <<query.machine_id<<","
                      <<query.GetMachineDecision()<<","
                      <<query.GetBagId()<<","
                      <<query.GetUserId()<<","
                      <<query.GetUserDecision()
                      );
            */

        std::vector<analogic::nss::RecallBagListEntry> query_output;
        recall_client->Query(query, &query_output);

        QList<BagInfo> bagInfoList;
        foreach (analogic::nss::RecallBagListEntry var, query_output) {
          BagInfo temp;
          temp.nss_bag_id = var.nss_bag_id;
          temp.displayable_bag_id = var.displayable_bag_id;
          qint64 mSecs = var.bag_timestamp;
          QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(mSecs);
          temp.m_timeStampStr = dateTime.toString("yyyy/MM/dd - HH:mm:ss");
          temp.machine_alarm_decision = var.machine_alarm_decision;
          temp.machine_abort_reason = var.machine_abort_reason;
          temp.machine_abort_flag = var.machine_abort_flag;

          temp.user_alarm_decision = var.operator_alarm_decision;
          temp.user_abort_reason = var.operator_abort_reason;
          temp.user_abort_flag = var.operator_abort_flag;

          temp.thumbnail_url = var.thumbnail_url;
          temp.bag_timestamp = mSecs;
          temp.machine_id = var.machine_id;

          temp.rfid = var.rfid;
          temp.m_diverterReason = QString::fromStdString(var.diverter_decision_string);

          temp.m_location = QString::fromStdString("SVS");
          //TODO: Append location field as recived from server side.

          //temp.m_bagType = var.bag_type;

          if(var.bag_type == analogic::nss::BAG_TYPE_SCANNER_LIVE)
            temp.m_bagType = tr("LIVE");
          else if(var.bag_type == analogic::nss::BAG_TYPE_TIP)
            temp.m_bagType = tr("TIP");
          else
            temp.m_bagType = tr("UNKNOWN"); //for unknown bag type-> analogic::nss::BAG_TYPE_UNKNOWN

          bagInfoList.append(temp);
        }
        QVariant var;
        var.setValue(bagInfoList);
        if (query_output.size() == 0)
        {
          INFO_LOG("Zero Bag Received from archive.");
          emit notifyToChangeOrNotScreen(false, tr("Zero Bag Received. "), Errors::E_FAIL);
        }
        emit onBagListUpdated(QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER,
                              QMLEnums::BagsListAndBagDataCommands::BLBDC_SHOW_BAGS_LIST, var);

      }
      else
      {
        ERROR_LOG("Recieved searchCriteria for RemoteArchive command is incorrect.Error:" << jsonError.errorString().toStdString().c_str());
        return;
      }
    }
    else {
      ERROR_LOG("OSR Agent is null.");
    }
  }
  catch(...)
  {
    ERROR_LOG("Exception in fetching recall archive data.");
  }
}
#endif

/*!
* @fn       createWSAgent
* @param    analogic::nss::SearchAgentStaticProperties static_properties
* @return   Agent creation status (S_OK/E_POINTER - success/failure).
* @brief    Creates Search agent.
*/
int NSSAgentAccessInterface::createWSAgent(
    analogic::nss::SearchAgentStaticProperties static_properties)
{
  TRACE_LOG("");

  try
  {
    destroyWSAgent();
    if((NULL != m_connection_agent)
       && (m_search_agent == NULL) && m_NSSLiveControllerconnected)
    {
      DEBUG_LOG("Sending request to create Search Agent...");
      // Create Search agent and set credentials
      m_search_agent = m_connection_agent->CreateSearchAgent(
            &static_properties, (dynamic_cast<analogic::nss::
                                 SearchAgentListener*>(m_agentListener)),
            &m_registration_result);
      if (m_search_agent == NULL) {
        ERROR_LOG (" Failed to create Search Agent with registration"
                   " result: "<< m_registration_result.result);
        return Errors::E_POINTER;
      }
      DEBUG_LOG("OSR Agent created");
      return Errors::S_OK;
    }
    else if((m_connection_agent != NULL) &&
            m_search_agent)
    {
      DEBUG_LOG("SEARCH Agent is already created");
      return Errors::S_OK;
    }
    else
    {
      DEBUG_LOG("SEARCH agent creation fail");
      return Errors::E_FAIL;
    }
  }
  catch(...)
  {
    ERROR_LOG("SEARCH agent creation fail");
    return Errors::E_FAIL;
  }
}

/*!
* @fn       setCredentialsOfWSAgent
* @param    None
* @return   None
* @brief    set Credentials of Osr/Search agent.
*/
void NSSAgentAccessInterface::setCredentialsOfWSAgent()
{
  try
  {
    if ((NULL != m_osr_agent) && (m_wsType ==
                                  QMLEnums::WSType::OSR_WORKSTATION) &&
        m_NSSLiveControllerconnected)
    {
      INFO_LOG("Set credential of Osr Agent");
      m_osr_agent->SetCredential(m_auth_result.credential.credential_id);
    }
    else if ((NULL != m_search_agent) &&
             (m_wsType  == QMLEnums::WSType::SEARCH_WORKSTATION)
             && m_NSSLiveControllerconnected)
    {
      INFO_LOG("Set credential of Search Agent");
      m_search_agent->SetCredential(m_auth_result.credential.credential_id);
    }
    else if ((nullptr != m_supervisor_agent) &&
             (m_wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION)
             && m_NSSLiveControllerconnected)
    {
      INFO_LOG("Set credential of Supervisor Agent");
      m_supervisor_agent->SetCredential(m_auth_result.credential.credential_id);
    }
    else
    {
      ERROR_LOG("Invalid Workstation Type: setCredential of WS fail");
    }
  }
  catch(...)
  {
    ERROR_LOG("set WS agent credential fail");
  }
}

/*!
* @fn       CreateScannerOperationAgent
* @param    const std::string&  scannername
* @return   Agent creation status (S_OK/E_POINTER - success/failure).
* @brief    Creates Scanner Operation agent.
*/
int NSSAgentAccessInterface::CreateScannerOperationAgent(const std::string& scannername)
{
  TRACE_LOG("");
  try
  {
    destroyScannerAgent();
    if ((NULL == m_scannerAgent) && m_NSSLiveControllerconnected)
    {
      std::list<analogic::nss::DiscoveredScanner> scanner_list;

      if ((NULL != m_osr_agent) &&
          (m_wsType == QMLEnums::WSType::OSR_WORKSTATION))
      {
        m_osr_agent->GetRegisteredScanners(&scanner_list);
      }
      else if ((NULL != m_search_agent) &&
               (m_wsType == QMLEnums::WSType::SEARCH_WORKSTATION))
      {
        m_search_agent->GetRegisteredScanners(&scanner_list);
      }
      else if ((nullptr != m_supervisor_agent) &&
               (m_wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION))
      {
        //GetRegisteredScanners not available in SupervisorAgents
        //m_supervisor_agent->GetRegisteredScanners(&scanner_list)
      }
      else
      {
        DEBUG_LOG("Invalid Workstation Type: Scanner Operation agent"
                  " creation failed");
        return Errors::E_FAIL;
      }

      DEBUG_LOG("ScannerOperator::Start: visible scanner list size: " <<
                scanner_list.size());
      analogic::nss::DiscoveredScanner* discovered_scanner = NULL;
      for (auto iter = scanner_list.begin(); iter != scanner_list.end();
           ++iter) {
        analogic::nss::DiscoveredScanner& scanner = *iter;
        std::string a_name = scanner.get_general_equipment_module().GetMachineID().Get();
        DEBUG_LOG("Scanner found: " << a_name);
        if (scannername == a_name) {
          discovered_scanner = &scanner;
          break;
        }
      }

      if (discovered_scanner == NULL) {
        DEBUG_LOG("Did not find scanner [" << scannername << "]" <<
                  std::endl);
        return Errors::E_FAIL;
      }

      m_scannerAgent = discovered_scanner->
          CreateScannerOperationAgent(this);
      INFO_LOG("Created Scanner Operation Agent");
      return Errors::S_OK;
    }
    else
    {
      DEBUG_LOG("Scanner Operation agent creation failed");
      return Errors::E_FAIL;
    }
  }
  catch(...)
  {
    ERROR_LOG("Scanner Operation agent creation failed");
    return Errors::E_FAIL;
  }
  return Errors::S_OK;
}

/*!
* @fn       onSendBagRspToBHS();
* @param    const std::string& bhs_bag_id
* @param    BagStatusUpdate decision
* @return   void
* @brief    call on send Bag Rsp to BHS
*/
void NSSAgentAccessInterface::onSendBagRspToBHS(const std::string& bhs_bag_id, analogic::ancp00::BagStatusUpdate::BAG_DISPOSITION decision)
{
  if (m_BHSBagInterface)
  {
    INFO_LOG("Send Bag decision to BHS"<<bhs_bag_id.c_str() <<"decision:"<<decision);
    analogic::ancp00::ExternalBhsBagInterface::TypeDecisionMap decision_map
        = { { analogic::ancp00::ExternalBhsBagInterface::SWS, decision} };
    bool res = m_BHSBagInterface->SetBagDecisions(bhs_bag_id, decision_map);
    INFO_LOG("Send Bag decision to BHS"<<bhs_bag_id.c_str() <<"Result:"<<res);
  }
  else
  {
    ERROR_LOG("BHSBAGInterface is null.");
  }
}

/*!
* @fn       getNSSUserList
* @return   QStringList
* @brief    Gets list of users from nss services.
*/
QStringList NSSAgentAccessInterface::getNSSUserList()
{
  std::vector<analogic::nss::PublicUserInfo> nssUserVector;
  QStringList nssUserList;
  if ((m_wsType == QMLEnums::WSType::OSR_WORKSTATION) && (NULL != m_osr_agent))
  {
    m_osr_agent->get_registration()->GetUserList(&nssUserVector);
    foreach(PublicUserInfo user, nssUserVector)
    {
      nssUserList << QString::fromStdString(user.username);
    }
  }
  else if((m_wsType == QMLEnums::WSType::SEARCH_WORKSTATION) && (NULL != m_search_agent))
  {
    m_search_agent->get_registration()->GetUserList(&nssUserVector);
    foreach(PublicUserInfo user, nssUserVector)
    {
      nssUserList << QString::fromStdString(user.username);
    }
  }
  else if((m_wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION) && (nullptr != m_supervisor_agent))
  {
    m_supervisor_agent->get_registration()->GetUserList(&nssUserVector);
    foreach(PublicUserInfo user, nssUserVector)
    {
      nssUserList << QString::fromStdString(user.username);
    }
  }
  return nssUserList;
}

/*!
* @fn       getNSSAgentList
* @return   QStringList
* @brief    Gets list of nss agents from nss services.
*/
QStringList NSSAgentAccessInterface::getNSSAgentList()
{
  std::vector<std::string> agentVector;
  QStringList nssAgentList;
  if ((m_wsType == QMLEnums::WSType::OSR_WORKSTATION) && (NULL != m_osr_agent))
  {
    m_osr_agent->get_registration()->GetAgentList(analogic::nss::AgentType::AGENT_TYPE_SCANNER, &agentVector);
    foreach(std::string agent_name, agentVector)
    {
      nssAgentList << QString::fromStdString(agent_name);
    }
  }
  else if((m_wsType == QMLEnums::WSType::SEARCH_WORKSTATION) && (NULL != m_search_agent))
  {
    m_search_agent->get_registration()->GetAgentList(analogic::nss::AgentType::AGENT_TYPE_SCANNER, &agentVector);
    foreach(std::string agent_name, agentVector)
    {
      nssAgentList << QString::fromStdString(agent_name);
    }
  }
  else if((m_wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION) && (nullptr != m_supervisor_agent))
  {
    m_supervisor_agent->get_registration()->GetAgentList(analogic::nss::AgentType::AGENT_TYPE_SCANNER, &agentVector);
    foreach(std::string agent_name, agentVector)
    {
      nssAgentList << QString::fromStdString(agent_name);
    }
  }
  return nssAgentList;
}



#ifdef WORKSTATION
/*!
    * @fn       onSetObviousThreatFlagtoBHS
    * @param    const std::string& bhs_bag_id
    * @param    bool flag
    * @return   void
    * @brief    call on send obvious threat decision to BHS
    */
void NSSAgentAccessInterface::onSetObviousThreatFlagtoBHS(const std::string& bhs_bag_id, bool flag)
{
  if(bhs_bag_id.empty())
  {
    INFO_LOG("Can not send Stop-the-Check to BHS as Bag id is null"<<flag);
    return;
  }

  if (m_wsType == QMLEnums::SEARCH_WORKSTATION  || m_wsType == QMLEnums::SUPERVISOR_WORKSTATION)
  {
    if (m_BHSBagInterface)
    {
      INFO_LOG("SetObviousThreatFlag"<<bhs_bag_id.c_str() <<"falg:"<<flag);
      bool res = m_BHSBagInterface->SetObviousThreatFlag(bhs_bag_id, flag);
      INFO_LOG("SetObviousThreatFlag"<<bhs_bag_id.c_str() <<"Result:"<<res);
    }
    else
    {
      ERROR_LOG("BHSBAGInterface is null.");
    }
  }
  else
  {
    ERROR_LOG("Can not send Stop-the-Check to BHS as workstation type is not search.");
  }


}
/*!
    * @fn       isUserLoggedIn
    * @param    None
    * @return   bool
    * @brief    gets if user is logged in or not
    */
bool NSSAgentAccessInterface::isUserLoggedIn()
{
  if (m_auth_result.result_type == analogic::nss::AuthenticationResult::SUCCESS)
  {
    return true;
  }
  else
  {
    return false;
  }

}

/*!
    * @fn       onNotifyBagIsPickedUp
    * @param    QMLEnums::WSBasicCommandResult
    * @return   None
    * @brief    slot called when bhs diverter decision is send.
    */
void NSSAgentAccessInterface::onNotifyBagIsPickedUp(QMLEnums::WSBasicCommandResult commandResult)
{
  analogic::nss::BasicCommandResult nssCommandResult = mapWsBasicCommandResultToNSS(commandResult);
  emit notifyBagIsPickedUpsig(nssCommandResult);
}

/*!
    * @fn       createMockConnection
    * @param    analogic::nss::ConnectionAgent*
    * @param    const std::string address
    * @return   int  (S_OK/E_POINTER) connection success/failure.
    * @brief    Creates Mock connection agent and connects to nss.
    * @note     This function is created to handle mocking required in testsuite_nssagentaccessinterface
    */
int NSSAgentAccessInterface::createMockConnection(analogic::nss::ConnectionAgent *MockConnectionAgent, const std::string& nss_address)
{
  TRACE_LOG("");
  try{
    if(!nss_address.empty())
    {
      m_nssaddress = nss_address.c_str();
    }
    if((m_connection_agent !=NULL))
    {
      DEBUG_LOG("Destroy Existing Mock Connection Agent.");
      destroyScannerAgent();
      destroyAuthenticationAgent();
      destroyWSAgent();
      destroyConnection();
    }
    if (NULL == m_connection_agent)
    {
      // Create and intialize Mock connection agent to communicate with nss library
      m_connection_agent = MockConnectionAgent;
      if (NULL == m_connection_agent)
      {
        ERROR_LOG("Failed Mock Connection agent creation");
        return Errors::E_POINTER;
      }
      INFO_LOG("Mock Connection agent created");
    }
    else
    {
      DEBUG_LOG("Mock Connection agent is already created");
    }
  }catch(...)
  {
    ERROR_LOG("NSS create Mock connection agent failed");
    return Errors::E_FAIL;
  }
  try{
    if((m_connection_agent != NULL))
    {
      bool created = m_connection_agent->Connect(m_nssaddress.toStdString(),
                                                 "{transport: ssl, username: agent, password: a1cUk8b*gent, sasl-mechanism: PLAIN}", NULL);
      if(created)
      {
        INFO_LOG("Connected to Mock network");
        m_NSSLiveControllerconnected = true;
        return Errors::S_OK;
      }
      else
      {
        ERROR_LOG("Mock Network Connection failed");
        return Errors::E_FAIL;
      }
    }
    else if((m_connection_agent != NULL))
    {
      INFO_LOG("Already Connected to Mock network");
      return Errors::S_OK;
    }
  }catch(...)
  {
    ERROR_LOG("Mock Network Connection failed");
    return Errors::E_FAIL;
  }
  return Errors::S_OK;
}

/*!
    * @fn       getPasswordExpirationDays
    * @param    None
    * @return   int
    * @brief    returns number of days remaning for password to expire.
    */
int NSSAgentAccessInterface::getPasswordExpirationDays() const
{
  return m_passwordExpirationDays;
}

/*!
    * @fn       SetMaintenanceMode
    * @param    bool
    * @return   bool
    * @brief    enable/disable scanner maintenance mode.
    */
bool NSSAgentAccessInterface::SetMaintenanceMode(bool enabled)
{
  bool result = false;
  if(m_scannerAgent)
  {
    if(m_scannerAgent->SetMaintenanceMode(enabled) == BasicCommandResult::BASIC_COMMAND_RESULT_SUCCESS)
    {
      INFO_LOG("Setting scanner maintenance mode: " << enabled);
      result = true;
    }
  }
  return result;
}

/*!
    * @fn       reportEventToNssAgent
    * @param    NssEventInfo nssEventInfo
    * @return   bool
    * @brief    Report Event to NSS VM.
    */
bool NSSAgentAccessInterface::reportEventToNssAgent(NssEventInfo nssEventInfo)
{
  BasicCommandResult result = BasicCommandResult::BASIC_COMMAND_RESULT_UNDEFINED;
  NssEventInfo defaultNssEventInfo = m_nssEventData->getEventInfoFromEnum(nssEventInfo.eventEnum());
  if(!nssEventInfo.eventDescription().isEmpty())
  {
    defaultNssEventInfo.setEventDescription(nssEventInfo.eventDescription());
  }
  if((m_wsType == QMLEnums::OSR_WORKSTATION) && (m_osr_agent != nullptr))
  {
    result = m_osr_agent->ReportAgentEvent(defaultNssEventInfo.eventCode().toStdString(),
                                           AGENT_EVENT_LEVEL_INFO,
                                           QDateTime::currentMSecsSinceEpoch(),
                                           defaultNssEventInfo.eventDescription().toStdString());
  }
  else if((m_wsType == QMLEnums::SEARCH_WORKSTATION || m_wsType == QMLEnums::SUPERVISOR_WORKSTATION) && (m_search_agent != nullptr))
  {
    result = m_search_agent->ReportAgentEvent(defaultNssEventInfo.eventCode().toStdString(),
                                              AGENT_EVENT_LEVEL_INFO,
                                              QDateTime::currentMSecsSinceEpoch(),
                                              defaultNssEventInfo.eventDescription().toStdString());
  }
  if(result == BasicCommandResult::BASIC_COMMAND_RESULT_SUCCESS)
  {
    INFO_LOG("Reported Event: " << defaultNssEventInfo.eventCode().toStdString()
             << " ,Event Description: " << defaultNssEventInfo.eventDescription().toStdString() << " successfully.");
    return true;
  }
  else
  {
    INFO_LOG("Failed to report event " << defaultNssEventInfo.eventCode().toStdString());
    return false;
  }
}

/*!
* @fn       createWSAgent
* @param    analogic::nss::SupervisorAgentStaticProperties static_properties
* @return   Agent creation status (S_OK/E_POINTER - success/failure).
* @brief    Creates Search agent.
*/
int NSSAgentAccessInterface::createWSAgent(
    analogic::nss::SupervisorAgentStaticProperties static_properties)
{
  TRACE_LOG("");
  try
  {
    destroyWSAgent();
    if((NULL != m_connection_agent)
       && (m_supervisor_agent == nullptr) && (m_NSSLiveControllerconnected))
    {
      DEBUG_LOG("Sending request to create SUPERVISOR Agent...");
      // Create Search agent and set credentials
      m_supervisor_agent = m_connection_agent->CreateSupervisorAgent(
            &static_properties, (dynamic_cast<analogic::nss::
                                 SupervisorAgentListener*>(m_agentListener)),
            &m_registration_result);
      if(m_supervisor_agent == nullptr)
      {
        ERROR_LOG (" Failed to create Supervisor Agent with registration"
                   " result: "<< m_registration_result.result);
        return Errors::E_POINTER;
      }
      DEBUG_LOG("SUPERVISOR Agent created");
      return Errors::S_OK;
    }
    else if((m_connection_agent != NULL) && (m_supervisor_agent))
    {
      DEBUG_LOG("SUPERVISOR Agent is already created");
      return Errors::S_OK;
    }
    else
    {
      DEBUG_LOG("SUPERVISOR agent creation fail");
      return Errors::E_FAIL;
    }
  }
  catch(...)
  {
    ERROR_LOG("SUPERVISOR agent creation fail");
    return Errors::E_FAIL;
  }
}

/*!
* @fn       createSupervisorAgentListener
* @param    None
* @return   None
* @brief    creates analogic::nss::SupervisorAgentListener.
*/
void NSSAgentAccessInterface::createSupervisorAgentListener()
{
  INFO_LOG("Sending request to register with SupervisorAgentListener...")
  m_agentListener = new SupervisorAgentListenerInterface();
  if (m_agentListener != NULL)
  {
    //Connect signals from Supervisor AgentListener
    connect(m_agentListener, &AgentListener::requestBagDecision, this,
            &NSSAgentAccessInterface::requestBagDecision,
            Qt::QueuedConnection);
  }
}

/*!
    * @fn       getExpiredAuthMethod
    * @param    None
    * @return   QMLEnums::AuthMethodType
    * @brief    get Expired Auth Method Enum.
    */
QMLEnums::AuthMethodType NSSAgentAccessInterface::getExpiredAuthMethod() const
{
  return m_expiredAuthMethod;
}
#endif

#ifdef RERUN
/*!
    * @fn       onNotifyBagIsPickedUp
    * @param    QMLEnums::WSBasicCommandResult
    * @return   None
    * @brief    slot called when bhs diverter decision is send.
    */
void NSSAgentAccessInterface::onNotifyBagIsPickedUp(QMLEnums::WSBasicCommandResult commandResult)
{
  analogic::nss::BasicCommandResult nssCommandResult = mapWsBasicCommandResultToNSS(commandResult);
  emit notifyBagIsPickedUpsig(nssCommandResult);
}

/*!
    * @fn       onSetObviousThreatFlagtoBHS
    * @param    const std::string& bhs_bag_id
    * @param    bool flag
    * @return   void
    * @brief    call on send obvious threat decision to BHS
    */
void NSSAgentAccessInterface::onSetObviousThreatFlagtoBHS(const std::string& bhs_bag_id, bool flag)
{
  if(bhs_bag_id.empty())
  {
    INFO_LOG("Can not send Stop-the-Check to BHS as Bag id is null"<<flag);
    return;
  }

  if (m_wsType == QMLEnums::SEARCH_WORKSTATION || m_wsType == QMLEnums::SUPERVISOR_WORKSTATION)
  {
    if (m_BHSBagInterface)
    {
      INFO_LOG("SetObviousThreatFlag"<<bhs_bag_id.c_str() <<"falg:"<<flag);
      bool res = m_BHSBagInterface->SetObviousThreatFlag(bhs_bag_id, flag);
      INFO_LOG("SetObviousThreatFlag"<<bhs_bag_id.c_str() <<"Result:"<<res);
    }
    else
    {
      ERROR_LOG("BHSBAGInterface is null.");
    }
  }
  else
  {
    ERROR_LOG("Can not send Stop-the-Check to BHS as workstation type is not search.");
  }


}
#endif
}  // end of namespace ws
}  // end of namespace analogic
