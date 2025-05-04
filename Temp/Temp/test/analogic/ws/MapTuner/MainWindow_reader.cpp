//------------------------------------------------------------------------------
// File: MainWindow_reader.cpp
// Description: Main Window Implemenetation for Reader
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------

#include "MainWindow_reader.h"
#include "ui_MainWindow_reader.h"

QString g_appTitle = "Map Reader - Server App (Workstation)";



//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_console(nullptr),
  m_readerThread(nullptr),
  m_keepRunning(false),
  m_readerIPC(nullptr)
{
  ui->setupUi(this);
  m_console = new ConsoleWidget(ui->plainTextEdit);
  this->setupMenu();
  this->setWindowTitle(g_appTitle);
  m_console->logString("Server Up OK.");
  this->startIPC();
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete ui;
}

//------------------------------------------------------------------------------
void MainWindow::onAppExit()
{
  this->close();
}

//------------------------------------------------------------------------------
void MainWindow::setupMenu()
{
  this->connect(ui->actionExit, &QAction::triggered,
                this, &MainWindow::onAppExit);
}


//------------------------------------------------------------------------------
void MainWindow::onMessageReceived(int msgID)
{
  analogic::workstation::MAP_TUNE_MESSAGE ID =(analogic::workstation::MAP_TUNE_MESSAGE )msgID;

  QString strMessage = m_readerIPC->findMessage(ID);
  m_console->logString(strMessage);
}

//------------------------------------------------------------------------------
void MainWindow::startIPC()
{
  m_readerIPC = new analogic::workstation::TunerIPC(analogic::workstation::READER_SERVER);



  this->connect(m_readerIPC, &analogic::workstation::TunerIPC::messageRecieved,
                this, &MainWindow::onMessageReceived);

  m_readerIPC->Start();
}


