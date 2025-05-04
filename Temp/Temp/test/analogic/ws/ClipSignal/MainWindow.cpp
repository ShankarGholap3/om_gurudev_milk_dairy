//------------------------------------------------------------------------------
// File: MainWindow.cpp
// Description: Main Wuindow implementation.
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------
#include "MainWindow.h"
#include "ui_MainWindow.h"

QString g_mainWindowTitle = "Flip Data Volume Y values - ";

QString g_dataPath = "devData/VolumeData/";

QString g_clipSuffix = "-CLIP";

int g_mainWindowPosX   = 50;
int g_mainWindowPosY   = 50;

int g_mainWindowWidth  = 500;
int g_mainWindowHeight = 400;

uint16_t g_signalClipValue =  4095;


//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_currentFileShortNanme(""),
  m_currentFile(""),
  m_clipFile(""),
  m_console(nullptr),
  m_volumDataFolder(""),
  m_clipButton(nullptr),
  m_eraseButton(nullptr),
  m_statsButton(nullptr)
{
  ui->setupUi(this);

  m_console = new ConsoleWidget(ui->plainTextEdit);

  this->setWindowIcon(QIcon(":images/ClipSignalIcon.png"));
  this->setWindowTitleWithCurrent();

  this->move(g_mainWindowPosX,
             g_mainWindowPosY);

  this->setMinimumWidth(g_mainWindowWidth);
  this->setMinimumHeight(g_mainWindowHeight);

  this->setupMenu();
  this->setupToolbar();

  std::string homeFolder;

  bool getRes = analogic::workstation::getHomeFolder(homeFolder);
  if (getRes)
  {
    m_volumDataFolder = homeFolder.c_str();
    m_volumDataFolder += "/";
    m_volumDataFolder += g_dataPath;
  }
  else
  {
    QMessageBox::warning(this,
                         "Can't Find Home Folder!",
                          "Unknown error.");
  }
}


//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete ui;
  delete m_console;
}

//------------------------------------------------------------------------------
void MainWindow::setupMenu()
{
   this->connect(ui->actionSelect_File_To_Clip, &QAction::triggered,
                 this,  &MainWindow::onOpenDataFile);

  this->connect(ui->actionExit, &QAction::triggered,
                this,  &MainWindow::onCloseApp);
}


void MainWindow::setupToolbar()
{
  m_clipButton = new QPushButton(nullptr);


  QPixmap pixmapClip(":images/ClipSignalIcon02.png");
  QIcon buttonIconClip(pixmapClip);
  m_clipButton->setToolTip("Pereform Clipp");
  m_clipButton->setIcon(buttonIconClip);
  ui->mainToolBar->addWidget(m_clipButton);

  QPixmap pixmapErase(":images/EraserIcon.png");
  QIcon buttonIconErase(pixmapErase);

  m_eraseButton = new QPushButton(nullptr);
  m_eraseButton->setToolTip("Erase Console");
  m_eraseButton->setIcon(buttonIconErase);
  ui->mainToolBar->addWidget(m_eraseButton);


  QPixmap pixmapStats(":images/QuestionIcon.png");
  QIcon buttonIconStats(pixmapStats);
  m_statsButton = new QPushButton(nullptr);
  m_statsButton->setToolTip("Get Image Statistics");
  m_statsButton->setIcon(buttonIconStats);
  ui->mainToolBar->addWidget(m_statsButton);

  this->connect(m_clipButton, &QPushButton::clicked,
                this, &MainWindow::onClipdDtaFile);

  this->connect(m_eraseButton, &QPushButton::clicked,
                this, &MainWindow::onEraseConsol);

  this->connect(m_statsButton, &QPushButton::clicked,
                this, &MainWindow::onShowImageStats);
}



//------------------------------------------------------------------------------
void MainWindow::generateFipName()
{
  if (m_currentFile.isEmpty()) return;
  QString nameStub = m_currentFile.left(m_currentFile.length() - VOL_EXT_LENGTH);
  QString tail = m_currentFile.right(VOL_EXT_LENGTH);
  m_clipFile = nameStub + g_clipSuffix + tail;
  ui->lineEditOutputFile->setText(m_clipFile);
}

//------------------------------------------------------------------------------
void MainWindow::setWindowTitleWithCurrent()
{
  if (!m_currentFile.isEmpty())
  {
    QFileInfo testFile(m_currentFile);
    m_currentFileShortNanme = testFile.fileName();
  }
  else
  {
    m_currentFileShortNanme = "NO FILE";
  }
  QString aTitle = g_mainWindowTitle + m_currentFileShortNanme;
  this->setWindowTitle(aTitle);
}


//------------------------------------------------------------------------------
void MainWindow::onCloseApp()
{
  QApplication::closeAllWindows();
}

//------------------------------------------------------------------------------
void MainWindow::onOpenDataFile()
{
  QFileDialog loadDlg(this);
  QString nameFilter = ANALOGIC_VOLUME_FILE_NAME_FILTER;

  loadDlg.setAcceptMode(QFileDialog::AcceptOpen);
  loadDlg.setNameFilter(nameFilter);
  loadDlg.setDirectory(m_volumDataFolder);
  loadDlg.setMinimumWidth(900);
  loadDlg.setMinimumHeight(700);
  bool bRet = loadDlg.exec();
  if(bRet)
  {
    QStringList fileList = loadDlg.selectedFiles();
    if (fileList.size() > 0)
    {
      m_currentFile = fileList.at(0);
      this->generateFipName();
      this->setWindowTitleWithCurrent();
      QString message = "Loadef File: " + m_currentFile;
      m_console->logString(message);
    }
  }
}

//------------------------------------------------------------------------------
void MainWindow::onClipdDtaFile()
{
  m_clipFile = ui->lineEditOutputFile->text();
  if (m_currentFile.isEmpty() ||
      m_clipFile.isEmpty())
  {
    QMessageBox::warning(this,
                         "Error in Usage!",
                         "No input or output file specified.");
    return;
  }

  m_console->logString("Beginning data clip!");
  std::string inFile   = m_currentFile.toLocal8Bit().data();
  std::string clipFile = m_clipFile.toLocal8Bit().data();
  uint16_t  clipVal  = g_signalClipValue;

  bool flipResult  =  analogic::workstation::clipVolumeFileData(inFile,
                                                               clipFile,
                                                               clipVal);


  if (flipResult) m_console->logString("Data clip succesfull.");
  else m_console->logString("Data clip faild!");
}


//------------------------------------------------------------------------------
void MainWindow::onEraseConsol()
{
  m_console->clear();
}


//------------------------------------------------------------------------------
void MainWindow::onShowImageStats()
{
  if (m_currentFile.isEmpty())
  {
     QMessageBox::information(this,
                              "Informational Message",
                              "No input file currently sellected");
     return;
  }
  analogic::workstation::ImageStatistics imageStats;
  std::string inFile   = m_currentFile.toLocal8Bit().data();
  QString message  = "Reding Image Statistics for file: ";
  message += m_currentFile;
  m_console->logString(message);
  analogic::workstation::getVolumeFileStatistics(inFile, imageStats);

  message = "--  Image Statistic --:";
  m_console->logString(message);


  message = "image W: " + QString::number(imageStats.m_W);
  m_console->logString(message);

  message = "image H: " + QString::number(imageStats.m_H);
  m_console->logString(message);

  message = "image L: " + QString::number(imageStats.m_L);
  m_console->logString(message);


  message = "# of Pts: " + QString::number(imageStats.m_numberOfPoints);
  m_console->logString(message);


  message = "Min Val: " + QString::number(imageStats.m_minVal);
  m_console->logString(message);

  message = "Max Val: " + QString::number(imageStats.m_maxVal);
  m_console->logString(message);


  message = "Average: " + QString::number(imageStats.m_average);
  m_console->logString(message);



  message = "ClipVal: " + QString::number(imageStats.m_clipValue);
  m_console->logString(message);


  message = "# Above Clip: " + QString::number(imageStats.m_numberAboveClipValue);
  m_console->logString(message);


  message = "STD Dev: " + QString::number(imageStats.m_standardDeviation);
  m_console->logString(message);
}


