/*!
* @file     viewmanager.cpp
* @author   Agiliad
* @brief    This file contains functions related to ViewManager
*           which managing all views.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/view.h>
#include <analogic/ws/uihandler/viewmanager.h>
#ifdef WORKSTATION
#include <analogic/ws/common/faults/bhspickupnotificationdata.h>
#endif

//For supervisor workstation - to add data into charts
#include <QtCharts/QXYSeries>

namespace analogic
{
namespace ws
{
/*!
* @fn       ViewManager(QObject* parent = NULL);
* @param    QObject* - parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
ViewManager::ViewManager(QObject* parent):QObject(parent)
{
  DEBUG_LOG("Creating instance of View and connection signals with resp slots");

  m_hView.reset(new View(this));
  THROW_IF_FAILED((m_hView.use_count() == 0) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
#ifdef WORKSTATION
  BHSPickupNotificationData::insertBhsPickupData();

  if(WorkstationConfig::getInstance()->getCurrentLanuage() == "English")
  {
    std::string sFilePath = WorkstationConfig::getInstance()->getexecutablePath() + "/" + BHS_WARNING_MESSAGE_FILE_NAME;
    QList<FaultMapXMLDataStruct> bhsWarningList = Utility::updateScannerFaultWarningData(sFilePath);
    QMap <QString, BhsTrayPickupInfo> *bhsPickupInfo = BHSPickupNotificationData::getBhsPickupData();
    for(int i = 0; i < bhsWarningList.count(); i++)
    {
      FaultMapXMLDataStruct updatedWarningStructure= bhsWarningList.at(i);

      BhsTrayPickupInfo faultInfo = bhsPickupInfo->value(updatedWarningStructure.faultid);
      if((faultInfo.useraction != updatedWarningStructure.faultAction)
         || (faultInfo.description != updatedWarningStructure.faultMessage))
      {
        faultInfo.description = updatedWarningStructure.faultMessage;
        faultInfo.useraction = updatedWarningStructure.faultAction.remove("\\n");
        bhsPickupInfo->insert(updatedWarningStructure.faultid, faultInfo);
      }
    }
  }
#endif
}

/*!
* @fn       ~ViewManager
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
ViewManager::~ViewManager()
{
  DEBUG_LOG("Destroying members");
  m_hView.reset();
}

/*!
* @fn       Initialize
* @param    None
* @return   None
* @brief    Responsible for intialization of managers and controllers.
*/
void ViewManager::initialize()
{
  DEBUG_LOG("Start of ViewManager::initialize()");
  DEBUG_LOG("Initialize View");
  m_hView->initialize();
  DEBUG_LOG("Initialize Screen Map");
  intializeScreenMap();

  connect(m_hView.get(), &View::setAuthenticatedUserDetailsToScanner,
          this, &ViewManager::setAuthenticatedUserDetailsToScanner);
  connect(m_hView.get(), &View::notifyBagIsPickedUpsig, this, &ViewManager::notifyBagIsPickedUpsig);
  connect(m_hView.get(), SIGNAL(currentViewChanged(int)), this, SLOT(onScreenChanged(int)));
  connect(m_hView.get(), SIGNAL(commandButtonEvent(QMLEnums::ModelEnum, QString)),
          this, SLOT(onCommandButton(QMLEnums::ModelEnum, QString)));
  connect(m_hView.get(), &View::exit, this, &ViewManager::onExit);
  connect(m_hView.get(), SIGNAL(bagListCommand(QMLEnums::ArchiveFilterSrchLoc,
                                               QMLEnums::BagsListAndBagDataCommands, QVariant)), this, SIGNAL(
            bagListCommand(QMLEnums::ArchiveFilterSrchLoc, QMLEnums::BagsListAndBagDataCommands,
                           QVariant)), Qt::QueuedConnection);

  connect(m_hView.get(), SIGNAL(updateChartsCordinates(QAbstractSeries*,int)),
          this, SLOT(onUpdateChartsCordinates(QAbstractSeries*,int)));

  connect(m_hView.get(), SIGNAL(getSelectedGroup(QString)),
          this, SLOT(onGetSelectedGroup(QString)));

  connect(m_hView.get(), SIGNAL(processUpdateModels()),
          this, SIGNAL(processUpdateModels()));

  connect(m_hView.get(), SIGNAL(getSelectedGauge(int)),
          this, SIGNAL(getSelectedGauge(int)));

#ifdef WORKSTATION
  connect(m_hView.get(), &View::startScannerHeartBeat, this, &ViewManager::onStartScannerHeartBeat);
#endif
  connect(this, &ViewManager::notifyWSConfigUpdated, m_hView.get(),
          &View::onUpdateConfigParameters);
  connect(this, &ViewManager::bagListCommandResults, m_hView.get(),
          &View::bagListCommandResults, Qt::QueuedConnection);

  connect(this, &ViewManager::bagRetain, m_hView.get(),
          &View::bagRetain, Qt::QueuedConnection);

  // Connections for SearchWorkstation
  connect(this, SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), m_hView.get(),
          SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::QueuedConnection);

  INFO_LOG("Qml File Load success");
  DEBUG_LOG("End of ViewManager::initialize()");
}

/*!
* @fn       getViewObject();
* @param    None
* @return   boost::shared_ptr<View>
* @brief    This function responsible for getting view object
*/
boost::shared_ptr<View> ViewManager::getViewObject()
{
  DEBUG_LOG("Returning  handle to view object");
  return m_hView;
}

/*!
* @fn       onModelDataChanged();
* @param    QVariantList model data
* @param    QMLEnums::ModelEnum
* @return   None
* @brief    This function responsible for changing model data.
*/
void ViewManager::onModelDataChanged(QVariantList oData, QMLEnums::ModelEnum sModelName)
{
  DEBUG_LOG("Request to update modellist for modelname: "<< QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(sModelName));
  m_hView->updatemodellist(sModelName, oData);
}

/*!
* @fn       getModelNames();
* @param    None
* @return   ModelList model names
* @brief    This function responsible for getting model names.
*/
ModelList ViewManager::getModelNames()
{
  DEBUG_LOG("Getting model name list");
  THROW_IF_FAILED((m_hView.use_count() == 0)?Errors::E_POINTER:Errors::S_OK);
  QMLEnums::ScreenEnum sCurrentView = (QMLEnums::ScreenEnum )m_hView->getCurrentView();
  if(sCurrentView == QMLEnums::SCANNERADMIN_SCREEN)
  {
    QMLEnums::ScannerAdminScreenEnum sCurrentView =
        (QMLEnums::ScannerAdminScreenEnum )m_hView->getScanneradminview();
    if (m_screenModelMap.find(sCurrentView)!= m_screenModelMap.end())
      return m_screenModelMap[sCurrentView];
  }

  return ModelList();
}

/*!
* @fn       onScreenChanged();
* @param    None
* @return   None
* @brief    This function responsible for changing views.
*/
void ViewManager::onScreenChanged(int currentView)
{
  Q_UNUSED(currentView);
  DEBUG_LOG("Notify about the screen change event.")
  ModelList oModelNames = getModelNames();
  DEBUG_LOG("Changed screen have model count: "<< oModelNames.count());
  emit screenChanged( oModelNames );
}

/*!
* @fn       onCommandButton();
* @param    QMLEnums::ModelEnum
* @param    QString
* @return   None
* @brief    This function responsible for sending signals.
*/
void ViewManager::onCommandButton(QMLEnums::ModelEnum modelname, QString data)
{
  if(((modelname == QMLEnums::NETWORK_AUTHENTICATION_MODEL) ||
      (modelname == QMLEnums::RERUN_EMULATOR_AUTHENTICATION_MODEL)) && (data == "logout"))
  {
    DEBUG_LOG("Request to reauthenticate.");
    emit deAuthenticate();
  }
  else
  {
    DEBUG_LOG("Notify about command button clciked event and process the data for modelname: "<<
              QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(modelname));
    emit commandButtonEvent(modelname, data);
  }
}

/*!
* @fn      updateChartsCordinates
* @param   QAbstractSeries
* @return  None
* @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
*/
void ViewManager::onUpdateChartsCordinates(QAbstractSeries *series, int modelnum)
{
  emit updateChartsCordinates(series, modelnum);
}

/*!
* @fn       onDataTimerTick();
* @param    None
* @return   None
* @brief    This function responsible for timer.
*/
void ViewManager::onDataTimerTick()
{
  DEBUG_LOG("Requesting refresh ui models");
  m_hView->refreshUiModels();
}


/*!
* @fn       onauthstatuschanged
* @param    int - error
* @param    QString - status
* @param    int - modelno
* @return   None
* @brief    called on authentication status changed
*/
void ViewManager::onauthstatuschanged(int hr, QString status, int modelno)
{
  DEBUG_LOG("Autherization status changed. hr: "<<hr <<" Status: "<< status.toStdString() << "modelno :" <<
            QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(modelno));
  m_hView->onauthstatuschanged(hr, status, modelno);
}

/*!
* @fn       onGetSelectedGroup
* @param    QString - status
* @return   None
* @brief    called on group is selected
*/
void ViewManager::onGetSelectedGroup(QString selectedGroup)
{
  emit getSelectedGroup(selectedGroup);
}
#if defined WORKSTATION || defined RERUN
/*!
* @fn       onsslerrorreceived
* @param    int - error
* @param    QString - status
* @param    int - modelno
* @return   None
* @brief    called on ssl received
*/
void ViewManager::onsslerrorreceived(int hr, QString status, int modelno)
{
  DEBUG_LOG("onsslerrorreceived. hr: "<<hr <<" Status: "<< status.toStdString() << "modelno :" << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(modelno));
  m_hView->onsslerrorreceived(hr, status, modelno);
}
#endif
/*!
* @fn       initializeScreenMap();
* @param    None
* @return   None
* @brief    This function responsible for initializing screen map.
*/
void ViewManager::intializeScreenMap()
{
  TRACE_LOG("");
  m_screenModelMap[QMLEnums::STATUS_SCREEN] = ModelList();
  m_screenModelMap[QMLEnums::STATUS_SCREEN] << QMLEnums::KEYCONSOLE_MODEL
                                            << QMLEnums::ACCESSPANEL_MODEL
                                            << QMLEnums::ESTOPS_MODEL
                                            << QMLEnums::SAFETYCONTROLLERFAULTS_MODEL
                                            << QMLEnums::POWERLINKINVERTERDISCRETESTATUS_MODEL
                                            << QMLEnums::POWERLINKINVERTERSTATUS_MODEL
                                            << QMLEnums::XRAYDATA_MODEL
                                            << QMLEnums::XRAYSTATUS_MODEL
                                            << QMLEnums::XRAYTIME_MODEL
                                            << QMLEnums::XRAYFAULTS_MODEL
                                            << QMLEnums::MOTIONCONTROLDATA_MODEL
                                            << QMLEnums::SCANNERTEMPHUMIDITY_MODEL
                                            << QMLEnums::CONVEYORANDGANTRY_MODEL
                                            << QMLEnums::SCANNERBAGPROCESSEDMODEL
                                            << QMLEnums::MISCELLANEOUS_MODEL;

  m_screenModelMap[QMLEnums::RADIATIONSURVEY_SCREEN] = ModelList();
  m_screenModelMap[QMLEnums::RADIATIONSURVEY_SCREEN] << QMLEnums::XRAYDATA_MODEL
                                                     << QMLEnums::XRAYTIME_MODEL;
}

/*!
* @fn       onExit();
* @param    None
* @return   None
* @brief    Slot will call on close application
*/
void ViewManager::onExit()
{
  DEBUG_LOG("Notifiying application exit");
  emit exit();
}

/*!
* @fn       onfileDatadownloadFinished();
* @param    QString - sDownloadPath
* @param    QString - sDownloadError
* @param    QMLEnums::ModelEnum modelnum
* @return   None
* @brief    Slot will call on when filedatadownload is finished
*/
void ViewManager::onfileDatadownloadFinished(QString sDownloadPath, QString sDownloadError,
                                             QMLEnums::ModelEnum modelnum)
{
  DEBUG_LOG("Notify qml models about file data download is finished.");
  m_hView->sendfileDownloadfinshedSignal(sDownloadPath, sDownloadError, modelnum);
}


/*!
* @fn       onUpdateModelsig
* @param    QList<QObject*> qlist
* @return   None
* @brief    Slot will call when model list is updated according to current view.
*/
void ViewManager::onUpdateModelsig(QList<QObject*> qlist)
{
  DEBUG_LOG("Request to initalize ulm data list with the contents of qlist provided.");
  m_hView->setUlmDataList(qlist);
}

/*!
* @fn       onNetworkStatus
* @param    QMLEnums::NetworkState
* @return   None
* @brief    Slot calls on notification about status of network from workstationManager.
*/
void ViewManager::onNetworkStatus(QMLEnums::NetworkState status)
{
  DEBUG_LOG("Notify qml models about network status.");
  emit m_hView->networkStatus(status);
}

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
void ViewManager::onRequestBagPickup(const std::string& id_assigning_authority, const string &primary_bag_id, const string &rfid, const std::string& pickup_type, QMLEnums::BhsDiverterDecision
                                     diverter_decision_code, const string &diverter_decision_string)
{
  BhsTrayPickupInfo bhsPickInfo = BHSPickupNotificationData::getBhsPickupData()->value(pickup_type.c_str());
  const QString message = bhsPickInfo.description;
  const QString userAction = bhsPickInfo.useraction;
  m_hView->requestBagPickupsig(id_assigning_authority.c_str(), primary_bag_id.c_str(), rfid.c_str(), pickup_type.c_str(), diverter_decision_code, diverter_decision_string.c_str(), message, userAction);
}

/*!
* @fn       onStartScannerHeartBeat
* @param    bool start - enable/disable heart beat
* @return   None
* @brief    Slot call to start heart beat request on scanner server.
*/
void ViewManager::onStartScannerHeartBeat(bool start)
{
  ModelList oModelNames = getModelNames();
  if(start)
    oModelNames<<QMLEnums::SDSW_HEARTBEAT_MODEL;
  emit screenChanged( oModelNames );
}

/*!
* @fn       onSdswClientExited
* @param    int exitCode
* @return   None
* @brief    Slot call on sdswclient logout/exit.
*/
void ViewManager::onSdswClientExited(int exitCode)
{
  m_hView.get()->setMaintenanceMode(false);
}
#endif
}  // namespace ws
}  // namespace analogic
