//------------------------------------------------------------------------------
// Name: VolumeRendererBase.h
// Description: Base class for Volume renderer
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef  ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERBASE_H_
#define  ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERBASE_H_
//------------------------------------------------------------------------------

#include <QDebug>
#include <QTime>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkImageClip.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkStructuredPoints.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkProperty.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkWidgetRepresentation.h>
#include "CenterSelectBoxWidget.h"
#include "OpenGLVolumeWidget.h"
#include "ThreatObject.h"
#include "ScannerDefs.h"
#include "ViewLabelWidget.h"


//-----------------------------------------------
// Normal Background Colors For Main Render Window
#define VOLUME_BG_DEFAULT_RED    0.95
#define VOLUME_BG_DEFAULT_GREEN  0.95
#define VOLUME_BG_DEFAULT_BLUE   0.95
//-----------------------------------------------

//-----------------------------------------------
// Inverse Background Colors For Main Render Window
#define VOLUME_BG_INVERSE_RED    0.03
#define VOLUME_BG_INVERSE_GREEN  0.03
#define VOLUME_BG_INVERSE_BLUE   0.03
//-----------------------------------------------


//-----------------------------------------------
// Background Colors For Threat Render Window
#define THREAT_VOLUME_BG_DEFAULT_RED    0.93
#define THREAT_VOLUME_BG_DEFAULT_GREEN  0.93
#define THREAT_VOLUME_BG_DEFAULT_BLUE   0.93
//-----------------------------------------------



//----------------------------------------------
// Color for Operator Threat Widget creation
#define OPERATOR_THRERAT_INITIAL_RED    0.0
#define OPERATOR_THRERAT_INITIAL_GREEN  0.101
#define OPERATOR_THRERAT_INITIAL_BLUE   0.53

//------------------------------------------------
// Gray Scale HSV =  V val.
#define GRAY_SCALE_DEF_THREAT_V 0.35

//-----------------------------------------------
// Threshold Filter Map Scalar.
#define THRESHOLD_FILTER_MAP_SCALAR       0.9

//----------------------------------------------
// Color for a Suspected Threat
#define SUSPECTED_MACHINE_THREAT_RED      0.862
#define SUSPECTED_MACHINE_THREAT_GREEN    0.411
#define SUSPECTED_MACHINE_THREAT_BLUE     0.07



#define RENDER_TIMER_MESSAGE_THRESHOLD 100
//-----------------------------------------------
#define VOLUME_RENDERER_CAMERA_MOVED_MESSAGE_ID 201
#define THREAT_RENDERER_CAMERA_MOVED_MESSAGE_ID 202
#define VOLUME_RENDERER_ZOOM_MESSAGE_ID         203
#define THREAT_RENDERER_ZOOM_MESSAGE_ID         204
#define VOLUME_RENDERER_INIT_ORIENT_MESSAGE_ID  205
#define THREAT_RENDERER_INIT_ORIENT_MESSAGE_ID  206
#define THREAT_RENDERER_INIT_COMPLETE           207
#define VOLUME_PICK_POINT_ID                    208
#define THREAT_PICK_POINT_ID                    209

#define THREAT_STASH_SAVE_TIMER_ID              211
#define THREAT_STASH_RETRIEVE_TIMER_ID          212
#define THREAT_VTK_DISPLAY_TIMER_ID             213
#define MAIN_VTK_DISPLAY_TIMER_ID               214
#define VOL_RENDER_BASE_INITIAL_RENDER_ID       218
//-----------------------------------------------

enum vtkInterpolationType
{
  INTERPOLATE_NEAREST = 0,
  INTERPOLATE_LINEAR  = 1,
  INTERPOLATE_CUBIC   = 2,
};

enum VR_DisplayUnit
{
  VR_DISPLAY_INCH = 0,
  VR_DISPLAY_CM   = 1,
};

//------------------------------------------------------------------------------
class VolumeRendererBase : public QObject
{
  Q_OBJECT

public:
  // Constructor
  explicit VolumeRendererBase(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow,
                              vtkSmartPointer<vtkRenderer> renderer,
                              double pixelSpacingX,
                              double pixelSpacingY,
                              double pixelSpacingZ);

  // Destructor
  virtual ~VolumeRendererBase();

  // Sets the view label for the renderer
  void setViewLabel(ViewLabelWidget* viewLabel);
  ViewLabelWidget* getViewLabel();

  // Remove a volume from renderer.
  virtual void removeVolume();

  virtual void setShaderStatus(bool shaderStat);

  virtual void setBgNormal()  = 0;

  virtual void setBgInverse() = 0;

  void resetRenderSettings();

  void setLowDensityActive(bool setVal);
  bool isLowDensityActive();

  void setIverseBgValue(bool bgInverseVal);

  bool isBgInverse();

  /**
   * @brief setOrganicRemovable
   * @param allows one the ability to remove organic from the renderer.
   */
  void setOrganicRemovable(bool organicRemovable);

  /**
   * @brief setInorganicRemovable
   * @param allows one to enable/diable metal removal of inorganic.
   */
  void setInorganicRemovable(bool inorganicRemovable);

  /**
   * @brief setMetalRemovable
   * @param allows one to enable/diable metal removal of metal.
   */
  void setMetalRemovable(bool metalRemovable);

  /**
   * @brief removeThreatVisual
   * @param pActor
   */
  void removeThreatVisual(vtkActor* pActor);

  void setGPUStatus(bool useGPU);

  void setGrayscaleSettings(GrayscaleValueOpacityStruct& grayStruct);

  GrayscaleValueOpacityStruct& getGrayScaleSettings();

  int  vtkLabelIndexFromThreatOffset(uint16_t threataOffset,
                                     std::list<ThretOffsetProperty>& properyList);

  void setTransferFunctionValues(std::list<OpacitySigVal>&  opacityList,
                                 std::list<HSVColorSigVal>& colorList,
                                 std::map<MaterialTransition_E, double>& transitionMap,
                                 std::map<double, std::list<HSVColorSigVal>>& threatOffsetHsvMap,
                                 std::map<double, std::list<OpacitySigVal>>& threatOffsetOpacityMap);


  void reApplyColorTransferFunction();

  void setDisplayName(QString& aName);

  QString displayName();

  void setDisplayUnitToCentimeter();

  void setDisplayUnitToInch();

  void setVolumeComputeStartRange(uint16_t startMin,
                                  uint16_t startMax);

  double getVolumeMMCubed();

  double getVolumeSizeMB();

  // Get the precomputed bounds of the volume
  void getBounds(double* bounds);

  // Get the bouds via internal vtkVolume Object.
  void getVolumeBounds(double*  volBounds);


  // Set the boounds of the volume
  void setBounds(double* bounds);

  // Reset the camera to some default based on data
  void resetCamera();

  // Reset the camera based on the specified bounds.
  void resetCameraWithBounds(double* bounds);

  // Set the Background color of the render window
  void setRenderBackground();

  void setRenderWithColor(bool doColor);
  bool isRenderingWithColor();

  // Get the cutrret pixels spacing
  void getPixelSpacing(double& psX,
                       double& psY,
                       double& psZ);

  /**
   * @brief getNumberOfSlices
   * @param sizeBuffer
   * @param W
   * @param H
   * @return
   */
  int getNumberOfSlices(size_t sizeBuffer,
                        int W,
                        int H);

  void setLinearInterpolation(bool doLinear);

  bool isUsingLinearInterpolation();

  // Get the shader status
  bool isUsingShading();

  // Set the scale factor for dynamically modifying the Opacity, Contrast & Brightness values.
  void setOCBModifyScalar(double scaleValOpacity,
                          double scaleValContrast,
                          double scaleValBrightness);

  vtkSmartPointer<vtkActor> createThreatVisual(ThreatVolume& aVolume,
                                               double createColorRed,
                                               double createColorGreen,
                                               double createColorBlue,
                                               bool bIsShield);


  /**
   * @brief resetTransferFunctions
   * Reset the transfer function from the default initial values
   */
  virtual void resetTransferFunctions();




  /**
   * @brief updateTransferFunctions
   * Update the transfer functions using the current normal
   * color and opacity lsits.
   */
  void updateTransferFunctions();

  void updateRelativeTransferFunctions(std::list<HSVColorSigVal>& curHSVList,
                                       std::list<OpacitySigVal>& curOpacityList);

  void setMaterialHighPassCutoff(double setValue);


  /**
   * @brief setThreatViewOpacityAndColorTF
   * @param opacityList
   * @param colorList
   * This method allows for material selection when the right renderer is a threat viewer.
   */
  void setThreatViewOpacityAndColorTF(std::list<OpacitySigVal>&  opacityList,
                                      std::list<HSVColorSigVal>& colorList);

  void setColorizeThreat(bool setVal);
  bool isThreatColorized();

  void setLightingParams(double ambient,
                         double diffuse,
                         double specular);

  bool getLightingParams(double& ambient,
                         double& diffuse,
                         double& specular);

  bool isMsskClipped();

  void setDefaultLightingParams();

  void saveVTKColorMapToFile(
                       vtkSmartPointer<vtkColorTransferFunction> colorTF,
                       QString& filenameOut);

  void saveVTKOpacityMapToFile(
                       vtkSmartPointer<vtkPiecewiseFunction>  opacityTF,
                       QString& filenameOut);

  //------------------------------------------------------------
  // ARO-NOTE: Method to be deprecated.
  void initThreatOpacityTF(vtkSmartPointer<vtkPiecewiseFunction>& threatOpacityTF,
                           std::list<OpacitySigVal>& opasityLis);


  // ARO-NOTE: Method to be deprecated.
  void initThreatHSVColorTF(vtkSmartPointer<vtkColorTransferFunction>& threatColorTF,
                            std::list<HSVColorSigVal>& hsvColorList);

  // ARO-NOTE: Method to be deprecated.
  void initThreatBWTF(vtkSmartPointer<vtkColorTransferFunction>& threatColorTF,
                      std::list<HSVColorSigVal>& hsvColorList);

  //------------------------------------------------------------

  //------------------------------------------------------------
  // Preferred fransfer function methods.
  void initOpacityTransferFunction(vtkSmartPointer<vtkPiecewiseFunction>  opacityTF,
                                   std::list<OpacitySigVal>& opasityLis);

  void initHSVColorTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTF,
                                    std::list<HSVColorSigVal>& hsvColorList,
                                    bool isThreat = false);

  void initBWTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTF,
                              std::list<HSVColorSigVal>& hsvColorList,
                              bool isThreat = false);
  //------------------------------------------------------------


  void hsvToRgb(double h, double s, double v,
                double& r, double& g, double& b);
  void rgbToHsv(double dR, double dG, double dB,
                double& dH,  double& dS, double& dV);
  void createSubVolumeWidget(OpenGLVolumeWidget *qvtkWidget);

  void showSubVolume(QVector3D& pCenter,
                     QVector3D& pRange);


  void setShowThreatOnlyValue(bool threatOnly);
  bool getThreatOnlyViewStatus();

  void hideSubVolume();
  void increaseOperatorThreatsize();
  void decreaseOperatorThreatsize();

  void getSubWidgetBounds(QVector3D& p0,
                          QVector3D& p1);

  static void setInitTime(int msInit);
  static void addInitTime(int msInit);
  static int  getInitTime();

  static void setInitThreatInfo(int numThreats,
                                double startThreatVolume,
                                double fullVolunme);

  static void getInitThreatInfo(int& numThreats,
                                double& startThreatVolume,
                                double& fullVolunme);


  void flipBoxInY(QVector3D& p0,
                  QVector3D& p1,
                  QVector3D& flipP0,
                  QVector3D& flipP1);

  void flipPointInY(QVector3D& p0,
                    QVector3D& flipP0);

  // Scale the Opacity 0-1  based on the set point and the 0-100 pcnt change
  double scaleOpacityValue(int pcntChange, double setPoint);
  // Scale the Saturation Velue 0-1  based on the set point and the 0-100 pcnt change
  double scaleSaturationValue(int pcntChange, double setPoint);

  // Scale the signal 0-4095 based on the set point and the 0-100 pcnt change
  double scaleSignalValue(int pcntChange, double setPoint);

  void createCenteringSphere(vtkRenderer* renderer,
                        QVector3D& center);
  void removeCenteringSphereIfPresent();


  void centerScene(QVector3D rotPoint);

  void panScene(int deltaX, int deltyaY);
  void computeWorldToDisplay(vtkRenderer* ren,
                             double x,
                             double y,
                             double z,
                             double* displayPt);

  void computeDisplayToWorld(vtkRenderer* ren,
                             double x,
                             double y,
                             double z,
                             double* worldPt);

  void setMapperModified();

  void resetClipIndex();

  void setThreatOffsetPropertyList(std::list<ThretOffsetProperty>& propertyList);

  //---------------------------------
  // ARO-ADDED: 7-22-219
  void showColorTransferFunction(vtkColorTransferFunction* colorTF);
  void showOpacityTansferFunction(vtkPiecewiseFunction* opacityTF);
  //----------------------------------


  std::map<double, std::list<HSVColorSigVal>>& getThreatOffsetHsvMap();
  std::map<double, std::list<OpacitySigVal>>&  getThraOffsetOpacityMap();
  std::list<ThretOffsetProperty>& getThreatPropertyList();

signals:
  void debugMessage(QString message);
  void genericVRBaseMessage(int& msgID);
  void timerVRBaseMessage(int& msgID, int& timeVal);

protected:
  // Create the transfer functions for SAFE NORMAL.
  void generateSafeNormalTransferFunctions(vtkSmartPointer<vtkPiecewiseFunction> opacityTF,
                                           vtkSmartPointer<vtkColorTransferFunction> colorTF);

  // ARO-TESTING:
  void generateTestOpacityTF(vtkSmartPointer<vtkPiecewiseFunction>& testOpacityTF);
  void generateTestColorTF(vtkSmartPointer<vtkColorTransferFunction>& testColorTF);

  // Setup the 4x4 matrix used in flipping the Y axis.
  void setupYFlipMatrix();

  vtkSmartPointer<vtkMatrix4x4> getYFlipMatrix();

  void setVolumeInerpolation(vtkSmartPointer<vtkVolumeProperty> property,
                             vtkInterpolationType interpType);

  void dataBoundsToClipBounds(double* dataBounds,
                              int* clipBounds);

  void clampHSV(double& h, double& s, double& v);

  //--------------------------
  // When in Grayscale we use these methods to deterimine
  // opacity and HSV valyues for different materials .
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

  void opacityOfOrganicGray(double signalValue,
                            double& opacity);

  void opacityOfInorganicGray(double signalValue,
                              double& opacity);

  void opacityOfMetalGray(double signalValue,
                          double& opacity);


  double largetDim(double dimX,
                   double dimY,
                   double dimZ);

  void clampBounds(double bounds[6]);

  void generatePolyDataFromPoints(QVector3D volP0,
                                  QVector3D volP1,
                                  vtkSmartPointer<vtkPolyData> polyData,
                                  bool bIsShield);



  void debugCompareSubWidgetToBounds(QVector3D& p0,
                                     QVector3D& p1);

  void setPropertyColor(int index,
                        vtkColorTransferFunction* colorTF);

  void setPropertyScalarOpacity(int index,
                                vtkPiecewiseFunction* opacityTF);



  vtkSmartPointer<vtkActor>          m_sphereActor;

  //------------------------------------------------------
   size_t                            m_volumeSizeBytes;

  //--------------------------------------------
  // ARO- Added to allow vtkImageClip based
  //      threat rendering.
  vtkSmartPointer<vtkStructuredPoints>  m_keptImageData;
  int                                   m_currentClippedThreatIndex;
  bool                                  m_maskIsClipped;
  //--------------------------------------------


  //--------------------
  // Rener ID
  QString                            m_displayName;
  vtkGenericOpenGLRenderWindow*      m_renderWindowRef;
  vtkRenderer*                       m_rendererRef;
  vtkSmartPointer<CenterSelectBoxWidget>      m_subVolumeBoxWidget;
  QVector3D                          m_subVolumeBoxPlacenemt;

  //------------------------------------------------------
  // SmartPointer raw rererences needed for dynamic update
  vtkFixedPointVolumeRayCastMapper*      m_rawMapper;
  vtkVolumeProperty*                     m_rawProperty;
  vtkVolume*                             m_rawVolume;

  std::list<OpacitySigVal>               m_opasityList;
  std::list<HSVColorSigVal>              m_hsvColorList;

  //-------------------------------
  // We need to keep track of the
  // current OpacitySigVal list
  // and the current HSVColorSigVal list
  // because material stripping is now
  // indipendent of these values and we
  // need to keep track of what they are.
  std::list<OpacitySigVal>  m_currentOpacityList;
  std::list<HSVColorSigVal> m_currentHSVColorList;


  std::map<MaterialTransition_E, double> m_transtitonMap;

  double                                 m_imageDataBounds[6];


  ViewLabelWidget*                       m_viewLabel;
  VR_DisplayUnit                         m_diplayUnit;
  uint16_t                               m_volumeComputeStartMin;
  uint16_t                               m_volumeComputeStartMax;

  //------------------------------------------------------
  double m_pixelSpacingX;
  double m_pixelSpacingY;
  double m_pixelSpacingZ;


  //------------------------------------------------------
  // Colorize based on material type
  bool m_doColorizeOrganic;
  bool m_doColorizeInorganic;
  bool m_doColorizeMetal;
  bool m_doColorizeThreat;

  bool m_threatBMPOnly;

  //-----------------------------------------
  // Transfer Functions for right viewer as a threat viewer
  std::list<OpacitySigVal>     m_viewAsThreatrOpacityList;
  std::list<HSVColorSigVal>    m_vierwAsThreatHsvColorList;
  bool                         m_maskNeedsColorRemoval;

  // Inverese color flag.
  bool m_doInverseColor;


  double m_opacitySampleDistance;

  double m_interactiveSampleDistance;
  bool   m_autoAdjustSampleDistance;
  bool   m_useShader;
  bool   m_linearInterpolation;
  bool   m_renderWithColor;
  bool   m_organicRemovable;
  bool   m_inorganicRemovable;
  bool   m_metalRemovable;
  bool   m_useGPU;
  bool   m_inverseBackground;
  bool   m_lowDensityActive;
  double m_backgroundRed;
  double m_backgroundGreen;
  double m_backgroundBlue;


  double m_opacityModifyScalar;
  double m_contrastModifyScalar;
  double m_brightnessModifyScalar;

  double m_ambientLighting;
  double m_diffuseLighting;
  double m_specularLighting;

  GrayscaleValueOpacityStruct m_grayscaleStruct;


  vtkSmartPointer<vtkMatrix4x4> m_flipYMatrix;

  //------------------------------------------------
  // Useful initialization information
  static int                     s_initTime;
  static int                     s_numThreats;
  static double                  s_startThreatVolume;
  static double                  s_fullVolume;
  //------------------------------------------------

  // Performance Optimization related changes
  std::map<double, std::list<HSVColorSigVal>>        m_threatOffsetHsvMap;
  std::map<double, std::list<OpacitySigVal>>         m_threatOffsetOpacityMap;
  std::list<HSVColorSigVal>                          m_currThreatHsvList;
  std::list<OpacitySigVal>                           m_currThreatOpacityList;
  uint16_t                                           m_currThreatOffset;
  uint16_t                                           m_materialDisplayCutoff;
  std::list<ThretOffsetProperty>                     m_threatPropertyList;
  bool                                               m_colorizeThreat;
  vtkSmartPointer<vtkImageData>                      m_mask;
  bool                                               m_maskApplied;
  std::map<double, std::list<HSVColorSigVal>>        m_threatOffsetHsvBrightnessMap;
  bool                                               m_isBrightnessModified;
};

//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERBASE_H_
//------------------------------------------------------------------------------
