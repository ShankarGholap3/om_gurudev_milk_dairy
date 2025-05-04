//-----------------------------------------------------------------------------
// File: VolumeRendererConfig.cpp
// Descriptuion: Volume Rencderer config class implementation.
// Copyright 2016 Analogic Corp.
//-----------------------------------------------------------------------------
#include <QString>
#include <QStringList>
#include <iostream>
#include <system_error>
#include "Logger.h"
#include "VolumeRendererConfig.h"

//---------------------------------------------
// ARO-Added gor low density.
// g_TF_config_basis  = Number of Thranfer fuinctions that are NOT threats in
// the config file list.
const int g_TF_config_basis = 2;


//------------------------------------------------------------------------------
// begin analogic::workstation namespace
//------------------------------------------------------------------------------
namespace analogic
{

namespace workstation
{


const std::string g_default_VR_SettingsFolder = "volumeRenderer";
//-----------------------------------------------------------------------------
VolumeRendererConfig::VolumeRendererConfig():
  m_useShading(true),
  m_useLinearInterpolation(true),
  m_allowOrganicReomval(true),
  m_allowInorganicReomval(true),
  m_allowMetalReomval(true),
  m_opacityModifyScalar(1.0),
  m_contrastModifyScalar(1.0),
  m_brightnessModifyScalar(1.0),
  m_pinchThreshold(DEFAULT_PINCH_THRESHOLD),
  m_spreadThreshold(DEFAULT_SPREAD_THRESHOLD),
  m_moveThreshold(DEFAULT_MOVE_THRESHOLD),
  m_zoomDisplayLowerLimit(DEF_ZOOM_DISP_LOWER_LIMIT),
  m_zoomDisplayUpperLimit(DEF_ZOOM_DISP_UPPER_LIMIT),
  m_scaleInThreshold(DEFAULT_SCALE_IN_THRESHOLD),
  m_scaleOutThreshold(DEFAULT_SCALE_IN_THRESHOLD),
  m_zoomWheelDelta(DEF_ZOOM_WHEEL_DELTA),
  m_useGPU(true),
  m_enableThreatHandling(false),
  m_showThreatMass(true),
  m_showTIP(false),
  m_isTIPBag(false),
  m_mapFileDireectory(""),
  m_materialTransitionFilePath(""),
  m_internalSettingsFilePath(""),
  m_isValid(false)
{
}



//-----------------------------------------------------------------------------
VolumeRendererConfig::VolumeRendererConfig(const std::string& mapFileDirectory,
                                            std::vector<std::string>& opacityFileList,
                                            std::vector<std::string>& HSVMapFileList,
                                            const std::string& grayscaleValuesAndOpacityFilePath,
                                            const std::string& materialTransitionFilePath,
                                            const std::string& internalSettingsFilePath,
                                            const bool enableThreats,
                                            const bool useShading,
                                            const bool useLinearInterpolation,
                                            const bool allowOrganicRemoval,
                                            const bool allowInorganicRemoval,
                                            const bool allowMetalRemoval):

  m_useShading(useShading),
  m_useLinearInterpolation(useLinearInterpolation),
  m_allowOrganicReomval(allowOrganicRemoval),
  m_allowInorganicReomval(allowInorganicRemoval),
  m_allowMetalReomval(allowMetalRemoval),
  m_opacityModifyScalar(1.0),
  m_contrastModifyScalar(1.0),
  m_brightnessModifyScalar(1.0),
  m_pinchThreshold(DEFAULT_PINCH_THRESHOLD),
  m_spreadThreshold(DEFAULT_SPREAD_THRESHOLD),
  m_moveThreshold(DEFAULT_MOVE_THRESHOLD),
  m_zoomDisplayLowerLimit(DEF_ZOOM_DISP_LOWER_LIMIT),
  m_zoomDisplayUpperLimit(DEF_ZOOM_DISP_UPPER_LIMIT),
  m_scaleInThreshold(DEFAULT_SCALE_IN_THRESHOLD),
  m_scaleOutThreshold(DEFAULT_SCALE_IN_THRESHOLD),
  m_zoomWheelDelta(DEF_ZOOM_WHEEL_DELTA),
  m_useGPU(true),
  m_enableThreatHandling(enableThreats),
  m_showThreatMass(true),
  m_mapFileDireectory(mapFileDirectory),
  m_opacityFileList(opacityFileList),
  m_hsvColorMapFileList(HSVMapFileList),
  m_grayscalevaluesAndOpacityFilePath(grayscaleValuesAndOpacityFilePath),
  m_materialTransitionFilePath(materialTransitionFilePath),
  m_internalSettingsFilePath(internalSettingsFilePath),
  m_isValid(false)
{
  LOG(INFO) << "Normal OPACITY File List:";
  std::vector<std::string>::iterator iter;
  for (iter = m_opacityFileList.begin(); iter != m_opacityFileList.end(); iter++)
  {
    std::string strI = *iter;
    LOG(INFO) << strI;
  }

  LOG(INFO) << "Normal HSV ColorMap File List:";
  for (iter = m_hsvColorMapFileList.begin(); iter != m_hsvColorMapFileList.end(); iter++)
  {
    std::string strI = *iter;
    LOG(INFO) << strI;
  }


  LOG(INFO) << "Material Transition      : " << m_materialTransitionFilePath;
  LOG(INFO) << "Internal Settings        : " << m_internalSettingsFilePath;


  //------------------------------------------------
  // Read Normal &  Threat Opacitty and Color TFs
  QTime timeRead = QTime::currentTime();
  std::string volumeOpacityFilePath = mapFileDirectory;
  volumeOpacityFilePath += m_opacityFileList.at(0);
  this->readOpacityFile(volumeOpacityFilePath, m_volumeOpacityList);

  std::string volumeHSVColorMapFilePath = mapFileDirectory;
  volumeHSVColorMapFilePath += m_hsvColorMapFileList.at(0);
  this->readHSVColorFile(volumeHSVColorMapFilePath, m_volumeHsvList);


  //------------------------------------------------
  // Read Low Density Opacitty and Color TFs
  std::string lowDensityOpacityFilePath = mapFileDirectory;
  lowDensityOpacityFilePath += m_opacityFileList.at(1);
  this->readOpacityFile(lowDensityOpacityFilePath, m_lowDensityOpacityList);

  std::string lowDensityHSVColorMapFilePath = mapFileDirectory;
  lowDensityHSVColorMapFilePath += m_hsvColorMapFileList.at(1);
  this->readHSVColorFile(lowDensityHSVColorMapFilePath, m_lowDensityHsvList);




  //------------------------------------------------
  // Read the material transition file
  this->readTransitionFile(m_materialTransitionFilePath,
                           m_materialTransitionMap);

  // Define the Threat Offset from tje TransitionMap.
  std::vector<double> threatOffsetList =
  {
    // TO EXPAND JUST ADD ITEM
    m_materialTransitionMap[SIG_THREAT_SET_A],
    m_materialTransitionMap[SIG_THREAT_SET_B],
    m_materialTransitionMap[SIG_THREAT_SET_C],
    m_materialTransitionMap[SIG_THREAT_SET_D],
    m_materialTransitionMap[SIG_THREAT_SET_E],
    m_materialTransitionMap[SIG_THREAT_SET_F],
  };


  //----------------------------------------
  // Read the grayscaleValueOpacity list
  this->readGrayScaleSettings(m_grayscalevaluesAndOpacityFilePath,
                              m_grayScaleSettings);

  //----------------------------------------

  unsigned int nThreatTypes  = m_opacityFileList.size() - g_TF_config_basis;
  if  (nThreatTypes > threatOffsetList.size())
  {
    nThreatTypes = threatOffsetList.size();
  }

  // Apply any defined threat opacity and color maps
  if ( nThreatTypes > 0 )
  {
    for (unsigned int i = 0; i < nThreatTypes; i++)
    {
      double sigOffset = threatOffsetList.at(i);
      std::string strFileOpacity  = m_mapFileDireectory  + m_opacityFileList.at(i + g_TF_config_basis);
      std::string strFioleHSVColor = m_mapFileDireectory + m_hsvColorMapFileList.at(i + g_TF_config_basis);
      this->appendOpacityFile(strFileOpacity,    m_volumeOpacityList, sigOffset);
      this->appendHSVColorFile(strFioleHSVColor, m_volumeHsvList,     sigOffset);
    }
  }


 //------------------------------------------------
 // Read internal setings
 this->readInternalSettings();
 int msRead = timeRead.elapsed();
 QString readMessage = "*TTTTTT* - Time to read all TF, Mat Trans, and Settings (ms):" + QString::number(msRead);
 std::string stdMsg = readMessage.toLocal8Bit().data();

 LOG(INFO) << stdMsg;

 m_isValid = true;
}

//-----------------------------------------------------------------------------
VolumeRendererConfig::~VolumeRendererConfig()
{
  // Clear the maps
  m_threatOffsetHsvMap.clear();
  m_threatOffsetOpacityMap.clear();
}


//-----------------------------------------------------------------------------
void VolumeRendererConfig::throwExeption(const std::exception& error)
{
  throw std::runtime_error(error.what());
}



//------------------------------------------------------------------------------
bool VolumeRendererConfig::readOpacityFile(std::string& filePath,
                                           std::list<OpacitySigVal>& opacityList)
{
  LOG(INFO) << "**** Begin read opacity file:" << filePath;
  std::string inFilename = filePath;
  std::ifstream inFile(inFilename.c_str());
  if ( !inFile )
  {
    std::string msg;
    msg = QObject::tr("Could not open opacity file '").toStdString() + filePath + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  try
  {
    //--------------------------------------
    // Read header and throw it away.
    std::getline(inFile, str);
    //-------------------------------------
    while (std::getline(inFile, str))
    {
      if ( str.length() > MIN_FILE_READLN_LEN)
      {
        QString sEntry = str.c_str();
        int indexComent =  sEntry.indexOf(VOL_COLOR_COMMENT);
        QStringList strList = sEntry.split(VOL_COLOR_DELIM);
        // qDebug() << sEntry;
        if ( (strList.length() >= 2) &&
             (indexComent < 0      )
             )
        {
          OpacitySigVal opSig;
          opSig.m_signalVal = strList.at(0).toDouble();
          opSig.m_opacity   = strList.at(1).toDouble();
          opacityList.push_back(opSig);
          std::string message = opSig.toString();
          LOG(INFO) << message;
        }
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }

  if (opacityList.size() == 0 )
  {
    std::string errorMessage = QObject::tr("List size == 0, Possibly a corrupted file:").toStdString() +
        filePath;
    throw std::logic_error(errorMessage);
  }
  return true;
}



//------------------------------------------------------------------------------
bool VolumeRendererConfig::readHSVColorFile(std::string& filePath,
                                            std::list<HSVColorSigVal>& colorList)
{
  LOG(INFO) << "**** Begin read Color TF file:" << filePath;
  std::string inFilename = filePath;
  std::ifstream inFile(inFilename.c_str());
  if ( !inFile )
  {
    std::string msg;
    msg = QObject::tr("Could not open HSV color file '").toStdString() + filePath + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  //--------------------------------------
  // Read header and throw it away.
  std::getline(inFile, str);
  //-------------------------------------
  try
  {
    while (std::getline(inFile, str))
    {
      if ( str.length() > MIN_FILE_READLN_LEN)
      {
        QString sEntry = str.c_str();
        int indexComent =  sEntry.indexOf(VOL_COLOR_COMMENT);
        QStringList strList = sEntry.split(VOL_COLOR_DELIM);
        if ( (strList.length() >= 4) &&
             (indexComent < 0      )
             )
        {
          HSVColorSigVal colorVal;
          colorVal.m_signalVal = strList.at(0).toDouble();
          colorVal.m_h         = strList.at(1).toDouble();
          colorVal.m_s         = strList.at(2).toDouble();
          colorVal.m_v         = strList.at(3).toDouble();
          colorList.push_back(colorVal);
          std::string message = colorVal.toString();
          LOG(INFO) << message;
        }
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }

  if (colorList.size() == 0 )
  {
    std::string errorMessage = QObject::tr("List size == 0, Possibly a corrupted file:").toStdString() +
        filePath;
    throw std::logic_error(errorMessage);
  }
  return true;
}

//------------------------------------------------------------------------------
bool VolumeRendererConfig::appendOpacityFile(std::string& filePath,
                        std::list<OpacitySigVal>& opacityList,
                        double offset)
{
    Q_UNUSED(opacityList);
  std::list<OpacitySigVal> appendList;
  bool bReadNew = this->readOpacityFile(filePath, appendList);
  if (!bReadNew)
  {
    QString message = "**YY** ERROR reading Opacity Map for appending to main opacity map";
    message += filePath.c_str();
    LOG(INFO) <<  message.toLocal8Bit().data();
    return false;
  }

  // Performance Optimization related changes
  // Read threat opacity from file and add it to map with offset as key
  m_threatOffsetOpacityMap[offset] = appendList;

  return true;
}

//------------------------------------------------------------------------------
bool VolumeRendererConfig::appendHSVColorFile(std::string& filePath,
                        std::list<HSVColorSigVal>& clorList,
                        double offset)
{
    Q_UNUSED(clorList);
  std::list<HSVColorSigVal> appendList;
  bool bReadNew = this->readHSVColorFile(filePath, appendList);
  if (!bReadNew)
  {
    QString message = "**YY** ERROR reading Opacity Map for appending to main opacity map";
    message += filePath.c_str();
    LOG(INFO) <<  message.toLocal8Bit().data();
    return false;
  }

  // Performance Optimization related changes
  // Read threat color from file and add it to map with offset as key
  m_threatOffsetHsvMap[offset] = appendList;

  return true;
}


//------------------------------------------------------------------------------
bool VolumeRendererConfig::readGrayScaleSettings(std::string filePath,
                                                 GrayscaleValueOpacityStruct& grayScaleSettings)
{
  std::string inFilename = filePath;
  std::ifstream inFile(inFilename.c_str());
  if (!inFile)
  {
    std::string msg;
    msg = QObject::tr("Could not open material transitions file '").toStdString() + filePath + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  //--------------------------------------
  // Read header and throw it away.
  std::getline(inFile, str);

  try
  {
    //-------------------------------------
    while (std::getline(inFile, str))
    {
      if ( str.length() > MIN_FILE_READLN_LEN)
      {
        QString sEntry = str.c_str();
        int indexComent =  sEntry.indexOf(VOL_COLOR_COMMENT);
        QStringList strList = sEntry.split(VOL_COLOR_DELIM);
        if ( (strList.length() >= 2) &&
             (indexComent < 0      )
             )
        {
          //=========================================================
          // *** ORGANIC
          //--------------
          // V ORGANIC MIN
          if (strList.at(0).compare("GRAY_HSV_V_ORGANIC_MIN") == 0)
          {
            grayScaleSettings.m_HSV_V_organicMin = strList.at(1).toDouble();
          }
          //--------------
          // V ORGANIC MAX
          if (strList.at(0).compare("GRAY_HSV_V_ORGANIC_MAX") == 0)
          {
            grayScaleSettings.m_HSV_V_organicMax = strList.at(1).toDouble();
          }
          //---------------
          // V ORGANIC MIN INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_ORGANIC_MIN_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_organicMin_inv = strList.at(1).toDouble();
          }
          //---------------
          // V ORGANIC MAX INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_ORGANIC_MAX_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_organicMax_inv = strList.at(1).toDouble();
          }
          //---------------
          // OPACITY ORGANIC MIN
          if (strList.at(0).compare("GRAY_OPACITY_ORGANIC_MIN") == 0)
          {
            grayScaleSettings.m_opacityOrganicMin = strList.at(1).toDouble();
          }
          // OPACITY ORGANIC MAX
          if (strList.at(0).compare("GRAY_OPACITY_ORGANIC_MAX") == 0)
          {
            grayScaleSettings.m_opacityOrganicMax = strList.at(1).toDouble();
          }

          //=========================================================
          // *** INORGANIC
          //--------------
          // V INORGANIC MIN
          if (strList.at(0).compare("GRAY_HSV_V_INORGANIC_MIN") == 0)
          {
            grayScaleSettings.m_HSV_V_inorganicMin = strList.at(1).toDouble();
          }
          //--------------
          // V INORGANIC MAX
          if (strList.at(0).compare("GRAY_HSV_V_INORGANIC_MAX") == 0)
          {
            grayScaleSettings.m_HSV_V_inorganicMax = strList.at(1).toDouble();
          }
          //--------------
          // V INORGANIC MIN INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_INORGANIC_MIN_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_inorganicMin_inv = strList.at(1).toDouble();
          }
          //--------------
          // V INORGANIC MAX INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_INORGANIC_MAX_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_inorganicMax_inv = strList.at(1).toDouble();
          }
          //--------------
          // OPACITY INORGANIC MIN
          if (strList.at(0).compare("GRAY_OPACITY_INORGANIC_MIN") == 0)
          {
            grayScaleSettings.m_opacityInorganicMin = strList.at(1).toDouble();
          }
          // OPACITY INORGANIC MAX
          if (strList.at(0).compare("GRAY_OPACITY_INORGANIC_MAX") == 0)
          {
            grayScaleSettings.m_opacityInorganicMax = strList.at(1).toDouble();
          }

          //=========================================================
          // *** METAL
          //--------------
          // V METAL MIN
          if (strList.at(0).compare("GRAY_HSV_V_METAL_MIN") == 0)
          {
            grayScaleSettings.m_HSV_V_metalMin = strList.at(1).toDouble();
          }
          //--------------
          // V METAL MAX
          if (strList.at(0).compare("GRAY_HSV_V_METAL_MAX") == 0)
          {
            grayScaleSettings.m_HSV_V_metalMax = strList.at(1).toDouble();
          }
          //--------------
          // V METAL MIN INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_METAL_MIN_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_metalMin_inv = strList.at(1).toDouble();
          }
          //--------------
          // V METAL MAX INVERSE
          if (strList.at(0).compare("GRAY_HSV_V_METAL_MAX_INV") == 0)
          {
            grayScaleSettings.m_HSV_V_metalMax_inv = strList.at(1).toDouble();
          }
          // OPACITY METAL MIN
          if (strList.at(0).compare("GRAY_OPACITY_METAL_MIN") == 0)
          {
            grayScaleSettings.m_opacityMetalMin = strList.at(1).toDouble();
          }
          // OPACITY METAL MAX
          if (strList.at(0).compare("GRAY_OPACITY_METAL_MAX") == 0)
          {
            grayScaleSettings.m_opacityMetalMax = strList.at(1).toDouble();
          }
        }
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }
  return true;
}

//------------------------------------------------------------------------------
bool VolumeRendererConfig::readTransitionFile(std::string& filePath,
                                              std::map<MaterialTransition_E, double>& transitionMap)
{
  std::string inFilename = filePath;
  std::ifstream inFile(inFilename.c_str());
  if (!inFile)
  {
    std::string msg;
    msg = QObject::tr("Could not open material transitions file '").toStdString() + filePath + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  //--------------------------------------
  // Read header and throw it away.
  std::getline(inFile, str);

  try
  {
    //-------------------------------------
    while (std::getline(inFile, str))
    {
      if ( str.length() > MIN_FILE_READLN_LEN)
      {
        QString sEntry = str.c_str();
        int indexComent =  sEntry.indexOf(VOL_COLOR_COMMENT);
        QStringList strList = sEntry.split(VOL_COLOR_DELIM);
        if ( (strList.length() >= 2) &&
             (indexComent < 0      )
             )
        {
          //--------------
          // LOW DENSITY
          if (strList.at(0).compare(this->transitionName(SIG_LOW_DENSITY_MIN)) == 0)
          {
            transitionMap[SIG_LOW_DENSITY_MIN] = strList.at(1).toDouble();
          }
          //--------------
          // ORGANIC
          if (strList.at(0).compare(this->transitionName(SIG_ORGANIC_MIN)) == 0)
          {
            transitionMap[SIG_ORGANIC_MIN] = strList.at(1).toDouble();
          }
          if (strList.at(0).compare(this->transitionName(SIG_ORGANIC_MAX)) == 0)
          {
            transitionMap[SIG_ORGANIC_MAX] = strList.at(1).toDouble();
          }
          //----------------
          // INORGANIC
          if (strList.at(0).compare(this->transitionName(SIG_INORGANIC_MIN)) == 0)
          {
            transitionMap[SIG_INORGANIC_MIN] = strList.at(1).toDouble();
          }
          if (strList.at(0).compare(this->transitionName(SIG_INORGANIC_MAX)) == 0)
          {
            transitionMap[SIG_INORGANIC_MAX] = strList.at(1).toDouble();
          }
          //----------------
          // METAL
          if (strList.at(0).compare(this->transitionName(SIG_METAL_MIN)) == 0)
          {
            transitionMap[SIG_METAL_MIN] = strList.at(1).toDouble();
          }
          if (strList.at(0).compare(this->transitionName(SIG_METAL_MAX)) == 0)
          {
            transitionMap[SIG_METAL_MAX] = strList.at(1).toDouble();
          }
          //---------------
          // THREATS

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_MIN))== 0)
          {
            transitionMap[SIG_THREAT_MIN] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_A)) == 0)
          {
            transitionMap[SIG_THREAT_SET_A] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_B)) == 0)
          {
            transitionMap[SIG_THREAT_SET_B] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_C)) == 0)
          {
            transitionMap[SIG_THREAT_SET_C] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_D)) == 0)
          {
            transitionMap[SIG_THREAT_SET_D] = strList.at(1).toDouble();
          }

          if (strList.at(0).compare(this->transitionName(SIG_THREAT_SET_E)) == 0)
          {
            transitionMap[SIG_THREAT_SET_E] = strList.at(1).toDouble();
          }


          if (strList.at(0).compare(this->transitionName(SIG_THREAT_MAX)) == 0)
          {
            transitionMap[SIG_THREAT_MAX] = strList.at(1).toDouble();
          }
          if (strList.at(0).compare(this->transitionName(SIG_DEFAULT_NOISE_CLIP)) == 0 )
          {
            transitionMap[SIG_DEFAULT_NOISE_CLIP] = strList.at(1).toDouble();
          }
        }
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }
  if (transitionMap.size() == 0 )
  {
    std::string errorMessage = QObject::tr("List size == 0, Possibly a corrupted file:").toStdString() +
        filePath;
    throw std::logic_error(errorMessage);
  }
  return true;
}







//------------------------------------------------------------------------------
QString VolumeRendererConfig::transitionName(MaterialTransition_E transitionE)
{
  QString retVal = "ORGANIC_SIGNAL_MIN";
  switch(transitionE)
  {
  case SIG_LOW_DENSITY_MIN:
    retVal = "LOW_DENSITY_SIGNAL_MIN";
    break;

  case SIG_ORGANIC_MIN:
    retVal = "ORGANIC_SIGNAL_MIN";
    break;

  case SIG_ORGANIC_MAX:
    retVal = "ORGANIC_SIGNAL_MAX";
    break;

  case SIG_INORGANIC_MIN:
    retVal = "INORGANIC_SIGNAL_MIN";
    break;

  case SIG_INORGANIC_MAX:
    retVal = "INORGANIC_SIGNAL_MAX";
    break;

  case SIG_METAL_MIN:
    retVal = "METAL_SIGNAL_MIN";
    break;

  case SIG_METAL_MAX:
    retVal = "METAL_SIGNAL_MAX";
    break;

  case SIG_THREAT_MIN:
    retVal = "THREAT_SIGNAL_MIN";
    break;

  case SIG_THREAT_SET_A:
    retVal = "THREAT_SIGNAL_SETVAL_A";
    break;

  case SIG_THREAT_SET_B:
    retVal = "THREAT_SIGNAL_SETVAL_B";
    break;

  case SIG_THREAT_SET_C:
    retVal = "THREAT_SIGNAL_SETVAL_C";
    break;

  case SIG_THREAT_SET_D:
    retVal = "THREAT_SIGNAL_SETVAL_D";
    break;

  case SIG_THREAT_SET_E:
    retVal = "THREAT_SIGNAL_SETVAL_E";
    break;

  case SIG_THREAT_SET_F:
    retVal = "THREAT_SIGNAL_SETVAL_F";
    break;

  case SIG_THREAT_MAX:
    retVal = "THREAT_SIGNAL_MAX";
    break;
  case SIG_DEFAULT_NOISE_CLIP:
    retVal = "DEFAULT_NOISE_CLIP";
    break;
  }
  return retVal;
}


//-----------------------------------------------------------------------------
bool VolumeRendererConfig::readInternalSettings()
{
  if ( strlen(m_internalSettingsFilePath.c_str()) > 0 &&
       boost::filesystem::exists(m_internalSettingsFilePath.c_str())
     )
  {
    LOG(INFO) << "Volume Renderer internal settings file: " << m_internalSettingsFilePath;
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(m_internalSettingsFilePath, pt);


    ///====================================
    /// Touch Screen Settings
    //------------------------
    // Pich Threshold
    try
    {
      m_pinchThreshold  = pt.get<int>("TouchHandler.PinchThreshold");
      LOG(INFO) << "Internal settings read - pinch threshold: " << m_pinchThreshold;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }
    //------------------------
    // Spread Threshold
    try
    {
      m_spreadThreshold  = pt.get<int>("TouchHandler.SpreadThreshold");
      LOG(INFO) << "Internal settings read - spread threshold: " << m_spreadThreshold;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }

    //------------------------
    // Move Threshold
    try
    {
      m_moveThreshold  = pt.get<int>("TouchHandler.MoveThreshold");
      LOG(INFO) << "Internal settings read - move threshold: " << m_moveThreshold;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }


    ///=====================================
    /// Rendering Settings
    //-------------------------
    // Using GPU
    try
    {
      m_useGPU  = pt.get<bool>("RendererSettings.UseGPU");
      LOG(INFO) << "Internal settings read - Using GPU: " << m_useGPU;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }
    //-----------------------
    // Show Mass in Threat
    try
    {
      m_showThreatMass  = pt.get<bool>("RendererSettings.ShowMassInThreat");
      LOG(INFO) << "Internal settings read - Show Mass in Threat: " << m_showThreatMass;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }



    ///====================================
    /// Volume Widget Settings
    //-------------------------
    // Zoom Display Lower Limit
    try
    {
      m_zoomDisplayLowerLimit  = pt.get<double>("VolumeWidget.ZoomDisplayLowerLimit");
      LOG(INFO) << "Internal settings read - Zoom display lower limit: "
                << m_zoomDisplayLowerLimit;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
     this->throwExeption(error);
    }
    //-------------------------
    // Zoom Display Upper Limit
    try
    {
      m_zoomDisplayUpperLimit  = pt.get<double>("VolumeWidget.ZoomDisplayUpperLimit");
      LOG(INFO) << "Internal settings read - Zoom display upper limit: "
                << m_zoomDisplayUpperLimit;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }

    //------------------------
    // Scale In threshold
    try
    {
      m_scaleInThreshold  = pt.get<double>("VolumeWidget.ScaleInThreshold");
      LOG(INFO) << "Internal settings read - Scale In threshold: "
                << m_scaleInThreshold;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
     this->throwExeption(error);
    }


    //------------------------
    // Scale Out threshold.
    try
    {
      m_scaleOutThreshold  = pt.get<double>("VolumeWidget.ScaleOutThreshold");
      LOG(INFO) << "Internal settings read - Scale out threshold: "
                << m_scaleOutThreshold;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
     this->throwExeption(error);
    }


    //------------------------
    // Zoom Wheel delta
    try
    {
      m_zoomWheelDelta  = pt.get<double>("VolumeWidget.ZoomWheelDelta");
      LOG(INFO) << "Internal settings read - Zoom wheel delta: "
                << m_zoomWheelDelta;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
     this->throwExeption(error);
    }

    //------------------------------
    // Opacity Modify Scalar
    try
    {
      m_opacityModifyScalar = pt.get<double>("VolumeWidget.OpacityModifyScalar");
      LOG(INFO) << "Internal settings read - Opacity Modify Scalar: "
                << m_opacityModifyScalar;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }

    //-----------------------------
    // Contrast Modify Scalar
    try
    {
      m_contrastModifyScalar = pt.get<double>("VolumeWidget.ContrastModifyScalar");
      LOG(INFO) << "Internal settings read - Contrast Modify Scalar: "
                << m_contrastModifyScalar;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }

    //-----------------------------
    // Brightness Modify Scalar
    try
    {
      m_brightnessModifyScalar = pt.get<double>("VolumeWidget.BrightnessModifyScalar");
      LOG(INFO) << "Internal settings read - Brightness Modify Scalar: "
                << m_brightnessModifyScalar;
    }
    catch (const std::exception& error)
    {
      LOG(INFO) << error.what();
      this->throwExeption(error);
    }
    /// ===================================
  }
  else
  {
    std::string errorMessage = QObject::tr("Internal settigs file not found:").toStdString() +
        m_internalSettingsFilePath;
    LOG(INFO) << errorMessage;
    throw std::runtime_error(errorMessage);
  }
  return true;
}


//------------------------------------------------------------------------------
}  // namespace workstation

}  // namespace analogic
//------------------------------------------------------------------------------




