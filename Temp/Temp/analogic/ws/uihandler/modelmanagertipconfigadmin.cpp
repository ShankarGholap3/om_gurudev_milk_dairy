/*!
* @file     modelmanagertipconfigadmin.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view for tip config admin.
* @date     Aug, 02 2018
*
(c) Copyright <2018-2019> Analogic Corporation. All Rights Reserved
*/
#include "modelmanagertipconfigadmin.h"
#include <analogic/ws/uihandler/taskmanager.h>
#include <analogic/ws/uihandler/commandmodel.h>
#include <analogic/ws/uihandler/authenticationmodel.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/accesslayer/tipconfigadminaccessinterface.h>
#ifdef WORKSTATION
#include <analogic/ws/SSLErrorEnums.h>
#endif

namespace analogic
{
namespace ws
{
/*!
* @fn       ModelManagerTipConfigAdmin
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class ModelManagerTipConfigAdmin.
*/
ModelManagerTipConfigAdmin::ModelManagerTipConfigAdmin(QObject *parent):
    ModelManager(parent),
    m_rrestmap(),
    m_rTaskStatusManager(NULL),
    m_networreconnecttimer(NULL)

{
    m_modelEnumOffset = QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL;
}

/*!
* @fn       ModelManagerTipConfigAdmin
* @param    None
* @return   None
* @brief    Destructor for class ModelManagerTipConfigAdmin.
*/
ModelManagerTipConfigAdmin::~ModelManagerTipConfigAdmin()
{

}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManagerTipConfigAdmin::init()
{
    TRACE_LOG("");
    ModelManager::init();
    m_networreconnecttimer = new QTimer(this);
    m_networreconnecttimer->setInterval(WorkstationConfig::getInstance()->getNetworkReconnectTime());
    connect(m_networreconnecttimer, SIGNAL(timeout()), this, SLOT(onTaskTimeout()), Qt::QueuedConnection);

    DEBUG_LOG("Getting handle to tip config admin Access interface");
    TipConfigAdminAccessInterface* raal = TipConfigAdminAccessInterface::getInstance();
    DEBUG_LOG("Setting RestProtocol to: "<<"http://" << " tip config admin server ip address to: "<< WorkstationConfig::getInstance()->getTipConfigIpAddress().toStdString()
              <<" Portno to: "<< WorkstationConfig::getInstance()->getTipConfigPortNo().toStdString() << " ServerName to: /tipwebservice/");
    raal->setRestProtocol(DEFAULT_REST_PROTOCOL);
    raal->setipadress(WorkstationConfig::getInstance()->getTipConfigIpAddress());
    raal->setportno(WorkstationConfig::getInstance()->getTipConfigPortNo());
    raal->setpassphrase(WorkstationConfig::getInstance()->getpassphrase());
    raal->setServerName(DEFAULT_TIP_CONFIG_SERVERNAME);

    // creating new qtimer instance
    initRestMap();
    initRestContentMap();
    DEBUG_LOG(" Initializing modelupdater for tip config admin model");
    m_modelsUpdater = new ModelUpdater*[QMLEnums::TIP_CONFIG_ADMIN_MODELS_END - m_modelEnumOffset];
    THROW_IF_FAILED((m_modelsUpdater == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    DEBUG_LOG(" Initializing modelupdater for tip config admin authentication model");
    m_modelsUpdater[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset]
            = new Authenticationmodel(QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL,  m_rrestmap[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL],
            m_rrestContentMap[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL]);

    THROW_IF_FAILED(
                (m_modelsUpdater[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset] == NULL)?
                Errors::E_OUTOFMEMORY : Errors::S_OK);
    connect(m_modelsUpdater[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset],
            SIGNAL(authstatuschanged(int, QString, int)),
            this,
            SLOT(onauthstatuschanged(int, QString, int)));
#ifdef WORKSTATION
    connect(m_modelsUpdater[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset],
            SIGNAL(sslerrorreceived(int, QString, int)),
            this,
            SLOT(onsslerrorreceived(int, QString, int)));
#endif

    m_rTaskStatusManager = new TaskManager*[MAX_REPORT_PARALLEL_TASK];
    THROW_IF_FAILED((m_rTaskStatusManager == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    for (int i = 0; i < MAX_REPORT_PARALLEL_TASK; i++)
    {
        DEBUG_LOG("Creating instance of task manager");
        m_rTaskStatusManager[i] = new TaskManager(WorkstationConfig::getInstance()->getScannerAdminAsyncTaskRefreshInterval(),
                                                  WorkstationConfig::getInstance()->getScannerAdminAsyncTaskTimeoutInterval(),
                                                  QMLEnums::UPDATE_TIP_CONFIG_TASK_STATUS_MODEL);

        connect(m_rTaskStatusManager[i], SIGNAL
                (updateModels(QVariantList, QMLEnums::ModelEnum)), this, SLOT(
                    onUpdateModels(QVariantList, QMLEnums::ModelEnum)));

        connect(m_rTaskStatusManager[i], SIGNAL
                (authstatuschanged(int, QString, int)), this, SLOT(
                    onauthstatuschanged(int, QString, int)));

        connect(m_rTaskStatusManager[i], SIGNAL
                (taskcomplete(QString, QString, QMLEnums::ModelEnum)), this, SLOT(
                    ontaskComplete(QString, QString, QMLEnums::ModelEnum)));
    }

    for(int j = QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL+1; j < QMLEnums::TIP_CONFIG_ADMIN_MODELS_END; j++)
    {
        DEBUG_LOG("Creating CommandModel for modelnum: "<<j << " with resturi: "<< m_rrestmap[QMLEnums::ModelEnum(j)].toStdString());
        m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] = new CommandModel(
                    QMLEnums::ModelEnum(j), m_rrestmap[QMLEnums::ModelEnum(j)],
                m_rrestContentMap[QMLEnums::ModelEnum(j)]);
        THROW_IF_FAILED((m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

        DEBUG_LOG("Connecting modelupdater signal with its handler. modelnum is: " << j);
        THROW_IF_FAILED((m_modelsUpdater[j-m_modelEnumOffset] == NULL)?Errors::E_POINTER:Errors::S_OK);
        if (m_modelsUpdater[j-m_modelEnumOffset] != NULL)
        {
            connect(m_modelsUpdater[j-m_modelEnumOffset], SIGNAL(
                        updateModel(QVariantList, QMLEnums::ModelEnum)), this, SLOT(
                        onUpdateModels(QVariantList, QMLEnums::ModelEnum)));

            connect(m_modelsUpdater[j-m_modelEnumOffset], SIGNAL(
                        authstatuschanged(int, QString, int )), this, SLOT(
                        onauthstatuschanged(int, QString, int)));

            DEBUG_LOG("Connecting taskinfo signal and its handler for modelnum: "<<j);
            connect(m_modelsUpdater[j-m_modelEnumOffset], SIGNAL(taskInfoReceived(QVariantList, QMLEnums::ModelEnum)),
                    m_rTaskStatusManager[0], SLOT(ontaskInfoReceived(QVariantList, QMLEnums::ModelEnum)));
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
void ModelManagerTipConfigAdmin::onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data)
{
    if(modelname == QMLEnums::COMMAND_UPDATE_TIP_CONFIG_SEND_MODEL)
    {
        if((!data.isEmpty())&&(!updateXMLFileData(data)))
        {
            ERROR_LOG("Updation failed for xml file : " <<  m_configXMLFile.toStdString());
            return;
        }
    }
    ModelManager::onCommandButtonEvent(modelname,data);
}

/*!
 * @fn      onExit
 * @param   None
 * @return  None
 * @brief Function will call on exit AUTHENTICATIN_MODELof threads
 */
void ModelManagerTipConfigAdmin::onExit()
{
    TRACE_LOG("");
    DEBUG_LOG("Calling exit on ModelManager");
    ModelManager::onExit();
    DEBUG_LOG("Destroying modelupdater array");
    for (int i = QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL; i < QMLEnums::TIP_CONFIG_ADMIN_MODELS_END; i++){
        SAFE_DELETE( m_modelsUpdater[QMLEnums::ModelEnum(i-m_modelEnumOffset)]);
    }
    SAFE_DELETE( m_modelsUpdater);

    DEBUG_LOG("Destroying TaskManager array");
    for(int i = 0; i < MAX_REPORT_PARALLEL_TASK; i++)
    {
        SAFE_DELETE( m_rTaskStatusManager[i]);
    }
    SAFE_DELETE( m_rTaskStatusManager);
    TipConfigAdminAccessInterface::destroyInstance();
}

/*!
* @fn       onauthstatuschanged();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelnum
* @return   void
* @brief    Slot will call when authentication status changed
*/
void ModelManagerTipConfigAdmin::onauthstatuschanged(int hr, QString status,int modelnum )
{
    if((hr<=Errors::CONNECTION_ERROR)
            && (hr>=Errors::NETWORK_SESSION_OUT))
    {
        m_networreconnecttimer->start();
        Rial::getinstance()->initwebservices();
        emit authstatuschanged(hr, status,modelnum);
    }
    else
    {
        m_networreconnecttimer->stop();
        emit authstatuschanged(hr, status,modelnum);
    }
}
#ifdef WORKSTATION
void ModelManagerTipConfigAdmin::onsslerrorreceived(int hr, QString status,int modelnum )
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
void ModelManagerTipConfigAdmin::onTaskTimeout()
{
    m_modelsUpdater[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset]->postrequest("","");
}

/*!
 * @fn       initRestMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest mapping
 */
void ModelManagerTipConfigAdmin::initRestMap()
{
    TRACE_LOG("");
    QString uploadUrl = "upload";    
    m_rrestmap[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL]             = "credentials";
    m_rrestmap[QMLEnums::UPDATE_TIP_CONFIG_TASK_STATUS_MODEL]               = "taskinfo";
    m_rrestmap[QMLEnums::COMMAND_TIP_CONFIG_MODEL]                          = "tipconfig";
    m_rrestmap[QMLEnums::COMMAND_UPDATE_TIP_CONFIG_SEND_MODEL]              = uploadUrl + TIP_CONFIG_XML_FILE;
    m_rrestmap[QMLEnums::COMMAND_UPDATE_TIP_OBJECT_CONFIG_SEND_MODEL]       = uploadUrl + TIP_OBJECT_CONFIG_XML_FILE;
}

/*!
 * @fn       initRestContentMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest contentType mapping
 */
void ModelManagerTipConfigAdmin::initRestContentMap()
{
    TRACE_LOG("");
    m_rrestContentMap[QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL]          = DEFAULT_CONTENT_TYPE;
    m_rrestContentMap[QMLEnums::UPDATE_TIP_CONFIG_TASK_STATUS_MODEL]            = DEFAULT_CONTENT_TYPE;
    m_rrestContentMap[QMLEnums::COMMAND_UPDATE_TIP_CONFIG_SEND_MODEL]           = XML_CONTENT_TYPE;
    m_rrestContentMap[QMLEnums::COMMAND_UPDATE_TIP_OBJECT_CONFIG_SEND_MODEL]    = XML_CONTENT_TYPE;
    m_rrestContentMap[QMLEnums::COMMAND_TIP_CONFIG_MODEL]                       = DEFAULT_CONTENT_TYPE;
    m_rrestContentMap[QMLEnums::TIP_CONFIG_ADMIN_MODELS_END]                    = DEFAULT_CONTENT_TYPE;
}

/*!
 * @fn       onUpdateModels
 * @param    QVariantList
 * @param    QMLEnums::ModelEnum model name
 * @return   void
 * @brief    This slot will call on data updation from rest.
 */
void ModelManagerTipConfigAdmin::onUpdateModels(QVariantList list, QMLEnums::ModelEnum  modelname)
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
void ModelManagerTipConfigAdmin::ontaskComplete(QString sDownloadpath , QString sDownloadError,
                                             QMLEnums::ModelEnum modelnum)
{
    DEBUG_LOG("Send filedownload finished signal for modelnum: "<< modelnum
              << " download path: "<< sDownloadpath.toStdString() << " download error: "<< sDownloadError.toStdString());
    if (sDownloadpath.isEmpty())
    {
        return;
    }
    QString sDestinationPath = createDirAndMoveFile(sDownloadpath, modelnum);
    if (modelnum == QMLEnums::COMMAND_TIP_CONFIG_MODEL)
    {

        if (sDownloadpath.contains("xml")) {
            m_XmlReceived = true;
            m_configXMLFile = sDestinationPath;
        }
        else if (sDownloadpath.contains("xsd")) {
            m_XsdReceived = true;
            m_configXSDFile = sDestinationPath;
        }
        if (m_XmlReceived == true && m_XsdReceived == true)
        {
            QVariantList list;
            QString errMsg;
            if (!(XmlService::getXmlAndXsdContent(m_configXMLFile, m_configXSDFile,
                                                  m_configXMLData, m_configXSDData, errMsg)))
            {
                emit authstatuschanged(Errors::E_FAIL, errMsg, QMLEnums::COMMAND_TIP_CONFIG_MODEL);
            }
            else
            {
                if (!(XmlService::validateAndParseXml(m_configXMLData, m_configXSDData, list, errMsg)))
                {
                    emit authstatuschanged(Errors::E_FAIL, errMsg, QMLEnums::COMMAND_TIP_CONFIG_MODEL);
                }
                else
                {
                    emit fileDatadownloadFinished( m_configXMLFile, sDownloadError, QMLEnums::COMMAND_TIP_CONFIG_MODEL);
                    emit fileDatadownloadFinished( m_configXSDFile, sDownloadError, QMLEnums::COMMAND_TIP_CONFIG_MODEL);
                    emit updateModels(list, QMLEnums::COMMAND_TIP_CONFIG_MODEL);
                }
            }
            m_XmlReceived = false;
            m_XsdReceived = false;
            if(!list.empty())
            {
                 populateTIPInfo(list);
            }
        }
    }
    else
    {
        emit fileDatadownloadFinished(sDestinationPath, sDownloadError, modelnum);
    }
}

/*!
 * @fn       populateTIPInfo
 * @param    None
 * @return   None
 * @brief    Populate TIP info
 */
void ModelManagerTipConfigAdmin::populateTIPInfo(QVariantList data)
{
    for (int i=0; i<data.size(); i++)
    {
        ConfigBaseElement* configelement = (reinterpret_cast<ConfigBaseElement*>(data[i].data()));
        if (configelement->m_tagName.compare(TIP_HIT_TAG)==0)
        {
            WorkstationConfig::getInstance()->setTipHITMsgDialog(configelement->m_value);
        }
        else if (configelement->m_tagName.compare(TIP_MISS_TAG)==0)
        {
            WorkstationConfig::getInstance()->setTipMISSMsgDialog(configelement->m_value);
        }
        else if (configelement->m_tagName.compare(TIP_NON_TIP_TAG)==0)
        {
            WorkstationConfig::getInstance()->setTipNONTIPMsgDialog(configelement->m_value);
        }

        if (configelement->m_type == QMLEnums::LISTTYPE) {
            populateTIPInfo(configelement->m_values);
        }
    }
}
}  // end of namespace ws
}  // end of namespace analogic
