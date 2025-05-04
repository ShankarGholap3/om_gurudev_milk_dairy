#include <analogic/ws/common/accesslayer/supervisoraccessinterface.h>
#include <analogic/ws/uihandler/commandmodel.h>
#include <analogic/ws/uihandler/taskmanager.h>
#include <analogic/ws/uihandler/authenticationmodel.h>
#include <analogic/ws/uihandler/modelmanagersupervisor.h>
#include <analogic/ws/uihandler/modelupdater.h>
//For supervisor workstation - to add data into charts
#include <QtCharts/QXYSeries>
#include <QDebug>
#include <QDateTime>

namespace analogic
{
namespace ws
{
ModelManagerSupervisor::ModelManagerSupervisor(QObject* parent): ModelManager(parent),
  m_pUpdateSlowTimer(NULL),
  m_pUpdateFastTimer(NULL),
  m_scrmodels(),
  m_restmap()
{
  m_modelEnumOffset = QMLEnums::ANTIVIRUS_MODELS_END + 1;
  m_chartsAverageDataList.insert(0,0);
  m_chartsAverageDataList.insert(0,0);
  m_chartsAverageDataList.insert(0,0);
  m_chartsAverageDataList.insert(0,0);
  m_supervisorScannerUtilization = 0.0f;
  m_supervisorPvsUtilization = 0.0f;
  m_supervisorSvsUtilization = 0.0f;
  m_currentGroupUuid = "";
  m_currentAgentUuid = "";
  m_selectedGroup = "";

}
ModelManagerSupervisor::~ModelManagerSupervisor()
{
}

void ModelManagerSupervisor::onExit()
{

  TRACE_LOG("");
  DEBUG_LOG("Calling exit on ModelManager");
  ModelManager::onExit();
  DEBUG_LOG("Destroying modelupdater array");
  for (int i = QMLEnums::AUTHENTICATIN_MODEL; i < QMLEnums::SCANNERADMIN_MODELS_END; i++){
    SAFE_DELETE( m_modelsUpdater[QMLEnums::ModelEnum(i)]);
  }
  SAFE_DELETE( m_modelsUpdater);

  m_pUpdateSlowTimer->stop();
  m_pUpdateSlowTimer.reset();
  m_pUpdateFastTimer->stop();
  m_pUpdateFastTimer.reset();

  DEBUG_LOG("Destroying Supervisor Access Interface");
  SupervisorAccessInterface::destroyInstance();
  DEBUG_LOG("Destroying Group data Model");
  GroupDataModel::destroyGroupDataModelInstance();
  m_groupDataModel = nullptr;
  DEBUG_LOG("Destroying Supervisor Agents Data Model");
  SupervisorAgentsDataModel::destroySupervisorAgentsDataModelInstance();
  m_agentsDataModel = nullptr;
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManagerSupervisor::init()
{
  TRACE_LOG("");
  DEBUG_LOG("Initializing modelmanager");
  ModelManager::init();

  // creating new qtimer instance
  m_pUpdateSlowTimer.reset(new QTimer(this));
  m_pUpdateFastTimer.reset(new QTimer(this));

  // setting timer interval
  m_pUpdateSlowTimer->setInterval(SLOW_TIMER);
  m_pUpdateFastTimer->setInterval(FAST_TIMER);

  // connecting timer events to proper slot
  connect(m_pUpdateSlowTimer.get(), SIGNAL(timeout()), this,
          SLOT(apiRequestSlow()) ,
          Qt::QueuedConnection);

  connect(m_pUpdateFastTimer.get(), SIGNAL(timeout()), this,
          SLOT(apiRequestFast()) ,
          Qt::QueuedConnection);
  DEBUG_LOG("Initializing rest map");

  DEBUG_LOG("Start model update timer");

  DEBUG_LOG("Getting ChartAccessInterface handle");
  SupervisorAccessInterface* pSupervisorAccessInterface = SupervisorAccessInterface::getInstance();
  THROW_IF_FAILED(((pSupervisorAccessInterface == nullptr) ? Errors::E_POINTER : Errors::S_OK));

  DEBUG_LOG("Setting SupervisorService IP address to: "<<WorkstationConfig::getInstance()->getSupervisorSeverIPAddress().toStdString()
            <<" portno to: "<< WorkstationConfig::getInstance()->getSupervisorSeverPortNo().toStdString()
            <<" Pass phrase to: "<< WorkstationConfig::getInstance()->getpassphrase().toStdString());
  pSupervisorAccessInterface->setRestProtocol(DEFAULT_REST_PROTOCOL);
  pSupervisorAccessInterface->setipadress(WorkstationConfig::getInstance()->getSupervisorSeverIPAddress());
  pSupervisorAccessInterface->setportno(WorkstationConfig::getInstance()->getSupervisorSeverPortNo());
  pSupervisorAccessInterface->setpassphrase(WorkstationConfig::getInstance()->getpassphrase());
  pSupervisorAccessInterface->setServerName(DEFAULT_SUPERVISOR_SERVERNAME);

  initRestMap();
  initRestContentMap();

  DEBUG_LOG("Creating instance for handling array of models");
  m_modelsUpdater = new ModelUpdater*[QMLEnums::SUPERVISOR_MODEL_END -m_modelEnumOffset];
  THROW_IF_FAILED((m_modelsUpdater == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  DEBUG_LOG("Creating instance of authentication model");
  m_modelsUpdater[QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL-m_modelEnumOffset] = new Authenticationmodel(
        QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL, m_restmap[QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL],
      m_restContentMap[QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL]);

  THROW_IF_FAILED(
        (m_modelsUpdater[QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL - m_modelEnumOffset] == NULL)?
        Errors::E_OUTOFMEMORY : Errors::S_OK);

  connect(m_modelsUpdater[QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL - m_modelEnumOffset],
      SIGNAL(authstatuschanged(int, QString, int)),
      this,
      SLOT(onauthstatuschanged(int, QString, int)));

#ifdef WORKSTATION
  connect(m_modelsUpdater[QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL - m_modelEnumOffset], SIGNAL
      (sslerrorreceived(int, QString, int )), this, SLOT(onsslerrorreceived(int, QString, int)));
#endif

  for(int j = QMLEnums::BAGS_PER_HOUR; j < QMLEnums::SUPERVISOR_MODEL_END; j++)
  {
    DEBUG_LOG("Creating CommandModel for modelnum: "<< convertModelEnumToString(j).toStdString() <<
              " with resturi: "<< m_restmap[QMLEnums::ModelEnum(j)].toStdString());

    m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] = new CommandModel(
          QMLEnums::ModelEnum(j), m_restmap[QMLEnums::ModelEnum(j)],
        m_restContentMap[QMLEnums::ModelEnum(j)]);
    THROW_IF_FAILED((m_modelsUpdater[QMLEnums::ModelEnum(j)-m_modelEnumOffset] == NULL) ?
          Errors::E_OUTOFMEMORY : Errors::S_OK);
  }
  for(int i = QMLEnums::BAGS_PER_HOUR; i < QMLEnums::SUPERVISOR_MODEL_END; i++)
  {
    DEBUG_LOG("Connecting modelupdater signal with its handler. modelnum is: " << convertModelEnumToString(i).toStdString());
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

  m_agentsDataModel = SupervisorAgentsDataModel::getSupervisorAgentsDataModelInstance ();
  connect(this, SIGNAL(agentsDataReceived(QVector<SupervisorAgents>)),
          m_agentsDataModel, SLOT(onAgentsDataReceived(QVector<SupervisorAgents>)));

  m_supervisorRecentEventsmodel = SupervisorEventsDataModel::getSupervisorEventsDataModelInstance();

  connect(this, SIGNAL(recentEventsDataReceived(QVector<SupervisorRecentEvents>)),
          m_supervisorRecentEventsmodel, SLOT(onRecenteventsdatarecieved(QVector<SupervisorRecentEvents>)));

  m_groupDataModel = GroupDataModel::getGroupDataModelInstance();
  connect(this, SIGNAL(groupDataReceived(QVector<SupervisorGroups>)),
          m_groupDataModel, SLOT(onGroupDataReceived(QVector<SupervisorGroups>)));

  connect(m_agentsDataModel, SIGNAL(currentAgentChanged(QString)),
          this, SLOT(onCurrentAgentChanged(QString)));
}
/*!
* @fn       onauthstatuschanged();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelnum
* @return   void
* @brief    Slot will call when authentication status changed
*/
void ModelManagerSupervisor::onauthstatuschanged(int hr, QString status, int modelnum )
{

  emit authstatuschanged(hr, status, modelnum);

  if(hr == 1)
  {
    resetAllModels();
  }
  else if((hr == 200 || hr == 201) && (modelnum >= QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL && modelnum < QMLEnums::SUPERVISOR_MODEL_END))
  {
    //Retriving data from JSON response
    QString strReplyData = status;
    strReplyData = QString ("{\"data\":") +strReplyData + "}";
    //Parsing JSON response to QVariantMap
    QJsonDocument jsondoc = QJsonDocument::fromJson(strReplyData.toUtf8());
    QJsonObject jobj =  jsondoc.object();
    QVariantMap mapobj =  jobj.toVariantMap();
    QVariantList listobj;
    if ( mapobj["data"].type() == QVariant::List)
    {
      listobj = mapobj["data"].toList();
    }
    else if ( mapobj["data"].type() != NULL)
    {
      listobj = mapobj.values();
    }

    if (!listobj.empty())
    {
      updateResponse(listobj, modelnum);
    }

  }

}

/*!
* @fn      updateChartsCordinates
* @param   QAbstractSeries
* @return  None
* @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
*/
void ModelManagerSupervisor::onUpdateChartsCordinates(QAbstractSeries *series, int modelnum)
{
  m_lineChartSeriesMap[modelnum] = series;
}

#ifdef WORKSTATION
void ModelManagerSupervisor::onsslerrorreceived(int hr, QString status,int modelnum )
{
  emit sslerrorreceived(hr, status,modelnum);
}
#endif
/*!
 * @fn       onScreenModelChanged(ModelList oModels)
 * @param    ModelList
 * @return   None
 * @brief    slot call on screen model changed
 */
void ModelManagerSupervisor::onScreenModelChanged(ModelList oModels)
{
  DEBUG_LOG("Update screen model list where list count is: "<<oModels.count());
  m_scrmodels = oModels;
}
/*!
* @fn       onprocessUpdateModels()
* @param    None
* @return   None
* @brief    timer thread that run every configurable interval.
*/
void ModelManagerSupervisor::onProcessUpdateModels()
{

  DEBUG_LOG("Update all models");
  //m_modelsUpdater[QMLEnums::RECENT_SEARCH_LIST_LENGHT-m_modelEnumOffset]->getrequest();
  apiRequestOnEvent(QMLEnums::SUPERVISOR_GROUPS);
}
/*!
* @fn       apiRequestSlow
* @param    None
* @return   None
* @brief    timer thread that run every configurable interval (Currently 1 minute).
*/
void ModelManagerSupervisor::apiRequestSlow()
{
  if(m_agentsDataModel != nullptr)
  {
    apiRequestOnEvent(QMLEnums::BAGS_PER_HOUR);
    apiRequestOnEvent(QMLEnums::SUSPECT_RATE_PER_HOUR);
    apiRequestOnEvent(QMLEnums::RECENT_OPERATOR_DECISION_TIME);
    apiRequestOnEvent(QMLEnums::RECENT_TIME_TO_DIVERT);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SCANNER_UTILIZATION, RequestType::GROUP);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_PVS_UTILIZATION, RequestType::GROUP);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SVS_UTILIZATION, RequestType::GROUP);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SCANNER_UTILIZATION, RequestType::AGENT);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_PVS_UTILIZATION, RequestType::AGENT);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SVS_UTILIZATION, RequestType::AGENT);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SCANNER_DETAIL);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_PVS_DETAIL);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SVS_DETAIL);
    emit dataTimerTick();
  }
}

/*!
* @fn       apiRequestFast
* @param    None
* @return   None
* @brief    timer thread that run every configurable interval (Currently 15 second).
*/
void ModelManagerSupervisor::apiRequestFast()
{
  if(m_agentsDataModel != nullptr)
  {
    apiRequestOnEvent(QMLEnums::SUPERVISOR_GROUPS);
    apiRequestOnEvent(QMLEnums::RECENT_EVENTS);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_AGENTS);
    emit dataTimerTick();
  }
}
/*!
 * @fn      apiRequestOnEvent
 * @param   QMLEnum
 * @return  None
 * @brief   to call api request on event
 */
void ModelManagerSupervisor::apiRequestOnEvent(QMLEnums::ModelEnum modelname, RequestType requestType)
{
  DEBUG_LOG("Sending request for " << convertModelEnumToString(modelname).toStdString() << " with " << QString::number(requestType).toStdString() << " with group uuid " << m_currentGroupUuid.toStdString() << " and agent uuid " << m_currentAgentUuid.toStdString());
  switch (modelname) {
  case QMLEnums::BAGS_PER_HOUR:
  {
    m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?group=" + m_currentGroupUuid + "&agent=" + m_currentAgentUuid));
    break;
  }
  case QMLEnums::SUSPECT_RATE_PER_HOUR :
  {
    m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?group=" + m_currentGroupUuid + "&agent=" + m_currentAgentUuid));
    break;
  }
  case QMLEnums::RECENT_TIME_TO_DIVERT:
  {
    m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?group=" + m_currentGroupUuid + "&agent=" + m_currentAgentUuid));
    break;
  }
  case QMLEnums::RECENT_OPERATOR_DECISION_TIME:
  {
    m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?group=" + m_currentGroupUuid + "&agent=" + m_currentAgentUuid));
    break;
  }
  case QMLEnums::SUPERVISOR_SCANNER_UTILIZATION:
  {
    if((requestType == RequestType::GROUP) && (m_currentGroupUuid != ""))
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?group=" + m_currentGroupUuid));
    }
    else if((requestType == RequestType::AGENT) && (m_currentAgentUuid != ""))
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?agent=" + m_currentAgentUuid));
    }
    break;
  }
  case QMLEnums::SUPERVISOR_PVS_UTILIZATION:
  {
    if((requestType == RequestType::GROUP) && (m_currentGroupUuid != ""))
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?group=" + m_currentGroupUuid));
    }
    else if((requestType == RequestType::AGENT) && (m_currentAgentUuid != ""))
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?agent=" + m_currentAgentUuid));
    }
    break;
  }
  case QMLEnums::SUPERVISOR_SVS_UTILIZATION:
  {
    if((requestType == RequestType::GROUP) && (m_currentGroupUuid != ""))
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?group=" + m_currentGroupUuid));
    }
    if((requestType == RequestType::AGENT) && (m_currentAgentUuid != ""))
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?agent=" + m_currentAgentUuid));
    }
    break;
  }
  case QMLEnums::SUPERVISOR_SCANNER_DETAIL:
  {
    if(m_currentAgentUuid != "")
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?agent=" + m_currentAgentUuid));
    }
    break;
  }
  case QMLEnums::SUPERVISOR_PVS_DETAIL:
  {
    if(m_currentAgentUuid != "")
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?agent=" + m_currentAgentUuid));
    }
    break;
  }
  case QMLEnums::SUPERVISOR_SVS_DETAIL:
  {
    if(m_currentAgentUuid != "")
    {
      m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?agent=" + m_currentAgentUuid));
    }
    break;
  }
  case QMLEnums::RECENT_EVENTS:
  {
    m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?group=" + m_currentGroupUuid));
    break;
  }
  case QMLEnums::SUPERVISOR_GROUPS:
  {

    m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest();
    break;
  }
  case QMLEnums::SUPERVISOR_AGENTS:
  {
    m_modelsUpdater[modelname-m_modelEnumOffset]->getrequest((m_restmap[modelname] + "?type=" + m_currentAgentsType + "&registered=" + m_agentsDataModel->getCurrentAgentRegistered() + "&group=" + m_currentGroupUuid));
    break;
  }
  default:
    break;
  }
}
/*!
 * @fn      onCurrentAgentChanged
 * @param    None
 * @return  None
 * @brief   to call api on agent change
 */
void ModelManagerSupervisor::onCurrentAgentChanged(QString agentUuid)
{
  if((m_currentAgentUuid == "") && (agentUuid != ""))
  {
    apiRequestOnEvent(QMLEnums::BAGS_PER_HOUR);
    apiRequestOnEvent(QMLEnums::SUSPECT_RATE_PER_HOUR);
    apiRequestOnEvent(QMLEnums::RECENT_OPERATOR_DECISION_TIME);
    apiRequestOnEvent(QMLEnums::RECENT_TIME_TO_DIVERT);
  }
  if(m_currentAgentUuid != agentUuid)
  {
    m_currentAgentUuid = agentUuid;
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SCANNER_UTILIZATION, RequestType::AGENT);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_PVS_UTILIZATION, RequestType::AGENT);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SVS_UTILIZATION, RequestType::AGENT);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SCANNER_DETAIL);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_PVS_DETAIL);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SVS_DETAIL);
  }
}

/*!
* @fn       onCommandButtonEvent
* @param    QMLEnums::ModelEnum  - modelname
* @param    QString data to be post
* @return   void
* @brief    This slot will call on command button clicked, this is done for inital authentication postrequest which is done from ModelManager::onCommandButtonEvent
*              This  is called from ModelManagerS::onCommandButtonEvent
*/
void ModelManagerSupervisor::onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data)
{
  ModelManager::onCommandButtonEvent(modelname,data);
}

/*!
* @fn       onTaskTimeout
* @param    None
* @return   None
* @brief    slots check if task has got timeout.
*/
void ModelManagerSupervisor::onTaskTimeout()
{
  m_modelsUpdater[QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL - m_modelEnumOffset]->postrequest("","");
}

/*!
 * @fn       initRestMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest mapping
 *
 */
void ModelManagerSupervisor::initRestMap()
{
  TRACE_LOG("");
  m_restmap[QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL]          = "credentials";
  m_restmap[QMLEnums::BAGS_PER_HOUR]                            = "recent_bag_scan_rate";
  m_restmap[QMLEnums::SUSPECT_RATE_PER_HOUR]                    = "recent_operator_suspect_rate";
  m_restmap[QMLEnums::RECENT_TIME_TO_DIVERT]                    = "recent_time_to_divert";
  m_restmap[QMLEnums::SUPERVISOR_AGENTS]                        = "agents";
  m_restmap[QMLEnums::SUPERVISOR_PINNED_EVENTS]                 = "pinned_events";
  m_restmap[QMLEnums::SUPERVISOR_GROUP_OVERVIEW]                = "group_overview";
  m_restmap[QMLEnums::SUPERVISOR_API_VERSION]                   = "api_version";
  m_restmap[QMLEnums::RECENT_SEARCH_LIST_LENGHT]                = "recent_search_list_length";
  m_restmap[QMLEnums::RECENT_EVENTS]                            = "recent_events";
  m_restmap[QMLEnums::SUPERVISOR_GROUPS]                        = "groups";
  m_restmap[QMLEnums::RECENT_OPERATOR_DECISION_TIME]            = "recent_operator_decision_time";
  m_restmap[QMLEnums::SUPERVISOR_SCANNER_UTILIZATION]           = "scanner_utilization";
  m_restmap[QMLEnums::SUPERVISOR_PVS_UTILIZATION]               = "pvs_utilization";
  m_restmap[QMLEnums::SUPERVISOR_SVS_UTILIZATION]               = "svs_utilization";
  m_restmap[QMLEnums::SUPERVISOR_SCANNER_DETAIL]                = "scanner_detail";
  m_restmap[QMLEnums::SUPERVISOR_PVS_DETAIL]                    = "pvs_detail";
  m_restmap[QMLEnums::SUPERVISOR_SVS_DETAIL]                    = "svs_detail";
}

/*!
 * @fn       initRestContentMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest contentType mapping
 */
void ModelManagerSupervisor::initRestContentMap()
{
  TRACE_LOG("");
  for (int i = QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL ; i < QMLEnums::SUPERVISOR_MODEL_END ; i++)
  {
    m_restContentMap[QMLEnums::ModelEnum(i)] = DEFAULT_CONTENT_TYPE;
  }
}

/*!
 * @fn       onUpdateModels
 * @param    QVariantList
 * @param    QMLEnums::ModelEnum model name
 * @return   void
 * @brief    This slot will call on data updation from rest.
 */
void ModelManagerSupervisor::onUpdateModels(QVariantList list, QMLEnums::ModelEnum  modelname)
{
  DEBUG_LOG("Send update signal for modelname: "<< modelname << " with list count: "<< list.count());
  emit updateModels(list, modelname);
}
/*!
 * @fn       updateResponse();
 * @param    QVariantMap - listOfDataForChart
 * @param    int - modelnum
 * @return   None
 * @brief    Stored cordinates in Vector to display it on charts
 */
void ModelManagerSupervisor::updateResponse(QVariantList listobj, int modelnum)
{
  if(!listobj.isEmpty())
  {
    QVariantMap listOfDataForChart = listobj.at(0).toMap();
    QStringList key_list = listOfDataForChart.keys();
    QVariantList yCoordinates;
    QString key;
    QString startTimeKey;
    QString endTimeKey;
    quint64 dtEpc;
    QVector<QPointF> sChartCoordinateVec;
    QPair<QDateTime, QDateTime> chartTimePairs;
    float sumRecords = 0;
    unsigned int intervalInSecond = 0;

    if( modelnum == QMLEnums::SUPERVISOR_AGENTS )
    {
      updateSupervisorAgentsInfo(listobj , modelnum);
    }
    else if(modelnum == QMLEnums::SUPERVISOR_GROUP_OVERVIEW)
    {
      updateSupervisorGroupOverviewInfo(listobj);
    }
    else if(modelnum == QMLEnums::RECENT_EVENTS)
    {
      updateRecentEventInfo(listobj);
    }
    else if(modelnum == QMLEnums::SUPERVISOR_GROUPS)
    {
      updateSupervisorGroupsInfo(listobj);
    }
    else if(modelnum == QMLEnums::SUPERVISOR_SCANNER_UTILIZATION || modelnum == QMLEnums::SUPERVISOR_PVS_UTILIZATION || modelnum == QMLEnums::SUPERVISOR_SVS_UTILIZATION)
    {
      updateAgentsUtilizationInfo(listobj, modelnum);
    }
    else if((modelnum == QMLEnums::SUPERVISOR_SCANNER_DETAIL ) || (modelnum == QMLEnums::SUPERVISOR_PVS_DETAIL) || (modelnum == QMLEnums::SUPERVISOR_SVS_DETAIL ))
    {
      updateAgentsDetailInfo(listobj, modelnum);
    }
    else
    {
      //Retriving data from JSON response i.e status to QVector i.e m_ChartCordinates
      if(modelnum == QMLEnums::SUSPECT_RATE_PER_HOUR || modelnum == QMLEnums::BAGS_PER_HOUR ||
         modelnum == QMLEnums::RECENT_TIME_TO_DIVERT || modelnum == QMLEnums::RECENT_OPERATOR_DECISION_TIME)
      {
        //Get records and startTime as a key
        for(int i = 0; i < key_list.size(); ++i)
        {
          if(key_list.at(i) == CHART_RECORDS_KEY)
          {
            key = key_list.at(i);
          }
          if(key_list.at(i) == CHART_START_TIME_KEY)
          {
            startTimeKey = key_list.at(i);
          }
          if(key_list.at(i) == CHART_END_TIME_KEY)
          {
            endTimeKey = key_list.at(i);
          }
          if(key_list.at(i) == CHART_INTERVAL_KEY)
          {
            intervalInSecond = listOfDataForChart[key_list.at(i).toLocal8Bit()].toUInt();
          }
        }
        yCoordinates = listOfDataForChart[key.toLocal8Bit()].toList ();


        QString startTimeValue = listOfDataForChart[startTimeKey.toLocal8Bit()].toString();
        QString endTimeValue = listOfDataForChart[endTimeKey.toLocal8Bit()].toString();
        QString format = "yyyy-MM-ddTHH:mm:ss.zzzZ";
        QDateTime utcStartTime(QDateTime::fromString (startTimeValue, format));
        utcStartTime.setTimeSpec(Qt::OffsetFromUTC);
        QDateTime utcEndTime(QDateTime::fromString (endTimeValue, format));
        utcEndTime.setTimeSpec(Qt::OffsetFromUTC);

        chartTimePairs.first = utcStartTime;
        chartTimePairs.second = utcEndTime;
        m_chartTimeMap[modelnum] = chartTimePairs;

        //Epoch time is taken as the x-axis
        dtEpc = utcStartTime.toMSecsSinceEpoch();
      }
      if(yCoordinates.size() > 0)
      {
        foreach(QVariant yAxis, yCoordinates)
        {
          //x value is the time
          qreal x = dtEpc;
          //y value is the points that are received from webservice
          qreal y = yAxis.toInt();
          //This is stored in cordinates to display it on charts when its updated on it in DashBoardScreen.qml
          sChartCoordinateVec.append(QPointF(x,y));
          sumRecords += y;

          dtEpc += intervalInSecond*1000;
        }
        if(sChartCoordinateVec.size() > 0)
        {
          //Charts data get updated with the updated coordinates
          if(modelnum == QMLEnums::BAGS_PER_HOUR)
          {
            m_bagsPerHourCordinates = sChartCoordinateVec;
            if(m_chartsAverageDataList.size() > 0)
            {
              m_chartsAverageDataList[0] = qRound(sumRecords/m_bagsPerHourCordinates.size());
            }
          }
          else if(modelnum == QMLEnums::SUSPECT_RATE_PER_HOUR)
          {
            m_suspectRateCordinates = sChartCoordinateVec;
            if(m_chartsAverageDataList.size() > 1)
            {
              m_chartsAverageDataList[1] = qRound(sumRecords/m_suspectRateCordinates.size());
            }
          }
          else if(modelnum == QMLEnums::RECENT_OPERATOR_DECISION_TIME)
          {
            m_recentOperatorTimeCordinates = sChartCoordinateVec;
            if(m_chartsAverageDataList.size() > 2)
            {
              m_chartsAverageDataList[2] = qRound(sumRecords/m_recentOperatorTimeCordinates.size());
            }
          }
          else if(modelnum == QMLEnums::RECENT_TIME_TO_DIVERT)
          {
            m_recentTimeToDivertCordinates = sChartCoordinateVec;
            if(m_chartsAverageDataList.size() > 3)
            {
              m_chartsAverageDataList[3] = qRound(sumRecords/m_recentTimeToDivertCordinates.size());
            }
          }
          displayLineChart(modelnum);
        }
      }
    }
  }
}
/*!
 * @fn       updateSupervisorAgentsInfo();
 * @param    QVariantList
 * @param    int
 * @return   None
 * @brief    Update the data of Agents rest response into SupervisorAgents
 */
void ModelManagerSupervisor::updateSupervisorAgentsInfo(QVariantList listobj, int modelnum)
{
  m_pvsCount = 0 ;
  m_scannerCount = 0;
  m_svsCount = 0;
  m_maxPvsCount = 0;
  m_maxSvsCount = 0;
  m_maxScannerCount = 0;
  SupervisorAgents sAgents;
  QVector<QString> sGroupsData;
  QVector<SupervisorAgents> slocalAgentsData;

  for(int k = 0; k < listobj.size(); ++k)
  {
    QVariantMap listOfBagsPerHr = listobj.at(k).toMap();
    QVariant apiValue;

    for(auto itMap = listOfBagsPerHr.begin(); itMap != listOfBagsPerHr.end(); ++itMap)
    {
      QString key = itMap.key();
      apiValue = itMap.value();

      if(key == AGENTS_GROUPS)
      {
        QVariantList listGrp;
        if ( listOfBagsPerHr["groups"].type() ==QVariant::List)
        {
          listGrp = listOfBagsPerHr["groups"].toList();
        }

        for(int a = 0; a < listGrp.size(); ++a)
        {
          QVariantMap listOfAgentsGroups = listGrp.at(a).toMap();

          QVariant groupValue;
          for(auto itMapGrp = listOfAgentsGroups.begin(); itMapGrp != listOfAgentsGroups.end(); ++itMapGrp)
          {
            QString keyGrp = itMapGrp.key();
            groupValue = itMapGrp.value();
            if(keyGrp == AGENTS_GROUPS_UUID)
            {
              sGroupsData.append(groupValue.toString());
              sAgents.setAgentsGroupUuid(sGroupsData);
            }
          }
        }
      }
      else if(key == AGENTS_TYPE)
      {
        sAgents.setAgentsType(apiValue.toInt());
      }
      else if(key == AGENTS_MACHINE_ID)
      {
        sAgents.setAgentsMachine_id(apiValue.toString());
      }
      else if(key == AGENTS_MANUFACTURER)
      {
        sAgents.setAgentsManufacturer(apiValue.toString());
      }
      else if(key == AGENTS_REGISTERED)
      {
        sAgents.setAgentsRegistered(apiValue.toBool());
      }
      else if(key == AGENTS_READINESS)
      {
        sAgents.setAgentsReadiness(apiValue.toInt());
      }
      else if(key == AGENTS_STATE)
      {
        sAgents.setAgentsState(apiValue.toString());
      }
      else if(key == AGENTS_GROUPS_UUID)
      {
        sAgents.setAgentsUuid(apiValue.toString());
      }
      else if(key == AGENTS_CREDENTIALS)
      {
        sAgents.setAgentsUser(apiValue.toString());
      }
      else if(key == AGENTS_LOCATION)
      {
        sAgents.setAgentsLocation(apiValue.toString());
      }
    }
    slocalAgentsData.append(sAgents);
  }
  updateAgentsInformation(slocalAgentsData);
}

/*!
 * @fn       updateAgentsInformation
 * @param    None
 * @return   None
 * @brief    updateAgentsInformation updates the data in QVector<SupervisorAgents> from agent's detail and utilization rest api
 */
void ModelManagerSupervisor::updateAgentsInformation()
{
  QVector<SupervisorAgents> slocalAgentsData;
  slocalAgentsData.clear();
  updateAgentsInformation(slocalAgentsData);
}

/*!
 * @fn       updateAgentsInformation
 * @param    const QVector<SupervisorAgents> &type, const QVector<SupervisorAgents> &slocalAgentsData
 * @return   None
 * @brief    updateAgentsInformation updates the data in QVector<SupervisorAgents> from agents rest api
 */
void ModelManagerSupervisor::updateAgentsInformation(QVector<SupervisorAgents> &slocalAgentsData)
{
  if(slocalAgentsData.size() <= 0)
  {
    slocalAgentsData = m_supervisorAgents;
  }
  for(int iAgents = 0; iAgents < slocalAgentsData.size(); iAgents++)
  {
    for(int iDetails = 0; iDetails < m_supervisorScannerDetailInfo.size(); iDetails++)
    {
      if(m_supervisorScannerDetailInfo[iDetails].getAgentsUuid() == slocalAgentsData[iAgents].getAgentsUuid())
      {
        slocalAgentsData[iAgents].setAgentsUuid(m_supervisorScannerDetailInfo[iDetails].getAgentsUuid());
        slocalAgentsData[iAgents].setDetailMachine_id(m_supervisorScannerDetailInfo[iDetails].getDetailMachine_id());
        slocalAgentsData[iAgents].setDetailLocation(m_supervisorScannerDetailInfo[iDetails].getDetailLocation());
        slocalAgentsData[iAgents].setAgentsSublocation(m_supervisorScannerDetailInfo[iDetails].getAgentsSublocation());
        slocalAgentsData[iAgents].setOnline_time(m_supervisorScannerDetailInfo[iDetails].getOnline_time());
        slocalAgentsData[iAgents].setUnlink_rate(m_supervisorScannerDetailInfo[iDetails].getUnlink_rate());
        slocalAgentsData[iAgents].setTrays_screened(m_supervisorScannerDetailInfo[iDetails].getTrays_screened());
        slocalAgentsData[iAgents].setTrays_searched(m_supervisorScannerDetailInfo[iDetails].getTrays_searched());
      }
    }
    for(int iDetails = 0; iDetails < m_supervisorPvsDetailInfo.size(); iDetails++)
    {
      if(m_supervisorPvsDetailInfo[iDetails].getAgentsUuid() == slocalAgentsData[iAgents].getAgentsUuid())
      {
        slocalAgentsData[iAgents].setAgentsUuid(m_supervisorPvsDetailInfo[iDetails].getAgentsUuid());
        slocalAgentsData[iAgents].setDetailMachine_id(m_supervisorPvsDetailInfo[iDetails].getDetailMachine_id());
        slocalAgentsData[iAgents].setDetailLocation(m_supervisorPvsDetailInfo[iDetails].getDetailLocation());
        slocalAgentsData[iAgents].setAgentsSublocation(m_supervisorPvsDetailInfo[iDetails].getAgentsSublocation());
        slocalAgentsData[iAgents].setOnline_time(m_supervisorPvsDetailInfo[iDetails].getOnline_time());
        slocalAgentsData[iAgents].setUnlink_rate(m_supervisorPvsDetailInfo[iDetails].getUnlink_rate());
        slocalAgentsData[iAgents].setTrays_screened(m_supervisorPvsDetailInfo[iDetails].getTrays_screened());
        slocalAgentsData[iAgents].setTrays_searched(m_supervisorPvsDetailInfo[iDetails].getTrays_searched());
      }
    }
    for(int iDetails = 0; iDetails < m_supervisorSvsDetailInfo.size(); iDetails++)
    {
      if(m_supervisorSvsDetailInfo[iDetails].getAgentsUuid() == slocalAgentsData[iAgents].getAgentsUuid())
      {
        slocalAgentsData[iAgents].setAgentsUuid(m_supervisorSvsDetailInfo[iDetails].getAgentsUuid());
        slocalAgentsData[iAgents].setDetailMachine_id(m_supervisorSvsDetailInfo[iDetails].getDetailMachine_id());
        slocalAgentsData[iAgents].setDetailLocation(m_supervisorSvsDetailInfo[iDetails].getDetailLocation());
        slocalAgentsData[iAgents].setAgentsSublocation(m_supervisorSvsDetailInfo[iDetails].getAgentsSublocation());
        slocalAgentsData[iAgents].setOnline_time(m_supervisorSvsDetailInfo[iDetails].getOnline_time());
        slocalAgentsData[iAgents].setUnlink_rate(m_supervisorSvsDetailInfo[iDetails].getUnlink_rate());
        slocalAgentsData[iAgents].setTrays_screened(m_supervisorSvsDetailInfo[iDetails].getTrays_screened());
        slocalAgentsData[iAgents].setTrays_searched(m_supervisorSvsDetailInfo[iDetails].getTrays_searched());
      }
    }
    if(m_supervisorScannerUtilization >= 0 && slocalAgentsData[iAgents].getAgentsType() == QMLEnums::SCANNER)
    {
      slocalAgentsData[iAgents].setUtilization(QString::number(m_supervisorScannerUtilization));
    }
    if(m_supervisorPvsUtilization >= 0 && slocalAgentsData[iAgents].getAgentsType() == QMLEnums::PVS)
    {
      slocalAgentsData[iAgents].setUtilization(QString::number(m_supervisorPvsUtilization));
    }
    if(m_supervisorSvsUtilization >= 0 && slocalAgentsData[iAgents].getAgentsType() == QMLEnums::SVS)
    {
      slocalAgentsData[iAgents].setUtilization(QString::number(m_supervisorSvsUtilization));
    }
    if(m_chartsAverageDataList.size() > 0)
    {
      slocalAgentsData[iAgents].setThroughput(m_chartsAverageDataList[0].toString());
    }
    if(m_chartsAverageDataList.size() > 1)
    {
      slocalAgentsData[iAgents].setReject_rate(m_chartsAverageDataList[1].toString());
    }
    if(m_chartsAverageDataList.size() > 2)
    {
      slocalAgentsData[iAgents].setAvg_decision_time(m_chartsAverageDataList[2].toString());
    }
  }
  if(slocalAgentsData.size() > 0)
  {
    m_supervisorAgents = slocalAgentsData;
  }
  if(m_supervisorAgents.size() > 0)
  {
    filterAgentsTableDataInfoBasedOnSelectedGrp();
  }
}

/*!
 * @fn       updateSupervisorGroupOverviewInfo();
 * @param    QVariantList listobj
 * @return   None
 * @brief    Update the data of Group Overview rest response into SupervisorGroupOverview
 */
//Deprecated API
void ModelManagerSupervisor::updateSupervisorGroupOverviewInfo(QVariantList listobj)
{
  m_supervisorGroupOverview.clear();
  for(int k = 0; k < listobj.size(); ++k)
  {
    QVariantMap listOfGroupOverview = listobj.at(k).toMap();
    QVariant apiValue;
    SupervisorGroupOverview sGroupOverview;

    for(auto itMap = listOfGroupOverview.begin(); itMap != listOfGroupOverview.end(); ++itMap)
    {
      QString key = itMap.key();
      apiValue = itMap.value();
      if(key == GROUP_OVERVIEW_AGENTS)
      {
        QVariantList listAgents;
        if(listOfGroupOverview["agents"].type() == QVariant::List)
        {
          listAgents = listOfGroupOverview["agents"].toList();
        }

      }
      else if(key == GROUP_OVERVIEW_PINNED_EVENTS)
      {
        updatePinnedEventsInfo(listOfGroupOverview);
        sGroupOverview.setGroupOverviewPinnedEvents(m_supervisorGroupOverviewPinnedEvents);
      }
      else if(key == GROUP_OVERVIEW_UUID)
      {
        sGroupOverview.setGroupOverviewUuid(apiValue.toString());
      }
      else if(key == GROUP_OVERVIEW_NAME)
      {
        sGroupOverview.setGroupOverviewName(apiValue.toString());
      }
      else if(key == GROUP_OVERVIEW_READINESS)
      {
        sGroupOverview.setGroupOverviewReadiness(apiValue.toString());
      }
    }
    m_supervisorGroupOverview.append(sGroupOverview);
  }
}

/*!
 * @fn       filterAgentsTableDataInfo
 * @param    QVector<SupervisorAgent>
 * @return   None
 * @brief    to filter Agents Data based on selected gauge type
 */
void ModelManagerSupervisor::filterAgentsTableDataInfo(const QVector<SupervisorAgents> &agentsData)
{
  QVector<SupervisorAgents>::const_iterator itr;
  QVector<SupervisorAgents> filteredSAgentData;
  if(agentsData.size() != 0)
  {
    for(itr = agentsData.begin(); itr != agentsData.end() ; itr++)
    {
      SupervisorAgents sAgent = *itr;
      if(sAgent.getAgentsType() == m_currentAgentsType)
      {
        filteredSAgentData.append(sAgent);
      }
    }
    emit agentsDataReceived(filteredSAgentData);
  }
}
/*!
 * @fn       filterAgentsTableDataInfoBasedOnSelectedGrp
 * @param    QVector<SupervisorAgent>
 * @return   None
 * @brief    to filter Agents Data based on selected gauge type
 */
void ModelManagerSupervisor::filterAgentsTableDataInfoBasedOnSelectedGrp()
{
  m_pvsCount = 0;
  m_svsCount = 0;
  m_scannerCount = 0;
  m_maxPvsCount = 0;
  m_maxSvsCount = 0;
  m_maxScannerCount = 0;
  int hr = 200 ;                                //200: Success
  QVector<SupervisorAgents>::const_iterator itr;
  QVector<SupervisorAgents> filteredSAgentData;
  if(m_supervisorAgents.size() != 0)
  {
    for(itr = m_supervisorAgents.begin(); itr != m_supervisorAgents.end() ; itr++)
    {
      SupervisorAgents sAgent = *itr;
      QVector<QString> agentGroups = sAgent.getAgentsGroupUuid();
      for(int i= 0; i < agentGroups.size(); i++)
      {
        if(agentGroups[i] == m_currentGroupUuid)
        {
          setAgentsTypeCount(sAgent.getAgentsType(), sAgent.getAgentsReadiness());
          if(sAgent.getAgentsType() == m_currentAgentsType)
          {
            filteredSAgentData.append(sAgent);
            break;
          }
          break;
        }
      }
    }
    m_dashBoardScreenModel.setPvsCount(m_pvsCount);
    m_dashBoardScreenModel.setScannerCount(m_scannerCount);
    m_dashBoardScreenModel.setSvsCount(m_svsCount);
    m_dashBoardScreenModel.setMaxPvsCount(m_maxPvsCount);
    m_dashBoardScreenModel.setMaxScannerCount(m_maxScannerCount);
    m_dashBoardScreenModel.setMaxSvsCount(m_maxSvsCount);

    emit agentsDataReceived(filteredSAgentData);
    emit authstatuschanged(hr, "", QMLEnums::SUPERVISOR_AGENTS);
  }
}
/*!
 * @fn       updatePinnedEventsInfo();
 * @param    QVariantMap
 * @return   None
 * @brief    Update the data of Pinned Events rest response into SupervisorPinnedEvents
 */
void ModelManagerSupervisor:: updatePinnedEventsInfo(QVariantMap listOfGroupOverview)
{
  m_supervisorGroupOverviewPinnedEvents.clear();
  SupervisorAgents sourceAgents;
  SupervisorPinnedEvents sPinnedEvents;
  QVariantList listPinnedEvents;
  if(listOfGroupOverview["pinned_events"].type() == QVariant::List)
  {
    listPinnedEvents = listOfGroupOverview["pinned_events"].toList();
  }

  for(int a = 0; a < listPinnedEvents.size(); ++a)
  {
    QVariantMap listOfGrpOverviewPinnedEvents = listPinnedEvents.at(a).toMap();
    QVariant eventValue;
    for(auto itMapEvents = listOfGrpOverviewPinnedEvents.begin(); itMapEvents != listOfGrpOverviewPinnedEvents.end(); ++itMapEvents)
    {
      QString keyEvent = itMapEvents.key();
      eventValue = itMapEvents.value();

      if(keyEvent == PINNED_EVENTS_SOURCE_AGENT)
      {
        QVariant listGrp;
        if(listOfGrpOverviewPinnedEvents["source_agent"].type() != 0)
        {
          listGrp = itMapEvents.value();
        }
        QVariantMap listOfAgentsGroups = listGrp.toMap();
        QVariant groupValue;
        for(auto itMapGrp = listOfAgentsGroups.begin(); itMapGrp != listOfAgentsGroups.end(); ++itMapGrp)
        {
          QString keyGrp = itMapGrp.key();
          groupValue = itMapGrp.value();
          if(keyGrp == AGENTS_GROUPS_UUID)
          {
            sourceAgents.setAgentsUuid(groupValue.toString());
          }
          else if(keyGrp == AGENTS_TYPE)
          {
            sourceAgents.setAgentsType(groupValue.toInt());
          }
          else if(keyGrp == AGENTS_MACHINE_ID)
          {
            sourceAgents.setAgentsMachine_id(groupValue.toString());
          }
          else if(keyGrp == AGENTS_MANUFACTURER)
          {
            sourceAgents.setAgentsManufacturer(groupValue.toString());
          }
        }
        sPinnedEvents.setPinnedEventsSourceAgents(sourceAgents);
      }
      else if(keyEvent == PINNED_EVENTS_NAME)
      {
        sPinnedEvents.setPinnedEventsName(eventValue.toString());
      }
      else if(keyEvent == PINNED_EVENTS_CREATED_TIME)
      {
        sPinnedEvents.setPinnedEventsCreatedTime(eventValue.toString());
      }
      else if(keyEvent == PINNED_EVENTS_PARAM)
      {
        QVariantList listParam;
        if(listOfGrpOverviewPinnedEvents["param"].type() == QVariant::List)
        {
          listParam = listOfGrpOverviewPinnedEvents["param"].toList();
        }
        foreach(QVariant itrListParam,listParam)
        {
          sPinnedEvents.setPinnedEventsParams(itrListParam.toString());
        }
      }
    }
    m_supervisorGroupOverviewPinnedEvents.append(sPinnedEvents);
  }
}

/*!
 * @fn       setAgentsTypeCount
 * @param    const QString &type
 * @return   None
 * @brief    set type count respected to Scanner, PVS ,SVS
 */
void ModelManagerSupervisor::setAgentsTypeCount(const int &type, const int &readiness)
{
  if(readiness)
  {
    if(type == QMLEnums::PVS)
    {
      m_pvsCount++;
      m_maxPvsCount++;
    }
    else if(type == QMLEnums::SVS)
    {
      m_svsCount++;
      m_maxSvsCount++;
    }
    else if(type == QMLEnums::SCANNER)
    {
      m_scannerCount++;
      m_maxScannerCount++;
    }
    else
    {
      INFO_LOG("Failed to increment agent type")
    }
  }
  else
  {
    if(type == QMLEnums::PVS)
    {
      m_maxPvsCount++;
    }
    else if(type == QMLEnums::SVS)
    {
      m_maxSvsCount++;
    }
    else if(type == QMLEnums::SCANNER)
    {
      m_maxScannerCount++;
    }
  }
}

/*!
 * @fn       updateRecentEventInfo();
 * @param    QVariantList
 * @param    int
 * @return   None
 * @brief    Update the data of Agents rest response into RecentEvent
 */
void ModelManagerSupervisor::updateRecentEventInfo(QVariantList listobj)
{
  QDateTime curent = QDateTime::currentDateTime();
  m_supervisorRecentEvents.clear();
  SupervisorRecentEvents sRecentEvents;
  QString time;
  for(int k = 0; k < listobj.size(); ++k)
  {
    QVariantMap listOfBagsPerHr = listobj.at(k).toMap();
    QVariant apiValue;

    for(auto itMap = listOfBagsPerHr.begin(); itMap != listOfBagsPerHr.end(); ++itMap)
    {
      QString key = itMap.key();
      apiValue = itMap.value();

      if(key == RECENT_EVENTS_CREATEDTIME)
      {
        QString format = "yyyy-MM-ddTHH:mm:ssZ";
        QDateTime setTime(QDateTime::fromString(apiValue.toString(),format));
        if(setTime.isValid())
        {
          qint64 msec = setTime.msecsTo(curent);
          int minutes = (int)((msec / (1000*60)));
          if(minutes <= 5)
          {
            time = "Few minutes ago";
          }
          else if(minutes > 5 && minutes <=59)
          {
            time = QString::number(minutes)+" minutes ago";
          }
          else
          {
            setTime.setTimeSpec(Qt::OffsetFromUTC);
            time = "at "+setTime.toString()+" Time."; //final string will be event observeed at ____ time
          }
        }
        else
        {
          INFO_LOG("Date/Time Received is Invalid");
        }
        sRecentEvents.setRecentEventsCreatedTime(time);
      }
      else if(key == RECENT_EVENTS_EVENTNAME)
      {
        sRecentEvents.setRecentEventsEventName(apiValue.toString());
      }
    }
    m_supervisorRecentEvents.append(sRecentEvents);
  }
  emit recentEventsDataReceived(m_supervisorRecentEvents);
}

/*!
* @fn      onGetSelectedGroup
* @param   QString
* @return  None
* @brief   This function is responsible for getting selected group
*/
void ModelManagerSupervisor::onGetSelectedGroup(QString selectedGroup)
{
  QString uuid = "";
  if(selectedGroup != m_selectedGroup)
  {
    m_selectedGroup = selectedGroup;
  }
  if(m_selectedGroup == "")
  {
    if(m_supervisorGroups.size() > 0)
    {
      m_selectedGroup = m_supervisorGroups[0].getGroupsName();
    }
  }
  m_agentsDataModel->setGroupName(m_selectedGroup);
  foreach(SupervisorGroups groupIndex, m_supervisorGroups)
  {
    if(m_selectedGroup == groupIndex.getGroupsName())
    {
      uuid = groupIndex.getGroupsUuid();
      break;
    }
    else
    {
      INFO_LOG("No match between current selected group name from drop down and group name from groups REST API")
    }
  }
  if(m_currentGroupUuid != uuid)
  {
    m_currentGroupUuid = uuid;
    apiRequestOnEvent(QMLEnums::SUPERVISOR_AGENTS);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SCANNER_UTILIZATION, RequestType::GROUP);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_PVS_UTILIZATION, RequestType::GROUP);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SVS_UTILIZATION, RequestType::GROUP);
  }
  filterGroupData(m_currentGroupUuid);
}

/*!
* @fn      onGetSelectedGauge
* @param   QString
* @return  None
* @brief   This function is responsible for getting selected gauge
*/
void ModelManagerSupervisor::onGetSelectedGauge(int selectedGauge)
{
  setCurrentAgentType(selectedGauge);
  onGetSelectedGroup(m_selectedGroup);
}

/*!
* @fn       setCurrentAgentType
* @param    QString
* @return   None
* @brief    Set the agent type basis of gauge selected.
*/
void ModelManagerSupervisor::setCurrentAgentType(int currentAgentType)
{  
  m_currentAgentsType = currentAgentType;
}

/*!
    * @fn       updateSupervisorGroupsInfo();
    * @param    QVariantList
    * @return   None
    * @brief    Update the data of groups rest response into SupervisorGroups
    */
void ModelManagerSupervisor::updateSupervisorGroupsInfo(QVariantList listobj)
{
  QVector<SupervisorGroups> slocalGroupsData;
  foreach(QVariant listOfGroups, listobj)
  {
    SupervisorGroups sGroups;
    QVariant groupValue;
    QVariantMap groupsMap = listOfGroups.toMap();
    foreach(QString groupKeys, groupsMap.keys())
    {
      QString key = groupKeys;
      groupValue = groupsMap.value(groupKeys);
      if(key == SUPERVISOR_GROUPS_UUID)
      {
        sGroups.setGroupsUuid(groupValue.toString());
      }
      else if(key == SUPERVISOR_GROUPS_NAME)
      {
        sGroups.setGroupsName(groupValue.toString());
      }
      else if(key == SUPERVISOR_GROUPS_TYPE)
      {
        sGroups.setGroupsType(groupValue.toInt());
      }
    }
    slocalGroupsData.append(sGroups);
  }
  m_supervisorGroups = slocalGroupsData;

  if((m_currentGroupUuid == "") && (m_supervisorGroups.size ()>0))
  {
    m_currentGroupUuid = m_supervisorGroups[0].getGroupsUuid();
    apiRequestOnEvent(QMLEnums::RECENT_EVENTS);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_AGENTS);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SCANNER_UTILIZATION, RequestType::GROUP);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_PVS_UTILIZATION, RequestType::GROUP);
    apiRequestOnEvent(QMLEnums::SUPERVISOR_SVS_UTILIZATION, RequestType::GROUP);
    m_pUpdateSlowTimer->start();
    m_pUpdateFastTimer->start();
  }
  emit groupDataReceived(m_supervisorGroups);
}

/*!
    * @fn       filterGroupData();
    * @param    QString
    * @return   None
    * @brief    filter agents data based on groups uuid
    */
void ModelManagerSupervisor::filterGroupData(QString uuid)
{
  int hr = 200;       //200 or 201 for success response on authstatus changed
  QString status = "";        //status argument
  m_pvsCount = 0;
  m_svsCount = 0;
  m_scannerCount = 0;
  m_maxPvsCount = 0;
  m_maxSvsCount = 0;
  m_maxScannerCount = 0;
  QVector<SupervisorAgents> sAgents ;
  foreach(SupervisorAgents agentIndex, m_supervisorAgents)
  {
    QVector<QString> agentGroups = agentIndex.getAgentsGroupUuid();
    for(int i= 0; i < agentGroups.size(); i++)
    {
      if(agentGroups[i] == uuid)
      {
        setAgentsTypeCount(agentIndex.getAgentsType(), agentIndex.getAgentsReadiness());
        sAgents.append(agentIndex);
        break;
      }
      else
      {
        INFO_LOG("No match between Group UUID and Agent Group UUID")
      }
    }
  }
  m_dashBoardScreenModel.setPvsCount(m_pvsCount);
  m_dashBoardScreenModel.setScannerCount(m_scannerCount);
  m_dashBoardScreenModel.setSvsCount(m_svsCount);
  m_dashBoardScreenModel.setMaxPvsCount(m_maxPvsCount);
  m_dashBoardScreenModel.setMaxScannerCount(m_maxScannerCount);
  m_dashBoardScreenModel.setMaxSvsCount(m_maxSvsCount);
  filterAgentsTableDataInfo(sAgents);

  emit authstatuschanged(hr, status, QMLEnums::SUPERVISOR_AGENTS);
}

/*!
    * @fn       updateAgentsUtilizationInfo();
    * @param    QVariantList
    * @return   None
    * @brief    Update the data of updateAgentsUtilizationInfo (Scanner/PVS/SVS) rest response
    */
void ModelManagerSupervisor::updateAgentsUtilizationInfo(QVariantList listobj, int modelnum)
{
  float data = 0.0f;
  QString agentUuid = "";
  QString groupUuid = "";
  foreach(QVariant listVariable, listobj)
  {
    data = 0.0f;
    QVariant utilizationValue;
    QVariantMap utilizationMap = listVariable.toMap();
    foreach(QString utilizationKeys, utilizationMap.keys())
    {
      QString key = utilizationKeys;
      utilizationValue = utilizationMap.value(utilizationKeys);
      if(key == SUPERVISOR_UTILIZATION_GROUPS_UUID)
      {
        groupUuid = utilizationValue.toString();
      }
      else if(key == SUPERVISOR_UTILIZATION_AGENTS_UUID)
      {
        agentUuid = utilizationValue.toString();
      }
      else if(key == SUPERVISOR_UTILIZATION_DATA)
      {
        data = utilizationValue.toFloat();
      }
    }
  }

  if(modelnum == QMLEnums::SUPERVISOR_SCANNER_UTILIZATION)
  {
    if(agentUuid == m_agentsDataModel->getCurrentAgentUuid())
    {
      m_supervisorScannerUtilization = data;
      updateAgentsInformation();
    }
    if(groupUuid == m_currentGroupUuid)
    {
      m_agentsDataModel->setScannerUtilization(data);
      m_agentsDataModel->getUtilizationData(QMLEnums::SCANNER);
    }
  }
  else if(modelnum == QMLEnums::SUPERVISOR_PVS_UTILIZATION)
  {
    if(agentUuid == m_agentsDataModel->getCurrentAgentUuid())
    {
      m_supervisorPvsUtilization = data;
      updateAgentsInformation();
    }
    if(groupUuid == m_currentGroupUuid)
    {
      m_agentsDataModel->setPvsUtilization(data);
      m_agentsDataModel->getUtilizationData(QMLEnums::PVS);
    }
  }
  else if(modelnum == QMLEnums::SUPERVISOR_SVS_UTILIZATION)
  {
    if(agentUuid == m_agentsDataModel->getCurrentAgentUuid())
    {
      m_supervisorSvsUtilization = data;
      updateAgentsInformation();
    }
    if(groupUuid == m_currentGroupUuid)
    {
      m_agentsDataModel->setSvsUtilization(data);
      m_agentsDataModel->getUtilizationData(QMLEnums::SVS);
    }
  }
}

/*!
    * @fn       updateAgentsDetailInfo();
    * @param    QVariantList
    * @return   None
    * @brief    Update the data of updateAgentsDetailInfo (Scanner/PVS/SVS) rest response
    */
void ModelManagerSupervisor::updateAgentsDetailInfo(QVariantList listobj, int modelnum)
{
  int iTotalDataToUpdate = 0;
  QVariant scannerDetailValue;
  QVariantMap scannerDetailMap;
  QVector<SupervisorAgents> slocalAgentsData;
  QString scannerDeatilKeys;
  foreach(QVariant listOfScannerDetail, listobj)
  {
    scannerDetailMap = listOfScannerDetail.toMap();

    scannerDeatilKeys = "";
    scannerDetailValue = scannerDetailMap.value(AGENT_UUID);

    SupervisorAgents  sAgents;
    foreach(scannerDeatilKeys, scannerDetailMap.keys())
    {
      iTotalDataToUpdate++;
      QString key = scannerDeatilKeys;
      scannerDetailValue = scannerDetailMap.value(scannerDeatilKeys);
      if(key == AGENT_UUID )//P1
      {
        sAgents.setAgentsUuid(scannerDetailValue.toString());
      }
      else if(key == AGENT_MACHINE_ID)
      {
        sAgents.setDetailMachine_id(scannerDetailValue.toString());
      }
      else if(key == AGENT_LOCATION)
      {
        sAgents.setDetailLocation(scannerDetailValue.toString());
      }
      else if(key == AGENT_SUBLOCATION)
      {
        sAgents.setAgentsSublocation(scannerDetailValue.toString());
      }
      else if(key == AGENT_ONLINETIME)
      {
        sAgents.setOnline_time(scannerDetailValue.toString());
      }
      else if(key == AGENT_UNLINK_RATE)
      {
        sAgents.setUnlink_rate(scannerDetailValue.toFloat());
      }
      else if(key == AGENT_TRAYS_SCREENED)
      {
        sAgents.setTrays_screened(scannerDetailValue.toInt());
      }
      else if(key == AGENT_TRAYS_SEARCHED)
      {
        sAgents.setTrays_searched(scannerDetailValue.toInt());
      }
    }
    slocalAgentsData.append(sAgents);
    if(modelnum == QMLEnums::SUPERVISOR_SCANNER_DETAIL)
    {
      m_supervisorScannerDetailInfo = slocalAgentsData ;
    }
    else if(modelnum == QMLEnums::SUPERVISOR_PVS_DETAIL)
    {
      m_supervisorPvsDetailInfo = slocalAgentsData;
    }
    else if(modelnum == QMLEnums::SUPERVISOR_SVS_DETAIL)
    {
      m_supervisorSvsDetailInfo = slocalAgentsData;
    }
  }
  updateAgentsInformation();
}

/*!
   * @fn       resetAllModels
   * @param    none
   * @return   None
   * @brief    reset the data of all models
   */
void ModelManagerSupervisor::resetAllModels()
{
  QVector<SupervisorAgents> supervisorAgentData;
  supervisorAgentData.clear();
  emit agentsDataReceived(supervisorAgentData);
  onGetSelectedGroup("");
  m_agentsDataModel->getUtilizationData(-1);
  m_supervisorRecentEvents.clear();
  emit recentEventsDataReceived(m_supervisorRecentEvents);
  for(int i=0; i<m_chartsAverageDataList.size();i++)
  {
    m_chartsAverageDataList[i] = "";
  }
  emit chartsAverageData(m_chartsAverageDataList);
  m_supervisorGroups.clear();
  emit groupDataReceived(m_supervisorGroups);
  m_bagsPerHourCordinates.clear();
  m_suspectRateCordinates.clear();
  m_recentOperatorTimeCordinates.clear();
  m_recentTimeToDivertCordinates.clear();
  m_lineChartSeriesMap.clear();
  m_chartTimeMap.clear();
}

void ModelManagerSupervisor::displayLineChart(int modelnum)
{
  QXYSeries *xySeries = nullptr;
  if((m_bagsPerHourCordinates.count() > 0) && (modelnum == QMLEnums::BAGS_PER_HOUR))
  {
    if((m_lineChartSeriesMap.contains(QMLEnums::BAGS_PER_HOUR)) && (m_lineChartSeriesMap.value(QMLEnums::BAGS_PER_HOUR) != NULL))
    {
      xySeries = dynamic_cast<QXYSeries *>(m_lineChartSeriesMap.value(QMLEnums::BAGS_PER_HOUR));
      // Use replace instead of clear + append, it's optimized for performance
      if(xySeries)
      {
        xySeries->replace(m_bagsPerHourCordinates);
        setXYAxes(m_bagsPerHourCordinates, modelnum);
      }
    }
  }
  else if((m_suspectRateCordinates.count() > 0) && (modelnum == QMLEnums::SUSPECT_RATE_PER_HOUR))
  {
    if((m_lineChartSeriesMap.contains(QMLEnums::SUSPECT_RATE_PER_HOUR)) && (m_lineChartSeriesMap.value(QMLEnums::SUSPECT_RATE_PER_HOUR) != NULL))
    {
      xySeries = dynamic_cast<QXYSeries *>(m_lineChartSeriesMap.value(QMLEnums::SUSPECT_RATE_PER_HOUR));
      // Use replace instead of clear + append, it's optimized for performance
      if(xySeries)
      {
        xySeries->replace(m_suspectRateCordinates);
        setXYAxes(m_suspectRateCordinates, modelnum);
      }
    }
  }
  else if((m_recentTimeToDivertCordinates.count() > 0) && (modelnum == QMLEnums::RECENT_TIME_TO_DIVERT))
  {
    if((m_lineChartSeriesMap.contains(QMLEnums::RECENT_TIME_TO_DIVERT)) && (m_lineChartSeriesMap.value(QMLEnums::RECENT_TIME_TO_DIVERT) != NULL))
    {
      xySeries = dynamic_cast<QXYSeries *>(m_lineChartSeriesMap.value(QMLEnums::RECENT_TIME_TO_DIVERT));
      // Use replace instead of clear + append, it's optimized for performance
      if(xySeries)
      {
        xySeries->replace(m_recentTimeToDivertCordinates);
        setXYAxes(m_recentTimeToDivertCordinates, modelnum);
      }
    }
  }
  else if((m_recentOperatorTimeCordinates.count() > 0) && (modelnum == QMLEnums::RECENT_OPERATOR_DECISION_TIME))
  {
    if((m_lineChartSeriesMap.contains(QMLEnums::RECENT_OPERATOR_DECISION_TIME)) && (m_lineChartSeriesMap.value(QMLEnums::RECENT_OPERATOR_DECISION_TIME) != NULL))
    {
      xySeries = dynamic_cast<QXYSeries *>(m_lineChartSeriesMap.value(QMLEnums::RECENT_OPERATOR_DECISION_TIME));
      // Use replace instead of clear + append, it's optimized for performance
      if(xySeries)
      {
        xySeries->replace(m_recentOperatorTimeCordinates);
        setXYAxes(m_recentOperatorTimeCordinates, modelnum);
      }
    }
  }
  else
  {
    INFO_LOG("Failed to update chart. Wrong model received")
  }
  emit chartsAverageData(m_chartsAverageDataList);
}

/*!
* @fn       displayLineChart();
* @param    QVector<QPointF>
* @param    QDateTime
* @param    QDateTime
* @param    int
* @return   None
* @brief    set x and y axes in line chart
*/
void ModelManagerSupervisor::setXYAxes(QVector<QPointF> pointCoordinates, int modelno)
{
  if(!pointCoordinates.isEmpty())
  {
    qreal minYAxis = pointCoordinates.at(0).y();
    qreal maxYAxis = pointCoordinates.at(0).y();
    for(int i=0; i< pointCoordinates.size(); i++)
    {
      if(pointCoordinates.at(i).y() < minYAxis)
      {
        minYAxis = pointCoordinates.at(i).y();
      }
      if(pointCoordinates.at(i).y() > maxYAxis)
      {
        maxYAxis = pointCoordinates.at(i).y();
      }
    }
    minYAxis *= 0.9;
    maxYAxis *= 1.1;
    if((modelno == QMLEnums::BAGS_PER_HOUR) && (m_lineChartSeriesMap.contains(QMLEnums::BAGS_PER_HOUR)) && (m_chartTimeMap.contains(QMLEnums::BAGS_PER_HOUR)))
    {
      m_lineChartSeriesMap[QMLEnums::BAGS_PER_HOUR]->attachedAxes().at(1)->setMin(minYAxis);
      m_lineChartSeriesMap[QMLEnums::BAGS_PER_HOUR]->attachedAxes().at(1)->setMax(maxYAxis);
      m_lineChartSeriesMap[QMLEnums::BAGS_PER_HOUR]->attachedAxes().at(0)->setMin(m_chartTimeMap[QMLEnums::BAGS_PER_HOUR].first); //m_chartTimeMap.first = startTime
      m_lineChartSeriesMap[QMLEnums::BAGS_PER_HOUR]->attachedAxes().at(0)->setMax(m_chartTimeMap[QMLEnums::BAGS_PER_HOUR].second); //m_chartTimeMap.second = endTime
    }
    else if((modelno == QMLEnums::SUSPECT_RATE_PER_HOUR) && (m_lineChartSeriesMap.contains(QMLEnums::SUSPECT_RATE_PER_HOUR)) && (m_chartTimeMap.contains(QMLEnums::SUSPECT_RATE_PER_HOUR)))
    {
      m_lineChartSeriesMap[QMLEnums::SUSPECT_RATE_PER_HOUR]->attachedAxes().at(1)->setMin(minYAxis);
      m_lineChartSeriesMap[QMLEnums::SUSPECT_RATE_PER_HOUR]->attachedAxes().at(1)->setMax(maxYAxis);
      m_lineChartSeriesMap[QMLEnums::SUSPECT_RATE_PER_HOUR]->attachedAxes().at(0)->setMin(m_chartTimeMap[QMLEnums::SUSPECT_RATE_PER_HOUR].first); //m_chartTimeMap.first = startTime
      m_lineChartSeriesMap[QMLEnums::SUSPECT_RATE_PER_HOUR]->attachedAxes().at(0)->setMax(m_chartTimeMap[QMLEnums::SUSPECT_RATE_PER_HOUR].second); //m_chartTimeMap.second = endTime
    }
    else if((modelno == QMLEnums::RECENT_OPERATOR_DECISION_TIME) && (m_lineChartSeriesMap.contains(QMLEnums::RECENT_OPERATOR_DECISION_TIME)) && (m_chartTimeMap.contains(QMLEnums::RECENT_OPERATOR_DECISION_TIME)))
    {
      m_lineChartSeriesMap[QMLEnums::RECENT_OPERATOR_DECISION_TIME]->attachedAxes().at(1)->setMin(minYAxis);
      m_lineChartSeriesMap[QMLEnums::RECENT_OPERATOR_DECISION_TIME]->attachedAxes().at(1)->setMax(maxYAxis);
      m_lineChartSeriesMap[QMLEnums::RECENT_OPERATOR_DECISION_TIME]->attachedAxes().at(0)->setMin(m_chartTimeMap[QMLEnums::RECENT_OPERATOR_DECISION_TIME].first); //m_chartTimeMap.first = startTime
      m_lineChartSeriesMap[QMLEnums::RECENT_OPERATOR_DECISION_TIME]->attachedAxes().at(0)->setMax(m_chartTimeMap[QMLEnums::RECENT_OPERATOR_DECISION_TIME].second); //m_chartTimeMap.second = endTime
    }
    else if((modelno == QMLEnums::RECENT_TIME_TO_DIVERT) && (m_lineChartSeriesMap.contains(QMLEnums::RECENT_TIME_TO_DIVERT)) && (m_chartTimeMap.contains(QMLEnums::RECENT_TIME_TO_DIVERT)))
    {
      m_lineChartSeriesMap[QMLEnums::RECENT_TIME_TO_DIVERT]->attachedAxes().at(1)->setMin(minYAxis);
      m_lineChartSeriesMap[QMLEnums::RECENT_TIME_TO_DIVERT]->attachedAxes().at(1)->setMax(maxYAxis);
      m_lineChartSeriesMap[QMLEnums::RECENT_TIME_TO_DIVERT]->attachedAxes().at(0)->setMin(m_chartTimeMap[QMLEnums::RECENT_TIME_TO_DIVERT].first); //m_chartTimeMap.first = startTime
      m_lineChartSeriesMap[QMLEnums::RECENT_TIME_TO_DIVERT]->attachedAxes().at(0)->setMax(m_chartTimeMap[QMLEnums::RECENT_TIME_TO_DIVERT].second); //m_chartTimeMap.second = endTime
    }
    else
    {
      INFO_LOG("Failed to update chart series. Wrong model received or series not available ")
    }
  }
}

/*!
* @fn       onCurrentScreenChanged
* @param    int
* @return   None
* @brief    slot get called when current screen changed.
*/
void ModelManagerSupervisor::onCurrentScreenChanged(int currentView)
{
  if(currentView == QMLEnums::SUPERVISOR_SCREEN)
  {
    m_pUpdateSlowTimer->start();
    m_pUpdateFastTimer->start();
    INFO_LOG("Timer started to request REST APIs when current screen is Supervisor Screen")
  }
  else
  {
    m_pUpdateSlowTimer->stop();
    m_pUpdateFastTimer->stop();
    INFO_LOG("Timer stopped to request REST APIs when current screen is not Supervisor Screen")
  }
}

}
}
