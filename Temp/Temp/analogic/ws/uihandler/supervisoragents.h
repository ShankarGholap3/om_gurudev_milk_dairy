/*!
* @file     supervisoragents.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling agents data.
* @date     Apr, 07 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORAGENTS_H
#define SUPERVISORAGENTS_H

#include <QDebug>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorAgents
 * \brief   This class contains parameters related to
 *          handling agents data.
 */
class SupervisorAgents: public QObject
{
  Q_OBJECT

public:
  /*!
      * @fn       SupervisorAgents
      * @param    None
      * @return   None
      * @brief    constructor for SupervisorAgents
      */
  explicit SupervisorAgents(QObject* parent = NULL);

  /*!
      * @fn       SupervisorAgents
      * @param    SupervisorAgents&
      * @return   SupervisorAgents
      * @brief    copy constructor for SupervisorAgents
      */
  SupervisorAgents(const SupervisorAgents &obj);

  /*!
      * @fn       operator=
      * @param    SupervisorAgents&
      * @return   SupervisorAgents&
      * @brief    declaration for private assignment operator.
      */
  SupervisorAgents &operator=(const SupervisorAgents &obj);

  /*!
    * @fn       ~SupervisorAgents
    * @param    None
    * @return   None
    * @brief    Destructor for class SupervisorAgents.
    */
  virtual ~SupervisorAgents();

  /*!
     * @fn       getAgentsUuid
     * @param    None
     * @return   QString
     * @brief    get uuid
     */
  QString getAgentsUuid() const;

  /*!
     * @fn       setAgentsUuid
     * @param    None
     * @return   QString
     * @brief    set uuid
     */
  void setAgentsUuid(const QString &uuid);

  /*!
     * @fn       getAgentsType
     * @param    None
     * @return   QString
     * @brief    get type
     */
  int getAgentsType() const;

  /*!
     * @fn       setAgentsType
     * @param    const QString &type
     * @return   None
     * @brief    set type
     */
  void setAgentsType(const int &type);

  /*!
     * @fn       getAgentsMachine_id
     * @param    None
     * @return   QString
     * @brief    get machine_id
     */
  QString getAgentsMachine_id() const;

  /*!
     * @fn       setAgentsMachine_id
     * @param    const QString &machine_id
     * @return   None
     * @brief    set machine_id
     */
  void setAgentsMachine_id(const QString &machine_id);

  /*!
     * @fn       getDetailMachine_id
     * @param    None
     * @return   QString
     * @brief    get machine_id_detail
     */
  QString getDetailMachine_id() const;

  /*!
     * @fn       setDetailMachine_id
     * @param    const QString &machine_id_detail
     * @return   None
     * @brief    set machine_id_detail
     */
  void setDetailMachine_id(const QString &machine_location);

  /*!
     * @fn       getDetailLocation
     * @param    None
     * @return   QString
     * @brief    get detail location
     */
  QString getDetailLocation() const;

  /*!
     * @fn       setDetailLocation
     * @param    const QString &location
     * @return   None
     * @brief    set detail location
     */
  void setDetailLocation(const QString &machine_location);
  /*!
     * @fn       getAgentsManufacturer
     * @param    None
     * @return   QString
     * @brief    get manufacturer
     */
  QString getAgentsManufacturer() const;

  /*!
     * @fn       setAgentsManufacturer
     * @param    const QString &manufacturer
     * @return   None
     * @brief    set manufacturer
     */
  void setAgentsManufacturer(const QString &manufacturer);

  /*!
     * @fn       getAgentsRegistered
     * @param    None
     * @return   bool
     * @brief    get registered
     */
  bool getAgentsRegistered() const;

  /*!
     * @fn       setAgentsRegistered
     * @param    const QString &registered
     * @return   None
     * @brief    set registered
     */
  void setAgentsRegistered(const bool registered);

  /*!
     * @fn       getAgentsReadiness
     * @param    None
     * @return   QString
     * @brief    get readiness
     */
  int getAgentsReadiness() const;

  /*!
     * @fn       setAgentsReadiness
     * @param    const QString &readiness
     * @return   None
     * @brief    set readiness
     */
  void setAgentsReadiness(const int &readiness);

  /*!
     * @fn       getAgentsState
     * @param    None
     * @return   QString
     * @brief    get state
     */
  QString getAgentsState() const;

  /*!
     * @fn       setAgentsState
     * @param    const QString &state
     * @return   None
     * @brief    set state
     */
  void setAgentsState(const QString &state);

  /*!
     * @fn       getAgentsGroupUuid
     * @param    None
     * @return   QVector<QString>
     * @brief    get Group Uuid
     */
  QVector<QString> getAgentsGroupUuid() const;

  /*!
     * @fn       setAgentsGroupUuid
     * @param    const QString &GroupUuid
     * @return   None
     * @brief    set Group Uuid
     */
  void setAgentsGroupUuid(const QVector<QString> &GroupUuid);

  /*!
     * @fn       getUser
     * @param    None
     * @return   QString
     * @brief    get user
     */
  QString getAgentsUser() const;

  /*!
     * @fn       setUser
     * @param    const QString &user
     * @return   None
     * @brief    set user
     */
  void setAgentsUser(const QString &user);

  /*!
     * @fn       getLocation
     * @param    None
     * @return   QString
     * @brief    get location
     */
  QString getAgentsLocation() const;

  /*!
     * @fn       setLocation
     * @param    const QString &location
     * @return   None
     * @brief    set location
     */
  void setAgentsLocation(const QString &location);

  /*!
     * @fn       getsublocation
     * @param    None
     * @return   QString
     * @brief    get sublocation
     */
  QString getAgentsSublocation() const;

  /*!
     * @fn       setsublocation
     * @param    const QString &sublocation
     * @return   None
     * @brief    set sublocation
     */
  void setAgentsSublocation(const QString &sublocation);

  /*!
     * @fn       getonline_bags_scanned
     * @param    None
     * @return   QString
     * @brief    get online_bags_scanned
     */
  QString getAgentsOnlineBagScanned() const;

  /*!
     * @fn       setOnlineBagScanned
     * @param    const QString &OnlineBagScanned
     * @return   None
     * @brief    set OnlineBagScanned
     */
  void setAgentsOnlineBagScanned(const QString &OnlineBagScanned);

  /*!
     * @fn       getUtilization
     * @param    None
     * @return   QString
     * @brief    get utilization
     */
  QString getUtilization() const;

  /*!
     * @fn       setUtilization
     * @param    const QString &utilization
     * @return   None
     * @brief    set utilization
     */
  void setUtilization(const QString &utilization);

  /*!
     * @fn       getOnline_time
     * @param    None
     * @return   QString
     * @brief    get Online_time
     */
  QString getOnline_time() const;

  /*!
     * @fn       setOnline_time
     * @param    const QString &Online_time
     * @return   None
     * @brief    set Online_time
     */
  void setOnline_time(const QString &Online_time);

  /*!
     * @fn       getTrays_screened
     * @param    None
     * @return   int
     * @brief    get trays_screened
     */
  int getTrays_screened() const;

  /*!
     * @fn       setTrays_screened
     * @param    const int &trays_screened
     * @return   None
     * @brief    set trays_screened
     */
  void setTrays_screened(const int &trays_screened);

  /*!
     * @fn       getUnlink_rate
     * @param    None
     * @return   float
     * @brief    get unlink_rate
     */
  float getUnlink_rate() const;

  /*!
     * @fn       setUnlink_rate
     * @param    const float &unlink_rate
     * @return   None
     * @brief    set unlink_rate
     */
  void setUnlink_rate(const float &unlink_rate);

  /*!
     * @fn       getThroughput
     * @param    None
     * @return   QString
     * @brief    get throughput
     */
  QString getThroughput() const;

  /*!
     * @fn       setThroughput
     * @param    const QString &throughput
     * @return   None
     * @brief    set throughput
     */
  void setThroughput(const QString &throughput);

  /*!
     * @fn       getSession_time
     * @param    None
     * @return   QString
     * @brief    get session_time
     */
  QString getSession_time() const;

  /*!
     * @fn       setSession_time
     * @param    const QString &session_time
     * @return   None
     * @brief    set session_time
     */
  void setSession_time(const QString &session_time);

  /*!
     * @fn       getAvg_decision_time
     * @param    None
     * @return   QString
     * @brief    get avg_decision_time
     */
  QString getAvg_decision_time() const;

  /*!
     * @fn       setAvg_decision_time
     * @param    const QString &avg_decision_time
     * @return   None
     * @brief    set avg_decision_time
     */
  void setAvg_decision_time(const QString &avg_decision_time);

  /*!
     * @fn       getReject_rate
     * @param    None
     * @return   QString
     * @brief    get reject_rate
     */
  QString getReject_rate() const;

  /*!
     * @fn       setReject_rate
     * @param    const QString &reject_rate
     * @return   None
     * @brief    set reject_rate
     */
  void setReject_rate(const QString &reject_rate);

  /*!
     * @fn       getTrays_searched
     * @param    None
     * @return   int
     * @brief    get trays_searched
     */
  int getTrays_searched() const;

  /*!
     * @fn       setTrays_searched
     * @param    const int &trays_searched
     * @return   None
     * @brief    set  trays_searched
     */
  void setTrays_searched(const int & trays_searched);

  //  /*!
  //     * @fn       setUser
  //     * @param    const QString &user
  //     * @return   None
  //     * @brief    set  user
  //     */
  //  void setUser(const QString & user);

  //  /*!
  //     * @fn       getUser
  //     * @param    None
  //     * @return   QString
  //     * @brief    get user
  //     */
  //  QString getUser() const;

private:
  QString                 m_uuid;                     //!< uuid of agents
  int                     m_type;                     //!< type of agents
  QString                 m_machine_id;               //!< machine id of agents
  QString                 m_machine_id_detail;         //!< machine id from agent detail
  QString                 m_manufacturer;             //!< manufacturer of agents
  bool                    m_registered;               //!< registered of agents
  int                     m_readiness;                //!< readiness of agents
  QString                 m_state;                    //!< state of agents
  QVector<QString>        m_GroupUuid;                //!< Uuid for groups of agents
  QString                 m_user;                     //!< user of agents
  QString                 m_location;                 //!< location of agents
  QString                 m_machine_location;         //!< machine location of agent detail
  QString                 m_utilization;              //!< utilization of agents
  QString                 m_online_time;              //!< scan_time of agents
  int                     m_trays_screened;           //!< trays_screened of agents
  float                   m_unlink_rate;              //!< unlink_rate of agents
  QString                 m_throughput;               //!< throughput of agents
  QString                 m_session_time;             //!< session_time of agents
  QString                 m_avg_decision_time;        //!< avg_decision_time of agents
  QString                 m_reject_rate;              //!< reject_rate of agents
  int                     m_trays_searched;           //!< trays_searched of agents
  QString                 m_sublocation;              //!< sublocation of agents
  QString                 m_online_bags_scanned;      //!< Not in use now
};
}
}
#endif // SUPERVISORAGENTS_H
