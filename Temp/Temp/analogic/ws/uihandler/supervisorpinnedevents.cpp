/*!
* @file     supervisorpinnedevents.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling pinned events data.
* @date     Apr, 07 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDebug>
#include "analogic/ws/uihandler/supervisorpinnedevents.h"

namespace analogic
{
namespace ws
{
/*!
  * @fn       SupervisorPinnedEvents
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorPinnedEvents
  */
SupervisorPinnedEvents::SupervisorPinnedEvents(QObject *parent):QObject(parent)
{
    m_uuid          = "uuid";
    m_name          = "name";
    m_createdTime   = "created_time";
}

/*!
  * @fn       SupervisorPinnedEvents
  * @param    const SupervisorPinnedEvents&
  * @return   SupervisorPinnedEvents&
  * @brief    copy constructor for SupervisorPinnedEvents
  */
SupervisorPinnedEvents::SupervisorPinnedEvents(const SupervisorPinnedEvents &obj)
{
    m_uuid            = obj.m_uuid;
    m_name            = obj.m_name;
    m_createdTime     = obj.m_createdTime;
    m_params          = obj.m_params;
    m_sourceAgents    = obj.m_sourceAgents;
}

/*!
  * @fn       operator=
  * @param    SupervisorPinnedEvents&
  * @return   SupervisorPinnedEvents&
  * @brief    declaration for private assignment operator.
  */
SupervisorPinnedEvents &SupervisorPinnedEvents::operator=(const SupervisorPinnedEvents &obj)
{
  //Checking if the assigned object is same as the current one
  if(this == &obj)
  {
    return *this;
  }
    m_uuid            = obj.m_uuid;
    m_name            = obj.m_name;
    m_createdTime     = obj.m_createdTime;
    m_params          = obj.m_params;
    m_sourceAgents    = obj.m_sourceAgents;

    return *this;
}

/*!
* @fn       ~SupervisorPinnedEvents
* @param    None
* @return   None
* @brief    Destructor for class SupervisorPinnedEvents.
*/
SupervisorPinnedEvents::~SupervisorPinnedEvents()
{

}

/*!
  * @fn       getPinnedEventsUuid
  * @param    None
  * @return   QString
  * @brief    get uuid
  */
QString SupervisorPinnedEvents::getPinnedEventsUuid() const
{
    return m_uuid;
}

/*!
  * @fn       setPinnedEventsUuid
  * @param    const QString &uuid
  * @return   None
  * @brief    set uuid
  */
void SupervisorPinnedEvents::setPinnedEventsUuid(const QString &uuid)
{
    m_uuid = uuid;
}

/*!
  * @fn       getPinnedEventsName
  * @param    None
  * @return   QString
  * @brief    get name
  */
QString SupervisorPinnedEvents::getPinnedEventsName() const
{
    return m_name;
}

/*!
  * @fn       setPinnedEventsUuid
  * @param    const QString &name
  * @return   None
  * @brief    set name
  */
void SupervisorPinnedEvents::setPinnedEventsName(const QString &name)
{
    m_name = name;
}

/*!
  * @fn       getPinnedEventsCreatedTime
  * @param    None
  * @return   QString
  * @brief    get created time
  */
QString SupervisorPinnedEvents::getPinnedEventsCreatedTime() const
{
    return m_createdTime;
}

/*!
  * @fn       setPinnedEventsCreatedTime
  * @param    const QString &createdTime
  * @return   None
  * @brief    set created time
  */
void SupervisorPinnedEvents::setPinnedEventsCreatedTime(const QString &createdTime)
{
    m_createdTime = createdTime;
}

/*!
  * @fn       getPinnedEventsParams
  * @param    None
  * @return   QVector<QString>
  * @brief    get params
  */
QVector<QString> SupervisorPinnedEvents::getPinnedEventsParams() const
{
    return m_params;
}

/*!
  * @fn       setPinnedEventsParams
  * @param    const QString &params
  * @return   None
  * @brief    set params
  */
void SupervisorPinnedEvents::setPinnedEventsParams(const QString &params)
{
    m_params.append(params);
}

/*!
  * @fn       getPinnedEventsSourceAgents
  * @param    None
  * @return   SupervisorAgents
  * @brief    get source agent
  */
 SupervisorAgents SupervisorPinnedEvents::getPinnedEventsSourceAgents() const
 {
    return m_sourceAgents;
 }

 /*!
   * @fn       setPinnedEventsSourceAgents
   * @param    const SupervisorAgents &sourceAgents
   * @return   None
   * @brief    set source agents
   */
 void SupervisorPinnedEvents::setPinnedEventsSourceAgents(const SupervisorAgents &sourceAgents)
 {
     m_sourceAgents = sourceAgents;
 }

} // namespace ws
} // namespace analogic

