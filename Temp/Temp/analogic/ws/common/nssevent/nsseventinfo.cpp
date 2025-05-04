/*!
* @file     nsseventinfo.h
* @author   Agiliad
* @brief    This class holds information nss event.
* @date     Apr, 21 2020
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/nssevent/nsseventinfo.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       NssEventInfo
* @param    None
* @return   None
* @brief    Constructor for class NssEventInfo.
*/
NssEventInfo::NssEventInfo()
{

}

/*!
* @fn       NssEventInfo
* @param    QMLEnums::NssEventEnum eventEnum
* @param    QString eventCode
* @param    QString eventDescription
* @return   None
* @brief    Constructor for class NssEventInfo.
*/
NssEventInfo::NssEventInfo(QMLEnums::NssEventEnum eventEnum, QString eventCode, QString eventDescription):
  m_eventEnum(eventEnum),
  m_eventCode(eventCode),
  m_eventDescription(eventDescription)
{

}

/*!
* @fn       eventEnum
* @param    None
* @return   None
* @brief    Getter Method for m_eventEnum.
*/
QMLEnums::NssEventEnum NssEventInfo::eventEnum() const
{
  return m_eventEnum;
}

/*!
* @fn       eventCode
* @param    None
* @return   None
* @brief    Getter Method for m_eventCode.
*/
QString NssEventInfo::eventCode() const
{
  return m_eventCode;
}

/*!
* @fn       eventDescription
* @param    None
* @return   None
* @brief    Getter Method for m_eventDescription.
*/
QString NssEventInfo::eventDescription() const
{
  return m_eventDescription;
}

/*!
* @fn       eventDescription
* @param    None
* @return   None
* @brief    Setter Method for m_eventDescription.
*/
void NssEventInfo::setEventDescription(QString eventDescription)
{
  m_eventDescription = eventDescription;
}
}
}
