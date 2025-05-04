//------------------------------------------------------------------------------
// File: MainWindow.cpp
// Description: Main window imnplimentation
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "MainWindow.h"
#include "LocalVRUtils.h"
#include "LocalScannerDefs.h"

#include "ui_MainWindow.h"

//===============================================================================
// VERSION 2.0 Wish list
// Input:
// -A directory containing only .vol files
// -File extension to be loaded is .vol
// -The dimensions of the .vol files are hard coded as 630x430xLENGTH
// -No .properties files necessary

// Output:
// -A viewer which opens those .vol files and displays them in a similar
//  way to how the scanner will display them.
// -The viewer displays the name of the file being viewed at the top of
//  the window.
// -The left and right arrow keys toggle between files in the input directory.

//===============================================================================




//--------------------------------------------------------------
// Matrix axis elements used for flipping in Y all Actors.
//--------------------------------------------------------------
static const double g_flipMatrixAxialElements[16] =
{
    1,  0, 0,  0,
    0, -1, 0,  0,
    0,  0, 1,  0,
    0,  0, 0,  1
};

//-------------------------------------------------




QString g_mainWindowTitle = "Qt VTK 8.1 Volume Renderer Application";
int g_mainWindowWidth     = 1600;
int g_mainWindowHeight    = 880;
int g_mainWindowPosX      = 50;
int g_mainWindowPosY      = 50;


int g_toolButtonMinimumWidth  = 50;
int g_toolButtonMinimumHeight = 50;


const double g_threatTFSharpness   = 1.0;
const double g_threatTFMidopoint   = 1.0;

QString g_helpAboutTitle = "Qt VTK 8.1 Volume Renderer Test Application - version 2.0";
QString g_helpAboutText  = "<p>This <b>Volume Renderer</b> application is a generic viewer of "
                           "volume data with ConneCT Workstation's opacity and color "
                           "transferfunctions.</p> "
                           "<p>It also has some density and other colorization functionality "
                           "similar to the ConneCT Workstation.</p>"
                           "<p>This project depends on Qt 5.7 and CMake 3.x  and VTK 8.1.0 .</p>"
                           "<p>Author: Andre' R. Oughton.</p>"
                           "<p>(c) Analogic Corporation - 2018. </p>";


QString g_styleCheckButton = "QPushButton::checked { background-color: #6699FF; }";


// ONE FOR MY TESTING:

QString g_defaultbagQueueFile  = DEFAULT_BAG_QUEUE_FILE;

QString g_defaultVolumeFile      = "ConneCT00001_20180207_092905_000035770";
QString g_volumeExtensionA       = GENERIC_VOLFILE_EXT_A;
QString g_volumeExtensionB       = GENERIC_VOLFILE_EXT_B;
QString g_propertiesExtension    = PROPERTIES_FILE_EXTENSION;


QString g_TranslucentHSVMapFile     = "NormalHSVColor.colmap";
QString g_TranslucentOpacityMapFile = "NormalOpacity.opmap";
QString g_SurfaceHSVMapFile         = "SurfaceHSVColor.colmap";
QString g_SurfaceOpacityMapFile     = "SurfaceOpacity.opmap";
QString g_TransitionMapfile         = "MaterialTransition.txt";
QString g_grayScalecolors           = "GrayscaleValuesAndOpacities.txt";

//-----------------------------------
// Low density Left only:
QString g_TranslucentLowDensitHSVMapFile        = "LowDensityNormalHSVColor.colmap";
QString g_TranslucentLowDensityOpacityMapFile   = "LowDensityNormalOpacity.opmap";



//------------------------------------------------------------------------------
MainWindow::MainWindow(QString& pathTooBagList,
                       QWidget *parent) :
  QMainWindow(parent),
  m_haveBagQueue(false),
  m_bagFilePath(pathTooBagList),
  m_resourceDirectory(""),
  m_bagFile(""),
  m_propertiesFile(""),
  m_bagListFile(""),
  m_bagQueueIndex(0),
  m_pIP(nullptr),
  m_dataIsLoaded(false),
  m_ptrRawdata(nullptr),
  m_translucentUseShading(false),
  m_surfaceUseShading(true),
  m_organicRemovable(true),
  m_inorganicRemovable(true),
  m_metalRemovable(true),
  m_inverseBackground(false),
  m_renderWithColor(true),
  m_doColorizeOrganic(true),
  m_doColorizeInorganic(true),
  m_doColorizeMetal(true),
  m_doColorizeThreat(true),
  m_totalRenderTime(0),
  m_buttonAllMaterials(nullptr),
  m_buttonOrganic(nullptr),
  m_buttonInOrganic(nullptr),
  m_buttonMetal(nullptr),
  m_buttonGrayScale(nullptr),
  m_buttonInverserBG(nullptr),
  m_buttonLowDensity(nullptr),
  m_buttonCamera(nullptr),
  m_buttonNextBag(nullptr),
  m_buttonPreviousBag(nullptr),
  m_buttonResetView(nullptr),
  m_measureRulerActive(false),
  m_measureP0(0, 0, 0),
  m_measureP1(1, 1, 1),
  m_opacityModifyScalar(1.23),
  m_volumeWidgetTranslucent(nullptr),
  m_volumeWidgetSurface(nullptr),
  m_volumeProprtyRefRight(nullptr),
  m_volumeProprtyRefLeft(nullptr),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  if (m_bagFilePath.length() > 1)
  {
    m_haveBagQueue = true;
  }

  QSurfaceFormat defaultFormat = QVTKOpenGLWidget::defaultFormat();
  defaultFormat.setSamples(0);
  QSurfaceFormat::setDefaultFormat(defaultFormat);

  this->setWindowIcon(QIcon(":images/VRMainIcon.png"));

  this->setWindowTitle(g_mainWindowTitle);

  this->move(g_mainWindowPosX,
             g_mainWindowPosY);

  this->setMinimumWidth(g_mainWindowWidth);
  this->setMinimumHeight(g_mainWindowHeight);

  this->setupMenu();
  this->setupToolbar();

  //------------------------------------------------
  // Lets Only use Check[point scanners
  m_pIP = new analogic::test_vr::ImageProperties(analogic::test_vr::SCANNER_TYPE_CHECKPOINT);
  //------------------------------------------------


  this->setupYFlipMatrix();

  //--------------------------------------------
  // OK setup directories

  std::string resourcesDir;
  bool bGetRet = analogic::test_vr::getResourcesDirectory(resourcesDir);
  if (bGetRet)
  {
    m_resourceDirectory = resourcesDir.c_str();
    std::cout << "Resources  Directory :" << m_resourceDirectory.toStdString() << std::endl;
  }
  if (!m_haveBagQueue)
  {
    this->getDefaultBagFilePath(m_bagFilePath);
  }
//  std::string homeDir;
//  bGetRet = analogic::test_vr::getHomeDirectory(homeDir);
//  if (bGetRet)
//  {
//    m_bagDataDirectory  = g_bag_data_dir;
//    std::cout << "Bag data  Directory :" << m_bagDataDirectory.toStdString() << std::endl;
//  }

  //----------------------------
  // Read Transfer Functions

  // Left HSV
  QString mapFileHSVLeft = m_resourceDirectory +  g_TranslucentHSVMapFile;
  std::string readfile = mapFileHSVLeft.toStdString();
  bool readRet =  analogic::test_vr::readHSVColorFile(readfile,
                                  m_translucentHSVColorList);
  if (!readRet)
  {
    QString message = "Error reading file: " + mapFileHSVLeft;
    QMessageBox::information(this, "Error reading HSV Colormap!", message);
    this->onCloseApp();
  }

  // LEFT OPACITY
  QString mapFileOpacitytLeft = m_resourceDirectory +  g_TranslucentOpacityMapFile;
  readfile = mapFileOpacitytLeft.toStdString();
  readRet =  analogic::test_vr::readOpacityFile(readfile,
                                  m_translucentOpacityList);
  if (!readRet)
  {
    QString message = "Error reading file: " + mapFileOpacitytLeft;
    QMessageBox::information(this, "Error reading Opacity map!", message);
    this->onCloseApp();
  }


  // RIGHT HSV
  QString mapFileHSVRight = m_resourceDirectory +  g_SurfaceHSVMapFile;
  readfile = mapFileHSVRight.toStdString();
  readRet  =  analogic::test_vr::readHSVColorFile(readfile,
                                  m_surfaceHSVColorList);
  if (!readRet)
  {
    QString message = "Error reading file: " + mapFileHSVRight;
    QMessageBox::information(this, "Error reading Opacity map!", message);
    this->onCloseApp();
  }


  // RIGHT OPACITY
  QString mapFileOpacityRight = m_resourceDirectory +  g_SurfaceOpacityMapFile;
  readfile = mapFileOpacityRight.toStdString();
  readRet  =  analogic::test_vr::readOpacityFile(readfile,
                                  m_surfaceOpacityList);
  if (!readRet)
  {
    QString message = "Error reading file: " + mapFileOpacityRight;
    QMessageBox::information(this, "Error reading Opacity map!", message);
    this->onCloseApp();
  }

  //--------------------------------
  // Left Low Density HSV Colors
  QString mapFileHSVLowDensityLeft = m_resourceDirectory +  g_TranslucentLowDensitHSVMapFile;

  readfile = mapFileHSVLowDensityLeft.toStdString();
  readRet  =  analogic::test_vr::readHSVColorFile(readfile,
                                  m_lowDensityTranslucentHSVColorList);
  if (!readRet)
  {
    QString message = "Error reading file: " + mapFileHSVLowDensityLeft;
    QMessageBox::information(this, "Error reading HSV map!", message);
    this->onCloseApp();
  }
  //--------------------------------
  // Left Low Density Opacity
  QString mapFileOpacityLowDensityLeft = m_resourceDirectory +  g_TranslucentLowDensityOpacityMapFile;

  readfile = mapFileOpacityLowDensityLeft.toStdString();
  readRet  =  analogic::test_vr::readOpacityFile(readfile,
                                  m_lowDensityTranslucentOpacityList);
  if (!readRet)
  {
    QString message = "Error reading file: " + mapFileOpacityLowDensityLeft;
    QMessageBox::information(this, "Error reading Opacity map!", message);
    this->onCloseApp();
  }

  //---------------------------------

  // Read Transition map
  QString mapFileTransition = m_resourceDirectory +  g_TransitionMapfile;
  this->readTransitionFile(mapFileTransition, m_materialTransitionMap);


  QString grayScaleSettingsfile = m_resourceDirectory + g_grayScalecolors;
  this->readGrayScaleSettings(grayScaleSettingsfile, m_grayscaleStruct);


  //------------------------
  // Setup  Bag queue
  m_bagListFile = m_bagFilePath;
  std::cout << m_bagListFile.toStdString() << std::endl;
  this->readBagQueue();
  this->initBagInputs();
  this->onBeginRendering();
  m_buttonAllMaterials->setChecked(true);
  m_buttonGrayScale->setChecked(false);
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete ui;
}

//------------------------------------------------------------------------------
void MainWindow::onCloseApp()
{
  this->close();
}


//------------------------------------------------------------------------------
void MainWindow::onHelpAbout()
{
  QMessageBox::information(this,
                           g_helpAboutTitle,
                           g_helpAboutText);
}

//------------------------------------------------------------------------------
void MainWindow::setupMenu()
{
  this->connect(ui->actionAdvanceBag, &QAction::triggered,
                 this,      &MainWindow::onNextBag);


 this->connect(ui->actionAbout, &QAction::triggered,
                this, &MainWindow::onHelpAbout);


  this->connect(ui->actionLoad_Volume, &QAction::triggered,
                 this, &MainWindow::onLoadVolumeFromFile);

  this->connect(ui->actionExit, &QAction::triggered,
                this, &MainWindow::onCloseApp);
}


//------------------------------------------------------------------------------
void MainWindow::getDefaultBagFilePath(QString& bagFilePath)
{
  QString currentPath =   qApp->applicationDirPath();
  if (!currentPath.endsWith("/"))
  {
    currentPath += "/";
  }
  bagFilePath = currentPath + g_defaultbagQueueFile;

  QFile qFileSetup(bagFilePath);

  if (!qFileSetup.exists())
  {
    QString dlgTitle  = "Error - default bag queue not found";
    QString  dlgText  = "File: ./BagQueue.txt";
    QMessageBox::information(this, dlgTitle, dlgText);
  }
}

//------------------------------------------------------------------------------
bool MainWindow::readSetupFile(const QString& setupFile,
                               QString& bagdataFolder)
{
  bool foundVal = false;
  std::string stdFileName = setupFile.toLocal8Bit().data();
  std::ifstream inFile(stdFileName.c_str());
  if ( !inFile )
  {
    std::string msg;
    msg = "Could not open Setup file '" + stdFileName + "'";
    return false;
  }
  std::string str;
  while (std::getline(inFile, str))
  {
      QString sLine = str.c_str();
      std::cout << str << std::endl;
      if (sLine.at(0) != '#')
      {
        // Not a comment
        QStringList lineList = sLine.split(SETUP_DELIMETER);
        QString sKey = lineList.at(0);
        QString sVal = lineList.at(1);
        if (!sVal.endsWith("/"))
        {
          sVal += "/";
        }
        if (sKey.compare(BAG_PATH_STRING) == 0)
        {
          bagdataFolder = sVal;
          foundVal = true;
          break;
        }
      }
   }
   inFile.close();
   return foundVal;
}

//------------------------------------------------------------------------------
QPushButton* MainWindow::createToolBarButton(QString strIconName,
                                             QString toolTip,
                                             bool checkable)
{
  QPushButton* retButton = new QPushButton("", nullptr);
  retButton->setMinimumHeight(g_toolButtonMinimumHeight);
  retButton->setMinimumWidth(g_toolButtonMinimumWidth);
  retButton->setIcon(QIcon(strIconName));
  retButton->setIconSize(QSize(g_toolButtonMinimumWidth -1,
                               g_toolButtonMinimumHeight - 1));
  retButton->setToolTip(toolTip);
  if (checkable)
  {
    retButton->setCheckable(true);
  }
  retButton->setStyleSheet(g_styleCheckButton);
  return retButton;
}

//------------------------------------------------------------------------------
void MainWindow::setupToolbar()
{
  m_buttonAllMaterials = this->createToolBarButton(":images/AllMaterialsIcon.png",
                                                   "Show ALL materials",
                                                   true);
  ui->mainToolBar->addWidget(m_buttonAllMaterials);

  m_buttonOrganic = this->createToolBarButton(":images/OrganicIcon.png",
                                              "Show ORGANIC materials",
                                              true);
  ui->mainToolBar->addWidget(m_buttonOrganic);

  m_buttonInOrganic = this->createToolBarButton(":images/InOrganicIcon.png",
                                                "Show INORGANIC materials",
                                                true);
  ui->mainToolBar->addWidget(m_buttonInOrganic);

  m_buttonMetal = this->createToolBarButton(":images/MetalIcon.png",
                                            "Show METAL materals",
                                            true);
  ui->mainToolBar->addWidget(m_buttonMetal);


  m_buttonGrayScale = this->createToolBarButton(":images/GrayScaleIcon.png",
                                                "Show Image in Grayscale",
                                                true);
  ui->mainToolBar->addWidget(m_buttonGrayScale);


  m_buttonInverserBG = this->createToolBarButton(":images/InverseIcon.png",
                                                "Set inverse backround",
                                                 true);
  ui->mainToolBar->addWidget(m_buttonInverserBG);


  m_buttonLowDensity = this->createToolBarButton(":images/LowDensityIcon.png",
                                                 "Show Low Density material left",
                                                 true);
  ui->mainToolBar->addWidget(m_buttonLowDensity);


  m_buttonCamera = this->createToolBarButton(":images/CameraIcon.png",
                                             "Show Opaque right image",
                                              true);
  ui->mainToolBar->addWidget(m_buttonCamera);

  //----------------------------------------
  // Add Spacer
  QWidget* empty = new QWidget();
  empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  empty->setMinimumWidth(TOOLBAR_SEPERATOR_WIDTH);
  empty->setMaximumWidth(TOOLBAR_SEPERATOR_WIDTH);
  ui->mainToolBar->addWidget(empty);

  m_buttonPreviousBag = this->createToolBarButton(":images/PreviousItemIcon.png",
                                                  "Show Previous Bag in Queue",
                                                  false);

  ui->mainToolBar->addWidget(m_buttonPreviousBag);

  m_buttonNextBag = this->createToolBarButton(":images/NextItemIcon.png",
                                              "Show Next Bag in Queue",
                                              false);

  ui->mainToolBar->addWidget(m_buttonNextBag);


  m_buttonResetView = this->createToolBarButton(":images/ResetIcon.png",
                                                "Reset Bag View",
                                                false);

   ui->mainToolBar->addWidget(m_buttonResetView);



   m_testFilter = this->createToolBarButton(":images/RulerIcon64.png",
                                            "Measure Item",
                                             true);
   ui->mainToolBar->addWidget(m_testFilter);

  //----------------------------------------
  // Current loaded file
  // QLabel* curFileLabel = new QLabel("Current File: ", nullptr);
  // m_crurrentLoadedFile = new QLineEdit(this);onMeasureRulerClick
  // m_crurrentLoadedFile->setMinimumWidth(CURRENT_FILE_LINE_EDIT_WIDTH);
  // m_crurrentLoadedFile->setMaximumWidth(CURRENT_FILE_LINE_EDIT_WIDTH);
  // m_crurrentLoadedFile->setReadOnly(true);
  // ui->mainToolBar->addWidget(curFileLabel);
  // ui->mainToolBar->addWidget(m_crurrentLoadedFile);

  //----------------------------------------
  // Setup connections
  this->connect(m_buttonAllMaterials, &QPushButton::clicked,
                this, &MainWindow::onViewAllMaterials);

  this->connect(m_buttonOrganic, &QPushButton::clicked,
                this, &MainWindow::onViewOrganicMaterials);

  this->connect(m_buttonInOrganic, &QPushButton::clicked,
                this, &MainWindow::onViewInOrganicMaterials);

  this->connect(m_buttonMetal, &QPushButton::clicked,
                this, &MainWindow::onViewMetalMaterials);

  this->connect(m_buttonGrayScale, &QPushButton::clicked,
                this, &MainWindow::onViewGrayScale);

  this->connect(m_buttonInverserBG, &QPushButton::clicked,
                this, &MainWindow::onViewInverseBackground);

  this->connect(m_buttonLowDensity, &QPushButton::clicked,
                this, &MainWindow::onViewLowDensityLeft);

  this->connect(m_buttonCamera, &QPushButton::clicked,
                this, &MainWindow::onViewCameraRight);

  this->connect(m_buttonNextBag, &QPushButton::clicked,
                this, &MainWindow::onNextBag);


  this->connect(m_buttonPreviousBag, &QPushButton::clicked,
                this, &MainWindow::onPreviousBag);

   this->connect(m_buttonResetView, &QPushButton::clicked,
                 this, &MainWindow::onResetViewAlignment);


   this->connect(m_testFilter, &QPushButton::clicked,
                 this,  &MainWindow::onMeasureRulerClick);
}

//------------------------------------------------------------------------------
void MainWindow::onBeginRendering()
{
  m_totalRenderTime   = 0;

  m_volumeWidgetTranslucent  = this->renderVolumeData(ui->LeftLabel,
                                                      true,
                                                      m_translucentHSVColorList,
                                                      m_translucentOpacityList,
                                                      m_translucentUseShading,
                                                      INTERPOLATE_LINEAR);

  m_volumeWidgetSurface = this->renderVolumeData(ui->RightLabel,
                                                 false,
                                                 m_surfaceHSVColorList,
                                                 m_surfaceOpacityList,
                                                 m_surfaceUseShading,
                                                 INTERPOLATE_LINEAR);

  //-----------------------------
  m_volumeWidgetTranslucent->resetViewAlignment();
  m_volumeWidgetSurface->resetViewAlignment();
  //-----------------------------

  delete [] m_ptrRawdata;
  m_ptrRawdata = nullptr;

  std::cout << "Total Render Time (s): " << m_totalRenderTime << std::endl;
}

//------------------------------------------------------------------------------
void MainWindow::showCurrentFile()
{
  ui->lineEditCurrentFile->setText(m_bagFile);
}

//------------------------------------------------------------------------------
void MainWindow::removeVolume()
{
  m_volumeWidgetTranslucent->removeVolume();
  m_volumeWidgetSurface->removeVolume();
  m_volumeWidgetTranslucent->updateRender();
  m_volumeWidgetSurface->updateRender();
}


//------------------------------------------------------------------------------
void MainWindow::onNextBag()
{
  m_bagQueueIndex++;
  if (m_bagQueueIndex > m_bagQueueList.size() -1)
  {
    m_bagQueueIndex = 0;
  }
  this->removeVolume();
  this->initBagInputs();
  this->onBeginRendering();
}

//------------------------------------------------------------------------------
void MainWindow::onPreviousBag()
{
  m_bagQueueIndex--;
  if (m_bagQueueIndex < 0)
  {
    m_bagQueueIndex =  m_bagQueueList.size() -1;
  }
  this->removeVolume();
  this->initBagInputs();
  this->onBeginRendering();
}

//------------------------------------------------------------------------------
bool MainWindow::readBagQueue()
{
  QFileInfo fileInfo(m_bagListFile);
  if (!fileInfo.exists())
  {
    std::cout << "Baglist file not found : "<< m_bagListFile.toStdString() << std::endl;
    return false;
  }
  std::string stdFileName = m_bagListFile.toStdString();
  std::ifstream inFile(stdFileName.c_str());
  if ( !inFile )
  {
    std::string msg;
    msg = "Could not open bag listg file '" + stdFileName + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  try
  {
    while (std::getline(inFile, str))
    {
      QString sLine = str.c_str();
      if (sLine.at(0) != BAG_QUEUE_COMMENT_CHAR)
      {
        std::cout << sLine.toStdString() << std::endl;
        m_bagQueueList.push_back(sLine);
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }
  return true;
}

//------------------------------------------------------------------------------
void MainWindow::initBagInputs()
{
  // Get the next file in the baglist
  m_bagFile =  m_bagQueueList.at(m_bagQueueIndex);

  // Get the extension
  // It's either *.vol or *.cp_vol

  QString strExt   = "";
  QString nameLeft = "";

  if (m_bagFile.contains(GENERIC_VOLFILE_EXT_A))
  {
    strExt = GENERIC_VOLFILE_EXT_A;
    nameLeft =  m_bagFile.left(m_bagFile.length() - strExt.length());
  }
  else
  {
    strExt = GENERIC_VOLFILE_EXT_B;
    nameLeft =  m_bagFile.left(m_bagFile.length() - strExt.length());
  }
  m_propertiesFile = nameLeft + PROPERTIES_FILE_EXTENSION;
  this->loadBagData();
}


//------------------------------------------------------------------------------
void MainWindow::loadBagData()
{
  QFileInfo bagTest(m_bagFile);
  if (!bagTest.exists())
  {
    QString message = "File not found: " + m_bagFile;

    QMessageBox::information(this,
                             "Bag Volume file does not exist.",
                             message);
    return;
  }
  QFileInfo propTest(m_propertiesFile);
  if (propTest.exists())
  {
    analogic::test_vr::readVolumeProperties(m_propertiesFile,
                                            m_pIP,
                                            m_roiStart,
                                            m_roiEnd);
  }
  else
  {
    QString message = "File not found. using DEFAULTS : " + m_propertiesFile;

    std::cout << message.toStdString() << std::endl;
    m_pIP->m_imageWidth  = DEFAULT_BAG_WIDTH;
    m_pIP->m_imageHeight = DEFAULT_BAG_HEIGHT;
    int64_t fileSize = bagTest.size();

    int64_t bagLength = fileSize/(DEFAULT_BAG_WIDTH * DEFAULT_BAG_HEIGHT * sizeof(uint16_t));
    m_pIP->m_imageMaxLength = bagLength;
    m_roiStart.setX(0);
    m_roiStart.setY(0);
    m_roiStart.setZ(0);

    m_roiEnd.setX(DEFAULT_BAG_WIDTH  -1);
    m_roiEnd.setY(DEFAULT_BAG_HEIGHT -1);
    m_roiEnd.setZ(m_pIP->m_imageMaxLength -1);
  }



  std::cout << "Read properties      : " << "\n"
            << " P0[x,y,z]           : " <<  m_roiStart.x() << ", " <<  m_roiStart.y() << ", "
            << m_roiStart.z() << "\n"
            << " P1[x,y,z]           : " <<  m_roiEnd.x()   << ", " <<  m_roiEnd.y()   << ", "
            << m_roiEnd.z()   << "\n"
            << " PixelSpacing[x,y,z] : " <<  m_pIP->m_pixelSizeX << ", " << m_pIP->m_pixelSizeY << ", "
            << m_pIP->m_pixelSizeZ << "\n"
            << " Imagesize[W,H,L]    : " <<  m_pIP->m_imageWidth << ", " << m_pIP->m_imageHeight << ", "
            << m_pIP->m_imageMaxLength << std::endl;


  this->readFileData(m_bagFile);
  this->showCurrentFile();
}

//------------------------------------------------------------------------------
bool MainWindow::readFileData(QString& volumeFile)
{
  if (!m_pIP) return false;
  m_dataIsLoaded = false;

  std::string dataFileName = volumeFile.toStdString();
  QFileInfo volfile(volumeFile);
  int64_t sizeFile = volfile.size();

  int numSlices = sizeFile/( sizeof(uint16_t)* m_pIP->m_imageWidth * m_pIP->m_imageHeight );
  std::cout << "Estimeated # Slices : " << numSlices << std::endl;

  int estimatedSize  = numSlices*m_pIP->m_imageWidth*m_pIP->m_imageHeight*sizeof(uint16_t);

  // int numData = sizeFile/( sizeof(uint16_t));

  int64_t sizeBuffer = sizeFile/(sizeof(uint16_t));
  if (m_ptrRawdata)
  {
    delete [] m_ptrRawdata;
    m_ptrRawdata = nullptr;
  }

  m_ptrRawdata = new uint16_t[sizeBuffer];

  FILE* filep = fopen(dataFileName.c_str(), "rb");
  if (!filep)
  {
    QString errorMessage = "Error opening file: ";
    errorMessage += volumeFile;
    QMessageBox::warning(0,
                         "Application Error.",
                         errorMessage);
    delete [] m_ptrRawdata;
    return false;
  }

  int bytes_read = fread(reinterpret_cast<char*>(m_ptrRawdata), sizeof(char), sizeFile, filep);

  double dSizeMB      = (1.0*sizeFile)/(1024.0*1024.0);
  double dSizeReadMB  = (1.0*bytes_read)/(1024.0*1024.0);
  double dEstSizeMB   = (1.0*estimatedSize)/(1024.0*1024.0);

  std::cout << "Size of Volume Data File (MB):" << dSizeMB << " - MBytes read:" << dSizeReadMB << std::endl;
  std::cout << "Estimated Size Volume (MB)   :" << dEstSizeMB << std::endl;
  fclose(filep);
  m_dataIsLoaded = true;
  return true;
}

//------------------------------------------------------------------------------
bool MainWindow::readTransitionFile(QString materialTransitionFilePath,
                         std::map<MaterialTransition_E, double>& transitionMap)
{
  std::string inFilename = materialTransitionFilePath.toStdString();
  std::ifstream inFile(inFilename.c_str());
  if (!inFile)
  {
    std::string msg;
    msg = "Could not open material transitions file '" + materialTransitionFilePath.toStdString() + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  //--------------------------------------
  // Read header and throw it away.
  std::getline(inFile, str);

  try
  {
    //-------------------------------------
    while (std::getline(inFile, str))
    {
      if ( str.length() > MIN_FILE_READLN_LEN)
      {
        QString sEntry = str.c_str();
        int indexComent =  sEntry.indexOf(VOL_COLOR_COMMENT);
        QStringList strList = sEntry.split(VOL_COLOR_DELIM);
        if ( (strList.length() >= 2) &&
             (indexComent < 0      )
             )
        {
          std::cout << str << std::endl;
          //--------------
          // ORGANIC
          if (strList.at(0).compare(this->transitionName(SIG_ORGANIC_MIN)) == 0)
          {
            transitionMap[SIG_ORGANIC_MIN] = strList.at(1).toDouble();
          }
          if (strList.at(0).compare(this->transitionName(SIG_ORGANIC_MAX)) == 0)
          {
            transitionMap[SIG_ORGANIC_MAX] = strList.at(1).toDouble();
          }
          //----------------
          // INORGANIC
          if (strList.at(0).compare(this->transitionName(SIG_INORGANIC_MIN)) == 0)
          {
            transitionMap[SIG_INORGANIC_MIN] = strList.at(1).toDouble();
          }
          if (strList.at(0).compare(this->transitionName(SIG_INORGANIC_MAX)) == 0)
          {
            transitionMap[SIG_INORGANIC_MAX] = strList.at(1).toDouble();
          }
          //----------------
          // METAL
          if (strList.at(0).compare(this->transitionName(SIG_METAL_MIN)) == 0)
          {
            transitionMap[SIG_METAL_MIN] = strList.at(1).toDouble();
          }
          if (strList.at(0).compare(this->transitionName(SIG_METAL_MAX)) == 0)
          {
            transitionMap[SIG_METAL_MAX] = strList.at(1).toDouble();
          }
          //---------------
          // THREATS

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_MIN))== 0)
          {
            transitionMap[SIG_THREAT_MIN] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_A)) == 0)
          {
            transitionMap[SIG_THREAT_SET_A] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_B)) == 0)
          {
            transitionMap[SIG_THREAT_SET_B] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_C)) == 0)
          {
            transitionMap[SIG_THREAT_SET_C] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_D)) == 0)
          {
            transitionMap[SIG_THREAT_SET_D] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_E)) == 0)
          {
            transitionMap[SIG_THREAT_SET_E] = strList.at(1).toDouble();
          }


          if (strList.at(0).compare(this->transitionName(SIG_THREAT_MAX)) == 0)
          {
            transitionMap[SIG_THREAT_MAX] = strList.at(1).toDouble();
          }
          if (strList.at(0).compare(this->transitionName(SIG_DEFAULT_NOISE_CLIP)) == 0 )
          {
            transitionMap[SIG_DEFAULT_NOISE_CLIP] = strList.at(1).toDouble();
          }
        }
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }
  if (transitionMap.size() == 0 )
  {
    std::string errorMessage = "List size == 0, Possibly a corrupted file:" +
        materialTransitionFilePath.toStdString();
    throw std::logic_error(errorMessage);
  }
  return true;
}


//------------------------------------------------------------------------------
QString MainWindow::transitionName(MaterialTransition_E transitionE)
{
  QString retVal = "ORGANIC_SIGNAL_MIN";
  switch(transitionE)
  {
  case SIG_ORGANIC_MIN:
    retVal = "ORGANIC_SIGNAL_MIN";
    break;

  case SIG_ORGANIC_MAX:
    retVal = "ORGANIC_SIGNAL_MAX";
    break;

  case SIG_INORGANIC_MIN:
    retVal = "INORGANIC_SIGNAL_MIN";
    break;

  case SIG_INORGANIC_MAX:
    retVal = "INORGANIC_SIGNAL_MAX";
    break;

  case SIG_METAL_MIN:
    retVal = "METAL_SIGNAL_MIN";
    break;

  case SIG_METAL_MAX:
    retVal = "METAL_SIGNAL_MAX";
    break;

  case SIG_THREAT_MIN:
    retVal = "THREAT_SIGNAL_MIN";
    break;

  case SIG_THREAT_SET_A:
    retVal = "THREAT_SIGNAL_SETVAL_A";
    break;

  case SIG_THREAT_SET_B:
    retVal = "THREAT_SIGNAL_SETVAL_B";
    break;

  case SIG_THREAT_SET_C:
    retVal = "THREAT_SIGNAL_SETVAL_C";
    break;

  case SIG_THREAT_SET_D:
    retVal = "THREAT_SIGNAL_SETVAL_D";
    break;

  case SIG_THREAT_SET_E:
    retVal = "THREAT_SIGNAL_SETVAL_E";
    break;

  case SIG_THREAT_SET_F:
    retVal = "THREAT_SIGNAL_SETVAL_F";
    break;

  case SIG_THREAT_MAX:
    retVal = "THREAT_SIGNAL_MAX";
    break;
  case SIG_DEFAULT_NOISE_CLIP:
    retVal = "DEFAULT_NOISE_CLIP";
    break;
  }
  return retVal;
}



//------------------------------------------------------------------------------
void MainWindow::initHSVColorTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTF,
                                                         std::list<HSVColorSigVal>& hsvColorList)
{
  if (!m_renderWithColor)
  {
    this->initBWTransferFunction(colorTF, hsvColorList);
    return;
  }

  std::list<HSVColorSigVal>::iterator colIt;
  for (colIt = hsvColorList.begin(); colIt!= hsvColorList.end(); colIt++)
  {
    HSVColorSigVal colVal = *colIt;
    double sigVal  = colVal.m_signalVal;
    double h, s, v;

    v = 0;

    //-----------------------
    // BWELOW  ORGANIC
    if (sigVal <= m_materialTransitionMap[SIG_ORGANIC_MIN])
    {
      h = colVal.m_h;
      s = colVal.m_s;
      v = colVal.m_v;
      colorTF->AddHSVPoint(sigVal, h, s, v);
    }
    //-----------------------

    //-----------------------
    // ORGANIC
    if ( (sigVal >= m_materialTransitionMap[SIG_ORGANIC_MIN]) &&
         (sigVal <= m_materialTransitionMap[SIG_ORGANIC_MAX]))
    {
      if (m_doColorizeOrganic)
      {
          h = colVal.m_h;
          s = colVal.m_s;
          v = colVal.m_v;
      }
      else
      {
        this->hsvOfOrganicGray(sigVal, h, s, v);
      }
      colorTF->AddHSVPoint(sigVal, h, s, v);
    }
    //-----------------------



    //-----------------------
    // INORGANIC
    if ((sigVal >= m_materialTransitionMap[SIG_INORGANIC_MIN]) &&
        (sigVal <= m_materialTransitionMap[SIG_INORGANIC_MAX])
        )
    {
      if (m_doColorizeInorganic)
      {
        h = colVal.m_h;
        s = colVal.m_s;
        v = colVal.m_v;
      }
      else
      {
        this->hsvOfInorganicGray(sigVal, h, s, v);
      }
      colorTF->AddHSVPoint(sigVal, h, s, v);
    }

    //-----------------------

    //-----------------------
    // METAL
    if ((sigVal >= m_materialTransitionMap[SIG_METAL_MIN]) &&
        (sigVal <= m_materialTransitionMap[SIG_METAL_MAX])
        )
    {
      if (m_doColorizeMetal)
      {
         h = colVal.m_h;
         s = colVal.m_s;
         v = colVal.m_v;
      }
      else
      {
        this->hsvOfMetalGray(sigVal, h, s, v);
      }
      colorTF->AddHSVPoint(sigVal, h, s, v, g_threatTFMidopoint, g_threatTFSharpness);
    }

    //-----------------------

   //------------------------
   // Threats
   if ( (sigVal >= m_materialTransitionMap[SIG_THREAT_MIN]) &&
        (sigVal <= m_materialTransitionMap[SIG_THREAT_MAX])
        )
    {
     if (m_doColorizeThreat)
     {
        h = colVal.m_h;
        s = colVal.m_s;
        v = colVal.m_v;
     }
     colorTF->AddHSVPoint(sigVal, h, s, v, g_threatTFMidopoint, g_threatTFSharpness);
    }
   //-------------------------
  }
}

//------------------------------------------------------------------------------
void MainWindow::initOpacityTransferFunction(vtkSmartPointer<vtkPiecewiseFunction>
                                                           opacityTF,
                                                           std::list<OpacitySigVal>& opasityLis)
{
  std::list<OpacitySigVal>::iterator  opIt;
  for (opIt = opasityLis.begin(); opIt != opasityLis.end(); opIt++)
  {
    OpacitySigVal opVal = *opIt;
    double sigVal       = opVal.m_signalVal;
    double opacity      = 1;

    //-----------------------
    // BELOW  ORGANIC
    if ( sigVal <= m_materialTransitionMap[SIG_ORGANIC_MIN] )
    {
        opacity = opVal.m_opacity;
        opacityTF->AddPoint(sigVal, opacity);
    }
    //-----------------------

    //-----------------------
    // ORGANIC
    if (
        ( sigVal >= m_materialTransitionMap[SIG_ORGANIC_MIN] ) &&
        ( sigVal <= m_materialTransitionMap[SIG_ORGANIC_MAX] )
        )
    {
      if ((m_organicRemovable) &&
          (!m_doColorizeOrganic))
      {
        opacity = 0;
        opacityTF->AddPoint(sigVal, opacity);
      }
      else
      {
        opacity = opVal.m_opacity;
        opacityTF->AddPoint(sigVal, opacity);
      }
    }
    //-----------------------

    //-----------------------
    // INORGANIC
    if (
        ( sigVal >= m_materialTransitionMap[SIG_INORGANIC_MIN] ) &&
        ( sigVal <= m_materialTransitionMap[SIG_INORGANIC_MAX] )
        )
    {
       if ((m_inorganicRemovable) &&
           (!m_doColorizeInorganic) )
       {
         opacity = 0;
         opacityTF->AddPoint(sigVal, opacity);
       }
       else
       {
        opacity = opVal.m_opacity;
        opacityTF->AddPoint(sigVal, opacity);
       }
    }

    //-----------------------

    //-----------------------
    // METAL
    if ( ( sigVal >= m_materialTransitionMap[SIG_METAL_MIN] ) &&
         ( sigVal <= m_materialTransitionMap[SIG_METAL_MAX] )
       )
    {
      if((m_metalRemovable) &&
         (!m_doColorizeMetal) )
      {
        opacity = 0;
        opacityTF->AddPoint(sigVal, opacity);
      }
      else
      {
       opacity = opVal.m_opacity;
       opacityTF->AddPoint(sigVal, opacity);
      }
    }
    //-----------------------

    //-----------------------
    // Threats
    if (( sigVal >= m_materialTransitionMap[SIG_THREAT_MIN] ) &&
        ( sigVal <= m_materialTransitionMap[SIG_THREAT_MAX] )
        )
    {
          opacity = opVal.m_opacity;
          opacityTF->AddPoint(sigVal, opacity);
    }
    //---------------------
  }
}


//------------------------------------------------------------------------------
void MainWindow::initBWTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTF,
                                         std::list<HSVColorSigVal>& hsvColorList)
{
  std::list<HSVColorSigVal>::iterator colIt;
  for (colIt = hsvColorList.begin(); colIt!= hsvColorList.end(); colIt++)
  {
    HSVColorSigVal colVal = *colIt;
    double sigVal  = colVal.m_signalVal;
    double h, s, v;

    v = 0;

    //-----------------------
    // BWELOW  ORGANIC
    if (sigVal <= m_materialTransitionMap[SIG_ORGANIC_MIN])
    {
      h = colVal.m_h;
      s = colVal.m_s;
      v = colVal.m_v;
      this->hsvOfOrganicGray(sigVal, h, s, v);
      colorTF->AddHSVPoint(sigVal, h, s, v);
    }
    //-----------------------

    //-----------------------
    // ORGANIC
    if ( (sigVal >= m_materialTransitionMap[SIG_ORGANIC_MIN]) &&
         (sigVal <= m_materialTransitionMap[SIG_ORGANIC_MAX]))
    {
      if (m_doColorizeOrganic)
      {
        this->hsvOfOrganicGray(sigVal, h, s, v);
        colorTF->AddHSVPoint(sigVal, h, s, v);
      }
    }
    //-----------------------



    //-----------------------
    // INORGANIC
    if ((sigVal >= m_materialTransitionMap[SIG_INORGANIC_MIN]) &&
        (sigVal <= m_materialTransitionMap[SIG_INORGANIC_MAX])
        )
    {
      if (m_doColorizeInorganic)
      {
        this->hsvOfInorganicGray(sigVal, h, s, v);
        colorTF->AddHSVPoint(sigVal, h, s, v);
      }
    }

    //-----------------------

    //-----------------------
    // METAL
    if ((sigVal >= m_materialTransitionMap[SIG_METAL_MIN]) &&
        (sigVal <= m_materialTransitionMap[SIG_THREAT_MAX])
        )
    {
      if (m_doColorizeMetal)
      {
        this->hsvOfMetalGray(sigVal, h, s, v);
        colorTF->AddHSVPoint(sigVal, h, s, v);
      }
    }
    //-----------------------
    //------------------------
    // Threats
    if ( (sigVal >= m_materialTransitionMap[SIG_THREAT_MIN]) &&
         (sigVal <= m_materialTransitionMap[SIG_THREAT_MAX])
         )
    {
      if (m_doColorizeThreat)
      {
        h = colVal.m_h;
        s = colVal.m_s;
        v = colVal.m_v;
      }
      this->hsvOfMetalGray(sigVal, h, s, v);
      colorTF->AddHSVPoint(sigVal, h, s, v, g_threatTFMidopoint, g_threatTFSharpness);
    }
    //-------------------------
  }

  //-------------------------------------------------------------
  // ARO-DEBUGGING
  // Uncomment to save the grayscale Transferfunction to a file
  //
  //  QString sFile = "/home/analogic/junk/LeftViewGrayscaleTF.csv";
  //  if (this->displayName().contains("Left View"))
  //  {
  //     this->saveVTKColorMapToFile(colorTF,sFile);
  //  }
  //-------------------------------------------------------------
}



//------------------------------------------------------------------------------
void MainWindow::hsvOfOrganicGray(double signalValue,
                      double& h,
                      double& s,
                      double& v)
{
  h = 0;
  s = 0;
  //----------------------------
  // Linear relationship
  // v = A*signalValue + B;
  if (!m_inverseBackground)
  {
    double A = (m_grayscaleStruct.m_HSV_V_organicMax - m_grayscaleStruct.m_HSV_V_organicMin)/
        ( m_materialTransitionMap[SIG_ORGANIC_MAX] - m_materialTransitionMap[SIG_ORGANIC_MIN] );
    double B = m_grayscaleStruct.m_HSV_V_organicMin - A*m_materialTransitionMap[SIG_ORGANIC_MIN];
    v = A*signalValue + B;
  }
  else
  {
    double A = (m_grayscaleStruct.m_HSV_V_organicMax_inv - m_grayscaleStruct.m_HSV_V_organicMin_inv)/
        ( m_materialTransitionMap[SIG_ORGANIC_MAX] - m_materialTransitionMap[SIG_ORGANIC_MIN] );
    double B = m_grayscaleStruct.m_HSV_V_organicMin_inv - A*m_materialTransitionMap[SIG_ORGANIC_MIN];
    v = A*signalValue + B;
  }
  this->clampHSV(h, s, v);
}


//------------------------------------------------------------------------------
void MainWindow::hsvOfInorganicGray(double signalValue,
                      double& h,
                      double& s,
                      double& v)
{
  h = 0;
  s = 0;
  //----------------------------
  // Linear relationship
  // v = A*signalValue + B;
  if (!m_inverseBackground)
  {
    double A =(m_grayscaleStruct.m_HSV_V_inorganicMax - m_grayscaleStruct.m_HSV_V_inorganicMin)/
        (m_materialTransitionMap[SIG_INORGANIC_MAX] - m_materialTransitionMap[SIG_INORGANIC_MIN]);
    double B = m_grayscaleStruct.m_HSV_V_inorganicMin - A*m_materialTransitionMap[SIG_INORGANIC_MIN];
    v = A*signalValue + B;
  }
  else
  {
    double A =(m_grayscaleStruct.m_HSV_V_inorganicMax_inv - m_grayscaleStruct.m_HSV_V_inorganicMin_inv)/
        (m_materialTransitionMap[SIG_INORGANIC_MAX] - m_materialTransitionMap[SIG_INORGANIC_MIN]);
    double B = m_grayscaleStruct.m_HSV_V_inorganicMin_inv - A*m_materialTransitionMap[SIG_INORGANIC_MIN];
    v = A*signalValue + B;
  }
  this->clampHSV(h, s, v);
}

//------------------------------------------------------------------------------
void MainWindow::hsvOfMetalGray(double signalValue,
                      double& h,
                      double& s,
                      double& v)
{
  h = 0;
  s = 0;
  //----------------------------
  // Linear relationship
  // v = A*signalValue + B;
  if (!m_inverseBackground)
  {
    double A = (m_grayscaleStruct.m_HSV_V_metalMax - m_grayscaleStruct.m_HSV_V_metalMin)/
        ( m_materialTransitionMap[SIG_METAL_MAX] - m_materialTransitionMap[SIG_METAL_MIN] );
    double B = m_grayscaleStruct.m_HSV_V_metalMin - A*m_materialTransitionMap[SIG_METAL_MIN];
    v = A*signalValue + B;
  }
  else
  {
    double A = (m_grayscaleStruct.m_HSV_V_metalMax_inv - m_grayscaleStruct.m_HSV_V_metalMin_inv)/
        ( m_materialTransitionMap[SIG_METAL_MAX] - m_materialTransitionMap[SIG_METAL_MIN] );
    double B = m_grayscaleStruct.m_HSV_V_metalMin_inv - A*m_materialTransitionMap[SIG_METAL_MIN];
    v = A*signalValue + B;
  }
  this->clampHSV(h, s, v);
}


//------------------------------------------------------------------------------
void MainWindow::clampHSV(double& h, double& s, double& v)
{
  if (h > 1.0)  h = 1.0;
  if (s > 1.0)  s = 1.0;
  if (v > 1.0)  v = 1.0;
  if (h < 0.0)  h = 0.0;
  if (s < 0.0)  s = 0.0;
  if (v < 0.0)  v = 0.0;
}


//------------------------------------------------------------------------------
bool MainWindow::readGrayScaleSettings(QString filePath,
                                        GrayscaleValueOpacityStruct& grayScaleSettings)
{
  std::string inFilename = filePath.toStdString();
  std::ifstream inFile(inFilename.c_str());
  if (!inFile)
  {
    std::string msg;
    msg = "Could not open material transitions file '" + filePath.toStdString() + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  //--------------------------------------
  // Read header and throw it away.
  std::getline(inFile, str);

  try
  {
    //-------------------------------------
    while (std::getline(inFile, str))
    {
      if ( str.length() > MIN_FILE_READLN_LEN)
      {
        QString sEntry = str.c_str();
        int indexComent =  sEntry.indexOf(VOL_COLOR_COMMENT);
        QStringList strList = sEntry.split(VOL_COLOR_DELIM);
        if ( (strList.length() >= 2) &&
             (indexComent < 0      )
             )
        {
          //=========================================================
          // *** ORGANIC
          //--------------
          // V ORGANIC MIN
          if (strList.at(0).compare("GRAY_HSV_V_ORGANIC_MIN") == 0)
          {
            grayScaleSettings.m_HSV_V_organicMin = strList.at(1).toDouble();
          }
          //--------------
          // V ORGANIC MAX
          if (strList.at(0).compare("GRAY_HSV_V_ORGANIC_MAX") == 0)
          {
            grayScaleSettings.m_HSV_V_organicMax = strList.at(1).toDouble();
          }
          //---------------
          // V ORGANIC MIN INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_ORGANIC_MIN_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_organicMin_inv = strList.at(1).toDouble();
          }
          //---------------
          // V ORGANIC MAX INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_ORGANIC_MAX_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_organicMax_inv = strList.at(1).toDouble();
          }
          //---------------
          // OPACITY ORGANIC MIN
          if (strList.at(0).compare("GRAY_OPACITY_ORGANIC_MIN") == 0)
          {
            grayScaleSettings.m_opacityOrganicMin = strList.at(1).toDouble();
          }
          // OPACITY ORGANIC MAX
          if (strList.at(0).compare("GRAY_OPACITY_ORGANIC_MAX") == 0)
          {
            grayScaleSettings.m_opacityOrganicMax = strList.at(1).toDouble();
          }

          //=========================================================
          // *** INORGANIC
          //--------------
          // V INORGANIC MIN
          if (strList.at(0).compare("GRAY_HSV_V_INORGANIC_MIN") == 0)
          {
            grayScaleSettings.m_HSV_V_inorganicMin = strList.at(1).toDouble();
          }
          //--------------
          // V INORGANIC MAX
          if (strList.at(0).compare("GRAY_HSV_V_INORGANIC_MAX") == 0)
          {
            grayScaleSettings.m_HSV_V_inorganicMax = strList.at(1).toDouble();
          }
          //--------------
          // V INORGANIC MIN INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_INORGANIC_MIN_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_inorganicMin_inv = strList.at(1).toDouble();
          }
          //--------------
          // V INORGANIC MAX INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_INORGANIC_MAX_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_inorganicMax_inv = strList.at(1).toDouble();
          }
          //--------------
          // OPACITY INORGANIC MIN
          if (strList.at(0).compare("GRAY_OPACITY_INORGANIC_MIN") == 0)
          {
            grayScaleSettings.m_opacityInorganicMin = strList.at(1).toDouble();
          }
          // OPACITY INORGANIC MAX
          if (strList.at(0).compare("GRAY_OPACITY_INORGANIC_MAX") == 0)
          {
            grayScaleSettings.m_opacityInorganicMax = strList.at(1).toDouble();
          }

          //=========================================================
          // *** METAL
          //--------------
          // V METAL MIN
          if (strList.at(0).compare("GRAY_HSV_V_METAL_MIN") == 0)
          {
            grayScaleSettings.m_HSV_V_metalMin = strList.at(1).toDouble();
          }
          //--------------
          // V METAL MAX
          if (strList.at(0).compare("GRAY_HSV_V_METAL_MAX") == 0)
          {
            grayScaleSettings.m_HSV_V_metalMax = strList.at(1).toDouble();
          }
          //--------------
          // V METAL MIN INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_METAL_MIN_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_metalMin_inv = strList.at(1).toDouble();
          }
          //--------------
          // V METAL MAX INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_METAL_MAX_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_metalMax_inv = strList.at(1).toDouble();
          }
          // OPACITY METAL MIN
          if (strList.at(0).compare("GRAY_OPACITY_METAL_MIN") == 0)
          {
            grayScaleSettings.m_opacityMetalMin = strList.at(1).toDouble();
          }
          // OPACITY METAL MAX
          if (strList.at(0).compare("GRAY_OPACITY_METAL_MAX") == 0)
          {
            grayScaleSettings.m_opacityMetalMax = strList.at(1).toDouble();
          }
        }
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }
  return true;
}


//------------------------------------------------------------------------------
void MainWindow::setMateralVisibility()
{
  //------------------------------------
  // Left
  vtkNew<vtkColorTransferFunction> colorFunctionLeft;
  this->initHSVColorTransferFunction(colorFunctionLeft,
                                     m_translucentHSVColorList);

  vtkNew<vtkPiecewiseFunction> opacityFunctionLeft;
  this->initOpacityTransferFunction(opacityFunctionLeft,
                                    m_translucentOpacityList);

  if (m_volumeProprtyRefLeft)
  {
    m_volumeProprtyRefLeft->SetScalarOpacity(opacityFunctionLeft.Get());
    m_volumeProprtyRefLeft->SetColor(colorFunctionLeft.Get());
  }


  //------------------------------------
  // Right
  vtkNew<vtkColorTransferFunction> colorFunctionRight;
  this->initHSVColorTransferFunction(colorFunctionRight,
                                     m_surfaceHSVColorList);

  vtkNew<vtkPiecewiseFunction> opacityFunctionRight;
  this->initOpacityTransferFunction(opacityFunctionRight,
                                    m_surfaceOpacityList);

  if (m_volumeProprtyRefRight)
  {
    m_volumeProprtyRefRight->SetScalarOpacity(opacityFunctionRight.Get());
    m_volumeProprtyRefRight->SetColor(colorFunctionRight.Get());
  }

  m_volumeWidgetTranslucent->updateRender();
  m_volumeWidgetSurface->updateRender();
}

//------------------------------------------------------------------------------
VKVTVolumeWidget_Def* MainWindow::renderVolumeData(QWidget *parent,
                                                   bool isLeftWidget,
                                                   std::list<HSVColorSigVal>& HSVColorList,
                                                   std::list<OpacitySigVal>&  OpacityList,
                                                   bool useShading,
                                                   vtkInterpolationType interpType)
{
  // Timer for testing
  QTime msProcessTimer = QTime::currentTime();


  // Set up volume rendering
  vtkNew<vtkColorTransferFunction> colorFunction;
  this->initHSVColorTransferFunction(colorFunction,
                                     HSVColorList);

   vtkNew<vtkPiecewiseFunction> opacityFunction;
  this->initOpacityTransferFunction(opacityFunction,
                                    OpacityList);
  int nx;
  int ny;
  int nz;

  //  READ FROM FILE
  vtkNew<vtkImageData> imageData;
  if (!m_dataIsLoaded)
  {
    nx = DEFAULT_BAG_WIDTH;
    ny = DEFAULT_BAG_HEIGHT;
    nz = DEFAULT_BAG_LENGTH;

    uint16_t def_signal = 0x00f0;

    imageData->SetExtent(0, nx, 0, ny, 0, nz);
    imageData->AllocateScalars(VTK_UNSIGNED_SHORT, 1);
    std::fill_n(static_cast<uint16_t *>(imageData->GetScalarPointer()), nx*ny*nz, def_signal);
  }
  else
  {
    nx = m_pIP->m_imageWidth;
    ny = m_pIP->m_imageHeight;
    nz = m_pIP->m_imageMaxLength;

    int nData = nx*ny*nz*sizeof(uint16_t);


    imageData->SetExtent(0, nx-1,
                         0, ny-1,
                         0, nz-1);

    imageData->SetSpacing(m_pIP->m_pixelSizeX,
                          m_pIP->m_pixelSizeY,
                          m_pIP->m_pixelSizeZ);

    imageData->AllocateScalars(VTK_UNSIGNED_SHORT, 1);
    memcpy(static_cast<uint16_t *>(imageData->GetScalarPointer()),
           m_ptrRawdata, nData);
  }


  //---------------------------------------------------
  // Look at time to allocate and setup volume
  float setupAndallocationTime_s = (1.0*msProcessTimer.elapsed())/1000.0;
  std::cout<< "Setup and Allocation time (s): " << setupAndallocationTime_s <<  std::endl;

  // Mapping and Render timme
  QTime msMapRenderTimer = QTime::currentTime();
  //------------------------------------------------------
  vtkSmartPointer<vtkImageClip> imageClip  = vtkSmartPointer<vtkImageClip>::New();
  imageClip->SetInputData(imageData.Get());
  imageClip->SetOutputWholeExtent(m_roiStart.x(),
                                  m_roiEnd.x(),
                                  m_roiStart.y(),
                                  m_roiEnd.y(),
                                  m_roiStart.z(),
                                  m_roiEnd.z());


  vtkNew<vtkOpenGLGPUVolumeRayCastMapper> volumeMapper;

  volumeMapper->SetInputConnection(imageClip->GetOutputPort());


  volumeMapper->Update();


  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeProperty->SetScalarOpacity(opacityFunction.Get());
  volumeProperty->SetColor(colorFunction.Get());
  if (useShading)
  {
    volumeProperty->ShadeOn();
  }
  else
  {
    volumeProperty->ShadeOff();
  }


  //=====================
  switch(interpType)
  {
  case  INTERPOLATE_NEAREST:
    volumeProperty->SetInterpolationTypeToNearest();
    break;
  case   INTERPOLATE_LINEAR:
    volumeProperty->SetInterpolationTypeToLinear();
    break;
  }
  //=======================

   if (isLeftWidget)
   {
    m_volumeProprtyRefLeft = volumeProperty.GetPointer();
   }
   else
   {
      m_volumeProprtyRefRight = volumeProperty.GetPointer();
   }

  vtkNew<vtkVolume> volume;
  volume->SetMapper(volumeMapper.Get());
  volume->SetProperty(volumeProperty.Get());
  volume->SetUserMatrix(m_flipYMatrix);




  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();


   renderer->AddVolume(volume.Get());


   renderer->SetBackground(VOLUME_BG_DEFAULT_RED,
                          VOLUME_BG_DEFAULT_GREEN,
                          VOLUME_BG_DEFAULT_BLUE);


#ifdef USE_OPENGL_VTK_WIDGET
   // Show with QVTKOpenGLWidget
   vtkNew<vtkGenericOpenGLRenderWindow> window;
   window->AddRenderer(renderer.Get());

   OpenGLVolumeWidget* widget = new OpenGLVolumeWidget(parent);
   widget->SetRenderWindow(window.Get());
   widget->setMinimumWidth(VR_WIDGET_DEF_WIDTH);
   widget->setMinimumHeight(VR_WIDGET_DEF_HEIGHT);
   widget->show();

   widget->setRenderer(renderer);
   widget->finalize();


#else
  vtkSmartPointer<vtkRenderWindow> renderWin =
      vtkSmartPointer<vtkRenderWindow>::New();
  renderWin->AddRenderer(m_renderer);

  QVTKWidget* widget = new QVTKWidget(parent);

  widget->setMinimumWidth(VR_WIDGET_DEF_WIDTH);
  widget->setMinimumHeight(VR_WIDGET_DEF_HEIGHT);
  widget->SetRenderWindow(renderWin);

  widget->show();
#endif


   float mapRendertime_s = (1.0*msMapRenderTimer.elapsed())/1000.0;

   float processTime_s   = (1.0*msProcessTimer.elapsed())/1000.0;

   m_totalRenderTime += mapRendertime_s;

   std::cout << "Mapping an Render time (s): " << mapRendertime_s << std::endl;
   std::cout << "Full Process time (s): " << processTime_s << std::endl;


   return widget;
}



//------------------------------------------------------------------------------
void MainWindow::setupYFlipMatrix()
{
  m_flipYMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  m_flipYMatrix->DeepCopy(g_flipMatrixAxialElements);
}


//------------------------------------------------------------------------------
void MainWindow::onLoadVolumeFromFile()
{
  QFileDialog loadDlg(this);
  QString nameFilter = GENERIC_VOLFILE_NAME_FILTER;

  loadDlg.setAcceptMode(QFileDialog::AcceptOpen);
  loadDlg.setNameFilter(nameFilter);
  // loadDlg.setDirectory(m_bagDataDirectory);
  loadDlg.setMinimumWidth(900);
  loadDlg.setMinimumHeight(700);
  bool bRet = loadDlg.exec();
  QString selectedFile;
  if(bRet)
  {
    QStringList fileList = loadDlg.selectedFiles();
    if (fileList.size() > 0)
    {
      selectedFile = fileList.at(0);
      m_bagFile = selectedFile;
      //====================================================================
      QString strExt   = "";
      QString nameLeft = "";

      if (m_bagFile.contains(GENERIC_VOLFILE_EXT_A))
      {
        strExt = GENERIC_VOLFILE_EXT_A;
        nameLeft =  m_bagFile.left(m_bagFile.length() - strExt.length());
      }
      else
      {
        strExt = GENERIC_VOLFILE_EXT_B;
        nameLeft =  m_bagFile.left(m_bagFile.length() - strExt.length());
      }
      //====================================================================
      m_propertiesFile = nameLeft + PROPERTIES_FILE_EXTENSION;
      std::cout <<  "Loading bag     : " << m_bagFile.toStdString() << std::endl;
      std::cout <<  "With properties : " << m_propertiesFile.toStdString() << std::endl;
      this->loadBagData();
      this->onBeginRendering();
    }
  }
}




//------------------------------------------------------------------------------
void MainWindow::onViewAllMaterials()
{
  if (m_buttonAllMaterials->isChecked())
  {
    m_doColorizeOrganic   = true;
    m_doColorizeInorganic = true;
    m_doColorizeMetal     = true;
    m_buttonOrganic->setChecked(false);
    m_buttonInOrganic->setChecked(false);
    m_buttonMetal->setChecked(false);
  }
  else
  {
    m_doColorizeOrganic   = false;
    m_doColorizeInorganic = false;
    m_doColorizeMetal     = false;
  }

  this->setMateralVisibility();
}

//------------------------------------------------------------------------------
void MainWindow::onViewOrganicMaterials()
{
  if(m_buttonOrganic->isChecked())
  {
    m_doColorizeOrganic   = true;
  }
  else
  {
    m_doColorizeOrganic   = false;
  }
  m_buttonAllMaterials->setChecked(false);
  this->setMateralVisibility();
}

//------------------------------------------------------------------------------
void MainWindow::onViewInOrganicMaterials()
{
  if(m_buttonInOrganic->isChecked())
  {
    m_doColorizeInorganic   = true;
    this->setMateralVisibility();
  }
  else
  {
    m_doColorizeInorganic   = false;
    this->setMateralVisibility();
  }
}

//------------------------------------------------------------------------------
void MainWindow::onViewMetalMaterials()
{
  if(m_buttonMetal->isChecked())
  {
    m_doColorizeMetal   = true;
    this->setMateralVisibility();
  }
  else
  {
    m_doColorizeMetal   = false;
    this->setMateralVisibility();
  }
}

//------------------------------------------------------------------------------
void MainWindow::onViewGrayScale()
{
  if (!m_buttonGrayScale->isChecked())
  {
    m_renderWithColor = true;
  }
  else
  {
    m_renderWithColor = false;
  }
  this->setMateralVisibility();
}

//------------------------------------------------------------------------------
void MainWindow::onViewInverseBackground()
{
  if (m_buttonInverserBG->isChecked())
  {
    m_volumeWidgetTranslucent->setRendererBGColor(VOLUME_BG_INVERSE_RED,
                                                  VOLUME_BG_INVERSE_GREEN,
                                                  VOLUME_BG_INVERSE_BLUE);

    m_volumeWidgetSurface->setRendererBGColor(VOLUME_BG_INVERSE_RED,
                                              VOLUME_BG_INVERSE_GREEN,
                                              VOLUME_BG_INVERSE_BLUE);
  }
  else
  {
    m_volumeWidgetTranslucent->setRendererBGColor(VOLUME_BG_DEFAULT_RED,
                                                  VOLUME_BG_DEFAULT_GREEN,
                                                  VOLUME_BG_DEFAULT_BLUE);
    m_volumeWidgetSurface->setRendererBGColor(VOLUME_BG_DEFAULT_RED,
                                              VOLUME_BG_DEFAULT_GREEN,
                                              VOLUME_BG_DEFAULT_BLUE);
  }
  m_volumeWidgetTranslucent->updateRender();
  m_volumeWidgetSurface->updateRender();
}

//------------------------------------------------------------------------------
void MainWindow::onViewLowDensityLeft()
{
  if (m_buttonLowDensity->isChecked())
  {
    vtkNew<vtkColorTransferFunction> colorFunctionLeft;
    this->initHSVColorTransferFunction(colorFunctionLeft,
                                       m_lowDensityTranslucentHSVColorList);

    vtkNew<vtkPiecewiseFunction> opacityFunctionLeft;
    this->initOpacityTransferFunction(opacityFunctionLeft,
                                      m_lowDensityTranslucentOpacityList);

    if (m_volumeProprtyRefLeft)
    {
      m_volumeProprtyRefLeft->SetScalarOpacity(opacityFunctionLeft.Get());
      m_volumeProprtyRefLeft->SetColor(colorFunctionLeft.Get());
    }
  }
  else
  {
    vtkNew<vtkColorTransferFunction> colorFunctionLeft;
    this->initHSVColorTransferFunction(colorFunctionLeft,
                                       m_translucentHSVColorList);

    vtkNew<vtkPiecewiseFunction> opacityFunctionLeft;
    this->initOpacityTransferFunction(opacityFunctionLeft,
                                      m_translucentOpacityList);

    if (m_volumeProprtyRefLeft)
    {
      m_volumeProprtyRefLeft->SetScalarOpacity(opacityFunctionLeft.Get());
      m_volumeProprtyRefLeft->SetColor(colorFunctionLeft.Get());
    }
  }
  m_volumeWidgetTranslucent->updateRender();
}

//------------------------------------------------------------------------------
void MainWindow::onViewCameraRight()
{
  if (m_buttonCamera->isChecked())
  {
    this->setRightOpacitySignalLevel(95);
  }
  else
  {
    this->setRightOpacitySignalLevel(50);
  }
  m_volumeWidgetSurface->updateRender();
}

//------------------------------------------------------------------------------
void MainWindow::onResetViewAlignment()
{
  m_volumeWidgetTranslucent->resetViewAlignment();
  m_volumeWidgetSurface->resetViewAlignment();
}


//------------------------------------------------------------------------------
void MainWindow::onMeasureRulerClick()
{
  if (!m_measureRulerActive)
  {
    m_volumeWidgetSurface->armMeasuringRuler(m_measureP0,
                                             m_measureP1);
    m_measureRulerActive = true;
    this->showRulerPosition();
  }
  else
  {
    m_volumeWidgetSurface->disarmMeasuringRuler(m_measureP0,
                                                m_measureP1);
    this->showRulerPosition();
    m_measureRulerActive = false;
  }
}

//------------------------------------------------------------------------------
void MainWindow::showRulerPosition()
{
  std::cout << "Ruler Position P0-P1[x,y,z]: ["
            <<  m_measureP0.x() << ", "
            <<  m_measureP0.y() << ", "
            <<  m_measureP0.z() << "] - ["
            <<  m_measureP1.x() << ", "
            <<  m_measureP1.y() << ", "
            <<  m_measureP1.z() << "]"
            <<  std::endl;
}

//------------------------------------------------------------------------------
void MainWindow::setRightOpacitySignalLevel(int percentSet)
{
  m_currentOpacityList.clear();

  std::list<OpacitySigVal> opacityList;
  std::list<OpacitySigVal>::iterator itL;
  if ( (m_volumeProprtyRefRight !=  nullptr ) &&
       (m_surfaceOpacityList.size() !=0)

       )
  {
    for (itL = m_surfaceOpacityList.begin(); itL !=m_surfaceOpacityList.end(); itL++)
    {
      OpacitySigVal opI = *itL;
      double vOrig = opI.m_signalVal;

      double vNew = this->scaleSignalValue(percentSet,
                                           vOrig);
      // double allowedMax = m_transtitonMap[SIG_METAL_MAX];
      // if (vNew > allowedMax ) vNew = allowedMax;

      OpacitySigVal tmpI  = opI;
      tmpI.m_signalVal = vNew;
      opacityList.push_back(tmpI);
      m_currentOpacityList.push_back(tmpI);
    }

    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
        vtkSmartPointer<vtkPiecewiseFunction>::New();

    this->initOpacityTransferFunction(opacityTransferFunction, opacityList);
    m_volumeProprtyRefRight->SetScalarOpacity(opacityTransferFunction);
  }
}

//------------------------------------------------------------------------------
double  MainWindow::scaleSignalValue(int pcntChange, double setPoint)
{
 double deltaChange = (pcntChange - 50.0)/100.0;
 double y = setPoint - m_opacityModifyScalar*setPoint*deltaChange;
 return y;
}

