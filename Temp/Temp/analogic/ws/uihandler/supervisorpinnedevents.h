/*!
* @file     supervisorpinnedevents.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling pinned events data.
* @date     Apr, 07 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORPINNEDEVENTS_H
#define SUPERVISORPINNEDEVENTS_H

#include "analogic/ws/uihandler/supervisoragents.h"

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorPinnedEvents
 * \brief   This class contains parameters related to
 *          handling pinned enevts data.
 */
class SupervisorPinnedEvents: public QObject
{
    Q_OBJECT

 public:
    /*!
      * @fn       SupervisorPinnedEvents
      * @param    None
      * @return   None
      * @brief    constructor for SupervisorPinnedEvents
      */
    explicit SupervisorPinnedEvents(QObject* parent = NULL);

    /*!
      * @fn       SupervisorPinnedEvents
      * @param    const SupervisorPinnedEvents&
      * @return   SupervisorPinnedEvents&
      * @brief    copy constructor for SupervisorPinnedEvents
      */
    SupervisorPinnedEvents(const SupervisorPinnedEvents &obj);

    /*!
      * @fn       operator=
      * @param    SupervisorPinnedEvents&
      * @return   SupervisorPinnedEvents&
      * @brief    declaration for private assignment operator.
      */
    SupervisorPinnedEvents &operator=(const SupervisorPinnedEvents &obj);

    /*!
    * @fn       ~SupervisorPinnedEvents
    * @param    None
    * @return   None
    * @brief    Destructor for class SupervisorPinnedEvents.
    */
    virtual ~SupervisorPinnedEvents();

    /*!
     * @fn       getPinnedEventsUuid
     * @param    None
     * @return   QString
     * @brief    get uuid
     */
     QString getPinnedEventsUuid() const;

     /*!
      * @fn       setPinnedEventsUuid
      * @param    const QString &uuid
      * @return   None
      * @brief    set uuid
      */
     void setPinnedEventsUuid(const QString &uuid);

     /*!
       * @fn       getPinnedEventsName
       * @param    None
       * @return   QString
       * @brief    get name
       */
     QString getPinnedEventsName() const;

     /*!
       * @fn       setPinnedEventsUuid
       * @param    const QString &name
       * @return   None
       * @brief    set name
       */
     void setPinnedEventsName(const QString &name);

     /*!
       * @fn       getPinnedEventsCreatedTime
       * @param    None
       * @return   QString
       * @brief    get created time
       */
     QString getPinnedEventsCreatedTime() const;

     /*!
       * @fn       setPinnedEventsCreatedTime
       * @param    const QString &createdTime
       * @return   None
       * @brief    set created time
       */
     void setPinnedEventsCreatedTime(const QString &createdTime);

     /*!
       * @fn       getPinnedEventsParams
       * @param    None
       * @return   QVector<QString>
       * @brief    get params
       */
     QVector<QString> getPinnedEventsParams() const;

     /*!
       * @fn       setPinnedEventsParams
       * @param    const QString &params
       * @return   None
       * @brief    set params
       */
     void setPinnedEventsParams(const QString &params);

     /*!
       * @fn       getPinnedEventsSourceAgents
       * @param    None
       * @return   SupervisorAgents
       * @brief    get source agent
       */
     SupervisorAgents getPinnedEventsSourceAgents() const;

     /*!
       * @fn       setPinnedEventsSourceAgents
       * @param    const QString &sourceAgents
       * @return   None
       * @brief    set source agents
       */
     void setPinnedEventsSourceAgents(const SupervisorAgents &sourceAgents);

private:
     QString                 m_uuid;                //!< uuid for pinned events
     QString                 m_name;                //!< name for pinned events
     QString                 m_createdTime;         //!< created time for pinned events
     QVector<QString>        m_params;              //!< QVector params for pinned events
     SupervisorAgents        m_sourceAgents;        //!< Supervisor Agents for source agent
};
}
}
#endif // SUPERVISORPINNEDEVENTS_H
