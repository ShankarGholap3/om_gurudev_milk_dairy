//------------------------------------------------------------------------------
// File: MainWindow.h
// Description: Main Window header file
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_QTVTKVOLUMERENDERER_MAINWINDOW_H_
#define TEST_ANALOGIC_WS_QTVTKVOLUMERENDERER_MAINWINDOW_H_
//------------------------------------------------------------------------------
#include <QVTKOpenGLWidget.h>
#include <QTime>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPushButton>
#include <QLineEdit>
#include <vector>
#include <algorithm>
#include <vtkMatrix4x4.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkCubeSource.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKWidget.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkImageClip.h>
#include <vtkBoxClipDataSet.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include "OpenGLVolumeWidget.h"
#include "LocalVRUtils.h"



//-----------------------------------------------
// Normal Background Colors For Main Render Window
#define VOLUME_BG_DEFAULT_RED    0.94
#define VOLUME_BG_DEFAULT_GREEN  0.94
#define VOLUME_BG_DEFAULT_BLUE   0.94
//-----------------------------------------------

//-----------------------------------------------
// Inverse Background Colors For Main Render Window
#define VOLUME_BG_INVERSE_RED    0.03
#define VOLUME_BG_INVERSE_GREEN  0.03
#define VOLUME_BG_INVERSE_BLUE   0.03
//-----------------------------------------------


#define DEFAULT_BAG_QUEUE_FILE "BagQueue.txt"

#define DEFAULT_BAG_WIDTH     630
#define DEFAULT_BAG_HEIGHT    430
#define DEFAULT_BAG_LENGTH    513


#define VR_WIDGET_DEF_WIDTH  812
#define VR_WIDGET_DEF_HEIGHT 812

#define TOOLBAR_SEPERATOR_WIDTH 120


#define CURRENT_FILE_LINE_EDIT_WIDTH 400


typedef OpenGLVolumeWidget VKVTVolumeWidget_Def;


#define BAG_QUEUE_COMMENT_CHAR '#'
#define SETUP_DELIMETER        "="

#define BAG_PATH_STRING "BagDataPath"

enum vtkInterpolationType
{
  INTERPOLATE_NEAREST = 0,
  INTERPOLATE_LINEAR  = 1,
  INTERPOLATE_CUBIC   = 2,
};




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QString& pathTooBagList,
                      QWidget *parent = 0);
  ~MainWindow();

public slots:
  void onHelpAbout();
  void onCloseApp();
  void onBeginRendering();
  void onNextBag();
  void onPreviousBag();
  void onLoadVolumeFromFile();

  //---------------------------------------
  // View Mode slots
  void onViewAllMaterials();
  void onViewOrganicMaterials();
  void onViewInOrganicMaterials();
  void onViewMetalMaterials();
  void onViewGrayScale();
  void onViewInverseBackground();
  void onViewLowDensityLeft();
  void onViewCameraRight();
  void onResetViewAlignment();


  // Testing in volume measuring ruler.
  void onMeasureRulerClick();
  void showRulerPosition();




private:
  void getDefaultBagFilePath(QString& bagFolder);
  bool readSetupFile(const QString& setupFile,
                     QString& bagdataFolder);
  QPushButton* createToolBarButton(QString strIconName,
                                   QString toolTip,
                                   bool checkable);
  void setupMenu();
  void setupToolbar();
  void showCurrentFile();
  void removeVolume();


  bool readTransitionFile(QString materialTransitionFilePath,
                           std::map<MaterialTransition_E, double>& transitionMap);

  bool readGrayScaleSettings(QString filePath,
                             GrayscaleValueOpacityStruct& grayScaleSettings);

  void setRightOpacitySignalLevel(int percentSet);
  double scaleSignalValue(int pcntChange, double setPoint);

  void hsvOfOrganicGray(double signalValue,
                        double& h,
                        double& s,
                        double& v);

  void hsvOfInorganicGray(double signalValue,
                        double& h,
                        double& s,
                        double& v);

  void hsvOfMetalGray(double signalValue,
                        double& h,
                        double& s,
                        double& v);

  void clampHSV(double& h, double& s, double& v);

  QString transitionName(MaterialTransition_E transitionE);

  void initHSVColorTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTF,
                                                           std::list<HSVColorSigVal>& hsvColorList);

  void initOpacityTransferFunction(vtkSmartPointer<vtkPiecewiseFunction> opacityTF,
                                   std::list<OpacitySigVal>& opasityLis);

  void initBWTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTF,
                                           std::list<HSVColorSigVal>& hsvColorList);

  void setMateralVisibility();

  VKVTVolumeWidget_Def* renderVolumeData(QWidget* parent,
                                         bool isLeftWidget,
                                         std::list<HSVColorSigVal>& HSVColorList,
                                         std::list<OpacitySigVal>& OpacityList,
                                         bool useShading,
                                         vtkInterpolationType interpType);

  bool readBagQueue();
  void initBagInputs();
  void loadBagData();
  bool readFileData(QString& volumeFile);
  void setupYFlipMatrix();


  // Configuration instace variables
  bool                            m_haveBagQueue;
  QString                         m_bagFilePath;
  QString                         m_resourceDirectory;
  QString                         m_bagFile;
  QString                         m_propertiesFile;
  QString                         m_bagListFile;
  std::vector<QString>            m_bagQueueList;
  int                             m_bagQueueIndex;

  analogic::test_vr::ImageProperties* m_pIP;
  QVector3D                       m_roiStart;
  QVector3D                       m_roiEnd;
  bool                            m_dataIsLoaded;
  uint16_t*                       m_ptrRawdata;
  vtkSmartPointer<vtkMatrix4x4>   m_flipYMatrix;

  std::list<HSVColorSigVal>       m_translucentHSVColorList;
  std::list<OpacitySigVal>        m_translucentOpacityList;
  std::list<HSVColorSigVal>       m_surfaceHSVColorList;
  std::list<OpacitySigVal>        m_surfaceOpacityList;
  std::list<OpacitySigVal>        m_currentOpacityList;

  // Low Density (left only)
  std::list<HSVColorSigVal>       m_lowDensityTranslucentHSVColorList;
  std::list<OpacitySigVal>        m_lowDensityTranslucentOpacityList;

  // Photographis Right
  double                          m_opacityModifyScalar;


  bool                            m_translucentUseShading;
  bool                            m_surfaceUseShading;

  VKVTVolumeWidget_Def*           m_volumeWidgetTranslucent;
  VKVTVolumeWidget_Def*           m_volumeWidgetSurface;

  vtkVolumeProperty*              m_volumeProprtyRefRight;
  vtkVolumeProperty*              m_volumeProprtyRefLeft;

  bool                            m_organicRemovable;
  bool                            m_inorganicRemovable;
  bool                            m_metalRemovable;
  bool                            m_inverseBackground;
  bool                            m_renderWithColor;
  bool                            m_doColorizeOrganic;
  bool                            m_doColorizeInorganic;
  bool                            m_doColorizeMetal;
  bool                            m_doColorizeThreat;

  GrayscaleValueOpacityStruct     m_grayscaleStruct;

  std::map<MaterialTransition_E, double>  m_materialTransitionMap;


  double                          m_totalRenderTime;

  //--------------------------------------------------
  // View mode buttons:
  QPushButton*                m_buttonAllMaterials;
  QPushButton*                m_buttonOrganic;
  QPushButton*                m_buttonInOrganic;
  QPushButton*                m_buttonMetal;
  QPushButton*                m_buttonGrayScale;
  QPushButton*                m_buttonInverserBG;
  QPushButton*                m_buttonLowDensity;
  QPushButton*                m_buttonCamera;


  QPushButton*                m_buttonNextBag;
  QPushButton*                m_buttonPreviousBag;
  QPushButton*                m_buttonResetView;
  QPushButton*                m_testFilter;
  //---------------------------------------

  //---------------------------------------
  bool                        m_measureRulerActive;
  QVector3D                   m_measureP0;
  QVector3D                   m_measureP1;
  //---------------------------------------


  //--------------------------
  // Ui stuff
  Ui::MainWindow* ui;
};

//------------------------------------------------------------------------------
#endif  //  TEST_ANALOGIC_WS_QTVTKVOLUMERENDERER_MAINWINDOW_H_
//------------------------------------------------------------------------------
