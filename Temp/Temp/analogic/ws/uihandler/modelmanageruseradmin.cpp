/*!
* @file     modelmanageruseradmin.cpp
* @author   Agiliad
* @brief    This file contains functions related to ModelManagerUserAdmin
*           which handles updating the data for user admin screen view.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#include <analogic/ws/uihandler/modelmanageruseradmin.h>


namespace analogic
{
namespace ws
{
/*!
* @fn       ModelManagerUserAdmin
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class ModelManagerUserAdmin.
*/
ModelManagerUserAdmin::ModelManagerUserAdmin(QObject* parent):
    ModelManager(parent),
    m_pTaskStatusManager(NULL),
    m_restmap()
{
    m_file = NULL;
    m_networkManager = NULL;
    m_multiPart = NULL;
    m_modelEnumOffset = QMLEnums::SCANNERADMIN_MODELS_END + 1;
}

/*!
* @fn       ~ModelManagerUserAdmin
* @param    None
* @return   None
* @brief    Destructor for class ModelManagerUserAdmin.
*/
ModelManagerUserAdmin::~ModelManagerUserAdmin()
{
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManagerUserAdmin::init()
{
    TRACE_LOG("");
    ModelManager::init();
    DEBUG_LOG("Getting handle to User admin Access interface");
    UserAdminAccessInterface* uaal = UserAdminAccessInterface::getInstance();
    DEBUG_LOG("Setting RestProtocol to: "<<"http://" << " User admin server ip address to: "<< WorkstationConfig::getInstance()->getuseradminseveripaddress().toStdString()
              <<" Portno to: "<< WorkstationConfig::getInstance()->getuseradminserverportno().toStdString() << " ServerName to: /adminwebservice/");
    uaal->setRestProtocol(DEFAULT_REST_PROTOCOL);
    uaal->setipadress(WorkstationConfig::getInstance()->getuseradminseveripaddress());
    uaal->setportno(WorkstationConfig::getInstance()->getuseradminserverportno());
    uaal->setServerName(DEFAULT_USER_ADMIN_SERVERNAME);

    initRestMap();
    initRestContentMap();

    DEBUG_LOG(" Initializing modelupdater for user admin model");
    m_modelsUpdater = new ModelUpdater*[QMLEnums::USERADMIN_MODELS_END - m_modelEnumOffset];
    THROW_IF_FAILED((m_modelsUpdater == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    DEBUG_LOG(" Initializing modelupdater for user admin authentication model");
    m_modelsUpdater[QMLEnums::USERADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset]
            = new Authenticationmodel(QMLEnums::USERADMIN_AUTHENTICATION_MODEL, "credentials", DEFAULT_CONTENT_TYPE);

    THROW_IF_FAILED(
                (m_modelsUpdater[QMLEnums::USERADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset] == NULL)?
                Errors::E_OUTOFMEMORY : Errors::S_OK);

    connect(m_modelsUpdater[QMLEnums::USERADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset],
            SIGNAL(authstatuschanged(int, QString, int)),
            this,
            SLOT(onauthstatuschanged(int, QString, int)));
#ifdef WORKSTATION
    connect(m_modelsUpdater[QMLEnums::USERADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset],
            SIGNAL(sslerrorreceived(int, QString, int)),
            this,
            SLOT(onsslerrorreceived(int, QString, int)));
#endif

    m_pTaskStatusManager = new TaskManager*[MAX_REPORT_PARALLEL_TASK];
    THROW_IF_FAILED((m_pTaskStatusManager == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);


    for (int i = 0; i < MAX_REPORT_PARALLEL_TASK; i++)
    {
        DEBUG_LOG("Creating instance of task manager");
        m_pTaskStatusManager[i] = new TaskManager(WorkstationConfig::getInstance()->getScannerAdminAsyncTaskRefreshInterval(),
                                                  WorkstationConfig::getInstance()->getScannerAdminAsyncTaskTimeoutInterval(),
                                                  QMLEnums::UPDATE_USERADMIN_TASK_STATUS_MODEL);

        connect(m_pTaskStatusManager[i], SIGNAL
                (updateModels(QVariantList, QMLEnums::ModelEnum)), this, SLOT(
                    onUpdateModels(QVariantList, QMLEnums::ModelEnum)));

        connect(m_pTaskStatusManager[i], SIGNAL
                (authstatuschanged(int, QString, int)), this, SLOT(
                    onauthstatuschanged(int, QString, int)));

        connect(m_pTaskStatusManager[i], SIGNAL
                (taskcomplete(QString, QString, QMLEnums::ModelEnum)), this, SLOT(
                    ontaskComplete(QString, QString, QMLEnums::ModelEnum)));
    }

    for (int j = QMLEnums::COMMAND_USERADMIN_USER_DB_IMPORT_MODEL; j < QMLEnums::USERADMIN_MODELS_END; j++)
    {
        DEBUG_LOG("Creating CommandModel for modelnum: "<<j << " with resturi: "<< m_restmap[QMLEnums::ModelEnum(j)].toStdString());
        m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] = new CommandModel(
                    QMLEnums::ModelEnum(j), m_restmap[QMLEnums::ModelEnum(j)],
                m_restContentMap[QMLEnums::ModelEnum(j)]);
        THROW_IF_FAILED((m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    }

    for(int i = QMLEnums::COMMAND_USERADMIN_USER_DB_IMPORT_MODEL; i < QMLEnums::USERADMIN_MODELS_END; i++)
    {
        DEBUG_LOG("Connecting modelupdater signal with its handler. modelnum is: " << i);
        THROW_IF_FAILED((m_modelsUpdater[i-m_modelEnumOffset] == NULL)?Errors::E_POINTER:Errors::S_OK);
        if (m_modelsUpdater[i-m_modelEnumOffset] != NULL)
        {
            connect(m_modelsUpdater[i-m_modelEnumOffset], SIGNAL(
                        updateModel(QVariantList, QMLEnums::ModelEnum)), this, SLOT(
                        onUpdateModels(QVariantList, QMLEnums::ModelEnum)));

            connect(m_modelsUpdater[i-m_modelEnumOffset],
                    SIGNAL(authstatuschanged(int, QString, int )), this,
                    SLOT(
                        onauthstatuschanged(int, QString, int)));

            DEBUG_LOG("Connecting taskinfo signal and its handler for modelnum: "<<i);
            connect(m_modelsUpdater[i-m_modelEnumOffset],
                    SIGNAL(taskInfoReceived(QVariantList, QMLEnums::ModelEnum)),
                    m_pTaskStatusManager[0], SLOT(ontaskInfoReceived(QVariantList, QMLEnums::ModelEnum)));
        }
    }
    m_uploadReply = NULL;
}


/*!
 * @fn       initRestMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest mapping
 */
void ModelManagerUserAdmin::initRestMap()
{
    TRACE_LOG("");
    m_restmap[QMLEnums::USERADMIN_AUTHENTICATION_MODEL]             = "credentials";
    m_restmap[QMLEnums::UPDATE_USERADMIN_TASK_STATUS_MODEL]         = "taskinfo";

    m_restmap[QMLEnums::GET_USERADMIN_USER_DB_IMPORT_ENABLED_MODEL] = "users/import";
    m_restmap[QMLEnums::GET_USERADMIN_USER_DB_EXPORT_ENABLED_MODEL] = "users/export";
    m_restmap[QMLEnums::GET_USERADMIN_KEY_IMPORT_ENABLED_MODEL]     = "users/import";
    m_restmap[QMLEnums::GET_USERADMIN_KEY_EXPORT_ENABLED_MODEL]     = "users/export";
    m_restmap[QMLEnums::GET_USERADMIN_KEY_GENERATE_ENABLED_MODEL]   = "users/export/generate";
    m_restmap[QMLEnums::COMMAND_USERADMIN_USER_DB_IMPORT_MODEL]     = "users/import";
    m_restmap[QMLEnums::COMMAND_USERADMIN_USER_DB_EXPORT_MODEL]     = "users/export";
    m_restmap[QMLEnums::COMMAND_USERADMIN_KEY_IMPORT_MODEL]         = "users/import";
    m_restmap[QMLEnums::COMMAND_USERADMIN_KEY_EXPORT_MODEL]         = "users/export";
    m_restmap[QMLEnums::COMMAND_USERADMIN_KEY_GENERATE_MODEL]       = "users/export/generate";
    m_restmap[QMLEnums::COMMAND_USERADMIN_FILE_UPLOAD]              = "uploadFile";

}

/*!
 * @fn       initRestContentMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest contentType mapping
 */
void ModelManagerUserAdmin::initRestContentMap()
{
    TRACE_LOG("");

    for (int i = QMLEnums::USERADMIN_AUTHENTICATION_MODEL; i < QMLEnums::USERADMIN_MODELS_END; i++)
    {
        m_restContentMap[QMLEnums::ModelEnum(i)] = DEFAULT_CONTENT_TYPE;
    }

}

/*!
* @fn       onCommandButtonEvent
* @param    QMLEnums::ModelEnum  - modelname
* @param    QString data to be post
* @return   void
* @brief    This slot will call on command button clicked.
*/
void ModelManagerUserAdmin::onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data)
{
    if (modelname == QMLEnums::COMMAND_USERADMIN_FILE_UPLOAD)
    {
        QString tempIp = WorkstationConfig::getInstance()->getuseradminseveripaddress();
        QString tempPort = WorkstationConfig::getInstance()->getuseradminserverportno();
        QString restProto = DEFAULT_REST_PROTOCOL;
        QString urlString = restProto + tempIp + ":" + tempPort + SYSADMIN_APP_UPLOAD_SERVELET;
        sendMultipartFile(data,urlString);
    }
    else
    {
        ModelManager::onCommandButtonEvent(modelname,data);
    }
}

/*!
* @fn       onauthstatuschanged();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelnum
* @return   void
* @brief    Slot will call when authentication status changed
*/
void ModelManagerUserAdmin::onauthstatuschanged(int hr, QString status,int modelnum )
{
    emit authstatuschanged(hr, status,modelnum);
}
#if defined WORKSTATION || defined RERUN
void ModelManagerUserAdmin::onsslerrorreceived(int hr, QString status,int modelnum )
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
void ModelManagerUserAdmin::onTaskTimeout()
{
    m_modelsUpdater[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset]->postrequest("","");
}
/*!
 * @fn       onUpdateModels
 * @param    QVariantList
 * @param    QMLEnums::ModelEnum model name
 * @return   void
 * @brief    This slot will call on data updation from rest.
 */
void ModelManagerUserAdmin::onUpdateModels(QVariantList list, QMLEnums::ModelEnum  modelname)
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
void ModelManagerUserAdmin::ontaskComplete(QString sDownloadpath , QString sDownloadError,
                                           QMLEnums::ModelEnum modelnum)
{
    DEBUG_LOG("Send filedownload finished signal for modelnum: "<< modelnum
              << " download path: "<< sDownloadpath.toStdString() << " download error: "<< sDownloadError.toStdString());
    QString sDestinationPath = createDirAndMoveFile(sDownloadpath, modelnum);
    emit fileDatadownloadFinished(sDestinationPath, sDownloadError, modelnum);
}

/*!
 * @fn      onExit
 * @param   None
 * @return  None
 * @brief Function will call on exit AUTHENTICATIN_MODELof threads
 */
void ModelManagerUserAdmin::onExit()
{
    TRACE_LOG("");
    DEBUG_LOG("Calling exit on modelmanager.");
    ModelManager::onExit();
    DEBUG_LOG("Destroy modelupdater");
    SAFE_DELETE(m_modelsUpdater[0]);
    SAFE_DELETE(m_modelsUpdater);
    DEBUG_LOG("Destroy unseradmin access interfacde handle");
    UserAdminAccessInterface::destroyInstance();
}


/*!
* @fn      sendMultipartFile
* @param   QString filePath
* @param   QString urlPath
* @return  None
* @brief   Function will append data to file
*/
void ModelManagerUserAdmin::sendMultipartFile(QString filePath , QString urlPath)
{

    SAFE_DELETE_LATER(m_multiPart);
    m_multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    QStringList pathStringList = filePath.split("/");
    QString fileNameTemp = pathStringList.at(pathStringList.size() - 1);
    QString fileInfo = "form-data; name=\"file\"; filename=\"" + fileNameTemp + "\"";

    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(fileInfo));
    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
    textPart.setBody("data");
    SAFE_DELETE_LATER(m_file);
    m_file = new QFile(filePath);
    m_file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(m_file);

    m_file->setParent(m_multiPart);

    m_multiPart->append(textPart);
    m_multiPart->append(imagePart);

    QUrl url(urlPath);
    QNetworkRequest request(url);
    SAFE_DELETE_LATER(m_networkManager);
    m_networkManager = new QNetworkAccessManager;
    m_uploadReply = m_networkManager->post(request, m_multiPart);
    m_multiPart->setParent(m_uploadReply); // delete the multiPart with the reply


    QByteArray bytesReply = m_uploadReply->readAll();
    INFO_LOG("Reply from multipart : " << bytesReply.toStdString());


    connect(m_uploadReply, SIGNAL(finished()),
            this, SLOT  (uploadDone()));

    connect(m_uploadReply, SIGNAL(uploadProgress(qint64, qint64)),
            this, SLOT  (uploadProgress(qint64, qint64)));


}

/*!
* @fn       uploadDone
* @param    None
* @return   None
* @brief    slot to call after upload success
*/
void ModelManagerUserAdmin::uploadDone()
{
    m_file->close();
    SAFE_DELETE_LATER(m_file);    
    INFO_LOG("File Upload finished: "<< getUploadReply()->readAll().toStdString());
    UsbStorageSrv usbsrvs;
    usbsrvs.unMountUsb();

    //TODO the post request should be sent after this
}

/*!
* @fn       uploadProgress
* @param    qint64 val1
* @param    qint64 val2
* @return   None
* @brief    slot to call for file upload progress
*/
void ModelManagerUserAdmin::uploadProgress(qint64 val1, qint64 val2)
{
    DEBUG_LOG("File Upload progress: uploaded "<< val1 << " of " << val2);
}

/*!
 * @fn      getUploadReply
 * @param   None
 * @return  QNetworkReply*
 * @brief   get Network Reply instance
 */
QNetworkReply* ModelManagerUserAdmin::getUploadReply()
{
    return m_uploadReply;
}

/*!
 * @fn      setUploadReply
 * @param   QNetworkReply*
 * @return  None
 * @brief    set Network Reply instance
 */
void ModelManagerUserAdmin::setUploadReply(QNetworkReply *reply)
{
    m_uploadReply = reply;
}

}  // end of namespace ws
}  // end of namespace analogic
