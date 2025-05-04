//-------------------------------------------------------------------------------
// File: LocalVRUtils.h
// Description: Header file for local Volume Rendering Utilities.
//    This is a place holder for regularly used functions. Especially
//    ones that want to be built seperately from main line definitions
//    that Workstation needs.
// Written by: Andre R. Oughton
// Copyright 2018 Analogic corp.
//-------------------------------------------------------------------------------

#ifndef TEST_ANALOGIC_WS_COMMON_LOCALVRUTILS_H_
#define TEST_ANALOGIC_WS_COMMON_LOCALVRUTILS_H_


#include <QString>
#include <QVector3D>
#include "LocalScannerDefs.h"

namespace analogic
{

namespace test_vr
{

#define GENERIC_VOLFILE_NAME_FILTER  "Volume File (*.cp_vol *.vol);;"
#define GENERIC_VOLFILE_EXT_A         ".vol"
#define GENERIC_VOLFILE_EXT_B         ".cp_vol"


#define DICOS_CT_FILE_EXTENSION          ".dicos_ct_vol"
#define PROPERTIES_FILE_EXTENSION        ".properties"
#define PROPERTIES_DELIMITER             "="


#define MIN_FILE_READLN_LEN   3
#define VOL_COLOR_DELIM      ","
#define VOL_COLOR_COMMENT    "#"

enum SCANNER_TYPE
{
  SCANNER_TYPE_CHECKPOINT = 0,
  SCANNER_TYPE_COBRA      = 1,
};





class ImageProperties
{
public:
  explicit ImageProperties(SCANNER_TYPE scType)
  {
    if (scType == SCANNER_TYPE_CHECKPOINT )
    {
      m_imageWidth     = CHECKPOINT_MAX_X_DIM;
      m_imageHeight    = CHECKPOINT_MAX_Y_DIM;
      m_imageMaxLength = CHECKPOINT_MAX_Z_DIM;
      m_pixelSizeX     = CHECKPOINT_PIXEL_SPACING_X;
      m_pixelSizeY     = CHECKPOINT_PIXEL_SPACING_Y;
      m_pixelSizeZ     = CHECKPOINT_PIXEL_SPACING_Z;
    }
    if (scType == SCANNER_TYPE_COBRA )
    {
      m_imageWidth     = COBRA_MAX_X_DIM;
      m_imageHeight    = COBRA_MAX_Y_DIM;
      m_imageMaxLength = COBRA_MAX_Z_DIM;
      m_pixelSizeX     = COBRA_PIXEL_SPACING_X;
      m_pixelSizeY     = COBRA_PIXEL_SPACING_Y;
      m_pixelSizeZ     = COBRA_PIXEL_SPACING_Z;
    }
  }

public:
  int m_imageWidth;
  int m_imageHeight;
  int m_imageMaxLength;

  double m_pixelSizeX;
  double m_pixelSizeY;
  double m_pixelSizeZ;
};


bool getHomeDirectory(std::string& homeFolder);

bool getResourcesDirectory(std::string& resFolder);

bool getDevRootDirectory(std::string& devRootFolder);

void readVolumeProperties(QString& propertiesFile,
                          ImageProperties* iP,
                          QVector3D& roiStart,
                          QVector3D& roiEnd);

bool readOpacityFile(std::string& filePath,
                     std::list<OpacitySigVal>& opacityList);

bool readHSVColorFile(std::string& filePath,
                      std::list<HSVColorSigVal>& colorList);



}  //  namespace test_vr

}  //  namespace analogic

#endif  // TEST_ANALOGIC_WS_COMMON_LOCALVRUTILS_H_
