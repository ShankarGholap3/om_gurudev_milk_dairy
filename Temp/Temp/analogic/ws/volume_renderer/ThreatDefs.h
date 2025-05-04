//------------------------------------------------------------------------------
// File: ThreatDefs.h
// Description: Threat definitions
// (c) Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_THREATDEFS_H_
#define ANALOGIC_WS_VOLUME_RENDERER_THREATDEFS_H_
//------------------------------------------------------------------------------

#include <QString>
#include <QColor>
#include <map>


namespace analogic
{

namespace workstation
{

enum ThreatAlarmType
{
  // SDICOS STANDARD TYPES
  EXPLOSIVE          = 0,
  LIQUID             = 1,
  WEAPON             = 2,
  CONTRABAND         = 3,
  ANOMALY            = 4,
  // HITORIC ANALOGIC TYPES
  BULK               = 5,
  SHIELD             = 6,
  LIQUID_AEROSOL_GAS = 7,
  LAPTOP             = 8,
  SHARP              = 9,
  GUN                = 10,
  BLUNT              = 11,
  UNDEFINED          = 12,
};


enum ThreatGenerationType
{
  ATR_GEN         = 0,
  OPERATOR_GEN    = 1,
  UNKNOWN_GEN     = 2,
};

typedef std::map<ThreatAlarmType, QString> ThreatTypeMap;

typedef std::map<ThreatAlarmType, QColor> ThreatBoxTypeColorMap;


}  // namespace workstation

}  // namespace analogic


//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_THREATDEFS_H_
//------------------------------------------------------------------------------
