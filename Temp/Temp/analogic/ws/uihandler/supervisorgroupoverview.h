/*!
* @file     supervisorgroupoverview.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling group overview data.
* @date     Apr, 07 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORGROUPOVERVIEW_H
#define SUPERVISORGROUPOVERVIEW_H

#include "analogic/ws/uihandler/supervisoragents.h"
#include "analogic/ws/uihandler/supervisorpinnedevents.h"


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorGroupOverview
 * \brief   This class contains parameters related to
 *          handling group overview data.
 */
class SupervisorGroupOverview: public QObject
{
    Q_OBJECT

 public:
    /*!
      * @fn       SupervisorGroupOverview
      * @param    None
      * @return   None
      * @brief    constructor for SupervisorGroupOverview
      */
    explicit SupervisorGroupOverview(QObject* parent = NULL);

    /*!
      * @fn       SupervisorGroupOverview
      * @param    SupervisorGroupOverview&
      * @return   SupervisorGroupOverview
      * @brief    copy constructor for SupervisorGroupOverview
      */
    SupervisorGroupOverview(const SupervisorGroupOverview &obj);

    /*!
      * @fn       operator=
      * @param    SupervisorGroupOverview&
      * @return   SupervisorGroupOverview&
      * @brief    declaration for private assignment operator.
      */
    SupervisorGroupOverview &operator=(const SupervisorGroupOverview &obj);

    /*!
    * @fn       ~SupervisorGroupOverview
    * @param    None
    * @return   None
    * @brief    Destructor for class SupervisorGroupOverview.
    */
    virtual ~SupervisorGroupOverview();

    /*!
      * @fn       getGroupOverviewUuid
      * @param    None
      * @return   QString
      * @brief    get uuid
      */
    QString getGroupOverviewUuid() const;

    /*!
      * @fn       setGroupOverviewUuid
      * @param    const QString &uuid
      * @return   None
      * @brief    set uuid
      */
    void setGroupOverviewUuid(const QString &uuid);

    /*!
      * @fn       getGroupOverviewName
      * @param    None
      * @return   QString
      * @brief    get name
      */
    QString getGroupOverviewName() const;

    /*!
      * @fn       setGroupOverviewName
      * @param    const QString &name
      * @return   None
      * @brief    set name
      */
    void setGroupOverviewName(const QString &name);

    /*!
      * @fn       getGroupOverviewReadiness
      * @param    None
      * @return   QString
      * @brief    get readiness
      */
    QString getGroupOverviewReadiness() const;

    /*!
      * @fn       setGroupOverviewReadiness
      * @param    const QString &readiness
      * @return   None
      * @brief    set readinessme
      */
    void setGroupOverviewReadiness(const QString &readiness);

    /*!
      * @fn       getGroupOverviewAgents
      * @param    None
      * @return   QVector<SupervisorAgents>
      * @brief    get agents
      */
    QVector<SupervisorAgents> getGroupOverviewAgents() const;

    /*!
      * @fn       setGroupOverviewAgents
      * @param    const QVector<SupervisorAgents> &agents
      * @return   None
      * @brief    set agents
      */
    void setGroupOverviewAgents(const QVector<SupervisorAgents> &agents);

    /*!
      * @fn       getGroupOverviewPinnedEvents
      * @param    None
      * @return   QVector<SupervisorPinnedEvents>
      * @brief    get pinned events
      */
    QVector<SupervisorPinnedEvents> getGroupOverviewPinnedEvents() const;

    /*!
      * @fn       setGroupOverviewPinnedEvents
      * @param    const QVector<SupervisorPinnedEvents> &pinnedEvents
      * @return   None
      * @brief    set pinned events
      */
    void setGroupOverviewPinnedEvents(const QVector<SupervisorPinnedEvents> &pinnedEvents);

private:
    QString                                m_uuid;              //!< uuid of group overview
    QString                                m_name;              //!< name of group overview
    QString                                m_readiness;         //!< readiness of group overview
    QVector<SupervisorAgents>              m_agents;            //!< agents of group overview
    QVector<SupervisorPinnedEvents>        m_pinnedEvents;      //!< pinned events of group overview
};
}
}
#endif // SUPERVISORGROUPOVERVIEW_H
