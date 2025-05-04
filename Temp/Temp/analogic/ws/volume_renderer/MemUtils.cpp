//------------------------------------------------------------------------------
// File: MemUtils.cpp
// Purpose: Memory Utilitiy functions (must work for both Windows and Linux)
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "MemUtils.h"

//----------------------
#ifdef WIN32
#include <windows.h>
#else
// LINUX
#include <sys/sysinfo.h>
#endif

namespace MemUtils
{

//------------------------------------------------------------------------------
bool testForAvailableMemory(uint64_t bytesRequsted, uint64_t& bytesAvailable)
{
  bool retVal = true;
  uint64_t sizeMem;

#ifdef WIN32
  retVal = false;
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof (statex);
  GlobalMemoryStatusEx(&statex);
  sizeMem = statex.ullAvailPhys;
  bytesAvailable = sizeMem;
  qDebug() << "Available mem = " << sizeMem/(1024*1024) << "MByte";
  qDebug() << "Requested mem = " << bytesRequsted/(1024*1024) << "MByte";
  if (sizeMem > bytesRequsted)
  {
     retVal = true;
  }
  else
  {
    retVal = false;
    qDebug() <<   "Not enough available Memory for this request!";
  }
#else
  retVal = false;
  struct sysinfo si;
  int siResp =  sysinfo(&si);
  Q_UNUSED(siResp);
  sizeMem = si.freeram;
  bytesAvailable = sizeMem;
  qDebug() << "Available mem = " << sizeMem/(1024*1024) << "MByte";
  qDebug() << "Requested mem = " << bytesRequsted/(1024*1024) << "MByte";
  if (sizeMem > bytesRequsted)
  {
     retVal = true;
  }
  else
  {
    retVal = false;
    qDebug() <<   "Not enough available Memory for this request!";
  }
#endif
  return retVal;
}

//------------------------------------------------------------------------------
void getTotalAndFreeMemoryBytes(uint64_t& bytesTotal,
                            uint64_t& bytesAvailable)
{
  uint64_t sizeMemAvail;
  uint64_t sizeMemTot;

#ifdef WIN32
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof (statex);
  GlobalMemoryStatusEx(&statex);
  sizeMemAvail = statex.ullAvailPhys;
  sizeMemTot   = statex.ullTotalPhys;

  bytesAvailable = sizeMemAvail;
  bytesTotal = sizeMemTot;
#else
  struct sysinfo si;
  int siResp =  sysinfo(&si);
  Q_UNUSED(siResp);
  sizeMemAvail = si.freeram;
  sizeMemTot   = si.totalram;

  bytesAvailable = sizeMemAvail;
  bytesTotal = sizeMemTot;
#endif
}


//------------------------------------------------------------------------------
}  // namespace MemUtils
//------------------------------------------------------------------------------
