//------------------------------------------------------------------------------
// File: LogWindow.cpp
// Description: Log Window Implimentation
// (c) Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------
#include "LogWindow.h"

const int g_logWindowWidth  = 1100;
const int g_logWindowHeight = 600;


//------------------------------------------------------------------------------
LogWindow::LogWindow(QIcon& winIcon,
                     QIcon& clearIcon,
                     bool* parentClosed,
                     QWidget* parent):
    QWidget(parent),
    m_toolBar(nullptr),
    m_clearIconAction(nullptr),
    m_clearIcon(clearIcon),
    m_menuBar(nullptr),
    m_fileMenu(nullptr),
    m_actionMenu(nullptr),
    m_clearAction(nullptr),
    m_saveAction(nullptr),
    m_console(nullptr),
    m_plaintTextEdit(nullptr),
    m_defaultSavePath(""),
    m_saveEnabled(false),
    m_parentCloseFlag(parentClosed)
{
  this->setWindowIcon(winIcon);
  this->setWindowFlags(Qt::Window);
  this->setMinimumWidth(g_logWindowWidth);
  this->setMinimumHeight(g_logWindowHeight);

  this->createMenu();
  this->createToolBar();

  QVBoxLayout* mainLayout = new QVBoxLayout(nullptr);
  this->setLayout(mainLayout);
  m_plaintTextEdit = new QPlainTextEdit(nullptr);
  this->layout()->addWidget(m_menuBar);
  this->layout()->addWidget(m_toolBar);
  this->layout()->addWidget(m_plaintTextEdit);

  m_console = new ConsoleWidget(m_plaintTextEdit);
  m_console->setFixedFont(12);
  QString winTitle = this->windowTitle();
  winTitle += "VTK Volume Renderer - Log Console";
  this->setWindowTitle(winTitle);
  this->show();
}

//------------------------------------------------------------------------------
LogWindow::~LogWindow()
{
    delete m_console;
}

//------------------------------------------------------------------------------
void LogWindow::closeEvent(QCloseEvent* event)
{
  bool parentClosed = *m_parentCloseFlag;
  if (parentClosed)
  {
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

//------------------------------------------------------------------------------
void LogWindow::logString(const QString& logStr)
{
    m_console->logString(logStr);
}

//------------------------------------------------------------------------------
void LogWindow::clear()
{
  m_console->clear();
}

//------------------------------------------------------------------------------
void LogWindow::enableSave(const QString& savePath)
{
  m_defaultSavePath = savePath;
  m_saveEnabled     = true;
}

//------------------------------------------------------------------------------
void LogWindow::onClearConsole()
{
  m_console->clear();
}

//------------------------------------------------------------------------------
void LogWindow::onSafveToFile()
{
  if (!m_saveEnabled)
  {
    QMessageBox::information(this,
                             "Warning",
                             "Save not Enabled - init with LogWindow::enableSave(defaultSavepath).");
    return;
  }
  QFileInfo  saveFileInfo(m_defaultSavePath);
  QString    filePath = saveFileInfo.absolutePath();
  QFileDialog newFileDlg(this,
                         "Select Output File",
                         filePath,
                         CONSOLE_TEXT_FILE_FILTER);
  newFileDlg.setAcceptMode(QFileDialog::AcceptSave);

  bool dlgRet =  newFileDlg.exec();
  if (dlgRet)
  {
    QString saveFile = newFileDlg.selectedFiles().first();
    this->saveConsole(saveFile);
  }
}

//------------------------------------------------------------------------------
void LogWindow::createMenu()
{
  m_menuBar = new QMenuBar(this);
  m_fileMenu    =  m_menuBar->addMenu("&File");
  m_actionMenu  =  m_menuBar->addMenu("&Action");
  m_clearAction = new QAction(m_clearIcon,
                              "Clear",
                              nullptr);

  m_saveAction  = new QAction("Save to file...", nullptr);
  m_fileMenu->addAction(m_saveAction);
  m_actionMenu->addAction(m_clearAction);

  this->connect(m_clearAction, &QAction::triggered,
                this, &LogWindow::onClearConsole);

  this->connect(m_saveAction, &QAction::triggered,
                this, &LogWindow::onSafveToFile);
}

//------------------------------------------------------------------------------
void LogWindow::createToolBar()
{
  m_toolBar = new QToolBar(this);
  m_clearIconAction = new QAction(m_clearIcon, "", nullptr);
  m_toolBar->addAction(m_clearIconAction);
  this->connect(m_clearIconAction, &QAction::triggered,
                this,  &LogWindow::onClearConsole);
}

//------------------------------------------------------------------------------
void LogWindow::saveConsole(QString& saveFile)
{
  std::ofstream fileOut(saveFile.toLocal8Bit().data());
  QString sData = m_plaintTextEdit->toPlainText();
  std::string outStr = sData.toLocal8Bit().data();
  fileOut << outStr;
}
