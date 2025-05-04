//------------------------------------------------------------------------------
// File: VRUtils.h
// Description: Headers file for utilities used by Volume Renderer classes.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_VRUTILS_H_
#define ANALOGIC_WS_VOLUME_RENDERER_VRUTILS_H_
//------------------------------------------------------------------------------

#include <QFile>
#include <QVector3D>
#include <QStringList>
#include <QFileInfo>
#include <map>
#include <string>
#include "ThreatDefs.h"
#include "ScannerDefs.h"
#include "ThreatList.h"
#include "ThreatVolume.h"

namespace analogic
{

namespace workstation
{


class ImageStatistics
{
public:
  double  m_minVal;
  double  m_maxVal;
  double  m_average;
  double  m_standardDeviation;
  int16_t m_clipValue;
  int     m_W;
  int     m_H;
  int     m_L;
  int     m_numberOfPoints;
  int     m_numberAboveClipValue;
};


enum MaterialEnumeration
{
  ORGANIC   = 0,
  INORGANIC = 1,
  METAL     = 2,
  THREAT    = 3,
};

enum UserDecisionEnum
{
   UD_SUSPECT     = 0,
   UD_CLEAR       = 1,
   UD_NO_DECISION = 2,
};

typedef std::map<UserDecisionEnum, QString> UserDecisionNameMap;



class CropCorners
{
public:
  CropCorners(int dimX,
              int dimY,
              int dimZ,
              double px,
              double py,
              double pz)
  {
    m_pMin = QVector3D(0, 0, 0);
    double dx = dimX;
    double dy = dimY;
    double dz = dimZ;
    m_pMax = QVector3D(dx*px,
                       dy*py,
                       dz*pz);
  }
  CropCorners()
  {
    // Default is just a cube of size 1
    m_pMin = QVector3D(0, 0, 0);
    m_pMax = QVector3D(1.0, 1.0, 1.0);
  }
  void setBounds(QVector3D& pMin,
                 QVector3D& pMax)
  {
    m_pMax = pMax;
    m_pMin = pMin;
  }
public:
  QVector3D m_pMin;
  QVector3D m_pMax;
};

//------------------------------------------------------------------------------
// Miscellaneous functions used by classes.

/**
 * @brief getHomeFolder
 * @param homeFolder (return value)
 * @return true
 */
bool getHomeFolder(std::string& homeFolder);


/**
 * @brief getDevRootFolder
 * @param devRootFolder (return value)
 * @return true if ENVIRONMENT VARIABLE "DEV_ROOT" is defined
 *         false otherwise.
 */
bool getDevRootFolder(std::string& devRootFolder);

/**
 * @brief getResourcesFolder
 * @param resFolder
 * Get the folder use to contain resources used by the Worstation Application.
 */
void getResourcesFolder(std::string& resFolder);

/**
 * @brief readCropCorners - read the corners for crioping from a file
 * @param cropFile
 * @param cc
 */
bool readCropCorners(QString& cropFile,
                     CropCorners& cc);

/**
 * @brief setCornerToPreventVTKOptimization
 * @param buffer
 * @param imageWidth
 * @param imageHeight
 * @param clipBounds
 */
void setCornerToPreventVTKOptimization(uint16_t* buffer,
                                        int imageWidth,
                                        int imageHeight,
                                        QVector3D& PSet,
                                        uint16_t setVal);

/**
 * @brief readTDRModule
 * @param tdrFile
 * @param sdicosErrorLogFile
 * @param errors
 * @param retTdr
 * @return
 */
bool readTDRModule(const std::string& tdrFile,
                   const std::string& sdicosErrorLogFile,
                   std::string& errors,
                   SDICOS::TDRModule** retTdr);

/**
 * @brief writeTDRModule
 * @param tdrFile
 * @param sdicosErrorLogFle
 * @param errors
 * @param tdrModule
 * @return
 */
bool writeTDRModule(const std::string& tdrFile,
                    const std::string& sdicosErrorLogFle,
                    std::string& errors,
                    SDICOS::TDRModule* tdrModule);

/**
 * @brief getThreatAlarmTypeFromTdrThreatObject
 * @param threatItem
 * @return ThreatAlarmType
 */
ThreatAlarmType getThreatAlarmTypeFromTdrThreatObject(SDICOS::TDRTypes::ThreatSequence::ThreatItem& threatItem);


/**
 * @brief writeTDR
 * @param pTdr
 * @param filename
 * @param logFile
 * @return
 */
bool writeTDR(SDICOS::TDRModule* pTdr,
              std::string& filename,
              std::string& logFile);

/**
 * @brief threatTypeName
 * @param aType
 * @return Name of alarm type
 */
QString threatTypeName(ThreatAlarmType aType);


/**
 * @brief threatTypeLab
 * @return "Type" label
 */
QString threatTypeLab();



/**
 * @brief constructOperatorThreatText
 * @param aTtype
 * @return
 */
QString constructOperatorThreatTextFromType(ThreatAlarmType aType);


/**
 * @brief constructRawOperatorThreatText
 * @param typeText
 * @return
 */
QString constructRawOperatorThreatText(QString typeText);

/**
 * @brief allThreatTypeNames
 * @return
 */
QStringList allThreatTypeNames();

/**
 * @brief threatAlarmTypeFromName
 * @param aName
 * @return  ThreatAlarmType
 */
ThreatAlarmType threatAlarmTypeFromName(QString aName);


/**
 * @brief saveTransferFunctions
 * @param hsvFile
 * @param hsvMap
 * @param opacityFile
 * @param opacityMap
 */
void saveTransferFunctions(QString& hsvFile,
                           std::list<HSVColorSigVal>& hsvMap,
                           QString& opacityFile,
                           std::list<OpacitySigVal> opacityMap);

/**
 * @brief saveFullMaps
 * @param hsvFile
 * @param hsvMap
 * @param opacityFile
 * @param opacityMap
 */
void saveFullMaps(QString& fullMapFile,
                  std::list<HSVColorSigVal>& hsvMap,
                  std::list<OpacitySigVal>& opacityMap,
                  uint16_t maxSig);

/**
 * @brief parseLine
 * @param char* line
 */
int parseLine(char* line);

/**
 * @brief getValue
 */
int getValue();

/**
 * @brief PrintSystemMemoryInfo
 */
void PrintSystemMemoryInfo();

/**
 * @brief WriteCounter
 * @param char* string
 * @param int counter
 */
void WriteCounter(char* string, int counter);

/**
 * @brief WriteLogGPL
 * @param char* string
 */
void WriteLogGPL(const char* string);

/**
 * @brief WriteLogGPL
 * @param QString& str
 */
void WriteLogGPL(QString& str);


/**
 * @brief getThreatBoxColorMap
 * @return The map of color by threat type
 */
ThreatBoxTypeColorMap getThreatBoxColorMap();


/**
 * @brief getOperatorthreatBoxColor
 * @return The color for the operator threat Box
 */
QColor getOperatorthreatBoxColor();


/**
 * @brief clipVolumeRange
 * @param pMin
 * @param pMax
 * @param p0
 * @param p1
 */
void clipVolumeRange(QVector3D& pMin, QVector3D& pMax,
                     QVector3D& p0, QVector3D& p1);

/**
 * @brief clipThreatRange
 * @param Rmin
 * @param Rmax
 * @param Tmin
 * @param Tmax
 */
void clipThreatRange(double& Rmin, double& Rmax,
             double& Tmin, double& Tmax);

/**
 * @brief convertMapToString
 * @param ThreatAlarmType aType
 * @return QString
 */
QString convertMapToString(ThreatAlarmType aType);
//------------------------------------------------------------------------------
}  // namespace workstation

}   // namespace analogic
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_VRUTILS_H_
//------------------------------------------------------------------------------
