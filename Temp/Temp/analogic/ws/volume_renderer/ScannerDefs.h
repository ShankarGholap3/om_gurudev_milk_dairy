//-----------------------------------------------------------------------------
// File: ScannerDefs.h
// Purpose: Defines constants used in by the various scanners
// Copyright 2017-2020 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_SCANNERDEFS_H_
#define ANALOGIC_WS_VOLUME_RENDERER_SCANNERDEFS_H_
//------------------------------------------------------------------------------

#include <stdint.h>
#include <QColor>
#include <QDebug>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>


#define DEFAULT_OPACITY_SAMPLE_DISTANCE 1.0
#define DEFAULT_INTERACTIVE_SAMPLE_DISTANCE 1.0

//------------------------------------------------------------------------------

const double CHECKPOINT_MIN_NUMBER_SLICES = 7;

//------------------------------------------------------------------------------

template<class T>
std::string t_to_string(T i)
{
  std::stringstream ss;
  std::string s;
  ss << i;
  s = ss.str();

  return s;
}


//----------------------------------------------
// Assuming  MAX SIZE = (1042)x(1024)x(1024)x2
//  W x H x L x (2 bytes)
#define GPU_RENDERER_MEMORY_SIZE     2147483648
#define GPU_RENDERER_MEMORY_FRACTION 0.99
//----------------------------------------------



enum MaterialTransition_E
{
  SIG_LOW_DENSITY_MIN     = 0,
  SIG_ORGANIC_MIN         = 1,
  SIG_ORGANIC_MAX         = 2,
  SIG_INORGANIC_MIN       = 3,
  SIG_INORGANIC_MAX       = 4,
  SIG_METAL_MIN           = 5,
  SIG_METAL_MAX           = 6,
  SIG_THREAT_MIN          = 7,
  SIG_THREAT_SET_A        = 8,
  SIG_THREAT_SET_B        = 9,
  SIG_THREAT_SET_C        = 10,
  SIG_THREAT_SET_D        = 11,
  SIG_THREAT_SET_E        = 12,
  SIG_THREAT_SET_F        = 13,
  SIG_THREAT_MAX          = 14,
  SIG_DEFAULT_NOISE_CLIP  = 15,
};

class ThretOffsetProperty
{
public:
  ThretOffsetProperty(uint16_t anOffset,
                      int anIndex)
  {
    m_threatOffset = anOffset;
    m_vtkIndex     = anIndex;
  }
  uint16_t m_threatOffset;
  int      m_vtkIndex;
private:
  ThretOffsetProperty();
};

class OpacitySigVal
{
public:
  double m_signalVal;
  double m_opacity;
  std::string toString()
  {
    std::string retS = "SIGNAL: " +
        t_to_string(m_signalVal) + "," +
        "VALUE: " + t_to_string(m_opacity);
    return retS;
  }
};


class HSVColorSigVal
{
public:
  double m_signalVal;
  double m_h;
  double m_s;
  double m_v;
  std::string toString()
  {
    std::string retS = "SIGNAL: " +
        t_to_string(m_signalVal) + "," +
        "H,S,V: " + t_to_string(m_h) + "," +
        t_to_string(m_s) + "," +
        t_to_string(m_v);
    return retS;
  }
};


class GrayscaleValueOpacityStruct
{
public:
  GrayscaleValueOpacityStruct():
    m_HSV_V_organicMin(0),
    m_HSV_V_organicMax(0),
    m_HSV_V_organicMin_inv(0),
    m_HSV_V_organicMax_inv(0),
    m_opacityOrganicMin(0),
    m_opacityOrganicMax(0),

    m_HSV_V_inorganicMin(0),
    m_HSV_V_inorganicMax(0),
    m_HSV_V_inorganicMin_inv(0),
    m_HSV_V_inorganicMax_inv(0),
    m_opacityInorganicMin(0),
    m_opacityInorganicMax(0),

    m_HSV_V_metalMin(0),
    m_HSV_V_metalMax(0),
    m_HSV_V_metalMin_inv(0),
    m_HSV_V_metalMax_inv(0),
    m_opacityMetalMin(0),
    m_opacityMetalMax(0)
  {
  }

  double m_HSV_V_organicMin;
  double m_HSV_V_organicMax;
  double m_HSV_V_organicMin_inv;
  double m_HSV_V_organicMax_inv;
  double m_opacityOrganicMin;
  double m_opacityOrganicMax;

  double m_HSV_V_inorganicMin;
  double m_HSV_V_inorganicMax;
  double m_HSV_V_inorganicMin_inv;
  double m_HSV_V_inorganicMax_inv;
  double m_opacityInorganicMin;
  double m_opacityInorganicMax;

  double m_HSV_V_metalMin;
  double m_HSV_V_metalMax;
  double m_HSV_V_metalMin_inv;
  double m_HSV_V_metalMax_inv;
  double m_opacityMetalMin;
  double m_opacityMetalMax;
};

//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_SCANNERDEFS_H_
//------------------------------------------------------------------------------
