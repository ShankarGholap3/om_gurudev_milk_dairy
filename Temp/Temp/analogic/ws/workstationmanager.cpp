/*!
* @file     workstationmanager.cpp
* @author   Agiliad
* @brief   This file contains interface to manage workstation.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include <analogic/ws/workstationmanager.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       WorkstationManager();
* @param    None
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
WorkstationManager::WorkstationManager()
{
  TRACE_LOG("");
}
/*!
* @fn       setBagDataHandlerThread();
* @param    boost::shared_ptr<QThread>
* @return   None
* @brief    set bag data thread
*/
void WorkstationManager::setBagDataHandlerThread(boost::shared_ptr<QThread> thread)
{
  m_bagdata_hdl_thread = thread;
}

/*!
* @fn       init();
* @param    None
* @return   None
* @brief    initialize class
*/
void WorkstationManager::init()
{
  TRACE_LOG("");
#ifdef WORKSTATION
  DEBUG_LOG("Creating instance of PullDataHandler");
  m_bagdata_acq_handle.reset(new PullDataHandler(m_bagdata_hdl_thread.get()));
  connect(m_bagdata_acq_handle.get(), SIGNAL(notifyBagReceivedFail(int, QString, int)),
          this, SIGNAL(authstatuschanged(int, QString, int)));
  DEBUG_LOG("Creating instance of ScannerFaultHandler");
  m_scannerfault_handle.reset(new ScannerFaultHandler());
  DEBUG_LOG("Creating instance of NetworkFaultHandler");
  m_networkfault_handle.reset(new NetworkFaultHandler());
#endif
  // !!! Uncomment once report is done !!!! Creating instance of ReportHandler
  // m_report_handler = new ReportHandler(this);
}
#ifdef WORKSTATION
/*!
* @fn       authenticate
* @param    QString username
* @param    QString password
* @return   QMLEnums::WSAuthenticationResult
* @brief    Authenticate user using connection and creates osr/ search agent.
*/
QMLEnums::WSAuthenticationResult  WorkstationManager::authenticate(
    QString username, QString password, QString pin)
{
  TRACE_LOG("");
  QMLEnums::WSAuthenticationResult ret = QMLEnums::IO_ERROR;
  if (m_nss_handler != NULL)
  {
    // authenticate user

    std::map<analogic::nss::AuthMethodType, std::string> auth_methods;
    if(!password.isEmpty())
    {
      auth_methods.insert(pair<analogic::nss::AuthMethodType, std::string>(analogic::nss::AuthMethodType::AUTH_METHOD_PASSWORD, password.toStdString()));
    }
    if(!pin.isEmpty())
    {
      auth_methods.insert(pair<analogic::nss::AuthMethodType, std::string>(analogic::nss::AuthMethodType::AUTH_METHOD_PIN, pin.toStdString()));
    }
    ret = m_nss_handler->authenticate(username, auth_methods);
    emit authstatuschanged(static_cast<int>(ret), "", QMLEnums::
                           NETWORK_AUTHENTICATION_MODEL);
    if(QMLEnums::WSAuthenticationResult::SUCCESS == ret)
    {
      INFO_LOG("User authentication Success");
      m_username = username;
      m_password = password;
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

/*!
* @fn       changePassword
* @param    const std::string& - username
* @param    const std::string& - oldAuthMethodDetail
* @param    const std::string& - newAuthMethodDetail
* @param    QMLEnums::AuthMethodType - authMethodType
* @return   QMLEnums::WSBasicCommandResult - result
* @brief    Changes password of the user to new password.
*/
QMLEnums::WSBasicCommandResult WorkstationManager::changePassword(
    const std::string &username, const std::string &oldAuthMethodDetail, const std::string &newAuthMethodDetail, QMLEnums::AuthMethodType authMethodType)
{
  TRACE_LOG("");
  QMLEnums::WSBasicCommandResult ret = QMLEnums::WSBASIC_COMMAND_RESULT_IO_ERROR;
  if (m_nss_handler != NULL)
  {
    // change password user
    ret = m_nss_handler->changePassword(username, oldAuthMethodDetail, newAuthMethodDetail, authMethodType);
    emit authstatuschanged(static_cast<int>(ret), QString::fromStdString(QMLEnums::getWSBasicCmdResultString(ret)),
                           QMLEnums::NETWORK_CHANGE_PASSWORD_MODEL);
    if(QMLEnums::WSBasicCommandResult::WSBASIC_COMMAND_RESULT_SUCCESS != ret)
    {
      ERROR_LOG("Failed User change password ");
    }
    else
    {
      INFO_LOG("User password change on Nss");
      // set currently logged in user at this agent
      m_username = QString::fromStdString(username);
      m_password = QString::fromStdString(newAuthMethodDetail);
      m_nss_handler->setCredentialsOfWSAgent();
      CreateScannerOperationAgent();
    }
    return ret;
  }
  else
  {
    ERROR_LOG("Nss Handler instance is NULL");
    emit authstatuschanged(static_cast<int>(ret), "", QMLEnums::
                           NETWORK_CHANGE_PASSWORD_MODEL);
    return ret;
  }
}

/*!
* @fn       CreateScannerOperationAgent
* @param    None
* @return   int- Agent creation status (S_OK/E_POINTER - success/failure).
* @brief    Creates Osr/ Search Scanner Operation agent.
*/
int WorkstationManager::CreateScannerOperationAgent()
{
  int ret = 1;  // E_FAIL
  QString scannername = WorkstationConfig::getInstance()->getScannername(); 
  if(NULL != m_nss_handler)
  {
      if(!scannername.isEmpty()){
                  ret = m_nss_handler->CreateScannerOperationAgent(
                      scannername.toStdString());
      }
    DEBUG_LOG("Scanner operation agent creation state ("
              "0-S_OK/1-E_FAIL) is: "<< ret);
    emit registScannerInDiscovered(ret);
  }
  return ret;
}

#endif

/*!
* @fn       ~WorkstationManager
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
WorkstationManager::~WorkstationManager()
{
#ifdef WORKSTATION
  TRACE_LOG("");
  DEBUG_LOG("Destroying members.");
  m_bagdata_acq_handle.reset();
  m_scannerfault_handle.reset();
  m_networkfault_handle.reset();
#endif
}

#ifdef WORKSTATION
/*!
    * @fn       WorkstationManager::getBagDataAcqHnd
    * @param    None
    * @return   boost::shared_ptr<BagDataAcqHandler>
    * @brief    Gets Bag Data acquisition handler.
    */
boost::shared_ptr<BagDataAcqHandler> WorkstationManager::getBagDataAcqHnd()
{
  DEBUG_LOG("Returning handle to bagdata acq handler");
  return m_bagdata_acq_handle;
}
/*!
* @fn       getScannerFaultHandle
* @param    None
* @return   boost::shared_ptr<ScannerFaultHandler>
* @brief    Gets scanner fault handler
*/
boost::shared_ptr<ScannerFaultHandler> WorkstationManager::getScannerFaultHandle() const
{
  DEBUG_LOG("Returning handle to scanner fault handler");
  return m_scannerfault_handle;
}

/*!
* @fn       getNWFaultHandle
* @param    None
* @return   boost::shared_ptr<NetworkFaultHandler>
* @brief    Gets network fault handler
*/
boost::shared_ptr<NetworkFaultHandler> WorkstationManager::getNWFaultHandle() const
{
  DEBUG_LOG("Returning handle to network fault handler");
  return m_networkfault_handle;
}

/*!
 * @fn       setAuthenticatedUserDetailsToScanner
 * @param    None
 * @return   void
 * @brief    Sets Authenticated user details to scanner agent.
 */
void WorkstationManager::setAuthenticatedUserDetailsToScanner()
{
  INFO_LOG("User authenticated on Nss");
  // set currently logged in user at this agent
  m_nss_handler->setCredentialsOfWSAgent();
  CreateScannerOperationAgent();
  BagsDataPool *bagpool = BagsDataPool::getInstance();
  if(bagpool)
  {
    for (unsigned int i = 0; i < bagpool->getSize(); i++)
    {
      boost::shared_ptr<BagData> bag = bagpool->getBagbyIndex(i);
      if (bag != NULL)
      {
        bag->setOperatorName(m_username);
      }
    }
  }
}
#endif
/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    Slot calls on exit if application.
*/
void WorkstationManager::onExit()
{
#ifdef WORKSTATION
  TRACE_LOG("");
  DEBUG_LOG("Sending exit");
  if (m_bagdata_acq_handle.get() != NULL)
  {
    DEBUG_LOG("Sending exit to bagdata acq handler");
    emit m_bagdata_acq_handle->exit();
  }
  if (m_scannerfault_handle.get() != NULL)
  {
    DEBUG_LOG("Sending exit to scanner fault handler");
    m_scannerfault_handle->onExit();
  }
  if (m_networkfault_handle.get() != NULL)
  {
    DEBUG_LOG("Sending exit to network fault handler");
    m_networkfault_handle->onExit();
  }
#endif
  Semaphore::getInstance()->release(1);
}

/*!
* @fn       getUsername
* @param    None
* @return   QString username
* @brief    Gets name of currently logged in user.
*/
QString WorkstationManager::getUsername()
{
  return m_username;
}

/*!
* @fn       validateAndParseXml
* @param    None
* @return   None
* @brief    Parse xml configuration.
*/
void WorkstationManager::validateAndParseXml()
{
  QString error;
  QVariantList list;
  if (!(XmlService::getXmlAndXsdContent(WorkstationConfig::getInstance()->getConfigxmlfilename(),
                                        WorkstationConfig::getInstance()->getConfigxsdfilename(),
                                        m_configXMLData, m_configXSDData, error)))
  {
    emit authstatuschanged(Errors::E_FAIL, error, QMLEnums::WORKSTATION_CONFIG_MODEL);
  }
  else
  {
    if (!(XmlService::validateAndParseXml(m_configXMLData, m_configXSDData, list, error)))
    {
      emit authstatuschanged(Errors::E_FAIL, error, QMLEnums::WORKSTATION_CONFIG_MODEL);
    }
    else
    {
      emit updateModels(list, QMLEnums::WORKSTATION_CONFIG_MODEL);
    }
  }
}

/*!
* @fn       updateXMLFileData
* @param    QMLEnums::modelname
* @param    QString &data
* @return   bool
* @brief    comon function for update xml
*/
bool WorkstationManager::updateXMLFileData(QMLEnums::ModelEnum modelname, QString &data)
{
  if(data.isEmpty())
  {
    validateAndParseXml();
  }
  else
  {
    if(modelname == QMLEnums::WORKSTATION_CONFIG_MODEL)
    {
      QString strReplyData = data;
      strReplyData = QString ("{\"data\":") +strReplyData + "}";

      QJsonDocument jsondoc = QJsonDocument::fromJson(strReplyData.toUtf8());

      QJsonObject jobj =  jsondoc.object();
      QVariantMap mapobj =  jobj.toVariantMap();
      QVariantList listobj;

      if ( mapobj["data"].type() ==QVariant::List)
      {
        listobj = mapobj["data"].toList();
      }
      if(!listobj.isEmpty())
      {
        QString errorMsg;
        if(!XmlService::updateXmlContent(listobj, m_configXMLData, m_configXSDData, errorMsg))
        {
          ERROR_LOG("Error in updating xml file : " <<
                    WorkstationConfig::getInstance()->getConfigxmlfilename().toStdString());
          return false;
        }
        //  update property value for all tags
        WorkstationConfig::getInstance()->clearWSConfigMap();
        for(int i = 0; i < listobj.size(); i++)
        {
          QMap<QString, QVariant> mapElement = listobj.at(i).toMap();
          WorkstationConfig::updateProperty(mapElement[XML_TAG_NAME].toString(),
                                            mapElement[XML_TAG_VALUE].toString(),
                                            mapElement[XML_ATTR_TYPE].toString());

        }
        if(!XmlService::generateXmlFile(
             WorkstationConfig::getInstance()->getConfigxmlfilename(), m_configXMLData, m_configXSDData))
        {
          ERROR_LOG("Error in writing xml data for file : " <<
                    WorkstationConfig::getInstance()->getConfigxmlfilename().toStdString());
          return false;
        }
        data = m_configXMLData;
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  return false;
}

#ifdef WORKSTATION
/*!
* @fn       getLivecontrollerHnd
* @param    None
* @return   boost::shared_ptr<NssHandler>  - live controller handler
* @brief    Gets live controller handler for Primary Workstation
*/
boost::shared_ptr<NssHandler> WorkstationManager::getLiveControllerHnd()
{
  TRACE_LOG("");
  return m_nss_handler;
}

#endif
}  // end of namespace ws
}  // end of namespace analogic




