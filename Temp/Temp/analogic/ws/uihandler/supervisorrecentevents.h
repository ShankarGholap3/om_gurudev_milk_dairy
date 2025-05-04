#ifndef SUPERVISORRECENTEVENTS_H
#define SUPERVISORRECENTEVENTS_H

#include <QObject>
#include <QTimer>
#include <QStringList>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class SupervisorRecentEvents : public QObject
{
    Q_OBJECT

public:

    /*!
      * @fn       SupervisorRecentEvents
      * @param    None
      * @return   None
      * @brief    constructor for SupervisorRecentEvents
      */
    explicit SupervisorRecentEvents(QObject* parent = NULL);

    /*!
      * @fn       SupervisorRecentEvents
      * @param    SupervisorRecentEvents&
      * @return   SupervisorRecentEvents
      * @brief    copy constructor for SupervisorRecentEvents
      */
    SupervisorRecentEvents(const SupervisorRecentEvents &obj);

    /*!
      * @fn       operator=
      * @param    SupervisorRecentEvents&
      * @return   SupervisorRecentEvents&
      * @brief    declaration for private assignment operator.
      */
    SupervisorRecentEvents &operator=(const SupervisorRecentEvents &obj);

    /*!
    * @fn       ~SupervisorRecentEvents
    * @param    None
    * @return   None
    * @brief    Destructor for class SupervisorAgents.
    */
    virtual ~SupervisorRecentEvents();

    /*!
     * @fn       getRecentEventsUuid
     * @param    None
     * @return   QString
     * @brief    get uuid
     */
    QString getRecentEventsUuid() const;

    /*!
     * @fn       setRecentEventsUuid
     * @param    None
     * @return   QString
     * @brief    set uuid
     */
    void setRecentEventsUuid(const QString &uuid);

    /*!
     * @fn       getRecentEventsEventName
     * @param    None
     * @return   QString
     * @brief    get EventName
     */
    QString getRecentEventsEventName() const;

    /*!
     * @fn       setRecentEventsEventName
     * @param    None
     * @return   QString
     * @brief    set EventName
     */
    void setRecentEventsEventName(const QString &eventName);

    /*!
     * @fn       getRecentEventsCreatedTime
     * @param    None
     * @return   QString
     * @brief    get CreatedTime
     */
    QString getRecentEventsCreatedTime() const;

    /*!
     * @fn       setRecentEventsCreatedTime
     * @param    None
     * @return   QString
     * @brief    set CreatedTime
     */
    void setRecentEventsCreatedTime(const QString &createdTime);
private:
    QString                 m_uuid;                     //!< uuid of recentEvents
    QString                 m_eventName;                //!< eventName of recentEvents
    QString                 m_createdTime;              //!< createdTime of recentEvents

};
}
}
#endif // SUPERVISORRECENTEVENTS_H
