//------------------------------------------------------------------------------
// File: MainWindow.cpp
// Description: Implementation of MainWindow for testing integrated
//  Volume Rendering
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include <QGridLayout>
#include <QDebug>
#include <QMessageBox>
#include <QVector2D>
#include <vector>

// #include "MemoryTrace.hpp"
#include "MainWindow.h"
#include "CropDialog.h"
#include "ui_MainWindow.h"


#define VOLUME_AMBIENT_INIT_VALUE   0.1
#define VOLUME_DIFFUSE_INIT_VALUE   0.7
#define VOLUME_SPECULAR_INIT_VALUE  0.2


const int g_toolBarButtonHeight = 40;
const int g_toolBarButtonWidth  = 40;


int g_zoomTestCount = 0;

QString g_defaultWindowStyleSheet = "background-color: #4B4B4B;"
                                    "color: #EEEEEE;";


QString g_styleCheckButton = "QPushButton::checked { background-color: #239B56; }";

bool g_doRightOperatorThreats = true;



#define BAG_QUEUE_DELIMITER ":"

QString g_bagQueuePath   = "bagdir/";
QString g_bagQueueFile   = "BagQueue.txt";


QString g_cycleLoadLogPath   = "junk/";
QString g_cycleLoadFile      = "CycleLoadLog.csv";
QString g_defaultConsoleSave = "VRConsoleSave.txt";

QString g_leftViewerLabel    = "Left View";
QString g_rightViewerLabel   = "Right View";

QString g_volExportPath      = "/home/analogic/junk/";
QString g_volExportFileType  = "VOL File (*.vol)";

QString g_threatExportPath      = "/home/analogic/junk/";
QString g_leftThreatExportFile = "TranslucentThreatObject.txt";
QString g_rightThreatExportFile = "SurfaceThreatObject.txt";

QString g_renderIniFile = "IntegratedVolumeRenderer.ini";

QString g_leakFileOut   = "VolumeRendererLeaks.out";

int g_mainWindowPosX  = 20;
int g_mainWindowPosY  = 50;
int g_mainWindowWidth = 1732;

int g_threatComboWidth = 220;

int g_tdrLineEditWidth = 350;

int g_intValWidth = 40;

int g_queInfoWidth = 70;

int g_autoLoadTimerTimeout = 7000;  // milliseconds.

//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_rendererConfigLeft(nullptr),
  m_rendererConfigRight(nullptr),
  m_internalDataDirectory(""),
  m_grayscaleValuesAndOpacitiesFelePath(""),
  m_materialTransitionFilePath(""),
  m_internalSettingsFilePath(""),
  m_tdrFilepath(""),
  m_sdicosLogFile(""),
  m_currentDataFile(""),
  m_enableThreatHandling(false),
  m_dataManager(nullptr),
  m_bagDataBufferLeft(nullptr),
  m_bagDataBufferRight(nullptr),
  m_doInversecolor(false),
  m_doColorRender(true),
  m_enableLaptopRemoval(false),
  m_cropStatus(false),
  m_leakCheckStatus(false),
  m_syncViewers(true),
  m_useShadingForLeftViewer(true),
  m_useShadingForRightViewer(true),
  m_linearInterpolationForLeftViewer(false),
  m_linearInterpolationForRightViewer(false),
  m_leftOrganicRemovable(true),
  m_leftInorganicRemovable(true),
  m_leftMetalRemovable(true),
  m_rightOrganicRemovable(true),
  m_rightInorganicRemovable(true),
  m_rightMetalRemovable(true),
  m_scanDirLeftToRight(true),
  m_vtkSharpenSplash(false),
  m_vtkDynamicSharpenSplash(false),
  m_currentZoom(1.0),
  m_currentOpacity(DEFAULT_VALUE_OPACITY),
  m_currentImageWidth(0),
  m_currentImageHeight(0),
  m_currentNumSlices(0),
  m_bagLoadTimer(nullptr),
  m_autoLoadEnabled(false),
  m_loadCount(0),
  m_threatCombo(nullptr),
  m_lineEditTDR(nullptr),
  m_machineOperatorIndex(-1),
  m_laptopIndex(-1),
  m_windowLevelLineEdit(nullptr),
  m_colorLevelLineEdit(nullptr),
  m_bagQueueStatusLineEdit(nullptr),
  m_clearConsole(nullptr),
  m_loadBagButton(nullptr),
  m_cycleLoadButton(nullptr),
  m_leakCheckIcon(nullptr),
  m_cropIcon(nullptr),
  m_reloadButton(nullptr),
  m_cycleLoadLogFile(""),
  m_bagQueueIndex(0),
  m_bagTag(""),
  m_renderMutex(nullptr),
  m_cropFile(""),
  m_logWindow(nullptr),
  m_canClose(false),
  m_operatorThreatSelectionArmed(false),
  m_operatorThreatP0(QVector3D(0, 0, 0)),
  m_operatorThreatExtent(QVector3D(0, 0, 0)),
  m_rightviewMode(VREnums::VIEW_AS_THREAT)
{
//----------------------------
#ifdef ARO_TEST_DEFINE
  std::cout << "Special Build with DEFINE: ARO_TEST_DEFINE" << std::endl;
#endif
//----------------------------
  LOG(INFO) << "Begin MainWindow Constructor";
  ui->setupUi(this);

  this->setStyleSheet(g_defaultWindowStyleSheet);


  this->setWindowIcon(QIcon(":images/MainIcon.png"));

  this->move(g_mainWindowPosX,
             g_mainWindowPosY);
  this->setMinimumWidth(g_mainWindowWidth);

  this->getRenderSettingsFromIni();

  QLabel* leftRenwin = new QLabel("", nullptr);
  ui->vtkLayout->addWidget(leftRenwin,  0, 0, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
  m_volumeRendererTrancelucentView = new analogic::workstation::VolumeRendererWidget(leftRenwin);

  QLabel* rightRenwin = new QLabel("", nullptr);
  ui->vtkLayout->addWidget(rightRenwin, 0, 1, 1, Qt::AlignRight | Qt::AlignBottom );
  rightRenwin->lower();
  m_volumeRendererOpaqueView  = new analogic::workstation::VolumeRendererWidget(rightRenwin);


  //-------------------------------------------------
  // Set use splash screen status fopr both viewers.
  m_volumeRendererTrancelucentView->SetEnableSplashScreen(m_vtkSharpenSplash);
  m_volumeRendererOpaqueView->SetEnableSplashScreen(m_vtkSharpenSplash);

  m_volumeRendererTrancelucentView->SetEnableDynamicSplash(m_vtkDynamicSharpenSplash);
  m_volumeRendererOpaqueView->SetEnableDynamicSplash(m_vtkDynamicSharpenSplash);

  //-------------------------------------------------
  // Set the View Label for each window.
  m_volumeRendererTrancelucentView->SetDisplayLabel(g_leftViewerLabel);
  m_volumeRendererOpaqueView->SetDisplayLabel(g_rightViewerLabel);

  //-------------------------------------------------
  // Turn off Threat Renderer for Right Viewwer.
  m_volumeRendererOpaqueView->ShowThreatView(false);


  //-------------------------------------------------
  // Turn off View Label for right viewer.
  m_volumeRendererOpaqueView->ShowViewLabel(false);



  std::string homeFolder;
  analogic::workstation::getHomeFolder(homeFolder);
  m_cycleLoadLogFile =  homeFolder.c_str();
  m_cycleLoadLogFile += "/";
  m_cycleLoadLogFile += g_cycleLoadLogPath;
  m_cycleLoadLogFile += g_cycleLoadFile;


  this->setupMenu();
  this->setupToolbar();
  this->setupOperatorSelect();
  this->setupSceneSettingsSelection();
  this->setupBagQueue();
  this->setupLogWindiow();
  this->setupModeRadio();
  this->initUIComponents();

  // .................

  this->showBagQueueInfo();


  //----------------------------
  // Syncing Viewers
  std::list<analogic::workstation::VolumeRendererWidget*> sibRight;
  std::list<analogic::workstation::VolumeRendererWidget*> sibLeft;
  sibRight.push_back(m_volumeRendererTrancelucentView);
  sibLeft.push_back(m_volumeRendererOpaqueView);

  m_volumeRendererTrancelucentView->SetLinkSiblingsList(sibLeft);
  m_volumeRendererOpaqueView->SetLinkSiblingsList(sibRight);
  m_volumeRendererTrancelucentView->SetScannerDirectionLeftToRight(m_scanDirLeftToRight);
  m_volumeRendererOpaqueView->SetScannerDirectionLeftToRight(m_scanDirLeftToRight);

  this->linkVtkViewers(m_syncViewers);
  //----------------------------



  bool initOK  = this->initSettingsPaths();
  if (!initOK)
  {
    LOG(INFO) << "Error initializing settings path! Closing app.";
    QApplication::quit();
  }

  try
  {
    m_rendererConfigLeft = new analogic::workstation::VolumeRendererConfig(m_mapFileDir,
                                                                           m_normalOpacityFileList,
                                                                           m_normalHSVColorMapFileList,
                                                                           m_grayscaleValuesAndOpacitiesFelePath,
                                                                           m_materialTransitionFilePath,
                                                                           m_internalSettingsFilePath,
                                                                           m_enableThreatHandling,
                                                                           m_useShadingForLeftViewer,
                                                                           m_linearInterpolationForLeftViewer,
                                                                           m_leftOrganicRemovable,
                                                                           m_leftInorganicRemovable,
                                                                           m_leftMetalRemovable);
  }
  catch( const std::exception& error)
  {
    std::string errorMessage = error.what();
    QMessageBox::warning(this,
                         "Application Error.",
                         errorMessage.c_str());
    QApplication::quit();
  }

  try
  {
    m_rendererConfigRight = new analogic::workstation::VolumeRendererConfig(m_mapFileDir,
                                                                            m_surfaceOpacityFileList,
                                                                            m_surfaceHSVColorMapFileList,
                                                                            m_grayscaleValuesAndOpacitiesFelePath,
                                                                            m_materialTransitionFilePath,
                                                                            m_internalSettingsFilePath,
                                                                            m_enableThreatHandling,
                                                                            m_useShadingForRightViewer,
                                                                            m_linearInterpolationForRightViewer,
                                                                            m_rightOrganicRemovable,
                                                                            m_rightInorganicRemovable,
                                                                            m_rightMetalRemovable);
  }
  catch( const std::exception& error)
  {
    std::string errorMessage = error.what();
    QMessageBox::warning(this,
                         "Application Error.",
                         errorMessage.c_str());
    QApplication::quit();
  }

  m_enableThreatHandling     = m_rendererConfigLeft->m_enableThreatHandling;

  BagQueueItem bi = m_bagQueue.at(0);

  // std::string dataFile   =  m_internalDataDirectory.toLocal8Bit().data();
  // dataFile += bi.m_volumeFileName.toLocal8Bit().data();

  std::string dataFile   = bi.m_volumeFileName.toLocal8Bit().data();


  m_currentDataFile      =  dataFile.c_str();


  m_currentTDRFile       =  bi.m_tdrList.front();

  this->setWindowTileWithDataFile(m_currentDataFile);
  this->setTDRLabel(m_currentTDRFile);

  LOG(INFO) << "Ready to populate and Init data";


  std::list<QString>::iterator it;
  std::list<QString> tdrList;

  for (it = bi.m_tdrList.begin(); it != bi.m_tdrList.end(); it++)
  {
    QString tdrFilepath  = *it;
    tdrList.push_back(tdrFilepath);
  }
  m_dataManager = new DataManager(m_currentDataFile, tdrList);

  m_dataManager->readTDRModules(m_sdicosLogFile);

  m_dataManager->loadVolumeData();

  this->populateAndInit(m_dataManager);

   // Initialize the widget used to look at sub volumes.
   m_volumeRendererTrancelucentView->InitPickBoxWidget();
   m_volumeRendererOpaqueView->InitPickBoxWidget();


  //---------------------------------------
  // Setup Auto Load timer
   m_bagLoadTimer = new QTimer(this);
   m_bagLoadTimer->setInterval(g_autoLoadTimerTimeout);
   m_bagLoadTimer->setSingleShot(true);
   this->connect(m_bagLoadTimer, &QTimer::timeout,
                 this, &MainWindow::onBagLoadTimerUpdate);

  this->setupThreatCombo();




  this->doAppendToTextLabel();
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  m_logWindow->close();
  delete ui;
  delete m_dataManager;
  delete m_rendererConfigLeft;
  delete m_rendererConfigRight;
  delete m_renderMutex;
  delete m_logWindow;
}

//------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent* event)
{
  m_logWindow->logString("Recieving Close Event: at MainWindow!");
  m_canClose = true;
  if (m_logWindow)
  {
   m_logWindow->close();
  }
  event->accept();
}

//------------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_F7)
  {
    if (event->modifiers() & Qt::ShiftModifier)
    {
      this->onTestImageOverlay();
      QMessageBox::information(this,
                               "Screen Shot caprured",
                               "Screen shot saved to file!");
    }
  }
}

//------------------------------------------------------------------------------
void MainWindow::setButtonImageAndText(QPushButton* pButton,
                                       QString imageSource,
                                       QString buttonTip,
                                       int buttonWidth)
{
  QPixmap pixmap(imageSource);
  QIcon buttonIncon(pixmap);
  pButton->setText("");
  pButton->setToolTip(buttonTip);
  pButton->setIcon(buttonIncon);
  pButton->setIconSize(pixmap.rect().size());
  pButton->setMaximumWidth(buttonWidth);
}

//------------------------------------------------------------------------------
bool MainWindow::getRenderSettingsFromIni()
{
  std::string resourceFolder;
  analogic::workstation::getResourcesFolder(resourceFolder);
  std::string renderIniFile = resourceFolder + VOLUME_RENDERER_GUI_INI_FILE;

  qDebug() << renderIniFile.c_str();


  if ( strlen(renderIniFile.c_str()) > 0 &&
       boost::filesystem::exists(renderIniFile.c_str())
       )
  {
    LOG(INFO) << "Volume Renderer render settings file: " << renderIniFile;
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(renderIniFile, pt);

    //------------------
    // Enable Threat handling
    try
    {
      bool enabelThreats = pt.get<bool>("RenderParameter.EnableThreatHandling");
      m_enableThreatHandling = enabelThreats;
    }
    catch(...)
    {
      m_enableThreatHandling = DEF_PARAM_SHOW_THREATS;
    }
    //------------------
    //------------------
    // Shading Left Viewer
    try
    {
      bool useShadingForLeft = pt.get<bool>("RenderParameter.TranslucentPanelShading");
      m_useShadingForLeftViewer = useShadingForLeft;
    }
    catch (...)
    {
      m_useShadingForLeftViewer = DEF_PARAM_TRANSLUCENT_SHADING;
    }
    LOG(INFO) << "Setting RenderParameter.TranslucentPanelShading: " << m_useShadingForLeftViewer;
    //-----------------
    //------------------
    // Shading Right Viewer
    try
    {
      bool useShadingForRight = pt.get<bool>("RenderParameter.SurfacePanelShading");
      m_useShadingForRightViewer = useShadingForRight;
    }
    catch (...)
    {
      m_useShadingForRightViewer = DEF_PARAM_SURFACE_SHADING;
    }
    LOG(INFO) << "Setting RenderParameter.SurfacePanelShading: " << m_useShadingForRightViewer;
    //-----------------
    //------------------
    // Left Viewer linear interpolation
    try
    {
      bool linearInterpolationForLeft = pt.get<bool>("RenderParameter.TranslucentInterpolationLinear");
      m_linearInterpolationForLeftViewer = linearInterpolationForLeft;
    }
    catch (...)
    {
      m_linearInterpolationForLeftViewer = DEF_PARAM_LINEAR_INTERPOL_LEFT;
    }
    LOG(INFO) << "Setting RenderParameter.TranslucentInterpolationLinear: " << m_linearInterpolationForLeftViewer;
    //-----------------
    //------------------
    // Right Viewer linear interpolation
    try
    {
      bool linearInterpolationForRight = pt.get<bool>("RenderParameter.SurfaceInterpolationLinear");
      m_linearInterpolationForRightViewer = linearInterpolationForRight;
    }
    catch (...)
    {
      m_linearInterpolationForRightViewer = DEF_PARAM_LINEAR_INTERPOL_RIGHT;
    }
    LOG(INFO) << "Setting RenderParameter.SurfaceInterpolationLinear: " << m_linearInterpolationForRightViewer;
    //-----------------
    //-----------------
    // Left Translucent organic removable
    try
    {
      bool translucentOrganicRemovable = pt.get<bool>("RenderParameter.TranslucentOrganicRemovable");
      m_leftOrganicRemovable = translucentOrganicRemovable;
    }
    catch (...)
    {
      m_leftOrganicRemovable = DEF_PARAM_TRANSLUCENT_ORG_REMOVAL;
    }
    LOG(INFO) << "Setting RenderParameter.TranslucentOrganicRemovable: " << m_leftOrganicRemovable;
    //-----------------
    //-----------------
    // Surface inorganic removable
    try
    {
      bool translucentInorganicRemovable = pt.get<bool>("RenderParameter.TranslucentInorganicRemovable");
      m_leftInorganicRemovable = translucentInorganicRemovable;
    }
    catch (...)
    {
      m_leftInorganicRemovable = DEF_PARAM_TRANSLUCENT_INORG_REMOVAL;
    }
    LOG(INFO) << "Setting RenderParameter.TranslucentInorganicRemovable: " << m_leftInorganicRemovable;
    //-----------------
    //-----------------
    // Surface metal removable
    try
    {
      bool translucentMetalRemovable = pt.get<bool>("RenderParameter.TranslucentMetalRemovable");
      m_leftMetalRemovable = translucentMetalRemovable;
    }
    catch (...)
    {
      m_leftMetalRemovable = DEF_PARAM_TRANSLUCENT_METAL_REMOVAL;
    }
    LOG(INFO) << "Setting RenderParameter.TranslucentMetalRemovable: " << m_leftMetalRemovable;
    //-----------------
    //-----------------
    // Surface organic removable
    try
    {
      bool surfaceOrganicRemovable = pt.get<bool>("RenderParameter.SurfaceOrganicRemovable");
      m_rightOrganicRemovable = surfaceOrganicRemovable;
    }
    catch (...)
    {
      m_rightOrganicRemovable = DEF_PARAM_SURFACE_ORG_REMOVAL;
    }
    LOG(INFO) << "Setting RenderParameter.SurfaceOrganicRemovable: " << m_rightOrganicRemovable;
    //-----------------
    //-----------------
    // Surface inorganic removable
    try
    {
      bool surfaceInorganicRemovable = pt.get<bool>("RenderParameter.SurfaceInorganicRemovable");
      m_rightInorganicRemovable = surfaceInorganicRemovable;
    }
    catch (...)
    {
      m_rightInorganicRemovable = DEF_PARAM_SURFACE_INORG_REMOVAL;
    }
    LOG(INFO) << "Setting RenderParameter.SurfaceInorganicRemovable: " << m_rightInorganicRemovable;
    //-----------------
    //-----------------
    // Surface metal removable
    try
    {
      bool surfaceMetalRemovable = pt.get<bool>("RenderParameter.SurfaceMetalRemovable");
      m_rightMetalRemovable = surfaceMetalRemovable;
    }
    catch (...)
    {
      m_rightMetalRemovable = DEF_PARAM_SURFACE_METAL_REMOVAL;
    }
    LOG(INFO) << "Setting RenderParameter.SurfaceMetalRemovable: " << m_rightMetalRemovable;
    //-----------------
    //----------
    // Translucent Ambient Lighting
    try
    {
        double translucent_ambient_lighting = pt.get<double>("RenderParameter.TranslucentAmbientLighting");
        m_translucentAmbientLighting = translucent_ambient_lighting;
    }
    catch (...)
    {
        m_translucentAmbientLighting = DEF_RENDER_PARAM_TRANSLUCENT_AMBIENT_LIGHTING;
    }
    LOG(INFO) << "Setting RenderParameter.TranslucentAmbientLighting: " << m_translucentAmbientLighting;
    //----------
    //----------
    // Translucent Diffuse Lighting
    try
    {
        double translucent_diffuse_lighting = pt.get<double>("RenderParameter.TranslucentDiffuseLighting");
        m_translucentDiffuseLighting = translucent_diffuse_lighting;
    }
    catch (...)
    {
        m_translucentDiffuseLighting = DEF_RENDER_PARAM_TRANSLUCENT_DIFFUSE_LIGHTING;
    }
    LOG(INFO) << "Setting RenderParameter.TranslucentDiffuseLighting: " << m_translucentDiffuseLighting;
    //----------
    //----------
    // Translucent Specular Lighting
    try
    {
        double translucent_specular_lighting = pt.get<double>("RenderParameter.TranslucentSpecularLighting");
        m_translucentSpecularLighting = translucent_specular_lighting;
    }
    catch (...)
    {
        m_translucentSpecularLighting = DEF_RENDER_PARAM_TRANSLUCENT_SPECULAR_LIGHTING;
    }
    LOG(INFO) << "Setting RenderParameter.TranslucentSpecularLighting: " << m_translucentSpecularLighting;
    //----------
    //----------
    // Surface Ambient Lighting
    try
    {
        double surface_ambient_lighting = pt.get<double>("RenderParameter.SurfaceAmbientLighting");
        m_surfaceAmbientLighting = surface_ambient_lighting;
    }
    catch (...)
    {
        m_surfaceAmbientLighting = DEF_RENDER_PARAM_SURFACE_AMBIENT_LIGHTING;
    }
    LOG(INFO) << "Setting RenderParameter.SurfaceAmbientLighting: " << m_surfaceAmbientLighting;
    //----------
    //----------
    // Surface diffuse Lighting
    try
    {
        double surface_diffuse_lighting = pt.get<double>("RenderParameter.SurfaceDiffuseLighting");
        m_surfaceDiffuseLighting = surface_diffuse_lighting;
    }
    catch (...)
    {
        m_surfaceDiffuseLighting = DEF_RENDER_PARAM_SURFACE_DIFFUSE_LIGHTING;
    }
    LOG(INFO) << "Setting RenderParameter.SurfaceDiffuseLighting: " << m_surfaceDiffuseLighting;
    //----------
    //----------
    // Surface Specular Lighting
    try
    {
        double surface_specular_lighting = pt.get<double>("RenderParameter.SurfaceSpecularLighting");
        m_surfaceSpecularLighting = surface_specular_lighting;
    }
    catch (...)
    {
        m_surfaceSpecularLighting = DEF_RENDER_PARAM_SURFACE_SPECULAR_LIGHTING;
    }
    LOG(INFO) << "Setting RenderParameter.SurfaceSpecularLighting: " << m_surfaceSpecularLighting;
    //----------
    //-----------------
    // Default scan direction
    try
    {
      bool defScanDirLeftToRight = pt.get<bool>("RenderParameter.ScanDirectionLeftToRight");
      m_scanDirLeftToRight = defScanDirLeftToRight;
    }
    catch (...)
    {
      m_scanDirLeftToRight = DEF_SCAN_DIR_LEFT_TO_RIGHT;
    }
    LOG(INFO) << "Setting RenderParameter.ScanDirectionLeftToRight: " << m_scanDirLeftToRight;
    //-----------------
    // VTK Sharpen Splash Screen
    try
    {
      bool vtkSharpenSplah = pt.get<bool>("RenderParameter.VTKSharpenSplashScreen");
      m_vtkSharpenSplash = vtkSharpenSplah;
    }
    catch (...)
    {
      m_vtkSharpenSplash = DEF_RENDER_VTK_SHARPEN_SPLASH;
    }
    //-----------------
    // VTK Dynamic Sharpen Splash Screen
    try
    {
      bool vtkDynamicSharpenSplah = pt.get<bool>("RenderParameter.VTKDynamicSplashScreen");
      m_vtkDynamicSharpenSplash = vtkDynamicSharpenSplah;
    }
    catch (...)
    {
      m_vtkDynamicSharpenSplash = DEF_RENDER_VTK_DYNAMIC_SHARPEN_SPLASH;
    }
  }
  else
  {
    std::string errorMessage = "Volume Renderer render settings file not found:" +
        renderIniFile;
    LOG(INFO) << errorMessage;
    throw std::runtime_error(errorMessage);
  }
  return true;
}


//------------------------------------------------------------------------------
void MainWindow::setupMenu()
{
  this->connect(ui->actionReset_View, &QAction::triggered,
                this, &MainWindow::onResetView);


  this->connect(ui->actionMakeRightViewerAThreatRenderer, &QAction::triggered,
                this, &MainWindow::onSetRightAsThreat);

  this->connect(ui->actionPan_Right_Viewer, &QAction::triggered,
                this, &MainWindow::onPanRightView);


  this->connect(ui->action_ResetRightRenderer, &QAction::triggered,
                this, &MainWindow::onResetRightRenderer);

  this->connect(ui->actionCenter_Right_Viewer, &QAction::triggered,
                this, &MainWindow::onCenterRightViewer);

  this->connect(ui->actionNew_Bag, &QAction::triggered,
                this, &MainWindow::onNewBag);

  this->connect(ui->actionReload_Bag, &QAction::triggered,
                this, &MainWindow::onReloadBag);

  this->connect(ui->actionShowCameraInfo, &QAction::triggered,
                this, &MainWindow::onShowCameraInfo);

  this->connect(ui->actionShow_Volume_Bounds, &QAction::triggered,
                this, &MainWindow::onShowVolumeBounds);

  this->connect(ui->actionEdit_Crop_ROI, &QAction::triggered,
                this, &MainWindow::onEditCropROI);

  this->connect(ui->actionExit,  &QAction::triggered,
                this, &MainWindow::onAppClose);

  this->connect(ui->actionClear_Bag, &QAction::triggered,
                this,  &MainWindow::onClearScene);

  this->connect(ui->actionExport_Full_HSV_Op_TFs, &QAction::triggered,
                this,  &MainWindow::onExportFullTFs);

  this->connect(ui->actionCheck_Leaks, &QAction::triggered,
                this, &MainWindow::onLeakCheck);

  this->connect(ui->actionExportThreatData, &QAction::triggered,
                this, &MainWindow::onExportThreatData);

  this->connect(ui->actionThreat_Window_Print_Self, &QAction::triggered,
                this, &MainWindow::onThreatWindowsPrintSelf);

  this->connect(ui->actionShow_Current_Threat_Volume, &QAction::triggered,
                 this, &MainWindow::onShowCurrentThreatVolume);


  this->connect(ui->actionTest_Camera_Transform,  &QAction::triggered,
                this, &MainWindow::onTestTransformCamera);

  this->connect(ui->actionTestWindowLevel,  &QAction::triggered,
                this, &MainWindow::onTestWindowLevel);

  this->connect(ui->actionTestColorLevel,  &QAction::triggered,
                this, &MainWindow::onTestColorLevel);

  this->connect(ui->actionTestImageOverlay, &QAction::triggered,
                this, &MainWindow::onTestImageOverlay);

  this->connect(ui->actionStart_Leak_Monitor, &QAction::triggered,
                this, &MainWindow::onStartLeakCheck);

  this->connect(ui->actionPause_Leak_Monitor, &QAction::triggered,
                this, &MainWindow::onPauseLeakCheck);

  this->connect(ui->actionInsert_VTK_Lebel_Text, &QAction::triggered,
                this, &MainWindow::onInsertVTKLabelText);

  this->connect(ui->actionTest_Zoom_Value, &QAction::triggered,
                this, &MainWindow::onTestZoomValue);


  this->connect(ui->actionShowCameraAndSceneProperties, &QAction::triggered,
                this, &MainWindow::onResetThreatWindow);


  this->connect(ui->actionSet_Test_Update_Rate, &QAction::triggered,
                this, &MainWindow::onTestUpdateRate);

  this->connect(ui->actionTest_View_Mode, &QAction::triggered,
                this, &MainWindow::onTestViewMode);


  this->connect(ui->actionStash_Camera, &QAction::triggered,
                this, &MainWindow::onStashCurrentCamera);

  this->connect(ui->actionRestore_Camera, &QAction::triggered,
                this, &MainWindow::onRestoreStashCamera);
 }

//------------------------------------------------------------------------------
void MainWindow::setupToolbar()
{
  m_threatCombo = new QComboBox(nullptr);
  m_threatCombo->setMaximumWidth(g_threatComboWidth);
  m_threatCombo->setMinimumWidth(g_threatComboWidth);
  ui->mainToolBar->addWidget(m_threatCombo);

  m_lineEditTDR = new QLineEdit(nullptr);
  m_lineEditTDR->setMinimumWidth(g_tdrLineEditWidth);
  m_lineEditTDR->setMaximumWidth(g_tdrLineEditWidth);
  m_lineEditTDR->setReadOnly(true);
  QLabel*tdrLab = new QLabel("TDR file:", nullptr);
  ui->mainToolBar->addWidget(tdrLab);
  ui->mainToolBar->addWidget(m_lineEditTDR);


  // Window Level and Color level
  QLabel* wlLab = new QLabel("Wind Level:", nullptr);
  m_windowLevelLineEdit = new QLineEdit(nullptr);

  QLabel* clLab = new QLabel("Color Level:", nullptr);
  m_colorLevelLineEdit = new QLineEdit(nullptr);
  m_windowLevelLineEdit->setMinimumWidth(g_intValWidth);
  m_windowLevelLineEdit->setMaximumWidth(g_intValWidth);
  m_colorLevelLineEdit->setMinimumWidth(g_intValWidth);
  m_colorLevelLineEdit->setMaximumWidth(g_intValWidth);
  m_windowLevelLineEdit->setReadOnly(true);
  m_colorLevelLineEdit->setReadOnly(true);

  m_windowLevelLineEdit->setText("1.0");
  m_colorLevelLineEdit->setText("0.5");

  ui->mainToolBar->addWidget(wlLab);
  ui->mainToolBar->addWidget(m_windowLevelLineEdit);
  ui->mainToolBar->addWidget(clLab);
  ui->mainToolBar->addWidget(m_colorLevelLineEdit);

  // Bag Queue index and count
  QLabel* bagQueueStatusLab = new QLabel("Bag Queue index:", nullptr);
  m_bagQueueStatusLineEdit = new QLineEdit(nullptr);
  m_bagQueueStatusLineEdit->setMinimumWidth(g_queInfoWidth);
  m_bagQueueStatusLineEdit->setMaximumWidth(g_queInfoWidth);
  m_bagQueueStatusLineEdit->setReadOnly(true);
  ui->mainToolBar->addWidget(bagQueueStatusLab);
  ui->mainToolBar->addWidget(m_bagQueueStatusLineEdit);


  // Clear console button
  m_clearConsole = new QPushButton(this);
  m_clearConsole->setMinimumWidth(g_toolBarButtonWidth);
  m_clearConsole->setMinimumHeight(g_toolBarButtonHeight);
  m_clearConsole->setIcon(QIcon(":images/ClearLogIcon.png"));
  m_clearConsole->setIconSize(QSize(g_toolBarButtonWidth  - 3,
                                    g_toolBarButtonHeight - 3));
  m_clearConsole->setToolTip("Clear Console");
  ui->mainToolBar->addWidget(m_clearConsole);



  // Load bag from file button
  m_loadBagButton = new QPushButton(this);
  m_loadBagButton->setMinimumWidth(g_toolBarButtonWidth);
  m_loadBagButton->setMinimumHeight(g_toolBarButtonHeight);
  m_loadBagButton->setIcon(QIcon(":images/LoadFileIcon.png"));
  m_loadBagButton->setIconSize(QSize(g_toolBarButtonWidth  - 3,
                                    g_toolBarButtonHeight - 3));
  m_loadBagButton->setToolTip("Load Bag From File");
  ui->mainToolBar->addWidget(m_loadBagButton);


  // Cycle Load Button
  m_cycleLoadButton = new QPushButton(this);
  m_cycleLoadButton->setMinimumWidth(g_toolBarButtonWidth);
  m_cycleLoadButton->setMinimumHeight(g_toolBarButtonHeight);
  m_cycleLoadButton->setIcon(QIcon(":images/LifeCycleIcon.png"));
  m_cycleLoadButton->setIconSize(QSize(g_toolBarButtonWidth  - 3,
                                    g_toolBarButtonHeight - 3));
  m_cycleLoadButton->setToolTip("Cycle Trough Bag Queue");
  m_cycleLoadButton->setCheckable(true);
  m_cycleLoadButton->setStyleSheet(g_styleCheckButton);
  ui->mainToolBar->addWidget(m_cycleLoadButton);


  m_leakCheckIcon = new QLabel("", nullptr);
  this->setLeakCheckStatus();
  ui->mainToolBar->addWidget(m_leakCheckIcon);


  m_cropIcon = new QLabel("", nullptr);
  this->setCropStatus();
  ui->mainToolBar->addWidget(m_cropIcon);

  // Reload Button
  m_reloadButton = new QPushButton(this);
  m_reloadButton->setMinimumWidth(g_toolBarButtonWidth);
  m_reloadButton->setMinimumHeight(g_toolBarButtonHeight);
  m_reloadButton->setIcon(QIcon(":images/ReloadFileIcon.png"));
  m_reloadButton->setIconSize(QSize(g_toolBarButtonWidth  - 3,
                                    g_toolBarButtonHeight - 3));
  m_reloadButton->setToolTip("Reload Current Bag");
  ui->mainToolBar->addWidget(m_reloadButton);
}


//------------------------------------------------------------------------------
void MainWindow::setupOperatorSelect()
{
  ui->buttonOperatorThreatRubberBand->setIcon(QIcon(":images/PickBoxArm.png"));
  ui->buttonOperatorThreatRubberBand->setIconSize(QSize(g_toolBarButtonWidth,
                                                  g_toolBarButtonHeight));
  ui->buttonOperatorThreatRubberBand->setToolTip("Add Operator Threat");


  ui->buttonOpperatorAccept->setIcon(QIcon(":images/PickBoxAccept.png"));
  ui->buttonOpperatorAccept->setIconSize(QSize(g_toolBarButtonWidth,
                                         g_toolBarButtonHeight));
  ui->buttonOpperatorAccept->setToolTip("Accept Operator Threat");




  ui->operatorThreatCancel->setIcon(QIcon(":images/PickBoxCancel.png"));
  ui->operatorThreatCancel->setIconSize(QSize(g_toolBarButtonWidth,
                                        g_toolBarButtonHeight));
  ui->operatorThreatCancel->setToolTip("Cancel Operator Threat");

  ui->deleteOperatorThreat->setIcon(QIcon(":images/DeleteOperatorThreatIcon.png"));
  ui->deleteOperatorThreat->setIconSize(QSize(g_toolBarButtonWidth,
                                              g_toolBarButtonHeight));
  ui->deleteOperatorThreat->setToolTip("Delete Operator Threat");

  ui->pickPlusButton->setIcon(QIcon(":images/PickBoxSizeUp.png"));
  ui->pickPlusButton->setIconSize(QSize(g_toolBarButtonWidth,
                                  g_toolBarButtonHeight));
  ui->pickPlusButton->setToolTip("Enlarge Operator Threat");


  ui->pickMinusButton->setIcon(QIcon(":images/PickBoxSizeDown.png"));
  ui->pickMinusButton->setIconSize(QSize(g_toolBarButtonWidth,
                                   g_toolBarButtonHeight));
  ui->pickMinusButton->setToolTip("Reduce Operator Threat");
}



//------------------------------------------------------------------------------
void MainWindow::setupSceneSettingsSelection()
{
  ui->allMaterialsButton->setIcon(QIcon(":images/AllMaterialsIcon.png"));
  ui->allMaterialsButton->setIconSize(QSize(g_toolBarButtonWidth,
                                      g_toolBarButtonHeight));
  ui->allMaterialsButton->setToolTip("Show All Materials");

  ui->showMetalButton->setIcon(QIcon(":images/ShowMetalIcon.png"));
  ui->showMetalButton->setIconSize(QSize(g_toolBarButtonWidth,
                                      g_toolBarButtonHeight));
  ui->showMetalButton->setToolTip("Show Metals");

  ui->showOrganicButton->setIcon(QIcon(":images/ShowOrganicIcon.png"));
  ui->showOrganicButton->setIconSize(QSize(g_toolBarButtonWidth,
                                      g_toolBarButtonHeight));
  ui->showOrganicButton->setToolTip("Show Organics");

  ui->showGrayscaleButton->setIcon(QIcon(":images/GrayscaleIcon.png"));
  ui->showGrayscaleButton->setIconSize(QSize(g_toolBarButtonWidth,
                                      g_toolBarButtonHeight));
  ui->showGrayscaleButton->setToolTip("Show Grayscale");

  ui->showInverseButton->setIcon(QIcon(":images/InverseIcon.png"));
  ui->showInverseButton->setIconSize(QSize(g_toolBarButtonWidth,
                                      g_toolBarButtonHeight));
  ui->showInverseButton->setToolTip("Show Inverse");


  ui->cameraButton->setIcon(QIcon(":images/CameraIcon02.png"));
  ui->cameraButton->setIconSize(QSize(g_toolBarButtonWidth,
                                      g_toolBarButtonHeight));
  ui->cameraButton->setToolTip("Show Right as Photo");
}


//------------------------------------------------------------------------------
void MainWindow::showWindowLevel(float winLevel)
{
 m_windowLevelLineEdit->setText(QString::number(winLevel));
}

//------------------------------------------------------------------------------
void MainWindow::showColorLevel(float colLevel)
{
  m_colorLevelLineEdit->setText(QString::number(colLevel));
}


//------------------------------------------------------------------------------
void MainWindow::setCropStatus()
{
  QString labTip = "Crop Status";
  int defaultLebelWidth = 20;
  if (m_cropStatus)
  {
    QPixmap pixmap(":images/GreenLEDIcon.png");
    QPixmap tinyMap = pixmap.scaledToWidth(defaultLebelWidth,
                                           Qt::SmoothTransformation);
    m_cropIcon->setText("");
    m_cropIcon->setToolTip(labTip);
    m_cropIcon->setPixmap(tinyMap);
    m_cropIcon->setMaximumWidth(defaultLebelWidth);
  }
  else
  {
    QPixmap pixmap(":images/GrayLEDIcon.png");
    QPixmap tinyMap = pixmap.scaledToWidth(defaultLebelWidth,
                                           Qt::SmoothTransformation);
    pixmap.scaledToWidth(defaultLebelWidth);
    m_cropIcon->setText("");
    m_cropIcon->setToolTip(labTip);
    m_cropIcon->setPixmap(tinyMap);
    m_cropIcon->setMaximumWidth(defaultLebelWidth);
  }
}

//------------------------------------------------------------------------------
void MainWindow::setLeakCheckStatus()
{
  QString labTip = "Leak Check Status";
  int defaultLebelWidth = 20;
  if (m_leakCheckStatus)
  {
    QPixmap pixmap(":images/SquareRedLed.png");
    QPixmap tinyMap = pixmap.scaledToWidth(defaultLebelWidth,
                                           Qt::SmoothTransformation);
    m_leakCheckIcon->setText("");
    m_leakCheckIcon->setToolTip(labTip);
    m_leakCheckIcon->setPixmap(tinyMap);
    m_leakCheckIcon->setMaximumWidth(defaultLebelWidth);
  }
  else
  {
    QPixmap pixmap(":images/SquareGrayLed.png");
    QPixmap tinyMap = pixmap.scaledToWidth(defaultLebelWidth,
                                           Qt::SmoothTransformation);
    pixmap.scaledToWidth(defaultLebelWidth);
    m_leakCheckIcon->setText("");
    m_leakCheckIcon->setToolTip(labTip);
    m_leakCheckIcon->setPixmap(tinyMap);
    m_leakCheckIcon->setMaximumWidth(defaultLebelWidth);
  }
}

//------------------------------------------------------------------------------
void MainWindow::setupBagQueue()
{
  // First get the foler with the file.
  std::string homeFolder;
  analogic::workstation::getHomeFolder(homeFolder);
  std::string devRoot;
  bool gotDevRoot = analogic::workstation::getDevRootFolder(devRoot);
  if (!gotDevRoot) return;

  QString testFile = "./";
  testFile += g_bagQueueFile;
  QFileInfo testFileInfo(testFile);
  if (!testFileInfo.exists())
  {
    QString message = "Bag Queue File not found: ";
    message += testFile;
    this->consoleLogString(message);
    return;
  }
  this->readBagQueue(testFile);
}


//------------------------------------------------------------------------------
void MainWindow::showBagQueueInfo()
{
  QString labText = QString::number(m_bagQueueIndex+1) +
                    " of ";
  labText += QString::number(m_bagQueue.size());
  m_bagQueueStatusLineEdit->setText(labText);
}

//------------------------------------------------------------------------------
void MainWindow::setupThreatCombo()
{
  this->connect(m_threatCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &MainWindow::onThreatComboSlectionChanged);

  this->connect(ui->buttonCycleThreat, &QPushButton::clicked,
                this, &MainWindow::onCycleThreat);

  this->connect(ui->buttonClearThreat, &QPushButton::clicked,
                this, &MainWindow::onClearThreat);

  this->connect(ui->buttonSuspectBag, &QPushButton::clicked,
                this, &MainWindow::onSuspectThreat);
}

//------------------------------------------------------------------------------
void MainWindow::setRadioselectionStatus(VREnums::VRViewModeEnum aMode)
{
  ui->checkBoxThreat->setChecked(false);
  ui->checkBoxSurface->setChecked(false);
  ui->checkBoxLaptop->setChecked(false);
  if (aMode == VREnums::VIEW_AS_THREAT )
  {
    ui->checkBoxThreat->setChecked(true);
  }
  if (aMode == VREnums::VIEW_AS_SURFACE )
  {
    ui->checkBoxSurface->setChecked(true);
  }
  if (aMode == VREnums::VIEW_AS_LAPTOP )
  {
    ui->checkBoxLaptop->setChecked(true);
  }
}

//------------------------------------------------------------------------------
VREnums::VRViewModeEnum MainWindow::getCurrentViewMode()
{
  VREnums::VRViewModeEnum aMode = VREnums::VIEW_AS_SURFACE;
  if (ui->checkBoxSurface->isChecked())
  {
    aMode = VREnums::VIEW_AS_SURFACE;
  }
  if (ui->checkBoxThreat->isChecked())
  {
    aMode = VREnums::VIEW_AS_THREAT;
  }
  if (ui->checkBoxLaptop->isChecked())
  {
    aMode = VREnums::VIEW_AS_LAPTOP;
  }
  return aMode;
}


//------------------------------------------------------------------------------
void MainWindow::setupModeRadio()
{
  this->setRadioselectionStatus(VREnums::VIEW_AS_THREAT);

  this->connect(ui->checkBoxThreat, &QCheckBox::clicked,
                this, &MainWindow::onViewModeThreat);

  this->connect(ui->checkBoxSurface, &QCheckBox::clicked,
                this, &MainWindow::onViewModeSurface);

  this->connect(ui->checkBoxLaptop, &QCheckBox::clicked,
                this, &MainWindow::onViewModeLaptop);
}

//------------------------------------------------------------------------------
void MainWindow::setupLogWindiow()
{
  QIcon logIcon(":images/LogIcon.png");
  QIcon clearIcon(":images/EraserIcon.png");

  m_logWindow = new LogWindow(logIcon,
                              clearIcon,
                              &m_canClose,
                              nullptr);
  std::string homeFolder;
  analogic::workstation::getHomeFolder(homeFolder);
  QString savePath = homeFolder.c_str();
  savePath += "/";
  savePath += g_cycleLoadLogPath;
  savePath += g_defaultConsoleSave;
  m_logWindow->enableSave(savePath);
  m_logWindow->setStyleSheet(g_defaultWindowStyleSheet);
  m_logWindow->show();


  this->connect(ui->actionShow_Console, &QAction::triggered,
                this, &MainWindow::onShowLogWindow);

  this->connect(ui->actionHide_Console, &QAction::triggered,
                this, &MainWindow::onHideLogWindow);
}

//------------------------------------------------------------------------------
void MainWindow::readBagQueue(QString& queueFile)
{
  std::string stdFileName = queueFile.toLocal8Bit().data();
  std::ifstream inFile(stdFileName.c_str());
  if ( !inFile )
  {
    std::string msg;
    msg = "Could not open Bag Queue file '" + stdFileName + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  try
  {
    while (std::getline(inFile, str))
    {
      QString sLine = str.c_str();
      if (sLine.at(0) != '#')
      {
        // Not a comment
        // QStringList lineList = sLine;.split(BAG_QUEUE_DELIMITER);
        QString sVolume = sLine;

        std::list<QString> tdrList;
        this->getTdrListFromVolumeName(tdrList, sVolume);
        BagQueueItem bi;
        bi.setValues(sVolume, tdrList);
        m_bagQueue.push_back(bi);
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }
}


//------------------------------------------------------------------------------
void MainWindow::getTdrListFromVolumeName(std::list<QString>& tdrList,
                                          QString volFilename)
{
  QFileInfo volFileInfo(volFilename);
  QString baseName = volFileInfo.baseName();
  QString canonicalPath  = volFileInfo.canonicalPath() + "/";
  QString tdrFileName    = canonicalPath + baseName + DEF_TDR_EXTENSION;
  tdrList.push_back(tdrFileName);
}

//------------------------------------------------------------------------------
void MainWindow::populateAndInit(DataManager* pDataManager)
{
  m_currentDataFile =  pDataManager->volumeFilePath();
  m_mainTimer = QTime::currentTime();
  SDICOS::CTModule ctModule;
  std::vector<SDICOS::TDRModule*> tdrList = pDataManager->tdrList();

  m_cropCorners.m_pMin = QVector3D(0,    0,   0);
  m_cropCorners.m_pMax = QVector3D(629, 429, 700);
  QVector3D volumeDims = QVector3D(630, 430, 700);



  m_volumeRendererTrancelucentView->ShowThreatView(false);
  m_volumeRendererOpaqueView->ShowViewLabel(false);


  this->populateDICOSCtObjct(pDataManager,
                             ctModule);


  m_bagDataBufferLeft = pDataManager->dataPoionter();

  int numSlices = pDataManager->sliceCount();
  QVector3D roiStart   = m_cropCorners.m_pMin;
  QVector3D roiEnd     = m_cropCorners.m_pMax;

  volumeDims = QVector3D(m_currentImageWidth,
                         m_currentImageHeight,
                         numSlices);

  // LEFT VIEWER
  m_volumeRendererTrancelucentView->enableVolumeCropping(false);

  std::vector<std::pair<int, std::string>> dummPairs =
  {{1, "Nada"}, {2, "nada"}};


 m_volumeRendererTrancelucentView->Init(&ctModule,
                                         m_bagDataBufferLeft,
                                         volumeDims,
                                         roiStart,
                                         roiEnd,
                                         tdrList,
                                         dummPairs,
                                         m_rendererConfigLeft);

  m_volumeRendererTrancelucentView->SetLightingParameters(m_translucentAmbientLighting,
                                                          m_translucentDiffuseLighting,
                                                          m_translucentSpecularLighting);



  //
  //  ======================= ARO-FIX HERE ==========================
  // RIGHT VIEWER

  m_bagDataBufferRight = pDataManager->dataPoionter();

  m_volumeRendererOpaqueView->SetLightingParameters(m_surfaceAmbientLighting,
                                                   m_surfaceDiffuseLighting,
                                                   m_surfaceSpecularLighting);

  m_volumeRendererOpaqueView->enableVolumeCropping(false);

  m_volumeRendererOpaqueView->SetAsThreatRenderer(false);



  m_volumeRendererOpaqueView->Init(&ctModule,
                                          m_bagDataBufferRight,
                                          volumeDims,
                                          roiStart,
                                          roiEnd,
                                          tdrList,
                                          dummPairs,
                                          m_rendererConfigRight);

/*

  if (m_volumeRendererTrancelucentView->ThreatCount() == 0 )
  {
   m_volumeRendererOpaqueView->SetLightingParameters(m_surfaceAmbientLighting,
                                                    m_surfaceDiffuseLighting,
                                                    m_surfaceSpecularLighting);
   m_volumeRendererOpaqueView->SetAsThreatRenderer(false);
  }
  else
  {
    m_volumeRendererOpaqueView->initAsThreatrenderer();
   // m_volumeRendererOpaqueView->SetLightingParameters(m_translucentAmbientLighting,
   //                                                  m_translucentDiffuseLighting,
   //                                                  m_translucentSpecularLighting);
   // m_volumeRendererOpaqueView->setAsThreatRenderer(true);
  }

*/
}





//------------------------------------------------------------------------------
void MainWindow::setButtonImages()
{
  int defaultButtonWidth = 80;

  // UI BUTTON Componments

  //--------------------------------------
  // LEFT VERTICAL BUTTONS
  // Zoom In, Zoom Out, Threat Only, Cycle Threat, Reset Orientation


  this->setButtonImageAndText(ui->buttonZoomIn,
                              ":images/ZoomInIcon.png",
                              "Zoom In",
                              defaultButtonWidth);

  this->setButtonImageAndText(ui->buttonZoomOut,
                              ":images/ZoomOutIcon.png",
                              "Zoom Out",
                              defaultButtonWidth);


  this->setButtonImageAndText(ui->buttonThreatOnly,
                              ":images/ThreatOnlyIcon.png",
                              "Threat Only",
                              defaultButtonWidth);

  this->setButtonImageAndText(ui->buttonCycleThreat,
                              ":images/CycleThreatIcon.png",
                              "Cycle Threat",
                              defaultButtonWidth);

  this->setButtonImageAndText(ui->buttonResetGeometry,
                              ":images/ResetIcon.png",
                              "Reset Geometry",
                              defaultButtonWidth);
  //--------------------------------------

  //--------------------------------------
  // LOEWER HORIZONTAL BUTTONS
  // Clear Threat, Clear Bag, Suspect bag,

  this->setButtonImageAndText(ui->buttonClearThreat,
                              ":images/ClearThreatIcon.png",
                              "Clear Threat",
                              defaultButtonWidth);

  this->setButtonImageAndText(ui->buttonClearBag,
                              ":images/ClearBagIcon.png",
                              "Clear Bag",
                              defaultButtonWidth);

  this->setButtonImageAndText(ui->buttonSuspectBag,
                              ":images/SuspectBagIcon.png",
                              "Suspect Bag",
                              defaultButtonWidth);

  //-------------------------------------
  // Some buttons are TOGGLEABLE
  ui->buttonThreatOnly->setCheckable(true);
  ui->buttonThreatOnly->setChecked(false);
  ui->buttonThreatOnly->setStyleSheet(g_styleCheckButton);


  //-------------------------------------
}

//------------------------------------------------------------------------------
void MainWindow::showMenuStatus(QAction* menuItem,
                                     bool bStatus)
{
  if (menuItem == ui->actionLinkUnlinkViewers)
  {
    if (bStatus)
    {
      menuItem->setText("Unlink VTK Viewers");
      menuItem->setIcon(QIcon(":images/UnlinkIcon.png"));
    }
    else
    {
      menuItem->setText("Link VTK Viewers");
      menuItem->setIcon(QIcon(":images/LinkIcon.png"));
    }
  }
}



//------------------------------------------------------------------------------
void MainWindow::initUIComponents()
{
  this->resetCropwidget();



  this->setButtonImages();

  this->showMenuStatus(ui->actionLinkUnlinkViewers,
                       m_syncViewers);

  this->connect(m_volumeRendererTrancelucentView, &analogic::workstation::VolumeRendererWidget::initComplete,
                this, &MainWindow::onLoadComplete);

  this->connect(m_volumeRendererOpaqueView, &analogic::workstation::VolumeRendererWidget::initComplete,
             this, &MainWindow::onLoadComplete);

  //--------------------
  // Some Timing and Debugging messages - can be removed later.

  // Generic messages
  this->connect(m_volumeRendererTrancelucentView,
                &analogic::workstation::VolumeRendererWidget::genericVRWidgetMessage,
                this, &MainWindow::onVRWidgetGenericMessage);

  this->connect(m_volumeRendererOpaqueView,
                &analogic::workstation::VolumeRendererWidget::genericVRWidgetMessage,
                this, &MainWindow::onVRWidgetGenericMessage);


   // Position messages
  this->connect(m_volumeRendererTrancelucentView,
                &analogic::workstation::VolumeRendererWidget::positionVRWidgetMessage,
                this, &MainWindow::onVRWidgetPositionMessage);

  this->connect(m_volumeRendererOpaqueView,
                &analogic::workstation::VolumeRendererWidget::positionVRWidgetMessage,
                this, &MainWindow::onVRWidgetPositionMessage);


  // Timing messages
  this->connect(m_volumeRendererTrancelucentView,
                &analogic::workstation::VolumeRendererWidget::timerVRWidgetMessage,
                this, &MainWindow::onVRWidgetTimerMessageLeft);

  this->connect(m_volumeRendererOpaqueView,
                &analogic::workstation::VolumeRendererWidget::timerVRWidgetMessage,
                this, &MainWindow::onVRWidgetTimerMessageRight);

  //-------------------
  // Rubber Band Selection
  this->connect(m_volumeRendererTrancelucentView,
                 &analogic::workstation::VolumeRendererWidget::rubberBandPonitsSeleted,
                 this,
                 &MainWindow::onRubberBandPointsSelected);

  this->connect(m_volumeRendererOpaqueView,
                 &analogic::workstation::VolumeRendererWidget::rubberBandPonitsSeleted,
                 this,
                 &MainWindow::onRubberBandPointsSelected);
  //----------------------

  //----------------------

  this->connect(ui->buttonResetGeometry, &QPushButton::clicked,
                this, &MainWindow::onResetView);

  this->connect(ui->buttonThreatOnly, &QPushButton::clicked,
                this, &MainWindow::onThreatOnly);

  this->connect(ui->buttonZoomIn, &QPushButton::clicked,
                this, &MainWindow::onZoomIn);

  this->connect(ui->buttonZoomOut, &QPushButton::clicked,
                this, &MainWindow::onZoomOut);

  this->connect(ui->actionLinkUnlinkViewers, &QAction::triggered,
                this, &MainWindow::onLinkUnlinkviewers);

  this->connect(m_loadBagButton, &QPushButton::clicked,
                this, &MainWindow::onLoadNewFile);

  this->connect(ui->buttonClearBag, &QPushButton::clicked,
                this, &MainWindow::onNextBag);

  this->connect(m_cycleLoadButton, &QPushButton::clicked,
                 this, &MainWindow::onCycleBagLoad);


  this->connect(m_reloadButton, &QPushButton::clicked,
                this, &MainWindow::onReloadBagQuestion);


  //----------------------
  // Reset Renderer Settings
  m_volumeRendererTrancelucentView->ResetParameters();
  m_volumeRendererOpaqueView->ResetParameters();

  //-------------------------
  // Reset GUI compontnts.
  this->resetCropwidget();
  m_volumeRendererTrancelucentView->ResetDisplay(false);
  m_volumeRendererOpaqueView->ResetDisplay(false);


  //--------------------------------------------
  // Operator generated Threats:
  this->connect(ui->buttonOperatorThreatRubberBand, &QPushButton::clicked,
                this, &MainWindow::onOperatorThreatSelectrubberband);

  this->connect(ui->buttonOpperatorAccept, &QPushButton::clicked,
                this, &MainWindow::onOperatorThreatSelectComplete);

  this->connect(ui->operatorThreatCancel, &QPushButton::clicked,
                this, &MainWindow::onOperatorThreatCancel);


  this->connect(ui->pickMinusButton, &QPushButton::clicked,
                this, &MainWindow::onReducePickSize);

  this->connect(ui->pickPlusButton, &QPushButton::clicked,
                this, &MainWindow::onIncreasePickSize);

  this->connect(ui->deleteOperatorThreat, &QPushButton::clicked,
                this, &MainWindow::onDeleteOperatorThreat);

  //-------------------------------------------
  // Renderer scene properties



  this->connect(ui->allMaterialsButton, &QPushButton::clicked,
                this, &MainWindow::onShowAllMaterisls);


  this->connect(ui->showMetalButton, &QPushButton::clicked,
                this,  &MainWindow::onShowMetalOnly);


  this->connect(ui->showOrganicButton, &QPushButton::clicked,
                this,  &MainWindow::onShowOrganicOnly);


  this->connect(ui->showGrayscaleButton, &QPushButton::clicked,
                this, &MainWindow::onRemoveColor);

  this->connect(ui->showInverseButton, &QPushButton::clicked,
                this, &MainWindow::onInvertScene);

  this->connect(ui->cameraButton, &QPushButton::clicked,
                this, &MainWindow::onSetRightViewPhotographic);

  //---------------------------------------------
  // Clear materials.
  this->connect(m_clearConsole, &QPushButton::clicked,
                this, &MainWindow::onClearLog);


  // Crop Slider
  this->connect(ui->cropSlider, &QSlider::valueChanged,
                this, &MainWindow::onCropSlidervalueChanged);

  this->connect(ui->checkSlabX, &QCheckBox::clicked,
                 this, &MainWindow::onCropRadoXChecked);

  this->connect(ui->checkSlabY, &QCheckBox::clicked,
                 this, &MainWindow::onCropRadoYChecked);

  this->connect(ui->checkSlabZ, &QCheckBox::clicked,
                 this, &MainWindow::onCropRadoZChecked);
}

//------------------------------------------------------------------------------
bool MainWindow::initSettingsPaths()
{
  std::string homeFolder;
  bool bRespHome = analogic::workstation::getHomeFolder(homeFolder);

  std::string devRoot;
  bool bRespDdev = analogic::workstation::getDevRootFolder(devRoot);

  if ((!bRespHome) ||
      (!bRespDdev))
  {
    return false;
  }

  std::string settingsDir = homeFolder+ "/" + devRoot;

  settingsDir += "/workstation/resources/volume_renderer/";

  m_mapFileDir = settingsDir;

  // Translucent view
  m_normalOpacityFileList      =  { "NormalOpacity.opmap",
                                    "Threat_A_NormalOpacity.opmap",
                                    "Threat_B_NormalOpacity.opmap",
                                  };

  m_normalHSVColorMapFileList  =  {"NormalHSVColor.colmap",
                                   "Threat_A_NormalHSVColor.colmap",
                                   "Threat_B_NormalHSVColor.colmap",
                                  };

  m_surfaceOpacityFileList     =  {"SurfaceOpacity.opmap" ,
                                   "Threat_A_SurfaceOpacity.opmap",
                                   "Threat_B_SurfaceOpacity.opmap",
                                   };

  m_surfaceHSVColorMapFileList =  {"SurfaceHSVColor.colmap",
                                  "Threat_A_SurfaceHSVColor.colmap",
                                  "Threat_B_SurfaceHSVColor.colmap",
                                  };


  m_grayscaleValuesAndOpacitiesFelePath = settingsDir + "GrayscaleValuesAndOpacities.txt";
  m_materialTransitionFilePath          = settingsDir + "MaterialTransition.txt";
  m_internalSettingsFilePath            = settingsDir + "InternalVolumeSettings.txt";

  //---------------------------
  // Internal data directory
  m_internalDataDirectory = homeFolder.c_str();
  m_internalDataDirectory += "/bagdir/";

  m_internalTDRDirectory  = homeFolder.c_str();
  m_internalTDRDirectory  += "/bagdir/";
  return true;
}

//------------------------------------------------------------------------------
void MainWindow::populateDICOSCtObjct(DataManager* pDataManager,
                                      SDICOS::CTModule& ctModule)
{
    QString currentVolumeFile = pDataManager->volumeFilePath();

    QFile volFile(currentVolumeFile);
    if (!volFile.exists())
    {
      QString errorMessage = currentVolumeFile;
      errorMessage += " \nDoes not exist!";
      QMessageBox::warning(this,
                           "Application Error.",
                           errorMessage);
      QApplication::quit();
    }


    QString ctModuleFile;
    QString tdrModuleFile;
    QString propFile;
    bool bFoundRelatedFiles = this->getRelatedFilesFromVolumeFile(currentVolumeFile,
                                                                  tdrModuleFile,
                                                                  ctModuleFile,
                                                                  propFile);
    if (!bFoundRelatedFiles)
    {
       QMessageBox::information(this,
                                "Error on Find Related files",
                                "Closing Application");
        QApplication::quit();
    }
    SDICOS::ErrorLog ctreadLog;
    bool readResult = ctModule.Read(ctModuleFile.toStdString(),
                                   ctreadLog);
    if (!readResult)
    {
      QString quitMsg  = "CT-FILE:" + ctModuleFile;

      QMessageBox::information(this,
                               "Error Reading Ct Module.",
                                quitMsg);

      QApplication::quit();
    }




    m_cropStatus = false;

    this->setCropStatus();



     analogic::workstation::ImageProperties *iP;

     iP = new analogic::workstation::ImageProperties(analogic::workstation::SCANNER_TYPE_CHECKPOINT);

     QString propertiesFile = propFile;

     QVector3D cP0;
     QVector3D cP1;

     analogic::workstation::readVolumeProperties(propertiesFile, iP, cP0, cP1);

     cP0 = QVector3D(0  ,   0, 0);
     cP1 = QVector3D(430, 430, iP->m_imageMaxLength);


     std::cout << "CP0 (x,y,z): "
               << cP0.x() << ", "
               << cP0.y() << ", "
               << cP0.z() << std::endl;

     std::cout << "CP1 (x,y,z): "
               << cP1.x() << ", "
               << cP1.y() << ", "
               << cP1.z() << std::endl;

     pDataManager->setSliceCount(iP->m_imageMaxLength);
     m_cropCorners.setBounds(cP0, cP1);

     ctModule.GetCTImage().DeletePixelData();


     ctModule.GetCTImage().SetImageInfo(iP->m_imageHeight,
                                        iP->m_imageWidth,
                                        16,
                                        14,
                                        14,
                                        SDICOS::ImagePixelMacro::enumUnsignedInteger);


    //---------------------
    m_currentImageWidth  = iP->m_imageWidth;
    m_currentImageHeight = iP->m_imageHeight;
    m_currentNumSlices   = iP->m_imageMaxLength;
    //---------------------
    delete iP;
}


//------------------------------------------------------------------------------
bool MainWindow::getRelatedFilesFromVolumeFile(const QString volumeFile,
                             QString& dicosTdrFile,
                             QString& dicosCtFile,
                             QString& propFile)
{
  QString nameTail = ".vol";
  QString filenameCtHdr;
  filenameCtHdr  = volumeFile.left(volumeFile.length() - nameTail.length());
  dicosTdrFile = filenameCtHdr +  ".dicos_tdr_1";
  dicosCtFile  = filenameCtHdr +  ".dicos_ct_vol";
  propFile     = filenameCtHdr +  ".properties";

  QFile file1(dicosTdrFile);
  QFile file2(dicosCtFile);
  QFile file3(propFile);

  bool retVal = false;
  if ( file1.exists() &&
       file2.exists() &&
       file3.exists() )
  {
    retVal = true;
  }
  return retVal;
}



//------------------------------------------------------------------------------
void MainWindow::populateThreatCombo()
{
  if (!m_enableThreatHandling) return;
  this->setComboHeader();
  this->setAllComboData();
}


//------------------------------------------------------------------------------
void MainWindow::setComboHeader()
{
  QVariant v(-1);
  m_threatCombo->addItem("****---*****", v);
}

//------------------------------------------------------------------------------
void MainWindow::setupViewModeThreatLists()
{
  m_machineOperatorThreatVec.clear();
  m_laptopThreatVec.clear();
  std::list<ThreatObject*>::iterator itThr;
  for (itThr = m_threatObjectlLst.begin();  itThr != m_threatObjectlLst.end(); itThr++)
  {
    ThreatObject* pThr = *itThr;
    if (  ( pThr->getGenType() == analogic::workstation::ATR_GEN) &&
          ( !pThr->isLapTop())
        )
    {
      int thID = pThr->getIndex();
      m_machineOperatorThreatVec.push_back(thID);
    }
    if (pThr->isLapTop())
    {
      int thID = pThr->getIndex();
      m_laptopThreatVec.push_back(thID);
    }
  }
  //----------------------------------------------
  // Initialize the machine operator index
  if (m_machineOperatorThreatVec.size() > 0 )
  {
    m_machineOperatorIndex = 0;
  }
  else
  {
    m_machineOperatorIndex = -1;
  }

  //-----------------------------------
  // Initialize the laptop index
  if (m_laptopThreatVec.size() > 0)
  {
    m_laptopIndex = 0;
  }
  else
  {
    m_laptopIndex = -1;
  }


  if (m_machineOperatorThreatVec.size() == 0)
  {
    m_rightviewMode = VREnums::VIEW_AS_SURFACE;
  }
}

//-----------------------------------------------------------------------------
void MainWindow::setupComboList()
{
  m_threatCombo->clear();
  QStringList threats = m_volumeRendererTrancelucentView->getThreatList();
  for (int i = 0;  i < threats.size(); i++)
  {
    QVariant vi(i);
    m_threatCombo->addItem(threats.at(i), vi);
  }
}

//------------------------------------------------------------------------------
void MainWindow::setAllComboData()
{
  this->setupComboList();
  m_threatObjectlLst = m_volumeRendererTrancelucentView->getThreatObjectList();
  this->setupViewModeThreatLists();
}

//------------------------------------------------------------------------------
void MainWindow::viewThreat(int comboIndex)
{
  m_threatCombo->setCurrentIndex(comboIndex);
  QVariant vi = m_threatCombo->currentData();
  int threatIndex = vi.toInt();
  if (threatIndex >= 0)
  {
     m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, true, 0, 0);
     m_volumeRendererOpaqueView->SetThreatVisibility(nullptr, threatIndex, true, true, 0, 0);

     /*
SetThreatVisibility(SDICOS::TDRModule *pTdr,
                         int  rawIndex,
                         bool visible,
                         bool keepOrientation,
                         int  threatTypeIndex,
                         int  threatTypeTotal)
*/
  }
}

//------------------------------------------------------------------------------
void MainWindow::updateCombo(int newIndex)
{
  int comboSize = m_threatCombo->count();
  int updateIndex = 0;
  for (int i = 0; i < comboSize; i++)
  {
    QVariant vi = m_threatCombo->itemData(i);
    if (vi.toInt() == newIndex)
    {
      updateIndex = i;
      break;
    }
  }
}

//------------------------------------------------------------------------------
void MainWindow::clearThreatCombo()
{
  m_threatCombo->clear();
}

//------------------------------------------------------------------------------
void MainWindow::resetCropwidget()
{
  ui->cropSlider->setValue(0);
  ui->checkSlabX->setChecked(false);
  ui->checkSlabY->setChecked(true);
  ui->checkSlabZ->setChecked(false);
  m_volumeRendererTrancelucentView->SetCutViewAxis(VREnums::AXIS::Y);
  m_volumeRendererOpaqueView->SetCutViewAxis(VREnums::AXIS::Y);

  m_volumeRendererTrancelucentView->SetSlabUnpackPosition(0, VREnums::AXIS::Y, false);
  m_volumeRendererOpaqueView->SetSlabUnpackPosition(0, VREnums::AXIS::Y, false);
}

//------------------------------------------------------------------------------
void MainWindow::onAppClose()
{
  QApplication::quit();
}

//------------------------------------------------------------------------------
void MainWindow::onLoadComplete()
{
  ui->buttonThreatOnly->setChecked(false);
  QObject* sndObj = this->sender();

  if (sndObj == m_volumeRendererTrancelucentView )
  {
    int msLeft = m_volumeRendererTrancelucentView->firstVolumeRenderTime();

    QString message = "LEFT-ET (ms)," +
        QString::number(msLeft) + ",";
    message += m_bagTag;
    this->consoleLogString(message);
  }
  if (sndObj == m_volumeRendererOpaqueView )
  {
    int msRight = m_volumeRendererOpaqueView->firstVolumeRenderTime();
    QString message = "RGHT-ET (ms)," +
        QString::number(msRight) + ",";
    message += m_bagTag;
    this->consoleLogString(message);
  }

  m_volumeRendererTrancelucentView->ResetDisplay(true);
  //-----------------------------------------------------------
  // ARO-NOTE:
  // Don't need to Reset Display on the other window because
  // we set sync to true -  on Load new bag complete.
  // So we ommit:
  //    m_volumeRendererOpaqueView->ResetDisplay(false);
  //-----------------------------------------------------------



  // On bag load complete set right viewer to threat mode.
  m_rightviewMode = VREnums::VIEW_AS_THREAT;
  this->setRadioselectionStatus(m_rightviewMode);


  //-----------------------------
  // Memory check
  uint64_t memCheck = m_currentImageWidth*m_currentImageHeight*m_currentNumSlices*sizeof(uint16_t);
  uint64_t memAvailable;
  bool haveMem = MemUtils::testForAvailableMemory(memCheck, memAvailable);
  // QString strMemTest = "Memory Test:[ReQuest, Avail]";
  // strMemTest += QString::number(memCheck) + ", " + QString::number(memAvailable);
  // m_console->logString(strMemTest);
  if (!haveMem)
  {
    // QMessageBox::information(this, "ERROR!!",
    //            "Out of Memory");
    LOG(INFO) << "APPLICATION OUT OF MEMORY!";
  }



  //--------------------------------------
  // If Auto Clear enabled start timer
  m_loadCount++;
  qDebug() << "ON load Complete #" << m_loadCount;

  if ( m_autoLoadEnabled && (m_loadCount == 2) )
  {
    m_bagLoadTimer->start();
  }

  if (m_loadCount == 2)
  {
    this->populateThreatCombo();
    m_loadCount = 0;

    // Finaly show the first threat.
    this->showFirstThreat();
  }
  //--------------------------------------
}




//------------------------------------------------------------------------------
void MainWindow::onVRWidgetGenericMessage(int& msgID)
{
  QString message = m_volumeRendererTrancelucentView->getGenericMessageStrig(msgID);
  m_logWindow->logString(message);
}

//------------------------------------------------------------------------------
void MainWindow::onVRWidgetPositionMessage(int& msgID,
                               QVector3D& aPoint)
{
  QString message = m_volumeRendererTrancelucentView->getGenericMessageStrig(msgID);
  message += " P(x,y,z):";
  message += QString::number(aPoint.x()) + ",";
  message += QString::number(aPoint.y()) + ",";
  message += QString::number(aPoint.z());
  m_logWindow->logString(message);
}


//------------------------------------------------------------------------------
void MainWindow::onVRWidgetTimerMessageLeft(int& msgID,
                            int& timeVal)
{
  QString dispID = m_volumeRendererTrancelucentView->displayLabel();
  QString message = dispID + ":";
  message +="Volume Renderer timer message: ";
  message += m_volumeRendererTrancelucentView->getGenericMessageStrig(msgID);
  message += " - ";
  message += QString::number(timeVal);
  m_logWindow->logString(message);
}

//------------------------------------------------------------------------------
void MainWindow::onRubberBandPointsSelected(QVector2D& p0,
                     QVector2D& p1)
{
 qDebug() << "OPERATOR SELECTED P0 and P1: " << p0 << ", " << p1;
}

//------------------------------------------------------------------------------
void MainWindow::onVRWidgetTimerMessageRight(int& msgID,
                            int& timeVal)
{
  QString dispID = m_volumeRendererOpaqueView->displayLabel();
  QString message = dispID + ":";
  message +="Volume Renderer timer message: ";
  message += m_volumeRendererOpaqueView->getGenericMessageStrig(msgID);
  message += " - ";
  message += QString::number(timeVal);
  m_logWindow->logString(message);
}



//------------------------------------------------------------------------------
void MainWindow::onResetView()
{
  //----------------------
  // Reset Renderer Settings
  m_volumeRendererTrancelucentView->ResetParameters();
  m_volumeRendererOpaqueView->ResetParameters();

  //-------------------------
  // Reset GUI compontnts.
  this->resetCropwidget();
  this->setRadioselectionStatus(m_rightviewMode);
  m_volumeRendererTrancelucentView->ResetDisplay(false);
  m_volumeRendererOpaqueView->ResetDisplay(false);
}

//-------------------------------------------------------------------------------
void MainWindow::onSetRightAsThreat()
{
  m_volumeRendererOpaqueView->SetAsThreatRenderer(true);
  this->showFirstThreat();
}

//-------------------------------------------------------------------------------
void MainWindow::onPanRightView()
{
}

//-------------------------------------------------------------------------------
void MainWindow::onResetRightRenderer()
{
  m_volumeRendererOpaqueView->revertRightViewerFormThreatSettings();
}

//-------------------------------------------------------------------------------
void MainWindow::onCenterRightViewer()
{
  m_volumeRendererOpaqueView->centerWindowOnPointAndExtent(m_operatorThreatP0,
                                                           m_operatorThreatExtent,
                                                           0);
}

//------------------------------------------------------------------------------
void MainWindow::onShowSplash()
{
  int numThreats = m_volumeRendererTrancelucentView->ThreatCount();
  if (numThreats == 0)
  {
    m_volumeRendererTrancelucentView->showVtkSplashScreenNew();
    m_volumeRendererOpaqueView->showVtkSplashScreenNew();
  }
}

//------------------------------------------------------------------------------
void MainWindow::onThreatOnly()
{
  m_volumeRendererTrancelucentView->DisplayThreatOnly(ui->buttonThreatOnly->isChecked());
  m_volumeRendererOpaqueView->DisplayThreatOnly(ui->buttonThreatOnly->isChecked());
}

//------------------------------------------------------------------------------
void MainWindow::onZoomIn()
{
  m_currentZoom *= 1.1;
  m_volumeRendererTrancelucentView->SetZoom(m_currentZoom);
  m_volumeRendererOpaqueView->SetZoom(m_currentZoom);
  g_zoomTestCount++;
}

//---------------------------------------------------------------------------------
void MainWindow::onZoomOut()
{
  m_currentZoom /= 1.1;
  m_volumeRendererTrancelucentView->SetZoom(m_currentZoom);
  m_volumeRendererOpaqueView->SetZoom(m_currentZoom);
  g_zoomTestCount--;
}


//------------------------------------------------------------------------------
void MainWindow::onClearScene()
{
 int mbRet = QMessageBox::question(this,
                                   "Remove Volume",
                                   "Do you want to remove the volume from the viewer?");
 if (mbRet == QMessageBox::Yes)
 {
  m_volumeRendererTrancelucentView->RemoveCurrentBag();
  m_volumeRendererOpaqueView->RemoveCurrentBag();
  this->clearThreatCombo();
 }
}

//------------------------------------------------------------------------------
void MainWindow::onExportFullTFs()
{
  int mbResp = QMessageBox::question(this,
                                     "Save Transfer Functions",
                                     "Save Current Opacity and HSV Transfer function to file?");

  if (mbResp != QMessageBox::Yes) return;


  //----------------------------------------------
  // ARO-DEBUGGING - Saving HSV Colormap to a file.
  // We ONLY want the translucent view.
  m_volumeRendererTrancelucentView->saveVTKTranferFunctions();
  //-----------------------------------------
}

//-----------------------------------------------------------------------------
void MainWindow::onLeakCheck()
{
  if (vtkDebugLeaks::PrintCurrentLeaks() == 1)
  {
    QString leakMessage = "There were VTK leaks found in VolumeRenderer Application";
    LOG(INFO) << leakMessage.toLocal8Bit().data();
    qDebug() << leakMessage;
  }
}

//------------------------------------------------------------------------------
void MainWindow::onExportThreatData()
{
  QFileDialog exportFileDlg(this,
                         "Select volume output file",
                         g_volExportPath,
                         g_volExportFileType);
  exportFileDlg.setAcceptMode(QFileDialog::AcceptSave);

  bool dlgRet =  exportFileDlg.exec();
  if (dlgRet)
  {
    QString saveFile = exportFileDlg.selectedFiles().first();
    m_volumeRendererTrancelucentView->exportThreatData(saveFile);
  }
}

//------------------------------------------------------------------------------
void MainWindow::onThreatWindowsPrintSelf()
{
  int msgResp = QMessageBox::question(this, "Save File Question",
                        "Save threat window info to file?");
  if (msgResp == QMessageBox::No) return;

  QString strLeft;
  QString strRight;
  m_volumeRendererTrancelucentView->threatWidowPrintSelf(strLeft);
  m_volumeRendererOpaqueView->threatWidowPrintSelf(strRight);
  QString fileLeft  = g_threatExportPath + g_leftThreatExportFile;
  QString fileRight = g_threatExportPath + g_rightThreatExportFile;

  std::string filename = fileLeft.toLocal8Bit().data();
  std::ofstream file;
  file.open(filename.c_str(),  ios::out);
  file.write(strLeft.toLocal8Bit().data(), strLeft.length());
  file.close();

  filename = fileRight.toLocal8Bit().data();
  file.open(filename.c_str(),  ios::out);
  file.write(strRight.toLocal8Bit().data(), strRight.length());
  file.close();
}

//------------------------------------------------------------------------------
void MainWindow::onShowCurrentThreatVolume()
{
  QVector3D p0;
  QVector3D p1;
  bool retVal = m_volumeRendererTrancelucentView->getCurrentThreatVolume(p0,
                                                                         p1);
  if (retVal)
  {
    QString message = "Threat Volume P0[x y z], ";
          message +=  QString::number(p0.x()) + ", ";
          message +=  QString::number(p0.y()) + ", ";
          message +=  QString::number(p0.z()) + "\n";
          message +=  "Threat Volume P1[x y z], ";
          message +=  QString::number(p1.x()) + ", ";
          message +=  QString::number(p1.y()) + ", ";
          message +=  QString::number(p1.z()) + "\n";
    m_logWindow->logString(message);
  }
  else
  {
    m_logWindow->logString("Failed obtaining Threat Volume Conders");
  }
}


//------------------------------------------------------------------------------
void MainWindow::onTestTransformCamera()
{
  static const double flip[16] =
  {
    1,  0, 0, 0,
    0, -1, 0, 0,
    0,  0, 1, 0,
    0,  0, 0, 1
  };
  vtkCamera* camera = m_volumeRendererTrancelucentView->getRenderCamera();
  vtkSmartPointer<vtkMatrix4x4> resliceMatrix = vtkMatrix4x4::New();
  resliceMatrix->DeepCopy(flip);

  camera->SetModelTransformMatrix(resliceMatrix);
  camera->Modified();
  m_volumeRendererTrancelucentView->forceUpdate();
}

//------------------------------------------------------------------------------
void MainWindow::onInsertVTKLabelText()
{
  QStringList insertList =
  {
    "Label A: this is an inserted text",
    // "Label B: this is another inserted text",
    // "Label C: The final inserted text",
  };
  m_volumeRendererTrancelucentView->AppendDisplayText(insertList);
}

//------------------------------------------------------------------------------
void MainWindow::onTestZoomValue()
{
  g_zoomTestCount = 0;
  double testZoom = m_volumeRendererTrancelucentView->estimetedZoom();
  std::cout <<  -1 << ", " << testZoom << std::endl;
}

//------------------------------------------------------------------------------
void MainWindow::onTestViewMode()
{
  ui->checkBoxLaptop->setChecked(true);
}

//------------------------------------------------------------------------------
void MainWindow::onStashCurrentCamera()
{
  m_volumeRendererOpaqueView->stashCurrentCamera();
}

//------------------------------------------------------------------------------
void MainWindow::onRestoreStashCamera()
{
  m_volumeRendererOpaqueView->restoreStashedCamera();
}

//------------------------------------------------------------------------------
void MainWindow::onResetThreatWindow()
{
  qDebug() << "Not Implemented!";
}

//------------------------------------------------------------------------------
void MainWindow::onTestUpdateRate()
{
  QInputDialog inputDlg(this);
  float fVal = m_volumeRendererTrancelucentView->getDesiredUpdateRate();
  QString fText = QString::number(fVal);
  inputDlg.setLabelText("Input New Desired Update Rate");
  inputDlg.setTextValue(fText);
  inputDlg.exec();
  fVal = inputDlg.textValue().toDouble();
  m_volumeRendererTrancelucentView->setDesiredUpdateRate(fVal);
  m_volumeRendererOpaqueView->setDesiredUpdateRate(fVal);
}


//------------------------------------------------------------------------------
void MainWindow::onTestWindowLevel()
{
  QInputDialog inputDlg(this);
  float fVal = m_windowLevelLineEdit->text().toFloat();
  QString fText = QString::number(fVal);
  inputDlg.setLabelText("Input Window Level");
  inputDlg.setTextValue(fText);
  inputDlg.exec();
  fVal = inputDlg.textValue().toDouble();
  m_volumeRendererTrancelucentView->setWindowLevel(fVal);
  m_volumeRendererOpaqueView->setWindowLevel(fVal);
  this->showWindowLevel(fVal);
}


//------------------------------------------------------------------------------
void MainWindow::onTestColorLevel()
{
  QInputDialog inputDlg(this);
  float fVal = m_colorLevelLineEdit->text().toFloat();
  QString fText = QString::number(fVal);
  inputDlg.setLabelText("Input Color Level");
  inputDlg.setTextValue(fText);
  inputDlg.exec();
  fVal = inputDlg.textValue().toDouble();
  m_volumeRendererTrancelucentView->setColorLevel(fVal);
  m_volumeRendererOpaqueView->setColorLevel(fVal);
  this->showColorLevel(fVal);
}


//------------------------------------------------------------------------------
void MainWindow::onTestImageOverlay()
{
 this->onShowSplash();
}

//------------------------------------------------------------------------------
void MainWindow::onStartLeakCheck()
{
  /*
 int bResp = QMessageBox::question(this,
                          "Srart Leak checking?",
                          "Atarting LeakTracer library monitoring for memory allocations... ");
 if (bResp != QMessageBox::Yes) return;
 leaktracer::MemoryTrace::GetInstance().startMonitoringAllThreads();
 m_leakCheckStatus = true;
 this->setLeakCheckStatus();
 */
}

//------------------------------------------------------------------------------
void MainWindow::onPauseLeakCheck()
{
  /*
  // Stop allocation Monitoring
  leaktracer::MemoryTrace::GetInstance().stopMonitoringAllocations();

  // Stop all monitoring, print report
  leaktracer::MemoryTrace::GetInstance().stopAllMonitoring();

  std::ofstream oleaks;
  std::string leakFile = g_leakFileOut.toLocal8Bit().data();

  oleaks.open(leakFile, std::ios_base::out);

  if (oleaks.is_open())
  {
    leaktracer::MemoryTrace::GetInstance().writeLeaks(oleaks);
  }
  else
  {
    qDebug() << "Failed to write leak outpue to:" << g_leakFileOut;
  }
  m_leakCheckStatus = false;
  this->setLeakCheckStatus();
  */
}

//------------------------------------------------------------------------------
void MainWindow::showFirstThreat()
{
  //-----------------------------------------
  // Select the first to show if any.
  //  First determine the current view mode
  //  A. We need the number of OPERATOR/MACHINE threats.
  //  B. We need the number of LAPTOP threats.
  if (m_threatObjectlLst.size() == 0)
  {
    m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_SURFACE);
    return;
  }
  int threatIndex = -1;
  switch (m_rightviewMode)
  {
  case VREnums::VIEW_AS_SURFACE:
    m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_SURFACE);
    if(m_machineOperatorThreatVec.size() == 0)
    {
      // We have no threats to show wo leave right viewer as surface
      return;
    }
//    m_machineOperatorIndex = 0;
//    threatIndex = m_machineOperatorThreatVec[m_machineOperatorIndex];
//    m_volumeRendererTrancelucentView->SetCurrentThreat(threatIndex);
//    threatIndex = m_volumeRendererTrancelucentView->CurrentThreat();
//    m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false);
//    m_threatCombo->setCurrentIndex(threatIndex + 1);
//    this->onThreatOnly();
    break;
  case VREnums::VIEW_AS_THREAT:
    if(m_machineOperatorThreatVec.size() == 0)
    {
      // We have no threats to show wo leave right viewer as surface
      m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_SURFACE);
      return;
    }
//    m_machineOperatorIndex = 0;
//    m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_THREAT);
//    threatIndex = m_machineOperatorThreatVec[m_machineOperatorIndex];
//    m_volumeRendererTrancelucentView->SetCurrentThreat(threatIndex);
//    threatIndex = m_volumeRendererTrancelucentView->CurrentThreat();
//    m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false);
//    m_threatCombo->setCurrentIndex(threatIndex + 1);
//    this->onThreatOnly();
    break;
  case VREnums::VIEW_AS_LAPTOP:
    if(m_laptopThreatVec.size() == 0)
    {
      // We have no threats to show wo leave right viewer as surface
      m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_SURFACE);
      return;
    }
//    m_laptopIndex = 0;
//    threatIndex = m_laptopThreatVec[m_laptopIndex];
//    m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_LAPTOP);
//    m_volumeRendererTrancelucentView->SetCurrentThreat(threatIndex);
//    m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false);
//    m_threatCombo->setCurrentIndex(threatIndex + 1);
    this->onThreatOnly();
    break;
  }
}



//------------------------------------------------------------------------------
void MainWindow::onCycleThreat()
{
  if (m_threatObjectlLst.size() == 0)
  {
    // m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_SURFACE);
    return;
  }
  int threatIndex;
  switch (m_rightviewMode)
  {
  case VREnums::VIEW_AS_SURFACE:
    if (m_machineOperatorThreatVec.size() == 0 ) return;
    m_machineOperatorIndex++;
    if (m_machineOperatorIndex > m_machineOperatorThreatVec.size() - 1)
    {
      m_machineOperatorIndex = 0;
    }
    threatIndex = m_machineOperatorThreatVec[m_machineOperatorIndex];
    m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false, 0 , 0);
    m_threatCombo->setCurrentIndex(threatIndex + 1);
    break;
  case VREnums::VIEW_AS_THREAT:
    if (m_machineOperatorThreatVec.size() == 0 ) return;
    m_machineOperatorIndex++;
    if (m_machineOperatorIndex > m_machineOperatorThreatVec.size() - 1)
    {
      m_machineOperatorIndex = 0;
    }
    // m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_THREAT);
    threatIndex = m_machineOperatorThreatVec[m_machineOperatorIndex];
    m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false, 0 , 0);
    m_threatCombo->setCurrentIndex(threatIndex + 1);
    break;
  case VREnums::VIEW_AS_LAPTOP:
    if (m_laptopThreatVec.size() == 0 ) return;
    m_laptopIndex++;
    if (m_laptopIndex > m_laptopThreatVec.size() - 1)
    {
      m_laptopIndex = 0;
    }
    // m_volumeRendererOpaqueView->SetRightViewerDisplayMode(VREnums::VIEW_AS_LAPTOP);
    threatIndex = m_laptopThreatVec[m_laptopIndex];
    m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false, 0, 0);
    m_threatCombo->setCurrentIndex(threatIndex + 1);
    break;
  }
}

//------------------------------------------------------------------------------
void MainWindow::onViewModeThreat()
{
  ui->checkBoxSurface->setChecked(false);
  ui->checkBoxLaptop->setChecked(false);
  m_rightviewMode = VREnums::VIEW_AS_THREAT;
  if (m_machineOperatorThreatVec.size() == 0)
  {
     m_rightviewMode = VREnums::VIEW_AS_SURFACE;
     m_volumeRendererOpaqueView->SetRightViewerDisplayMode(m_rightviewMode);
     return;
  }

  m_volumeRendererOpaqueView->SetRightViewerDisplayMode(m_rightviewMode);
  int threatIndex = m_machineOperatorIndex;
//  m_volumeRendererTrancelucentView->SetCurrentThreat(m_machineOperatorIndex);
//  threatIndex = m_volumeRendererTrancelucentView->CurrentThreat();
//  m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false);
  m_threatCombo->setCurrentIndex(threatIndex + 1);
}

//------------------------------------------------------------------------------
void MainWindow::onViewModeSurface()
{
  ui->checkBoxThreat->setChecked(false);
  ui->checkBoxLaptop->setChecked(false);
  m_rightviewMode = VREnums::VIEW_AS_SURFACE;
  m_volumeRendererOpaqueView->SetRightViewerDisplayMode(m_rightviewMode);
  if (m_machineOperatorThreatVec.size() == 0)
  {
     return;
  }
  int threatIndex = m_machineOperatorIndex;
//  m_volumeRendererTrancelucentView->SetCurrentThreat(m_machineOperatorIndex);
//  threatIndex = m_volumeRendererTrancelucentView->CurrentThreat();
//  m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false);
  m_threatCombo->setCurrentIndex(threatIndex + 1);
}

//------------------------------------------------------------------------------
void MainWindow::onViewModeLaptop()
{
  ui->checkBoxThreat->setChecked(false);
  ui->checkBoxSurface->setChecked(false);
  m_rightviewMode = VREnums::VIEW_AS_LAPTOP;
  if (m_laptopThreatVec.size() == 0)
  {
     m_rightviewMode = VREnums::VIEW_AS_SURFACE;
     m_volumeRendererOpaqueView->SetRightViewerDisplayMode(m_rightviewMode);
     return;
  }
  int threatIndex = m_laptopIndex;
//  m_volumeRendererOpaqueView->SetCurrentThreat(m_laptopIndex);
  m_volumeRendererOpaqueView->SetRightViewerDisplayMode(m_rightviewMode);
  m_threatCombo->setCurrentIndex(threatIndex + 1);
}


//------------------------------------------------------------------------------
void MainWindow::onNextBag()
{
  m_bagQueueIndex++;
  if (m_bagQueueIndex > (m_bagQueue.size() - 1) )
  {
    m_bagQueueIndex = 0;
  }
  QString  volDir     = m_internalDataDirectory;
  QString  tdrDir     = m_internalTDRDirectory;
  QString  newBagFile;

  BagQueueItem bi = m_bagQueue.at(m_bagQueueIndex);
  newBagFile = volDir;
  newBagFile += bi.m_volumeFileName;

  std::list<QString> tdrFiles = bi.m_tdrList;
  std::list<QString>::iterator it;
  for (it =  tdrFiles.begin(); it != tdrFiles.end(); it++)
  {
    QString fullTDRpath = tdrDir + *it;
    *it = fullTDRpath;
  }
  QString tmpTdr = bi.m_tdrList.front();
  QFileInfo fInfo(tmpTdr);

  m_currentTDRFile = fInfo.fileName();
  this->setTDRLabel(m_currentTDRFile);
  this->loadNewBag(newBagFile, tdrFiles);
}

//------------------------------------------------------------------------------
void MainWindow::onBagLoadTimerUpdate()
{
  this->onNextBag();
  this->cycleLoadWriteData();
}

//------------------------------------------------------------------------------
void MainWindow::onCycleBagLoad()
{
   if (!m_autoLoadEnabled)
   {
    m_autoLoadEnabled = true;
    this->cycleLoadWriteHeader();
    this->onNextBag();
  }
  else
  {
    m_autoLoadEnabled = false;
    m_bagLoadTimer->stop();
  }
}


//------------------------------------------------------------------------------
void MainWindow::onOperatorThreatSelectrubberband()
{
  int machineThreatsremaining = m_volumeRendererTrancelucentView->unclearedMachineThreats();

  if (machineThreatsremaining > 0) return;

  m_volumeRendererTrancelucentView->StartOperatorThreatGeneration();
  m_volumeRendererOpaqueView->armRubberBand(true);
}


//------------------------------------------------------------------------------
void MainWindow::onOperatorThreatSelectComplete()
{
   QStringList selectList = {"ANOMALY",
                             "EXPLOSIVE",
                             "LIQUID",
                             "WEAPON",
                             "CONTRABAND"};

    SelectFromListDialog slectFromList(selectList, this);
    slectFromList.exec();
    int retval = slectFromList.getSelected();

    QString threatText;
    if (retval >= 0)
    {
      threatText = selectList.at(retval);
    }
    else
    {
      threatText = selectList.at(0);
    }


    int newID;
    if (g_doRightOperatorThreats)
    {
      newID = m_volumeRendererTrancelucentView->RightAcceptOperatorGeneratedThreat(threatText,
                                                                   m_operatorThreatP0,
                                                                   m_operatorThreatExtent);
    }
    else
    {
       newID = m_volumeRendererTrancelucentView->AcceptOperatorGeneratedThreat(threatText,
                                                                    m_operatorThreatP0,
                                                                    m_operatorThreatP0);
    }
    QString sNewThreat = "OPERATOR Gen-" + QString::number(newID);
    QVariant vId(newID);


    int currentComboSize  = m_threatCombo->count();
    if (currentComboSize == 0)
    {
      this->setComboHeader();
    }

    m_threatCombo->addItem(sNewThreat, vId);
    this->updateCombo(newID);
    m_operatorThreatSelectionArmed = false;
}

//------------------------------------------------------------------------------
void MainWindow::onOperatorThreatCancel()
{
  std::cout << "Cancel Operator Threat Generation" << std::endl;
  m_volumeRendererTrancelucentView->CancelOperatorThreatGeneration();
}



//------------------------------------------------------------------------------
void MainWindow::onReducePickSize()
{
//  if ( !this->isArmedOperatorThreat() )
//  {
//    return;
//  }
//  m_volumeRendererTrancelucentView->ShrinkOperatorThreatVolume();
}

//------------------------------------------------------------------------------
void MainWindow::onIncreasePickSize()
{
// if ( !this->isArmedOperatorThreat() )
// {
//   return;
// }
// m_volumeRendererTrancelucentView->ExpandOperatorThreatVolume();
}

//------------------------------------------------------------------------------
void MainWindow::onDeleteOperatorThreat()
{
  int currentIndex = m_volumeRendererTrancelucentView->CurrentThreat();
  ThreatObject* thObj = m_volumeRendererTrancelucentView->getCurrentThreatObject();
  if (thObj->getGenType() == analogic::workstation::OPERATOR_GEN)
  {
    m_volumeRendererTrancelucentView->DeleteOperatorThreat(currentIndex);
  }
}



//------------------------------------------------------------------------------
void MainWindow::onShowAllMaterisls()
{
  std::vector<analogic::workstation::MaterialEnumeration> matList;
  matList.push_back(analogic::workstation::MaterialEnumeration::ORGANIC);
  matList.push_back(analogic::workstation::MaterialEnumeration::INORGANIC);
  matList.push_back(analogic::workstation::MaterialEnumeration::METAL);
  // matList.push_back(analogic::workstation::MaterialEnumeration::THREAT);   // ??
  m_volumeRendererTrancelucentView->SetHighlightedMaterials(matList);
  m_volumeRendererOpaqueView->SetHighlightedMaterials(matList);
}

//------------------------------------------------------------------------------
void MainWindow::onShowMetalOnly()
{
  std::vector<analogic::workstation::MaterialEnumeration> matList;
  matList.push_back(analogic::workstation::MaterialEnumeration::METAL);
  m_volumeRendererTrancelucentView->SetHighlightedMaterials(matList);
  m_volumeRendererOpaqueView->SetHighlightedMaterials(matList);
}

//------------------------------------------------------------------------------
void MainWindow::onShowOrganicOnly()
{
  std::vector<analogic::workstation::MaterialEnumeration> matList;
  matList.push_back(analogic::workstation::MaterialEnumeration::ORGANIC);
  m_volumeRendererTrancelucentView->SetHighlightedMaterials(matList);
  m_volumeRendererOpaqueView->SetHighlightedMaterials(matList);
}

//------------------------------------------------------------------------------
void MainWindow::onRemoveColor()
{
  m_doColorRender = !m_doColorRender;
  m_volumeRendererTrancelucentView->RenderWithColor(m_doColorRender);
  m_volumeRendererOpaqueView->RenderWithColor(m_doColorRender);
}

//------------------------------------------------------------------------------
void MainWindow::onInvertScene()
{
  m_doInversecolor = !m_doInversecolor;
  m_volumeRendererTrancelucentView->SetInverse(m_doInversecolor);
  m_volumeRendererOpaqueView->SetInverse(m_doInversecolor);
}

//------------------------------------------------------------------------------
void MainWindow::onSetRightViewPhotographic()
{
  if (m_currentOpacity != DEFAULT_VALUE_OPACITY)
  {
    m_currentOpacity = DEFAULT_VALUE_OPACITY;
  }
  else
  {
    m_currentOpacity = PHOTOGRAPIC_OPACITY;
  }
  m_volumeRendererOpaqueView->SetOpacity(m_currentOpacity);
}

//------------------------------------------------------------------------------
void MainWindow::cycleLoadWriteHeader()
{
  QDateTime dt = QDateTime::currentDateTime();
  QString message = "START CYCLE TEST:" + dt.toString() + "\n";
  this->appenStringToFile(m_cycleLoadLogFile, message);
  message = "Bag,Used (MB),Free (MB),Threat Size (MB)\n";
  this->appenStringToFile(m_cycleLoadLogFile, message);
}

//------------------------------------------------------------------------------
void MainWindow::cycleLoadWriteData()
{
  uint64_t memTotal;
  uint64_t memFree;

  MemUtils::getTotalAndFreeMemoryBytes(memTotal, memFree);
  memTotal /= (1024*1024);
  memFree  /= (1024*1024);
  int bagCount  = m_volumeRendererTrancelucentView->getDbgBagCount();
  double threatSize = m_volumeRendererTrancelucentView->getLastThreatVolumeSize();
  QString message = QString::number(bagCount)                + ", " +
                    QString::number(memTotal - memFree)      + ", " +
                    QString::number(memFree)                 + ", " +
                    QString::number(threatSize)              + "\n";
  this->appenStringToFile(m_cycleLoadLogFile, message);
}

//------------------------------------------------------------------------------
void MainWindow::appenStringToFile(QString& filename,
                                   QString& str)
{
  std::ofstream ofs;
  ofs.open (filename.toLocal8Bit().data(), std::ofstream::out | std::ofstream::app);
  ofs << str.toLocal8Bit().data();
  ofs.close();
}

//------------------------------------------------------------------------------
void MainWindow::loadNewBag(QString& pathToBagData,
                            std::list<QString>& tdrList)
{
  m_currentDataFile = pathToBagData;

  // OK delete and load
  m_dataManager->deleteData();
  m_dataManager->resetPaths(m_currentDataFile, tdrList);
  m_dataManager->readTDRModules(m_sdicosLogFile);
  m_dataManager->loadVolumeData();



  this->setWindowTileWithDataFile(m_currentDataFile);

  m_volumeRendererTrancelucentView->RemoveCurrentBag();
  m_volumeRendererTrancelucentView->ResetDisplay(false);

  m_volumeRendererOpaqueView->RemoveCurrentBag();
  m_volumeRendererOpaqueView->ResetDisplay(false);


  m_volumeRendererOpaqueView->clearAllThreats();
  m_volumeRendererTrancelucentView->clearAllThreats();

  this->linkVtkViewers(m_syncViewers);


  //--------------------------------------------

  this->clearThreatCombo();
  this->resetCropwidget();


  this->populateAndInit(m_dataManager);


  this->doAppendToTextLabel();

  this->showBagQueueInfo();
}


//------------------------------------------------------------------------------
void MainWindow::doAppendToTextLabel()
{
  QStringList listStr = {"Search Reason:  Threat Suspected",
                          "Threat Assesment: High",
                         };
  m_volumeRendererTrancelucentView->AppendDisplayText(listStr);
}

//------------------------------------------------------------------------------
void MainWindow::consoleLogString(const QString& logStr)
{
  m_logWindow->logString(logStr);
}

//------------------------------------------------------------------------------
void MainWindow::onNewBag()
{
  int mbRet = QMessageBox::question(this,
                                    "Next Bag",
                                    "Do you want to load another bag?");

  if (mbRet == QMessageBox::Yes)
  {
    this->onLoadNewFile();
  }
}

//------------------------------------------------------------------------------
void MainWindow::onShowCameraInfo()
{
  vtkCamera* mainCamera   = m_volumeRendererTrancelucentView->getMainCamera();
  vtkCamera* threatCamera = m_volumeRendererTrancelucentView->getThreatCamera();

  std::stringstream ss;
  mainCamera->PrintSelf(ss, vtkIndent(2));
  std::string camProps = ss.str();
  this->consoleLogString("--------------- Main Camera ------------------");
  this->consoleLogString(camProps.c_str());

  if (threatCamera)
  {
    std::stringstream ssT;
    threatCamera->PrintSelf(ssT, vtkIndent(2));
    std::string camPropsT = ssT.str();
    this->consoleLogString("-------------- Threat Camera ----------------");
    this->consoleLogString(camPropsT.c_str());
  }
}

//------------------------------------------------------------------------------
void MainWindow::onShowVolumeBounds()
{
  vtkCamera* mainCamera   = m_volumeRendererTrancelucentView->getMainCamera();
  vtkCamera* threatCamera = m_volumeRendererTrancelucentView->getThreatCamera();



  // double planesArray[24];
  // mainCamera->GetFrustumPlanes(1,planesArray);
  QString message;

  double* fp = mainCamera->GetFocalPoint();
  message = "Main camera Focal Point  :";
  message += QString::number(fp[0]) +  ", " +
             QString::number(fp[1]) +  ", " +
             QString::number(fp[2]);
  this->consoleLogString(message);

  fp = threatCamera->GetFocalPoint();
  message = "Threat camera Focal Point:";
  message += QString::number(fp[0]) +  ", " +
             QString::number(fp[1]) +  ", " +
             QString::number(fp[2]);
  this->consoleLogString(message);

  QString shortName;
  QFileInfo fInfo(m_currentDataFile);
  shortName = fInfo.fileName();
  message = "Input Files (vol,tdr):";

  message += shortName;
  message += " - ";
  message += m_currentTDRFile;
  this->consoleLogString(message);
}

//------------------------------------------------------------------------------
void MainWindow::onReloadBag()
{
  QString tdrFile = m_internalTDRDirectory;
  tdrFile += m_currentTDRFile;

  std::list<QString> tdrList;
  tdrList.push_back(tdrFile);
  this->loadNewBag(m_currentDataFile, tdrList);
}

//------------------------------------------------------------------------------
void MainWindow::onReloadBagQuestion()
{
  int queryResp = QMessageBox::question(this,
                                        "Volume Renderer Application",
                                        "Do you want to reload the current volume");

  if (queryResp != QMessageBox::Yes) return;

  QString tdrFile = m_internalTDRDirectory;
  tdrFile += m_currentTDRFile;

  std::list<QString> tdrList;
  tdrList.push_back(tdrFile);
  this->loadNewBag(m_currentDataFile, tdrList);
}


//------------------------------------------------------------------------------
void MainWindow::onEditCropROI()
{
  QString filePath = m_internalDataDirectory;
  filePath +=   m_cropFile;

  QFile cropFile(filePath);
  bool cropFileExists = cropFile.exists();

  // See if the file exists. If it doens't ask the user if they want to create one.
  if (!cropFileExists)
  {
        int mbResp = QMessageBox::question(this,
                             "Edit Crop File...",
                             "Crop file for this data doesn't exist.\nDo you eant to create one?");
        if (mbResp == QMessageBox::No) return;
        analogic::workstation::writeCropCorners(filePath,
                                                m_cropCorners);
  }

  analogic::workstation::readCropCorners(filePath,
                                         m_cropCorners);
  CropDialog cropDlg(this);
  cropDlg.setCropCorners(m_cropCorners.m_pMin,
                         m_cropCorners.m_pMax);
  int dlgResp = cropDlg.exec();
  if (dlgResp == QDialog::Accepted)
  {
    cropDlg.getCropCorners(m_cropCorners.m_pMin,
                           m_cropCorners.m_pMax);
    analogic::workstation::writeCropCorners(filePath,
                                            m_cropCorners);
  }
}

//------------------------------------------------------------------------------
void MainWindow::onThreatComboSlectionChanged(int curSel)
{
  QString message = "Threat Combo sel change - onThreatComboSlectionChanged(curSel):  curSel= " +
          QString::number(curSel);

  m_logWindow->logString(message);

  QVariant vi = m_threatCombo->currentData();
  int threatIndex = vi.toInt();
  if (threatIndex >= 0)
  {
    m_volumeRendererTrancelucentView->SetThreatVisibility(nullptr, threatIndex, true, false, 0, 0);
    m_volumeRendererOpaqueView->SetThreatVisibility(nullptr, threatIndex, true, false, 0, 0 );
  }
}

//------------------------------------------------------------------------------
void MainWindow::onLinkUnlinkviewers()
{
  m_syncViewers = !m_syncViewers;
  this->linkVtkViewers(m_syncViewers);
  this->showMenuStatus(ui->actionLinkUnlinkViewers,
                           m_syncViewers);
}




//------------------------------------------------------------------------------
void MainWindow::onClearThreat()
{
  int currenThreat = m_volumeRendererTrancelucentView->CurrentThreat();
  if (currenThreat >= 0)
  {
    //-----------------
    // Underlying object
    m_volumeRendererTrancelucentView->ClearCurrentThreat();
    m_volumeRendererOpaqueView->ClearCurrentThreat();
    //------------------
    m_volumeRendererTrancelucentView->forceUpdate();
    m_volumeRendererOpaqueView->forceUpdate();


    //--------------------------
    // UPDATE QT GUI
    for (int i=0; i < m_threatCombo->count();  i++)
    {
      QVariant v = m_threatCombo->itemData(i);
      if (v.toInt() == currenThreat)
      {
        m_threatCombo->removeItem(i);
        break;
      }
    }
    //--------------------------
  }
}

//-----------------------------------------------------------------------------
void MainWindow::onSuspectThreat()
{
  m_volumeRendererTrancelucentView->SuspectCurrentThreat(0, 0);
  m_volumeRendererOpaqueView->SuspectCurrentThreat(0, 0);
}


//------------------------------------------------------------------------------
void MainWindow::onApplyVolumePropertyLeft()
{
//  double ambientVal  = ui->ambientSpinBox->value();
//  double diffuseVal  = ui->diffuseSpinBox->value();
//  double specularVal = ui->specularSpinBox->value();
//  m_volumeRendererTrancelucentView->SetLightingParameters(ambientVal,
//                                                          diffuseVal,
//                                                          specularVal);
//  m_volumeRendererTrancelucentView->forceUpdate();
}


//------------------------------------------------------------------------------
void MainWindow::onApplyVolumePropertyRight()
{
//  double ambientVal  = ui->ambientSpinBox->value();
//  double diffuseVal  = ui->diffuseSpinBox->value();
//  double specularVal = ui->specularSpinBox->value();
//  m_volumeRendererOpaqueView->SetLightingParameters(ambientVal,
//                                                          diffuseVal,
//                                                          specularVal);
//  m_volumeRendererOpaqueView->forceUpdate();
}


//------------------------------------------------------------------------------
void MainWindow::onShowLogWindow()
{
  m_logWindow->show();
}


//------------------------------------------------------------------------------
void MainWindow::onHideLogWindow()
{
  m_logWindow->hide();
}



//------------------------------------------------------------------------------
void MainWindow::linkVtkViewers(bool doLink)
{
  m_volumeRendererOpaqueView->SetSyncEnabled(doLink);
  m_volumeRendererTrancelucentView->SetSyncEnabled(doLink);
}

//------------------------------------------------------------------------------
void MainWindow::onLoadNewFile()
{
  QString pathToBagdata = "";
  QFileDialog newFileDlg(this,
                         "Select VolimeFile",
                         m_internalDataDirectory,
                         ANALOGIC_VOLUME_FILE_NAME_FILTER);

  bool dlgRet =  newFileDlg.exec();

  if (!dlgRet) return;

  QStringList retList = newFileDlg.selectedFiles();
  if (retList.size() == 0) return;

  pathToBagdata = retList.first();
  QString tdrFileA = m_tdrFilepath.c_str();
  std::list<QString> tdrFiles;
  tdrFiles.push_back(tdrFileA);

  this->loadNewBag(pathToBagdata, tdrFiles);
}

//------------------------------------------------------------------------------
void MainWindow::onClearLog()
{
  m_logWindow->clear();
}


//------------------------------------------------------------------------------
void MainWindow::onCropSlidervalueChanged()
{
  double valPcnet = static_cast<double>(ui->cropSlider->value())/100.0;
  VREnums::AXIS mcurrentAxis  = VREnums::AXIS::Y;
  if (ui->checkSlabX->isChecked())
  {
    mcurrentAxis = VREnums::AXIS::X;
  }
  if (ui->checkSlabY->isChecked())
  {
    mcurrentAxis = VREnums::AXIS::Y;
  }
  if (ui->checkSlabZ->isChecked())
  {
    mcurrentAxis = VREnums::AXIS::Z;
  }
  m_volumeRendererTrancelucentView->SetSlabUnpackPosition(valPcnet,
                                                         mcurrentAxis, false);
  m_volumeRendererOpaqueView->SetSlabUnpackPosition(valPcnet,
                                                   mcurrentAxis, false);
}


//------------------------------------------------------------------------------
void MainWindow::onCropRadoXChecked()
{
  ui->checkSlabY->setChecked(false);
  ui->checkSlabZ->setChecked(false);
  m_volumeRendererTrancelucentView->SetCutViewAxis(VREnums::AXIS::X);
  m_volumeRendererOpaqueView->SetCutViewAxis(VREnums::AXIS::X);
  m_volumeRendererTrancelucentView->SetSlabUnpackPosition(0, VREnums::AXIS::X, false);
  m_volumeRendererOpaqueView->SetSlabUnpackPosition(0, VREnums::AXIS::X, false);
  ui->cropSlider->setValue(0);
}

//------------------------------------------------------------------------------
void MainWindow::onCropRadoYChecked()
{
  ui->checkSlabX->setChecked(false);
  ui->checkSlabZ->setChecked(false);
  m_volumeRendererTrancelucentView->SetCutViewAxis(VREnums::AXIS::Y);
  m_volumeRendererOpaqueView->SetCutViewAxis(VREnums::AXIS::Y);
  m_volumeRendererTrancelucentView->SetSlabUnpackPosition(0, VREnums::AXIS::Y, false);
  m_volumeRendererOpaqueView->SetSlabUnpackPosition(0, VREnums::AXIS::Y, false);
  ui->cropSlider->setValue(0);
}

//------------------------------------------------------------------------------
void MainWindow::onCropRadoZChecked()
{
  ui->checkSlabX->setChecked(false);
  ui->checkSlabY->setChecked(false);

  m_volumeRendererTrancelucentView->SetCutViewAxis(VREnums::AXIS::Z);
  m_volumeRendererOpaqueView->SetCutViewAxis(VREnums::AXIS::Z);
  m_volumeRendererTrancelucentView->SetSlabUnpackPosition(0, VREnums::AXIS::Z, false);
  m_volumeRendererOpaqueView->SetSlabUnpackPosition(0, VREnums::AXIS::Z, false);
  ui->cropSlider->setValue(0);
}



//------------------------------------------------------------------------------
void MainWindow::setWindowTileWithDataFile(QString dataFile)
{
  QFileInfo fileInfo(dataFile);
  QString shortName = fileInfo.fileName();

  m_bagTag = shortName;

  QString winTitle = "Qt VTK Volume Renderer - ";
  winTitle += shortName;

  this->setWindowTitle(winTitle);
  this->setCroFileFromDataFile(shortName);
}

//------------------------------------------------------------------------------
void MainWindow::setTDRLabel(QString tdrFileStr)
{
  QString tdrLabel = tdrFileStr;
  m_lineEditTDR->setText(tdrLabel);
}

//------------------------------------------------------------------------------
void ::MainWindow::setCroFileFromDataFile(QString& shortName)
{
   QString stub = shortName.left(shortName.length() - VOL_EXT_LENGTH);
   QString tail = shortName.right(VOL_EXT_LENGTH);
   if (tail.compare(CHECKPOINT_VOL_FILE_EXTENSION) == 0)
   {
     m_cropFile = stub + CHECKPOINT_CROP_FILE_EXTENSION;
   }
   if (tail.compare(COBRA_VOL_FILE_EXTENSION) == 0)
   {
      m_cropFile = stub + COBRA_CROP_FILE_EXTENSION;
   }
}
