//-----------------------------------------------------------------------------
// File: VolumeRendererWidget.h
// Description: Header file for Volume Renderer Widget class
// Copyright 2016 Analogic Corp.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERWIDGET_H_
#define ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERWIDGET_H_
//-----------------------------------------------------------------------------
#include <QLabel>
#include <QGridLayout>
#include <QDebug>
#include <QMutex>
#include <QPixmap>
#include <QTimer>
#include <QVTKOpenGLWidget.h>
#include <SDICOS/SdcsTemplateCT.h>
#include <SDICOS/SdcsSopClassUID.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkCallbackCommand.h>
#include <vtkDebugLeaks.h>
#include <vtkImageMapper.h>
#include <vtkImageActor.h>
#include <vtkActor2D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkWidgetEvent.h>
#include <vtkImageMathematics.h>
#include <vtkImageShiftScale.h>
#include <vtkQImageToImageSource.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageReader.h>
#include <vtkPNGWriter.h>
#include <vtkDistanceWidget.h>
#include <vtkDistanceRepresentation.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkDistanceRepresentation3D.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkAxisActor.h>
#include <vtkHandleRepresentation.h>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <utility>

#include "VolumeRendererConfig.h"
#include "VolumeRendererInterface.h"
#include "VolumeRendererBase.h"
#include "VolumeRenderer.h"
#include "OpenGLVolumeWidget.h"
#include "MarginWidget.h"
#include "VRUtils.h"
#include "TouchDistanceWidget.h"

typedef uint64_t vtk_callback_ulong;


//------------------------------------------------------------------------------
// begin analogic::workstation namespace
//------------------------------------------------------------------------------
namespace analogic
{

namespace workstation
{


#define VTK_RENDERED_BAG_ID        "rendererBagId"
#define VTK_RENDERED_THREAT_TEXT   "rendererThreatType"
#define VTK_RENDERED_THREAT_ID     "rendererThreatId"
#define VTK_RENDERED_RFID          "rendererRFID"
#define VTK_RENDERED_DATE_TIME     "rendererDateTime"

#define VTK_RFID_LABEL_TAG         "RFID: "

#define VTK_DEFAULT_ZOOM 1

#define DEFAULT_VTK_UPDATE_RATE  15.0
#define DESIRED_VTK_UPDATE_RATE   4.0

#define ENABLE_DYNAMIC_SPLASH false

#define DYNAMIC_SPLASH_FILTER_SCALE 0.15

#define RIGHT_THREAT_MAX_ZOOM           3.1

#define RIGHT_THREAT_VIEW_OFFSET_SCALE  5.0

#define RIGHT_SLAB_THREAT_ZOOM            3.763

#define DISABLE_VOLUME_SPLASH_TEXT        "X"
#define DISABLE_VOLUME_SPLASH_POS_X       162
#define DISABLE_VOLUME_SPLASH_POS_Y       -62
#define DISABLE_VOLUME_SPLASH_HEIGHT      800
#define DISABLE_VOLUME_SPLASH_COLOR_RED   0.90
#define DISABLE_VOLUME_SPLASH_COLOR_GREEN 0.01
#define DISABLE_VOLUME_SPLASH_COLOR_BLUE  0.01
#define OPERATOR_THREAT_MIN_SIZE_PIXELS   5.0

//---------------------------------------------------------------
// In Volume Distance Measurement:
#define MEASURE_RULER_HANDEL_SIZE_POINT     40
#define MEASURE_RULER_HANDEL_SIZE_SPHERE    10

#define MEASURE_RULER_TEXT_COLOR_RED        0.19
#define MEASURE_RULER_TEXT_COLOR_GREEN      0.78
#define MEASURE_RULER_TEXT_COLOR_BLUE       0.30

#define MEASURE_RULER_LINE_COLOR_RED        0.19
#define MEASURE_RULER_LINE_COLOR_GREEN      0.88
#define MEASURE_RULER_LINE_COLOR_BLUE       0.88


#define MEASURE_RULER_HANDLE_COLOR_RED      0.84
#define MEASURE_RULER_HANDLE_COLOR_GREEN    0.10
#define MEASURE_RULER_HANDLE_COLOR_BLUE     0.10


#define MEASURE_RULER_GLYPH_COLOR_RED      0.95
#define MEASURE_RULER_GLYPH_COLOR_GREEN    0.10
#define MEASURE_RULER_GLYPH_COLOR_BLUE     0.10

//---------------------------------------------------------------
// This is in screen units (PIXELS)
#define TOUCH_MEASURE_HANDLE_PROXIMITY            15

//---------------------------------------------------------------
// These are in real units volume distance.
#define TOUCH_MEASURE_RESET_SEPERATION_MINIMUM    3.0
#define TOUCH_MEASURE_RESET_DISTANCE              20.0
//---------------------------------------------------------------


#define OUNCES_PER_GRANM 0.035274

//---------------------
// Image Debugging:
#define DEFAULT_VOL_IMAGE_FILE "/home/analogic/RawImageData.vol"
#define DEFAULT_VOL_PROP_FILE  "/home/analogic/RawImageData.properties"
//---------------------

enum VolumeComputeMethod
{
  FLYING_EDGES   = 0,
  MARCHING_CUBES = 1,
  SDICOS_BITMAP  = 3,
};



class VolumeRendererWidget : public OpenGLVolumeWidget, public VolumeRendererInterface
{
  Q_OBJECT

  //-------------------------------------------------------------------------
  // Properties put in to help Squish Automated test to verify rendered text
  Q_PROPERTY(QString rendererBagId READ rendererBagId)
  Q_PROPERTY(QString rendererDateTime READ rendererDateTime)
  Q_PROPERTY(QString rendererThreatType READ rendererThreatType)
  Q_PROPERTY(QString rendererThreatId READ rendererThreatId)
  Q_PROPERTY(QString rendererRFID READ rendererRFID)
  //-------------------------------------------------------------------------

public:
  explicit VolumeRendererWidget(QLabel* parent);

  //-----------------------------------------
  // Overrides of the VolumeRendererInterface
  ~VolumeRendererWidget() override;

  void Init(SDICOS::CTModule* ctModule,
              uint16_t* buffer,
              const QVector3D& volDimensions,
              const QVector3D& roiStart,
              const QVector3D& roiEnd,
              std::vector<SDICOS::TDRModule*> tdrs,
              const std::vector< std::pair<int, std::string> >&  vecDisplayThreatIdentifier,
              analogic::workstation::VolumeRendererConfig* configObject) override;


  QString rendererBagId() const { return m_prop_bagId; }
  QString rendererDateTime() const { return m_prop_DateTime; }
  QString rendererThreatType() const { return m_prop_ThreatType; }
  QString rendererThreatId() const { return m_prop_ThreatId; }
  QString rendererRFID()  const {return m_prop_RFID; }


  void SetContrast(float value) override;

  void SetBrightness(float value) override;

  void SetOpacity(float value) override;

  void SetInverse(bool value) override;

  void RenderWithColor(bool doColor) override;

  void SetHighlightedMaterials(std::vector<MaterialEnumeration> list) override;

  void SetMaterialFilterCutoff(double material_cutoff) override;

  bool GetMaterialFilterActivityStatus() override;

  void SetMaterialFilterActivityStatus(bool setVal) override;

  void EnableLowDensity(bool setVal) override;

  void SetZoom(float value) override;

  void SetThreatVisibility(SDICOS::TDRModule *pTdr,
                           int rawIndex,
                           bool visible,
                           bool keepOrientation,
                           int  threatTypeIndex,
                           int  threatTypeTotal) override;

  void ResetParameters() override;

  void ResetDisplay(bool dosync) override;

  void RemoveCurrentBag() override;

  void RemoveSplashIfPresent() override;

  void SetLinkSiblingsList(std::list<VolumeRendererWidget*> sibList) override;

  void SetSyncEnabled(bool enableStatus) override;

  void SetScannerDirectionLeftToRight(bool scanLeftToRight) override;

  void DisplayThreatOnly(bool  showBMPOnly) override;

  int ThreatCount() override;

  int CurrentThreat() override;

  void ClearCurrentThreat() override;

  void SuspectCurrentThreat(int threatTypeIndex,
                            int threatTypeTotal) override;

  void InitPickBoxWidget() override;

  void StartOperatorThreatGeneration() override;

  int RightAcceptOperatorGeneratedThreat(QString& threatType,
                                         QVector3D& p0Ret,
                                         QVector3D& extentRet) override;

  void CancelOperatorThreatGeneration() override;

  void ExpandOperatorThreatVolume() override;

  void ShrinkOperatorThreatVolume() override;

  int NextThreat(int index) override;

  void SetDisplayLabel(const QString displayLab);

  void ShowViewLabel(bool doShow) override;

  void ShowThreatView(bool doShow) override;

  void SetEnableSplashScreen(bool doSplash) override;

  void SetEnableDynamicSplash(bool doDynamicSplash) override;

  void SetLightingParameters(double sceneAmbient,
                             double sceneDiffuse,
                             double sceneSpecular) override;

  void AppendDisplayText(QStringList& list) override;

  void ShowThreatUserDecision(QString& userDecision) override;

  void SetEnableLaptopRemoval(bool doEnable) override;

  void SetSlabThickness(double thickness) override;

  void SetSlabUnpackPosition(double value,
                            VREnums::AXIS axis,
                            bool isSlab) override;

  void SetSlabViewType(VREnums::SlabViewType newSetSlabType) override;


  void SetCutViewAxis(VREnums::AXIS axis) override;

  void SetAsThreatRenderer(bool setVal) override;

  void SetRightViewerDisplayMode(VREnums::VRViewModeEnum aMode) override;

  void SetVTKDisplayUnit(VREnums::DispalyUnitEnum dispUnit) override;

  void SetVTKDisplayUnitSystem(VREnums::DispalyUnitSystemEnum unitSystem) override;

  void DeleteOperatorThreat(int threatIndex) override;

  void RepaintViewers() override;

  void UpdateRenderer() override;

  void SuspectBag() override;

  void NotifyDetectionException() override;

  void SetToggleThreatColorization(bool setValue) override;

  void MeasureVolumeByValue(bool setVal) override;

  void SetEnableDistanceMeasurement(bool setVal) override;

  void SetVoumeMeasurementStartRange(uint16_t minVal,
                                     uint16_t maxVal) override;

  void SlabNonThreatLaptop(int laptopID) override;

  void SetEnableThreatRedering(bool setVal) override;


  //----------------------------------------------------
  // Public methods not in the generinc Interface.

  void resetRendererTransferFunctions();



  //----------------------------------
  // Useful public functions that we
  // don't want to define in the VolumeRendererInterface
  // because they might quickly change.

  void reDisplayCurrentThreat();
  void internalClearCurrentThreat();

  void triggeredActionZoomIn();
  void triggeredActionZoomOut();


  void immediateRender();

  QString displayModeString(VREnums::VRViewModeEnum aMode);
  QString displayLabel();
  void triggerThreatInitTimer();

  void initAsThreatrenderer();
  bool isAThreatRenderer();
  bool isInSlabMode();
  void revertRightViewerFormThreatSettings();

  void getCurrentTransferFunctions(std::list<HSVColorSigVal>& retListHSV,
                                   std::list<OpacitySigVal>&  retListOpcity);
  void getNormalHSVTransferFunction(std::list<HSVColorSigVal>& retList);
  void getNormalOpacityTransferFunction(std::list<OpacitySigVal>& retList);

  void getThreatOpacityTransferFunction(uint16_t threatOffset,
                                        std::list<OpacitySigVal>& retOpList);
  void getThreatHSVTransferFunction(uint16_t threatOffset,
                                    std::list<HSVColorSigVal>& retHsvList);
  void slabModeRenderThreat(ThreatObject* pThr,
                            bool centerOnThreat);

   void syncSibling();
   void syncUserPickPpint();
   void setActiveCamera(vtkCamera* setCam);
   void deepCopyCameraAndUpdate(vtkCamera* aCamera);
   void removeText();
   vtkCamera* getRenderCamera();

   void forceUpdate();

   void forceRedraw();

   void localZoom(float value);
   void siblingSetZoomValue(double newVal);

   void setOperatorThreatLabel(int index,
                               int size,
                               QString& operatorThreatType);


   void directUpdateCutViewAxis(VREnums::AXIS axis);

   void reorientBagForCut(VREnums::AXIS axis);

   void reorientBagForSlab(VREnums::AXIS axis);

   void setRawThreatLabel(QString& rawLabel);
   void clearThreatLabel();
   void vtkWidgetUpdate();
   void enableThreatText();
   void selectSubVolume();
   void hideSubVolume();
   void hideLaptopROI();
   void setSurfaceROIEnable(bool setVal);
   void setMutexForRendering(QMutex* aMutex);
   void getUserThreatRange(QVector3D& p0,
                           QVector3D& p1);

   bool getCurrentThreatVolume(QVector3D& p0,
                               QVector3D& p1);
   ThreatObject* getCurrentThreatObject();

   void increaseOperatorThreatSize();
   void decreaseOperatorThreatSize();
   void setLaptopSlabMode(bool slabVal);

   /**
    * @brief initThreatsFromTDR
    * @param threats
    * @param pTdr
    * @param volMin
    * @param volMax
    * @param pixelSpacing
    * @param vecDisplayThreatIdentifier
    * @param treatOffsetMap
    * @param doMass
    */
   void initThreatsFromTDR(ThreatList* threats,
                           SDICOS::TDRModule* pTdr,
                           QVector3D& volMin,
                           QVector3D& volMax,
                           QVector3D& pixelSpacing,
                           const std::vector<std::pair<int, std::string> >&  vecDisplayThreatIdentifier,
                           std::map<ThreatAlarmType, uint16_t>& treatOffsetMap,
                           bool doMass);


   double getDesiredUpdateRate();
   void setDesiredUpdateRate(double updateRate);

   int addOperatorThreatObject(ThreatVolume& newTV,
                               QString& threatTypeText);

   int firstVolumeRenderTime();

   QString getGenericMessageStrig(int msgID);

   int unclearedThreats();
   int unclearedMachineThreats();
   void clearAllThreats();
   void disarmOperatorThreatPick();

   void clearTheratData();

   void armRubberBand(bool armVal);



   static void setCurrentThreatPicker(VolumeRendererWidget* pVRWidget);
   static VolumeRendererWidget* currentThreatPicker();

   static bool pickerActive();
   static void setPickerActive(bool setVal);
   void leftViewerShowThreatAtIndex(int rawIndex);

   static void mainRendererCameraMoveCallback(vtkObject* caller,
                                  vtk_callback_ulong eventId,
                                  void* clientData,
                                  void* callData);

  static void threatRendererCameraMoveCallback(vtkObject* caller,
                                  vtk_callback_ulong eventId,
                                  void* clientData,
                                  void* callData);

   void threatWidowPrintSelf(QString& strSelf);

   void hadleVTKAutoCameraMove();
   void enableVolumeCropping(bool doCrop);
   void setCropCorners(CropCorners& corners);

   vtkCamera* getMainCamera();
   vtkCamera* getThreatCamera();
   VolumeRendererWidget* getSibling();

   QStringList getThreatList();

   std::list<ThreatObject*> getThreatObjectList();

   void updateThreatRuler();

   void saveVTKTranferFunctions();

   QVector3D getLastRightPickPoint();

   void setLastRightPickPoint(QVector3D& pickPoint);

   int getDbgBagCount();

   double  getLastThreatVolumeSize();

   void showLeaks();
   void showThreatInfo();

   void exportThreatData(QString& filenameExport);


   void setColorLevel(float colorLevel);

   void setWindowLevel(float windowLevel);

   void setGrayscaleSettings(GrayscaleValueOpacityStruct& grayScaleStruct);
   void hideLabelsAndAxes();
   void showLabelsAndAxes();
   void testImageOverlay(QWidget* mainWin);
   void createColorImage(vtkImageData* image);
   void distanceMeasurementComplete();


   /**
    * @brief constructMachineThreatText
    * @param aTtype
    * @param mass
    * @return
    */
   QString constructMachineThreatText(ThreatAlarmType aType,
                                      double mass,
                                      bool doMass);

   void showVtkSplashScreenNew();

   virtual void removeSplash();
   virtual void removeDisabledSplash();
   double estimetedZoom();

   vtkRenderer* getVTKRenderer();

   VolumeRenderer* getVolumeRenderer();

   void getLeftAndRightViewers(VolumeRendererWidget** ppLeftViewer,
                               VolumeRendererWidget** ppRightViewer);

   void centerWindowOnPointAndExtent(QVector3D& p0,
                                     QVector3D& extent,
                                     bool useY);

   void centerWindowOnPointAndExtentNew(QVector3D& p0,
                                     QVector3D& extent);


   void slabCenterThreat(QVector3D& p0, QVector3D& extent);

   void centerCroppedThreatWindow(QVector3D& p0, QVector3D& extent);

   void centerCroppedThreatWindowNew(QVector3D& p0,
                                     QVector3D& extent);



   void stashCurrentCamera();

   void restoreStashedCamera();

   void showThreatText(ThreatObject* pThr,
                       int threatTypeIndex,
                       int threatTypeTotal);

   void showThreatByMode(ThreatObject* pThr,
                         SDICOS::Bitmap* pBMP, int rawIndex,
                         int threatTypeIndex,
                         int threatTypeTotal,
                         bool operatorGenerated,
                         bool keepOrientation);

   void showWindowCenterInfo(QVector3D& p0,
                             QVector3D& extent,
                             const std::string strCaller);

   virtual void mouseMoveEvent(QMouseEvent* event);
   virtual void wheelEvent(QWheelEvent* event);


signals:
  /**
   * @brief initComplete
   *  Sent from the VolumeRendererWidget when the Init() call is completed.
  */
  void initComplete();

  /**
   * @brief userThreatSelected
   *  Sent from the VolumeRendererWidget when the the user has selected a volume region for creating
   *  a USER THREAT.
   */
  void userThreatSelected();

  /**
   * @brief userThreatComplete
   * VolumeRendererWidget when the the USER THREAT is ready for a storage persistence.
   */
  void userThreatComplete();

  /**
   * @brief userThreatBoxDrawingCompleted
   * signal that the OPERATOR has completed the Pick Box drawing
   */
  void userThreatBoxDrawingCompleted();

  /**
   * @brief slabtPositionChanged
   * @param newSlabPosition
   * signal that the slab position has been changed and the GUI SLIDER should be updated
   * acordingly
   *  NOTE: WITHOUT calling
   *   SetSlabUnpackPosition(double value, VREnums::AXIS axis, bool isSlab)
   * as this has been called internally.
   */
  void slabPositionChanged(double newSlabPosition);

  /**
   * @brief operatorThreatSelected
   * @param flag
   * Signal to indicate that current threat selected is operator threat
   */
  void operatorThreatSelected(bool flag);

  /**
   * @brief viewModeEnabled
   * @param aMode
   * Notification of the Current View Mode. Can be { Threat, Surface, Laptop}
   */
  void viewModeEnabled(VREnums::VRViewModeEnum aMode);


  /**
   * @brief inputRedrawNeeded
   */
  void inputRedrawNeeded();


  void loadUpdate(int pcnt);
  void panPerformed();
  void zoomPerformed();
  void genericVRWidgetMessage(int& aMessageID);
  void timerVRWidgetMessage(int& msgID,
                            int& timeval);
  void positionVRWidgetMessage(int& aMessageID,
                               QVector3D& pos);

  void rubberBandPonitsSeleted(QVector2D& p0,
                               QVector2D& p1);



public slots:
  void onVolumeRenderDBGUpdate(QString message);
  void onVolumeRenderGenericMessage(int& msgID);
  void onVolumeRenderTimerMessage(int& msgID,
                                  int& timeVal);

  void onThreatRenderTimerMessage(int& msgID,
                                  int& timeval);

  void onReadUpdate(double updateVal);
  void onImageLoadComplete(int msLoad);
  void onVolumeMeasurementComplete();
  void onReadDataRange(double rangeX,
                       double rangeY,
                       double rangeZ);
  void onThreatLoadComplete();
  void onVolumeRenderMemRequest(uint64_t memRequested,
                                uint64_t memAvailable);
  void onSliderRedraw();
  void onCameraPositionChanged();
  void onThreatCameraPositionChanged();
  void onMainVolumePick(QVector3D& aPoint);
  void measureItemVolumeByValue();
  void onVolumeMeasurementByPick(QVector3D&  worldPoint);
  void onThreatVolumePick(QVector3D& aPoint);
  void onBeginInteractiveRender();
  void onPaintComplete();
  void onEndInputInteraction();
  void setRendererDisplayName();
  void onRubberBandSelected(QVector2D& p0, QVector2D& p1);
  void onThreatInitTimerTimeout();
  void onIntractionEndTimeout();
  void onPercentSliderUpdate();
  void disableDirectDistanceMeasurement();
  void measurePickPointFromSibling(QVector3D& worldPoint,
                                                     bool bFirst);
  std::list<OpacitySigVal>&  getNormalOpacityList();
  std::list<HSVColorSigVal>& getNormalHsvColorList();
  std::list<OpacitySigVal>&  getLowDensityOpacityList();
  std::list<HSVColorSigVal>& getLowDensityHsvColorList();



protected:
  virtual void distanceMeasurementReset();
  virtual bool isDistanceMeasurementComplete();
  virtual void distanceMeasurementEndInteraction();
  virtual double distanceMeasurementCurrentLength();
  virtual bool distanceMeasurePickPoint(QVector3D& worldPoint, bool bFirst);
  virtual void distanceMeasurePickPointTouch(QVector3D& worldPoint);
  virtual void distancMeasureZeroLengthAt(QVector2D& screenPoint);
  virtual bool isDistancToolStateManipulate();
  virtual void getViewCenter(QVector3D& viewCenter);
  virtual bool isToucPointNearHandle(QPoint touchPoint, QPoint& retPoint);
  virtual void logKeyStrokeEvent(QString keyStroke, QString alarm_type);


private:
  void  createVTKWindow();
  void  createDistanceMeasurer();
  void  setDistanceMeasureUnit(bool asMetric);

  void setMaterialTransitions(std::map<MaterialTransition_E,
                              double>& transitionMap);

  void  setVolumeTransferFunctions(std::list<HSVColorSigVal>& colorList,
                                   std::list<OpacitySigVal>&  opacityList);

  void  setLowDensityThransferFunctions(std::list<HSVColorSigVal>& colorList,
                                       std::list<OpacitySigVal>& opacityList);

  void  setThreatTransferFunctions(std::list<HSVColorSigVal>& colorList,
                                   std::list<OpacitySigVal>&  opacityList);

  void setupVolumRendererParams();

  void syncZoomText();

  void syncZoomCount(int& zoomCount,
                     int& threatZoomCount);


  void initViewAlignment();
  void resetMainRendererViewAlignment();
  void resetThereatViewAlignment();
  void slabResetView();
  void hardResetRendreYView();

  void resetThreatRendererViewAlignment();
  void initThreatList();
  void initRendererThreatOffsetMap();

  void rightAapplyLowDensity(bool setVal);
  void worldToImageVolumeRange(QVector3D   p0wld,
                                QVector3D  p1wld,
                                QVector3D& p0img,
                                QVector3D& p1img);

  void imageToWorldVolumeRange(QVector3D  p0img,
                               QVector3D  p1img,
                               QVector3D& p0wld,
                               QVector3D& p1wld);


  void generateCubeOperatorThreat(QVector2D &p0,
                                  QVector2D &p1);

  void generateElogatedOperatorThreat(QVector2D& p0,
                                      QVector2D& p1);

  void finalizePickBoxHeight(QVector3D& pCenter,
                             QVector3D& pRange);

  void ClampPickBoxToVolumeBounds(QVector3D&  pCenter,
                                QVector3D&  pRange);

  void createVTKThreatActors();

  void readThreatList(SDICOS::TDRModule* pTdr);

  void yawView(vtkSmartPointer<vtkRenderer> aRenderer,
               double angle);

  void rollView(vtkSmartPointer<vtkRenderer> aRenderer,
                double angle);

  void pitchView(vtkSmartPointer<vtkRenderer> aRenderer,
                 double angle);

  void systemMemoryUsageToLog();
  void showCameraAndSceneSettings();
  void writeVolume();
  void saveTransferFunctions();
  void saveFullMaps();
  void suspectNonThreatTDR(SDICOS::TDRModule* pTdr,
                   SDICOS::CTModule *ctModule);

  void disableVolume();
  void disableMeasurementWidget();

  //-------------------------------
  // ARO-FOR DEBUGGING ONLY
  void logMemoryUsage();
  void cycleLoadWriteData();
  void cycleLoadWriteHeader();
  void logBagSizeInfo();
  void sizeInfoWriteHeader();
  void sizeInfoWriteData();
  void appenStringToFile(QString& filename,
                         QString& str);

  QString stripRFIDLeadingZeros(QString& fullRFIDStr);
  void clearUserDecision();
  void disableSlabing();
  void stopLoadTimers();

  //-------------------------------

  QLabel*                   m_parentWidget;
  QString                   m_displayName;
  QString                   m_userDecision;
  QGridLayout*              m_parentLayout;
  ViewLabelWidget*          m_textLabel;
  AxisWidget*               m_viewAxisWidget;
  RulerWidget*              m_viewRuler;

  bool                      m_useGPU;
  bool                      m_enableThreats;
  bool                      m_enableThreatViewing;
  bool                      m_showThreatMass;
  bool                      m_threatOnlyViewStatus;
  bool                      m_dynamicSplash;
  bool                      m_isATipBag;
  bool                      m_isSlabbing;
  VREnums::SlabViewType     m_slabViewType;
  VREnums::AXIS             m_currentCutAxis;
  double                    m_slabThickness;
  ThreatList*               m_threatList;


  int                 m_threatRawIndex;
  int                 m_threatTypeIndex;
  int                 m_threatTypeTotal;

  bool                m_doInternalCropping;
  VREnums::DispalyUnitSystemEnum m_displayUnitSystem;
  CropCorners         m_cropBox;
  float               m_contrastSliderVal;
  float               m_brightnessSliderVal;
  float               m_currentOpacityValue;
  float               m_windowLevelSliderVal;
  float               m_colorLevelSliderVal;
  float               m_currentZoom;
  double              m_lightingAmbient;
  double              m_lightingDiffuse;
  double              m_lightingSpecular;
  double              m_cutPercentageY;


  std::list<OpacitySigVal>               m_opacityMapNormal;
  std::list<HSVColorSigVal>              m_colorMapNormal;

  std::list<OpacitySigVal>               m_opacityMapLowDensityl;
  std::list<HSVColorSigVal>              m_colorMapLowDensity;

  std::list<OpacitySigVal>               m_opacityMapThreat;
  std::list<HSVColorSigVal>              m_colorMapThreat;

  // Performance optimizatin related changes
  std::map<double, std::list<HSVColorSigVal>>  m_threatOffsetHsvMap;
  std::map<double, std::list<OpacitySigVal>>   m_threatOffsetOpacityMap;
  std::map<MaterialTransition_E, double>       m_materialTransitionMap;
  std::map<ThreatAlarmType, uint16_t>          m_threatSignalOffsetMap;

  VolumeRenderer*                              m_volumeRenderer;

  //--------------------------
  // Sync two or more windows
  bool                             m_syncActive;
  std::list<VolumeRendererWidget*> m_renderSiblingsList;
  bool                             m_scannerDirLefToRight;



  vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindowRef;
  vtkSmartPointer<vtkRenderer>                  m_widgetRenderer;
  vtkActor2D*                                   m_imageSplashRef;
  vtkActor2D*                                   m_disabledViewerSplashRef;

  //-----------------------------------------
  vtkSmartPointer<vtkCamera>       m_stashCamera;

  //------------------------------------------
  // VolumeRendererWidget can be a bag
  // renderer or a threat renderer
  bool                             m_isThreatRenderer;
  bool                             m_isInSlabbingMode;

  //-------------------------------------------
  // Tentative Variable used for testing
  int                                 m_dbgBagCount;
  bool                                m_memoryUseLogged;
  uint16_t*                           m_debugWriteBuffer;
  int                                 m_debugImageWidth;
  int                                 m_debugImageHeight;
  int                                 m_debugSliceCount;
  QVector3D                           m_lastRightPickPoint;

  //-------------------------------------------
  bool                                m_removeTimerStarted;
  QTime                               m_VR_ProcessTimerFullLoad;
  QTime                               m_VR_ProcessFromInit;

  QTime                               m_VR_ProccesFromThreatClear;

  // Better distance measurements
  vtkSmartPointer<TouchDistanceWidget>           m_distanceWidget;
  vtkSmartPointer<vtkDistanceRepresentation3D>   m_distanceRep;
  bool                                           m_distacneMeasurementComplete;

  // Slider throttle for density cut-off filter
  // see SetMaterialFilterCutoff(double material_cutoff)
  bool                                m_sliderThrottleOn;

  //---------------------------------------
  static VolumeRendererWidget*     s_curretnThreatPicker;
  static bool                      s_pickerActive;
  static VREnums::VRViewModeEnum   s_rightViewerViewMode;


  //-------------------------------------------------------------------------
  // Properties put in to help Squish Automated test to verify rendered text
  QString m_prop_bagId;
  QString m_prop_DateTime;
  QString m_prop_ThreatType;
  QString m_prop_ThreatId;
  QString m_prop_RFID;
  //-------------------------------------------------------------------------
};


//------------------------------------------------------------------------------
}  // namespace workstation

}  // namespace analogic
//------------------------------------------------------------------------------



//-----------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERWIDGET_H_
//-----------------------------------------------------------------------------
