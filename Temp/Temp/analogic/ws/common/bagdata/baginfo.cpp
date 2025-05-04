/*!
* @file     baginfo.cpp
* @author   Agiliad
* @brief    Simple structure for storing info about bag.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/baginfo.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       BagInfo
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class BagInfo.
*/
BagInfo::BagInfo() :
  analogic::nss::SearchListEntry("",  // arg_nss_bag_id
                                 "",   // arg_displayable_bag_id
                                 0,    // arg_bag_timestamp
                                 "",   // arg_machine_id
                                 SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision,   // arg_machine_alarm_decision
                                 SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAbortFlag,       // arg_machine_abort_flag
                                 SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAbortReason,     // arg_machine_abort_reason
                                 "",  // arg_user_id
                                 SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision,   // arg_user_alarm_decision
                                 SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAbortFlag,       // arg_user_abort_flag
                                 SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAbortReason,     // arg_user_abort_reason
                                 "",     // arg_reason_for_search
                                 "",     // arg_thumbnail_url
                                 "",     // arg_displayable_current_search_station_id
                                 false,  // arg_has_obvious_threat
                                 "",      // arg_rfid
                                 ""       //arg_location
                                 )
{
  m_bagType = "UNKNOWN";  // default to unknown
  m_diverterReason = "";  // for now set default as empty
  TRACE_LOG("");
}

/*!
* @fn       BagInfo
* @param    const BagInfo &
* @return   None
* @brief    Copy Constructor for class BagInfo.
*/
BagInfo::BagInfo(const BagInfo &sbli) :
  analogic::nss::SearchListEntry(sbli.nss_bag_id,
                                 sbli.displayable_bag_id,
                                 sbli.bag_timestamp,
                                 sbli.machine_id,
                                 sbli.machine_alarm_decision,
                                 sbli.machine_abort_flag,
                                 sbli.machine_abort_reason,
                                 sbli.user_id,
                                 sbli.user_alarm_decision,
                                 sbli.user_abort_flag,
                                 sbli.user_abort_reason,
                                 sbli.reason_for_search,
                                 sbli.thumbnail_url,
                                 sbli.displayable_current_search_station_id,
                                 sbli.has_obvious_threat,
                                 sbli.rfid,
                                 sbli.location
                                  )
{
  TRACE_LOG("");
  m_machineId = sbli.machine_id.c_str();
  m_fileNamePattern = sbli.m_fileNamePattern;
  m_sliceCount = sbli.m_sliceCount;
  m_timeStampStr = sbli.m_timeStampStr;
  m_volHeight = sbli.m_volHeight;
  m_volWidth = sbli.m_volWidth;
  m_sliceFormat = sbli.m_sliceFormat;
  m_userId = sbli.m_userId;
  m_bagType = sbli.m_bagType;
  m_diverterReason = sbli.m_diverterReason;
  m_location = sbli.m_location;
}

/*!
* @fn       ~BagInfo
* @param    None
* @return   None
* @brief    Destructor for class BagInfo.
*/
BagInfo::~BagInfo()
{
  clear();
}
/*!
* @fn       clear
* @param    none
* @return   none
* @brief    Clear it.
*/
void BagInfo::clear()
{
  nss_bag_id.clear();
  displayable_bag_id.clear();
  m_timeStampStr.clear();
  m_sliceCount.clear();
  m_volWidth.clear();
  m_volHeight.clear();
  m_fileNamePattern.clear();
  m_machineId.clear();
  m_sliceFormat.clear();
  m_userId.clear();
  m_bagType.clear();
  rfid.clear();
  m_diverterReason.clear();
}

/*!
* @fn       writeToFile
* @param    const QString filename
* @return   bool success or failure
* @brief    Write contents to a file.
*/
bool BagInfo::writeToFile(const QString filename)
{
  FILE* fp = fopen(filename.toStdString().c_str(), "w");
  if(fp)
  {
    std::fprintf(fp, "volume_width=%s\n", m_volWidth.toStdString().c_str());
    std::fprintf(fp, "volume_height=%s\n", m_volHeight.toStdString().c_str());
    std::fprintf(fp, "slice_count=%s\n", m_sliceCount.toStdString().c_str());
    std::fprintf(fp, "slice_format=%s\n", m_sliceFormat.toStdString().c_str());
    std::fprintf(fp, "volume_roi_start_x=%s\n", QString::number(m_roiStart.x()).toStdString().c_str());
    std::fprintf(fp, "volume_roi_start_y=%s\n", QString::number(m_roiStart.y()).toStdString().c_str());
    std::fprintf(fp, "volume_roi_start_z=%s\n", QString::number(m_roiStart.z()).toStdString().c_str());
    std::fprintf(fp, "volume_roi_end_x=%s\n", QString::number(m_roiEnd.x()).toStdString().c_str());
    std::fprintf(fp, "volume_roi_end_y=%s\n", QString::number(m_roiEnd.y()).toStdString().c_str());
    std::fprintf(fp, "volume_roi_end_z=%s\n", QString::number(m_roiEnd.z()).toStdString().c_str());
    fclose(fp);
    return true;
  }
  else
  {
    ERROR_LOG("File creation failed: " << filename.toStdString());
    return false;
  }
  return true;
}

/*!
* @fn       constructFromFile
* @param    const QString filename
* @return   bool success or failure
* @brief    Construct from file.
*/
bool BagInfo::constructFromFile(const QString filename)
{
  TRACE_LOG("");
  QString fileNamePattern = filename;
  fileNamePattern.remove(TIP_BAG_PREFIX);
  fileNamePattern = fileNamePattern.section("/", -1, -1).section(".", 0, 0);
  // Read filename to extract MachineID, DateTime, and BagID
  QStringList list = fileNamePattern.split("_");
  if (list.count() < 4) {
    ERROR_LOG("Bag FileName Pattern is incorrect: " << fileNamePattern.toStdString());
    return false;
  }
  m_machineId = list[0];
  machine_id = m_machineId.toStdString();

  QString dateTime = list[1];
  dateTime.insert(4, "/");
  dateTime.insert(7, "/");
  m_timeStampStr = dateTime;
  m_timeStampStr += " - ";
  dateTime = list[2];
  dateTime.insert(2, ":");
  dateTime.insert(5, ":");
  m_timeStampStr += dateTime;

  // Remove MachineID_Date_Time_ from fileNamePattern and
  // remaining string would be bagID
  fileNamePattern.remove((list[0]+"_"+list[1]+"_"+list[2]+"_"));
  displayable_bag_id = fileNamePattern.toStdString();
  //

  boost::property_tree::ptree pt;
  try
  {
    boost::property_tree::ini_parser::read_ini(filename.toStdString(), pt);
  }
  catch(...)
  {
    return false;
  }

  try
  {
    m_volWidth = (pt.get<std::string>("volume_width")).c_str();
  }
  catch (...)
  {
  }
  try
  {
    m_volHeight = (pt.get<std::string>("volume_height")).c_str();
  }
  catch (...)
  {
  }
  try
  {
    m_sliceCount = (pt.get<std::string>("slice_count")).c_str();
  }
  catch (...)
  {
  }
  try
  {
    m_sliceFormat = (pt.get<std::string>("slice_format")).c_str();
  }
  catch (...)
  {
  }
  try
  {
    m_roiStart.setX(QString::fromStdString(pt.get<std::string>("volume_roi_start_x")).toUInt());
  }
  catch (...)
  {
  }
  try
  {
    m_roiStart.setY(QString::fromStdString(pt.get<std::string>("volume_roi_start_y")).toUInt());
  }
  catch (...)
  {
  }
  try
  {
    m_roiStart.setZ(QString::fromStdString(pt.get<std::string>("volume_roi_start_z")).toUInt());
  }
  catch (...)
  {
  }
  try
  {
    m_roiEnd.setX(QString::fromStdString(pt.get<std::string>("volume_roi_end_x")).toUInt());
  }
  catch (...)
  {
  }
  try
  {
    m_roiEnd.setY(QString::fromStdString(pt.get<std::string>("volume_roi_end_y")).toUInt());
  }
  catch (...)
  {
  }
  try
  {
    m_roiEnd.setZ(QString::fromStdString(pt.get<std::string>("volume_roi_end_z")).toUInt());
  }
  catch (...)
  {
  }
  int diffX = m_roiEnd.x() - m_roiStart.x();
  int diffY = m_roiEnd.y() - m_roiStart.y();
  int diffZ = m_roiEnd.z() - m_roiStart.z();
  if((diffX <= 0) || (diffY <= 0) ||
     (diffZ <= 0))
  {
    return false;
  }
  return true;
}
}  // namespace ws
}  // namespace analogic
