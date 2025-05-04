//------------------------------------------------------------------------------
// File: ThreatObject.h
// Description: Implementation of Threat Object class
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef  ANALOGIC_WS_VOLUME_RENDERER_THREATOBJECT_H_
#define  ANALOGIC_WS_VOLUME_RENDERER_THREATOBJECT_H_
//------------------------------------------------------------------------------
#include <QVector3D>
#include <QString>
#include <QDebug>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <SDICOS/SdcsTemplateCT.h>
#include "ThreatDefs.h"
#include "ThreatVolume.h"


#define CLEAR_THREATBOX_OPACITY     0.0
#define HIDE_THREATBOX_OPACITY      0.01
#define ACTIVE_THREATBOX_OPACITY    0.95
#define INACTIVE_THREATBOX_OPACITY  0.3

#define THREAT_BOUNDS_LINE_WIDTH    4.8


#define EXPLOSIVE_THREAT_COLOR_RED     200
#define EXPLOSIVE_THREAT_COLOR_GREEN    13
#define EXPLOSIVE_THREAT_COLOR_BLUE     13

#define LIQUID_THREAT_COLOR_RED        216
#define LIQUID_THREAT_COLOR_GREEN       13
#define LIQUID_THREAT_COLOR_BLUE        13

#define WEAPON_THREAT_COLOR_RED        216
#define WEAPON_THREAT_COLOR_GREEN       13
#define WEAPON_THREAT_COLOR_BLUE        13


#define CONTRABAND_THREAT_COLOR_RED    216
#define CONTRABAND_THREAT_COLOR_GREEN   13
#define CONTRABAND_THREAT_COLOR_BLUE    13

#define ANOMALY_THREAT_COLOR_RED       216
#define ANOMALY_THREAT_COLOR_GREEN      13
#define ANOMALY_THREAT_COLOR_BLUE       13

#define SHIELD_THREAT_COLOR_RED        200
#define SHIELD_THREAT_COLOR_GREEN      200
#define SHIELD_THREAT_COLOR_BLUE        13

#define LAG_THREAT_COLOR_RED           216
#define LAG_THREAT_COLOR_GREEN          13
#define LAG_THREAT_COLOR_BLUE           13

#define LAPTOP_THREAT_COLOR_RED        200
#define LAPTOP_THREAT_COLOR_GREEN       13
#define LAPTOP_THREAT_COLOR_BLUE        13

#define SHARP_THREAT_COLOR_RED          13
#define SHARP_THREAT_COLOR_GREEN       173
#define SHARP_THREAT_COLOR_BLUE        216

#define GUN_THREAT_COLOR_RED            13
#define GUN_THREAT_COLOR_GREEN         173
#define GUN_THREAT_COLOR_BLUE          236

#define BLUNT_THREAT_COLOR_RED          13
#define BLUNT_THREAT_COLOR_GREEN       173
#define BLUNT_THREAT_COLOR_BLUE        236

#define UNDEFINED_THREAT_COLOR_RED     216
#define UNDEFINED_THREAT_COLOR_GREEN    13
#define UNDEFINED_THREAT_COLOR_BLUE     13

#define OPERATOR_THREAT_COLOR_RED      236
#define OPERATOR_THREAT_COLOR_GREEN     14
#define OPERATOR_THREAT_COLOR_BLUE     140

class ThreatObject
{
public:
  enum ThreatState
  {
    inactive  = 0,
    active    = 1,
    cleared   = 2,
    suspected = 3,
  };

  ThreatObject(int index,
               ThreatVolume& vol,
               QString id,
               QString text,
               SDICOS::Bitmap *bmpRef);

  virtual ~ThreatObject();

  ThreatState getState();
  void setThreatState(ThreatState aState,
                bool laptopViewMode,
                bool machineThretsEnabled,
                bool bagIsSuspected,
                bool isATipBag,
                bool slabbingLaptop,
                bool surfaceView);
  void suspectThreat();
  void nullActor();
  void setType(analogic::workstation::ThreatAlarmType aType);
  void setGenType(analogic::workstation::ThreatGenerationType aGenType);
  void setMass(double mass);
  double getMass();
  void setOffset(uint16_t offset);
  uint16_t getOffset();
  analogic::workstation::ThreatGenerationType getGenType();
  analogic::workstation::ThreatAlarmType getType();


  void setActor(vtkSmartPointer<vtkActor> anActor);
  vtkActor* getActor();
  bool isActorNull();
  ThreatVolume getVolume();
  int getVolumeSize();
  QString getText();
  QString getID();
  int     getIndex();
  void    setIndex(int newIndex);
  SDICOS::Bitmap* getBMP();
  void setBMP(SDICOS::Bitmap* aBMP);
  bool compare(ThreatObject& other);
  void clearThreat();
  bool isLapTop();
  void setAsLaptop(bool val);
  void setMachineThreatsEndabled(bool setVal);

private:
  ThreatState                                 m_state;
  vtkActor*                                   m_threatActorRef;
  ThreatVolume                                m_volume;
  QString                                     m_text;
  QString                                     m_ID;
  int                                         m_index;
  SDICOS::Bitmap*                             m_pBmp;
  analogic::workstation::ThreatAlarmType      m_threatType;
  analogic::workstation::ThreatGenerationType m_threatGenType;
  uint16_t                                    m_offset;
  double                                      m_mass;
  bool                                        m_isLaptop;
};

//------------------------------------------------------------------------------
#endif  //  ANALOGIC_WS_VOLUME_RENDERER_THREATOBJECT_H_
//------------------------------------------------------------------------------
