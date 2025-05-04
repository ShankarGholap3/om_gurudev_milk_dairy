/*!
* @file     viewmanager.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ViewManager
*           which managing all views.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_UIHANDLER_VIEWMANAGER_H_
#define ANALOGIC_WS_UIHANDLER_VIEWMANAGER_H_

#include <QVariant>
#include <boost/shared_ptr.hpp>
#include <analogic/ws/common.h>

#include <QtCharts/QAbstractSeries>
QT_CHARTS_USE_NAMESPACE
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class View;
class BagData;

/*!
 * \class   ViewManager
 * \brief   This class contains variable and function related to
 *          managing all views.
 */
class ViewManager : public QObject
{
  Q_OBJECT
public:
  /*!
    * @fn       ViewManager(QObject* parent = NULL));
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
  explicit ViewManager(QObject* parent = NULL);

  /*!
    * @fn       ~ViewManager
    * @param    None
    * @return   None
    * @brief    Destructor responsible for deinitialization of members, memory and resources.
    */
  virtual ~ViewManager();

  /*!
    * @fn       initialize
    * @param    None
    * @return   None
    * @brief    Responsible for intialization of managers and controllers.
    */
  void initialize();

  /*!
    * @fn       getModelNames();
    * @param    None
    * @return   ModelList - model names
    * @brief    This function responsible for getting model names.
    */
  ModelList getModelNames();

  /*!
    * @fn       getViewObject();
    * @param    None
    * @return   boost::shared_ptr<View>
    * @brief    This function responsible for getting view object
    */
  boost::shared_ptr<View> getViewObject();

signals:

  /*!
    * @fn       screenChanged();
    * @param    ModelList models
    * @return   None
    * @brief    This function responsible for changing views.
    */
  void screenChanged(ModelList models);

  /*!
    * @fn       commandButtonEvent
    * @param    QMLEnums::ModelEnum
    * @param    QString
    * @return   None
    * @brief    This function responsible for sending signals.
    */
  void commandButtonEvent(QMLEnums::ModelEnum, QString);

  /*!
    * @fn       GetSupervisorAgentsData
    * @param    QString
    * @return   None
    * @brief    This function responsible for sending signals for fetching agents api from the supervisor webservice
    */
  void getSupervisorAgentsData(QString sAPI);

  /*!
    * @fn      updateChartsCordinates
    * @param   QAbstractSeries
    * @return  None
    * @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
    */
  void updateChartsCordinates(QAbstractSeries *series,int modelnum);

  /*!
    * @fn       exit();
    * @param    None
    * @return   None
    * @brief    Signal will call on close application
    */
  void exit();

  /*!
    * @fn       deAuthenticate
    * @param    None
    * @return   None
    * @brief    signal to propagate call for Deauthenticate user
    */
  void deAuthenticate();

  /*!
    * @fn       bagListCommand
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @param    QVariant data
    * @return   None
    * @brief    Generic signal for archive processing from Model to View.
    */
  void bagListCommand(QMLEnums::ArchiveFilterSrchLoc,
                      QMLEnums::BagsListAndBagDataCommands command,
                      QVariant data);

  /*!
    * @fn       bagListCommandResults
    * @param    QMLEnums::ArchiveFilterSrchLoc srchLoc
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @param    QVariant results
    * @return   None
    * @brief    Generic signal for archive processing from View to Model.
    */
  void bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                             QMLEnums::BagsListAndBagDataCommands command,
                             QVariant results);

  /*!
    * @fn       notifyToChangeOrNotScreen
    * @param    bool toBeChanged
    * @param    QString erroMsg
    * @param    int hr
    * @return   None
    * @brief    notifies view to change screen to show Search bag.
    */
  void notifyToChangeOrNotScreen(bool toBeChanged, QString erroMsg, int hr);

  /*!
    * @fn       bagRetain
    * @param    None
    * @return   None
    * @brief    signal to retain bag
    */
  void bagRetain();

  /*!
    * @fn       notifyBagIsPickedUpsig
    * @param    QMLEnums::WSBasicCommandResult
    * @return   None
    * @brief    Signal To Notify Bag Is Picked Up.
    */
  void notifyBagIsPickedUpsig(QMLEnums::WSBasicCommandResult commandResult);

  /*!
     * @fn       setAuthenticatedUserDetailsToScanner
     * @param    None
     * @return   void
     * @brief    Signal to set Authenticated user details to scanner agent.
     */
  void setAuthenticatedUserDetailsToScanner();

  /*!
     * @fn       notifyWSConfigUpdated
     * @param    None
     * @return   void
     * @brief    Signal to notify config parameter are updated.
     */
  void notifyWSConfigUpdated();

  /*!
     * @fn       getSelectedGroup
     * @param    QString
     * @return   None
     * @brief    Signal to notify selected group.
     */
  void getSelectedGroup(QString selectedGroup);

  /*!
     * @fn       getSelectedGauge()
     * @param    QString
     * @return   None
     * @brief    signal emits when gauge is selected
     */
  void getSelectedGauge(int selectedGauge);

  /*!
    * @fn       processUpdateModels
    * @param    None
    * @return   None
    * @brief    process models.
    */
  void processUpdateModels();

public slots:    

  /*!
    * @fn       onModelDataChanged();
    * @param    QVariantList model data
    * @param    QMLEnums::ModelEnum
    * @return   None
    * @brief    This function responsible for changing model data.
    */
  void onModelDataChanged(QVariantList oData, QMLEnums::ModelEnum sModelName);

  /*!
    * @fn       onscreenChanged();
    * @param    None
    * @return   None
    * @brief    This function responsible for changing views.
    */
  void onScreenChanged(int currentView);

  /*!
    * @fn       commandButtonEvent();
    * @param    QMLEnums::ModelEnum
    * @param    QString
    * @return   None
    * @brief    This function responsible for sending signals.
    */
  void onCommandButton(QMLEnums::ModelEnum, QString);

  /*!
    * @fn      onUpdateChartsCordinates
    * @return  None
    * @brief   This function is responsible fupdating data of linechart
    */
  void onUpdateChartsCordinates(QAbstractSeries *series,int modelnum);


  /*!
    * @fn       onDataTimerTick();
    * @param    None
    * @return   None
    * @brief    This function responsible for timer.
    */
  void onDataTimerTick();

  /*!
    * @fn       onauthstatuschanged();
    * @param    int - error
    * @param    QString - status
    * @param    int - modelno
    * @return   None
    * @brief    called on authentication status changed
    */
  void onauthstatuschanged(int hr, QString status , int modelno);

  /*!
    * @fn       onGetSelectedGroup();
    * @param    QString - selectedGroup
    * @return   None
    * @brief    called on selection of group
    */
  void onGetSelectedGroup(QString selectedGroup);

#if defined WORKSTATION || defined RERUN
  /*!
    * @fn       onsslerrorreceived();
    * @param    int - error
    * @param    QString - status
    * @param    int - modelno
    * @return   None
    * @brief    called on onsslerrorreceived
    */
  void onsslerrorreceived(int hr, QString status , int modelno);
#endif
  /*!
    * @fn       onExit();
    * @param    None
    * @return   None
    * @brief    Slot will call on close application
    */
  void onExit();

  /*!
    * @fn       onfileDatadownloadFinished();
    * @param    QString - sDownloadPath
    * @param    QString - sDownloadError
    * @param    QMLEnums::ModelEnum - modelnum
    * @return   None
    * @brief    Slot will call when file download operation is finished.
    */
  void onfileDatadownloadFinished(QString sDownloadPath, QString sDownloadError, QMLEnums::ModelEnum modelnum);

  /*!
    * @fn       onUpdateModelsig
    * @param    QList<QObject*> qlist
    * @return   None
    * @brief    Slot will call when model list is updated according to current view.
    */
  void onUpdateModelsig(QList<QObject*> qlist);

  /*!
    * @fn       onNetworkStatus
    * @param    QMLEnums::NetworkState status
    * @return   None
    * @brief    Slot calls on notification about status of network from workstationManager.
    */
  void onNetworkStatus(QMLEnums::NetworkState status);
#ifdef WORKSTATION
  /*!
    * @fn       onRequestBagPickup
    * @param    std::string id_assigning_authority
    * @param    const std::string primary bag  ID
    * @param    const std::string rfid
    * @param    const QString pickup_type
    * @param    QMLEnums::BhsDiverterDecision
    * @param    const std::string diverter decision string
    * @return   None
    * @brief    Slot called when bag is ready for picked up.
    */
  void onRequestBagPickup(const std::string& id_assigning_authority, const std::string &primary_bag_id, const std::string &rfid,
                          const std::string& pickup_type, QMLEnums::BhsDiverterDecision diverter_decision_code, const std::string &diverter_decision_string);

  /*!
    * @fn       onStartScannerHeartBeat
    * @param    bool start - enable/disable heart beat
    * @return   None
    * @brief    Slot call to start heart beat request on scanner server.
    */
  void onStartScannerHeartBeat(bool start);

  /*!
    * @fn       onSdswClientExited
    * @param    int exitCode
    * @return   None
    * @brief    Slot call on sdswclient logout/exit.
    */
  void onSdswClientExited(int exitCode);
#endif    
private:
  /*!
    * @fn       initializeScreenMap();
    * @param    None
    * @return   None
    * @brief    This function responsible for initializing screen map.
    */
  inline void intializeScreenMap();

  /*!
    * @fn       ViewManager();
    * @param    ViewManager&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
  ViewManager(const ViewManager& viewManager);

  /*!
    * @fn       operator=
    * @param    ViewManager&
    * @return   ViewManager&
    * @brief    declaration for private assignment operator.
    */
  ViewManager& operator= (const ViewManager& viewmanager);

  boost::shared_ptr<View>                                 m_hView;           //!< handle for view
  QMap< QMLEnums::ScannerAdminScreenEnum, ModelList>      m_screenModelMap;  //!< map for models per screen

};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_VIEWMANAGER_H_

