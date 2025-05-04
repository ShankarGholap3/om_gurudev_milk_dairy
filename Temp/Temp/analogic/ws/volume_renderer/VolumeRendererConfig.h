//------------------------------------------------------------------------------
// File: VolumeRendererConfig.h
// Description: The header file for volume rendrerer configuration
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef  ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERCONFIG_H_
#define  ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERCONFIG_H_
//------------------------------------------------------------------------------
#include <QString>
#include <QTime>
#include <string>
#include <map>
#include "boost/filesystem.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include "boost/program_options.hpp"
#include "VRUtils.h"
#include "ScannerDefs.h"




//------------------------------------------------------------------------------
// begin analogic::workstation namespace
//------------------------------------------------------------------------------
namespace analogic
{

namespace workstation
{




#define MIN_FILE_READLN_LEN   3
#define VOL_COLOR_DELIM      ","
#define VOL_COLOR_COMMENT    "#"


class VolumeRendererConfig
{
public:
  /**
   * Creates an empty config instance to be populated later.
   */
  VolumeRendererConfig(void);

  /**
   * Reads and populates the config values from their respective files.
   *
   * Throws excprtion if :
   *  1. The file cannot be opened for read.
   *  2. The file is not found.
   *  3. The file is corrupted one of read initializeRightVRWcontaier class size is zero.
   *  4. If one of the required internal settings is missing from the internals
   *     settings file:
   *     <dev root>/workstation/resources/volume_renderer/InternalVolumeSettings.txt
   */
  /**
   * @brief VolumeRendererConfig
   * @param mapFileDirectory
   * @param opacityFileList
   * @param HSVMapFileList
   * @param grayscaleValuesAndOpacityFilePath
   * @param materialTransitionFilePath
   * @param settingFilePath
   * @param enableThreats
   * @param useShading
   * @param useLinearInterpolation
   * @param allowOrganicRemoval
   * @param allowInorganicRemoval
   * @param allowMetalRemoval
   */
  VolumeRendererConfig(const std::string& mapFileDirectory,
                       std::vector<std::string>& opacityFileList,
                       std::vector<std::string>& HSVMapFileList,
                       const std::string& grayscaleValuesAndOpacityFilePath,
                       const std::string& materialTransitionFilePath,
                       const std::string& settingFilePath,
                       const bool enableThreats          = true,
                       const bool useShading             = true,
                       const bool useLinearInterpolation = false,
                       const bool allowOrganicRemoval    = false,
                       const bool allowInorganicRemoval  = false,
                       const bool allowMetalRemoval      = false);


  ~VolumeRendererConfig(void);

   std::list<OpacitySigVal>                m_volumeOpacityList;
   std::list<HSVColorSigVal>               m_volumeHsvList;

   std::list<OpacitySigVal>                m_lowDensityOpacityList;
   std::list<HSVColorSigVal>               m_lowDensityHsvList;

   // Performance Optimization related changes
   std::map<double, std::list<HSVColorSigVal>>             m_threatOffsetHsvMap;
   std::map<double, std::list<OpacitySigVal>>              m_threatOffsetOpacityMap;

   std::map<MaterialTransition_E, double>  m_materialTransitionMap;
   bool                                    m_useShading;
   bool                                    m_useLinearInterpolation;
   bool                                    m_allowOrganicReomval;
   bool                                    m_allowInorganicReomval;
   bool                                    m_allowMetalReomval;
   double                                  m_opacityModifyScalar;
   double                                  m_contrastModifyScalar;
   double                                  m_brightnessModifyScalar;

   //--------------------------------------------------
   // Not all pich evens need to be handeled as
   // they are quite frequent. Threse thresholds allow
   // some touch events to go unhandleed.

   // Threshold for pinch
   uint32_t m_pinchThreshold;

   // Threshold for spread
   uint32_t m_spreadThreshold;

   // Threshold for move
   uint32_t m_moveThreshold;
   //--------------------------------------------------

   // The lower ZoomRate at which tre ruler stops updating.
   // Where zoom rate = (initial ruler distance/(current ruler distaqnce)
   double   m_zoomDisplayLowerLimit;

   // The upper ZoomRate at which tre ruler stops updating.
   // Where zoom rate = (initial ruler distance/(current ruler distaqnce)
   double   m_zoomDisplayUpperLimit;

   // Ratio of finger distance for a pinch IN to become active.
   double   m_scaleInThreshold;

   // Ratio of finger distance for a pinch OUT to become active.
   double   m_scaleOutThreshold;

   // QVTKWidget::wheelEvent default input for zoom in or zoom out.
   int32_t  m_zoomWheelDelta;

   // If true the GPU rendiring is used if not the CPU is used.
   bool     m_useGPU;
   bool     m_enableThreatHandling;
   bool     m_showThreatMass;
   bool     m_showTIP;
   bool     m_isTIPBag;

    GrayscaleValueOpacityStruct m_grayScaleSettings;


private:
   std::string              m_mapFileDireectory;
   std::vector<std::string> m_opacityFileList;
   std::vector<std::string> m_hsvColorMapFileList;


   std::string m_grayscalevaluesAndOpacityFilePath;
   std::string m_materialTransitionFilePath;
   std::string m_internalSettingsFilePath;



   bool m_isValid;


   bool readOpacityFile(std::string& filePath,
                        std::list<OpacitySigVal>& opacityList);

   bool readHSVColorFile(std::string& filePath,
                         std::list<HSVColorSigVal>& colorList);

   bool readGrayScaleSettings(std::string filePath,
                              GrayscaleValueOpacityStruct& grayScaleSettings);

   bool readTransitionFile(std::string& filePath,
                           std::map<MaterialTransition_E, double>& transitionMap);


   bool appendOpacityFile(std::string& filePath,
                           std::list<OpacitySigVal>& opacityList,
                           double offset);

   bool appendHSVColorFile(std::string& filePath,
                           std::list<HSVColorSigVal>& clorList,
                           double offset);


   bool readInternalSettings(void);


   QString transitionName(MaterialTransition_E transitionE);

   void throwExeption(const std::exception& error);
};



//------------------------------------------------------------------------------
}  // namespace workstation

}  // namespace analogic
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
#endif  //  ANALOGIC_WS_VOLUME_RENDERER_VOLUMERENDERERCONFIG_H_
//------------------------------------------------------------------------------
