//------------------------------------------------------------------------------
// File: MainWindow.cpp
// Description: Main Window Implementation
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include <QDebug>
#include "MainWindow.h"
#include "ReadDICOSHeader.h"
#include "ui_MainWindow.h"

QString g_mainWindowTitle         = "DICOS Utility Tool";
QString g_dicosDatadDireirectory  = "/home/analogic/devData/VolumeData/";
QString g_dicosOuputDirectory     = "/home/analogic/junk/";
QString g_defOutFile              = "DefaultOutput.dicos_ct";
QString g_docisLogFile            = "DICOS_log_file.txt";
QString g_defbagID                = "BAG-ID-0001";
QString g_defMachineID            = "ANAL-00001";


const int g_mainWindowPosX   = 100;
const int g_mainWindowPosY   = 50;
const int g_mainWindowWidth  = 900;
const int g_mainWindowHeight = 500;



const int g_dicos_name_width = 200;

const int g_bag_id_width     = 100;
const int g_machine_id_width = 100;

QStringList g_machineTypeList =  { "CheckPoint", "Cobra", };

//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_console(nullptr),
  m_nameOut(nullptr),
  m_bagIdOut(nullptr),
  m_machineIdOut(nullptr),
  m_machineType(nullptr),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->setGeometry(g_mainWindowPosX,
                    g_mainWindowPosY,
                    g_mainWindowWidth,
                    g_mainWindowHeight);


  this->setWindowTitle(g_mainWindowTitle);
  QIcon mainIcon(":/images/DICOSIcon.png");
  this->setWindowIcon(mainIcon);

  this->connect(ui->actionClose, &QAction::triggered,
                this,  &MainWindow::onCloseApp);

  this->connect(ui->actionAbout, &QAction::triggered,
                this, &MainWindow::onAbout);


  this->connect(ui->actionRead_DICOS_File, &QAction::triggered,
                this,  &MainWindow::onOpenDICOSFile);

  this->connect(ui->actionWrite_DICOS_File, &QAction::triggered,
               this, &MainWindow::onWriteDICOSFile);

  this->connect(ui->actionGen_From_Data_File, &QAction::triggered,
                this, &MainWindow::onGenFromDataFile);


  m_console = new ConsoleWidget(ui->plainTextEdit);

  LOG(INFO) << "Application MainWindow created!";
  m_console->logString("Console Created.");

  this->setupToolBar();
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
void MainWindow::onAbout()
{
  AboutDialog aboutDlg(this);
  aboutDlg.exec();
}


//------------------------------------------------------------------------------
void MainWindow::onWriteDICOSFile()
{
  QString fileOut    = g_dicosOuputDirectory + m_nameOut->text();
  QString logFileOut = g_dicosOuputDirectory + g_docisLogFile;

  QString message = "Output File:"  + fileOut;
  m_console->logString(message);


  std::string stdName = fileOut.toLocal8Bit().data();
  std::string stdLog  = logFileOut.toLocal8Bit().data();

  SDICOS::CTModule ctModule;

  //----------------------------------
  // Populate the ctModule here:
  analogic::workstation::ImageProperties* iProp;


  int currentScannerType = m_machineType->currentIndex();

  if (currentScannerType == 0)
  {
     iProp = new analogic::workstation::ImageProperties(analogic::workstation::SCANNER_TYPE_CHECKPOINT);
  }
  else
  {
    iProp = new analogic::workstation::ImageProperties(analogic::workstation::SCANNER_TYPE_COBRA);
  }

  std::string stdBagName     = m_bagIdOut->text().toLocal8Bit().data();
  std::string stdMachineName = m_machineIdOut->text().toLocal8Bit().data();
  DICOS::SimpleScanProperties scanProp(stdBagName,
                                       stdMachineName);

  DICOS::writeDICOSCtFile(&ctModule,
                          iProp,
                          scanProp,
                          stdName,
                          stdLog);

  this->showCtHeaderInfo(ctModule);
  delete iProp;
}

//------------------------------------------------------------------------------
void MainWindow::onOpenDICOSFile()
{
  QString pathToDICOSData = "";
  QString logPath  = g_dicosOuputDirectory + g_docisLogFile;

  QFileDialog newFileDlg(this,
                         "Select DICOS CT File",
                         g_dicosDatadDireirectory,
                         DICOS_FILE_FILTER);

  bool dlgRet =  newFileDlg.exec();

  if (!dlgRet) return;

  QStringList retList = newFileDlg.selectedFiles();
  if (retList.size() == 0) return;

  pathToDICOSData = retList.first();
  QString message = "DICOS File selected: " + pathToDICOSData;
  LOG(INFO) << message.toLocal8Bit().data();
  m_console->logString(message);

  std::string stdDataName = pathToDICOSData.toLocal8Bit().data();
  std::string stdLogName  = logPath.toLocal8Bit().data();


  SDICOS::CTModule ctModule;
  DICOS::readDICOSCtFile(ctModule,
                         stdDataName,
                         stdLogName);

  this->showCtHeaderInfo(ctModule);
}


//------------------------------------------------------------------------------
void MainWindow::onGenFromDataFile()
{
  QString pathToData = "";


  QFileDialog newFileDlg(this,
                         "Select Volume Data File",
                         g_dicosDatadDireirectory,
                         ANALOGIC_VOLUME_FILE_NAME_FILTER);

  bool dlgRet =  newFileDlg.exec();

  if (!dlgRet) return;
  pathToData = newFileDlg.selectedFiles().first();
  QFileInfo fInfo(pathToData);
  QString shortName = fInfo.fileName();

  QString nameTail = shortName.right(VOL_EXT_LENGTH);

  analogic::workstation::ImageProperties* iProp;

  if (nameTail.compare(COBRA_VOL_FILE_EXTENSION) == 0)
  {
     iProp = new analogic::workstation::ImageProperties(analogic::workstation::SCANNER_TYPE_COBRA);
     m_machineType->setCurrentIndex(1);
  }
  if (nameTail.compare(CHECKPOINT_VOL_FILE_EXTENSION) ==0 )
  {
    iProp = new analogic::workstation::ImageProperties(analogic::workstation::SCANNER_TYPE_CHECKPOINT);
    m_machineType->setCurrentIndex(0);
  }
  QString filenameCtHdr  = shortName.left(shortName.length() - VOL_EXT_LENGTH);
  filenameCtHdr += DICOS_CT_FILE_EXTENSION;
  m_nameOut->setText(filenameCtHdr);

  delete iProp;
}

//------------------------------------------------------------------------------
void MainWindow::showCtHeaderInfo(SDICOS::CTModule& ctModule)
{
  QString message;

  int imageSizeX = ctModule.GetCTImage().GetColumns();
  int imageSizeY = ctModule.GetCTImage().GetRows();

  double pixelSizeX =
      ctModule.GetMultiframeFunctionalGroups().GetSharedFunctionalGroups()->GetPixelMeasurement()->GetColumnSpacing();
  double pixelSizeY =
      ctModule.GetMultiframeFunctionalGroups().GetSharedFunctionalGroups()->GetPixelMeasurement()->GetRowSpacing();
  double pixelSizeZ =
      ctModule.GetMultiframeFunctionalGroups().GetSharedFunctionalGroups()->GetPixelMeasurement()->GetSliceThickness();

  QString msg;

  msg = "Image attribute imageSixe X: " +  QString::number(imageSizeX);
  m_console->logString(msg);

  msg =  "Image attribute imageSixe Y: " +  QString::number(imageSizeY);
  m_console->logString(msg);

  msg = "Pixel Size X: " +  QString::number(pixelSizeX);
  m_console->logString(msg);

  msg = "Pixel Size Y: " + QString::number(pixelSizeY);
  m_console->logString(msg);


  msg =  "Pixel Size Z: " + QString::number(pixelSizeZ );
  m_console->logString(msg);


  // Label info
  // 7. Bag ID
  // 8. Machine ID
  // 9. Bag Date/Time

  SDICOS::DcsString dicosBagID     = ctModule.GetObjectOfInspection().GetID().GetID();
  SDICOS::DcsString dicosMachineID = ctModule.GetGeneralEquipment().GetMachineID();
  SDICOS::DcsString dicosDate      = ctModule.GetSeries().GetDate();
  SDICOS::DcsString dicosTime      = ctModule.GetSeries().GetTime();

  // Format the Date
  std::string tmpDate = dicosDate.Get();
  std::string formattedYear  = tmpDate.substr(0, 4);
  std::string formattedMonth = tmpDate.substr(4, 2);
  std::string formattedDay   = tmpDate.substr(6, 2);
  std::string formatDate     = formattedMonth + "/" + formattedDay + "/" + formattedYear;


  // Format the Time
  std::string tmpTime = dicosTime.Get();
  std::string formattedHour    = tmpTime.substr(0, 2);
  std::string formattedMinute  = tmpTime.substr(2, 2);
  std::string formattedSecond  = tmpTime.substr(4, 2);
  std::string formatedTime     = formattedHour + ":" + formattedMinute + ":" + formattedSecond;


  QString bagLabel = "Bag ID: ";
  bagLabel += dicosBagID.Get();
  bagLabel += "\nMachine ID: ";
  bagLabel += dicosMachineID.Get();
  bagLabel += "\nDate: ";
  bagLabel += formatDate.c_str();
  bagLabel += " - ";
  bagLabel += formatedTime.c_str();
  bagLabel += "\n";


  message   =   "BAG ID     :";
  message  +=  dicosBagID.Get();
  LOG(INFO) << message.toLocal8Bit().data();
  m_console->logString(message);


  message  = "MACHINE ID :";
  message +=  dicosMachineID.Get();
  LOG(INFO) << message.toLocal8Bit().data();
  m_console->logString(message);


  message  =  "BAG DATE   :";
  message +=  dicosDate.Get();
  LOG(INFO) << message.toLocal8Bit().data();
  m_console->logString(message);

  message =  "BAG TIME   :";
  message += dicosTime.Get();
  LOG(INFO) << message.toLocal8Bit().data();
  m_console->logString(message);

  message = "Formated Bag Label:\n";
  message += bagLabel;
  LOG(INFO) << bagLabel.toLocal8Bit().data();
  m_console->logString(message);
}


//------------------------------------------------------------------------------
void MainWindow::setupToolBar()
{
  //----------------------------------------
  QLabel* nameLab = new QLabel("DICOS File Out:",
                               nullptr);

  m_nameOut = new QLineEdit();
  m_nameOut->setMaximumWidth(g_dicos_name_width);
  m_nameOut->setMinimumWidth(g_dicos_name_width);


  //------------------------------
  // BAG-DI and Machie-ID
  QLabel* bagIdLab =  new QLabel("BAG ID:",
                                 nullptr);
  m_bagIdOut = new QLineEdit();
  m_bagIdOut->setMinimumWidth(g_bag_id_width);
  m_bagIdOut->setMaximumWidth(g_bag_id_width);

  QLabel* machineIdLab = new QLabel("Machine ID:",
                                    nullptr);
  m_machineIdOut = new QLineEdit();
  m_machineIdOut->setMinimumWidth(g_machine_id_width);
  m_machineIdOut->setMaximumWidth(g_machine_id_width);

  //-----------------------------------
  // Machine type list
  QLabel* machineTypeLab = new QLabel("Machine Type:", nullptr);
  m_machineType = new QComboBox(nullptr);
  for (int i = 0; i < g_machineTypeList.size(); i++)
  {
    m_machineType->addItem(g_machineTypeList.at(i));
  }
  m_machineType->setCurrentIndex(0);

  ui->mainToolBar->addWidget(nameLab);
  ui->mainToolBar->addWidget(m_nameOut);
  ui->mainToolBar->addWidget(bagIdLab);
  ui->mainToolBar->addWidget(m_bagIdOut);
  ui->mainToolBar->addWidget(machineIdLab);
  ui->mainToolBar->addWidget(m_machineIdOut);
  ui->mainToolBar->addWidget(machineTypeLab);
  ui->mainToolBar->addWidget(m_machineType);


  m_nameOut->setText(g_defOutFile);
  m_bagIdOut->setText(g_defbagID);
  m_machineIdOut->setText(g_defMachineID);
}

//------------------------------------------------------------------------------
void MainWindow::setButtonImageAndText(QPushButton* pButton,
                                       QString imageSource,
                                       QString buttonText,
                                       int buttonWidth,
                                       int buttonHeight)
{
  QPixmap pixmap(imageSource);
  qDebug() << pixmap.size();
  QIcon buttonIncon(pixmap);
  pButton->setText(buttonText);
  pButton->setIcon(buttonIncon);
  pButton->setIconSize(pixmap.rect().size());
  pButton->setMaximumWidth(buttonWidth);
  pButton->setMaximumHeight(buttonHeight);
}


