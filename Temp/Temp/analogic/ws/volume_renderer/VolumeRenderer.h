//------------------------------------------------------------------------------
// File: VolumeRenderer.h
// Description: Header file for Volume renderer
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERER_H_
#define ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERER_H_
//------------------------------------------------------------------------------
#include <stdint.h>
#include <analogic/ws/common.h>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QTime>
#include <QMutex>
#include <sstream>
#include <fstream>
#include <vtkCubeSource.h>
#include <vtkButtonWidget.h>
#include <vtkActor2D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageShiftScale.h>
#include <vtkImageThreshold.h>
#include <vtkImageMathematics.h>
#include <vtkMarchingCubes.h>
#include <vtkMassProperties.h>
#include <vtkTextActor.h>
#include <vtkFlyingEdges3D.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkLookupTable.h>
#include <vtkSurfaceReconstructionFilter.h>
#include "Logger.h"
#include "VolumeRendererBase.h"
#include "ThreatList.h"
#include "MemUtils.h"
#include "VRUtils.h"

#define MASK_AND_MULTI_CALL_SPEEDUP 1

#define DEFAULT_SLAB_THICKNESS 0.1
#define SLAB_SHOW_OPACITY      0.05
#define SLAB_HIDE_OPACITY      0.0

#define THIN_SLAB_OPACITY_TRANSISTION_THICKNESS 5
#define THIN_SLAB_OPACITY_SET_VALUE 0.4

enum VolumeViewMode
{
    NORMAL_SEMI_TRANSPARENT = 0,
    SLICER3D_CT_AAA         = 1,
    THREAT_SEMI_TRANSPARENT = 2,
};


template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
  std::ostringstream out;
  out.precision(n);
  out << std::fixed << a_value;
  return out.str();
}

//------------------------------------------------------------------
// Constants used in Volume Measurement and Measurement Display
#define CUBIC_INCH_PER_CUBIC_MM           6.1024E-5
#define CUBIC_CM_PER_CUBIC_MM             0.001
#define VOLUME_MEASUREMENT_PRECISION      2
#define VOLUME_MEASUREMENT_BOX_WIDTH      10

//------------------------------------------------
#define VOL_MEASURE_LAPLACIAN_SMOOTH_ITT 35
#define VOL_MEASURE_RELAX_FACTOR         0.005
//------------------------------------------------

#define VOL_MEASURE_DIFFUSE_RED          0.1
#define VOL_MEASURE_DIFFUSE_GREEN        0.6
#define VOL_MEASURE_DIFFUSE_BLUE         0.1

#define VOL_MEASURE_SPECULAR_RED         0.15
#define VOL_MEASURE_SPECULAR_GREEN       0.55
#define VOL_MEASURE_SPECULAR_BLUE        0.15

#define VOL_MEASURE_SPECULAR_POWER       40
#define VOL_MEASURE_SPECULARITY          0.4


#define VOL_MEASURE_TEXT_RED             0.15
#define VOL_MEASURE_TEXT_GREEN           0.45
#define VOL_MEASURE_TEXT_BLUE            0.15

#define VOL_MEASURE_TEXT_POS_X           550
#define VOL_MEASURE_TEXT_POS_Y           60
#define VOL_MEASURE_TEXT_FONT_SIZE       25

#define VOL_MEASURE_DICOS_BMP_CLAMP_MIN     0
#define VOL_MEASURE_DICOS_BMP_CLAMP_MAX     4
#define VOL_MEASURE_DICOS_BMP_SET_VAL       2
#define VOL_MEASURE_DICOS_BMP_HSV_HUE_MIN   0.33
#define VOL_MEASURE_DICOS_BMP_HSV_HUE_MAX   0.35
#define VOL_MEASURE_DICOS_BMP_HSV_SAT_MIN   0.70
#define VOL_MEASURE_DICOS_BMP_HSV_SAT_MAX   0.80
#define VOL_MEASURE_DICOS_BMP_HSV_VAL_MIN   0.40
#define VOL_MEASURE_DICOS_BMP_HSV_VAL_MAX   0.65
#define VOL_MEASURE_DICOS_BMP_POINT_SZE     3

#define VOL_MEASURE_MARCH_CUBE_AVG_WINDOW   2



//------------------------------------------------------------------

#define PI_CLAMP_MAX                     4094

class VolumeRenderer : public VolumeRendererBase
{
    Q_OBJECT

public:
    // Constructor
    explicit VolumeRenderer(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow,
                            vtkSmartPointer<vtkRenderer> renderer,
                            double pixelSpacingX,
                            double pixelSpacingY,
                            double pixelSpacingZ,
                            int maxDimX,
                            int maxDimY,
                            int maxDimZ);

    // Destructor
    virtual ~VolumeRenderer();




    void initVolumeImageAttributes(double pixelSpacingX,
                                   double pixelSpacingY,
                                   double pixelSpacingZ,
                                   int maxDimX,
                                   int maxDimY,
                                   int maxDimZ);

    void initUnpackAndSlabbing();

    // Initialize from a data buffer
    bool initFromBuffer(uint16_t* buffer,
                        int sizeBuffer,
                        double* bounds);

    void setPixelSpacing(double psX,
                         double psY,
                         double psZ);


    void getPixelSpacing(double& px, double& py, double& pz);


    // Set the material colorization flag for Color Stripping
    void setMaterialColorize(bool doOrganic,
                             bool doInOrganic,
                             bool doMetal);

    // Set the opacity sample distance
    void setOpacitySampleDistance(double opSampledist);

    // Set the interactive sample distance
    void setInteractiveSampleDistince(double intSampleDist);


    // Set auto adjust sample distance
    void setAutoAdjustSampleDistance(bool adjVal);



    virtual void setBgNormal();

    virtual void setBgInverse();

    virtual bool getBagInverse();

    //----------------------------------------------------------------------------
    // ** We brighten the color by changing only the V component of the HSV part of
    // the color transfer function.
    //
    // The volume renderer uses a color transfer function as part of its rendering
    // algorithm. The transfer function is of the form:
    // { {s0, HSV0}, {s1, HSV1}, {s2, HSV2}, ... } where s = signal [0 - 4095] and
    // HSV = Hue, Saturation, & Value from the HSV color space.
    //
    // We brighten or darken the color by changing just the V component of the HSV
    // color space.
    void brightenColor(int percentSet);


    //------------------------------------------------------------------------------
    // ** Opacity level is defined by the signal level in the opacity Transfer Function.
    //
    // The volume renderer has an opacity transfer function which is a map of
    // opacity values (op) from 0 - 1.0, and signal strength (s) from 0 - 4096.
    //
    // So the map is of the form:
    // { {s0, op0}, {s1, op1}, {s2, op2}, ...}
    // So we make the renderer more or less "opaque" by lowering or raising each
    // signal value in the transfer function.
    //
    // So we Increase the thus defined "Opacity" by moving the transfer function LEFT.
    // (i.e. each signal value in the TF is Lower).
    //
    // And correspondingly we Reduce the thus defined "Opacity" by moving the transfer
    // function RIGHT (i.e. each signal value in the TF is Higher).
    void setOpacitySignalLevel(int percentSet);


    void setTempTransferFunctions(std::list<OpacitySigVal>&  opacityList,
                                 std::list<HSVColorSigVal>& colorList);


    //------------------------------------------------------------------------------
    // ** We change the Contrast by varying the Value part of the opacity transfer
    // function.
    // The volume renderer has an opacity transfer function which is a map of
    // opacity values (op) from 0 - 1.0, and signal strength (s) from 0 - 4096.
    //
    // So the map is of the form:
    // { {s0, op0}, {s1, op1}, {s2, op2}, ...}
    //
    // We change the contrast by changing the value part of the transfer function.
    //
    void setOpacityValueLevel(int percentSet);


    void increaseOpacityOnThinSlice(std::list<OpacitySigVal>& normalOpacityList,
                                  std::list<OpacitySigVal>& thinOpacityList);

    //------------------------------------------------------------------------------
    // ARO-NOTE:
    // 10-23-2019
    // We now use the same mechanism to either SLAB (segment) or UNPACK (peel) the contents
    // of a bag.  The value 0.0 to 1.0 is an input argument  and the axis on which the action
    // is to be preformed {X, Y, Z}

    //------------------------------------------------------------------------------
    // The volume renderer has function to set cut view of volume.
    // It will be calculated from the location of the slider on the UNPACK/SLAB slider.
    // Scaled from  0.00 to 1.00.
    // Id has TWO Modes: SLABBIBNG and UNPACKING
    void setPositionForSlabOrUnpacking(double cutPosition, VREnums::AXIS axis);

    void setSlabPosition(double slabPosition, VREnums::AXIS axis);

    void setUnPackingPosition(double cutPosition, VREnums::AXIS axis);

    void computeUnpackingBondingBoxForBag(double unpackPosition,
                                          VREnums::AXIS axis,
                                          double* unpackBounds);

    double getSlabUnpackPositionFromPoint(QVector3D pCent,
                                         VREnums::AXIS axis);

    void computeSlabBoundingBoxForBag(double slabPosition,
                                      VREnums::AXIS axis,
                                      double* boundingBox);


    // Add graphics cube indicating area of bag to be shown in thickness slabbing view
    void ApplyBoundingCube(double* bounds);

    // Set slabbing mode flag. It controls whether to show thickness slabbing view
    void setSlabbing(bool flag, double thickness);
    void setEnableSlabbing(bool flag);

    // Set the threat list
    void setThreats(ThreatList* threats);

    // Get the list of thres IDs
    QStringList getThreatList();

    // Get the list of threat object
    std::list<ThreatObject*> getThreatObjectList();

    // Return the number of threats pending
    int threatListSize();

    // Clear Machine/Operator Threat
    void clearMachineThreat(int index);
    void clearOperatorThreat(int index);

    // Clear All Threats
    void clearAllThreats();

    // Set current threat index
    void setCurrentThreat(int index);

    // Set the specified alarm to be active.
    void selectThreat(int index);

    // Hide the laptop threats.
    void hideLaptopThreats();
    void unHideMachineThreats();
    void allowThreatMask(bool setVal);
    bool isThreaMaskingAllowed();


    void setVolumeMeasurementSetPoint(uint16_t setval);



    /**
   * @brief getRenderBuildTime
   * @return The number of milliseconds from the beginning of the Init() call begins till the
   *  first full Render() is complete. This defines the throughput of the renderer.
   */
    int getRenderBuildTime();

    /**
   * @brief setRenderMutex - mutext to protect the initial m_renderWindow->Render()
   * @param aMutex
   */
    void setRenderMutex(QMutex* aMutex);

    /**
   * @brief resetBuildTime
   * Used to reset the value to zero for instrumentation purposes.
   */
    void resetBuildTime();

    QString currentThreatText();

    void writeThreatVolumeToFile(QString& volFile);

    void setColorLevel(float colorLevel);

    void setWindowLevel(float windowLevel);

    void getImageBounds(double* bounds);
    // Set actual bounds for given volume
    void setActualBounds(vtkSmartPointer<vtkVolume> volume);

    // Unmask all bits of given volume
    void UnmaskVolume(vtkImageData* image);

    void resetTransferFunctions();

    void mapClippedSubVolume(QVector3D P0,
                              int W,
                              int H,
                              int L,
                              bool doClamp);

    void reMapFullVolume();


    // Create Image mask for 3D volume and set all bits of it
    void createAndSetMask();

    // Reset all mask bits for 3D volume to zero
    void resetMask();
    void setMaskBitsToValueBySize(uint8_t val,
                                  int sizeX,
                                  int sizeY,
                                  int sizeZ);

    void resetMaskWithResize(int imageSizeX,
                             int imageSizeY,
                             int imageSizeZ);

    void resetAndUpdateMask();

    void setMaskBitsToValue(uint8_t val);

    void setMaskTypeToLabel();

    void setMaskTypeToBinary();

    void setRemoveLaptop(bool doRemove);

    void removeLabelMaskColors();
    void setUpForBagSlabbing();

    vtkImageData* getImageData();

    uint16_t getImageValueAt(QVector3D p0);

    void clearvolumeRegion(QVector3D p0,
                           int W,
                           int H,
                           int L);

    void setTransferfunctionsInderect(std::list<HSVColorSigVal>& newHsvColorList,
                                      std::list<OpacitySigVal>&  newOpacityList);

    void setTransferfunctionsDirect(std::list<HSVColorSigVal>& newHsvColorList,
                                    std::list<OpacitySigVal>&  newOpacityList);


    void displayOperatorThreatFromSibling(int W,
                                  int H,
                                  int L,
                                  QVector3D P0,
                                  double ambient,
                                  double diffuse,
                                  double specular,
                                  std::list<HSVColorSigVal>& sibThreatHsvList,
                                  std::list<OpacitySigVal>& sibThreatOpacityList);

    //-----------------------------------------------------
    // ARO-Note: added 8-29-2019 For Colorizing the Threat in
    //    both threat and non threat windows.
    //
    // This Method sets the THTREAT color in the
    //    RIGHT Viewer or in the case of the LAPTOP
    //    shows just the normal laptop pixels.

    void displayCroppedThreatFromSibling(int threatIndex,
                                         int W,
                                         int H,
                                         int L,
                                         QVector3D P0,
                                         SDICOS::Bitmap* pBMP,
                                         analogic::workstation::ThreatAlarmType threatType,
                                         bool isLaptop,
                                         double ambient,
                                         double diffuse,
                                         double specular,
                                         std::list<HSVColorSigVal>& sibNormalHsvList,
                                         std::list<OpacitySigVal>& sibNormalOpacityList,
                                         std::list<HSVColorSigVal>& sibThreatHsvList,
                                         std::list<OpacitySigVal>& sibThreatOpacityList);


    void setMaskColors(bool isLaptop,
                       SDICOS::Bitmap* pBMP,
                       int W, int H, int L,
                       vtkColorTransferFunction* normalColorTF,
                       vtkPiecewiseFunction* normalOpacityTF,
                       vtkColorTransferFunction* threatColorTF,
                       vtkPiecewiseFunction* threatOpacityTF);

    void resetFromSiblingThreat();

    void  getSlabOpacityList(std::list<OpacitySigVal>& normalOpacityList,
                             std::list<OpacitySigVal>& slabOpacityList);

    void slabMaskNonthreatLaptop(vtkSmartPointer<vtkColorTransferFunction> normalColorTF,
                                 vtkSmartPointer<vtkPiecewiseFunction> normalOpacityTF);

    void setPrimaryColorAndOpacity(std::list<HSVColorSigVal>& normalHsvColorList,
                                   std::list<OpacitySigVal>&  normalOpacityList);
    void directMapThreat(int W, int H, int L,
                         QVector3D P0,
                         SDICOS::Bitmap* pBMP,
                         bool  isLaptop,
                         std::list<HSVColorSigVal>& normalHsvList,
                         std::list<OpacitySigVal>& normalOpacityList,
                         std::list<HSVColorSigVal>& threatHsvList,
                         std::list<OpacitySigVal>& threatOpacityList);


    // ARO-Note: This Method sets the color in the LEFT Viewer or in
    //     the case of the LAPTOP
    //     removes the LAPTOP pixels by a BINARY Mask.
    bool threatMaskSubVolume(int W, int H, int L,
                             QVector3D p0,
                             SDICOS::Bitmap* pBMP,
                             vtkImageData* rawClippedImage,
                             uint16_t threatMapIndex,
                             vtkImageData* volumeMask,
                             bool removeROI);

    void setMutiThreatPropertyColors(std::map<double, std::list<HSVColorSigVal>>& hsvThreatMap,
                                     std::map<double, std::list<OpacitySigVal>>& opacityThreatMap,
                                     std::list<ThretOffsetProperty>& threatPropertyList);

    bool maskAllMachineThreats(ThreatList* threats,
                               std::list<ThretOffsetProperty>& propertyList);

    void applyOperatorRenderMask(int W, int H, int L,
                         QVector3D p0,
                         vtkImageData* rawClippedImage,
                         vtkImageData* volumeMask);

    void applyDirectRenderMask(int W, int H, int L,
                               QVector3D p0,
                               SDICOS::Bitmap *pBMP,
                               vtkImageData* rawClippedImage,
                               vtkImageData* volumeMask);

    void applyCroppedRenderMask(int W, int H, int L,
                                SDICOS::Bitmap* pBMP,
                                vtkImageData* volumeMask);

    void outputVolumeMeasurementResult(QString method,
                                       int computeTime,
                                       double computedVolume,
                                       int W,
                                       int H,
                                       int L);

    void measureVolumeByValueFlyingEdge(QVector3D threatP0,
                               int W,
                               int H,
                               int L);

    void measureVolumeByValueMarchingCube(QVector3D threatP0,
                                 int W,
                                 int H,
                                 int L);

    void measureVolumeByDICOSBitmap(QVector3D threatP0,
                                    int W,
                                    int H,
                                    int L,
                                    SDICOS::Bitmap* pBMP);

    void removeMeasuredVolume();
    void removeMeasuredText();
    void hideSlabCube();
    void showSlabCube();
    void resetClippingPlanes();

    void showBitMapDensity(SDICOS::Bitmap* pBMP);

    double averageImageData(vtkImageData* imageData,
                            QVector3D& P0,
                            int W,
                            int H,
                            int L);

    void saveImageData(std::string& filenameOutVol,
                       std::string& filenameOutProp,
                       vtkImageData* imageData,
                       QVector3D& P0,
                       int W,
                       int H,
                       int L);


    ThreatList* getThreats();

private:
    /**
   * @brief memLoadVolData - load volume data from memory
   * @param buffer - for volume data
   * @param bufferSize - size of data
   * @param volume - vtk volume pointer
   */
    void memLoadVolData(uint16_t* buffer,
                        int bufferSize,
                        vtkSmartPointer<vtkVolume> volume);

    void addBuildTime(int msBuildIncrement);

public slots:
    void onVolumeCreated(int msLoad);

signals:
    void readDataRange(double computedWidth,
                       double computedHeight,
                       double computedLength);

    void loadComplete(int msLoad);


    void memAllocationRequest(uint64_t memRequest,
                              uint64_t memAvailable);

    void volumeMeasureComplete();
private:
    int readVolFile(vtkSmartPointer<vtkImageData> imageData,
                    const QString filename);

    int initVolumeFromBuffer(vtkSmartPointer<vtkImageData> imageData,
                             uint16_t* buffer,
                             int sizeBuffer,
                             uint16_t clampVal,
                             uint16_t maxThreat);


    void showSettings();



    // Display the Border of a threat and the threat inside.
    void displayThreatAndBorder(int threatIndex);

    void initMeasuredVolumeText();

    void setMeasuredVolumeText(std::string& textStr);

    void sendDebugMessage(QString& message);

    double getBMPixelVolumeNew(SDICOS::Bitmap* pBMP);
    double getBMPixelVolume(SDICOS::Bitmap* pBMP);

    int                               m_initialBuildTime_ms;
    vtkVolume*                        m_volumeRef;
    vtkImageData*                     m_rawClippedImage;
    bool                              m_mtRender;
    QMutex*                           m_renderMutex;

    // Slabbing thickness related members
    vtkSmartPointer<vtkActor>         m_clip_box;
    bool                              m_slabbing;
    bool                              m_allowThreatMasking;
    double                            m_slabbingthickness;

    // ButtonWidget
    vtkSmartPointer<vtkButtonWidget>  m_buttonWidget[5];
    vtkSmartPointer<vtkActor2D>       m_button_text_actor[5];


    uint16_t                           m_meashuredVolumeStartPoint;
    vtkSmartPointer<vtkActor>          m_measuredVolumeActor;
    vtkSmartPointer<vtkTextActor>      m_measurementTextActor;
    vtkSmartPointer<vtkPlanes>         m_slabUnpackPlanes;
    vtkSmartPointer<vtkCubeSource>     m_clipCube;


    // Threat list
    ThreatList*                       m_threats;
    bool                              m_doReomveLaptop;


    //-----------------------
    // X, Y and Z axis slices
    int                                m_volSliceWidth;   // X
    int                                m_volSliceHeight;  // Y
    int                                m_volSliceDepth;   // Z
    double                             m_actual_bounds[6];

    //------------------------------
    QTime  m_eventTimer;
    QTime  m_initRenderTimer;
};

//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERER_H_
//------------------------------------------------------------------------------
