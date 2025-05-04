//------------------------------------------------------------------------------
// File: MainWindow.h
// Description: Main Window Header
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_FLIPBAG_MAINWINDOW_H_
#define TEST_ANALOGIC_WS_FLIPBAG_MAINWINDOW_H_
//------------------------------------------------------------------------------


#include <QMainWindow>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include "ConsoleWidget.h"
#include "VRUtils.h"

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
  void onOpenDataFile();
  void onFlipdDtaFile();
  void onEraseConsol();
  void onShowImageStats();


private:
  void setWindowTitleWithCurrent();
  void setupMenu();
  void setupToolbar();
  void generateFipName();

  QString           m_currentFileShortNanme;
  QString           m_currentFile;
  QString           m_flipFile;
  QString           m_volumDataFolder;

  QPushButton*      m_flipButton;
  QPushButton*      m_eraseButton;
  QPushButton*      m_statsButton;
  ConsoleWidget*    m_console;

  Ui::MainWindow*   ui;
};

//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_FLIPBAG_MAINWINDOW_H_
//------------------------------------------------------------------------------
