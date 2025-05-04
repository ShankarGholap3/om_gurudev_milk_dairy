//------------------------------------------------------------------------------
// File: MainWindow_reader.h
// Description:  Main Window header for reader
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_MAPTUNER_MAINWINDOW_READER_H_
#define TEST_ANALOGIC_WS_MAPTUNER_MAINWINDOW_READER_H_
//------------------------------------------------------------------------------


#include <QMainWindow>
#include <QDebug>
#include "ConsoleWidget.h"
#include "TunerIPC.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void onAppExit();
  void onMessageReceived(int msgID);


private:
  void setupMenu();
  void startIPC();


  ConsoleWidget*                   m_console;
  std::thread*                     m_readerThread;
  bool                             m_keepRunning;
  analogic::workstation::TunerIPC* m_readerIPC;
  Ui::MainWindow*                  ui;
};


//------------------------------------------------------------------------------
#endif  //   TEST_ANALOGIC_WS_MAPTUNER_MAINWINDOW_READER_H_
//------------------------------------------------------------------------------
