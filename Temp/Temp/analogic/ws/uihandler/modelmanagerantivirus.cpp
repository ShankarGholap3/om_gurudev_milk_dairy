/*!
* @file     ModelManagerAntiVirus.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view for Antivirus.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/taskmanager.h>
#include <analogic/ws/uihandler/commandmodel.h>
#include <analogic/ws/uihandler/authenticationmodel.h>
#include <analogic/ws/uihandler/modelmanagerantivirus.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/accesslayer/antivirusaccessinterface.h>
#include <analogic/ws/uihandler/modelupdater.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       ModelManagerAntiVirus
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class ModelManagerAntiVirus.
*/
ModelManagerAntiVirus::ModelManagerAntiVirus(QObject* parent):
  ModelManager(parent),
  // m_rTaskStatusManager(NULL),
  m_rrestmap()
{
  m_file = NULL;
  m_networkManager = NULL;
  m_multiPart = NULL;
  m_modelEnumOffset = QMLEnums::USER_ACCESS_RESET_MODEL + 1;
}

/*!
* @fn       ~ModelManagerAntiVirus
* @param    None
* @return   None
* @brief    Destructor for class ModelManagerAntiVirus.
*/
ModelManagerAntiVirus::~ModelManagerAntiVirus()
{
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManagerAntiVirus::init()
{
  TRACE_LOG("");
  ModelManager::init();
  DEBUG_LOG("Getting handle to Report admin Access interface");
  AntiVirusAccessInterface* raal = AntiVirusAccessInterface::getInstance();
  DEBUG_LOG("Setting RestProtocol to: " << "http://" << " Report admin server ip address to: "
            << WorkstationConfig::getInstance()->getantivirusseveripaddress().toStdString()
            << " Portno to: "<< WorkstationConfig::getInstance()->getantivirusserverportno().toStdString()
            << " ServerName to: /nsswebservice/");
  raal->setRestProtocol(DEFAULT_REST_PROTOCOL);
  raal->setipadress(WorkstationConfig::getInstance()->getantivirusseveripaddress());
  raal->setportno(WorkstationConfig::getInstance()->getantivirusserverportno());
  raal->setpassphrase(WorkstationConfig::getInstance()->getpassphrase());
  raal->setServerName(DEFAULT_ANTIVIRUS_SERVERNAME);
  initRestMap();
  initRestContentMap();

  DEBUG_LOG(" Initializing modelupdater for antivirus model");
  m_modelsUpdater = new ModelUpdater*[QMLEnums::ANTIVIRUS_MODELS_END - m_modelEnumOffset];
  THROW_IF_FAILED((m_modelsUpdater == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  DEBUG_LOG(" Initializing modelupdater for antivirus authentication model");
  m_modelsUpdater[QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL - m_modelEnumOffset]
      = new Authenticationmodel(QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL,
                                m_rrestmap[QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL],
      m_rrestContentMap[QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL]);

  THROW_IF_FAILED(
        (m_modelsUpdater[QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL - m_modelEnumOffset] == NULL)?
        Errors::E_OUTOFMEMORY : Errors::S_OK);
  connect(m_modelsUpdater[QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL - m_modelEnumOffset],
      SIGNAL(authstatuschanged(int, QString, int)),
      this,
      SLOT(onauthstatuschanged(int, QString, int)));
#ifdef WORKSTATION
  connect(m_modelsUpdater[QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL - m_modelEnumOffset], SIGNAL
      (sslerrorreceived(int, QString, int )), this, SLOT(onsslerrorreceived(int, QString, int)));
#endif

  for(int j = QMLEnums::COMMAND_GET_VERSION; j < QMLEnums::ANTIVIRUS_MODELS_END; j++)
  {
    DEBUG_LOG("Creating CommandModel for modelnum: "<< j <<
              " with resturi: "<< m_rrestmap[QMLEnums::ModelEnum(j)].toStdString());

    m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] = new CommandModel(
          QMLEnums::ModelEnum(j), m_rrestmap[QMLEnums::ModelEnum(j)],
        m_rrestContentMap[QMLEnums::ModelEnum(j)]);
    THROW_IF_FAILED((m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] == NULL) ?
          Errors::E_OUTOFMEMORY : Errors::S_OK);
  }
  //}

  for(int i = QMLEnums::COMMAND_GET_VERSION; i < QMLEnums::ANTIVIRUS_MODELS_END; i++)
  {
    DEBUG_LOG("Connecting modelupdater signal with its handler. modelnum is: " << i);
    THROW_IF_FAILED((m_modelsUpdater[i-m_modelEnumOffset] == NULL)?Errors::E_POINTER:Errors::S_OK);
    if (m_modelsUpdater[i-m_modelEnumOffset] != NULL)
    {
      connect(m_modelsUpdater[i-m_modelEnumOffset], SIGNAL(
            updateModel(QVariantList, QMLEnums::ModelEnum)), this, SLOT(
            onUpdateModels(QVariantList, QMLEnums::ModelEnum)));

      connect(m_modelsUpdater[i-m_modelEnumOffset], SIGNAL(
            authstatuschanged(int, QString, int )), this, SLOT(
            onauthstatuschanged(int, QString, int)));
    }
  }
}

/*!
* @fn       onCommandButtonEvent
* @param    QMLEnums::ModelEnum  - modelname
* @param    QString data to be post
* @return   void
* @brief    This slot will call on command button clicked.
*/
void ModelManagerAntiVirus::onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data)
{
  if((modelname == QMLEnums::UPDATE_LATEST_PACKAGE) || (modelname == QMLEnums::UPDATE_PACKAGE_WITH_VERSION))
  {
    QString tempIp = WorkstationConfig::getInstance()->getuseradminseveripaddress();
    QString tempPort = WorkstationConfig::getInstance()->getuseradminserverportno();
    QString restProto = DEFAULT_REST_PROTOCOL;
    QString urlString ;
    if(modelname == QMLEnums::UPDATE_LATEST_PACKAGE)
      urlString = restProto + tempIp + ":" + tempPort + ANTIVIRUDS_UPLOAD_SERVELET;
    else
      urlString = restProto + tempIp + ":" + tempPort + ANTIVIRUDS_UPLOAD_SERVELET+"/clamav-0.102.2.tar.gz";
    sendMultipartFile(data,urlString);
  }
  else
  {
    ModelManager::onCommandButtonEvent(modelname, data);
  }
}

/*!
* @fn      sendMultipartFile
* @param   QString filePath
* @param   QString urlPath
* @return  None
* @brief   Function will append data to file
*/
void ModelManagerAntiVirus::sendMultipartFile(QString filePath , QString urlPath)
{
  if(filePath.endsWith(".zip"))
  {
    SAFE_DELETE_LATER(m_multiPart);
    m_multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    QStringList pathStringList = filePath.split("/");
    QString fileNameTemp = pathStringList.at(pathStringList.size() - 1);
    QString fileInfo = "form-data; name=\"file\"; filename=\"" + fileNameTemp + "\"";

    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(fileInfo));

    SAFE_DELETE_LATER(m_file);
    m_file = new QFile(filePath);
    m_file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(m_file);
    m_file->setParent(m_multiPart);
    m_multiPart->append(imagePart);

    QUrl url(urlPath);
    QNetworkRequest request(url);
    SAFE_DELETE_LATER(m_networkManager);
    m_networkManager = new QNetworkAccessManager;
    AntiVirusAccessInterface* raal = AntiVirusAccessInterface::getInstance();

    QString m_strtoken = raal->gettoken();
    request.setRawHeader(AUTHORIZATION, m_strtoken.toLocal8Bit());
    m_multiPart->setContentType(QHttpMultiPart::FormDataType);
    m_uploadReply = m_networkManager->post(request, m_multiPart);
    m_multiPart->setParent(m_uploadReply); // delete the multiPart with the reply

    QByteArray bytesReply = m_uploadReply->readAll();
    INFO_LOG("Reply from multipart : " << bytesReply.toStdString());

    connect(m_uploadReply, SIGNAL(finished()),
            this, SLOT  (uploadDone()));

    connect(m_uploadReply, SIGNAL(uploadProgress(qint64, qint64)),
            this, SLOT  (uploadProgress(qint64, qint64)));
  }
  else
  {
    int hr = Errors::FILE_NOT_FOUND;
    QString status = "Input is not a valid zip file";
    QMLEnums::ModelEnum  m_modelnumber =  QMLEnums::UPDATE_LATEST_PACKAGE;
    emit authstatuschanged(hr, status, m_modelnumber);
  }

}

/*!
* @fn       uploadDone
* @param    None
* @return   None
* @brief    slot to call after upload success
*/
void ModelManagerAntiVirus::uploadDone()
{
  int hr = Errors::SUCCESS;
  QMLEnums::ModelEnum  m_modelnumber = QMLEnums::UPDATE_LATEST_PACKAGE;
  m_file->close();
  SAFE_DELETE_LATER(m_file);
  QString reply;
  reply = getUploadReply()->readAll();
  if(reply.compare("Upload successful") == 0)
    hr = Errors::SUCCESS;
  else
    hr = Errors::SERVER_ERROR;
  UsbStorageSrv usbsrvs;
  usbsrvs.unMountUsb();
  emit authstatuschanged(hr, reply, m_modelnumber);
}

/*!
* @fn       uploadProgress
* @param    qint64 val1
* @param    qint64 val2
* @return   None
* @brief    slot to call for file upload progress
*/
void ModelManagerAntiVirus::uploadProgress(qint64 val1, qint64 val2)
{
  DEBUG_LOG("File Upload progress: uploaded "<< val1 << " of " << val2);
  float byteReceived = val1;
  float totalBytes = val2;
  if((byteReceived != 0) && (totalBytes !=0))
  {
    int percentage =((byteReceived/totalBytes))*100;
    int hr = Errors::SUCCESS;
    QMLEnums::ModelEnum m_modelnumber = QMLEnums::COMMAND_UPDATE_PACKAGE_STATUS;
    QString reply = QString::number(percentage);
    emit authstatuschanged(hr, reply, m_modelnumber);
  }
}
/*!
 * @fn      getUploadReply
 * @param   None
 * @return  QNetworkReply*
 * @brief   get Network Reply instance
 */
QNetworkReply* ModelManagerAntiVirus::getUploadReply()
{
  return m_uploadReply;
}
/*!
 * @fn      setUploadReply
 * @param   QNetworkReply*
 * @return  None
 * @brief    set Network Reply instance
 */
void ModelManagerAntiVirus::setUploadReply(QNetworkReply *reply)
{
  m_uploadReply = reply;
}
/*!
 * @fn      onExit
 * @param   None
 * @return  None
 * @brief Function will call on exit AUTHENTICATIN_MODELof threads
 */
void ModelManagerAntiVirus::onExit()
{
  TRACE_LOG("");
  DEBUG_LOG("Calling exit on ModelManager");
  ModelManager::onExit();
  DEBUG_LOG("Destroying modelupdater array");
  SAFE_DELETE( m_modelsUpdater);
  //SAFE_DELETE( m_rTaskStatusManager);
  DEBUG_LOG("Destroying Report Admin Access Interface");
  AntiVirusAccessInterface::destroyInstance();
}

/*!
* @fn       onauthstatuschanged();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelnum
* @return   void
* @brief    Slot will call when authentication status changed
*/
void ModelManagerAntiVirus::onauthstatuschanged(int hr, QString status, int modelnum )
{
  emit authstatuschanged(hr, status, modelnum);
}
#ifdef WORKSTATION
void ModelManagerAntiVirus::onsslerrorreceived(int hr, QString status,int modelnum )
{
  emit sslerrorreceived(hr, status,modelnum);
}
#endif
/*!
* @fn       onTaskTimeout
* @param    None
* @return   None
* @brief    slots check if task has got timeout.
*/
void ModelManagerAntiVirus::onTaskTimeout()
{
  m_modelsUpdater[QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL - m_modelEnumOffset]->postrequest("", "");
}

/*!
 * @fn       initRestMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest mapping
 */
void ModelManagerAntiVirus::initRestMap()
{
  TRACE_LOG("");
  m_rrestmap[QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL]             = "credentials";
  m_rrestmap[QMLEnums::COMMAND_GET_VERSION]                        = "version";
  m_rrestmap[QMLEnums::COMMAND_GET_VERSION_LIST]                   = "versions";
  m_rrestmap[QMLEnums::UPDATE_LATEST_PACKAGE]                      = "package";
  m_rrestmap[QMLEnums::UPDATE_PACKAGE_WITH_VERSION]                = "package";
  m_rrestmap[QMLEnums::DOWNLOAD_LATEST_PACKAGE]                    = "nsswebservice/package";
  m_rrestmap[QMLEnums::DOWNLOAD_PACKAGE_WITH_VERSION]              = "nsswebservice/package";
}

/*!
 * @fn       initRestContentMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest contentType mapping
 */
void ModelManagerAntiVirus::initRestContentMap()
{
  TRACE_LOG("");
  for (int i = QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL ; i < QMLEnums::ANTIVIRUS_MODELS_END ; i++)
  {
    m_rrestContentMap[QMLEnums::ModelEnum(i)] = DEFAULT_CONTENT_TYPE;
  }
}

/*!
 * @fn       onUpdateModels
 * @param    QVariantList
 * @param    QMLEnums::ModelEnum model name
 * @return   void
 * @brief    This slot will call on data updation from rest.
 */
void ModelManagerAntiVirus::onUpdateModels(QVariantList list, QMLEnums::ModelEnum  modelname)
{
  DEBUG_LOG("Send update signal for modelname: "<< modelname << " with list count: "<< list.count());
  emit updateModels(list, modelname);
}

/*!
 * @fn       ontaskComplete
 * @param    QString - sDownloadpath
 * @param    QString - sDownloadError
 * @param    QMLEnums::ModelEnum - modelnum
 * @return   None
 * @brief    slot call on task comlete
 */
void ModelManagerAntiVirus::ontaskComplete(QString sDownloadpath , QString sDownloadError,
                                           QMLEnums::ModelEnum modelnum)
{
  DEBUG_LOG("Send filedownload finished signal for modelnum: "<< modelnum
            << " download path: "<< sDownloadpath.toStdString() << " download error: "<< sDownloadError.toStdString());
  QString sDestinationPath = createDirAndMoveFile(sDownloadpath, modelnum);
  emit fileDatadownloadFinished(sDestinationPath, sDownloadError, modelnum);
}
}  // end of namespace ws
}  // end of namespace analogic

