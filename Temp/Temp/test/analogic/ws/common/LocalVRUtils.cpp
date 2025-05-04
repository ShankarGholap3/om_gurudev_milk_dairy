//-------------------------------------------------------------------------------
// File: LocalVRUtils.cpp
// Description: Implementation of local Volume Rendering Utilities.
//    This is a place holder for regularly used functions. Especially
//    ones that want to be built seperately from main line definitions
//    that Workstation needs.
// Written by: Andre R. Oughton
// Copyright 2018 Analogic corp.
//-------------------------------------------------------------------------------

#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <pwd.h>
#include <iostream>
#include "LocalVRUtils.h"

namespace analogic
{

namespace test_vr
{


//------------------------------------------------------------------------------
bool getResourcesDirectory(std::string& resFolder)
{
//  std::string strHome;
//  bool bGetHome = getHomeDirectory(strHome);
//  if (!bGetHome) return false;

//  std::string strDevRoot;
//  getDevRootDirectory(strDevRoot);
//  std::string retStr = strHome + "/";
//  retStr += strDevRoot + "/workstation/resources/";
//  resFolder = retStr;
  resFolder = "./TransferFunctions/";
  return true;
}

//------------------------------------------------------------------------------
bool getDevRootDirectory(std::string& devRootFolder)
{
  // Linux
  char *devRoot = getenv("DEV_ROOT");
  devRootFolder = devRoot;
  return true;
}


//------------------------------------------------------------------------------
bool getHomeDirectory(std::string& homeFolder)
{
  // Linux
  uid_t uid = getuid();
  struct passwd  pwent;
  struct passwd* pwentp;
  char   buf[1024];
  getpwuid_r(uid,
             &pwent,
             buf,
             sizeof(buf),
             &pwentp);
  const char *homedir = pwentp->pw_dir;
  homeFolder = homedir;
  return true;
}


//------------------------------------------------------------------------------
void readVolumeProperties(QString& propertiesFile,
                          ImageProperties* iP,
                          QVector3D& roiStart,
                          QVector3D& roiEnd)
{
  std::string stdFileName = propertiesFile.toLocal8Bit().data();
  std::ifstream inFile(stdFileName.c_str());
  if ( !inFile )
  {
    std::string msg;
    msg = "Could not open Porperties file '" + stdFileName + "'";
    throw std::runtime_error(msg.c_str());
  }
  std::string str;
  try
  {
    while (std::getline(inFile, str))
    {
      QString sLine = str.c_str();
      QStringList lineList = sLine.split(PROPERTIES_DELIMITER);
      QString sLabel = lineList.at(0);
      QString sValue = lineList.at(1);

      if (sLabel.compare("volume_width") == 0 )
      {
        iP->m_imageWidth = sValue.toInt();
      }
      if (sLabel.compare("volume_height") == 0 )
      {
        iP->m_imageHeight = sValue.toInt();
      }

      if (sLabel.compare("slice_count") == 0 )
      {
        iP->m_imageMaxLength = sValue.toInt();
      }

      //---------------------
      // ROI Start
      if (sLabel.compare("volume_roi_start_x") == 0 )
      {
        roiStart.setX(sValue.toFloat());
      }

      if (sLabel.compare("volume_roi_start_y") == 0 )
      {
        roiStart.setY(sValue.toFloat());
      }

      if (sLabel.compare("volume_roi_start_z") == 0 )
      {
        roiStart.setZ(sValue.toFloat());
      }

      //---------------------
      // ROI End
      if (sLabel.compare("volume_roi_end_x") == 0 )
      {
        roiEnd.setX(sValue.toFloat());
      }

      if (sLabel.compare("volume_roi_end_y") == 0 )
      {
        roiEnd.setY(sValue.toFloat());
      }

      if (sLabel.compare("volume_roi_end_z") == 0 )
      {
        roiEnd.setZ(sValue.toFloat());
      }
    }
    inFile.close();
  }
  catch (const std::exception& error)
  {
    throw error;
  }
}


//------------------------------------------------------------------------------
bool readOpacityFile(std::string& filePath,
                     std::list<OpacitySigVal>& opacityList)
{
  std::cout <<  "**** Begin read opacity file:" << filePath << std::endl;
  std::string inFilename = filePath;
  std::ifstream inFile(inFilename.c_str());
  if ( !inFile )
  {
    std::string msg;
    msg = "Could not open opacity file '" + filePath + "'";
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
          std::cout << message << std::endl;
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
    std::string errorMessage = "List size == 0, Possibly a corrupted file:" +
        filePath;
    throw std::logic_error(errorMessage);
  }
  return true;
}



//------------------------------------------------------------------------------
bool readHSVColorFile(std::string& filePath,
                      std::list<HSVColorSigVal>& colorList)
{
  std::cout  << "**** Begin read Color TF file:" << filePath  << std::endl;
  std::string inFilename = filePath;
  std::ifstream inFile(inFilename.c_str());
  if ( !inFile )
  {
    std::string msg;
    msg = "Could not open HSV color file '" + filePath + "'";
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
          std::cout << message << std::endl;
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
    std::string errorMessage = "List size == 0, Possibly a corrupted file:" +
        filePath;
    throw std::logic_error(errorMessage);
  }
  return true;
}



}  // namespace test_vr
}  // namespace analogic

