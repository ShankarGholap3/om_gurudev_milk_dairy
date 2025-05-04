//------------------------------------------------------------------------------
// File: MainWindow_writer.h
// Description:  Main Window header for Writer
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_MAPTUNER_MAINWINDOW_WRITER_H_
#define TEST_ANALOGIC_WS_MAPTUNER_MAINWINDOW_WRITER_H_
//------------------------------------------------------------------------------
#include <QMainWindow>
#include <QDebug>
#include <QStringList>
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
  void onStartWriterThread();
  void onMessageEvent(int msgID);

private:
  void setupMenu();
  void startIPC();

  ConsoleWidget*                   m_console;
  std::thread*                     m_writerThread;
  bool                             m_keepRunning;
  analogic::workstation::TunerIPC* m_writerIPC;
  Ui::MainWindow*  ui;
};


//------------------------------------------------------------------------------
#endif  //   TEST_ANALOGIC_WS_MAPTUNER_MAINWINDOW_WRITER_H_
//------------------------------------------------------------------------------
