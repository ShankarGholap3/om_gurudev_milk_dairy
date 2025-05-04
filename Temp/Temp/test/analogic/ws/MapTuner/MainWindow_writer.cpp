//------------------------------------------------------------------------------
// File: MainWindow_writer.cpp
// Description: Main Window Implemenetation for Writer
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------

#include "MainWindow_writer.h"
#include "ui_MainWindow_writer.h"

QString g_appTitle = "Map Writer - Client App (Map Tuner)";



//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_console(nullptr),
  m_writerThread(nullptr),
  m_writerIPC(nullptr)
{
  ui->setupUi(this);
  m_console = new ConsoleWidget(ui->plainTextEdit);
  this->setupMenu();
  this->setWindowTitle(g_appTitle);
  m_console->logString("Client Up OK.");
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
void MainWindow::onStartWriterThread()
{
  this->startIPC();
}

//------------------------------------------------------------------------------
void MainWindow::onMessageEvent(int msgID)
{
   analogic::workstation::MAP_TUNE_MESSAGE ID =(analogic::workstation::MAP_TUNE_MESSAGE )msgID;

   QString strMessage = m_writerIPC->findMessage(ID);
   m_console->logString(strMessage);
}

//------------------------------------------------------------------------------
void MainWindow::setupMenu()
{
  this->connect(ui->actionExit, &QAction::triggered,
                this, &MainWindow::onAppExit);

  this->connect(ui->actionStartWriterThread, &QAction::triggered,
                this, &MainWindow::onStartWriterThread);
}


//------------------------------------------------------------------------------
void MainWindow::startIPC()
{
  m_writerIPC = new analogic::workstation::TunerIPC(analogic::workstation::WIRTER_CLIENT);
  this->connect(m_writerIPC, &analogic::workstation::TunerIPC::messageRecieved,
                this, &MainWindow::onMessageEvent);

  m_writerIPC->Start();
}



