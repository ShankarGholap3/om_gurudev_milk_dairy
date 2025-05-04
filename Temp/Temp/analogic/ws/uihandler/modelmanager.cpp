/*!
* @file     modelmanager.cpp
* @author   Agiliad
* @brief    This file contains functions related to ModelManager
*           which handles updating the data for current screen view.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#include <analogic/ws/uihandler/modelmanager.h>
#include <analogic/ws/uihandler/modelupdater.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       ModelManager
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class ModelManager.
*/
ModelManager::ModelManager(QObject* parent):
  QObject(parent),
  m_modelsUpdater(NULL),
  m_modelEnumOffset(0),
  m_XmlReceived(false),
  m_XsdReceived(false),
  m_configXMLFile(QString::null),
  m_configXSDFile(QString::null),
  m_configXMLData(QString::null),
  m_configXSDData(QString::null)
{
}

/*!
* @fn       ~ModelManager
* @param    None
* @return   None
* @brief    Destructor for class ModelManager.
*/
ModelManager::~ModelManager()
{
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManager::init()
{
}

/*!
* @fn       onCommandButtonEvent
* @param    QMLEnums::ModelEnum  - modelname
* @param    QString data to be post
* @return   void
* @brief    This slot will call on command button clicked.
*/
void ModelManager::onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data)
{
  DEBUG_LOG("Command for modelname : " << modelname);
  if((data.isEmpty()) &&
     ((modelname == QMLEnums::PARAMETER_SEND_MODEL)
      || (modelname == QMLEnums::COMMAND_UPDATE_TIP_CONFIG_SEND_MODEL)
      ))
  {
    QString errMsg;
    QVariantList list;
    if ((XmlService::validateAndParseXml(m_configXMLData, m_configXSDData, list, errMsg)))
    {
      emit updateModels(list, modelname);
    }
    return;
  }

  if ((QMLEnums::ERROR_LOG_SEARCH_MODEL != modelname) &&
      (QMLEnums::PARAMETER_MODEL != modelname) &&
      (QMLEnums::PARAMETER_XSD_MODEL != modelname) &&
      (QMLEnums::IMAGE_QUALITY_MASS_VALUE_MODEL != modelname) &&
      (QMLEnums::COMMAND_REPORT_SCANNER_MODEL != modelname) &&
      (QMLEnums::COMMAND_REPORT_OPERATOR_MODEL != modelname)&&
      (QMLEnums::DOWNLOAD_LATEST_PACKAGE!= modelname)&&
      (QMLEnums::DOWNLOAD_PACKAGE_WITH_VERSION!= modelname)&&
      (QMLEnums::COMMAND_GET_VERSION!= modelname)&&
      (QMLEnums::INSTALLABLE_ALARM_LIB_MODEL!= modelname)&&
      (QMLEnums::INSTALLED_ALARM_LIB_MODEL!= modelname) )
  {
    m_modelsUpdater[modelname - m_modelEnumOffset]->postrequest(data);
    DEBUG_LOG("Posting request");
  }
  else if(modelname == QMLEnums::DOWNLOAD_LATEST_PACKAGE)
  {
    m_modelsUpdater[modelname - m_modelEnumOffset]->getdwrequest();
  }
  else if(modelname == QMLEnums::DOWNLOAD_PACKAGE_WITH_VERSION)
  {
    m_modelsUpdater[modelname - m_modelEnumOffset]->getdwrequest();
  }
  else
  {
    DEBUG_LOG("Sending get request");
    m_modelsUpdater[modelname - m_modelEnumOffset]->getrequest();
  }
}

/*!
* @fn       onauthstatuschanged();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelno
* @return   void
* @brief    Slot will call when authentication status changed
*/
void ModelManager::onauthstatuschanged(int hr, QString status, int modelnum )
{
  if((hr <= Errors::Errors::CONNECTION_ERROR)
     && (hr >= Errors::Errors::NETWORK_SESSION_OUT))
  {
    Rial::getinstance()->initwebservices();
  }
  DEBUG_LOG("Authentication status changed to :"<<status.toStdString() <<" for modelnum: "<< modelnum);
  emit authstatuschanged(hr, status, modelnum);
}

QString ModelManager::createDirAndMoveFile(QString sDownloadpath, QMLEnums::ModelEnum modelnum)
{
  int            fileerrorcode;
  QStringList    pieces;
  QString        filename;
  QString        filepath;
  QString        srcanddstpath;
  filepath = "";
  pieces = sDownloadpath.split( "/" );
  filename += pieces.value( pieces.length() - 1 );
  filepath = createAndGetDirPath(modelnum, filename);
  fileerrorcode = moveFilePath(filepath, sDownloadpath);
  srcanddstpath = tr("Source Path: ") + sDownloadpath + tr(" Destination Path: ") + filepath;
  if (fileerrorcode != 0)
  {
    ERROR_LOG("File Move Failed From Temp To File Name Folder.");
    emit authstatuschanged(fileerrorcode, srcanddstpath, modelnum);
  }
  return filepath;
}

/*!
* @fn       updateXMLFileData
* @param    QString &data - return
* @return   bool
* @brief    common function for update xml
*/
bool ModelManager::updateXMLFileData(QString &data)
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
      ERROR_LOG("Error in updating xml file : " << m_configXMLFile.toStdString());
      return false;
    }
    if(!XmlService::generateXmlFile(m_configXMLFile, m_configXMLData, m_configXSDData))
    {
      ERROR_LOG("Error in writing xml data for file : " << m_configXMLFile.toStdString());
      return false;
    }
    data = m_configXMLData;
    return true;
  }
  return false;
}

/*!
 * @fn      onExit
 * @param   None
 * @return  None
 * @brief   Function will call on exit AUTHENTICATIN_MODELof threads
 */
void ModelManager::onExit()
{
}
}  // end of namespace ws
}  // end of namespace analogic
