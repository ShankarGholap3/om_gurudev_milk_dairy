//------------------------------------------------------------------------------
// File: VRUtils.cpp
// Description: Implementation for utilities used by Volume Renderer classes.
// Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------

#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <pwd.h>
#include <SDICOS/SdcsTemplateCT.h>
#include "VRUtils.h"
#include <QObject>


namespace analogic
{

namespace workstation
{

ThreatTypeMap g_threatNameMap =
{
  {EXPLOSIVE ,         "Explosive"  },
  {LIQUID,             "Liquid"     },
  {WEAPON,             "Weapon"     },
  {CONTRABAND,         "Contraband" },
  {ANOMALY,            "Anomaly"    },
  {SHIELD,             "Shield"     },
  {LIQUID_AEROSOL_GAS, "LAG"        },
  {LAPTOP,             "Laptop"     },
  {SHARP,              "Sharp"      },
  {GUN,                "Gun"        },
  {BLUNT,              "Blunt"      },
  {UNDEFINED,          "UNDEFINED"  },
};

ThreatBoxTypeColorMap g_threatColorMap =
{
  {EXPLOSIVE ,          QColor(EXPLOSIVE_THREAT_COLOR_RED,
                               EXPLOSIVE_THREAT_COLOR_GREEN,
                               EXPLOSIVE_THREAT_COLOR_BLUE) },
  {LIQUID,              QColor(LIQUID_THREAT_COLOR_RED,
                               LIQUID_THREAT_COLOR_GREEN,
                               LIQUID_THREAT_COLOR_BLUE)},
  {WEAPON,              QColor(WEAPON_THREAT_COLOR_RED,
                               WEAPON_THREAT_COLOR_GREEN,
                               WEAPON_THREAT_COLOR_BLUE)},
  {CONTRABAND,          QColor(CONTRABAND_THREAT_COLOR_RED,
                               CONTRABAND_THREAT_COLOR_GREEN,
                               CONTRABAND_THREAT_COLOR_BLUE) },
  {ANOMALY,             QColor(ANOMALY_THREAT_COLOR_RED,
                               ANOMALY_THREAT_COLOR_GREEN,
                               ANOMALY_THREAT_COLOR_BLUE) },
  {SHIELD,              QColor(SHIELD_THREAT_COLOR_RED,
                               SHIELD_THREAT_COLOR_GREEN,
                               SHIELD_THREAT_COLOR_BLUE) },
  {LIQUID_AEROSOL_GAS,  QColor(LAG_THREAT_COLOR_RED,
                               LAG_THREAT_COLOR_GREEN,
                               LAG_THREAT_COLOR_BLUE) },
  {LAPTOP,              QColor(LAPTOP_THREAT_COLOR_RED,
                               LAPTOP_THREAT_COLOR_GREEN,
                               LAPTOP_THREAT_COLOR_BLUE) },
  {SHARP,               QColor(SHARP_THREAT_COLOR_RED,
                               SHARP_THREAT_COLOR_GREEN,
                               SHARP_THREAT_COLOR_BLUE) },
  {GUN,                 QColor(GUN_THREAT_COLOR_RED,
                               GUN_THREAT_COLOR_GREEN,
                               GUN_THREAT_COLOR_BLUE) },
  {BLUNT,               QColor(BLUNT_THREAT_COLOR_RED,
                               BLUNT_THREAT_COLOR_GREEN,
                               BLUNT_THREAT_COLOR_BLUE) },
  {UNDEFINED,           QColor(UNDEFINED_THREAT_COLOR_RED,
                               UNDEFINED_THREAT_COLOR_GREEN,
                               UNDEFINED_THREAT_COLOR_BLUE) },
};


QColor g_OperatorThreatColor(OPERATOR_THREAT_COLOR_RED,
                             OPERATOR_THREAT_COLOR_GREEN,
                             OPERATOR_THREAT_COLOR_BLUE);


const double g_fakedensity     = 0.000001;


//------------------------------------------------------------------------------
void setCornerToPreventVTKOptimization(uint16_t* buffer,
                                       int imageWidth,
                                       int imageHeight,
                                       QVector3D& PSet,
                                       uint16_t setVal)
{
  int pX = PSet.x();
  int pY = PSet.y();
  int pZ = PSet.z();
  int imageLoc = (imageWidth*imageHeight)*pZ + (imageWidth*pY) +  pX;
  buffer[imageLoc] = setVal;
}

//------------------------------------------------------------------------------
bool readTDRModule(const std::string& tdrFile,
                   const std::string& sdicosErrorLogFile,
                   std::string& errors,
                   SDICOS::TDRModule** retTdr)
{
  SDICOS::ErrorLog errorlog;
  SDICOS::Filename tdrFilename(tdrFile);

  SDICOS::TDRModule* newTdr;
  newTdr = new SDICOS::TDRModule();

  bool bResp = newTdr->Read(tdrFilename, errorlog);
  if (!bResp)
  {
    QString message = QObject::tr("Error reading TDR file:");
    message += tdrFile.c_str();
    errors = message.toLocal8Bit().data();
    *retTdr = nullptr;
    return false;
  }


  if(errorlog.HasErrors() || errorlog.HasWarnings())
  {
    errorlog.WriteLog(SDICOS::Filename(sdicosErrorLogFile));
    std::cout << errorlog << std::endl;
    QString erroMessage = QObject::tr("Read Error on TDR File - ");
    erroMessage +=  errorlog.GetErrorLog().Get();
    errors = erroMessage.toLocal8Bit().data();
    *retTdr = nullptr;
    return false;
  }



  SDICOS::TDRTypes::ThreatDetectionReport& pReport = newTdr->GetThreatDetectionReport();
  int nAlarms = pReport.GetNumberOfAlarmObjects();

  QString resultMessage = "Number Alarms: " + QString::number(nAlarms) + "\n";


  int nTotalObjects =  pReport.GetNumberOfTotalObjects();
  resultMessage += "Number of Alarm Objects: ";
  resultMessage += QString::number(nTotalObjects);



  SDICOS::TDRTypes::ThreatSequence &ts = newTdr->GetThreatSequence();

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects =
            ts.GetPotentialThreatObject();



  int nTI = threatObjects.GetSize();
  resultMessage += "ThreatObject size: ";
  resultMessage += QString::number(nTI);
  *retTdr = newTdr;
  return true;
}


//------------------------------------------------------------------------------
bool writeTDRModule(const std::string& tdrFile,
                    const std::string& sdicosErrorLogFle,
                    std::string& errors,
                    SDICOS::TDRModule* tdrModule)
{
  SDICOS::ErrorLog errorlog;
  SDICOS::Filename tdrFilename(tdrFile);

  SDICOS::DicosFile::TRANSFER_SYNTAX nSyntax = SDICOS::DicosFile::enumLittleEndianExplicit;

  bool bResp = tdrModule->Write(tdrFilename,
                                nSyntax,
                                errorlog);

  if (!bResp)
  {
    errorlog.WriteLog(SDICOS::Filename(sdicosErrorLogFle));
    std::cout << errorlog <<std::endl;
    QString message = QObject::tr("Error writing TDR file:");
    message += tdrFile.c_str();
    errors = message.toLocal8Bit().data();
    return false;
  }


  if(errorlog.HasErrors() || errorlog.HasWarnings())
  {
    errorlog.WriteLog(SDICOS::Filename(sdicosErrorLogFle));
    std::cout << errorlog << std::endl;
    QString erroMessage = QObject::tr("Write Error on TDR File - ");
    erroMessage +=  errorlog.GetErrorLog().Get();
    errors = erroMessage.toLocal8Bit().data();
    return false;
  }
  return true;
}



//------------------------------------------------------------------------------
void clipVolumeRange(QVector3D& pMin, QVector3D& pMax,
                     QVector3D& p0, QVector3D& p1)
{
  double Xmin = pMin.x();
  double Xmax = pMax.x();

  double Ymin = pMin.y();
  double Ymax = pMax.y();

  double Zmin = pMin.z();
  double Zmax = pMax.z();
  //------------------------------------------------------------
  // DEBUG CLIP ERROR:

  // std::cout << "==================================================" << std::endl;
  // std::cout << "VOLUME RANGE:" << std::endl;
  // std::cout << "VOL-MIN-MAX X:" << Xmin
  //           << ", "             << Xmax << std::endl;

  // std::cout << "VOL-MIN-MAX Y:" << Ymin
  //           << ", "             << Ymax << std::endl;

  // std::cout << "VOL-MIN-MAX Z:" << Zmin
  //           << ", "             << Zmax << std::endl;
  // std::cout << "==================================================" << std::endl;

  double TXmin = p0.x();
  double TXmax = p1.x();

  double TYmin = p0.y();
  double TYmax = p1.y();

  double TZmin = p0.z();
  double TZmax = p1.z();

  // DEBUG CLIP ERROR:
  // std::cout << "==================================================" << std::endl;
  // std::cout << "Before CLIP" << std::endl;
  // std::cout << "THR-MIN-MAX X:" << TXmin
  //           << ", "             << TXmax << std::endl;

  // std::cout << "THR-MIN-MAX Y:" << TYmin
  //           << ", "             << TYmax << std::endl;

  // std::cout << "THR-MIN-MAX Z:" << TZmin
  //           << ", "             << TZmax << std::endl;

  // std::cout << "==================================================" << std::endl;

  clipThreatRange(Xmin, Xmax, TXmin, TXmax);
  clipThreatRange(Ymin, Ymax, TYmin, TYmax);
  clipThreatRange(Zmin, Zmax, TZmin, TZmax);

  // std::cout << "==================================================" << std::endl;
  // std::cout << "AFTER CLIP" << std::endl;
  // std::cout << "THR-MIN-MAX X:" << TXmin
  //           << ", "             << TXmax << std::endl;

  // std::cout << "THR-MIN-MAX Y:" << TYmin
  //           << ", "             << TYmax << std::endl;

  // std::cout << "THR-MIN-MAX Z:" << TZmin
  //           << ", "             << TZmax << std::endl;

  // std::cout << "==================================================" << std::endl;

  p0 = QVector3D(TXmin, TYmin, TZmin);
  p1 = QVector3D(TXmax, TYmax, TZmax);
}

//------------------------------------------------------------------------------
void clipThreatRange(double& Rmin, double& Rmax,
             double& Tmin, double& Tmax)
{
  if (Tmin < Rmin) Tmin = Rmin;
  if (Tmin > Rmax) Tmin = Rmax;

  if (Tmax > Rmax) Tmax = Rmax;
  if (Tmax < Rmin) Tmax = Rmin;
}


//------------------------------------------------------------------------------
bool writeTDR(SDICOS::TDRModule* pTdr,
              std::string& filename,
              std::string& logFile)
{
  SDICOS::ErrorLog errorLog;
  QFileInfo fInfo(filename.c_str());
  std::string path = fInfo.absolutePath().toLocal8Bit().data();
  path += "/";
  std::string name = fInfo.fileName().toLocal8Bit().data();

  SDICOS::Folder tdrFolder(path);
  SDICOS::Filename tdrFilename(tdrFolder, name);
  SDICOS::DicosFile::TRANSFER_SYNTAX nSyntax = SDICOS::DicosFile::enumLittleEndianExplicit;
  bool writeStat = pTdr->Write(tdrFilename, nSyntax, errorLog);
  if (writeStat)
  {
    qDebug() << "Successful DTRModule write to file:" << filename.c_str();
  }
  else
  {
    qDebug() << "Fail DTRModule write to file:" << filename.c_str();
    errorLog.WriteLog(SDICOS::Filename(logFile));
  }

  return writeStat;
}

//------------------------------------------------------------------------------
analogic::workstation::ThreatAlarmType
    getThreatAlarmTypeFromTdrThreatObject(SDICOS::TDRTypes::ThreatSequence::ThreatItem&
                                                threatItem)
{
  analogic::workstation::ThreatAlarmType retVal = UNDEFINED;

  SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem>&  assesmentArray
      = threatItem.GetAssessment().GetAssessment();

  if (assesmentArray.GetSize() ==0 )
  {
    return retVal;
  }


  //--------------------------------------
  // TEST ABILITY-ASSESSMENT for SHIELD
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ABILITY_ASSESSMENT
       abilityAssement = assesmentArray[0].GetAbilityAssessment();
  if (abilityAssement ==
      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumShield )
  {
    retVal  = SHIELD;
    return retVal;
  }


  //-------------------------------------
  // OK now look at Threat Type
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::THREAT_CATEGORY
      threatCat = assesmentArray[0].GetThreatCategory();

  std::string threatCatDesc = assesmentArray[0].GetThreatCategoryDescription().Get();

  if (threatCatDesc == "LAPTOP")
  {
    retVal = analogic::workstation::LAPTOP;
  }
  else if (threatCat == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumExplosive)
  {
    retVal = analogic::workstation::EXPLOSIVE;
  }
  // bdavidson: modified for DICOS v2.0a
  else if (threatCat == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumProhibitedItem)
  {
    if (threatCatDesc == "LIQUID")
    {
      retVal = analogic::workstation::LIQUID;
    }
    else if (threatCatDesc == "WEAPON")
    {
      retVal = analogic::workstation::WEAPON;
    }
    else if (threatCatDesc == "SHARP")
    {
      retVal = analogic::workstation::SHARP;
    }
    else if (threatCatDesc == "GUN")
    {
      retVal = analogic::workstation::GUN;
    }
    else if (threatCatDesc == "BLUNT")
    {
      retVal = analogic::workstation::BLUNT;
    }
    else
    {
      retVal = analogic::workstation::UNDEFINED;
    }
  }
  else if (threatCat == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumContraband)
  {
    retVal = analogic::workstation::CONTRABAND;
  }
  else if (threatCat == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumAnomaly)
  {
    retVal = analogic::workstation::ANOMALY;
  }
  return retVal;
}

//------------------------------------------------------------------------------
QStringList allThreatTypeNames()
{
  ThreatTypeMap::iterator it;
  QStringList retList;
  for (it = g_threatNameMap.begin(); it != g_threatNameMap.end(); it++ )
  {
      QString nameI =  it->second;
      retList.push_back(nameI);
  };
  return retList;
}


//------------------------------------------------------------------------------
ThreatBoxTypeColorMap getThreatBoxColorMap()
{
   return g_threatColorMap;
}

//------------------------------------------------------------------------------
QColor getOperatorthreatBoxColor()
{
  return g_OperatorThreatColor;
}

//------------------------------------------------------------------------------
QString threatTypeLab()
{
  return QObject::tr("Type: ");
}

//------------------------------------------------------------------------------
QString threatTypeName(ThreatAlarmType aType)
{
  QString retVal =  g_threatNameMap[aType];
  return retVal;
}


//------------------------------------------------------------------------------
QString constructOperatorThreatTextFromType(ThreatAlarmType aType)
{
  QString typeText    = convertMapToString(aType);
  QString threatText  = QObject::tr("Type: ");
  threatText         += typeText;
  return threatText;
}

//------------------------------------------------------------------------------
QString constructRawOperatorThreatText(QString typeText)
{
  QString threatText  = QObject::tr("Type: ");
  threatText         += typeText;
  return threatText;
}


//------------------------------------------------------------------------------
ThreatAlarmType threatAlarmTypeFromName(QString aName)
{
  ThreatTypeMap::iterator it;
  ThreatAlarmType retVal = ThreatAlarmType::UNDEFINED;
  for (it = g_threatNameMap.begin(); it != g_threatNameMap.end(); it++ )
  {
      QString nameI =  it->second;
      if (nameI.compare(aName) == 0)
      {
        retVal = it->first;
        break;
      }
  };
  return retVal;
}



//------------------------------------------------------------------------------
void saveTransferFunctions(QString& hsvFile,
                           std::list<HSVColorSigVal>& hsvMap,
                           QString& opacityFile,
                           std::list<OpacitySigVal> opacityMap)
{
  // HSV Map
  std::list<HSVColorSigVal>::iterator itc;
  std::ofstream file_hsv;
  file_hsv.open (hsvFile.toLocal8Bit().data());
  file_hsv << "HSV Color map For Volume renderer [Signal, H, S, V]\n";

  for (itc = hsvMap.begin(); itc != hsvMap.end(); itc++)
  {
    HSVColorSigVal sv = *itc;
    QString fileOutStr = QString::number(sv.m_signalVal) + ", " +
                         QString::number(sv.m_h)         + ", " +
                         QString::number(sv.m_s)         + ", " +
                         QString::number(sv.m_v) + "\n";
    file_hsv << fileOutStr.toLocal8Bit().data();
  }
  file_hsv.close();

  // Opacity Map
  std::list<OpacitySigVal>::iterator ito;
  std::ofstream file_op;
  file_op.open (opacityFile.toLocal8Bit().data());
  file_op << "Opacity Map For Volume renderer [Signal, Opacity]\n";

  for (ito = opacityMap.begin(); ito != opacityMap.end(); ito++)
  {
    OpacitySigVal sv = *ito;
    QString fileOutStr = QString::number(sv.m_signalVal) + ", " +
                         QString::number(sv.m_opacity) + "\n";
    file_op << fileOutStr.toLocal8Bit().data();
  }
  file_op.close();
}


//------------------------------------------------------------------------------
HSVColorSigVal closestHsvMatch(std::list<HSVColorSigVal>& hsvMap,
                            uint16_t sigTest)
{
  std::list<HSVColorSigVal>::iterator itHsv;
  std::vector<HSVColorSigVal> hsvVector;
  for ( itHsv = hsvMap.begin(); itHsv != hsvMap.end(); itHsv++)
  {
    HSVColorSigVal hsvI = *itHsv;
    hsvVector.push_back(hsvI);
  }



  HSVColorSigVal retVal = hsvVector.at(0);
  int sizeVec = hsvVector.size();
  bool foundMatch = false;

  for ( int i = 0; i < (sizeVec - 1); i++)
  {
     HSVColorSigVal vCur  = hsvVector.at(i);
     HSVColorSigVal vNext = hsvVector.at( i + 1);

     if ((sigTest <= vCur.m_signalVal) &&
         (sigTest <  vNext.m_signalVal)
         )
      {
         foundMatch = true;
         retVal.m_signalVal = sigTest;
         retVal.m_h = vCur.m_h;
         retVal.m_s = vCur.m_s;
         retVal.m_v = vCur.m_v;
         break;
      }
  }
  if (!foundMatch)
  {
    retVal.m_signalVal = 0;
    retVal.m_h = 0;
    retVal.m_s = 0;
    retVal.m_v = 0;
  }
  return retVal;
}


//------------------------------------------------------------------------------
OpacitySigVal closestOpacityMatch(std::list<OpacitySigVal>& opacityMap,
                            uint16_t sigTest)
{
  std::list<OpacitySigVal>::iterator itOpacity;
  std::vector<OpacitySigVal> opVector;
  for ( itOpacity = opacityMap.begin(); itOpacity != opacityMap.end(); itOpacity++)
  {
    OpacitySigVal opI = *itOpacity;
    opVector.push_back(opI);
  }



  OpacitySigVal retVal = opVector.at(0);
  int sizeVec = opVector.size();
  bool foundMatch = false;

  for ( int i = 0; i < (sizeVec - 1); i++)
  {
     OpacitySigVal vCur  = opVector.at(i);
     OpacitySigVal vNext = opVector.at( i + 1);

     if ((sigTest <= vCur.m_signalVal) &&
         (sigTest <  vNext.m_signalVal)
         )
      {
         foundMatch = true;
         retVal.m_signalVal = sigTest;
         retVal.m_opacity   = vCur.m_opacity;
         break;
      }
  }
  if (!foundMatch)
  {
    retVal.m_signalVal = 0;
    retVal.m_opacity   = 0;
  }
  return retVal;
}

//------------------------------------------------------------------------------
void saveFullMaps(QString& fullMapFile,
                  std::list<HSVColorSigVal>& hsvMap,
                  std::list<OpacitySigVal> &opacityMap,
                  uint16_t maxSig)
{
  std::ofstream file_fullMap;
  file_fullMap.open (fullMapFile.toLocal8Bit().data());
  file_fullMap << "Full HSV/Opacity Map for Scanner [Signal, H, S, V ,Opacity]\n";


  double sigVal   = 0.0;
  double deltaSig = 10.0;

  while (sigVal < maxSig)
  {
    HSVColorSigVal closeHsv = closestHsvMatch(hsvMap, sigVal);
    OpacitySigVal  closeOp =  closestOpacityMatch(opacityMap, sigVal);

    QString sOut = QString::number(sigVal)            + ", " +
                   QString::number(closeHsv.m_h)      + ", " +
                   QString::number(closeHsv.m_s)      + ", " +
                   QString::number(closeHsv.m_v)      + ", " +
                   QString::number(closeOp.m_opacity) + "\n";
    file_fullMap << sOut.toLocal8Bit().data();

    sigVal += deltaSig;
  }
  file_fullMap.close();
}

// -------------------------------------------------------------
// Utility function used by System memory checking functions
// -------------------------------------------------------------
int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

// -------------------------------------------------------------
// Utility function used by System memory checking functions
// -------------------------------------------------------------
int getValue()
{
  // Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];

  while (fgets(line, 128, file) != NULL){
    if (strncmp(line, "VmRSS:", 6) == 0){
      result = parseLine(line);
      break;
    }
  }
  fclose(file);
  return result;
}

// -------------------------------------------------------------
// Function to print system memory information in log file.
// This is useful for debugging memory leak problems
// -------------------------------------------------------------
void PrintSystemMemoryInfo()
{
  struct sysinfo memInfo;

  sysinfo (&memInfo);
  int64_t totalVirtualMem = memInfo.totalram;
  // Add other values in next statement to avoid int overflow on right hand side...
  totalVirtualMem += memInfo.totalswap;
  totalVirtualMem *= memInfo.mem_unit;

  int64_t totalPhysMem = memInfo.totalram;
  // Multiply in next statement to avoid int overflow on right hand side...
  totalPhysMem *= memInfo.mem_unit;

  int64_t physMemUsed = memInfo.totalram - memInfo.freeram;
  // Multiply in next statement to avoid int overflow on right hand side...
  physMemUsed *= memInfo.mem_unit;

  int num_kb = getValue();
  ofstream myfile;
  myfile.open ("/analogic/Girish/memory_log.txt", ios::app);
  myfile << "\n\n Current memory snapshot";
  myfile << "\n Total virtual memory:" << (totalVirtualMem/(1024*1024));
  myfile << "\n Total RAM:" << (totalPhysMem/(1024*1024));
  myfile << "\n USED RAM:" << (physMemUsed/(1024*1024));
  myfile << "\n USED RAM by current process:" << (num_kb/1024) << " MB";

  myfile.close();
}

// -----------------------------------------------------------------------------------------------
// Function for writing counter of object creation/deletion in log file
// Example :
// 1) call in constructor
// analogic::workstation::WriteCounter("TouchHandler constructor",++memoryleakcounter);
// 2) in destructor
// analogic::workstation::WriteCounter("TouchHandler destructor",memoryleakcounter--);
// -----------------------------------------------------------------------------------------------
void WriteCounter(char* string, int counter)
{
  ofstream myfile;
  myfile.open ("/analogic/Girish/memory_leak_log.txt", ios::app);
  myfile << "\n " << string << " counter:" << counter;
  myfile.close();
}

// ------------------------------------------
// Write temporary logs for debug purpose
// analogic::workstation::WriteLogGPL("Hello");
// ------------------------------------------
void WriteLogGPL(const char* string)
{
  ofstream myfile;
  myfile.open ("/analogic/Girish/log.txt", ios::app);
  myfile << "\n " << string;
  myfile.close();
}

// ------------------------------------------
// Write temporary logs for debug purpose
// analogic::workstation::WriteLogGPL(QString("Hello"));
// ------------------------------------------
void WriteLogGPL(QString& str)
{
  ofstream myfile;
  myfile.open ("/analogic/Girish/log.txt", ios::app);
  myfile << "\n " << str.toStdString();
  myfile.close();
}

/**
 * @brief convertMapToString
 * @param ThreatAlarmType aType
 * @return QString
 */
QString convertMapToString(ThreatAlarmType aType)
{
  QString typeText;
  switch (aType) {
  case EXPLOSIVE:
    typeText = QObject::tr("Explosive");
    break;
  case LIQUID:
    typeText = QObject::tr("Liquid");
    break;
  case WEAPON:
    typeText = QObject::tr("Weapon");
    break;
  case CONTRABAND:
    typeText = QObject::tr("Contraband");
    break;
  case ANOMALY:
    typeText = QObject::tr("Anomaly");
    break;
  case SHIELD:
    typeText = QObject::tr("Shield");
    break;
  case LIQUID_AEROSOL_GAS:
    typeText = QObject::tr("LAG");
    break;
  case LAPTOP:
    typeText = QObject::tr("Laptop");
    break;
  case SHARP:
  case GUN:
  case BLUNT:
    typeText = QObject::tr("Prohibited-Item");
    break;
  case UNDEFINED:
    typeText = QObject::tr("UNDEFINED");
    break;
  default:
    typeText = QObject::tr("UNDEFINED");
    break;
  }
  return typeText;
}

//------------------------------------------------------------------------------
}  // namespace workstation

}  // namespace analogic
//------------------------------------------------------------------------------
