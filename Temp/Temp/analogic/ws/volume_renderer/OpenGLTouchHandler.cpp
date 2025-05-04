//------------------------------------------------------------------------------
// File: OpenGLTouchHandler.cpp
// Description: Implementation of touch Pich-Zoom and Pich-Move handler that
//    works with the OpenGlVolumeWidget.
// Copyright 2018 Analogic Corp.
//------------------------------------------------------------------------------
#include "OpenGLVolumeWidget.h"
#include "OpenGLTouchHandler.h"


const int sameLocationTouchFilter = 0;



//------------------------------------------------------------------------------
OpenGLTouchHandler::OpenGLTouchHandler(OpenGLVolumeWidget* parent):
 m_currentKeyEvent(TPR_Ignore),
 m_twoFingerTouchActive(false),
 m_volumeWidget(parent),
 m_touchPinchCount(0),
 m_touchSpreadCount(0),
 m_touchMoveCount(0),
 m_pinchThreshold(DEFAULT_PINCH_THRESHOLD),
 m_spreadThreshold(DEFAULT_SPREAD_THRESHOLD),
 m_moveThreshold(DEFAULT_MOVE_THRESHOLD),
 m_startPos0(0, 0),
 m_startPos1(0, 0),
 m_currentPos0(0, 0),
 m_currentPos1(0, 0)
{
}




//------------------------------------------------------------------------------
OpenGLTouchHandler::~OpenGLTouchHandler()
{
}


//------------------------------------------------------------------------------
void OpenGLTouchHandler::adjustThreshod(int pinchThreshold,
                    int spreadThreshold,
                    int moveThreshold)
{
  m_pinchThreshold  = pinchThreshold;
  m_spreadThreshold = spreadThreshold;
  m_moveThreshold   = moveThreshold;
}



//------------------------------------------------------------------------------
bool OpenGLTouchHandler::isTwoFingerTouchActive()
{
  return m_twoFingerTouchActive;
}

//------------------------------------------------------------------------------
bool OpenGLTouchHandler::setTwoFingerTouchActive(bool setVal)
{
  bool retVal = false;
  if ((!setVal) && (m_twoFingerTouchActive))
  {
    // qDebug() << "Changeover from TWO finngers to one!!!!";
    retVal = true;
  }
  m_twoFingerTouchActive = setVal;
  return retVal;
}


//------------------------------------------------------------------------------
void OpenGLTouchHandler::setCurrentPos(QPoint pb0, QPoint pb1)
{
  QPointF fpB0 = QPointF(pb0.rx(), pb0.ry());
  QPointF fpB1 = QPointF(pb1.rx(), pb1.ry());

  QPointF fpC0 = QPointF(m_currentPos0.rx(), m_currentPos0.ry());
  QPointF fpC1 = QPointF(m_currentPos1.rx(), m_currentPos1.ry());

  float d0 =  QLineF(fpC0, fpB0).length();
  float d1 =  QLineF(fpC1, fpB1).length();

  int di0 = abs(d0);
  int di1 = abs(d1);

  if ( (di0 > sameLocationTouchFilter) ||
       (di1 > sameLocationTouchFilter)
       )
  {
    m_currentPos0 = pb0;
    m_currentPos1 = pb1;
  }
  else
  {
    m_startPos0 = pb0;
    m_startPos1 = pb1;
  }
}

//------------------------------------------------------------------------------
void OpenGLTouchHandler::setStartPos(QPoint ps0, QPoint ps1)
{
  m_startPos0 = ps0;
  m_startPos1 = ps1;
}

//------------------------------------------------------------------------------
TouchProcResulst_E OpenGLTouchHandler::getCurrentKeyEvent()
{
  return m_currentKeyEvent;
}


//------------------------------------------------------------------------------
void OpenGLTouchHandler::resetCurrentKeyEvent()
{
  m_currentKeyEvent = TPR_Ignore;
}

//------------------------------------------------------------------------------
TouchProcResulst_E OpenGLTouchHandler::processOneFingerTouch(QPoint& currentPos)
{
    currentPos = m_currentPos0;
    // ARO-NOTE:  Not sure we need this method yet.
    // It's a place hoilder.
    return TPR_Rotate;
}

//------------------------------------------------------------------------------
TouchProcResulst_E OpenGLTouchHandler::processTwoFingerTouch(double& deltaX, double& deltaY)
{
  double LS = QLineF(m_startPos0, m_startPos1).length();
  double LC = QLineF(m_currentPos0, m_currentPos1).length();
  QVector2D tau0( (m_currentPos0.rx() - m_startPos0.rx()),
                  (m_currentPos0.ry() - m_startPos0.ry()) );

  QVector2D tau1( (m_currentPos1.rx() - m_startPos1.rx()),
                  (m_currentPos1.ry() - m_startPos1.ry()) );
  tau0.normalize();
  tau1.normalize();
  if ( (tau0.length() < 0.01) ||
       (tau1.length() < 0.01))
  {
   // qDebug() << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK Bad Vector Normalize";
   // qDebug() << "TAU0[x,y] = (" << tau0.x() <<"," << tau0.y() <<
   //             ") TAU1[x,y]= (" << tau1.x() <<"," << tau1.y() << ")";
     return TPR_Ignore;
  }

  double angle_between_strokes = (180.0/M_PI)*acos(QVector2D::dotProduct(tau0, tau1));

  // qDebug() << "Angle Between strokes=" << angle_between_strokes;

  if ( ((LS - LC) > TOUCH_PROCESS_THRESHOLD ) &&
       (angle_between_strokes > PINCH_STRETCH_ANGLE_THRESHOLLD )
       )
  {
     deltaX = 0;
     deltaY = 0;
     return TPR_Pinch;
  }
  if (   ((LC - LS ) > TOUCH_PROCESS_THRESHOLD ) &&
         (angle_between_strokes > PINCH_STRETCH_ANGLE_THRESHOLLD)
      )
  {
    deltaX = 0;
    deltaY = 0;
    return TPR_Spread;
  }
  if (angle_between_strokes > MAX_ANGLE_BETWEEN_MOVE_STROKE)
  {
    deltaX = 0;
    deltaY = 0;
    return TPR_Ignore;
  }

  QVector2D  mov0( (m_currentPos0.rx() - m_startPos0.rx()),
                   (m_currentPos0.ry() - m_startPos0.ry()));


  QVector2D  mov1( (m_currentPos1.rx() - m_startPos1.rx()),
                   (m_currentPos1.ry() - m_startPos1.ry()));

  deltaX =      TOUCH_SCALE_MOVE*( mov0.x() + mov1.x() )/2.0;
  deltaY = -1.0*TOUCH_SCALE_MOVE*( mov0.y() + mov1.y() )/2.0;

  // qDebug() << "MOVE (x,y)" << deltaX << "," << deltaY;

  return TPR_Move;
}


//------------------------------------------------------------------------------
void OpenGLTouchHandler::performTouchPinch()
{
  QWheelEvent  wheelOutEvent(QPoint(0, 0),
                                 -1*DEF_ZOOM_WHEEL_DELTA,
                                 Qt::NoButton,
                                 Qt::NoModifier);
  m_volumeWidget->performWheelEvent(&wheelOutEvent);
  m_currentKeyEvent = TPR_Pinch;
}


//------------------------------------------------------------------------------
void OpenGLTouchHandler::performTouchSpread()
{
  QWheelEvent  wheelInEvent(QPoint(0, 0),
                                 DEF_ZOOM_WHEEL_DELTA,
                                 Qt::NoButton,
                                 Qt::NoModifier);
  m_volumeWidget->performWheelEvent(&wheelInEvent);
  m_currentKeyEvent = TPR_Spread;
}

//------------------------------------------------------------------------------
void OpenGLTouchHandler::performTouchMove(int panX,
                              int panY)
{
  m_volumeWidget->performPan(panX, panY);
  m_currentKeyEvent = TPR_Move;
}

//------------------------------------------------------------------------------
void OpenGLTouchHandler::debugShowStatus()
{
  qDebug() << "Start_P0("
           << m_startPos0.rx() << ", "
           << m_startPos0.ry()
           << ") Start_P1("
           << m_startPos1.rx() << ", "
           << m_startPos1.ry()
           << ") - Current_P0("
           << m_currentPos0.rx() << ", "
           << m_currentPos0.ry()
           << ") Current_P1("
           << m_currentPos1.rx() << ", "
           << m_currentPos1.ry() << " )";
}


//------------------------------------------------------------------------------
void OpenGLTouchHandler::startEventTimer(QString messageStart)
{
  m_eventTimer = QTime::currentTime();
  QString message = "Begin Event Timer: " + messageStart;
  qDebug() << message;
  // LOG(INFO) << message.toLocal8Bit().data();
}


//------------------------------------------------------------------------------
void OpenGLTouchHandler::stopEventTimer(QString messageStop)
{
  int msEvent = m_eventTimer.elapsed();
  QString message = "Stop Event Timer: " + messageStop + " - Elapsed (ms) , " + QString::number(msEvent);
  qDebug() << message;
 // LOG(INFO) << message.toLocal8Bit().data();
}
