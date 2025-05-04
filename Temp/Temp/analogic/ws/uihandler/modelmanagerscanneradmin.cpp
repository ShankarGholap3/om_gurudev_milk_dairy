/*!
* @file     modelmanagerscanneradmin.cpp
* @author   Agiliad
* @brief    This file contains functions related to ModelManager
*           which handles updating the data for current screen view of scanner admin.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/accesslayer/scanneradminaccessinterface.h>
#include <analogic/ws/uihandler/modelmanagerscanneradmin.h>
#include <analogic/ws/uihandler/taskmanager.h>
#ifdef WORKSTATION
#include <analogic/ws/SSLErrorEnums.h>
#endif
#include <analogic/ws/uihandler/authenticationmodel.h>
#include <analogic/ws/uihandler/commandmodel.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       ModelManagerScannerAdmin
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class ModelManagerScannerAdmin.
*/
ModelManagerScannerAdmin::ModelManagerScannerAdmin(QObject* parent): ModelManager(parent),
    m_bpaused(false),
    m_scrmodels(),
    m_pTaskStatusManager(NULL),
    m_pUpdateTimer(NULL),
    m_restmap()
{
    TRACE_LOG("");
    m_modelEnumOffset = 0;
}

/*!
* @fn       ~ModelManagerScannerAdmin
* @param    None
* @return   None
* @brief    Destructor for class ModelManagerScannerAdmin.
*/
ModelManagerScannerAdmin::~ModelManagerScannerAdmin()
{
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManagerScannerAdmin::init()
{
    TRACE_LOG("");
    DEBUG_LOG("Initializing modelmanager");
    ModelManager::init();

    // creating new qtimer instance
    m_pUpdateTimer.reset(new QTimer(this));
    THROW_IF_FAILED((m_pUpdateTimer.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    // setting timer interval
    m_pUpdateTimer->setInterval(1000);

    // connecting timer events to proper slot
    connect(m_pUpdateTimer.get(), SIGNAL(timeout()), this,
            SLOT(onprocessUpdateModels()) ,
            Qt::QueuedConnection);
    DEBUG_LOG("Initializing rest map");
    initRestMap();
    initRestContentMap();
    DEBUG_LOG("Start model update timer");
    m_pUpdateTimer->start();
    m_networreconnecttimer = new QTimer(this);
    m_networreconnecttimer->setInterval(WorkstationConfig::getInstance()->getNetworkReconnectTime());
    connect(m_networreconnecttimer, SIGNAL(timeout()), this, SLOT(onTaskTimeout()), Qt::QueuedConnection);

    DEBUG_LOG("Getting ScannerAdminAccessInterface handle");
    ScannerAdminAccessInterface* saal = ScannerAdminAccessInterface::getInstance();
    THROW_IF_FAILED(((saal == NULL) ? Errors::E_POINTER : Errors::S_OK));

    DEBUG_LOG("Setting ScannerAdminService IP address to: "<<WorkstationConfig::getInstance()->getscanneradminseveripaddress().toStdString()
              <<" portno to: "<< WorkstationConfig::getInstance()->getscanneradminserverportno().toStdString()
              <<" Pass phrase to: "<< WorkstationConfig::getInstance()->getpassphrase().toStdString());
    saal->setipadress(WorkstationConfig::getInstance()->getscanneradminseveripaddress());
    saal->setportno(WorkstationConfig::getInstance()->getscanneradminserverportno());
    saal->setpassphrase(WorkstationConfig::getInstance()->getpassphrase());
#ifdef WORKSTATION
    saal->setserverpassphrase(WorkstationConfig::getInstance()->getserverpassphrase());
    if(WorkstationConfig::getInstance()->getsslauth())
    saal->loadsrvrpfxcertificate(WorkstationConfig::getInstance()->getservercertificate());
#endif
    DEBUG_LOG("Creating instance for handling array of models");
    m_modelsUpdater = new ModelUpdater*[QMLEnums::SCANNERADMIN_MODELS_END];
    THROW_IF_FAILED((m_modelsUpdater == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    DEBUG_LOG("Creating instance of authentication model");
    m_modelsUpdater[QMLEnums::AUTHENTICATIN_MODEL] = new Authenticationmodel
            (QMLEnums::AUTHENTICATIN_MODEL, m_restmap[QMLEnums::AUTHENTICATIN_MODEL],
            m_restContentMap[QMLEnums::AUTHENTICATIN_MODEL]);
    THROW_IF_FAILED((m_modelsUpdater[QMLEnums::AUTHENTICATIN_MODEL] == NULL)?
                Errors::E_OUTOFMEMORY:Errors::S_OK);
    connect(m_modelsUpdater[QMLEnums::AUTHENTICATIN_MODEL], SIGNAL
            (authstatuschanged(int, QString, int )), this, SLOT(onauthstatuschanged(int, QString, int)));    
#ifdef WORKSTATION
    connect(m_modelsUpdater[QMLEnums::AUTHENTICATIN_MODEL], SIGNAL
        (sdswClientExited(int )), this, SIGNAL(sdswClientExited(int)));
    connect(m_modelsUpdater[QMLEnums::AUTHENTICATIN_MODEL], SIGNAL
            (sslerrorreceived(int, QString, int )), this, SLOT(onsslerrorreceived(int, QString, int)));
#endif
    DEBUG_LOG("Creating handle to array of taskmanager");
    m_pTaskStatusManager = new TaskManager*[MAX_PARALLEL_TASK];
    THROW_IF_FAILED((m_pTaskStatusManager == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    for (int i = 0; i < MAX_PARALLEL_TASK; i++)
    {
        DEBUG_LOG("Creating instance of task manager");
        m_pTaskStatusManager[i] = new TaskManager(WorkstationConfig::getInstance()->getScannerAdminAsyncTaskRefreshInterval(),
                                                  WorkstationConfig::getInstance()->getScannerAdminAsyncTaskTimeoutInterval(),
                                                  QMLEnums::UPDATE_TASK_STATUS_MODEL);
        THROW_IF_FAILED((m_pTaskStatusManager[i] == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);
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

    for (int i = QMLEnums::KEYCONSOLE_MODEL; i < QMLEnums::COMMAND_RADIATIONSURVEY_MODEL; i++)
    {
        DEBUG_LOG("Creating modelupdater for modelnum: "<<i << " with resturi: "<< m_restmap[QMLEnums::ModelEnum(i)].toStdString());
        m_modelsUpdater[QMLEnums::ModelEnum(i)] = new ModelUpdater(
                    QMLEnums::ModelEnum(i), m_restmap[QMLEnums::ModelEnum(i)],
                m_restContentMap[QMLEnums::ModelEnum(i)]);
        THROW_IF_FAILED((m_modelsUpdater[QMLEnums::ModelEnum(i)] == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    }

    for(int j = QMLEnums::COMMAND_RADIATIONSURVEY_MODEL; j < QMLEnums::SCANNERADMIN_MODELS_END; j++)
    {
        DEBUG_LOG("Creating CommandModel for modelnum: "<<j << " with resturi: "<< m_restmap[QMLEnums::ModelEnum(j)].toStdString());
        m_modelsUpdater[QMLEnums::ModelEnum(j)] = new CommandModel(
                    QMLEnums::ModelEnum(j), m_restmap[QMLEnums::ModelEnum(j)],
                m_restContentMap[QMLEnums::ModelEnum(j)]);
        THROW_IF_FAILED((m_modelsUpdater[QMLEnums::ModelEnum(j)] == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    }

    for(int i = QMLEnums::KEYCONSOLE_MODEL; i < QMLEnums::SCANNERADMIN_MODELS_END; i++)
    {
        DEBUG_LOG("Connecting modelupdater signal with its handler. modelnum is: " << i);
        THROW_IF_FAILED((m_modelsUpdater[i] == NULL)?Errors::E_POINTER:Errors::S_OK);
        if (m_modelsUpdater[i] != NULL)
        {
            connect(m_modelsUpdater[i], SIGNAL(
                        updateModel(QVariantList, QMLEnums::ModelEnum)), this, SLOT(
                        onUpdateModels(QVariantList, QMLEnums::ModelEnum)));

            connect(m_modelsUpdater[i], SIGNAL(
                        authstatuschanged(int, QString, int )), this, SLOT(
                        onauthstatuschanged(int, QString, int)));
            if (((QMLEnums::ModelEnum)i == QMLEnums::COMMAND_IMAGEQUALITY_TEST_MODEL)
                    || ((QMLEnums::ModelEnum)i == QMLEnums::COMMAND_RADIATIONSURVEY_MODEL)
                    || ((QMLEnums::ModelEnum)i == QMLEnums::ERROR_LOG_SEARCH_MODEL)
                    || ((QMLEnums::ModelEnum)i == QMLEnums::PARAMETER_MODEL)
                    || ((QMLEnums::ModelEnum)i == QMLEnums::PARAMETER_XSD_MODEL)
                    )
            {
                DEBUG_LOG("Connecting taskinfo signal and its handler for modelnum: "<<i);
                connect(m_modelsUpdater[i], SIGNAL(taskInfoReceived(QVariantList, QMLEnums::ModelEnum)),
                        m_pTaskStatusManager[0], SLOT(ontaskInfoReceived(QVariantList, QMLEnums::ModelEnum)));
            }
            if ((QMLEnums::ModelEnum)i == QMLEnums::COMMAND_RADIATIONSURVEY_CANCEL_MODEL)
            {
                connect(m_modelsUpdater[i], SIGNAL(taskInfoReceived(QVariantList,QMLEnums::ModelEnum)),
                        m_pTaskStatusManager[1], SLOT(ontaskInfoReceived(QVariantList,QMLEnums::ModelEnum)));
            }
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
void ModelManagerScannerAdmin::onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data)
{
    m_pTaskStatusManager[0]->cancelTask();
    if((!data.isEmpty()) && (modelname == QMLEnums::PARAMETER_SEND_MODEL))
    {
        if(!updateXMLFileData(data))
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
 * @brief Function will call on exit of threads
 */
void ModelManagerScannerAdmin::onExit()
{
    TRACE_LOG("");
    DEBUG_LOG("Calling exit on ModelManager");
    ModelManager::onExit();
    DEBUG_LOG("Destroying modelupdater array");
    for (int i = QMLEnums::AUTHENTICATIN_MODEL; i < QMLEnums::SCANNERADMIN_MODELS_END; i++){
        SAFE_DELETE( m_modelsUpdater[QMLEnums::ModelEnum(i)]);
    }
    SAFE_DELETE( m_modelsUpdater);

    m_pUpdateTimer->stop();
    m_pUpdateTimer.reset();
    DEBUG_LOG("Destroying TaskManager array");
    for(int i = 0; i < MAX_PARALLEL_TASK; i++)
    {
        SAFE_DELETE( m_pTaskStatusManager[i]);
    }
    SAFE_DELETE( m_pTaskStatusManager);
    DEBUG_LOG("Destroying Scanner Admin Access Interface");
    ScannerAdminAccessInterface::destroyInstance();
}

/*!
* @fn       onprocessUpdateModels()
* @param    None
* @return   None
* @brief    timer thread that run every configurable interval.
*/
void ModelManagerScannerAdmin::onprocessUpdateModels()
{
    if(!m_bpaused)
    {
        DEBUG_LOG("Update all models");
        for(int i = static_cast<int>(QMLEnums::KEYCONSOLE_MODEL);
            i < static_cast<int>(QMLEnums::SCANNERADMIN_MODELS_END);
            i++)
        {
            if(m_scrmodels.contains((QMLEnums::ModelEnum)i))
            {
                m_modelsUpdater[i]->getrequest();
            }
        }
        // as data requests has been made data timer tick will notify other threads to update UI
        emit dataTimerTick();
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
void ModelManagerScannerAdmin::onauthstatuschanged(int hr, QString status,int modelnum )
{
    if((hr<=Errors::CONNECTION_ERROR)
            && (hr>=Errors::NETWORK_SESSION_OUT))
    {
        if(!m_bpaused)
        {
            m_bpaused = true;
            m_networreconnecttimer->start();
            Rial::getinstance()->initwebservices();
            emit authstatuschanged(hr, status,modelnum);
        }
    }
    else
    {
        if (m_bpaused)
        {
            m_networreconnecttimer->stop();
            m_bpaused = false;
            emit authstatuschanged(hr, status,modelnum);
        }
        else
        {
            emit authstatuschanged(hr, status,modelnum);
        }
    }


}
#ifdef WORKSTATION
/*!
* @fn       onsslerrorreceived();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelnum
* @return   void
* @brief    Slot will call when ssl signal is received
*/
void ModelManagerScannerAdmin::onsslerrorreceived(int hr, QString status,int modelnum )
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
void ModelManagerScannerAdmin::onTaskTimeout()
{
    m_modelsUpdater[QMLEnums::AUTHENTICATIN_MODEL]->postrequest("","");
}

/*!
 * @fn       initRestMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest mapping
 */
void ModelManagerScannerAdmin::initRestMap()
{
    TRACE_LOG("");
    m_restmap[QMLEnums::UPDATE_TASK_STATUS_MODEL]               = "taskinfo";
    m_restmap[QMLEnums::AUTHENTICATIN_MODEL]                    = "credentials/login";
    m_restmap[QMLEnums::SDSW_HEARTBEAT_MODEL]                   = "heartbeat";
    m_restmap[QMLEnums::KEYCONSOLE_MODEL]                       = "keyconsole";
    m_restmap[QMLEnums::ACCESSPANEL_MODEL]                      = "accesspanel";
    m_restmap[QMLEnums::ESTOPS_MODEL]                           = "estop";
    m_restmap[QMLEnums::SAFETYCONTROLLERFAULTS_MODEL]           = "scanner/safetyfaults";
    m_restmap[QMLEnums::POWERLINKINVERTERSTATUS_MODEL]          = "powerlink/inverter/status";
    m_restmap[QMLEnums::POWERLINKINVERTERDISCRETESTATUS_MODEL]  = "powerlink/powerlinkdiscrete/status";
    m_restmap[QMLEnums::POWERLINKINVERTERSERIALSTATUS_MODEL]    = "powerlink/inverterserial/status";
    m_restmap[QMLEnums::POWERLINKINVERTERSERIALFAULTS_MODEL]    = "powerlink/inverterserial/faults";
    m_restmap[QMLEnums::POWERLINKRECTIFIERSTATUS_MODEL]         = "powerlink/rectifier/status";
    m_restmap[QMLEnums::CONVEYORANDGANTRY_MODEL]                = "motioncontrol/conveyorandgantry";
    m_restmap[QMLEnums::RCBGANTRYSTATUS_MODEL]                  = "motioncontrol/rcbgantry";
    m_restmap[QMLEnums::BHSSTATUS_MODEL]                        = "motioncontrol/bhs";
    m_restmap[QMLEnums::MOTIONCONTROLDATA_MODEL]                = "motioncontrol/data";
    m_restmap[QMLEnums::XRAYSTATUS_MODEL]                       = "xray/status";
    m_restmap[QMLEnums::XRAYTIME_MODEL]                         = "xray/time";
    m_restmap[QMLEnums::XRAYFAULTS_MODEL]                       = "xray/faults";
    m_restmap[QMLEnums::XRAYDATA_MODEL]                         = "xray/data";
    m_restmap[QMLEnums::XRAYCOMMANDSTATUS_MODEL]                = "xray/commandstatus";
    m_restmap[QMLEnums::IMAGE_QUALITY_MASS_VALUE_MODEL]         = "imagequality/getimagequalitymassvalue";
    m_restmap[QMLEnums::MISCELLANEOUS_MODEL]                    = "scanner/operatorpresent";
    m_restmap[QMLEnums::SCANNERTEMPHUMIDITY_MODEL]              = "scanner/temphumidity";
    m_restmap[QMLEnums::SCANNERCALIBRATION_MODEL]               = "scanner/calibration";
    m_restmap[QMLEnums::SCANNERBAGPROCESSEDMODEL]               = "baglifecycle";
    m_restmap[QMLEnums::PARAMETER_MODEL]                        = "workstationparam";
    m_restmap[QMLEnums::PARAMETER_SEND_MODEL]                   = "workstationparam";
    m_restmap[QMLEnums::PARAMETER_XSD_MODEL]                    = "workstationparam/validation";
    m_restmap[QMLEnums::ERROR_LOG_SEARCH_MODEL]                 = "errorlog/errorlogfile";
    m_restmap[QMLEnums::COMMAND_RADIATIONSURVEY_MODEL]          = "scanner/radiationsurvey";
    m_restmap[QMLEnums::COMMAND_RADIATIONSURVEY_CANCEL_MODEL]   = "scanner/radiationsurvey";
    m_restmap[QMLEnums::COMMAND_IMAGEQUALITY_TEST_MODEL]        = "imagequality/getimagequalitystatus";
    m_restmap[QMLEnums::COMMAND_IMAGEQUALITY_CANCEL_MODEL]      = "imagequality/getimagequalitystatus";
    m_restmap[QMLEnums::COMMAND_MOTIONCONTROLCONVEYOR_MODEL]    = "motioncontrol/conveyor";
    m_restmap[QMLEnums::COMMAND_MOTIONCONTROLGANTRY_MODEL]      = "motioncontrol/gantry";
    m_restmap[QMLEnums::COMMAND_SCANFLUSHCONTROL_MODEL]         = "scanner/scanflushcontrol";
    m_restmap[QMLEnums::INSTALLED_ALARM_LIB_MODEL]              = "alarmlib/installedLibraries";
    m_restmap[QMLEnums::INSTALLABLE_ALARM_LIB_MODEL]            = "alarmlib/installableLibraries";
    m_restmap[QMLEnums::COMMAND_INSTALL_ALARM_LIB]              = "alarmlib/installalramlib";
}

/*!
 * @fn       initRestContentMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest contentType mapping
 */
void ModelManagerScannerAdmin::initRestContentMap()
{
    TRACE_LOG("");
    m_restContentMap[QMLEnums::UPDATE_TASK_STATUS_MODEL]               = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::AUTHENTICATIN_MODEL]                    = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::SDSW_HEARTBEAT_MODEL]                   = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::KEYCONSOLE_MODEL]                       = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::ACCESSPANEL_MODEL]                      = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::ESTOPS_MODEL]                           = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::SAFETYCONTROLLERFAULTS_MODEL]           = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::POWERLINKINVERTERSTATUS_MODEL]          = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::POWERLINKINVERTERDISCRETESTATUS_MODEL]  = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::POWERLINKINVERTERSERIALSTATUS_MODEL]    = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::POWERLINKINVERTERSERIALFAULTS_MODEL]    = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::POWERLINKRECTIFIERSTATUS_MODEL]         = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::CONVEYORANDGANTRY_MODEL]                = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::RCBGANTRYSTATUS_MODEL]                  = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::BHSSTATUS_MODEL]                        = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::MOTIONCONTROLDATA_MODEL]                = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::XRAYSTATUS_MODEL]                       = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::XRAYTIME_MODEL]                         = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::XRAYFAULTS_MODEL]                       = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::XRAYDATA_MODEL]                         = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::XRAYCOMMANDSTATUS_MODEL]                = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::SCANNERTEMPHUMIDITY_MODEL]              = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::IMAGE_QUALITY_MASS_VALUE_MODEL]         = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::MISCELLANEOUS_MODEL]                    = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::SCANNERCALIBRATION_MODEL]               = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::SCANNERBAGPROCESSEDMODEL]               = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::PARAMETER_MODEL]                        = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::PARAMETER_SEND_MODEL]                   = XML_CONTENT_TYPE;
    m_restContentMap[QMLEnums::PARAMETER_XSD_MODEL]                    = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::ERROR_LOG_SEARCH_MODEL]                 = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::COMMAND_RADIATIONSURVEY_MODEL]          = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::COMMAND_RADIATIONSURVEY_CANCEL_MODEL]   = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::COMMAND_IMAGEQUALITY_TEST_MODEL]        = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::COMMAND_IMAGEQUALITY_CANCEL_MODEL]      = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::COMMAND_MOTIONCONTROLCONVEYOR_MODEL]    = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::COMMAND_MOTIONCONTROLGANTRY_MODEL]      = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::COMMAND_SCANFLUSHCONTROL_MODEL]         = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::INSTALLED_ALARM_LIB_MODEL]              = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::INSTALLABLE_ALARM_LIB_MODEL]            = DEFAULT_CONTENT_TYPE;
    m_restContentMap[QMLEnums::COMMAND_INSTALL_ALARM_LIB]              = DEFAULT_CONTENT_TYPE;
}

/*!
 * @fn       onUpdateModels
 * @param    QVariantList
 * @param    QMLEnums::ModelEnum model name
 * @return   void
 * @brief    This slot will call on data updation from rest.
 */
void ModelManagerScannerAdmin::onUpdateModels(QVariantList list, QMLEnums::ModelEnum  modelname)
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
void ModelManagerScannerAdmin::ontaskComplete(QString sDownloadpath , QString sDownloadError,
                                              QMLEnums::ModelEnum modelnum)
{
    DEBUG_LOG("Send filedownload finished signal for modelnum: "<< modelnum
              << " download path: "<< sDownloadpath.toStdString() << " download error: "<< sDownloadError.toStdString());
    if (sDownloadpath.isEmpty())
    {
        return;
    }
    QString sDestinationPath = createDirAndMoveFile(sDownloadpath, modelnum);
    if (modelnum == QMLEnums::PARAMETER_MODEL || modelnum == QMLEnums::PARAMETER_XSD_MODEL)
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
            QString errMsg;
            QVariantList list;

            if (!(XmlService::getXmlAndXsdContent(m_configXMLFile, m_configXSDFile,
                                                  m_configXMLData, m_configXSDData, errMsg)))
            {
                emit authstatuschanged(Errors::E_FAIL, errMsg, QMLEnums::PARAMETER_MODEL);
            }
            else
            {
                if (!(XmlService::validateAndParseXml(m_configXMLData, m_configXSDData, list, errMsg)))
                {
                    emit authstatuschanged(Errors::E_FAIL, errMsg, QMLEnums::PARAMETER_MODEL);
                }
                else
                {
                    emit updateModels(list, QMLEnums::PARAMETER_MODEL);
                }
            }
            m_XmlReceived = false;
            m_XsdReceived = false;
            emit fileDatadownloadFinished( m_configXMLFile, sDownloadError, QMLEnums::PARAMETER_MODEL);
            emit fileDatadownloadFinished( m_configXSDFile, sDownloadError, QMLEnums::PARAMETER_XSD_MODEL);
        }
    }
    else
    {
        emit fileDatadownloadFinished(sDestinationPath, sDownloadError, modelnum);
    }
}

/*!
 * @fn       onScreenModelChanged(ModelList oModels)
 * @param    ModelList
 * @return   None
 * @brief    slot call on screen model changed
 */
void ModelManagerScannerAdmin::onScreenModelChanged(ModelList oModels)
{
    DEBUG_LOG("Update screen model list where list count is: "<<oModels.count());
    m_scrmodels = oModels;
}
}  // end of namespace ws
}  // end of namespace analogic
