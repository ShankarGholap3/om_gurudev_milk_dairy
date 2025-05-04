//------------------------------------------------------------------------------
// File: ScopedTimer.cpp
// Description: Scoped Timer implimentation
// (c) Copyright 2018  Analogic Corp.
//------------------------------------------------------------------------------
#include "ScopedTimer.h"

double ScopedTimer::s_elapsedProcessTime = 0;


//------------------------------------------------------------------------------
ScopedTimer::ScopedTimer()
{
  m_msprocessTimer = QTime::currentTime();
}


//------------------------------------------------------------------------------
ScopedTimer::~ScopedTimer()
{
  int msTimeElapsed   =   m_msprocessTimer.elapsed();
  s_elapsedProcessTime +=   static_cast<double>(msTimeElapsed);
}


//------------------------------------------------------------------------------
double ScopedTimer::getProcessTimerValue()
{
  return s_elapsedProcessTime;
}

//------------------------------------------------------------------------------
void ScopedTimer::resetProcessTimer()
{
   s_elapsedProcessTime = 0;
}

