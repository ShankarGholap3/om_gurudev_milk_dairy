//-------------------------------------------------------------------------------
// File: ThreatList.h
// Purpose: Maintain threat information to displayed in the volume renderer
// Copyright 2016 Analogic Corp.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_THREATLIST_H_
#define ANALOGIC_WS_VOLUME_RENDERER_THREATLIST_H_
//-------------------------------------------------------------------------------
#include <stdint.h>
#include <QTime>
#include <QDebug>
#include <QVector3D>
#include <list>
#include <map>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include "Logger.h"
#include "VolumeRendererBase.h"
#include "ThreatObject.h"
#include "ScannerDefs.h"


#define EXP_EXT_LENGTH  4
#define EXP_EXTENSION   ".vol"

class ThreatList
{
  public:
  explicit ThreatList(double pixelSpacingX,
                      double pixelSpacingY,
                      double pixelSpacingZ,
                      QVector3D imageOrigin,
                      bool visibleThreatwidget);

  virtual ~ThreatList();

  void setVolumeRenderer(VolumeRendererBase* renderer);

  int  listSize();
  void getThreatBoundaries(int threatIndex,
                        int& outW,
                        int& outH,
                        int& outL,
                        QVector3D& outP0);

  int getNewOperatorThreatID();

  //--------------------------------------------------------
  // ARO-Note: Why we need 2 clear threat at index methods.
  //
  //   At the PVS we can clear OPERATOR generated threats
  // at will and final index of the threat is not set until
  // we suspect the bag.  So a clear here needs to be able
  // to re-order OPERATOR generated threats.
  void clearThreatMoveOperators(int threatIndex);
  // At the SVS the threat order is fixed so the threat clearing is
  // straight forward.
  void clearThreatDirect(int threatIndex);
  //--------------------------------------------------------


  void suspectThreatAt(int threatIndex);
  void clearAllThreats();
  void addThreatObject(ThreatObject* newThreat);
  void setActorAtIndex(unsigned int setIndex,
                       vtkSmartPointer<vtkActor> anActor);


  QStringList getThreatIDs();

  std::list<ThreatObject*> getThreatObjectList();

  void appendSDICOSBmp(SDICOS::Bitmap& aBmp);
  bool isActive();
  void clear();
  void clearData();
  int  size();
  int  unClearedThreats();
  int  machineThreats();
  int  operatorThreats();
  int  laptopThreats();
  int  unClearedMachineThreats();
  int  clearedMachineThreats();


  ThreatObject* getThreatAtIndex(int itemIndex);

  ThreatObject* findNextAvailbleThreatIndex();
  ThreatObject* findFirstNonThreatLaptop();
  int nonThreatLaptopCount();

  std::vector<uint16_t>& getThreatImageData();
  std::vector<uint16_t>& getThreatStashBuffer();

  void setCurrent(int index);
  void hideLaptops();
  void forceHideLaptops();
  void hideMachineThreats();
  void unHideMachineThreats();
  void hideThreatAt(int index);
  int getCurrent();
  QString currentText();
  int     currentVolume();
  QString getTextAt(int index);
  QString getIdAt(int index);
  SDICOS::Bitmap* bmpAtIndex(unsigned int index);
  bool isLaptopAtIndex(int index);
  void setAsTip();
  void setAsTipFalse();
  void suspectBag();
  bool isCurrent(int itemIndex);
  void setLaptopViewMode(bool lapMode);
  void setSurfaceViewMode(bool surfaceMode);
  void setSlabingLaptop(bool bSetVal);
  void setMachineThreatsEndbled(bool setVal);

private:
  void  saveVolumeData(QString& dataFile,
                      uint16_t* dataBuffer,
                      int sizeBuffer);

  void saveVolumeHeader(QString& headerFile,
                        int W,
                        int H,
                        int L);


  QVector3D                    m_imageOffset;
  bool                         m_visibleThreatWidget;
  bool                         m_laptopViewMode;
  bool                         m_surfaceViewMode;
  bool                         m_slabbingLaptop;
  bool                         m_isATipBag;
  bool                         m_bagIsSuspected;
  bool                         m_machineThreatsEnabled;
  double                       m_pixelSpacingX;
  double                       m_pixelSpacingY;
  double                       m_pixelSpacingZ;
  int                          m_currentIndex;
  int                          m_stashIndex;
  int                          m_sizeList;
  std::vector<ThreatObject*>   m_objectVec;

  //----------------------------------------------------
  // Vectors used to keep the image data used in
  // the threats.
  // A. m_stachVec:
  //    An unadulterated image data that is coppied from the
  //    vtrkVolume renderer image buffer. This data is
  //    replaced when the active threat is cleard or changed.
  //
  // B. m_threatBuffer:
  //    Used to hold data that is then sent to the
  //    ThreatVolumeRenderer. This buffer should be
  //    deleted when the ThthreatVollumeRenderer has
  //    completed it's first full render.

  std::vector<uint16_t>        m_stashVec;
  std::vector<uint16_t>        m_threatBuffer;

  //----------------------------------------------------
  VolumeRendererBase*          m_volumeRenderer;
};

//-------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_THREATLIST_H_
//-------------------------------------------------------------------------------
