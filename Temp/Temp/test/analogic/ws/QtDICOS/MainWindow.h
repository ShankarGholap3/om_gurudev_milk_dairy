//------------------------------------------------------------------------------
// File: MainWindow.h
// Description: Main Window header
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_QTDICOS_MAINWINDOW_H_
#define TEST_ANALOGIC_WS_QTDICOS_MAINWINDOW_H_
//------------------------------------------------------------------------------

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>
#include <string>
#include "ReadDICOSHeader.h"
#include "Logger.h"
#include "VRUtils.h"
#include "ConsoleWidget.h"
#include "AboutDialog.h"

#define DICOS_FILE_FILTER "DICOS File (*.dicos_ct_vol)"

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
  void onCloseApp();
  void onAbout();
  void onOpenDICOSFile();
  void onWriteDICOSFile();
  void onGenFromDataFile();

private:
  void setupToolBar();
  void setButtonImageAndText(QPushButton* pButton,
                                QString imageSource,
                                QString buttonText,
                                int buttonWidth,
                                int buttonHeight);

  void showCtHeaderInfo(SDICOS::CTModule& ctModule);


  QLineEdit*      m_nameOut;
  QLineEdit*      m_bagIdOut;
  QLineEdit*      m_machineIdOut;
  QComboBox*      m_machineType;
  ConsoleWidget*  m_console;
  Ui::MainWindow* ui;
};


//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_QTDICOS_MAINWINDOW_H_
//------------------------------------------------------------------------------
