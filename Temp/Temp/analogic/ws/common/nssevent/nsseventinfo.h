/*!
* @file     nsseventinfo.h
* @author   Agiliad
* @brief    This class holds information nss event.
* @date     Apr, 21 2020
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_NSSEVENT_NSSEVENTINFO_H
#define ANALOGIC_WS_COMMON_NSSEVENT_NSSEVENTINFO_H

#include <QString>
#include <analogic/ws/common.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   NssEventInfo
 * \brief   This class holds information about nss events.
 */
class NssEventInfo
{
public:
  /*!
  * @fn       NssEventInfo
  * @param    None
  * @return   None
  * @brief    Constructor for class NssEventInfo.
  */
  NssEventInfo();

  /*!
  * @fn       NssEventInfo
  * @param    QMLEnums::NssEventEnum eventEnum
  * @param    QString eventCode
  * @param    QString eventDescription
  * @return   None
  * @brief    Constructor for class NssEventInfo.
  */
  NssEventInfo(QMLEnums::NssEventEnum eventEnum, QString eventCode, QString eventDescription);

  /*!
  * @fn       eventEnum
  * @param    None
  * @return   None
  * @brief    Getter Method for m_eventEnum.
  */
  QMLEnums::NssEventEnum eventEnum() const;

  /*!
  * @fn       eventCode
  * @param    None
  * @return   None
  * @brief    Getter Method for m_eventCode.
  */
  QString eventCode() const;

  /*!
  * @fn       eventDescription
  * @param    None
  * @return   None
  * @brief    Getter Method for m_eventDescription.
  */
  QString eventDescription() const;

  /*!
  * @fn       eventDescription
  * @param    None
  * @return   None
  * @brief    Setter Method for m_eventDescription.
  */
  void setEventDescription(QString eventDescription);

private:
  QMLEnums::NssEventEnum m_eventEnum;
  QString m_eventCode;
  QString m_eventDescription;
};
}
}
#endif // ANALOGIC_WS_COMMON_NSSEVENT_NSSEVENTINFO_H
