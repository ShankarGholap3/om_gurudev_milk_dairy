/*!
* @file     idlescreendetector.cpp
* @author   Agiliad
* @brief    This file contains functions related to IdleScreenDetector
*           which handles idle screen activity.
* @date     Nov, 17 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/idlescreendetector.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       IdleScreenDetector
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class IdleScreenDetector.
*/
IdleScreenDetector::IdleScreenDetector(QObject*parent) : QObject(parent)
{
  m_EventTime = QTime::currentTime();
  connect(&m_checkInterval, SIGNAL(timeout()), this, SLOT(onTimeOut()));

  updateInactivityTimeoutInterval();

  INFO_LOG("Default screen timeout set to: "<<m_screenTimeout);
}


/*!
* @fn       IdleScreenDetector
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class IdleScreenDetector.
*/
IdleScreenDetector::~IdleScreenDetector()
{
  m_checkInterval.stop();
  m_isIdle = false;
  m_dragging = false;
}

/*!
* @fn       setScreenTimeout
* @param    int - val
* @return   None
* @brief    set screen time out
*/
void IdleScreenDetector::setScreenTimeout(int val)
{
  m_screenTimeout = val;
  INFO_LOG("Setting screen timeout to: "<<val);
}

/*!
* @fn       setIsIdle
* @param    bool - isIdle
* @return   None
* @brief    set status of idle
*/
void IdleScreenDetector::setIsIdle(bool isIdle)
{
    m_isIdle = isIdle ;
}

/*!
* @fn       getIsIdle
* @param    None
* @return   bool - isIdle
* @brief    set status of idle
*/
bool IdleScreenDetector::getIsIdle()
{
    return m_isIdle;
}

/*!
* @fn       updateInactivityTimeoutInterval
* @param    None
* @return   None
* @brief    Update Inactivity Timeout Interval.
*/
void IdleScreenDetector::updateInactivityTimeoutInterval()
{    
    if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::RERUN_EMULATOR)
    {
        m_screenTimeout = -1;
    }
    else
    {
        m_screenTimeout = WorkstationConfig::getInstance()->getinactivitytimeout();
    }
    m_checkInterval.setInterval(m_screenTimeout);

    if (m_screenTimeout >= 10)
    {
        m_checkInterval.setInterval(10000);
    }
    else
    {
        m_checkInterval.setInterval(m_screenTimeout * 500);
    }

    if (-1 != m_screenTimeout)
    {
        m_checkInterval.start();
    }
    else
    {
        m_checkInterval.stop();
    }
    m_isIdle = false;
    m_dragging = false;
}

/*!
* @fn       onTimeOut
* @param    None
* @return   void
* @brief    slot calls on timeout of idle screen
*/
void IdleScreenDetector::onTimeOut()
{
  QTime currTime = QTime::currentTime();
  QMLEnums::AppState current_state = UILayoutManager::getUILayoutManagerInstance()->getAppState();
  QMLEnums::ScreenEnum current_screen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  int elapsedtime = m_EventTime.secsTo(currTime);
  if (elapsedtime > m_screenTimeout)
  {
    if (!m_isIdle
        && (current_state != QMLEnums::ONLINE)
        && (current_state != QMLEnums::SET_TO_OFFLINE)
        && (current_screen != QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN)
        && (current_screen != QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN)
        && (current_screen != QMLEnums::SEARCH_BAG_SCREEN)
        && (current_screen != QMLEnums::TRAINING_BAG_SCREEN)
        && (current_screen != QMLEnums::RELIEVE_USER_LOGIN_PANEL_SCREEN))
    {
      m_isIdle = true;
      m_dragging = false;
      INFO_LOG("Screen idle state changed to: " << m_isIdle);
      emit idleChanged();
    }
    else if (elapsedtime > m_screenTimeout)
    {
        m_EventTime = QTime::currentTime() ;
        m_isIdle = false;
        emit idleChanged();
    }
  }
  else
  {
    if (m_isIdle)
    {
      m_isIdle = false;
      INFO_LOG("Screen idle state changed to: " << m_isIdle);
      emit idleChanged();
    }
  }
}

/*!
* @fn       eventFilter
* @param    QObject * - obj
* @param    QEvent
* @return   bool - status for idle screen
*/
bool IdleScreenDetector::IdleScreenDetector::eventFilter(QObject *obj, QEvent *event)
{
  if (m_dragging == true)
    m_EventTime = QTime::currentTime() ;
  if (event->type() == QEvent::KeyPress||
      event->type() == QEvent::MouseMove ||
      event->type() == QEvent::MouseButtonPress ||
      event->type() == QEvent::MouseButtonRelease ||
      event->type() == QEvent::DragMove ||
      event->type() == QEvent::Move ||
      event->type() == QEvent::TouchUpdate ||
      event->type() == QEvent::TouchBegin ||
      event->type() == QEvent::TouchEnd ||
      event->type() == QEvent::DragLeave ||
      event->type() == QEvent::DragResponse ||
      event->type() == QEvent::Wheel ||
      event->type() == QEvent::Drop ||
      event->type() == QEvent::MouseButtonDblClick ||
      event->type() == QEvent::NonClientAreaMouseButtonPress ||
      event->type() == QEvent::DragEnter)
  {
    if ( event->type() == QEvent::DragEnter && m_dragging == false)
    {
      m_dragging = true;
    }

    if ((event->type() == QEvent::DragLeave || event->type() == QEvent::DragResponse
          || event->type() == QEvent::Drop ) && m_dragging == true)
    {
      m_dragging = false;
    }

    if(event->type() == QEvent::MouseButtonPress)
    {
      emit mouseClickedEvent();
    }

    m_EventTime = QTime::currentTime();
    // standard event processing
    return QObject::eventFilter(obj, event);
  }
  return QObject::eventFilter(obj, event);
}
}  // end of namespace ws
}  // end of namespace analogic
