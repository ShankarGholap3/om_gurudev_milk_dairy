/*!
* @file     supervisorgroupoverview.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling group overview data.
* @date     Apr, 07 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDebug>
#include "analogic/ws/uihandler/supervisorgroupoverview.h"

namespace analogic
{
namespace ws
{
/*!
  * @fn       SupervisorGroupOverview
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorGroupOverview
  */
SupervisorGroupOverview::SupervisorGroupOverview(QObject *parent):QObject(parent)
{
  m_uuid          = "uuid";
  m_name          = "name";
  m_readiness     = "readiness";
}

/*!
  * @fn       SupervisorGroupOverview
  * @param    SupervisorGroupOverview&
  * @return   SupervisorGroupOverview
  * @brief    copy constructor for SupervisorGroupOverview
  */
SupervisorGroupOverview::SupervisorGroupOverview(const SupervisorGroupOverview &obj)
{
    m_uuid             = obj.m_uuid;
    m_name             = obj.m_name;
    m_readiness        = obj.m_readiness;
    m_agents           = obj.m_agents;
    m_pinnedEvents     = obj.m_pinnedEvents;
}

/*!
  * @fn       operator=
  * @param    SupervisorGroupOverview&
  * @return   SupervisorGroupOverview&
  * @brief    declaration for private assignment operator.
  */
SupervisorGroupOverview &SupervisorGroupOverview::operator=(const SupervisorGroupOverview &obj)
{
  //Checking if the assigned object is same as the current one
  if(this == &obj)
  {
    return *this;
  }
    m_uuid             = obj.m_uuid;
    m_name             = obj.m_name;
    m_readiness        = obj.m_readiness;
    m_agents           = obj.m_agents;
    m_pinnedEvents     = obj.m_pinnedEvents;

    return *this;
}

/*!
 * @fn       ~SupervisorGroupOverview
 * @param    None
 * @return   None
 * @brief    Destructor for class SupervisorGroupOverview.
 */
SupervisorGroupOverview::~SupervisorGroupOverview()
{

}

/*!
  * @fn       getGroupOverviewUuid
  * @param    None
  * @return   QString
  * @brief    get uuid
  */
QString SupervisorGroupOverview::getGroupOverviewUuid() const
{
    return m_uuid;
}

/*!
  * @fn       setGroupOverviewUuid
  * @param    const QString &uuid
  * @return   None
  * @brief    set uuid
  */
void SupervisorGroupOverview::setGroupOverviewUuid(const QString &uuid)
{
    m_uuid = uuid;
}

/*!
  * @fn       getGroupOverviewName
  * @param    None
  * @return   QString
  * @brief    get name
  */
QString SupervisorGroupOverview::getGroupOverviewName() const
{
    return m_name;
}

/*!
  * @fn       setGroupOverviewName
  * @param    const QString &name
  * @return   None
  * @brief    set name
  */
void SupervisorGroupOverview::setGroupOverviewName(const QString &name)
{
    m_name = name;
}

/*!
  * @fn       getGroupOverviewReadiness
  * @param    None
  * @return   QString
  * @brief    get readiness
  */
QString SupervisorGroupOverview::getGroupOverviewReadiness() const
{
    return m_readiness;
}

/*!
  * @fn       setGroupOverviewReadiness
  * @param    const QString &readiness
  * @return   None
  * @brief    set readinessme
  */
void SupervisorGroupOverview::setGroupOverviewReadiness(const QString &readiness)
{
    m_readiness = readiness;
}

/*!
  * @fn       getGroupOverviewAgents
  * @param    None
  * @return   QVector<SupervisorAgents>
  * @brief    get agents
  */
QVector<SupervisorAgents> SupervisorGroupOverview::getGroupOverviewAgents() const
{
    return m_agents;
}

/*!
  * @fn       setGroupOverviewAgents
  * @param    const QVector<SupervisorAgents> &agents
  * @return   None
  * @brief    set agents
  */
void SupervisorGroupOverview::setGroupOverviewAgents(const QVector<SupervisorAgents> &agents)
{
    m_agents = agents;
}

/*!
  * @fn       getGroupOverviewPinnedEvents
  * @param    None
  * @return   QVector<SupervisorPinnedEvents>
  * @brief    get pinned events
  */
QVector<SupervisorPinnedEvents> SupervisorGroupOverview::getGroupOverviewPinnedEvents() const
{
    return m_pinnedEvents;
}

/*!
  * @fn       setGroupOverviewPinnedEvents
  * @param    const QVector<SupervisorPinnedEvents> &pinnedEvents
  * @return   None
  * @brief    set pinned events
  */
void SupervisorGroupOverview::setGroupOverviewPinnedEvents(const QVector<SupervisorPinnedEvents> &pinnedEvents)
{
    m_pinnedEvents = pinnedEvents;
}

} // namespace ws
} // namespace analogic

