#include "analogic/ws/uihandler/supervisorrecentevents.h"

namespace analogic
{
namespace ws
{
SupervisorRecentEvents::SupervisorRecentEvents(QObject *parent): QObject(parent)
{
    m_uuid          = "uuid";
    m_eventName     = "eventName";
    m_createdTime   = "createdTime";
}

/*!
  * @fn       SupervisorRecentEvents
  * @param    SupervisorRecentEvents&
  * @return   SupervisorRecentEvents
  * @brief    copy constructor for SupervisorRecentEvents
  */
SupervisorRecentEvents::SupervisorRecentEvents(const SupervisorRecentEvents &obj)
{
    m_uuid          = obj.m_uuid;
    m_eventName     = obj.m_eventName;
    m_createdTime   = obj.m_createdTime;
}

/*!
  * @fn       operator=
  * @param    SupervisorRecentEvents&
  * @return   SupervisorRecentEvents&
  * @brief    declaration for private assignment operator.
  */
SupervisorRecentEvents &SupervisorRecentEvents::operator=(const SupervisorRecentEvents &obj)
{
    //Checking if the assigned object is same as the current one
    if(this == &obj)
    {
        return *this;
    }
    m_uuid          = obj.m_uuid;
    m_eventName     = obj.m_eventName;
    m_createdTime   = obj.m_createdTime;
    return *this;
}

/*!
* @fn       ~SupervisorRecentEvents
* @param    None
* @return   None
* @brief    Destructor for class SupervisorAgents.
*/
SupervisorRecentEvents::~SupervisorRecentEvents()
{

}

/*!
 * @fn       getRecentEventsUuid
 * @param    None
 * @return   QString
 * @brief    get uuid
 */
QString SupervisorRecentEvents::getRecentEventsUuid() const
{
    return m_uuid;
}

/*!
 * @fn       getRecentEventsEventName
 * @param    None
 * @return   QString
 * @brief    get EventName
 */
QString SupervisorRecentEvents::getRecentEventsEventName() const
{
    return m_eventName;
}

/*!
 * @fn       getRecentEventsCreatedTime
 * @param    None
 * @return   QString
 * @brief    get CreatedTime
 */
QString SupervisorRecentEvents::getRecentEventsCreatedTime() const
{
   return m_createdTime;
}

/*!
 * @fn       setRecentEventsUuid
 * @param    None
 * @return   QString
 * @brief    set uuid
 */
void SupervisorRecentEvents::setRecentEventsUuid(const QString &uuid)
{
    m_uuid = uuid;
}

/*!
 * @fn       setRecentEventsEventName
 * @param    None
 * @return   QString
 * @brief    set EventName
 */
void SupervisorRecentEvents::setRecentEventsEventName(const QString &eventName)
{
    m_eventName = eventName;
}

/*!
 * @fn       setRecentEventsCreatedTime
 * @param    None
 * @return   QString
 * @brief    set CreatedTime
 */
void SupervisorRecentEvents::setRecentEventsCreatedTime(const QString &createdTime)
{
    m_createdTime = createdTime;
}


} // namespace ws
} // namespace analogic

