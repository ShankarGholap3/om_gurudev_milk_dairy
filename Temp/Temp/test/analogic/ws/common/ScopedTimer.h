//----------------------------------------------------------------------
// File: ScopedTimer.h
// Description: Scoped Timer header file
// (c) Copyright 2018  Analogic Corp.
//----------------------------------------------------------------------
//----------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_COMMON_SCOPEDTIMER_H_
#define TEST_ANALOGIC_WS_COMMON_SCOPEDTIMER_H_
//----------------------------------------------------------------------

#include <QTime>

class ScopedTimer
{
public:
  ScopedTimer();
  virtual ~ScopedTimer();
  static double getProcessTimerValue();
  static void resetProcessTimer();

private:
  QTime          m_msprocessTimer;

  static double s_elapsedProcessTime;
};


//-----------------------------------------------------------------------
#endif  //  TEST_ANALOGIC_WS_COMMON_SCOPEDTIMER_H_
//-----------------------------------------------------------------------
