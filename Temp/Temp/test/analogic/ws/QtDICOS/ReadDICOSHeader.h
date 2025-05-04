//------------------------------------------------------------------------------
// File: ReadDICOSHeader.h
// Purpose: header file for Read/Write of DICOS CT files
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_QTDICOS_READDICOSHEADER_H_
#define TEST_ANALOGIC_WS_QTDICOS_READDICOSHEADER_H_
//------------------------------------------------------------------------------

#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <SDICOS/SdcsTemplateCT.h>

#include <string>
#include <thread>
#include <iostream>
#include <fstream>


#include "SDICOS/SdcsTDR.h"
#include "SDICOS/SdcsCT.h"

#include "VRUtils.h"

namespace DICOS
{

class SimpleScanProperties
{
public:
  SimpleScanProperties(const std::string bagId,
                       const std::string machineID):
    m_bagID(bagId),
    m_machineID(machineID)
  {
  }
  std::string m_bagID;
  std::string m_machineID;
};

bool readDICOSCtFile(SDICOS::CTModule& ctModule,
                     const std::string& dicosDataFile,
                     const std::string& dicosLogfile);

bool writeDICOSCtFile(SDICOS::CTModule *ct,
                      analogic::workstation::ImageProperties* iProp,
                      SimpleScanProperties& scanProps,
                      const std::string& dicosDataFile,
                      const std::string& dicosLogFile);


}   // namespace DICOS



//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_QTDICOS_READDICOSHEADER_H_
//------------------------------------------------------------------------------
