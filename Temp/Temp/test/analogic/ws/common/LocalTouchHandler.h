//------------------------------------------------------------------------------
// File: LocalTouchHandler.h
// Description: Header file for class to handle Touch-Pinch and
//    Touch-Move events.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_COMMON_LOCALTOUCHHANDLER_H_
#define TEST_ANALOGIC_WS_COMMON_LOCALTOUCHHANDLER_H_
//------------------------------------------------------------------------------
#include <QPoint>
#include <QTime>
#include <QDebug>

#define DEF_ZOOM_WHEEL_DELTA               120

#define TOUCH_PROCESS_THRESHOLD            7    // Pixels

#define PINCH_STRETCH_ANGLE_THRESHOLLD     120  // Degrees
#define MAX_ANGLE_BETWEEN_MOVE_STROKE      15   // Degrees

#define TOUCH_SCALE_MOVE                   0.8

class OpenGLVolumeWidget;

enum TouchProcResulst_E
{
  TPR_Pinch    = 0,
  TPR_Spread   = 1,
  TPR_Move     = 2,
  TPR_Rotate   = 3,
  TPR_Ignore   = 4,
};


class LocalTouchHandler
{
public:
    explicit LocalTouchHandler(OpenGLVolumeWidget* parent);
    virtual ~LocalTouchHandler();



    void adjustThreshod(int pinchThreshold,
                        int spreadThreshold,
                        int moveThreshold);

    bool isTwoFingerTouchActive();
    bool setTwoFingerTouchActive(bool setVal);
    void setCurrentPos(QPoint pb0, QPoint pb1);
    void setStartPos(QPoint ps0, QPoint ps1);

    TouchProcResulst_E processTwoFingerTouch(double& deltaX,
                             double& deltaY);

    TouchProcResulst_E processOneFingerTouch(QPoint& currentPos);

    void performTouchPinch();

    void performTouchSpread();

    void performTouchMove(int panX,
                    int panY);


    void debugShowStatus();

private:
    void startEventTimer(QString messageStart);
    void stopEventTimer(QString messageStop);



    bool           m_twoFingerTouchActive;
    OpenGLVolumeWidget*  m_volumeWidget;

    int            m_touchPinchCount;
    int            m_touchSpreadCount;
    int            m_touchMoveCount;

    int            m_pinchThreshold;
    int            m_spreadThreshold;
    int            m_moveThreshold;


    QPoint         m_startPos0;
    QPoint         m_startPos1;
    QPoint         m_currentPos0;
    QPoint         m_currentPos1;

    //------------------------------
    QTime  m_eventTimer;
};

//------------------------------------------------------------------------------
#endif  //  TEST_ANALOGIC_WS_COMMON_LOCALTOUCHHANDLER_H_
//------------------------------------------------------------------------------
