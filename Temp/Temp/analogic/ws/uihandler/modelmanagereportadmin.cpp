/*!
* @file     modelmanagereportadmin.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view for Report Admin.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/taskmanager.h>
#include <analogic/ws/uihandler/commandmodel.h>
#include <analogic/ws/uihandler/authenticationmodel.h>
#include <analogic/ws/uihandler/modelmanagereportadmin.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/accesslayer/reportadminaccessinterface.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       ModelManagerReportAdmin
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class ModelManagerReportAdmin.
*/
ModelManagerReportAdmin::ModelManagerReportAdmin(QObject* parent):
  ModelManager(parent),
  m_rTaskStatusManager(NULL),
  m_rrestmap()
{
  m_modelEnumOffset = QMLEnums::USERADMIN_MODELS_END + 1;
}

/*!
* @fn       ~ModelManagerReportAdmin
* @param    None
* @return   None
* @brief    Destructor for class ModelManagerReportAdmin.
*/
ModelManagerReportAdmin::~ModelManagerReportAdmin()
{
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManagerReportAdmin::init()
{
  TRACE_LOG("");
  ModelManager::init();
  DEBUG_LOG("Getting handle to Report admin Access interface");
  ReportAdminAccessInterface* raal = ReportAdminAccessInterface::getInstance();
  DEBUG_LOG("Setting RestProtocol to: " << "http://" << " Report admin server ip address to: "
            << WorkstationConfig::getInstance()->getreportadminseveripaddress().toStdString()
            << " Portno to: "<< WorkstationConfig::getInstance()->getreportadminserverportno().toStdString()
            << " ServerName to: /reportadminservice/");
  raal->setRestProtocol(DEFAULT_REST_PROTOCOL);
  raal->setipadress(WorkstationConfig::getInstance()->getreportadminseveripaddress());
  raal->setportno(WorkstationConfig::getInstance()->getreportadminserverportno());
  raal->setpassphrase(WorkstationConfig::getInstance()->getpassphrase());
  raal->setServerName(DEFAULT_REPORT_SERVERNAME);

  // creating new qtimer instance

  initRestMap();
  initRestContentMap();

  DEBUG_LOG(" Initializing modelupdater for report admin model");
  m_modelsUpdater = new ModelUpdater*[QMLEnums::REPORT_ADMIN_MODELS_END - m_modelEnumOffset];
  THROW_IF_FAILED((m_modelsUpdater == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  DEBUG_LOG(" Initializing modelupdater for report admin authentication model");
  m_modelsUpdater[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset]
      = new Authenticationmodel(QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL,
                                m_rrestmap[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL],
      m_rrestContentMap[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL]);

  THROW_IF_FAILED(
        (m_modelsUpdater[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset] == NULL)?
        Errors::E_OUTOFMEMORY : Errors::S_OK);
  connect(m_modelsUpdater[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset],
      SIGNAL(authstatuschanged(int, QString, int)),
      this,
      SLOT(onauthstatuschanged(int, QString, int)));
#ifdef WORKSTATION
  connect(m_modelsUpdater[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset], SIGNAL
          (sslerrorreceived(int, QString, int )), this, SLOT(onsslerrorreceived(int, QString, int)));
 #endif

  m_rTaskStatusManager = new TaskManager*[MAX_REPORT_PARALLEL_TASK];
  THROW_IF_FAILED((m_rTaskStatusManager == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);


  for (int i = 0; i < MAX_REPORT_PARALLEL_TASK; i++)
  {
    DEBUG_LOG("Creating instance of task manager");
    m_rTaskStatusManager[i] = new TaskManager(
          WorkstationConfig::getInstance()->getScannerAdminAsyncTaskRefreshInterval(),
          WorkstationConfig::getInstance()->getScannerAdminAsyncTaskTimeoutInterval(),
          QMLEnums::UPDATE_REPORT_TASK_STATUS_MODEL
          );

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

  for(int j = QMLEnums::COMMAND_REPORT_MODEL; j < QMLEnums::REPORT_ADMIN_MODELS_END; j++)
  {
    DEBUG_LOG("Creating CommandModel for modelnum: "<< j <<
              " with resturi: "<< m_rrestmap[QMLEnums::ModelEnum(j)].toStdString());

    if((j == QMLEnums::COMMAND_REPORT_SCANNER_MODEL) || (j == QMLEnums::COMMAND_REPORT_OPERATOR_MODEL))
    {
      m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] = new ModelUpdater(
            QMLEnums::ModelEnum(j), m_rrestmap[QMLEnums::ModelEnum(j)],
          m_rrestContentMap[QMLEnums::ModelEnum(j)]);
      THROW_IF_FAILED((m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] == NULL) ?
          Errors::E_OUTOFMEMORY : Errors::S_OK);
    }
    else
    {
      m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] = new CommandModel(
            QMLEnums::ModelEnum(j), m_rrestmap[QMLEnums::ModelEnum(j)],
          m_rrestContentMap[QMLEnums::ModelEnum(j)]);
      THROW_IF_FAILED((m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] == NULL) ?
            Errors::E_OUTOFMEMORY : Errors::S_OK);
    }
  }

  for(int i = QMLEnums::COMMAND_REPORT_MODEL; i < QMLEnums::REPORT_ADMIN_MODELS_END; i++)
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

      DEBUG_LOG("Connecting taskinfo signal and its handler for modelnum: " << i);
      connect(m_modelsUpdater[i-m_modelEnumOffset], SIGNAL(taskInfoReceived(QVariantList, QMLEnums::ModelEnum)),
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
void ModelManagerReportAdmin::onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data)
{
  ModelManager::onCommandButtonEvent(modelname, data);
}

/*!
 * @fn      onExit
 * @param   None
 * @return  None
 * @brief Function will call on exit AUTHENTICATIN_MODELof threads
 */
void ModelManagerReportAdmin::onExit()
{
  TRACE_LOG("");
  DEBUG_LOG("Calling exit on ModelManager");
  ModelManager::onExit();
  DEBUG_LOG("Destroying modelupdater array");
  for (int i = QMLEnums::COMMAND_REPORT_MODEL; i < QMLEnums::REPORT_ADMIN_MODELS_END; i++){
    SAFE_DELETE( m_modelsUpdater[QMLEnums::ModelEnum(i-m_modelEnumOffset)]);
  }
  SAFE_DELETE( m_modelsUpdater);

  DEBUG_LOG("Destroying TaskManager array");
  for(int i = 0; i < MAX_REPORT_PARALLEL_TASK; i++)
  {
    SAFE_DELETE( m_rTaskStatusManager[i]);
  }
  SAFE_DELETE( m_rTaskStatusManager);
  DEBUG_LOG("Destroying Report Admin Access Interface");
  ReportAdminAccessInterface::destroyInstance();
}

/*!
* @fn       onauthstatuschanged();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelnum
* @return   void
* @brief    Slot will call when authentication status changed
*/
void ModelManagerReportAdmin::onauthstatuschanged(int hr, QString status, int modelnum )
{
  emit authstatuschanged(hr, status, modelnum);
}
#ifdef WORKSTATION
void ModelManagerReportAdmin::onsslerrorreceived(int hr, QString status,int modelnum )
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
void ModelManagerReportAdmin::onTaskTimeout()
{
  m_modelsUpdater[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL - m_modelEnumOffset]->postrequest("", "");
}

/*!
 * @fn       initRestMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest mapping
 */
void ModelManagerReportAdmin::initRestMap()
{
  TRACE_LOG("");
  m_rrestmap[QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL]             = "credentials";
  m_rrestmap[QMLEnums::UPDATE_REPORT_TASK_STATUS_MODEL]               = "taskinfo";
  m_rrestmap[QMLEnums::COMMAND_REPORT_MODEL]                          = "report";
  m_rrestmap[QMLEnums::COMMAND_REPORT_SCANNER_MODEL]                  = "scannerid";
  m_rrestmap[QMLEnums::COMMAND_REPORT_OPERATOR_MODEL]                 = "operatorid";
}

/*!
 * @fn       initRestContentMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest contentType mapping
 */
void ModelManagerReportAdmin::initRestContentMap()
{
  TRACE_LOG("");
  for (int i = QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL; i < QMLEnums::REPORT_ADMIN_MODELS_END; i++)
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
void ModelManagerReportAdmin::onUpdateModels(QVariantList list, QMLEnums::ModelEnum  modelname)
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
void ModelManagerReportAdmin::ontaskComplete(QString sDownloadpath , QString sDownloadError,
                                             QMLEnums::ModelEnum modelnum)
{
  DEBUG_LOG("Send filedownload finished signal for modelnum: "<< modelnum
            << " download path: "<< sDownloadpath.toStdString() << " download error: "<< sDownloadError.toStdString());
  QString sDestinationPath = createDirAndMoveFile(sDownloadpath, modelnum);
  emit fileDatadownloadFinished(sDestinationPath, sDownloadError, modelnum);
}
}  // end of namespace ws
}  // end of namespace analogic

