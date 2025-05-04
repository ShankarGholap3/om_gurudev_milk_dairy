/*!
* @file     baginfo.h
* @author   Agiliad
* @brief    Info about bag data.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_BAGDATA_BAGINFO_H_
#define ANALOGIC_WS_COMMON_BAGDATA_BAGINFO_H_

#include <QString>
#include <QDateTime>
#include <QVector3D>
#include <qtranslator.h>
#include <analogic/nss/agent/search/SearchListEntry.h>
#include "SDICOS/SdcsThreatDetectionReport.h"
#include "SDICOS/SdcsTDR.h"

namespace analogic
{
namespace ws
{
/*!
 * \class   BagInfo
 * \brief   Information about a bag and a wrapper of SearchListEntry.
 */
class BagInfo : public analogic::nss::SearchListEntry
{
  Q_GADGET
  Q_PROPERTY(QString searchListEntryId READ searchListEntryId)
  Q_PROPERTY(QString displayableBagId READ displayableBagId)
  Q_PROPERTY(QString thumbnailUrl READ thumbnailUrl)
  Q_PROPERTY(QString displayableCurrentSearchStationId READ
             displayableCurrentSearchStationId)
  Q_PROPERTY(QString bagFileNamePattern READ bagFileNamePattern)
  Q_PROPERTY(QString dateTime READ dateTime)
  Q_PROPERTY(QString machineid READ machineid)
  Q_PROPERTY(QString machineDecision READ machineDecision)
  Q_PROPERTY(QString userDecision READ userDecision)
  Q_PROPERTY(QString reasonforsearch READ reasonforsearch)
  Q_PROPERTY(bool obviousthreatpresent READ obviousthreatpresent)
  Q_PROPERTY(QString bagType READ bagType)
  Q_PROPERTY(QString rfID READ rfID)
  Q_PROPERTY(QString diverterReason READ diverterReason)
  Q_PROPERTY(QString laneLocation READ laneLocation)
public:
  QString searchListEntryId() const { return QString::fromStdString(
          nss_bag_id); }
  QString displayableBagId() const { return QString::fromStdString(
          displayable_bag_id); }
  QString thumbnailUrl() const { return QString::fromStdString(
          thumbnail_url);}
  QString displayableCurrentSearchStationId() const {
    return QString::fromStdString(displayable_current_search_station_id); }
  QString bagFileNamePattern() const { return m_fileNamePattern; }
  QString dateTime() const { return m_timeStampStr; }
  QString machineid() const { return m_machineId; }
  QString reasonforsearch() const { return QString::fromStdString(reason_for_search); }
  bool obviousthreatpresent() const { return has_obvious_threat; }
  QString bagType() const { return m_bagType; }
  QString rfID() const { return QString::fromStdString(rfid); }
  QString diverterReason() const { return m_diverterReason; }
  QString laneLocation() const { return m_location; }

  QString machineDecision() const {
    QString value;
    if (machine_alarm_decision == SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision)
    {
      value = QObject::tr("UNKNOWN");
    }
    else if (machine_alarm_decision == SDICOS::TDRTypes::ThreatDetectionReport::enumClear)
    {
      value = QObject::tr("CLEAR");
    }
    else if (machine_alarm_decision == SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm)
    {
      value = QObject::tr("ALARM");
    }
    return value;
  }
  QString userDecision() const {
    QString value;
    if (user_alarm_decision == SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision)
    {
      value = QObject::tr("UNPROCESSED");
    }
    else if (user_alarm_decision == SDICOS::TDRTypes::ThreatDetectionReport::enumClear)
    {
      value = QObject::tr("CLEAR");
    }
    else if (user_alarm_decision == SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm)
    {
      value = QObject::tr("SUSPECT");
    }
    return value;
  }

  /*!
    * @fn       BagInfo
    * @param    None
    * @return   None
    * @brief    Constructor for class BagInfo.
    */
  BagInfo();

  /*!
    * @fn       BagInfo
    * @param    const BagInfo &
    * @return   None
    * @brief    Copy Constructor for class BagInfo.
    */
  BagInfo(const BagInfo &);

  /*!
    * @fn       ~BagInfo
    * @param    None
    * @return   None
    * @brief    Destructor for class BagInfo.
    */
  ~BagInfo();

  /*!
    * @fn       constructFromFile
    * @param    const QString filename
    * @return   bool success or failure
    * @brief    Construct from file.
    */
  bool constructFromFile(const QString filename);

  /*!
    * @fn       writeToFile
    * @param    const QString filename
    * @return   bool success or failure
    * @brief    Write contents to a file.
    */
  bool writeToFile(const QString filename);

  /*!
    * @fn       clear
    * @param    none
    * @return   none
    * @brief    Clear it.
    */
  void clear();

  QString m_timeStampStr;             //!< Time when bag was saved.
  QString m_sliceCount;               //!< Slice count.
  QString m_volWidth;                 //!< width.
  QString m_volHeight;                //!< height.
  QString m_fileNamePattern;          //!< Local folder path where saved.
  QString m_machineId;                //!< Machine Id.
  QString m_userId;                   //!< User Id.
  QString m_sliceFormat;              //!< Slice Format
  QVector3D m_roiStart;               //!< ROI start dimension
  QVector3D m_roiEnd;                 //!< ROI start dimension
  QString m_bagType;                  //!< Bag type - TIP or LIVE
  QString m_diverterReason;           //!< Diverter Reason
  QString m_location;                 //!< Location 
};
}  // namespace ws
}  // namespace analogic
Q_DECLARE_METATYPE(analogic::ws::BagInfo)
#endif  // ANALOGIC_WS_COMMON_BAGDATA_BAGINFO_H_
