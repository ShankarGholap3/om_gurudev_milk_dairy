/*!
* @file     VREnums.h
* @author   Agiliad
* @brief    Common include for ENUMS used across Workatation and Volumrenderer interface
* @date     Sep, 26 2016
*
(c) Copyright 2018 Analogic Corporation. All Rights Reserved
*/
//-------------------------------------------------------------------------------
// File: VREnums.h
// Purpose: Common include for ENUMS used across Workatation and Volumrenderer interface
// Copyright 2018 Analogic Corp.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VRENUMS_H_
#define ANALOGIC_WS_VRENUMS_H_
//-------------------------------------------------------------------------------

#include <QtGlobal>
#include <qqml.h>

/*!
 * \class   VREnums
 * \brief   This class contains ENUMS used across Workatation and Volumrenderer interface
 *
 */
class VREnums: public QObject
{
  Q_OBJECT

public:
  /*!
  * @fn       VREnums();
  * @param    QObject* - parent
  * @return   None
  * @brief    Constructor responsible for initialization of class members, memory and resources.
  */
  explicit VREnums(QObject *parent = NULL):QObject(parent) {}


  /**
 * Enumerat the ways the Volume viewer can view the bag data
 *
 */
  enum VRViewModeEnum
  {
    VIEW_AS_THREAT         = 0,
    VIEW_AS_SURFACE        = 1,
    VIEW_AS_LAPTOP         = 2,
    VIEW_AS_SURFACE_CAMERA = 3,
    VIEW_AS_SLAB           = 4,
  };


  /**
 * Enumeration for Threat Type
 *
 */
  enum ThreatType
  {
    THREATTYPE_UNKNOWN_THREAT  = 0,
    THREATTYPE_ATR_THREAT      = 1,
    THREATTYPE_OPERTOR_THREAT  = 2,
    THREATTYPE_LAPTOP          = 3,
    THREATTYPE_TRUTH_THREAT    = 4,
  };

  /**
 * Enumerate the kinds of units used to display dimensions
 */
  enum DispalyUnitEnum
  {
    DISPLAY_UNIT_CM   = 0,
    DISPLAY_UNIT_INCH = 1,
  };

  /**
 * Enumerate the units used to display systems.
 */
  enum DispalyUnitSystemEnum
  {
    METRIC        = 0,
    US_IMPERIAL   = 1,
  };


  /**
 * Enumerate the coordinate axes
 */
  enum AXIS
  {
    X = 0,
    Y = 1,
    Z = 2,
  };

  /**
 * Enumerate the slab type
 */
  enum SlabViewType
  {
    BAG_VIEW = 0,
    THREAT_VIEW = 1,
    LAPTOP_VIEW = 2,
  };


  Q_ENUM(VRViewModeEnum);
  Q_ENUM(DispalyUnitEnum);
  Q_ENUM(DispalyUnitSystemEnum);
  Q_ENUM(AXIS);
  Q_ENUM(ThreatType);
  Q_ENUM(SlabViewType);

  static void declareQML()
  {
    qmlRegisterType<VREnums>("VREnums", 1, 0, "VREnums");
  }
};

Q_DECLARE_METATYPE(VREnums::VRViewModeEnum)
Q_DECLARE_METATYPE(VREnums::DispalyUnitEnum)
Q_DECLARE_METATYPE(VREnums::DispalyUnitSystemEnum)
Q_DECLARE_METATYPE(VREnums::AXIS)
Q_DECLARE_METATYPE(VREnums::ThreatType)
Q_DECLARE_METATYPE(VREnums::SlabViewType)


//-------------------------------------------------------------------------------
#endif  //  ANALOGIC_WS_VRENUMS_H_
//-------------------------------------------------------------------------------
