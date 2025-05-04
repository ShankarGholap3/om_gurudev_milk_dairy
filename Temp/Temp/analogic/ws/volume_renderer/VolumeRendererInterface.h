//------------------------------------------------------------------------------
// File:  VolumeRendererInterface.h
// Description: Interface to Volume Renderer Object
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERINTERFACE_H_
#define ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERINTERFACE_H_
//------------------------------------------------------------------------------
#include <stdint.h>
#include <QObject>

//----------------------------------------------------------------
#include "VolumeRendererConfig.h"
#include <analogic/ws/common.h>
#include <SDICOS/SdcsTemplateCT.h>
#include <cstdint>
#include <vector>
//------------------------------------------------------------------------------

#define DEFAULT_VALUE_OPACITY     0.5
#define DEFAULT_VALUE_CONTRAST    0.5
#define DEFAULT_VALUE_BRIGHTNESS  0.5

//-------------------------------------
// Brighten PHOTO Image (Right View).
#define PHOTOGRAPIC_OPACITY       0.8

//------------------------------------------------------------------------------
// begin analogic::workstation namespace
//------------------------------------------------------------------------------
namespace analogic
{

namespace workstation
{


class VolumeRendererWidget;

class VolumeRendererInterface
{
public:
  virtual ~VolumeRendererInterface(){}

  /**
   * @brief Init
   * @param ct
   * @param buffer
   * @param volDimensions(x = width in pixels ,y = height in pixels , z = number of slices)
   * @param roiStart  Pmin(x, y, z) = (0, x - 1), (0, y – 1), (0 , z - 1)
   * @param roiEnd    Pmax(x, y, z) = (0, x - 1), (0, y – 1), (0 , z - 1)
   * @param tdrs
   * @param vecDisplayThreatIdentifier vector[pair <int , std::string> ]
   * @param configObject
   */
  virtual void  Init(SDICOS::CTModule* ct,
                     uint16_t* buffer,
                     const QVector3D& volDimensions,
                     const QVector3D& roiStart,
                     const QVector3D& roiEnd,
                     std::vector<SDICOS::TDRModule*> tdrs,
                     const std::vector<std::pair<int, std::string> >& vecDisplayThreatIdentifier,
                     analogic::workstation::VolumeRendererConfig* configObject) = 0;

  /**
   * @brief SetContrast
   * @param value
   * Method to set Volume Contrast
   */
  virtual void SetContrast(float value) = 0;

  /**
   * @brief SetBrightness
   * @param value
   * Method to set Volume Opacity
   */
  virtual void SetBrightness(float value) = 0;

  /**
   * @brief SetOpacity
   * @param value
   * Metiod to set Volume  Brightness
   */
  virtual void SetOpacity(float value) = 0;

  /**
  * @brief SetInverse
  * @param value
  * Turn on/off the black background
  */
  virtual void  SetInverse(bool value) = 0;

  /**
   * @brief RenderWithColor
   * @param doColor
   * Based on the doColor flag the VTK volume is rendered in color or grayscale.
   * doColor = true  -> HSV TF color map is used.
   * doColor = false -> A standard gray-scale color map is used.
   */
  virtual void RenderWithColor(bool doColor) = 0;

  /**
   * @brief SetHighlightedMaterials
   * @param list
   * Selects which materials are colorized (organic, inorganic, metal).
   */
  virtual void  SetHighlightedMaterials(std::vector<MaterialEnumeration> list) = 0;

  /**
   * @brief SetMaterialFilterCutoff
   * @param double material_cutoff_loc.
   *   0 corresponds to the lowest signal value (air or lower).
   *   1 corresponds to the highest signal value (above metal).
   */
  virtual void  SetMaterialFilterCutoff(double material_cutoff) = 0;


  virtual bool GetMaterialFilterActivityStatus()  = 0;

  // ARO-NOTE: We might not need this method.
  virtual void SetMaterialFilterActivityStatus(bool setVal)  = 0;


  /**
   * @brief EnableLowDensity
   * @param setVal
   * Turn On/Off the low density values in the scan for a particular analysis.
   */
  virtual void EnableLowDensity(bool setVal) = 0;


  /**
   * @brief SetZoom
   * @param value
   * Set the zoom rate for the VTK windows
   */
  virtual void SetZoom(float value) = 0;

  /**
   * @brief SetThreatVisibility
   * @param pTdr
   * @param rawIndex = index used to access threat list
   * @param visible
   * @param keepOrientation    = true == don't rotate
   * @param threatTypeIndex    = relative index provided for the threat type (THREAT/LAPTOP)
   * @param threatTypeTotal    = relative count LAPTOP/THREAT
   */
  virtual void SetThreatVisibility(SDICOS::TDRModule *pTdr,
                           int rawIndex,
                           bool visible,
                           bool keepOrientation,
                           int  threatTypeIndex,
                           int  threatTypeTotal) = 0;

  /**
  * @brief ResetParameters
  * Resets the view rotation, zoom and the Volume Renderer parameters such as opacity and color transfer functions.
  */
  virtual void ResetParameters() = 0;

  /**
   * @brief ResetDisplay
   * Re-orient the volume to the initial rotation and centering.
   */
  virtual void ResetDisplay(bool doSync) = 0;

  /**
   * @brief RemoveCurrentBag
   * Clears the Volume Renderer viewer in preparation for a new bag
   */
  virtual void RemoveCurrentBag() = 0;

  /**
   * @brief RemoveSplashIfPresent
   * if there is a splash screen remove it.
   */
  virtual void RemoveSplashIfPresent() = 0;

  /**
  * @brief SetLinkSiblingsList
  * @param sibList = std::list of the VolumeRendererWidget who are kept synchronized by zoom and rotation
  *     actions
  */
  virtual void SetLinkSiblingsList(std::list<VolumeRendererWidget*> sibList) = 0;

  /**
  * @brief SetSyncEnabled
  */
  virtual void  SetSyncEnabled(bool enableStatus) = 0;

  /**
   * @brief SetScannerDirectionLeftToRight
   * @param scanLeftToRight
   *  True: bags move from left to right
   *  False: bags move from right to left.
   */
  virtual void SetScannerDirectionLeftToRight(bool scanLeftToRight) = 0;

  /**
  * @brief DisplayThreatOnly
  * @param showBMPOnly true: in Threat window only show threat TDR BMP values.
  *     false: in Threat window show threat and non-threat TDR BMP values.
  */
  virtual void DisplayThreatOnly(bool  showBMPOnly) = 0;

  /**
   * @brief ThreatCount
   * Returns the number of threats in a bag
   */
  virtual int ThreatCount() = 0;


  /**
  * @brief CurrentThreat
  * Returns the index of the current threat (ZERO based)
  */
  virtual int CurrentThreat() = 0;

  /**
   * @brief ClearCurrentThreat
   * Clears the current threat.
   */
  virtual void  ClearCurrentThreat() = 0;


  /**
   * @brief SuspectCurrentThreat
   * Suspects the Current Threat.
   * @param threat type index of current treat in list TREAT/LAPTOP
   * @param total of threat type THREAT/LAPTOP
   */
  virtual void SuspectCurrentThreat(int threatTypeIndex,
                                    int threatTypeTotal) = 0;

  /**
   * @brief InitPickBoxWidget
   * Create the VTK widget for sub volume
   */
  virtual void InitPickBoxWidget() = 0;

  /**
    * @brief StartOperatorThreatGeneration
    * Begin OPERATOR threat creation will commence on the next
    * touch point or mouse click.
    */
   virtual void StartOperatorThreatGeneration() = 0;


  /**
   * @brief RightAcceptOperatorGeneratedThreat
   * @param threatType
   * @param p0Ret
   * @param extentRet
   * @return new threat ID on success and -1 on error
   * @note This threat uses the RIGHT vier as the rhreat window for creating the threat.
   */
  virtual int RightAcceptOperatorGeneratedThreat(QString& threatType,
                                         QVector3D& p0Ret,
                                         QVector3D& extentRet) = 0;


   /**
    * @brief CancelOperatorThreatGeneration
    * Stops the whole process of Operator Threat Generation and returns back to the
    * precious state (clearing any visual objects if created).
    */
   virtual void CancelOperatorThreatGeneration() = 0;

  /**
    * @brief ExpandOperatorThreatVolume
    * Enlarges the current Operator Threat Volume size
    */
   virtual void ExpandOperatorThreatVolume() = 0;

  /**
   * @brief ShrinkOperatorThreatVolume
   * Shrinks the current Operator Threat Volume size
   */
  virtual void ShrinkOperatorThreatVolume() = 0;

  /**
  * @brief NextThreat
  * @param index = the threat used to determine the "Next threat".
  *  The next threat will be the threat with a higher index that is not cleared or the
  *  first threat that isn't cleared if none fit the above description.
  *
  * Returns the index of the "Next threat" (ZERO based).  Skips cleared threats.
  */
  virtual int NextThreat(int index) = 0;

  /**
  * @brief SetDisplayLabel
  * @param displayLab
  * This method will identify the widget name for logging and other identification purposes
  */
  virtual void SetDisplayLabel(const QString displayLab) = 0;

  /**
   * @brief ShowViewLabel
   * @param doShow
   * This will allow the volume renderer widget to enable/disable the showing of the volume  information
   * label
   */
  virtual void ShowViewLabel(bool doShow)  = 0;

  /**
   * @brief ShowThreatView
   * @param doShow
   * This will allow the Volume renderer widget to not show the associated threat widget
   */
  virtual void ShowThreatView(bool doShow) = 0;


  /**
   * @brief SetEnableSplashScreen
   * @param doSplash
   * This will determine if the Renderer shows a sharpened splash screen at startup.
   */
  virtual void SetEnableSplashScreen(bool doSplash)  = 0;

  /**
   * @brief SetEnableDynamicSplash
   * @param doDynamicSplash
   */
  virtual void SetEnableDynamicSplash(bool doDynamicSplash) = 0;

  /**
   * @brief SetLightingParameters
   * @param sceneAmbient
   * @param sceneDiffuse
   * @param sceneSpecularbutton
   *
   * Set the lighting parameters for the VTK volume widgget
   */
  virtual void SetLightingParameters(double sceneAmbient,
                             double sceneDiffuse,
                             double sceneSpecular) = 0;

  /**
   * @brief AppendDisplayText
   * @param list
   * List of strings to be added to the VTK View Label.
   * Called after Init();
   */
  virtual void AppendDisplayText(QStringList& list) = 0;

  /**
   * @brief ShowThreatUserDecision;
   * @param userdecision
   * The user decisionmadefor the given threat
   * Called from SVS.
   */
  virtual void ShowThreatUserDecision(QString& userDecision) = 0;


  /**
   * @brief SetEnableLaptopRemoval
   * @param doEnable
   */
  virtual void SetEnableLaptopRemoval(bool doEnable) = 0;

  /**
   * @brief Slabbing
   * @param flag
   * @param thickness
   */
  virtual void SetSlabThickness(double thickness) = 0;

  /**
   * @brief SetSlabUnpackPosition
   * @param value {0 - 100}
   * @param axis {x, y, })
   * @param isSlab is this a slab or unpacking
   */
  virtual void SetSlabUnpackPosition(double value,
                                    VREnums::AXIS axis,
                                    bool isSlab) = 0;

  /**
   * @brief SetSlabViewType
   * @param newSetSlabType  {BAG_VIEW , THREAT_VIEW, LAPTOP_VIEW}
   */
  virtual void SetSlabViewType(VREnums::SlabViewType newSetSlabType)  = 0;



  /**
   * @brief SetCutViewAxis
   * @param axis
   */
  virtual void SetCutViewAxis(VREnums::AXIS axis) =  0;

  virtual void SetAsThreatRenderer(bool setVal) = 0;


  /**
   * @brief SetRightViewerDisplayMode
   * @param aMode
   *  aMode can be Threat, Surface
   */
  virtual void SetRightViewerDisplayMode(VREnums::VRViewModeEnum aMode) = 0;

  /**
   * @brief SetVTKDisplayUnit
   * @param dispUnit = CM or IN
   * NOTE: Thhis method isbeing DEPRICATED
   */
  virtual void SetVTKDisplayUnit(VREnums::DispalyUnitEnum dispUnit) = 0;

  /**
   * @brief SetVTKDisplayUnitSystem
   * @param unitSystem = METRIC or US_IMPERIAL
   * Mass/Weight dislplayed in VTK viewr.
   */
  virtual void SetVTKDisplayUnitSystem(VREnums::DispalyUnitSystemEnum unitSystem) = 0;

  /**
   * @brief DeleteOperatorThreat
   * @param threatindex
   */
  virtual void DeleteOperatorThreat(int threatIndex)  = 0;

  /**
   * @brief RepaintViewers
   */
  virtual void RepaintViewers() = 0;

  /**
   * @brief UpdateRenderer
   */
  virtual void UpdateRenderer() = 0;

  /**
   * @brief SuspectBag
   */
  virtual void SuspectBag()  = 0;

  /**
   * @brief NotifyDetectionException
   */
  virtual void NotifyDetectionException() = 0;

  /**
   * @brief SetToggleThreatColorization
   * @param setValue
   */
  virtual void SetToggleThreatColorization(bool setValue) = 0;

  /**
   * @brief MeasureVolumeByValue
   * The user selected point is the seed used to estimate the volume.
   * This is only Valid for the THREAT window.
   *
   * This is based on a toggle button.
   *  setVal = TRUE on button armed
   *  setVal = FALSE on button disarmned.
   */
  virtual void MeasureVolumeByValue(bool setVal) = 0;

  /**
   * @brief SetEnableDistanceMeasurement
   * @param setVal TRUE turns ON. FLSE turns OFF. 
   * Two selected points in the VOLUME define a fine 
   * grained distance measurement.
   */
  virtual void SetEnableDistanceMeasurement(bool setVal) = 0;

  /**
   * @brief SetVoumeMeasurementStartRange
   * @param minVal = the minimum start value for volume measuremnt calculations.
   * @param maxVal = the maximum start value for volume measuremnt calculations.
   */
  virtual void SetVoumeMeasurementStartRange(uint16_t minVal,
                                             uint16_t maxVal)  = 0;

  /**
   * @brief SlabNonThreatLaptop
   * @param laptopID = id of first "current" non-threat LAPTOP
   */
  virtual void SlabNonThreatLaptop(int laptopID)  = 0;
  /**
   * @brief SetEnableThreatRedering
   * @param setVal = allow threat rendering when set to true.
   */
  virtual void SetEnableThreatRedering(bool setVal)  = 0;
};


//------------------------------------------------------------------------------
}  // namespace workstation

}  // namespace analogic
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERINTERFACE_H_
//------------------------------------------------------------------------------
