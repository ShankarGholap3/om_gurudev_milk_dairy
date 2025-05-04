//------------------------------------------------------------------------------
// File: MainWindow.h
// Description: Mainwindow header file
// (c) Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef  TEST_ANALOGIC_WS_QTTDRTESTAPP_MAINWINDOW_H_
#define  TEST_ANALOGIC_WS_QTTDRTESTAPP_MAINWINDOW_H_
//------------------------------------------------------------------------------


#define TDR_FILE_FILTER      "SDICOS TDR file (*.tdr);;SDICOS TDR ii (*.dicos_tdr_1)"
#define TDR_FILE_EXTENSION_ii  ".dicos_tdr_1"

#define CONSOLE_TEXT_FILE_FILTER "Text File (*.txt)"

#include <SDICOS/SdcsDICOS.h>
#include <QMainWindow>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <vector>
#include <functional>
#include <fstream>
#include "VRUtils.h"
#include "ThreatDefs.h"
#include "ConsoleWidget.h"
#include "ThreatObject.h"
#include "ThreatVolume.h"
#include "InputDialog3D.h"

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
  void onConsoleClearButton();
  void onShowThreatInfo();
  void onReadTDRFromFile();
  void onSliceSliderValuechanged();
  void onSaveConsole();
  void onSaveTDRAs();
  void onMoveVolume();
  void onResizeVolume();
  void onAddTestThreat();
  void onThreatComboSlectionChanged(int selection);
  void onSetThreatType();
  void onVolumeSetMass();
  void onClearAllThreats();

private:
  void setupMenu();
  void setupToolBar();
  void setupSlider();
  void setupThreatCombo();
  void readTDRFile(QString filenameTDR);
  void writeAttributesFile();
  void showThreatInfo(int id, ThreatVolume& tv);
  void showBMP(int layer, SDICOS::Bitmap& bmp);
  int  bmpSliceFromSliderPosition();
  void saveConsoleToFile(QString& saveFile);
  bool saveTDRToFile(QString& saveFile);
  void moveTDR(int index, QVector3D& vecMove);
  void clearaAllThreats();
  void setTDRThreatType(int index,
                        analogic::workstation::ThreatAlarmType aType);
  void resizeTDR(int index, QVector3D& vecResize);
  void setTDRMass(int index);
  QString pickExtrudeDirection();
  void addThreatExtrudeAlognX(int curentIndex);
  void addThreatExtrudeAlognY(int curentIndex);
  void addThreatExtrudeAlognZ(int curentIndex);



  Ui::MainWindow*             ui;
  std::vector<ThreatVolume>   m_threatVolumeList;
  std::vector<std::reference_wrapper<SDICOS::Bitmap> > m_threatBMPList;
  std::vector<analogic::workstation::ThreatAlarmType> m_threatTypeList;
  std::vector<double>         m_massList;
  std::vector<QString>        m_threatStatus;
  ConsoleWidget*              m_console;
  SDICOS::TDRModule*          m_tdrModule;
  QComboBox*                  m_tdrListCombo;
  QImage*                     m_image;
  QPushButton*                m_clearButton;
  QLineEdit*                  m_tdrShortName;
};


//------------------------------------------------------------------------------
#endif   // TEST_ANALOGIC_WS_QTTDRTESTAPP_MAINWINDOW_H_
//------------------------------------------------------------------------------
