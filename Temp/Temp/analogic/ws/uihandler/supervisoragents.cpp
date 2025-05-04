/*!
* @file     supervisoragents.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling agents data.
* @date     Apr, 07 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDebug>
#include "analogic/ws/uihandler/supervisoragents.h"

namespace analogic
{
namespace ws
{
/*!
  * @fn       SupervisorAgents
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorAgents
  */
SupervisorAgents::SupervisorAgents(QObject *parent):QObject(parent)
{
  m_uuid          = "";
  m_type          = -1;
  m_machine_id    = "";
  m_machine_id_detail = "";
  m_manufacturer  = "";
  m_registered    = "";
  m_readiness     = -1; //false
  m_state         = "";
  m_user          =  "";
  m_location       = "";
  m_machine_location = "";
  m_sublocation    = "";
  m_utilization    = "";
  m_online_time      = "";
  m_trays_screened = -1;
  m_unlink_rate    = -1;
  m_throughput     = "";
  m_session_time   = "";
  m_avg_decision_time = "";
  m_reject_rate       = "";
  m_trays_searched    = -1;
}

/*!
  * @fn       SupervisorAgents
  * @param    None
  * @return   None
  * @brief    copy constructor for SupervisorAgents
  */
SupervisorAgents::SupervisorAgents(const SupervisorAgents &obj)
{
  m_uuid          = obj.m_uuid;
  m_type          = obj.m_type;
  m_machine_id    = obj.m_machine_id;
  m_machine_id_detail = obj.m_machine_id_detail;
  m_manufacturer  = obj.m_manufacturer;
  m_registered    = obj.m_registered;
  m_readiness     = obj.m_readiness;
  m_state         = obj.m_state;
  m_GroupUuid     = obj.m_GroupUuid;
  m_user          = obj.m_user;
  m_location       = obj.m_location;
  m_machine_location = obj.m_machine_location;
  m_sublocation    = obj.m_sublocation;
  m_utilization    = obj.m_utilization;
  m_online_time      = obj.m_online_time;
  m_trays_screened = obj.m_trays_screened;
  m_unlink_rate    = obj.m_unlink_rate;
  m_throughput     = obj.m_throughput;
  m_session_time   = obj.m_session_time;
  m_avg_decision_time = obj.m_avg_decision_time;
  m_reject_rate       = obj.m_reject_rate;
  m_trays_searched    = obj.m_trays_searched;
}

/*!
  * @fn       operator=
  * @param    SupervisorAgents&
  * @return   SupervisorAgents&
  * @brief    declaration for private assignment operator.
  */
SupervisorAgents &SupervisorAgents::operator=(const SupervisorAgents &obj)
{
  //Checking if the assigned object is same as the current one
  if(this == &obj)
  {
    return *this;
  }
  m_uuid          = obj.m_uuid;
  m_type          = obj.m_type;
  m_machine_id    = obj.m_machine_id;
  m_machine_id_detail = obj.m_machine_id_detail;
  m_registered    = obj.m_registered;
  m_readiness     = obj.m_readiness;
  m_state         = obj.m_state;
  m_GroupUuid     = obj.m_GroupUuid;
  m_user          = obj.m_user;
  m_location      = obj.m_location;
  m_machine_location = obj.m_machine_location;
  m_sublocation    = obj.m_sublocation;
  m_utilization    = obj.m_utilization;
  m_online_time      = obj.m_online_time;
  m_trays_screened = obj.m_trays_screened;
  m_unlink_rate    = obj.m_unlink_rate;
  m_throughput     = obj.m_throughput;
  m_session_time   = obj.m_session_time;
  m_avg_decision_time = obj.m_avg_decision_time;
  m_reject_rate       = obj.m_reject_rate;
  m_trays_searched    = obj.m_trays_searched;

  return *this;
}

/*!
 * @fn       ~SupervisorAgents
 * @param    None
 * @return   None
 * @brief    Destructor for class SupervisorAgents.
 */
SupervisorAgents::~SupervisorAgents()
{

}

/*!
 * @fn       getAgentsUuid
 * @param    None
 * @return   QString
 * @brief    get uuid
 */
QString SupervisorAgents::getAgentsUuid() const
{
  return m_uuid;
}

/*!
 * @fn       setAgentsUuid
 * @param    None
 * @return   QString
 * @brief    set uuid
 */
void SupervisorAgents::setAgentsUuid(const QString &uuid)
{
  m_uuid = uuid;
}

/*!
 * @fn       getAgentsType
 * @param    None
 * @return   QString
 * @brief    get type
 */
int SupervisorAgents::getAgentsType() const
{
  return m_type;
}

/*!
 * @fn       setAgentsType
 * @param    const QString &type
 * @return   None
 * @brief    set type
 */
void SupervisorAgents::setAgentsType(const int &type)
{
  m_type = type;
}

/*!
 * @fn       setDetailMachine_id
 * @param    None
 * @return   QString
 * @brief    get m_machine_id_detail
 */
QString SupervisorAgents::getDetailMachine_id() const
{
  return m_machine_id_detail;
}

/*!
 * @fn       setDetailMachine_id
 * @param    const QString &machine_id
 * @return   None
 * @brief    set m_machine_id_detail
 */
void SupervisorAgents::setDetailMachine_id(const QString &machine_id_detail)
{
  m_machine_id_detail = machine_id_detail;
}

/*!
   * @fn       getDetailLocation
   * @param    None
   * @return   QString
   * @brief    get machine_location
   */
QString SupervisorAgents::getDetailLocation() const
{
  return m_machine_location;
}

/*!
   * @fn       setDetailMachine_id
   * @param    const QString &machine_location
   * @return   None
   * @brief    set machine_location
   */
void SupervisorAgents::setDetailLocation(const QString &machine_location)
{
  m_machine_location = machine_location;
}

/*!
 * @fn       getAgentsMachine_id
 * @param    None
 * @return   QString
 * @brief    get machine_id
 */
QString SupervisorAgents::getAgentsMachine_id() const
{
  return m_machine_id;
}

/*!
 * @fn       setAgentsMachine_id
 * @param    const QString &machine_id
 * @return   None
 * @brief    set machine_id
 */
void SupervisorAgents::setAgentsMachine_id(const QString &machine_id)
{
  m_machine_id = machine_id;
}

/*!
 * @fn       getAgentsManufacturer
 * @param    None
 * @return   QString
 * @brief    get manufacturer
 */
QString SupervisorAgents::getAgentsManufacturer() const
{
  return m_manufacturer;
}

/*!
 * @fn       setAgentsManufacturer
 * @param    const QString &manufacturer
 * @return   None
 * @brief    set manufacturer
 */
void SupervisorAgents::setAgentsManufacturer(const QString &manufacturer)
{
  m_manufacturer = manufacturer;
}

/*!
 * @fn       getAgentsRegistered
 * @param    None
 * @return   bool
 * @brief    get registered
 */
bool SupervisorAgents::getAgentsRegistered() const
{
  return m_registered;
}

/*!
 * @fn       setAgentsRegistered
 * @param    const QString &registered
 * @return   None
 * @brief    set registered
 */
void SupervisorAgents::setAgentsRegistered(const bool registered)
{
  m_registered = registered;
}

/*!
 * @fn       getAgentsReadiness
 * @param    None
 * @return   QString
 * @brief    get readiness
 */
int SupervisorAgents::getAgentsReadiness() const
{
  return m_readiness;
}

/*!
 * @fn       setAgentsReadiness
 * @param    const QString &readiness
 * @return   None
 * @brief    set readiness
 */
void SupervisorAgents::setAgentsReadiness(const int &readiness)
{
  m_readiness = readiness;
}

/*!
 * @fn       getAgentsState
 * @param    None
 * @return   QString
 * @brief    set state
 */
QString SupervisorAgents::getAgentsState() const
{
  return m_state;
}

/*!
 * @fn       setAgentsState
 * @param    const QString &state
 * @return   None
 * @brief    set state
 */
void SupervisorAgents::setAgentsState(const QString &state)
{
  m_state = state;
}

/*!
 * @fn       getAgentsGroupUuid
 * @param    None
 * @return   QVector<QString>
 * @brief    get Group Uuid
 */
QVector<QString> SupervisorAgents::getAgentsGroupUuid() const
{
  return m_GroupUuid;
}

/*!
 * @fn       setAgentsGroupUuid
 * @param    const QString &GroupUuid
 * @return   None
 * @brief    set Group Uuid
 */
void SupervisorAgents::setAgentsGroupUuid(const QVector<QString> &GroupUuid)
{
  m_GroupUuid.append(GroupUuid);
}


/*!
 * @fn       getAgentsUser
 * @param    None
 * @return   QString
 * @brief    get user
 */
QString SupervisorAgents::getAgentsUser() const
{
  return m_user;
}

/*!
 * @fn       setUser
 * @param    const QString &user
 * @return   None
 * @brief    set user
 */
void SupervisorAgents::setAgentsUser(const QString &user)
{
  m_user = user;
}

/*!
 * @fn       getAgentsLocation
 * @param    None
 * @return   QString
 * @brief    get location
 */
QString SupervisorAgents::getAgentsLocation() const
{
  return m_location;
}

/*!
 * @fn       setLocation
 * @param    const QString &location
 * @return   None
 * @brief    set location
 */
void SupervisorAgents::setAgentsLocation(const QString &location)
{
  m_location = location;
}
/*!
   * @fn       getsublocation
   * @param    None
   * @return   QString
   * @brief    get sublocation
   */
QString SupervisorAgents::getAgentsSublocation() const
{
  return m_sublocation;
}

/*!
   * @fn       setsublocation
   * @param    const QString &sublocation
   * @return   None
   * @brief    set sublocation
   */
void SupervisorAgents::setAgentsSublocation(const QString &sublocation)
{
  m_sublocation = sublocation;
}
/*!
   * @fn       getonline_bags_scanned
   * @param    None
   * @return   QString
   * @brief    get online_bags_scanned
   */
QString SupervisorAgents::getAgentsOnlineBagScanned() const
{
  return m_online_bags_scanned;
}

/*!
   * @fn       setOnlineBagScanned
   * @param    const QString &OnlineBagScanned
   * @return   None
   * @brief    set OnlineBagScanned
   */
void SupervisorAgents::setAgentsOnlineBagScanned(const QString &OnlineBagScanned)
{
  m_online_bags_scanned = OnlineBagScanned;
}
/*!
 * @fn       getUtilization
 * @param    None
 * @return   QString
 * @brief    get utilization
 */
QString SupervisorAgents::getUtilization() const
{
  return m_utilization;
}

/*!
 * @fn       setUtilization
 * @param    const QString &utilization
 * @return   None
 * @brief    set utilization
 */
void SupervisorAgents::setUtilization(const QString &utilization)
{
  m_utilization = utilization;
}

/*!
 * @fn       getScan_time
 * @param    None
 * @return   QString
 * @brief    get scan_time
 */
QString SupervisorAgents::getOnline_time() const
{
  return m_online_time;
}

/*!
 * @fn       setScan_time
 * @param    const QString &scan_time
 * @return   None
 * @brief    set scan_time
 */
void SupervisorAgents::setOnline_time(const QString &online_time)
{
  m_online_time= online_time;
}

/*!
 * @fn       getTrays_screened
 * @param    None
 * @return   int
 * @brief    get trays_screened
 */
int SupervisorAgents::getTrays_screened() const
{
  return m_trays_screened;
}

/*!
 * @fn       setTrays_screened
 * @param    const int &trays_screened
 * @return   None
 * @brief    set trays_screened
 */
void SupervisorAgents::setTrays_screened(const int &trays_screened)
{
  m_trays_screened= trays_screened;
}

/*!
 * @fn       getUnlink_rate
 * @param    None
 * @return   float
 * @brief    get unlink_rate
 */
float SupervisorAgents::getUnlink_rate() const
{
  return m_unlink_rate;
}

/*!
 * @fn       setUnlink_rate
 * @param    const float &unlink_rate
 * @return   None
 * @brief    set unlink_rate
 */
void SupervisorAgents::setUnlink_rate(const float &unlink_rate)
{
  m_unlink_rate= unlink_rate;
}

/*!
 * @fn       getThroughput
 * @param    None
 * @return   QString
 * @brief    get throughput
 */
QString SupervisorAgents::getThroughput() const
{
  return m_throughput;
}

/*!
 * @fn       setThroughput
 * @param    const QString &throughput
 * @return   None
 * @brief    set throughput
 */
void SupervisorAgents::setThroughput(const QString &throughput)
{
  m_throughput= throughput;
}

/*!
 * @fn       getSession_time
 * @param    None
 * @return   QString
 * @brief    get session_time
 */
QString SupervisorAgents::getSession_time() const
{
  return m_session_time;
}

/*!
 * @fn       setSession_time
 * @param    const QString &session_time
 * @return   None
 * @brief    set session_time
 */
void SupervisorAgents::setSession_time(const QString &session_time)
{
  m_session_time= session_time;
}

/*!
 * @fn       getAvg_decision_time
 * @param    None
 * @return   QString
 * @brief    get avg_decision_time
 */
QString SupervisorAgents::getAvg_decision_time() const
{
  return m_avg_decision_time;
}

/*!
 * @fn       setAvg_decision_time
 * @param    const QString &avg_decision_time
 * @return   None
 * @brief    set avg_decision_time
 */
void SupervisorAgents::setAvg_decision_time(const QString &avg_decision_time)
{
  m_avg_decision_time= avg_decision_time;
}

/*!
 * @fn       getReject_rate
 * @param    None
 * @return   QString
 * @brief    get reject_rate
 */
QString SupervisorAgents::getReject_rate() const
{
  return m_reject_rate;
}

/*!
 * @fn       setReject_rate
 * @param    const QString &reject_rate
 * @return   None
 * @brief    set reject_rate
 */
void SupervisorAgents::setReject_rate(const QString &reject_rate)
{
  m_reject_rate= reject_rate;
}

/*!
 * @fn       getTrays_searched
 * @param    None
 * @return   int
 * @brief    get trays_searched
 */
int SupervisorAgents::getTrays_searched() const
{
  return m_trays_searched;
}

/*!
 * @fn       setTrays_searched
 * @param    const int &trays_searched
 * @return   None
 * @brief    set  trays_searched
 */
void SupervisorAgents::setTrays_searched(const int & trays_searched)
{
  m_trays_searched=  trays_searched;
}


} // namespace ws
} // namespace analogic


