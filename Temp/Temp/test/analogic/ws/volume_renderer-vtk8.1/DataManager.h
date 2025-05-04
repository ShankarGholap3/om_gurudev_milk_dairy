//------------------------------------------------------------------------------
// File: DataManager.h
// Description: Data Mamager for Volume Renderer Application  header
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_DATAMANAGER_H_
#define TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_DATAMANAGER_H_
//-------------------------------------------------------------------------------

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <cstdio>
#include <cstdlib>
#include "Logger.h"
#include "VRUtils.h"


class DataManager
{
public:
  DataManager(QString& volumeFile,
              std::list<QString>& tdrList);
  virtual ~DataManager();

  bool readTDRModules();
  bool loadVolumeData();
  bool readTDRModules(std::string& sdicosLogFile);
  void deleteData();
  void resetPaths(QString& volumeFile,
                 std::list<QString>& tdrList);
  void setSliceCount(int numSlices);
  QString volumeFilePath();
  QString tdrFilePathAt(int index);
  uint16_t* dataPoionter();
  std::vector<SDICOS::TDRModule*> tdrList();
  std::vector<int> tdrsGenList();
  int sliceCount();
  int dataCount();
  //------------------------------------------------------------------------------



private:
  void clearTDRList();
  void setTDRSourceList(std::list<QString>& tdrList);

  uint16_t*                       m_ptrData;
  QString                         m_volumeSourceFile;
  std::vector<QString>            m_tdrSourceList;
  std::vector<SDICOS::TDRModule*> m_tdrDataList;
  std::vector<int>                m_tdrsGenerators;
  int                             m_numSlices;
  int                             m_numData;
};

//-------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_DATAMANAGER_H_
//-------------------------------------------------------------------------------




