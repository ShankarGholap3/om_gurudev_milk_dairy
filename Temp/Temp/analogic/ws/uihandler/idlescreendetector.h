/*!
* @file     idlescreendetector.h
* @author   Agiliad
* @brief    This file contains functions related to IdleScreenDetector
*           which handles idle screen activity.
* @date     Nov, 17 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#ifndef ANALOGIC_WS_UIHANDLER_IDLESCREENDETECTOR_H_
#define ANALOGIC_WS_UIHANDLER_IDLESCREENDETECTOR_H_

#include <QTimer>
#include <QEvent>
#include <QObject>
#include <QDateTime>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   IdleScreenDetector
 * \brief   This class contains variable and function related to
 *          handling idle screen activity..
 */

class IdleScreenDetector : public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       IdleScreenDetector
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class IdleScreenDetector.
    */
    explicit IdleScreenDetector(QObject *parent = NULL);


    /*!
    * @fn       ~IdleScreenDetector
    * @param    None
    * @return   None
    * @brief    destructor for class IdleScreenDetector.
    */
    virtual ~IdleScreenDetector();

    /*!
    * @fn       setScreenTimeout
    * @param    int - val
    * @return   None
    * @brief    set screen time out
    */
    void setScreenTimeout(int val);

    /*!
    * @fn       updateInactivityTimeoutInterval
    * @param    None
    * @return   None
    * @brief    Update Inactivity Timeout Interval.
    */
    void updateInactivityTimeoutInterval();

    /*!
    * @fn       setIsIdle
    * @param    bool - isIdle
    * @return   None
    * @brief    set status of idle
    */
    void setIsIdle(bool isIdle);

    /*!
    * @fn       getIsIdle
    * @param    None
    * @return   bool - isIdle
    * @brief    set status of idle
    */
    bool getIsIdle();

    Q_PROPERTY(bool isIdle READ getIsIdle WRITE setIsIdle NOTIFY idleChanged)

signals:
    /*!
    * @fn       idleChanged
    * @param    None
    * @return   void
    * @brief    signal idle status is changed
    */
    void idleChanged();

    /*!
    * @fn       mouseClickedEvent
    * @param    None
    * @return   None
    * @brief    signal mouse button clicked.
    */
    void mouseClickedEvent();

public slots:
    /*!
    * @fn       onTimeOut
    * @param    None
    * @return   void
    * @brief    slot calls on timeout of idle screen
    */
    void onTimeOut();


protected:
    /*!
    * @fn       eventFilter
    * @param    QObject * - obj
    * @param    QEvent
    * @return   bool - status for idle screen
    */
    bool eventFilter(QObject *obj, QEvent *event);

private:
    /*!
    * @fn       IdleScreenDetector
    * @param    IdleScreenDetector&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    IdleScreenDetector(const IdleScreenDetector& obj);

    /*!
    * @fn       operator=
    * @param    IdleScreenDetector&
    * @return   IdleScreenDetector&
    * @brief    declaration for private assignment operator.
    */
    IdleScreenDetector& operator= (const IdleScreenDetector& obj);

    bool     m_isIdle;          //!< check system us idle
    int      m_screenTimeout;   //!< Screen timeout
    QTime    m_EventTime;       //!< current time
    QTimer   m_checkInterval;   //!< Timer to check user inactivity
    bool     m_dragging;        //!< hold the timout while dragging
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_IDLESCREENDETECTOR_H_
