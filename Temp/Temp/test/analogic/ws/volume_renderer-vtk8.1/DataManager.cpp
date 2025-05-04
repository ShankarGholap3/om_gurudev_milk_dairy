//------------------------------------------------------------------------------
// File: DataManager.cpp
// Description: Data Mamager for Volume Renderer Application  implementation
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------
#include "DataManager.h"


//------------------------------------------------------------------------------
DataManager::DataManager(QString& volumeFile,
                         std::list<QString>& tdrList):
  m_ptrData(nullptr),
  m_volumeSourceFile(volumeFile),
  m_numSlices(0),
  m_numData(0)
{
  this->setTDRSourceList(tdrList);
}


//------------------------------------------------------------------------------
DataManager::~DataManager()
{
  this->deleteData();
  this->clearTDRList();
}


//------------------------------------------------------------------------------
void DataManager::setTDRSourceList(std::list<QString>& tdrList)
{
  std::list<QString>::iterator it;
  m_tdrSourceList.clear();
  for (it = tdrList.begin(); it!= tdrList.end(); it++)
  {
    QString strIt = *it;
    m_tdrSourceList.push_back(strIt);
  }
}


//------------------------------------------------------------------------------
bool DataManager::readTDRModules(std::string& sdicosLogFile)
{
  QTime timerReadTDDs = QTime::currentTime();

  this->clearTDRList();
  int numTDRs = m_tdrSourceList.size();
  for (int i = 0; i < numTDRs; i++)
  {
    SDICOS::TDRModule* ptDR;
    QString sFile = m_tdrSourceList.at(i);
    std::string tdrFilePath = sFile.toLocal8Bit().data();
    std::string strResult;
    bool readResult = analogic::workstation::readTDRModule(tdrFilePath,
                                                            sdicosLogFile,
                                                            strResult,
                                                            &ptDR);

    if (readResult)
    {
      m_tdrDataList.push_back(ptDR);
      m_tdrsGenerators.push_back(analogic::workstation::ThreatGenerationType::ATR_GEN);
    }
  }
  int msReadTDRs = timerReadTDDs.elapsed();
  QString tdrMessage = "*TTTTTT* - TDR Read Time (ms)" + QString::number(msReadTDRs);
  LOG(INFO) << tdrMessage.toLocal8Bit().data();
}

//----------------------------------------------------------------------------
void DataManager::resetPaths(QString& volumeFile,
               std::list<QString>& tdrList)
{
   m_volumeSourceFile = volumeFile;
   this->setTDRSourceList(tdrList);
}

//----------------------------------------------------------------------------
void DataManager::clearTDRList()
{
  if (m_tdrDataList.size() > 0)
  {
    for (int i = 0; i < m_tdrDataList.size(); i++)
    {
      SDICOS::TDRModule* pTDR = m_tdrDataList.at(i);
      delete pTDR;
    }
  }
  m_tdrDataList.clear();
  m_tdrsGenerators.clear();
}

//------------------------------------------------------------------------------
bool DataManager::loadVolumeData()
{
  if (m_ptrData != nullptr)
  {
    this->deleteData();
  }
  m_numSlices = 0;

  QFile volFile(m_volumeSourceFile);
  if (!volFile.exists())
  {
    QString errorMessage = m_volumeSourceFile;
    errorMessage += " \nDoes not exist!";
    QMessageBox::warning(0,
                         "Application Error!",
                         errorMessage);
    return false;
  }

  std::string dataFileName = m_volumeSourceFile.toLocal8Bit().data();

  analogic::workstation::ImageProperties *iP;

  QString nameTail = m_volumeSourceFile.right(VOL_EXT_LENGTH);
  if (nameTail.compare(COBRA_VOL_FILE_EXTENSION) == 0)
  {
    iP = new analogic::workstation::ImageProperties(analogic::workstation::SCANNER_TYPE_COBRA);
  }
  else if (nameTail.compare(CHECKPOINT_VOL_FILE_EXTENSION) == 0)
  {
    iP = new analogic::workstation::ImageProperties(analogic::workstation::SCANNER_TYPE_CHECKPOINT);
  }
  else
  {
    if (m_tdrDataList.size() > 0)
    {
      for (int i = 0; i < m_tdrDataList.size() -1; i++)
      {
        SDICOS::TDRModule* pTDR = m_tdrDataList.at(i);
        delete pTDR;
      }
    }
    m_tdrDataList.clear();
    iP = new analogic::workstation::ImageProperties(analogic::workstation::SCANNER_TYPE_CHECKPOINT);
  }

  int64_t sizeFile = volFile.size();

  m_numSlices = sizeFile/( sizeof(uint16_t)* iP->m_imageWidth * iP->m_imageHeight );

  m_numData = sizeFile/( sizeof(uint16_t));

  int64_t sizeBuffer = sizeFile/(sizeof(uint16_t));
  if (m_ptrData != nullptr)
  {
    QMessageBox::warning(nullptr,
                         "Error on volume creation possible leak",
                         "Creatinf Volume data on non empty Pointer");
  }
  m_ptrData = new uint16_t[sizeBuffer];

  FILE* filep = fopen(dataFileName.c_str(), "rb");
  if (!filep)
  {
    QString errorMessage = "Error opening file: ";
    errorMessage += m_volumeSourceFile;
    QMessageBox::warning(0,
                         "Application Error.",
                         errorMessage);
    QApplication::quit();
  }

  int bytes_read = fread(reinterpret_cast<char*>(m_ptrData), sizeof(char), sizeFile, filep);

  double dSizeMB = (1.0*sizeFile)/(1024.0*1024.0);
  double dSizeReadMB = (1.0*bytes_read)/(1024.0*1024.0);

  LOG(INFO) << "*SSSS* - Size of Volume Data File (MB):" << dSizeMB << " - MBytes read:" << dSizeReadMB;
  fclose(filep);

  delete iP;
  return true;
}

//------------------------------------------------------------------------------
QString DataManager::volumeFilePath()
{
  return m_volumeSourceFile;
}

//-----------------------------------------------------------------------------
std::vector<SDICOS::TDRModule*> DataManager::tdrList()
{
  return m_tdrDataList;
}

//-----------------------------------------------------------------------------
std::vector<int> DataManager::tdrsGenList()
{
  return m_tdrsGenerators;
}

//------------------------------------------------------------------------------
QString DataManager::tdrFilePathAt(int index)
{
  if (index >  (m_tdrSourceList.size() -1)) return "";
  return m_tdrSourceList.at(index);
}



//------------------------------------------------------------------------------
void DataManager::deleteData()
{
  delete [] m_ptrData;
  m_ptrData = nullptr;
}


//------------------------------------------------------------------------------
void restPaths(QString& volumeFile,
               std::list<QString>& tdrList)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
uint16_t* DataManager::dataPoionter()
{
  return m_ptrData;
}

//------------------------------------------------------------------------------
int DataManager::sliceCount()
{
  return m_numSlices;
}

//------------------------------------------------------------------------------
void DataManager::setSliceCount(int numSlices)
{
 m_numSlices = numSlices;
}

//------------------------------------------------------------------------------
int DataManager::dataCount()
{
  return m_numData;
}
