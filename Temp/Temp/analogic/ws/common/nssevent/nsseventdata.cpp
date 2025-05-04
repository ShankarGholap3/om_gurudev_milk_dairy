/*!
* @file     nsseventdata.h
* @author   Agiliad
* @brief    This class holds map of all nss event.
* @date     Apr, 21 2020
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/nssevent/nsseventdata.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
* @fn       NssEventData
* @param    None
* @return   None
* @brief    Constructor for class NssEventData.
*/
NssEventData::NssEventData()
{

}

/*!
* @fn       insertNssEventData
* @param    None
* @return   None
* @brief    Initialises map of nss events.
*/
void NssEventData::insertNssEventData()
{
  m_nssEventInfoMap.insert(QMLEnums::EVENT_SDSW_LOGIN, {QMLEnums::EVENT_SDSW_LOGIN, "sdsw", "username"});
  m_nssEventInfoMap.insert(QMLEnums::EVENT_FDRS_DOWNLOAD, {QMLEnums::EVENT_FDRS_DOWNLOAD, "fdrs_download", "report name"});
  m_nssEventInfoMap.insert(QMLEnums::EVENT_WORKSTATION_SETTINGS_UPDATED, {QMLEnums::EVENT_WORKSTATION_SETTINGS_UPDATED, "mode", "vs settings"});
}

/*!
* @fn       getNssEventData
* @param    None
* @return   None
* @brief    Getter for nss events map.
*/
QMap<QMLEnums::NssEventEnum, NssEventInfo> NssEventData::getNssEventData()
{
  return m_nssEventInfoMap;
}

/*!
* @fn       getNssEventData
* @param    QMLEnums::NssEventEnum eventEnum
* @return   NssEventInfo
* @brief    returns NssEventInfo for eventEnum.
*/
NssEventInfo NssEventData::getEventInfoFromEnum(QMLEnums::NssEventEnum eventEnum)
{
  return m_nssEventInfoMap.value(eventEnum);
}
}
}
