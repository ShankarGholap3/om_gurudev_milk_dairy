/*!
* @file     view.cpp
* @author   Agiliad
* @brief    This file contains functions related to View
*           which handles models of views.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#include <errors.h>
#include <analogic/ws/uihandler/view.h>
#include <analogic/ws/ulm/dataobject.h>
#include <analogic/ws/ulm/uilayoutmanager.h>
#include <analogic/ws/common/bagdata/baginfo.h>
#include <analogic/ws/uihandler/idlescreendetector.h>
#include <analogic/ws/uihandler/volumedisplaymanager.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>


#ifdef WEBENGINE
#include <analogic/ws/uihandler/webengineurlrequestinterceptor.h>
#endif
#include <analogic/ws/uihandler/configbaseelement.h>
#include <analogic/ws/common/utility/xmlservice.h>
#ifdef WORKSTATION
#include <rialcommon.h>
#include <analogic/ws/uihandler/workstationconfigdao.h>
#include <analogic/ws/common/accesslayer/scanneragentstatushandler.h>
#endif

#include <analogic/ws/SSLErrorEnums.h>
#include<sys/stat.h>
//For supervisor workstation - to add data into charts
#include <QtCharts/QXYSeries>


namespace analogic
{
namespace ws
{
View* View::m_view = NULL;

/*!
* @fn       View();
* @param    QObject* - parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
View::View(QObject *parent):QObject(parent),
  m_sCurentView(-1),
  m_qmlContainer(nullptr),
  m_qmlModalFrameContainer(nullptr),
  m_qmlScannerFaultContainer(nullptr),
  m_qmlOperatorThreatOptions(nullptr),
  m_qmlNavigationMenu(nullptr),
  m_qmlBalloonPopup(nullptr)
{
  // qRegisterMetaType<QAbstractSeries*>();
  // qRegisterMetaType<QAbstractAxis*>();
  m_hidleState = nullptr;
  m_mainWindow = new QWidget(nullptr);
  THROW_IF_FAILED((m_mainWindow == nullptr)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_mainWindow->setWindowState(Qt::WindowFullScreen);
  m_mainWindow->setFocusPolicy(Qt::StrongFocus);
  m_mainWindow->setGeometry(QApplication::desktop()->screenGeometry());

  DEBUG_LOG("Initializing members of view class.");
  m_volDispMgr = new VolumeDisplayManager(this);
  THROW_IF_FAILED((m_volDispMgr == nullptr)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_qmlContainer = new QQuickView(m_mainWindow->windowHandle());
  THROW_IF_FAILED((m_qmlContainer == nullptr)? Errors::E_OUTOFMEMORY:Errors::S_OK);

  m_qmlContainer->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
  m_qmlModalFrameContainer = new QQuickView(m_mainWindow->windowHandle());
  THROW_IF_FAILED((m_qmlModalFrameContainer == nullptr)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_qmlScannerFaultContainer = new QQuickView(m_mainWindow->windowHandle());
  THROW_IF_FAILED((m_qmlScannerFaultContainer == nullptr)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_qmlOperatorThreatOptions = new QQuickView(m_mainWindow->windowHandle());
  THROW_IF_FAILED((m_qmlOperatorThreatOptions == nullptr)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_qmlNavigationMenu = new QQuickView(m_mainWindow->windowHandle());
  THROW_IF_FAILED((m_qmlNavigationMenu == nullptr)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_qmlBalloonPopup = new QQuickView(m_mainWindow->windowHandle());
  THROW_IF_FAILED((m_qmlBalloonPopup == nullptr)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_fileDlg = nullptr;
  m_groupShow = nullptr;
  m_groupHide = nullptr;
  m_centralWidget = nullptr;
  m_containerModalFrame = nullptr;
  m_containerScannerFault = nullptr;
  m_printdlg = nullptr;
  setCurrModalFrame(-1);
  m_useradminloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  m_reportadminloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  m_networkloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  m_scannerloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  m_rerunloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  m_tipconfigadminloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  m_tiplibconfigadminloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  m_antivirusserviceloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  m_supervisorLoginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
#ifdef WEBENGINE
  m_isWebengineInitialized = false;
#endif
  m_linuxTerminal = new LinuxTerminal();
}

/*!
* @fn       initialize
* @param    None
* @return   None
* @brief    initialize members of class View.
*/
void View::initialize()
{
  TRACE_LOG("");
  DEBUG_LOG("Creating instance of Idle screen detector");

  m_hidleState = new IdleScreenDetector();
  THROW_IF_FAILED((m_hidleState == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

  connect(m_hidleState, &IdleScreenDetector::mouseClickedEvent, this, &View::onMousueClickedEvent);
  m_utility = new Utility();
  THROW_IF_FAILED((m_utility == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
  m_workstationConfig = WorkstationConfig::getInstance();


  m_fileSystemItemModel = new FileSystemItemModel();
  THROW_IF_FAILED((m_fileSystemItemModel == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

  m_agentsDataModel = SupervisorAgentsDataModel::getSupervisorAgentsDataModelInstance ();//new SupervisorAgentsDataModel();
  THROW_IF_FAILED((m_agentsDataModel == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

  m_recentEventsDataModel = SupervisorEventsDataModel::getSupervisorEventsDataModelInstance();
  THROW_IF_FAILED((m_recentEventsDataModel == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

  m_dashBoardScreenModel = new DashBoardScreenModel();
  THROW_IF_FAILED((m_dashBoardScreenModel == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

  m_groupDataModel = GroupDataModel::getGroupDataModelInstance();
  THROW_IF_FAILED((m_groupDataModel == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

  m_supervisorConfigGroupModel = SupervisorConfigDataModel::getSupervisorConfigGroupInstance();
  THROW_IF_FAILED((m_supervisorConfigGroupModel == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

  m_supervisorConfigMemberModel = SupervisorConfigDataModel::getSupervisorConfigMemberInstance();
  THROW_IF_FAILED((m_supervisorConfigMemberModel == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

  m_supervisorConfigNonMemberModel = SupervisorConfigDataModel::getSupervisorConfigNonMemberInstance();
  THROW_IF_FAILED((m_supervisorConfigNonMemberModel == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
#ifdef RERUN
  m_rerunBatchManager = new RerunBatchManager();
  THROW_IF_FAILED((m_rerunBatchManager == nullptr)? Errors::E_OUTOFMEMORY : Errors::S_OK);
  m_rerunFormatConverter = new RerunFormatConverter();
  THROW_IF_FAILED((m_rerunFormatConverter== nullptr)? Errors::E_OUTOFMEMORY : Errors::S_OK);
  m_rerunConfigObj= new RerunConfig();
  THROW_IF_FAILED((m_rerunConfigObj== nullptr)? Errors::E_OUTOFMEMORY : Errors::S_OK);


#endif
  DEBUG_LOG("Registering Enums for use in qmls");
  // Register Enums which will be used for qmls
  registerDataTypesToQml();

#ifdef WORKSTATION
  m_pTipFileSystemUIModel = new TipFileSystemModel();
  m_pTipFileSystemUIModel->setModel(m_pTipFileSystemUIModel);
#endif

  DEBUG_LOG("Setting current view and previous view to LOGIN SCREEN");
  setCurrentView(QMLEnums::LOGIN_SCREEN);
  setPrevView(QMLEnums::LOGIN_SCREEN);
  setScanneradminview(QMLEnums::STATUS_SCREEN);

  DEBUG_LOG("Creating Centralwidget");
  m_centralWidget = new QWidget(m_mainWindow);
  THROW_IF_FAILED((m_centralWidget == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
  m_centralWidget->setGeometry(QApplication::desktop()->screenGeometry());
  m_rootWindow.setWidth(QApplication::desktop()->screenGeometry().width());
  m_rootWindow.setHeight(QApplication::desktop()->screenGeometry().height());
#ifdef WEBENGINE
  DEBUG_LOG("Setting context.");
  if ((getWorkstationNameEnum() == QMLEnums::WSType::OSR_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::SEARCH_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::SUPERVISOR_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION))
  {
    m_requestInterceptor = new WebEngineUrlRequestInterceptor();
    THROW_IF_FAILED((m_requestInterceptor == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);
    m_authToken.clear();
  }
#endif
  m_qmlContainer->rootContext()->setContextProperty("viewobject", this);
  m_qmlContainer->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
  m_qmlContainer->rootContext()->setContextProperty("WorkstationVolumeRenderer", m_volDispMgr);
  m_qmlContainer->rootContext()->setContextProperty("idlescreenobject", m_hidleState);
  m_qmlContainer->rootContext()->setContextProperty("utility", m_utility);
  m_qmlContainer->rootContext()->setContextProperty("workstationConfig", m_workstationConfig);
  m_qmlContainer->rootContext()->setContextProperty("fileSystemItemModel", m_fileSystemItemModel);
  m_qmlContainer->rootContext()->setContextProperty("dashBoardScreenModel",m_dashBoardScreenModel);
  m_qmlContainer->rootContext()->setContextProperty("agentsDataModel", m_agentsDataModel);
  m_qmlContainer->rootContext()->setContextProperty("recentEventsDataModel", m_recentEventsDataModel);
  m_qmlContainer->rootContext()->setContextProperty("groupDataModel", m_groupDataModel);
  m_qmlContainer->rootContext()->setContextProperty("supervisorConfigNotMemberModel", m_supervisorConfigNonMemberModel);
  m_qmlContainer->rootContext()->setContextProperty("supervisorConfigGroupingModel", m_supervisorConfigMemberModel);
  m_qmlContainer->rootContext()->setContextProperty("supervisorConfigGroupModel", m_supervisorConfigGroupModel);

#ifdef RERUN
  m_qmlContainer->rootContext()->setContextProperty("rerunBatchManager", m_rerunBatchManager);
  m_qmlContainer->rootContext()->setContextProperty("rerunFormatConverter", m_rerunFormatConverter);
#endif

#ifdef WORKSTATION
  m_qmlContainer->rootContext()->setContextProperty("tipFileSystemModel", m_pTipFileSystemUIModel);
  m_qmlContainer->engine()->addImageProvider(QLatin1String("rawimage"),
                                             new RawImage());

  connect(ScannerAccessInterface::getInstance(),
          &ScannerAccessInterface::bagReceived,
          m_volDispMgr,
          &VolumeDisplayManager::bagDataReceived,
          Qt::QueuedConnection);
#endif

  // This connect is to have a non blocking execution of setMaterialFilterCutoff()

  connect(this, &View::densitsliderChanged, m_volDispMgr,
          &VolumeDisplayManager::setMaterialFilterCutoff, Qt::QueuedConnection);

  DEBUG_LOG("Setting qml container source to: qrc:/authentication/MainScreen.qml");
  m_qmlContainer->setSource(QUrl(QStringLiteral("qrc:/authentication/MainScreen.qml")));
  m_qmlContainer->setGeometry(QApplication::desktop()->screenGeometry());
  m_qmlContainerWidget = QWidget::createWindowContainer(m_qmlContainer);
  m_qmlContainerWidget->setFocusPolicy(Qt::StrongFocus);
  m_qmlContainerWidget->setGeometry(QApplication::desktop()->screenGeometry());
  m_qmlContainerWidget->setParent(m_centralWidget);
  m_qmlContainer->setResizeMode(QQuickView::SizeRootObjectToView);
  m_qmlContainerWidget->setFocus(Qt::ActiveWindowFocusReason);
  m_qmlContainerWidget->lower();
  m_VRTopwidget = new QWidget(m_centralWidget);
  THROW_IF_FAILED((m_VRTopwidget == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);

  createOperatorThreatPopupScreen();
  createNavigationMenuPopupScreen();
  createNotificationBalloonPopup();
  m_mainWindow->show();

  DEBUG_LOG("Initialize VolumeDisplayManager");
  m_volDispMgr->initialize(m_qmlContainer, m_VRTopwidget);



  QCoreApplication* app = QApplication::instance();
  if(app)
  {
    DEBUG_LOG("Install event filter");
    app->installEventFilter(m_hidleState);
  }

#ifdef WEBENGINE
  DEBUG_LOG("Initialize QTWebEngine");
  QtWebEngine::initialize();
  m_isWebengineInitialized = true;
#endif
}

#ifdef WEBENGINE
void View::initializeWebEngine()
{
  if(!m_isWebengineInitialized)
  {
    DEBUG_LOG("Initialize QTWebEngine");
    QtWebEngine::initialize();
  }
}

#endif


/*!
* @fn       registerULMcontext
* @param    UILayoutManager*
* @return   None
* @brief    registers ULM context.
*/
void View::registerULMcontext(UILayoutManager *ulmObject)
{
  TRACE_LOG("");
  m_qmlContainer->rootContext()->setContextProperty("ulmObject", ulmObject);
  m_qmlContainer->rootContext()->setContextProperty("screenCtrList",
                                                    QVariant::fromValue(ulmObject->getulmDatalist()));
  m_qmlModalFrameContainer->rootContext()->setContextProperty("ulmObject", ulmObject);
  m_qmlNavigationMenu->rootContext()->setContextProperty("ulmObject", ulmObject);
}

/*!
* @fn       registerUsbFSModel
* @param    UsbFileSystemModel*
* @return   None
* @brief    registers UsbFS context.
*/
void View::registerUsbFSModel(UsbFileSystemModel *usbObject)
{
  m_qmlContainer->rootContext()->setContextProperty("usbfileSystemModel", usbObject);
}

/*!
* @fn      ~View
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
View::~View()
{
  TRACE_LOG("");

  m_qmlModalFrameContainer->close();
  m_qmlModalFrameContainer->engine()->quit();
  SAFE_DELETE(m_qmlModalFrameContainer);

  m_qmlScannerFaultContainer->close();
  m_qmlScannerFaultContainer->engine()->quit();
  SAFE_DELETE(m_qmlScannerFaultContainer);

  m_qmlOperatorThreatOptions->close();
  m_qmlOperatorThreatOptions->engine()->quit();
  SAFE_DELETE(m_qmlOperatorThreatOptions);

  m_qmlNavigationMenu->close();
  m_qmlNavigationMenu->engine()->quit();
  SAFE_DELETE(m_qmlNavigationMenu);

  m_qmlContainer->close();
  m_qmlContainer->engine()->quit();
  SAFE_DELETE(m_qmlContainer);

  DEBUG_LOG("Starting to destroy ulm data list.");
  for (int i = 0; i < m_ulmDataList.length(); ++i) {
    if (NULL != m_ulmDataList.at(i))
      delete m_ulmDataList.at(i);
  }
  DEBUG_LOG("Finished destroying ulm data list.");

  DEBUG_LOG("Destroying idle screen handle.");
  SAFE_DELETE(m_hidleState);

  DEBUG_LOG("Destroying handle to volume display manager.");
  SAFE_DELETE( m_volDispMgr);
  SAFE_DELETE(m_utility);
#ifdef RERUN
  SAFE_DELETE(m_rerunBatchManager);
  SAFE_DELETE(m_rerunFormatConverter);
#endif
#ifdef WORKSTATION
  SAFE_DELETE(m_pTipFileSystemUIModel);
#endif
  if(NULL != m_groupShow)
    SAFE_DELETE(m_groupShow);
  if(NULL != m_groupHide)
    SAFE_DELETE(m_groupHide);
  if(NULL != m_centralWidget)
    SAFE_DELETE(m_centralWidget);
  if(NULL != m_mainWindow)
    SAFE_DELETE(m_mainWindow);
  if(NULL != m_linuxTerminal)
    SAFE_DELETE(m_linuxTerminal);

#ifdef WEBENGINE
  if ((getWorkstationNameEnum() == QMLEnums::WSType::OSR_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::SEARCH_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::SUPERVISOR_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION))
  {
    DEBUG_LOG("Destroying request interceptor.");
    SAFE_DELETE(m_requestInterceptor);
  }
#endif
}

/*!
* @fn      scannerSetState
* @param   QMLEnums::ScannerState state
* @return  None
* @brief   This function is responsible for setting scanner state
*/
void View::scannerSetState(QMLEnums::ScannerState state)
{
  DEBUG_LOG("Send signal to update scanner state to: " << state);
  emit scannerStateChanged(state);
}

/*!
* @fn       setCurrentView();
* @param    int sView of type QMLEnums::ScreenEnum
* @return   None
* @brief    called internally when currentView changes.
*/
void View::setCurrentView(int sView)
{
  // Notify all sub-components that need to update themselves on screen change.
  DEBUG_LOG("Changing from screen: " << convertModelEnumToString(m_sCurentView).toStdString() << " to screen: " << convertModelEnumToString(sView).toStdString());
  m_volDispMgr->screenChanged(QMLEnums::ScreenEnum(m_sCurentView), QMLEnums::ScreenEnum(sView));

  m_sCurentView = sView;
  DEBUG_LOG("Notify screen change event.");
  emit currentViewChanged(m_sCurentView);
}

/*!
* @fn       getCurrentView();
* @param    int - of type QMLEnums::ScreenEnum
* @return   None
* @brief    qt property that updates on screen chanes in qml.
*/
int View::getCurrentView() const
{
  DEBUG_LOG("Current screen is : " << convertModelEnumToString(m_sCurentView).toStdString());
  return m_sCurentView;
}

/*!
* @fn       setUlmDataList
* @param    UILayoutManager*
* @return   None
* @brief    sets Ulm Data List.
*/
void View::setUlmDataList(QList<QObject*> inputDataList)
{
  DEBUG_LOG("Starting to initialize ulm data list of length: " << inputDataList.length());
  for (int i = 0; i < inputDataList.length(); ++i) {
    m_ulmDataList.append(inputDataList.at(i));
  }
  DEBUG_LOG("Finished initializing ulm data list");
}

/*!
* @fn       setPrevView
* @param    int
* @return   None
* @brief    called internally when previous View changes.
*/
void View::setPrevView(int sView)
{
  DEBUG_LOG("Setting PrevView screen to: " << sView);
  m_ePrevView = sView;
  emit prevViewChanged();
}


/*!
* @fn       getPrevView
* @param    None
* @return   int
* @brief    gets previous view
*/
int View::getPrevView() const
{
  DEBUG_LOG("Getting Previous view :" << m_ePrevView);
  return m_ePrevView;
}
/*!
* @fn       onUpdateState();
* @param    QMLEnums::AppState state
* @return   void
* @brief    call on update state.
*/
void View::onUpdateState(QMLEnums::AppState state)
{
  DEBUG_LOG("Updating state to: " << state);
}

/*!
* @fn       notifyToStopShowingBag
* @param    None
* @return   None
* @brief    signals to stop showing bags.
*/
void View::notifyToStopShowingBag()
{
  m_volDispMgr->vtkSettingsChanged(QMLEnums::ThreatEvalPanelCommand::TEPC_STOP_BAG);
}

/*!
   * @fn       onUpdateConfigParameters;
   * @param    None
   * @return   None
   * @brief    updates configuration parameters
   */
void View::onUpdateConfigParameters()
{
  INFO_LOG("Sending EVENT_WORKSTATION_SETTINGS_UPDATED event to NSS");
  reportEventToNssAgent(QMLEnums::EVENT_WORKSTATION_SETTINGS_UPDATED,"vs settings");
  m_hidleState->updateInactivityTimeoutInterval();
}

/*!
* @fn      animateScannerFaultStatus
* @param   None
* @return  None
* @brief   This function is responsible for animating Scanner Fault Status Screen.
*/
void View::animateScannerFaultStatus()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlScannerFaultContainer->rootObject();
  DEBUG_LOG("Animate Scanner fault status");
  if(rootObject)
  {
    if (false == m_ScannerFaultScrnShown)
    {
      DEBUG_LOG("Set property to show fault screen");
      rootObject->setProperty("state", "shown");
      m_groupShow->start();
      m_ScannerFaultScrnShown = true;
    }
    else if (true == m_ScannerFaultScrnShown)
    {
      DEBUG_LOG("Set property to hide fault screen");
      rootObject->setProperty("state", "hidden");
      m_groupHide->start();
    }
  }
}

/*!
* @fn      showHideScannerFaultStatus
* @param   bool flag
* @return  None
* @brief   This function is responsible for showing/ hiding Scanner Fault Status Screen.
*/
void View::showHideScannerFaultStatus(bool flag)
{
  if (m_containerScannerFault == NULL)
    return;
  if(flag == m_ScannerFaultScrnShown)
  {
    return;
  }

  if (false == m_ScannerFaultScrnShown)
  {
    DEBUG_LOG("Showing scanner fault screen");
    emit startScannerHeartBeat(true);
    m_containerScannerFault->setFocus();
    m_containerScannerFault->show();
  }
  else if (true == m_ScannerFaultScrnShown)
  {
    DEBUG_LOG("Hiding scanner fault screen");
    emit startScannerHeartBeat(true);
    m_containerScannerFault->hide();
    m_ScannerFaultScrnShown = false;
    m_qmlContainerWidget->setFocus(Qt::ActiveWindowFocusReason);
  }
}

/*!
* @fn      getRootWindowWidth
* @param   int
* @return  None
* @brief   This function returns width of rootWindow.
*/
int View::getRootWindowWidth()
{
  return m_rootWindow.width();
}

/*!
* @fn      getRootWindowHeight
* @param   int
* @return  None
* @brief   This function returns width of rootWindow.
*/
int View::getRootWindowHeight()
{
  return m_rootWindow.height();
}

int View::getNavigationPanelWidth()
{
  DEBUG_LOG("Navigation Panel width : "<< m_navigationPanel.width());
  return m_navigationPanel.width();
}

/*!
* @fn       getLanguageCode
* @param    None
* @return   string  - Language
* @brief    gets Language Code
*/
QString View::getLanguageCode()
{
  return WorkstationConfig::getInstance()->getLanguageCode();
}

/*!
* @fn       getCurrentLanguageLocale
* @param    None
* @return   string  - Language
* @brief    gets Language locale
*/
QString View::getCurrentLanguageLocale()
{
  return WorkstationConfig::getInstance()->getCurrentLanguageLocale();
}

/*!
* @fn      getScanneradminview
* @param   None
* @return  QMLEnums::ScannerAdminScreenEnum
* @brief   This function is responsible for getting current scanner admin view
*/
QMLEnums::ScannerAdminScreenEnum View::getScanneradminview() const
{
  DEBUG_LOG("Scanner admin view is: " << m_scanneradminview);
  return m_scanneradminview;
}
/*!
* @fn      setScanneradminview
* @param   const int& scanneradminview
* @return  None
* @brief   This function is responsible for setting current scanner admin view
*/
void View::setScanneradminview(const int& scanneradminview)
{
  DEBUG_LOG("Setting Scanner admin view to: " << scanneradminview);
  m_scanneradminview = (QMLEnums::ScannerAdminScreenEnum)scanneradminview;
  emit currentViewChanged(m_sCurentView);
}

/*!
* @fn      createScannerFaultStatusScreen
* @param   None
* @return  None
* @brief   This function is responsible for creating Scanner Fault Status Screen.
*/
void View::createScannerFaultStatusScreen()
{
  TRACE_LOG("");
  m_durationofAniamtion = 50;
  DEBUG_LOG("Creating scanner fault status screen");

  QQuickItem *rootWindowItem = m_qmlContainer->rootObject();
  if(rootWindowItem)
  {
    m_scannerFaultScrnModel = rootWindowItem->findChild<QObject*>("scannerFaultModel");
    m_nwConnectionModel = rootWindowItem->findChild<QObject*>("networkConnectionModel");
    m_navigationModel = rootWindowItem->findChild<QObject*>("navigationmodel");
    if(m_scannerFaultScrnModel)
    {
      m_scannerFaultScrnModel->setProperty("durationOfAnimation", m_durationofAniamtion);
    }
    QQuickItem *navigationPanelItem = qobject_cast<QQuickItem*>(
          rootWindowItem->findChild<QObject*>("navigationPanel"));
    if(navigationPanelItem)
    {
      m_navigationPanel.setWidth(navigationPanelItem->property("width").toDouble());
      m_navigationPanel.setHeight(navigationPanelItem->property("height").toDouble());
    }
    navigationPanelItem = NULL;
  }
  rootWindowItem = NULL;

  // For Scanner Fault Status
  m_ScannerFaultScrnShown = false;
  m_qmlScannerFaultContainer->rootContext()->setContextProperty("viewobject", this);
  m_qmlScannerFaultContainer->rootContext()->setContextProperty("scannerFaultStatusModel", m_scannerFaultScrnModel);

  m_qmlScannerFaultContainer->setSource(QUrl(QStringLiteral("qrc:/common/screenareas/ScannerFaultStatus.qml")));
  m_qmlScannerFaultContainer->setResizeMode(QQuickView::SizeRootObjectToView);
  m_qmlScannerFaultContainer->setClearBeforeRendering(true);
  m_qmlScannerFaultContainer->setColor("#525252");
  m_containerScannerFault = QWidget::createWindowContainer(m_qmlScannerFaultContainer);
  m_containerScannerFault->setParent(m_centralWidget);
  int width = getRootWindowWidth()*0.50;
  int height = getRootWindowHeight()*0.35;
  int y = getRootWindowHeight()*0.423;
  int x = getNavigationPanelWidth() - 36;
  m_containerScannerFault->setGeometry(x, y, width, height);

  DEBUG_LOG("Creating animation group to show screen");
  // Creating animation group to show screen
  QPropertyAnimation *m_animationGeo1 = new QPropertyAnimation(m_containerScannerFault,
                                                               "geometry",
                                                               m_containerScannerFault);
  THROW_IF_FAILED((m_animationGeo1 == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_animationGeo1->setDuration(m_durationofAniamtion);
  m_animationGeo1->setStartValue(QRect(x, y, width, height));
  m_animationGeo1->setEndValue(QRect(x, y, width, height));

  m_groupShow = new QParallelAnimationGroup(m_containerScannerFault);
  THROW_IF_FAILED((m_groupShow == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_groupShow->addAnimation(m_animationGeo1);

  DEBUG_LOG("Creating animation group to hide screen");
  // Creating animation group to hide screen
  QPropertyAnimation *m_animationGeo2 = new QPropertyAnimation(m_containerScannerFault,
                                                               "geometry",
                                                               m_containerScannerFault);
  THROW_IF_FAILED((m_animationGeo2 == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_animationGeo2->setDuration(m_durationofAniamtion);
  m_animationGeo2->setStartValue(QRect(x, y, width, height));
  m_animationGeo2->setEndValue(QRect(x, y, width, height));

  m_groupHide = new QParallelAnimationGroup(m_containerScannerFault);
  THROW_IF_FAILED((m_groupHide == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_groupHide->addAnimation(m_animationGeo2);
}

/*!
* @fn      createModalFrame
* @param   None
* @return  None
* @brief   This function is responsible for creating Modal Framefor screen.
*/
void View::createModalFrame()
{
  // ModalFrame doesnot appear on top if it is encountered after rendering of bag,
  // for this, that qml is moved in QWidget
  if(m_containerModalFrame == NULL)
  {
    DEBUG_LOG("Creating modal frame in Qwidget");
    m_qmlModalFrameContainer->rootContext()->setContextProperty("viewobject", this);
    m_qmlModalFrameContainer->rootContext()->setContextProperty("nwConnectionModel", m_nwConnectionModel);
    m_qmlModalFrameContainer->rootContext()->setContextProperty("navigationModelData", m_navigationModel);
    m_qmlModalFrameContainer->setSource(QUrl(QStringLiteral("qrc:/common/component/QWidgetModalFrame.qml")));
    m_qmlModalFrameContainer->setResizeMode(QQuickView::SizeRootObjectToView);
    m_containerModalFrame = QWidget::createWindowContainer(m_qmlModalFrameContainer);
    m_containerModalFrame->setParent(m_centralWidget);
    m_containerModalFrame->setGeometry(QApplication::desktop()->screenGeometry());
    m_containerModalFrame->setWindowModality(Qt::ApplicationModal);
  }
}

/*!
  * @fn      createNotificationBalloonPopup
  * @param   None
  * @return  None
  * @brief   This function is responsible for creating Ballon popup.
  */
void View::createNotificationBalloonPopup()
{
  TRACE_LOG("");

  m_balloonPopupShown = false;
  m_qmlBalloonPopup->rootContext()->setContextProperty("viewobject", this);

  QQuickItem *rootWindowItem = m_qmlContainer->rootObject();
  if(rootWindowItem)
  {
    m_balloonPopupModel = rootWindowItem->findChild<QObject*>("balloonPopupModel");
    if (m_balloonPopupModel)
    {
      m_qmlBalloonPopup->rootContext()->setContextProperty(
            "balloonPopupModel", m_balloonPopupModel);
    }
  }

  m_qmlBalloonPopup->setSource(QUrl(QStringLiteral(
                                      "qrc:/common/screenareas/BalloonPopup.qml")));

  m_containerBalloonPopup = QWidget::createWindowContainer(m_qmlBalloonPopup);
  m_containerBalloonPopup->setParent(m_mainWindow);
  m_qmlBalloonPopup->setResizeMode(QQuickView::SizeRootObjectToView);
  int width = m_containerBalloonPopup->geometry().width();
  int height = m_containerBalloonPopup->geometry().height();
  QPoint point = QPoint(142, 200);
  m_containerBalloonPopup->setGeometry(point.x(),
                                       point.y(),
                                       width,
                                       height);
  m_containerBalloonPopup->lower();
}

/*!
* @fn       updatemodellist();
* @param    QMLEnums::ScreenEnum  - sModelName
* @param    QVariantList data - model data
* @return   void
* @brief    sends signal to update Models.
*/
void View::updatemodellist(QMLEnums::ModelEnum sModelName, QVariantList oModel)
{
  DEBUG_LOG("Updating modellist with oModel having length: "<< oModel.count());
  if (m_allUiDataModels.find(sModelName) != m_allUiDataModels.end()) {
    if (!isEqualVariantList(m_allUiDataModels[sModelName], oModel)) {
      oModel.prepend(QVariant::fromValue(1));
      m_allUiDataModels.insert(sModelName, oModel);
    }
  }
  else
  {
    // entry for sModelName is not present in map
    oModel.prepend(QVariant::fromValue(1));
    m_allUiDataModels.insert(sModelName, oModel);
  }
  if ((sModelName == QMLEnums::PARAMETER_MODEL)
      || (sModelName == QMLEnums::WORKSTATION_CONFIG_MODEL)
      || (sModelName == QMLEnums::COMMAND_TIP_CONFIG_MODEL)
      || (sModelName == QMLEnums::COMMAND_TIP_LIB_CONFIG_MODEL)
      || (sModelName == QMLEnums::PARAMETER_SEND_MODEL)
      || (sModelName == QMLEnums::COMMAND_UPDATE_TIP_CONFIG_SEND_MODEL)
      )
  {
    emit parametersXMLReceived(oModel, sModelName);
  }
}

/*!
* @fn       sendfileDownloadfinshedSignal();
* @param    QString sDownloadPath
* @param    QString sDownloadError
* @param    QMLEnums::ModelEnum sModelName
* @return   None
* @brief    sends signal  that download is finished.
*/
void View::sendfileDownloadfinshedSignal(QString sDownloadpath, QString sDownloadError, QMLEnums::ModelEnum modelnum)
{
  switch(modelnum)
  {
  case QMLEnums::COMMAND_REPORT_MODEL:
    emit reportDownloadFinished(sDownloadpath, sDownloadError);
    DEBUG_LOG("Notfiy Report model. File Destination Path : " << sDownloadpath.toStdString()
              << " Download Error: " << sDownloadError.toStdString());
    break;
  case QMLEnums::COMMAND_USERADMIN_KEY_EXPORT_MODEL:
  case QMLEnums::COMMAND_USERADMIN_USER_DB_EXPORT_MODEL:

    DEBUG_LOG("Notfiy Export file download success. File Destination Path : " << sDownloadpath.toStdString()
              << " Download Error: " << sDownloadError.toStdString());
    emit filedownloadfinished(sDownloadpath, sDownloadError, modelnum);
    break;
  case QMLEnums::ERROR_LOG_SEARCH_MODEL:
    DEBUG_LOG("Notfiy Error log search model. File Destination Path : " << sDownloadpath.toStdString()
              << " Download Error: " << sDownloadError.toStdString());
    emit errorlogdownloaded(sDownloadpath, sDownloadError);
    break;
  case QMLEnums::COMMAND_IMAGEQUALITY_TEST_MODEL:
    DEBUG_LOG("Notfiy Image quality model. File Destination Path : " << sDownloadpath.toStdString()
              << " Download Error: " << sDownloadError.toStdString());
    emit imgQltyTestfileDownloadFinish(sDownloadpath, sDownloadError);
    break;
  case QMLEnums::PARAMETER_MODEL:
  case QMLEnums::PARAMETER_XSD_MODEL:
    break;
  case QMLEnums::COMMAND_TIP_CONFIG_MODEL:
    emit filedownloadfinished(sDownloadpath, sDownloadError, modelnum);
    break;
  case QMLEnums::COMMAND_TIP_LIB_CONFIG_MODEL:
    break;
  default:
    break;
  }
}

/*!
* @fn       refreshUiModels();
* @param    None
* @return   None
* @brief    This function is responsible for refreshing models.
*/
void View::refreshUiModels()
{
  if (!m_allUiDataModels.isEmpty())
  {
    DEBUG_LOG("Notify to refresh ui models");
    emit refreshUi();
  }
}

/*!
* @fn      moveFile
* @param   QString dstgetModelData
* @param   QString src
* @return  int
* @brief   Function will call on save file.
*/
int View::moveFile(QString dst , QString src)
{
  return moveFilePath(dst, src);
}

/*!
* @fn       onCommandClick
* @param    QMLEnums::ModelEnum modelname: Model that needs to be refered while sending commands to the Rest APIs.
* @param    QString data : Data that needs to be sent.
* @return   None
* @brief    invokable method from qml that send commands via rest APIs using model references.
*/
void View::onCommandClick(QMLEnums::ModelEnum modelname, QString data)
{
  DEBUG_LOG("Command button click event occured for modelname: " << modelname);

  if (modelname == QMLEnums::USER_ACCESS_RESET_MODEL)
  {
    m_networkloginStatus     = QMLEnums::WSAuthenticationResult::IO_ERROR;
    m_scannerloginStatus     = QMLEnums::WSAuthenticationResult::IO_ERROR;
    m_useradminloginStatus   = QMLEnums::WSAuthenticationResult::IO_ERROR;
    m_rerunloginStatus       = QMLEnums::WSAuthenticationResult::IO_ERROR;
    m_reportadminloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
    m_tipconfigadminloginStatus    = QMLEnums::WSAuthenticationResult::IO_ERROR;
    m_tiplibconfigadminloginStatus    = QMLEnums::WSAuthenticationResult::IO_ERROR;
    m_antivirusserviceloginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
    m_supervisorLoginStatus = QMLEnums::WSAuthenticationResult::IO_ERROR;
  }
  emit commandButtonEvent(modelname, data);
}

/*!
   * @fn       onprocessUpdateModels
   * @param    None
   * @return   None
   * @brief    process models.
   */
void View::onProcessUpdateModels()
{
  emit processUpdateModels();
}

/*!
* @fn      updateChartsCordinates
* @param   QAbstractSeries
* @return  None
* @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
*/
void View::updateCordinates(QAbstractSeries *series,int modelnum)
{
  emit updateChartsCordinates(series,modelnum);
}

/*!
* @fn       getModelData
* @param    QMLEnums::ModelEnum modelname: Index of Model for which data neds to be returned.
* @return   QVariantList: a map data
* @brief    invokable method from qml that send commands via rest APIs using model references.
*/
QVariantList View::getModelData(QMLEnums::ModelEnum modelname)

{
  QVariantList data;
  if (m_allUiDataModels.find(modelname) != m_allUiDataModels.end())
  {
    data = m_allUiDataModels[modelname];
    // m_allUiDataModels.remove(modelname);
    m_allUiDataModels[modelname][0] = QVariant::fromValue(0);
  }
  DEBUG_LOG("Getting modeldata for modelname: " << modelname << " data list count: " << data.count());
  return data;
}

/*!
* @fn       readImageQualityFile
* @param    QString -file
* @return   QString
* @brief    parse image quality file and return final result.
*/
QString View::readImageQualityFile(QString strImageQualityFile)
{
  TRACE_LOG("");
  INFO_LOG("View::readImageQualityFile: " << strImageQualityFile.toStdString());
  QStringList    pieces;
  QString        filename;
  QString        data;

  QRegExp rx("(\\_|\\.)");

  pieces = strImageQualityFile.split( "/" );
  filename = pieces.value( pieces.length() - 1 );
  pieces = filename.split(rx);
  data = pieces.value( pieces.length() - 4 );
  data = data.toUpper();

  if(data == "PASS")
  {
    data = tr("PASS");
  }
  else if(data == "FAIL")
  {
    data == tr("FAIL");
  }
  else if(data == "MARGINAL")
  {
    data == tr("MARGINAL");
  }
  else
    data = "";
  DEBUG_LOG(data.toStdString());
  return data;
}

/*!
* @fn      onGetSelectedGroup
* @param   QString
* @return  None
* @brief   This function is responsible for getting selected group
*/
void View::onGetSelectedGroup(QString selectedGroup)
{
  emit getSelectedGroup(selectedGroup);
}

/*!
  * @fn      onGetSelectedGauge
  * @param   QString
  * @return  None
  * @brief   This function is responsible for getting selected gauge
  */
void View::onGetSelectedGauge(int selectedGauge)
{
  emit getSelectedGauge(selectedGauge);
}

#ifdef WEBENGINE
/*!
* @fn       setAuthTokenToWebview();
* @param    None
* @return   None
* @brief    set auth token
*/
void View::setAuthTokenToWebview()
{
  if ( (m_authToken.length() !=0) &&
       ((getWorkstationNameEnum() == QMLEnums::WSType::OSR_WORKSTATION) ||
        (getWorkstationNameEnum() == QMLEnums::WSType::SEARCH_WORKSTATION) ||
        (getWorkstationNameEnum() == QMLEnums::WSType::SUPERVISOR_WORKSTATION) ||
        (getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)))
  {
    dynamic_cast<WebEngineUrlRequestInterceptor*>(m_requestInterceptor)->setAuthToken(m_authToken);
  }
}
#endif
/*!
* @fn       onauthstatuschanged();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelno
* @return   None
* @brief    called on authentication status changed
*/
void View::onauthstatuschanged(int hr, QString status, int modelno)
{
#ifdef WEBENGINE
  if ((getWorkstationNameEnum() == QMLEnums::WSType::OSR_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::SEARCH_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::SUPERVISOR_WORKSTATION) ||
      (getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION))
  {
    if(modelno == QMLEnums::ModelEnum::USERADMIN_AUTHENTICATION_MODEL)
    {
      dynamic_cast<WebEngineUrlRequestInterceptor*>(m_requestInterceptor)->setAuthToken(status);
      m_authToken = status;
    }
  }
#endif

  if((modelno == QMLEnums::NETWORK_AUTHENTICATION_MODEL) ||
     (modelno == QMLEnums::NETWORK_CHANGE_PASSWORD_MODEL ))
  {
    if ((getCurrentView() == QMLEnums::RELIEVE_USER_LOGIN_PANEL_SCREEN))
    {
      if (hr == Errors::SUCCESS)
      {
        m_networkloginStatus = (QMLEnums::WSAuthenticationResult) hr;
      }
    }
    else
    {
      m_networkloginStatus = (QMLEnums::WSAuthenticationResult) hr;
    }
  }
  else if(modelno == QMLEnums::AUTHENTICATIN_MODEL)
  {
    m_scannerloginStatus = (QMLEnums::WSAuthenticationResult) hr;
  }
  else if(modelno == QMLEnums::USERADMIN_AUTHENTICATION_MODEL)
  {
    m_useradminloginStatus = (QMLEnums::WSAuthenticationResult) hr;
  }
  else if(modelno == QMLEnums::REPORT_ADMIN_AUTHENTICATION_MODEL)
  {
    m_reportadminloginStatus = (QMLEnums::WSAuthenticationResult) hr;
  }
  else if(modelno == QMLEnums::RERUN_EMULATOR_AUTHENTICATION_MODEL)
  {
    m_rerunloginStatus = (QMLEnums::WSAuthenticationResult) hr;
  }
  else if(modelno == QMLEnums::TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL)
  {
    m_tipconfigadminloginStatus = (QMLEnums::WSAuthenticationResult) hr;
  }
  else if(modelno == QMLEnums::TIP_LIB_CONFIG_ADMIN_AUTHENTICATION_MODEL)
  {
    m_tiplibconfigadminloginStatus = (QMLEnums::WSAuthenticationResult) hr;
  }
  else if(modelno == QMLEnums::ANTIVIRUS_AUTHENTICATION_MODEL)
  {
    m_antivirusserviceloginStatus = (QMLEnums::WSAuthenticationResult) hr;
  }
  else if(modelno == QMLEnums::SUPERVISOR_AUTHENTICATION_MODEL)
  {
    m_supervisorLoginStatus = (QMLEnums::WSAuthenticationResult) hr;
  }

  emit authstatuschanged(hr, status , modelno);
}

#if defined WORKSTATION || defined RERUN
/*!
* @fn       onsslerrorreceived();
* @param    int - errorcode
* @param    QString - status
* @param    int - modelno
* @return   None
* @brief    called on onsslerrorreceived
*/
void View::onsslerrorreceived(int hr, QString status, int modelno)
{
  DEBUG_LOG("SSL eror . hr: "
            << hr << " status: "
            << status.toStdString()
            << " modelno: "
            << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(modelno));
  emit sslerrorreceived(hr, status , modelno);
}
#endif
/*!
* @fn       exitApp();
* @param    None
* @return   None
* @brief    Function will call on close application
*/
void View::exitApp()
{
  DEBUG_LOG("Notifying exit of application");
  emit exit();
}

/*!
* @fn      createAndGetDir
* @param   int modelindex
* @param   QString filename
* @return  QString
* @brief   This function is responsible for create and get dir.
*/
QString View::createAndGetDir(int modelindex, QString filename)
{
  return createAndGetDirPath(modelindex, filename);
}


/*!
* @fn      onopenfile
* @param   QString- rawdata filename
* @return
* @brief   This function is responsible for opening file
*/
QByteArray View::onopenfile(QString filename )
{
  QFile ifile(filename);
  QByteArray data;

  if(ifile.open(QFile::ReadOnly))
  {
    data = ifile.readAll();
    return data;
  }
  else
  {
    data = "No File Select For Display Data";
    ERROR_LOG("No File Select For Display Data");
    return data;
  }
}



/*!
* @fn      saveFile
* @param   QString dst
* @param   QString src
* @return  int
* @brief   Function will call on save file.
*/
int View::saveFile(QString dst , QString src)
{
  INFO_LOG("View::saveFile: <" << src.toStdString() << "> <" << dst.toStdString() << ">");
  qint64 size = checkFreeSpace();
  if(size >0)
  {
    QFile fileSizeCheck(src);
    if(size >= fileSizeCheck.size())
      return Utility::saveFile(dst, src);
    else
      return Errors::USB_DRIVE_FULL;
  }
  else
  {
    return Errors::USB_DRIVE_FULL;
  }
}

/*!
     * @fn       getModelString
     * @param    QMLEnums::ModelEnum
     * @return   QString
     * @brief    returns modelString
     */
QString View::getModelString(QMLEnums::ModelEnum modelno)
{
  QString modelString = QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(modelno);
  return modelString;
}

/*!
* @fn       getWorkstationNameEnum
* @param    None
* @return   int
* @brief    get workstation name from config.
*/
int View::getWorkstationNameEnum()
{
  return WorkstationConfig::getInstance()->getWorkstationNameEnum();
}

#if defined WORKSTATION || defined RERUN
/*!
* @fn       getUserAdminServer
* @param    None
* @return   QString
* @brief    get UserAdminServer IP from config.
*/
QString View::getUserAdminServer()
{
  QString userAdminServer = WorkstationConfig::getInstance()->getuseradminseveripaddress()
      + ":" + WorkstationConfig::getInstance()->getuseradminserverportno();
  DEBUG_LOG("user admin server: "<< userAdminServer.toStdString());
  return userAdminServer;
}

#endif

/*!
* @fn      readErrorLogData
* @param   QString
* @return  QVariantList
* @brief   Function will call on save file.
*/
QVariantList View::readErrorLogData(QString strPath)
{
  QVariantList stringlistarray;
  QString data, timezone;
  QFile infile(strPath);
  if(!infile.open(QFile::ReadOnly))
  {
    ERROR_LOG("Could not open file for reading so data is NULL. file: "<< strPath.toStdString());
    return stringlistarray;
  }
  QStringList list;
  std::multimap<uint, QStringList> maplist;
  DEBUG_LOG("Reading error logdata file: "<< strPath.toStdString());
  while(!infile.atEnd())
  {
    data.clear();
    data = infile.readLine();
    list.clear();
    list = data.split('^');
    if(list.count() != 5)
    {
      ERROR_LOG("View::readErrorLogData: Incorrect field count:"
                << list.count() << " " << data.toStdString());
      continue;
    }
    if(list[0].isEmpty())
    {
      ERROR_LOG("Data does not have proper format" << list[0].toStdString());
    }
    else
    {
      QString dateSimplified = list[0].simplified();
      QStringList datelist = dateSimplified.split(" ");
      timezone = datelist[4];
      QString dateformat = QString("ddd MMM d hh:mm:ss '") + timezone + QString("' yyyy");
      QDateTime datetime =  QDateTime::fromString(dateSimplified, dateformat);
      list.insert(0, QString::number(datetime.toTime_t()));
      maplist.insert(std::pair<uint, QStringList>(datetime.toTime_t(), list));
      if(list.count() != 6) {
        ERROR_LOG("View::readErrorLogData: Incorrect field count:"
                  << list.count() << " " << data.toStdString());
      }
    }
  }
  DEBUG_LOG("Finished reading error logdata file.");
  for (std::map <uint, QStringList>::reverse_iterator it = maplist.rbegin(); it != maplist.rend(); ++it)
  {
    stringlistarray.append(it->second);
  }
  return  stringlistarray;
}

/*!
* @fn      getulmDataList
* @param   None
* @return  QVariant
* @brief   Function gets ulm DataList.
*/
QVariant View::getulmDataList()
{
  DEBUG_LOG("Getting ulm datalist where datalist count is: "
            << m_ulmDataList.count());
  return QVariant::fromValue(m_ulmDataList);
}

/*!
* @fn      shutDownSystem
* @param   None
* @return  None
* @brief   This function is responsible for shutting down system
*/
void View::shutDownSystem()
{
  // TODO(Agiliad): Commented below code for ease of testing.
  // Will be enabled once code is developement is completed.
  // system("shutdown -P now");
}

/*!
* @fn      restartSystem
* @param   None
* @return  None
* @brief   This function is responsible for restarting system
*/
void View::restartSystem()
{
  // TODO(Agiliad): Commented below code for ease of testing.
  // Will be enabled once code is developement is completed.
  // system("reboot");
}

/*!
* @fn       setCurrModalFrame
* @param    int
* @return   None
* @brief    called internally when current Modal Frame changes.
*/
void View::setCurrModalFrame(int sMF)
{
  DEBUG_LOG("Current modal frame set to: "<< sMF);
  m_sCurentModalFrame = sMF;
}

/*!
* @fn       getCurrModalFrame
* @param    None
* @return   int
* @brief    gets Current Modal Frame
*/
int View::getCurrModalFrame() const
{
  DEBUG_LOG("Current Modal frame is: "<< m_sCurentModalFrame);
  return m_sCurentModalFrame;
}

/*!
* @fn      setModalFrame
* @param   int comp
* @param   QString errorMsgText=NULL
* @param   int nextScreen
* @return  int
* @brief   This function sets and then show component of ModalFrame for a error
*/
int View::setModalFrame(int modalComp, QString errorMsgText, int nextScreen)
{
  if (modalComp == QMLEnums::RELIEVE_USER_LOGIN_PANEL) {
    m_containerModalFrame->hide();
    m_qmlContainerWidget->setFocus(Qt::ActiveWindowFocusReason);
    emit setReliveUserScreen(modalComp, errorMsgText, nextScreen);
    return static_cast<int>(Errors::S_OK);
  }
  createModalFrame();
  RETURN_IF_FAILED((m_containerModalFrame == NULL)?Errors::E_FAIL:Errors::S_OK);

  m_containerModalFrame->setFocus(Qt::ActiveWindowFocusReason);
  m_containerModalFrame->show();
  DEBUG_LOG("Setting error modalframe where modal comp is: "
            << modalComp << " and errormsg is: "
            << errorMsgText.toStdString());
  emit setItemCompInModalFrame(modalComp, errorMsgText, nextScreen);
  return static_cast<int>(Errors::S_OK);
}

/*!
* @fn      hideModalFrame
* @param   None
* @return  None
* @brief   This function is responsible for hiding and disabling ModalFrame
*/
void View::hideModalFrame()
{
  DEBUG_LOG("Hiding modal frame.");
  if(m_containerModalFrame)
  {
    m_containerModalFrame->hide();
    m_qmlContainerWidget->setFocus(Qt::ActiveWindowFocusReason);
  }
}
#ifdef WEBENGINE
/*!
* @fn       webEngineViewUserAdminLoaded
* @param    none
* @return   none
* @brief    Called from QML when web view loads.
*/
void View::webEngineViewUserAdminLoaded()
{
  DEBUG_LOG("Web view loaded for user admin.");
  QQuickItem *rootObject = m_qmlContainer->rootObject();
  if(rootObject)
  {
    QQuickWebEngineProfile::defaultProfile()->setRequestInterceptor(m_requestInterceptor);
    /*
    QQuickWebEngineView *qobj = rootObject->findChild<QQuickWebEngineView*>("qmlObjectWebEngineView");
    if(qobj)
    {
      qobj->profile()->setRequestInterceptor(m_requestInterceptor);
    }
    */
  }
}
#endif

/*!
* @fn       onBLBDCCommand
* @param    QMLEnums::ArchiveFilterSrchLoc srchLoc
* @param    QMLEnums::BagsListAndBagDataCommands command
* @param    QString data : Data that needs to be sent.
* @return   None
* @brief    invokable method from qml that send BagsListAndBagData commands to viewManager.
*/
void View::onBLBDCCommand(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                          QMLEnums::BagsListAndBagDataCommands command, QVariant data)
{
  if(command <= QMLEnums::BagsListAndBagDataCommands::BLBDC_START ||
     command >= QMLEnums::BagsListAndBagDataCommands::BLBDC_END)
  {
    ERROR_LOG("Incorrect command: " << command);
    return;
  }

  DEBUG_LOG("Notification to send baglist or bagdata command.");
  emit bagListCommand(srchLoc, command, data);
}

/*!
* @fn       getLoginStatus
* @param    entity
* @return   int
* @brief    gets login status for entity
*/
int View::getLoginStatus(int entity)
{
  int status = -1;
  if(entity == static_cast<int>(QMLEnums::Servers::NW_SERVER))
  {
    status = m_networkloginStatus;
  }
  else  if(entity == static_cast<int>(QMLEnums::Servers::SCANNER_SERVER))
  {
    status = m_scannerloginStatus;
  }
  else  if(entity == static_cast<int>(QMLEnums::Servers::USER_ADMIN_SERVER))
  {
    status = m_useradminloginStatus;
  }
  else  if(entity == static_cast<int>(QMLEnums::Servers::RERUN_SERVER))
  {
    status = m_rerunloginStatus;
  }
  else  if(entity == static_cast<int>(QMLEnums::Servers::REPORT_ADMIN_SERVER))
  {
    status = m_reportadminloginStatus;
  }
  else  if(entity == static_cast<int>(QMLEnums::Servers::TIP_CONFIG_ADMIN_SERVER))
  {
    status = m_tipconfigadminloginStatus;
  }
  else  if(entity == static_cast<int>(QMLEnums::Servers::TIP_LIB_CONFIG_ADMIN_SERVER))
  {
    status = m_tiplibconfigadminloginStatus;
  }
  else if(entity == static_cast<int>(QMLEnums::Servers::ANTIVIRUS_SERVER))
  {
    status = m_antivirusserviceloginStatus;
  }
  else if(entity == static_cast<int>(QMLEnums::Servers::SUPERVISOR_SERVER))
  {
    status = m_supervisorLoginStatus;
  }
  return status;
}

#ifdef RERUN
/*!
   * @fn       rerunEmulatorCommand
   * @param    QMLEnums::RERUN_COMMANDS
   * @param    RerunTaskConfig* test
   * @return   none
   * @brief    send rerun command from UI
   */
void View::rerunEmulatorCommand(QMLEnums::RERUN_COMMANDS cmd, RerunTaskConfig* test)
{
  emit rerunCommand(cmd, *test);
}

/*!
* @fn       getPlaylistPath
* @param    None
* @return   QString
* @brief    Return Training Playlist Path
*/
QString View::getPlaylistPath()
{
  return RerunConfig::getPlaylistPath();
}

/*!
* @fn       getPlaylistDecisionPath
* @param    None
* @return   QString
* @brief    Return Training PlaylistDecision Path
*/
QString View::getPlaylistDecisionPath()
{
  if(!dirExists(RerunConfig::getPlaylistDecisionPath()))
  {
    QDir dir(RerunConfig::getPlaylistDecisionPath());
    dir.mkdir(RerunConfig::getPlaylistDecisionPath());
  }
  else
  {
    DEBUG_LOG("Dir exist");
  }
  return RerunConfig::getPlaylistDecisionPath();
}

/*!
* @fn       getPlaylistFileNames
* @param    None
* @return   QString
* @brief    Return Training Playlist File Names
*/
QStringList View::getPlaylistFileNames()
{
  QStringList fileList;
  QDir dir(RerunConfig::getPlaylistPath());
  if(!dir.exists())
  {
    dir.mkdir(RerunConfig::getPlaylistPath());
  }

  QStringList fileInfo = dir.entryList(QDir::Files);
  for(int i = 0 ; i < fileInfo.count(); i++)
  {
    if(!fileInfo.at(i).contains("."))
    {
      fileList.append(fileInfo.at(i));
    }
  }
  return fileList;
}
/*!
* @fn       setUserID
* @param    None
* @return   QString
* @brief    update User ID selected in training report screen
*/

QString View::setUserID(QString m_userID)
{
  m_userID=m_userID;
  return m_userID;
}
/*!
* @fn       getReportResultPath
* @param    None
* @return   QString
* @brief    Return Training Report Path
*/

QString View::getReportResultPath(QString m_userID)
{
  return m_rerunConfigObj->getReportResultPath(m_userID);
}

/*!
* @fn       getCsvReportFileNames
* @param    None
* @return   QStringList
* @brief    Return Training Report File Name for specified userID
*/

QStringList View::getCsvReportFileNames(QString reportTypeSelected, QString m_userID)
{
  QStringList fileList;
  QDir dir(m_rerunConfigObj->getReportResultPath(m_userID));
  //  if(!dir.exists())
  //  { QString error="Source Directory dosen't exist";
  //    //fileList<<error;
  //    //return fileList.at(0);
  //   // dir.mkdir(m_rerunConfigObj->getReportResultPath(m_userID));
  //  }
  if(m_userID!="")
  { QStringList fileInfo = dir.entryList(QDir::Files);

    for(int i = 0 ; i < fileInfo.count(); i++)
    {
      if(fileInfo.at(i).contains(".csv")&&(fileInfo.at(i).contains(reportTypeSelected)))
      {
        fileList.append(fileInfo.at(i));
      }
    }
    return fileList;
  }
  else {
    QStringList fileInfo = dir.entryList(QDir::Dirs);
    for(int i = 0 ; i < fileInfo.count(); i++)
    {
      if(!fileInfo.at(i).contains("."))
      {
        fileList.append(fileInfo.at(i));
      }
    }
    return fileList;
  }

}

#endif
/*!
* @fn       customFiledialog
* @param    int mode,int type,int acceptmode, QString title, QString path, QString nameFilter
* @return   QStringList
* @brief    send rerun config from UI
*/
QString View::customFiledialog(int mode, int type, int acceptmode, QString title, QString path, QString nameFilter)
{
  QStringList fileList;
  QString file;

  if(m_fileDlg)
  {
    m_fileDlg->close();
    SAFE_DELETE(m_fileDlg);
  }
  // Mode = 1 is for custom file dialog
  if (mode == 1)
  {
    m_fileDlg = CustomFileDialog::getCustomFileDialogInstance(m_mainWindow,
                                                              title,
                                                              path,
                                                              nameFilter);
    reinterpret_cast<CustomFileDialog*>(m_fileDlg)->setAcceptMode((QFileDialog::AcceptMode) acceptmode);
  }
  // Mode = 0 is for standard file dialog but it crashes some time which is handled in custom
  else if (mode == 0)
  {
    m_fileDlg = new QFileDialog(m_mainWindow,
                                title,
                                path,
                                nameFilter);
    m_fileDlg->setAcceptMode((QFileDialog::AcceptMode) acceptmode);
  }

  if(getWorkstationNameEnum() == QMLEnums::WSType::RERUN_EMULATOR)
  {
    m_fileDlg->setOption(QFileDialog::ReadOnly, false);
  }
  else
  {
    m_fileDlg->setOption(QFileDialog::ReadOnly, true);
  }
  m_fileDlg->setViewMode(QFileDialog::Detail);
  m_fileDlg->setConfirmOverwrite(false);
  m_fileDlg->setWindowModality(Qt::WindowModal);
  m_fileDlg->setFileMode((QFileDialog::FileMode)type);


  if(m_fileDlg->exec()) {
    fileList = m_fileDlg->selectedFiles();
    if(fileList.size())
    {
      file = fileList.at(0);
    }
  }
  return file;
}

/*!
* @fn       deleteFiledialogInstance
* @param    None
* @return   None
* @brief    delete file dialog instance
*/
void View::deleteFiledialogInstance()
{
  if(m_fileDlg)
  {
    if(!(m_fileDlg->close()))
    {
      m_fileDlg->close();
    }
    delete m_fileDlg;
    m_fileDlg = NULL;
  }
}

/*!
* @fn       hideFiledialogInstance
* @param    None
* @return   None
* @brief    hide file dialog instance
*/
void View::hideFiledialogInstance()
{
  if(m_fileDlg)
  {
    m_fileDlg->hide();
  }
}

/*!
* @fn      convertValue
* @param   QMLEnums::conversionEnum conversion
* @param   double inputValue
* @return  double
* @brief   Function will convert value according to unit.
*/
double View::convertValue(QMLEnums::conversionEnum conversion, double inputValue)
{
  double changed = 0.0;
  if(conversion == QMLEnums::conversionEnum::CENTIMETER_TO_INCH)
  {
    changed = CMTOINCH * inputValue;
  }
  else if(conversion == QMLEnums::conversionEnum::INCH_TO_CENTIMETER)
  {
    changed = INCHTOCM * inputValue;
  }
  return round(changed);
}

/*!
* @fn       viewSelectedBags
* @param    selectedBagList
* @param    QModelIndex outputFilePathIndex
* @param    QString opID
* @return   None
* @brief    called when ViewBags is pressed.
*/
void View::viewSelectedBags(QVariantList selectedBagsList, QModelIndex outputFilePathIndex, QString opID)
{
  emit notifyViewSelectedBags(selectedBagsList, outputFilePathIndex, opID);
}

/*!
* @fn       readFromFile
* @param    QString filePathName
* @return   QStringList
* @brief    return the contents of a given file line-by-line
*/
QStringList View::readFromFile(QString filePathName)
{
  QStringList list;
  QFile file(filePathName);
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      list << line;
    }
    file.close();
  }
  else
  {
    ERROR_LOG(filePathName.toStdString() + " failed to open.");
  }
  return list;
}

/*!
* @fn       writeInFile
* @param    QString filePathName
* @param    QString
* @return   bool
* @brief    writes the contents in a given file line-by-line
*/
bool View::writeInFile(QString filePathName, QString content)
{
  QFile file(filePathName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    ERROR_LOG(filePathName.toStdString() + " failed to open.");
    return false;
  }
  file.write(content.toStdString().c_str());
  file.close();
  return true;
}
/*!
* @fn       prepareRerunBagList
* @param    QString filename
* @param    QString folderPathName
* @param    QString nameFilter
* @return   QStringList
* @brief    returns rerun Bag List
*/
QStringList View::prepareRerunBagList(QString filename, QString folderPathName, QString nameFilter)
{
  TRACE_LOG("");
  QStringList fileList;
  if (fileExists(filename))
  {
    fileList = readFromFile(filename);
  }
  else
  {
    fileList = getFilterFilesFromInputFolder(folderPathName, nameFilter);
  }
  return fileList;
}
/*!
* @fn       getFilterFilesFromInputFolder
* @param    QString folderPathName
* @param    QString nameFilter
* @return   QStringList
* @brief    returns list of files for given name filter
*/
QStringList View::getFilterFilesFromInputFolder(QString folderPathName, QString nameFilter)
{
  QDir dir(folderPathName, nameFilter);
  QStringList fileList;
  foreach (QString file, dir.entryList()) {
    file.prepend(folderPathName+"/");
    fileList << file;
  }
  return fileList;
}

/*!
* @fn       getFilterFilesRecursivelyFromInputFolder
* @param    QString folderPathName
* @param    QString nameFilter
* @return   QStringList
* @brief    returns list of files recursively for given name filter
*/
QStringList View::getFilterFilesRecursivelyFromInputFolder(QString folderPathName, QString nameFilter)
{
  QDir dir(folderPathName);
  QStringList fileList;

  QStringList volFormatList = nameFilter.split(",");
  dir.setNameFilters(volFormatList);

  QFileInfoList list = dir.entryInfoList(dir.nameFilters(),
                                         QDir::NoDot | QDir::NoDotDot | QDir::AllDirs | QDir::Files);
  foreach(QFileInfo file, list)
  {
    if(file.isDir())
    {
      fileList.append(getFilterFilesRecursivelyFromInputFolder(file.filePath(), nameFilter));
    }
    else
    {
      if (!fileList.contains(file.filePath()))
      {
        fileList << file.filePath();
      }
    }
  }
  return fileList;
}
/*!
* @fn       getFilterFilesRecursivelyFromInputFolder
* @param    QString folderPathName
* @param    QString nameFilter
* @return   QStringList
* @brief    returns list of files nonrecursively for given name filter traversing in subdirectories avoided
*/
QStringList View::getFilterFilesNonRecursivelyFromInputFolder(QString folderPathName, QString nameFilter)
{
  QDir dir(folderPathName);
  QStringList fileList;

  QStringList volFormatList = nameFilter.split(",");
  dir.setNameFilters(volFormatList);

  QFileInfoList list = dir.entryInfoList(dir.nameFilters(),
                                         QDir::NoDot | QDir::NoDotDot | QDir::Files);
  foreach(QFileInfo file, list)
  {
    if(file.isFile())
    {
      if (!fileList.contains(file.filePath()))
      {
        fileList << file.filePath();
      }
    }
  }
  return fileList;
}

/*!
* @fn       validateRerunEmulatorList
* @param    QString baglist
* @return   QString - error msg
* @brief    validate input bag data list
*/
QString View::validateRerunEmulatorBagList(QStringList baglist)
{
  QStringList fileextlist;
  QString msg = "";
  for(int i = 0; i < baglist.size(); i++)
  {
    fileextlist.clear();
    QString filename = baglist.at(i);
    if(filename.contains(VOL_FILE_EXT))
    {
      filename.remove(QString(VOL_FILE_EXT));
      fileextlist << VOL_FILE_EXT << DE_FILE_EXT;
      if((fileExists(filename + PROJ0_FILE_EXT)) || (fileExists(filename + PROJ90_FILE_EXT)))
      {
        fileextlist << PROJ0_FILE_EXT << PROJ90_FILE_EXT;
      }
      else if((fileExists(filename + DICOS_PROJ00_FILE_EXT)) || (fileExists(filename + DICOS_PROJ90_FILE_EXT)))
      {
        fileextlist << DICOS_PROJ00_FILE_EXT << DICOS_PROJ90_FILE_EXT;
      }
    }
    else if(filename.contains(BAGFILE_JPEG_VOLUME_FORMAT))
    {
      filename.remove(QString(BAGFILE_JPEG_VOLUME_FORMAT));
      fileextlist << BAGFILE_JPEG_VOLUME_FORMAT << DE_FILE_EXT;
    }
    filename.remove("\n");
    for(int j = 0; j < fileextlist.size(); j++)
    {
      QString temp = filename + fileextlist.at(j);
      bool ret = fileExists(temp);
      if(!ret)
      {
        msg = QString("File: ") + temp + " does not exist";
        return msg;
      }
    }
  }
  return msg;
}
/*!
* @fn       folderExists();
* @param    QString
* @return   bool
* @brief    Function will check folder exists or not
*/
bool View::folderExists(QString filepathName)
{
  std::string str = filepathName.toStdString();
  const char* p = str.c_str();
  struct stat sb;
  if (stat(p, &sb) == 0 && S_ISDIR(sb.st_mode))
  {
    return true;
  }
  else
  {
    return false;
  }
}
/*!
* @fn       fileExists();
* @param    QString
* @return   bool
* @brief    Function will check file exists or not
*/
bool View::fileExists(QString filenamepath)
{
  QFileInfo check_file(filenamepath);
  // check if file exists and if yes: Is it really a file and no directory?
  if (check_file.exists() && check_file.isFile()) {
    return true;
  } else {
    return false;
  }
}
/*!
* @fn       checkFileFolderPermission();
* @param    QString
* @param    QString
* @return   int
* @brief    Function will check file exists or not
*/
int View::checkFileFolderPermission(QString filenamepath, QString errStr)
{
  int hr = Utility::checkFileFolderPermission(filenamepath, errStr);
  return hr;
}

/*!
 * @fn       removeTemporaryDirectory;
 * @param    none
 * @return   None
 * @brief    function to remove local storage created during rerun detection.
 */
void View::removeTemporaryDirectory()
{
  QDir tmpDir(QDir::tempPath());
  QString tmpPath = tmpDir.absoluteFilePath("rerunCompressed");
  QDir rerunDir(tmpPath);
  if(rerunDir.exists())
  {
    bool ret = rerunDir.removeRecursively();
    if(ret)
    {
      DEBUG_LOG("File deleted successfully");
    }
    else
    {
      ERROR_LOG("File could not be deleted");
    }
  }
}


/*!
* @fn      createOperatorThreatPopupScreen
* @param   None
* @return  None
* @brief   This function is responsible for creating Operator Threat Popup screen
*/
void View::createOperatorThreatPopupScreen()
{
  TRACE_LOG("");

  // For Scanner Fault Status
  m_OperatorThreatScrnShown = false;
  m_qmlOperatorThreatOptions->rootContext()->setContextProperty("viewobject", this);
  m_qmlOperatorThreatOptions->setSource(QUrl(QStringLiteral(
                                               "qrc:/common/screenareas/OperatorThreatPopupOptions.qml")));
  m_qmlOperatorThreatOptions->setResizeMode(QQuickView::SizeViewToRootObject);
  m_qmlOperatorThreatOptions->rootContext()->setContextProperty("WorkstationVolumeRenderer", m_volDispMgr);
  QQuickItem *rootWindowItem = m_qmlContainer->rootObject();
  if(rootWindowItem)
  {
    m_threatEvalPanelmodel = rootWindowItem->findChild<QObject*>("threatevalpanel");
    if (m_threatEvalPanelmodel)
    {
      m_qmlOperatorThreatOptions->rootContext()->setContextProperty(
            "threatevalpanelmodel", m_threatEvalPanelmodel);
    }
  }

  m_containerOperatorThreats = QWidget::createWindowContainer(m_qmlOperatorThreatOptions);
  m_containerOperatorThreats->setParent(m_mainWindow);
  m_qmlOperatorThreatOptions->setResizeMode(QQuickView::SizeRootObjectToView);
  m_containerOperatorThreats->resize(DEFAULT_OPERATORLIST_WINDOW_WIDTH, DEFAULT_OPERATORLIST_WINDOW_HEIGHT);
  int width = m_containerOperatorThreats->geometry().width();
  int height = m_containerOperatorThreats->geometry().height();
  QPoint point = m_mainWindow->rect().center() - QPoint(width/4, height/4);
  m_containerOperatorThreats->setGeometry(point.x(),
                                          point.y(),
                                          DEFAULT_OPERATORLIST_WINDOW_WIDTH,
                                          DEFAULT_OPERATORLIST_WINDOW_HEIGHT);
  m_containerOperatorThreats->lower();
}

/*!
  * @fn      createNavigationMenuPopupScreen
  * @param   None
  * @return  None
  * @brief   This function is responsible for creating Navigation Menu Screen.
  */
void View::createNavigationMenuPopupScreen()
{
  TRACE_LOG("");

  m_NavigationMenuScrnShown = false;
  m_qmlNavigationMenu->rootContext()->setContextProperty("viewobject", this);

  QQuickItem *rootWindowItem = m_qmlContainer->rootObject();
  if(rootWindowItem)
  {
    m_NavigationPanelModel = rootWindowItem->findChild<QObject*>("navigationmodel");
    if (m_NavigationPanelModel)
    {
      m_qmlNavigationMenu->rootContext()->setContextProperty(
            "navigationModelData", m_NavigationPanelModel);
    }
    m_networkConnectionModel = rootWindowItem->findChild<QObject*>("networkConnectionModel");
    if (m_networkConnectionModel)
    {
      m_qmlNavigationMenu->rootContext()->setContextProperty(
            "networkConnectionModel", m_networkConnectionModel);
    }
  }

  m_qmlNavigationMenu->setSource(QUrl(QStringLiteral(
                                        "qrc:/common/screenareas/NavigationMenuPanel.qml")));


  m_containerNavigationMenuScrn = QWidget::createWindowContainer(m_qmlNavigationMenu);
  m_containerNavigationMenuScrn->setParent(m_mainWindow);
  m_qmlNavigationMenu->setResizeMode(QQuickView::SizeRootObjectToView);
  m_containerNavigationMenuScrn->resize(static_cast<int>(getRootWindowWidth() * 0.317)
                                        , static_cast<int>(getRootWindowHeight() * 0.35));
  int width = m_containerNavigationMenuScrn->geometry().width();
  int height = m_containerNavigationMenuScrn->geometry().height();
  QPoint point = QPoint(115, 35);
  m_containerNavigationMenuScrn->setGeometry(point.x(),
                                             point.y(),
                                             width,
                                             height);
  m_containerNavigationMenuScrn->lower();

}

/*!
* @fn      showHideOperatorThreatOptions
* @param   bool flag
* @return  None
* @brief   This function is responsible for showing/ hiding operator threat options
*/
void View::showHideOperatorThreatOptions(bool flag)
{
  if ((flag == false) && m_threatEvalPanelmodel)
  {
    QVariant returnedValue;
    QVariant msg = QMLEnums::TEPC_PICKBOX_ADD_ACCEPT;
    QMetaObject::invokeMethod(m_threatEvalPanelmodel, "drawBoxButtonClicked",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, msg));
  }
  if(flag == m_OperatorThreatScrnShown)
  {
    return;
  }

  if (false == m_OperatorThreatScrnShown)
  {
    DEBUG_LOG("Showing operator threat popup screen");

    m_containerOperatorThreats->raise();
    m_containerOperatorThreats->setFocus();
    m_OperatorThreatScrnShown = true;
  }
  else if (true == m_OperatorThreatScrnShown)
  {
    DEBUG_LOG("Hiding operator threat popup screen");
    m_containerOperatorThreats->lower();
    m_OperatorThreatScrnShown = false;
    m_qmlContainerWidget->setFocus(Qt::ActiveWindowFocusReason);
  }
}

/*!
  * @fn      showHideNavigationMenu
  * @param   bool flag
  * @return  None
  * @brief   This function is responsible for showing/ hiding Navigation Menu Screen
  */
void View::showHideNavigationMenu(bool flag)
{
  if(flag != m_NavigationMenuScrnShown)
  {
    if (false == m_NavigationMenuScrnShown)
    {
      DEBUG_LOG("Showing Navigation Menu popup screen");
      resizeNavigationMenu();
      m_containerNavigationMenuScrn->adjustSize();
      m_containerNavigationMenuScrn->raise();
      m_containerNavigationMenuScrn->setFocus();
      m_NavigationMenuScrnShown = true;
    }

    else if (true == m_NavigationMenuScrnShown)
    {
      DEBUG_LOG("Hiding Navigation Menu popup screen");
      m_containerNavigationMenuScrn->lower();
      m_NavigationMenuScrnShown = false;
      m_qmlContainerWidget->setFocus(Qt::ActiveWindowFocusReason);
    }
  }
}

/*!
  * @fn      resizeNavigationMenu
  * @param   None
  * @return  None
  * @brief   This function will call to resize the navigation menu panel when its width changed and
  *          also it will call from showHideNavigationMenu function
  */
void View::resizeNavigationMenu()
{
  if(QQuickItem *navigationWindowItem = m_qmlNavigationMenu->rootObject())
  {
    QObject *navigationMenuObject = navigationWindowItem->findChild<QObject*>("navigationCol");
    double height = navigationMenuObject->property("height").toDouble() + 20;
    double width =  navigationMenuObject->property("width").toDouble() + 30;
    INFO_LOG("Inside resize navigation menu function : height - " << height << " width - " << width);
    m_containerNavigationMenuScrn->resize(static_cast<int>(width)
                                          , static_cast<int>(height));
  }
}

/*!
  * @fn      showHideBalloonPopup
  * @param   bool flag
  * @return  None
  * @brief   This function is responsible for showing/ hiding Balloon popup.
  */
void View::showHideBalloonPopup(bool flag)
{
  if(flag != m_balloonPopupShown)
  {
    if (false == m_balloonPopupShown)
    {
      DEBUG_LOG("Showing Balloon popup screen");
      //double height = m_qmlBalloonPopup->height();
      //double width = m_qmlBalloonPopup->width();
      QQuickItem *balloonPopupWindowItem = m_qmlBalloonPopup->rootObject();
      if(balloonPopupWindowItem)
      {
        QObject *balloonPopupObject = balloonPopupWindowItem->findChild<QObject*>("ballonPopupFrame");
        double height = balloonPopupObject->property("height").toDouble() + 8;
        double width = balloonPopupObject->property("width").toDouble() + 8;

        QObject *balloonPopupOkBtnObject = balloonPopupWindowItem->findChild<QObject*>("btnOk");
        if(balloonPopupOkBtnObject)
        {
          if(!balloonPopupOkBtnObject->property("visible").toBool())
          {
            height = getRootWindowHeight() * 0.112;
          }
          else
          {
            height = getRootWindowHeight() * 0.217;
          }
        }
        m_containerBalloonPopup->resize(static_cast<int>(width)
                                        , static_cast<int>(height));
      }
      m_containerBalloonPopup->adjustSize();
      m_containerBalloonPopup->raise();
      m_containerBalloonPopup->setFocus();
      m_balloonPopupShown = true;
    }

    else if (true == m_balloonPopupShown)
    {
      DEBUG_LOG("Hiding Navigation Menu popup screen");
      m_containerBalloonPopup->lower();
      m_balloonPopupShown = false;
      m_qmlContainerWidget->setFocus(Qt::ActiveWindowFocusReason);
    }
  }
}

/*!
* @fn      printFile
* @param   QString filename
* @return  None
* @brief   Print file to printer
*/
void View::printFile(QString filename)
{
  filename.replace("file://", "");
  QPrinter printer;
  SAFE_DELETE(m_printdlg);
  m_printdlg = new QPrintDialog(&printer, 0);
  if(m_printdlg->exec() == QDialog::Accepted) {
    QString cmd = QString("lpr ") + filename;
    executeCommand(cmd);
  }
}

/*!
* @fn      mountUsb
* @param   None
* @return  QString
* @brief   Function will mount usb.
*/
QString View::mountUsb()
{
  m_usb.setLoggedIn(true);
  m_usb.retriveStorageMountInfo();
  QString mountpath = QString::fromStdString(m_usb.getUsbfilePath());
  return mountpath;
}

/*!
* @fn      unMountUsb
* @param   None
* @return  None
* @brief   Function will unmount usb.
*/
void View::unMountUsb()
{
  m_usb.unMountUsb();
}

/*!
* @fn       isEqualVariantList
* @param    const QVariantList prevList
* @param    const QVariantList currList2
* @return   bool
* @brief    declaration for private assignment operator.
*/
bool View::isEqualVariantList(const QVariantList prevList, const QVariantList currList)
{
  for (int var = 0; var < currList.size(); ++var)
  {
    if (currList[var].canConvert<QVariantMap>() && prevList[var+1].canConvert<QVariantMap>())
    {
      QMap<QString, QVariant> currMap = currList[var].value<QMap<QString, QVariant>>();
      QMap<QString, QVariant> prevMap = prevList[var+1].value<QMap<QString, QVariant>>();
      auto iterator = currMap.constBegin();
      while (iterator != currMap.constEnd())
      {
        if (iterator.key() != "lastUpdated") {
          if (prevMap.contains(iterator.key())) {
            if (prevMap[iterator.key()] != iterator.value()) {
              return false;
            }
          }
        }
        ++iterator;
      }
    }
  }
  return true;
}

/*!
* @fn       registerDataTypesToQml
* @param    None
* @return   None
* @brief    Register CPP Datatypes to qml.
*/
void View::registerDataTypesToQml()
{
  QString sReason = "Enums";

  qmlRegisterUncreatableType<QMLEnums>("QMLEnums",  1, 0, "Enums",     sReason);
  qmlRegisterUncreatableType<VREnums>( "VREnums",   1, 0, "VREnums",   sReason);
  qmlRegisterUncreatableType<Errors>(  "ErrorEnum", 1, 0, "ErrorEnum", "ErrorEnum");
  qRegisterMetaType<QMLEnums::AppState>("QMLEnums::AppState");
  qRegisterMetaType<VREnums::VRViewModeEnum>("VREnums::VRViewModeEnum");
  qRegisterMetaType<VREnums::SlabViewType>("VREnums::SlabViewType");
  qRegisterMetaType<VREnums::AXIS>("VREnums::AXIS");
  VREnums::declareQML();
  qRegisterMetaType<QMLEnums::ThreatEvalPanelCommand>("QMLEnums::ThreatEvalPanelCommand");
  qRegisterMetaType<ModelList> ("ModelList");
  qRegisterMetaType<ConfigBaseElement>("ConfigBaseElement");
  qRegisterMetaType<QMLEnums::ScannerAdminScreenEnum> ("ScannerAdminScreenEnum");
  qRegisterMetaType<QMLEnums::FileDialogAcceptMode> ("FileDialogAcceptMode");
  qRegisterMetaType<uint64_t> ("uint64_t");
  qRegisterMetaType<BagInfo>("BagInfo");
  qRegisterMetaType<QMLEnums::ArchiveFilterSrchLoc> ("QMLEnums::ArchiveFilterSrchLoc");
  qRegisterMetaType<QMLEnums::BagsListAndBagDataCommands> ("BagsListAndBagDataCommands");
  qRegisterMetaType<std::string> ("std::string");
  qRegisterMetaType<QMLEnums::BhsDiverterDecision>("QMLEnums::BhsDiverterDecision");
  qRegisterMetaType<QMLEnums::SupervisorScreenEnum> ("SupervisorScreenEnum");
  qRegisterMetaType<QVector<SupervisorAgents>>("QVector<SupervisorAgents>");
  qRegisterMetaType<QVector<SupervisorRecentEvents>> ("QVector<SupervisorRecentEvents>");
  qRegisterMetaType<QVector<SupervisorGroups>>("QVector<SupervisorGroups>");

#ifdef WORKSTATION
  qRegisterMetaType<ScannerAgentStatusHandler> ("ScannerAgentStatusHandler");
  qRegisterMetaType<WorkstationConfigDao>("WorkstationConfigDao");
  qmlRegisterUncreatableType<TipFileSystemModel>("TipFileSystemModel", 1, 0,
                                                 "TipFileSystemModel",
                                                 "Cannot create a TipFileSystemModel instance.");
  qmlRegisterUncreatableType<SSLErrorEnums>(  "SSLErrorEnums", 1, 0, "SSLErrorEnums", "SSLErrorEnums");
#endif
#ifdef RERUN
  qRegisterMetaType<BagFileModel>("BagFileModel");


  qRegisterMetaType<RerunTaskConfig>("RerunTaskConfig");
  qmlRegisterType<RerunTaskConfig>("RerunTaskConfig", 1, 0, "RerunTaskConfig");
  qmlRegisterUncreatableType<UsbFileSystemModel>("usbfilesystembrowser", 1, 0,
                                                 "UsbFileSystemModel",
                                                 "Cannot create a UsbFileSystemModel instance.");
#endif
}

/*!
* @fn       getBalloonPopupShown
* @param    None
* @return   None
* @brief    get Balloon popup windows is visible.
*/
bool View::getBalloonPopupShown() const
{
  return m_balloonPopupShown;
}

/*!
* @fn       getExpiredAuthMethod
* @param    None
* @return   QMLEnums::AuthMethodType
* @brief    get Expired Auth Method Enum.
*/
int View::getExpiredAuthMethod() const
{
#ifdef WORKSTATION
  return NSSAgentAccessInterface::getInstance()->getExpiredAuthMethod();
#endif
}

/*!
* @fn       getPasswordExpirationDays
* @param    None
* @return   int
* @brief    returns number of days remaning for password to expire.
*/
int View::getPasswordExpirationDays() const
{
#ifdef WORKSTATION
  return NSSAgentAccessInterface::getInstance()->getPasswordExpirationDays();
#endif
}

bool View::setMaintenanceMode(bool enabled)
{
#ifdef WORKSTATION
  return NSSAgentAccessInterface::getInstance()->SetMaintenanceMode(enabled);
#endif
}

/*!
* @fn       reportEventToNssAgent
* @param    QMLEnums::NssEventEnum eventId
* @param    QString eventDescription
* @return   bool
* @brief    Report Event to NSS VM.
*/
bool View::reportEventToNssAgent(QMLEnums::NssEventEnum eventId, QString eventDescription)
{
#ifdef WORKSTATION
  bool result = false;
  NssEventInfo nssEventInfo(eventId, "", eventDescription);
  result = NSSAgentAccessInterface::getInstance()->reportEventToNssAgent(nssEventInfo);
  return result;
#endif
}

/*!
* @fn       setDensitySliderValue
* @param    double val
* @return   void
* @brief    set densssity slider value for non blocking call
*/
void View::setDensitySliderValue(double val) {
  emit densitsliderChanged(val);
}

/*!
* @fn       launchLinuxTerminal
* @param    bool mode
* @return   void
* @brief    launch linux xfce or gnome terminal
*/
void View::launchLinuxTerminal(bool mode) {
  m_linuxTerminal->launchXFCETerminal(mode);
}

/*!
* @fn       exitLinuxTerminal
* @param    void
* @return   void
* @brief    exit linux xfce or gnome terminal
*/
void View::exitLinuxTerminal() {
  m_linuxTerminal->exitXFCETerminal();
}

/*!
* @fn       getScannerFaultScrnShown
* @param    None
* @return   None
* @brief    get Scanner fault windows is visible.
*/
bool View::getScannerFaultScrnShown() const
{
  return m_ScannerFaultScrnShown;
}

/*!
* @fn       setMainContainerFocus
* @param    None
* @return   None
* @brief    set Main Container focus
*/
void View::setMainContainerFocus()
{
  m_qmlContainerWidget->setFocus(Qt::TabFocusReason);
}

/*!
* @fn       getContainer
* @param    None
* @return   None
* @brief    return qmlcontainer.
*/
QQuickView* View::getContainer()
{
  return m_qmlContainer;
}

/*!
* @fn       initAndGetViewInstance
* @param    None
* @return   View*
* @brief    Initalize and get view instance singletone
*/
View* View::initAndGetViewInstance()
{
  if( !m_view )
  {
    DEBUG_LOG("Creating instance of View");
    m_view = new View();
    UILayoutManager *ulmlayout = UILayoutManager::getUILayoutManagerInstance();
    ulmlayout->initialize();
    m_view->registerULMcontext(ulmlayout);
    m_view->initialize();
    DEBUG_LOG("Created new instance of View" << m_view);
    return m_view;
  }
  else
  {
    DEBUG_LOG("Return existing instance of View" << m_view);
    return m_view;
  }
}

/*!
  * @fn       onMousueClickedEvent
  * @param    None
  * @return   None
  * @brief    slot called when mouse click event occurs.
  */
void View::onMousueClickedEvent()
{
  if(m_containerNavigationMenuScrn && m_NavigationMenuScrnShown)
  {
    QPoint currentCursorPosition = m_containerNavigationMenuScrn->mapTo(m_containerNavigationMenuScrn, QCursor::pos());
    if((currentCursorPosition.x() < m_containerNavigationMenuScrn->geometry().x())
       || (currentCursorPosition.x() > (m_containerNavigationMenuScrn->geometry().x() + m_containerNavigationMenuScrn->geometry().width()))
       || (currentCursorPosition.y() > (m_containerNavigationMenuScrn->geometry().y() + m_containerNavigationMenuScrn->geometry().height()))
       || (currentCursorPosition.y() < (m_containerNavigationMenuScrn->geometry().y())))
    {
      showHideNavigationMenu(false);
    }
  }

  if(m_containerScannerFault && m_ScannerFaultScrnShown)
  {
    QPoint currentCursorPosition = m_containerScannerFault->mapTo(m_containerScannerFault, QCursor::pos());
    if((currentCursorPosition.x() < m_containerScannerFault->geometry().x())
       || (currentCursorPosition.x() > (m_containerScannerFault->geometry().x() + m_containerScannerFault->geometry().width()))
       || (currentCursorPosition.y() > (m_containerScannerFault->geometry().y() + m_containerScannerFault->geometry().height()))
       || (currentCursorPosition.y() < (m_containerScannerFault->geometry().y())))
    {
      showHideScannerFaultStatus(false);
    }
  }
}

/*!
* @fn      getOperatorName
* @param   none
* @return  Qstring
* @brief   Function will return current operator name
*/
QString View::getOperatorName()
{
  return currentOperatorName;
}

/*!
* @fn      setOperatorName
* @param   Qstring
* @return  none
* @brief   Function will set current operator name
*/
void View::setOperatorName(QString username)
{
  if(!username.isEmpty())
  {
    currentOperatorName = username;
  }
  else
  {
    currentOperatorName = "";
  }
}

/*!
  * @fn      getFileDialogBrowsePath
  * @param   none
  * @return  QString
  * @brief   returns file dialog browsepath
  */
QString View::getFileDialogBrowsePath()
{
  std::string root_file_path = "";
  root_file_path = UsbStorageSrv::getUsbfilePath();
#ifdef RERUN
  if (root_file_path.empty() || !(RerunConfig::isRemovableStorage()))
    root_file_path = WorkstationConfig::getInstance()->getDefaultDataPath().toStdString();
#endif
  return QString(root_file_path.c_str());
}


/*!
  * @fn      checkFreeSpace
  * @param   none
  * @return  qint64
  * @brief   returns for free bytes available on disk
  */

qint64 View::checkFreeSpace()
{
  QString path = "";
  std::string mountpath = m_usb.getUsbfilePath();
  if(mountpath.empty())
  {
    path = WorkstationConfig::getInstance()->getDefaultDataPath();
  }
  else
  {
    path = WorkstationConfig::getInstance()->getUsbMountPath();
  }

  qint64 size = m_usb.sizeofdrive(path);
  return size;
}
#ifdef WORKSTATION
/*!
* @fn       getNSSUserList
* @return   QStringList
* @brief    Gets list of users from nss services.
*/
QStringList View::getNSSUserList()
{
  return NSSAgentAccessInterface::getInstance()->getNSSUserList();
}

/*!
* @fn       getNSSAgentList
* @return   QStringList
* @brief    Gets list of nss agents from nss services.
*/
QStringList View::getNSSAgentList()
{
  return NSSAgentAccessInterface::getInstance()->getNSSAgentList();
}
#endif

}  // namespace ws
}  // namespace analogic
