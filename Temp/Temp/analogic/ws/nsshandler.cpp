/*!
* @file     nsshandler.cpp
* @author   Agiliad
* @brief    This file contains functions related to NssHandler
*           which manages interface between workstation and Nss Access interface.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/nsshandler.h>


namespace analogic
{
namespace ws
{
/*!
* @fn       NssHandler();
* @param    None
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
NssHandler::NssHandler()
{
  TRACE_LOG("");
  // initialize nss access interface
  DEBUG_LOG("Creating instance of nss agent access interface");
  m_nssAccesslayer = NSSAgentAccessInterface::getInstance();
  THROW_IF_FAILED((m_nssAccesslayer == NULL ? Errors::E_OUTOFMEMORY : Errors::S_OK));
  // connect bag notification signal
#ifdef WORKSTATION
  connect(m_nssAccesslayer, &NSSAgentAccessInterface::requestBagDecision,
          this, &NssHandler::bagNotification,
          Qt::QueuedConnection);

  // connect bagList notification signal
  connect(m_nssAccesslayer, &NSSAgentAccessInterface::onBagListUpdated,
          this, &NssHandler::bagListUpdated, Qt::DirectConnection);
  // connect bagData request signal
  connect(this, &NssHandler::getBagDataFromServer, m_nssAccesslayer,
          &NSSAgentAccessInterface::getBagDataFromServer, Qt::DirectConnection);

  connect(m_nssAccesslayer, SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)),
          this, SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)),
          Qt::DirectConnection);

  connect(ScannerAccessInterface::getInstance(), &ScannerAccessInterface::notifyToChangeOrNotScreen,
          this, &NssHandler::notifyToChangeOrNotScreen,
          Qt::DirectConnection);
#endif
}

/*!
* @fn       NssHandler();
* @param    None
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
NssHandler::NssHandler(NSSAgentAccessInterface* nssAccess)
{
  TRACE_LOG("");
  m_nssAccesslayer = nssAccess;
}

/*!
* @fn       fetchRemoteArchiveList
* @param    QVariant searchFields
* @return   None
* @brief    fetches RemoteArchive list from nss
*/
void NssHandler::fetchRemoteArchiveList(QVariant searchFields)
{
  TRACE_LOG("");
#ifdef WORKSTATION
  m_nssAccesslayer->fetchRemoteArchiveList(searchFields);
#endif
}

/*!
* @fn       ~NssHandler
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
NssHandler::~NssHandler()
{
  TRACE_LOG("");
  NSSAgentAccessInterface::destroyInstance();
  m_nssAccesslayer = NULL;
}

/*!
* @fn       init
* @param    None
* @return   int (Errors::S_OK -Successful /Errors::E_FAIL -failure)
* @brief    Intializes nss and connection agent.
*/
int NssHandler::init()
{
  TRACE_LOG("");
  // 1. nss address from configuration file
  m_nssaddress = WorkstationConfig::getInstance()->getNssipaddress()+":"+WorkstationConfig::getInstance()->getNssportno();

  // 2.  Create Connection agent
  int ret = QMLEnums::NssAgentError::NSS_FAIL;
  DEBUG_LOG("Creating connection agent.");
  ret = createConnectionAgent();
  if ( ret != static_cast<int>(Errors::S_OK))
  {   ERROR_LOG("Failed to create connection agent.");
    return QMLEnums::NssAgentError::CONN_AGENT_ERROR;
  }
  // 3. Create authentication agent
  DEBUG_LOG("Creating authentication agent.")
  ret = createAuthenticationAgent();
  if ( ret != static_cast<int>(Errors::S_OK))
  {
    ERROR_LOG("Failed to create Authentication agent.")
        return QMLEnums::NssAgentError::AUTH_AGENT_ERROR;
  }
  DEBUG_LOG("Created connection agent and authentication agent.")
  return ret;
}

/*!
* @fn       createConnectionAgent
* @param    None
* @return   int (Errors::S_OK -Successful /Errors::E_FAIL -failure)
* @brief    Creates connection agent and connects to nss.
*/
int NssHandler::createConnectionAgent()
{
  // Pass parameters to create connection agent of nss
  DEBUG_LOG("Creating connection agent and connection with Nss.")
  int ret = m_nssAccesslayer->createConnection(m_nssaddress.toStdString());
  if (static_cast<int>(Errors::S_OK != ret ))
  {
    ERROR_LOG("Failed to create connection agent and connection with Nss.");
  }
  return ret;
}

/*!
* @fn       createAuthenticationAgent
* @param    None
* @return   int (Errors::S_OK -Successful /Errors::E_FAIL -failure)
* @brief    Creates Authentication agent to authenticate user on the connection.
*/
int NssHandler::createAuthenticationAgent()
{
  // create authetication agent
  DEBUG_LOG("Creating authentication agent.")
  int authenresult =
      m_nssAccesslayer->createAuthenticationAgent();
  if (Errors::S_OK != authenresult )
  {
    ERROR_LOG("Authentication Agent creation Failed.");
  }
  return authenresult;
}

/*!
* @fn       onDeAuthenticate
* @param    None
* @return   None
* @brief    Call for Deauthenticate user
*/
void NssHandler::onDeAuthenticate()
{
  TRACE_LOG("");
  INFO_LOG("Deauthenticate user");
  emit m_nssAccesslayer->deAuthenticate();
}

/*!
* @fn       authenticate
* @param    QString username
* @param    QString password
* @return   QMLEnums::WSAuthenticationResult authentication result
* @brief    Authenticate user using connection.
*/
QMLEnums::WSAuthenticationResult  NssHandler::authenticate(QString username,
                                                           const std::map<analogic::nss::AuthMethodType, std::string>& auth_methods)
{
  TRACE_LOG("");
  QMLEnums::WSAuthenticationResult res = QMLEnums::WSAuthenticationResult::IO_ERROR;
  analogic::nss::AuthenticationResult ret = m_nssAccesslayer->authenticate
      (username.toStdString(), auth_methods);
  analogic::ws::UILayoutManager::getUILayoutManagerInstance()->onuserLoginStatusChanged(ret);
  res = m_nssAccesslayer->mapNssAuthenticationResultToWS(ret);

  return res;
}

/*!
* @fn       createWSAgent
* @param    analogic::nss::OsrAgentStaticProperties properties
* @return   int
* @brief    Creates OSR Agent.
*/
int NssHandler::createWSAgent(analogic::nss::OsrAgentStaticProperties
                              properties)
{
  TRACE_LOG("");

  // create osr agent
  int ret = m_nssAccesslayer->createWSAgent(properties);
  return ret;
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
QMLEnums::WSBasicCommandResult NssHandler::changePassword(
    const std::string &username, const std::string &oldAuthMethodDetail, const std::string &newAuthMethodDetail, QMLEnums::AuthMethodType authMethodType)
{
  TRACE_LOG("");
  QMLEnums::WSBasicCommandResult ret = m_nssAccesslayer->changePassword
      (username, oldAuthMethodDetail, newAuthMethodDetail, authMethodType);
  return ret;
}

/*!
* @fn       createWSAgent
* @param    analogic::nss::SearchAgentStaticProperties properties
* @return   int
* @brief    Creates Search Agent.
*/
int NssHandler::createWSAgent( analogic::nss::SearchAgentStaticProperties
                               properties)
{
  TRACE_LOG("");

  // create search agent
  int ret = m_nssAccesslayer->createWSAgent(properties);
  return ret;
}

/*!
* @fn       createWSAgent
* @param    analogic::nss::SupervisorAgentStaticProperties properties
* @return   int
* @brief    Creates Supervisor Agent.
*/
int NssHandler::createWSAgent(analogic::nss::SupervisorAgentStaticProperties
                              properties)
{
  TRACE_LOG("");
  // create supervisor agent
  int ret = m_nssAccesslayer->createWSAgent(properties);
  return ret;
}

/*!
* @fn       setCredentialsOfWSAgent
* @param    None
* @return   None
* @brief    set Credentials of OSR/SEARCH/SUPERVISOR agent.
*/
void NssHandler::setCredentialsOfWSAgent()
{
  TRACE_LOG("")
  m_nssAccesslayer->setCredentialsOfWSAgent();
}

/*!
* @fn       CreateScannerOperationAgent
* @param    const std::string&  scannername
* @return   int- Agent creation status (S_OK/E_POINTER - success/failure).
* @brief    Creates Osr/ Search/ Supervisor Scanner Operation agent.
*/
int NssHandler::CreateScannerOperationAgent(const std::string& name)
{
  TRACE_LOG("");
  int ret = m_nssAccesslayer->CreateScannerOperationAgent(name);
  return ret;
}
}  // end of namespace ws
}  // end of namespace analogic
