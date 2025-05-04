#ifndef MODELMANAGERSUPERVISOR_H
#define MODELMANAGERSUPERVISOR_H

#include <QTimer>
#include <QtCharts/QAbstractSeries>


#include <analogic/ws/uihandler/modelmanager.h>
#include <analogic/ws/uihandler/supervisoragents.h>
#include <analogic/ws/uihandler/supervisorgroupoverview.h>
#include <analogic/ws/uihandler/supervisorpinnedevents.h>
#include <analogic/ws/dashboardscreenmodel.h>
#include <analogic/ws/uihandler/supervisoragentsdatamodel.h>
#include <analogic/ws/uihandler/supervisorrecentevents.h>
#include <analogic/ws/uihandler/supervisoreventsdatamodel.h>
#include <analogic/ws/uihandler/groupdatamodel.h>
#include <analogic/ws/uihandler/supervisorscannerutilization.h>
#include <analogic/ws/uihandler/supervisorpvsutilization.h>
#include <analogic/ws/uihandler/supervisorsvsutilization.h>

#define SLOW_TIMER                 60000
#define FAST_TIMER                 15000
#define CHART_RECORDS_KEY         "records"
#define CHART_START_TIME_KEY      "startTime"
#define CHART_END_TIME_KEY        "endTime"
#define CHART_INTERVAL_KEY        "intervalInSeconds"

QT_CHARTS_USE_NAMESPACE

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{
namespace ws
{
class TaskManager;
/*!
 * \class   ModelManagerSupervisor
 * \brief   This class contains variable and function related to
 *          updating the data for current screen view of supervisor.
 */
class ModelManagerSupervisor : public ModelManager
{
  Q_OBJECT

public:
  explicit ModelManagerSupervisor(QObject *parent = NULL);
  virtual ~ModelManagerSupervisor();
  ModelList                               m_scrmodels;
  boost::scoped_ptr<QTimer>               m_pUpdateSlowTimer;
  boost::scoped_ptr<QTimer>               m_pUpdateFastTimer;
  QMap<QMLEnums::ModelEnum, QString>      m_restmap;              //!< map for rest URL
  QMap<QMLEnums::ModelEnum, QByteArray>   m_restContentMap;       //!< map for rest content type

  enum RequestType
  {
    ALL_TYPE,
    GROUP,
    AGENT
  };

  Q_ENUM(RequestType)
  /*!
     * @fn      onExit
     * @param   None
     * @return  None
     * @brief Function will call on exit of threads
     */
  void onExit();

  /*!
     * @fn       init
     * @param    None
     * @return   None
     * @brief    on thread start this function will initialize models.
     */
  void init();

  /*!
     * @fn       setAgentsTypeCount
     * @param    const QString &type, const QString &readiness
     * @return   None
     * @brief    set type count respected to Scanner, PVS ,SVS
     */
  void setAgentsTypeCount(const int &type, const int &readiness);

  /*!
     * @fn       resetAllModels
     * @param    none
     * @return   None
     * @brief    reset the data of all models
     */
  void resetAllModels();

  /*!
   * @fn      apiRequestOnEvent
   * @param   QMLEnum
   * @return  None
   * @brief   to call api request on event
   */
  void apiRequestOnEvent(QMLEnums::ModelEnum modelname, RequestType requestType = RequestType::ALL_TYPE);

signals:
  /*!
      * @fn       agentsdatarecieved
      * @param    QVector
      * @return   None
      * @brief    to populate supervisoragentsdata class on recieve of agents data.
      */
  void agentsDataReceived(QVector<SupervisorAgents> agentsData);

  /*!
    * @fn       recenteventsdatarecieved
    * @param    QVector
    * @return   None
    * @brief    to populate supervisoragentsdata class on recieve of agents data.
    */
  void recentEventsDataReceived(QVector<SupervisorRecentEvents> reventsData);

  /*!
   * @fn       groupDataReceived
   * @param    QVector
   * @return   None
   * @brief    to populate SupervisorGroups class on recieve of group data.
   */
  void groupDataReceived(const QVector<SupervisorGroups> &groupData);

  /*!
   * @fn       chartsAverageData
   * @param    QVector
   * @return   None
   * @brief    to send average data to line chart qml.
   */
  void chartsAverageData(QVariantList averageList);

public slots:

#ifdef WORKSTATION
  /*!
     * @fn      onsslerrorreceived
     * @param   int - error
     * @param   QString - status
     * @param   int - modelno
     * @return  void
     * @brief   Function will call on onsslerrorreceived
     */
  void onsslerrorreceived(int hr, QString status,int modelno);
#endif
  /*!
     * @fn       onScreenModelChanged(ModelList oModels)
     * @param    ModelList
     * @return   None
     * @brief    slot call on screen model changed
     */
  void onScreenModelChanged(ModelList oModels);
  /*!
     * @fn       onUpdateModels
     * @param    QVariantList
     * @param    QMLEnums::ModelEnum model name
     * @return   void
     * @brief    This slot will call on data updation from rest.
     */
  void onUpdateModels(QVariantList map, QMLEnums::ModelEnum modelname);

  /*!
     * @fn       onprocessUpdateModels
     * @param    None
     * @return   None
     * @brief    process models.
     */
  void onProcessUpdateModels();
  /*!
    * @fn       onCommandButtonEvent
    * @param    QMLEnums::ModelEnum  - modelname
    * @param    QString data to be post
    * @return   void
    * @brief    This slot will call on command button clicked.
    */
  void onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data);

  /*!
    * @fn      updateChartsCordinates
    * @param   QAbstractSeries
    * @return  None
    * @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
    */
  void onUpdateChartsCordinates(QAbstractSeries *series, int modelnum);

  /*!
    * @fn       onTaskTimeout
    * @param    None
    * @return   None
    * @brief    slots check if task has got timeout.
    */
  void onTaskTimeout();

  /*!
     * @fn      onauthstatuschanged
     * @param   int - error
     * @param   QString - status
     * @param   int - modelnum
     * @return  void
     * @brief   Function will call on authentication status changed
     */
  void onauthstatuschanged(int hr, QString status,int modelno);

  /*!
   * @fn      onGetSelectedGroup
   * @param   QString
   * @return  None
   * @brief   This function is responsible for getting selected group
   */
  void onGetSelectedGroup(QString selectedGroup);

  /*!
  * @fn      onGetSelectedGauge
  * @param   QString
  * @return  None
  * @brief   This function is responsible for getting selected gauge
  */
  void onGetSelectedGauge(int selectedGauge);

  /*!
  * @fn       setCurrentAgentType
  * @param    QString
  * @return   None
  * @brief    Set the agent type basis of gauge selected.
  */
  void setCurrentAgentType(int currentAgentType);
  /*!
  * @fn       apiRequestSlow
  * @param    None
  * @return   None
  * @brief    timer thread that run every configurable interval.
  */
  void apiRequestSlow();

  /*!
  * @fn       apiRequestFast
  * @param    None
  * @return   None
  * @brief    timer thread that run every configurable interval.
  */
  void apiRequestFast();

  /*!
  * @fn       onCurrentScreenChanged
  * @param    int
  * @return   None
  * @brief    slot get called when current screen changed.
  */
  void onCurrentScreenChanged(int currentView);

  /*!
   * @fn      onCurrentAgentChanged
   * @param   None
   * @return  None
   * @brief   to call api on agent change
   */
  void onCurrentAgentChanged(QString agentUuid);

private:
  /*!
    * @fn       ModelManagerSupervisor();
    * @param    ModelManagerSupervisor&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
  ModelManagerSupervisor(const ModelManagerSupervisor& modelManagerSupervisor);

  /*!
    * @fn       operator=
    * @param    ModelManagerSupervisor&
    * @return   ModelManagerSupervisor&
    * @brief    declaration for private assignment operator.
    */
  ModelManagerSupervisor& operator= (const ModelManagerSupervisor& modelManagerSupervisor);

  /*!
     * @fn       initRestMap
     * @param    None
     * @return   None
     * @brief    Initialize model rest mapping.
     */
  void initRestMap();

  /*!
     * @fn       initRestContentMap();
     * @param    None
     * @return   None
     * @brief    Initialize model rest contentType mapping
     */
  void initRestContentMap();

  /*!
     * @fn       updateSupervisorAgentsInfo();
     * @param    QVariantList
     * @param    int
     * @return   None
     * @brief    Update the data of Agents rest response into SupervisorAgents
     */
  void updateSupervisorAgentsInfo(QVariantList listobj , int modelnum = 0);

  /*!
       * @fn       updateAgentsInformation
       * @param    const QVector<SupervisorAgents> &type, const QVector<SupervisorAgents> &slocalAgentsData
       * @return   None
       * @brief    updateAgentsInformation updates the data in QVector<SupervisorAgents> from agents rest api
       */
  void updateAgentsInformation(QVector<SupervisorAgents> &slocalAgentsData);

  /*!
       * @fn       updateAgentsInformation
       * @param    None
       * @return   None
       * @brief    updateAgentsInformation updates the data in QVector<SupervisorAgents> from agent's detail and utilization rest api
       */
  void updateAgentsInformation();

  /*!
     * @fn       updateResponse();
     * @param    QVariantList - listobj
     * @param    int - modelnum
     * @return   None
     * @brief    Stored cordinates in Vector to display it on charts
     */
  void updateResponse(QVariantList listobj, int modelnum);

  /*!
     * @fn       updateSupervisorGroupOverviewInfo();
     * @param    QVariantList
     * @return   None
     * @brief    Update the data of Group Overview rest response into SupervisorGroupOverview
     */
  void updateSupervisorGroupOverviewInfo(QVariantList listobj);

  /*!
   * @fn       filterAgentsTableDataInfo
   * @param    QVector<SupervisorAgent>
   * @return   None
   * @brief    to filter Agents Data based on selected gauge type
   */
  void filterAgentsTableDataInfo(const QVector<SupervisorAgents> &agentsData);

  /*!
     * @fn       updatePinnedEventsInfo();
     * @param    QVariantList     * @return   None
     * @brief    Update the data of Pinned Events rest response into SupervisorPinnedEvents
     */
  void updatePinnedEventsInfo(QVariantMap listOfGroupOverview);

  /*!
     * @fn       updateRecentEventInfo();
     * @param    QVariantList
     * @param    int
     * @return   None
     * @brief    Update the data of Agents rest response into RecentEvent
     */
  void updateRecentEventInfo(QVariantList listobj);

  /*!
      * @fn       updateSupervisorGroupsInfo();
      * @param    QVariantList
      * @return   None
      * @brief    Update the data of groups rest response into SupervisorGroups
      */
  void updateSupervisorGroupsInfo(QVariantList listobj);

  /*!
       * @fn       filterGroupData();
       * @param    QString
       * @return   None
       * @brief    filter agents data based on groups uuid
       */
  void filterGroupData(QString uuid);

  /*!
 * @fn       filterAgentsTableDataInfoBasedOnSelectedGrp
 * @param    QVector<SupervisorAgent>
 * @return   None
 * @brief    to filter Agents Data based on selected gauge type
 */
  void filterAgentsTableDataInfoBasedOnSelectedGrp();

  /*!
       * @fn       updateAgentsUtilizationInfo();
       * @param    QVariantList
       * @return   None
       * @brief    Update the data of agent's utilization rest response into AgentsUtilizationInfo
       */
  void updateAgentsUtilizationInfo(QVariantList listobj,int modelnum);

  /*!
      * @fn       updateAgentsDetailInfo();
      * @param    QVariantList
      * @return   None
      * @brief    Update the data of agent's rest response into AgentsDetailInfo
      */
  void updateAgentsDetailInfo(QVariantList listobj, int modelnum);

  /*!
   * @fn       displayLineChart();
   * @param    QDateTime
   * @param    QDateTime
   * @param    int
   * @return   None
   * @brief    display line chart
   */
  void displayLineChart(int modelnum);

  /*!
   * @fn       displayLineChart();
   * @param    QVector<QPointF>
   * @param    QDateTime
   * @param    QDateTime
   * @param    int
   * @return   None
   * @brief    set x and y axes in line chart
   */
  void setXYAxes(QVector<QPointF> pointCoordinates, int modelno);

private:
  //4 different vectors are created with same data type to avoid synchronization issues
  //i.e it is possible that when m_ChartCordinates is used to update xyseries, the other api response is received and it updates the vector
  QVector<QPointF>                          m_bagsPerHourCordinates;                          //!< vector for bag rate chart coordinates
  QVector<QPointF>                          m_suspectRateCordinates;                    //!< vector for suspect rate chart coordinates
  QVector<QPointF>                          m_recentOperatorTimeCordinates;                    //!< vector for suspect rate chart coordinates
  QVector<QPointF>                          m_recentTimeToDivertCordinates;                    //!< vector for suspect rate chart coordinates
  QVector<SupervisorAgents>                 m_supervisorAgents;                         //!< vector for agents
  QVector<SupervisorAgents>                 m_supervisorScannerDetailInfo;              //!< vector for scanner detail
  QVector<SupervisorAgents>                 m_supervisorPvsDetailInfo;                  //!< vector for pvs detail
  QVector<SupervisorAgents>                 m_supervisorSvsDetailInfo;                  //!< vector for svs detail
  QVector<SupervisorGroupOverview>          m_supervisorGroupOverview;                  //!< vector for group overview
  QVector<SupervisorPinnedEvents>           m_supervisorGroupOverviewPinnedEvents;      //!< vector for pinned  events
  QVector<SupervisorRecentEvents>           m_supervisorRecentEvents;                   //!< vector for recent  events
  DashBoardScreenModel                      m_dashBoardScreenModel;                    //!< object for dashboardscreenmodel to set Nodes count
  SupervisorAgentsDataModel*                m_agentsDataModel;                               //!< object for supervisoragentsdatamodel
  SupervisorEventsDataModel                 *m_supervisorRecentEventsmodel;             //!< object for SupervisorRecentEventsmodel
  int                                       m_pvsCount;                                 //!< to store READY Pvs count
  int                                       m_scannerCount;                             //!< to store READY Scanner count
  int                                       m_svsCount;                                 //!< to store READY SVS count
  int                                       m_maxPvsCount;                              //!< to store total Pvs count from API
  int                                       m_maxScannerCount;                          //!< to store total Scanner count from API
  int                                       m_maxSvsCount;                              //!< to store total Svs count from API
  GroupDataModel*                           m_groupDataModel;                           //!< Holds an instance of GroupDataModel
  QVector<SupervisorGroups>                 m_supervisorGroups;                         //!< vector of groups api
  QString                                   m_selectedGroup;                            //!< holds value of selected group from drow down list qml
  QVariantList                              m_chartsAverageDataList;                    //!< holds average of line charts
  float                                     m_supervisorScannerUtilization;             //!< holds value of scanner utilization data
  float                                     m_supervisorPvsUtilization;                 //!< holds value of pvs utilization data
  float                                     m_supervisorSvsUtilization;                 //!< holds value of svs utilization data
  QString                                   m_currentGroupUuid;                         //!< holds vaalue of current group uuid
  QString                                   m_currentAgentUuid;                         //!< holds vaalue of current agent uuid
  QMap<int, QAbstractSeries*>               m_lineChartSeriesMap;                       //!< holds line chart series
  QMap<int,QPair<QDateTime, QDateTime>>     m_chartTimeMap;                             //!< store utc start time and end time, m_chartTimeMap.first = startTime and m_chartTimeMap.second = endTime
  int                                       m_currentAgentsType;

};
}
}
#endif // MODELMANAGERSUPERVISOR_H
