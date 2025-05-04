/*!
* @file     nsseventdata.h
* @author   Agiliad
* @brief    This class holds map of all nss event.
* @date     Apr, 21 2020
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_NSSEVENT_NSSEVENTDATA_H
#define ANALOGIC_WS_COMMON_NSSEVENT_NSSEVENTDATA_H
#include <QObject>
#include <QString>
#include <QMap>
#include <analogic/ws/common.h>
#include <analogic/ws/common/nssevent/nsseventinfo.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   NssEventData
 * \brief   This class holds map of all nss events.
 */
class NssEventData
{  
public:
  /*!
  * @fn       NssEventData
  * @param    None
  * @return   None
  * @brief    Constructor for class NssEventData.
  */
  NssEventData();

  /*!
  * @fn       insertNssEventData
  * @param    None
  * @return   None
  * @brief    Initialises map of nss events.
  */
  void insertNssEventData();

  /*!
  * @fn       getNssEventData
  * @param    None
  * @return   None
  * @brief    Getter for nss events map.
  */
  QMap<QMLEnums::NssEventEnum, NssEventInfo> getNssEventData();

  /*!
  * @fn       getNssEventData
  * @param    QMLEnums::NssEventEnum eventEnum
  * @return   NssEventInfo
  * @brief    returns NssEventInfo for eventEnum.
  */
  NssEventInfo getEventInfoFromEnum(QMLEnums::NssEventEnum eventEnum);

private:
  QMap<QMLEnums::NssEventEnum, NssEventInfo>   m_nssEventInfoMap;
};
}
}
#endif // ANALOGIC_WS_COMMON_NSSEVENT_NSSEVENTDATA_H
