//------------------------------------------------------------------------------
// File: MainWindow.h
// Description: Header file  Main Window for testing integrated Volume Renderer
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_MAINWINDOW_H_
#define TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_MAINWINDOW_H_
//------------------------------------------------------------------------------

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTime>
#include <QMutex>
#include <QTimer>
#include <cstdio>
#include <cstdlib>
#include "vtkGarbageCollector.h"
#include "vtkDebugLeaks.h"
#include "BagQueueItem.h"
#include "VRUtils.h"
#include "MemUtils.h"
#include "Logger.h"
#include "VolumeRendererConfig.h"
#include "VolumeRendererWidget.h"
#include "DataManager.h"
#include "LogWindow.h"
#include "SelectFromListDialog.h"
#include "InputDialog3D.h"
#include "ConsoleWidget.h"



#define VOLUME_RENDERER_GUI_INI_FILE "IntegratedVolumeRenderer.ini"



#define DEF_PARAM_SHOW_THREATS                 true
#define DEF_PARAM_TRANSLUCENT_SHADING          true
#define DEF_PARAM_SURFACE_SHADING              true
#define DEF_PARAM_LINEAR_INTERPOL_RIGHT        false
#define DEF_PARAM_LINEAR_INTERPOL_LEFT         true
#define DEF_PARAM_SURFACE_ORG_REMOVAL          true
#define DEF_PARAM_SURFACE_INORG_REMOVAL        true
#define DEF_PARAM_SURFACE_METAL_REMOVAL        true
#define DEF_PARAM_TRANSLUCENT_ORG_REMOVAL      true
#define DEF_PARAM_TRANSLUCENT_INORG_REMOVAL    true
#define DEF_PARAM_TRANSLUCENT_METAL_REMOVAL    true
#define DEF_SCAN_DIR_LEFT_TO_RIGHT             true
#define DEF_RENDER_VTK_SHARPEN_SPLASH          false
#define DEF_RENDER_VTK_DYNAMIC_SHARPEN_SPLASH  false

#define DEF_RENDER_PARAM_TRANSLUCENT_AMBIENT_LIGHTING   0.10
#define DEF_RENDER_PARAM_TRANSLUCENT_DIFFUSE_LIGHTING   0.70
#define DEF_RENDER_PARAM_TRANSLUCENT_SPECULAR_LIGHTING  0.20
#define DEF_RENDER_PARAM_SURFACE_AMBIENT_LIGHTING       0.10
#define DEF_RENDER_PARAM_SURFACE_DIFFUSE_LIGHTING       0.70
#define DEF_RENDER_PARAM_SURFACE_SPECULAR_LIGHTING      0.20




#define DEF_TDR_EXTENSION ".dicos_tdr_1"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  virtual void closeEvent(QCloseEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);

public slots:
  void onAppClose();
  void onLoadComplete();
  void onResetView();
  void onSetRightAsThreat();
  void onPanRightView();
  void onResetRightRenderer();
  void onCenterRightViewer();
  void onShowSplash();
  void onVRWidgetGenericMessage(int& msgID);
  void onVRWidgetPositionMessage(int& msgID,
                                 QVector3D& aPoint);

  void onVRWidgetTimerMessageLeft(int& msgID,
                              int& timeVal);

  void onVRWidgetTimerMessageRight(int& msgID,
                              int& timeVal);

  void onRubberBandPointsSelected(QVector2D& p0,
                      QVector2D& p1);


  void onThreatOnly();
  void onZoomIn();
  void onZoomOut();
  void onClearScene();
  void onExportFullTFs();
  void onNewBag();
  void onReloadBag();
  void onReloadBagQuestion();
  void onShowCameraInfo();
  void onLinkUnlinkviewers();
  void onLoadNewFile();
  void onClearLog();
  void onCropSlidervalueChanged();
  void onCropRadoXChecked();
  void onCropRadoYChecked();
  void onCropRadoZChecked();
  void onNextBag();
  void onCycleBagLoad();
  void onOperatorThreatCancel();
  void onOperatorThreatSelectrubberband();
  void onOperatorThreatSelectComplete();
  void onReducePickSize();
  void onDeleteOperatorThreat();
  void onIncreasePickSize();
  void onShowAllMaterisls();
  void onShowMetalOnly();
  void onShowOrganicOnly();
  void onRemoveColor();
  void onInvertScene();
  void onSetRightViewPhotographic();
  void onBagLoadTimerUpdate();
  void onEditCropROI();
  void onThreatComboSlectionChanged(int curSel);
  void onClearThreat();
  void onSuspectThreat();
  void onApplyVolumePropertyLeft();
  void onApplyVolumePropertyRight();
  void onShowLogWindow();
  void onHideLogWindow();
  void onShowVolumeBounds();
  void onLeakCheck();
  void onExportThreatData();
  void onThreatWindowsPrintSelf();
  void onShowCurrentThreatVolume();
  void onTestTransformCamera();
  void onInsertVTKLabelText();
  void onTestZoomValue();
  void onTestViewMode();
  void onStashCurrentCamera();
  void onRestoreStashCamera();
  void onResetThreatWindow();
  void onTestWindowLevel();
  void onTestUpdateRate();
  void onTestColorLevel();
  void onTestImageOverlay();
  void onStartLeakCheck();
  void onPauseLeakCheck();
  void onCycleThreat();
  void onViewModeThreat();
  void onViewModeSurface();
  void onViewModeLaptop();

private:
  bool getRenderSettingsFromIni();
  void setupMenu();
  void setupToolbar();
  void setupOperatorSelect();
  void setupSceneSettingsSelection();
  void setupBagQueue();
  void showBagQueueInfo();
  void setupThreatCombo();
  void setupModeRadio();
  void setRadioselectionStatus(VREnums::VRViewModeEnum aMode);
  VREnums::VRViewModeEnum getCurrentViewMode();
  void setupLogWindiow();
  void readBagQueue(QString& queueFile);
  void getTdrListFromVolumeName(std::list<QString>& tdrList, QString volFilename);
  void populateAndInit(DataManager* pDataManager);
  void showFirstThreat();
  void setButtonImages();
  void setButtonImageAndText(QPushButton* pButton,
                             QString imageSource,
                             QString buttonTip,
                             int buttonWidth);
  void linkVtkViewers(bool doLink);
  void showColorLevel(float colLevel);
  void showWindowLevel(float winLevel);
  void setCropStatus();
  void setLeakCheckStatus();
  void showColorRenderStatus();
  void showMenuStatus(QAction* menuItem,
                      bool bStatus);
  void initUIComponents();
  bool initSettingsPaths();
  void setWindowTileWithDataFile(QString dataFile);
  void setTDRLabel(QString tdrFileStr);
  void setCroFileFromDataFile(QString& shortName);
  void populateDICOSCtObjct(DataManager* pDataManager,
                            SDICOS::CTModule& ctModule);

  void setComboHeader();
  void setAllComboData();
  void setupComboList();
  void setupViewModeThreatLists();
  bool getRelatedFilesFromVolumeFile(const QString volumeFile,
                               QString& dicosTdrFile,
                               QString& dicosCtFile,
                               QString& propFile);
  void populateThreatCombo();

  /**
   * @brief viewThreat
   * @param threatIndex  this is 1 based.
   */
  void viewThreat(int comboIndex);
  void updateCombo(int newIndex);

  void clearThreatCombo();
  void resetCropwidget();
  void loadNewBag(QString& pathToBagData,
                  std::list<QString>& tdrList);

  void doAppendToTextLabel();
  void cycleLoadWriteHeader();
  void cycleLoadWriteData();
  void appenStringToFile(QString& filename,
                         QString& str);

  void consoleLogString(const QString& logStr);


  analogic::workstation::VolumeRendererWidget* m_volumeRendererTrancelucentView;
  analogic::workstation::VolumeRendererConfig* m_rendererConfigLeft;

  //----------------------------------------------------------------------
  analogic::workstation::VolumeRendererWidget* m_volumeRendererOpaqueView;
  analogic::workstation::VolumeRendererConfig* m_rendererConfigRight;

  QMutex*                                      m_renderMutex;
  QTime                                        m_mainTimer;


  //-------------------------------------------------
  // Bag Queue
  std::vector<BagQueueItem>                    m_bagQueue;

  int                                          m_bagQueueIndex;
  QString                                      m_bagTag;
  QTimer*                                      m_bagLoadTimer;
  bool                                         m_autoLoadEnabled;
  int                                          m_loadCount;

  //--------------------------------------------------
  QComboBox*                                   m_threatCombo;
  QLineEdit*                                   m_lineEditTDR;
  std::list<ThreatObject*>                     m_threatObjectlLst;

  std::vector<int>                             m_machineOperatorThreatVec;
  int                                          m_machineOperatorIndex;

  std::vector<int>                             m_laptopThreatVec;
  int                                          m_laptopIndex;


  //-------------------------------------------------
  // Window and Color levels
  QLineEdit*                                   m_windowLevelLineEdit;
  QLineEdit*                                   m_colorLevelLineEdit;
  QLineEdit*                                   m_bagQueueStatusLineEdit;

  QPushButton*                                 m_clearConsole;
  QPushButton*                                 m_loadBagButton;
  QPushButton*                                 m_cycleLoadButton;
  QLabel*                                      m_leakCheckIcon;
  QLabel*                                      m_cropIcon;
  QPushButton*                                 m_reloadButton;
  //--------------------------------------------------
  // Internal Data direm_cropStatusctory and File
  QString                                      m_internalDataDirectory;
  QString                                      m_internalTDRDirectory;
  QString                                      m_currentDataFile;
  QString                                      m_currentTDRFile;
  DataManager*                                 m_dataManager;
  QString                                      m_cycleLoadLogFile;

  //--------------------------------------------------
  // Cropping if used
  analogic::workstation::CropCorners m_cropCorners;
  QString                            m_cropFile;

  //--------------------------------------------------
  // Filepaths for the volumerenderer Config class.
  std::string              m_mapFileDir;
  std::vector<std::string> m_normalOpacityFileList;
  std::vector<std::string> m_normalHSVColorMapFileList;
  std::vector<std::string> m_surfaceOpacityFileList;
  std::vector<std::string> m_surfaceHSVColorMapFileList;

  std::string    m_grayscaleValuesAndOpacitiesFelePath;
  std::string    m_materialTransitionFilePath;
  std::string    m_internalSettingsFilePath;
  std::string    m_tdrFilepath;
  std::string    m_sdicosLogFile;

  //-------------------------------------------------
  // INI config setting
  bool m_useShadingForLeftViewer;
  bool m_useShadingForRightViewer;
  bool m_linearInterpolationForLeftViewer;
  bool m_linearInterpolationForRightViewer;
  bool m_leftOrganicRemovable;
  bool m_leftInorganicRemovable;
  bool m_leftMetalRemovable;
  bool m_rightOrganicRemovable;
  bool m_rightInorganicRemovable;
  bool m_rightMetalRemovable;

  double m_translucentAmbientLighting;
  double m_translucentDiffuseLighting;
  double m_translucentSpecularLighting;
  double m_surfaceAmbientLighting;
  double m_surfaceDiffuseLighting;
  double m_surfaceSpecularLighting;

  bool m_scanDirLeftToRight;
  bool m_vtkSharpenSplash;
  bool m_vtkDynamicSharpenSplash;
  //--------------------------------------------------
  float                 m_currentZoom;
  double                m_currentOpacity;
  bool                  m_enableThreatHandling;
  bool                  m_doInversecolor;
  bool                  m_doColorRender;
  bool                  m_enableLaptopRemoval;
  bool                  m_syncViewers;
  uint16_t*             m_bagDataBufferLeft;
  uint16_t*             m_bagDataBufferRight;
  bool                  m_cropStatus;
  bool                  m_leakCheckStatus;
  LogWindow*            m_logWindow;
  bool                  m_canClose;

  //----------------------------
  // Operator threat selection
  bool                  m_operatorThreatSelectionArmed;
  QVector3D             m_operatorThreatP0;
  QVector3D             m_operatorThreatExtent;

  //---------------------------
  // Right viewer mode
  VREnums::VRViewModeEnum m_rightviewMode;

  //-------------------------------------------------
  // Current volume properties
  int                  m_currentImageWidth;
  int                  m_currentImageHeight;
  int                  m_currentNumSlices;



  Ui::MainWindow*       ui;
};

//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_MAINWINDOW_H_
//------------------------------------------------------------------------------
