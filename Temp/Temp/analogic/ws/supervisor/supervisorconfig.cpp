/*!
* @file     supervisorconfig.cpp
* @author   Agiliad
* @brief    This file contains interface having configuration for screens of supervisor Workstation.
* @date     Jul, 11 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include "supervisorconfig.h"

namespace analogic
{
namespace ws
{

int SupervisorConfig::m_supervisorListUpdateInterval;
analogic::nss::SupervisorAgentStaticProperties SupervisorConfig::m_supervisorProperties;

/*!
* @fn       SupervisorConfig
* @param    None
* @return   None
* @brief    Constructor for class SupervisorConfig.
*/
SupervisorConfig::SupervisorConfig()
{
  TRACE_LOG("");
  readSupervisorConfig(WorkstationConfig::getInstance()->getexecutablePath() + "/" + INI_FILE_NAME);
}

/*!
* @fn       ~SupervisorConfig
* @param    None
* @return   None
* @brief    Destructor for class SupervisorConfig.
*/
SupervisorConfig::~SupervisorConfig()
{

}

/*!
* @fn       readSupervisorConfig
* @param    const std::string - config file
* @return   None
* @brief    initialization for configuration.
*/
void SupervisorConfig::readSupervisorConfig(const std::string ini_file)
{
  if (ini_file.length() > 0 && boost::filesystem::exists(ini_file))
  {
    INFO_LOG( "ini file: " << ini_file);
    boost::property_tree::ptree ptr;
    boost::property_tree::ini_parser::read_ini(ini_file, ptr);

    //transfer_options not available in SupervisorAgentStaticProperties
    try
    {
      //std::string ls = ptr.get<std::string>("OsrConfiguration.TransferOption");
      //m_supervisorProperties.transfer_options.push_back(analogic::nss::TransferOption(ls.c_str()));
    }
    catch (...)
    {
      //m_supervisorProperties.transfer_options.push_back(analogic::nss::TransferOption("dummy,0,dummy"));
    }

    try
    {
      std::string ls = ptr.get<std::string>("SupervisorConfiguration.SupervisorListUpdateInterval");
      m_supervisorListUpdateInterval  = QString::fromStdString(ls).toInt();
    }
    catch (...)
    {
      m_supervisorListUpdateInterval = DEFAULT_TIMEOUT;
    }
    INFO_LOG("Setting time interval of checking supervisor list update: " << m_supervisorListUpdateInterval);
    INFO_LOG("Setting supervisor static properties passphrase ");
  }

  QString temp;
  try
  {
    temp = WorkstationConfig::getInstance()->getWorkstationMachineId();
  }
  catch(...)
  {
    temp = DEFAULT_MACHINE_ID;
  }
  m_supervisorProperties.dicos_general_equipment_module.SetMachineID(temp.toStdString().c_str());
  INFO_LOG( "MachineId:"<<temp.toStdString().c_str());

  try
  {
    temp = WorkstationConfig::getInstance()->getWorkstationManufactId();
  } catch(...)
  {
    temp = DEFAULT_MANUFACTURER;
  }
  m_supervisorProperties.dicos_general_equipment_module.SetManufacturer(temp.toStdString().c_str());
  INFO_LOG( "Manufacturer:"<<temp.toStdString().c_str());

  try
  {
    temp =  WorkstationConfig::getInstance()->getManufacturerModelNumber();
  } catch(...)
  {
    temp = DEFAULT_MODEL_NUMBER;
  }
  m_supervisorProperties.dicos_general_equipment_module.SetManufacturerModelName(temp.toStdString().c_str());
  INFO_LOG( "Model Number:"<<temp.toStdString().c_str());

  try
  {
    temp = WorkstationConfig::getInstance()->getWorkstationSerialNumber();
  } catch(...)
  {
    temp = DEFAULT_SERIAL_NUMBER;
  }
  m_supervisorProperties.dicos_general_equipment_module.SetDeviceSerialNumber(temp.toStdString().c_str());
  INFO_LOG( "Workstation serial Number:"<<temp.toStdString().c_str());

  try
  {
    temp = WorkstationConfig::getInstance()->getMachineAddress();
  } catch(...)
  {
    temp = DEFAULT_SERIAL_NUMBER;
  }
  m_supervisorProperties.dicos_general_equipment_module.SetMachineAddress(temp.toStdString().c_str());
  m_supervisorProperties.dicos_general_equipment_module.SetMachineSubLocation(temp.toStdString().c_str());
  INFO_LOG( "Machine Address:"<<temp.toStdString().c_str());

  try
  {
    temp = WorkstationConfig::getInstance()->getMachineLocation();
  } catch(...)
  {
    temp = DEFAULT_SERIAL_NUMBER;
  }
  m_supervisorProperties.dicos_general_equipment_module.SetMachineLocation(temp.toStdString().c_str());
  INFO_LOG( "Machine Location:"<<temp.toStdString().c_str());

  try
  {
    temp = WorkstationConfig::getInstance()->getWsVersion();
  } catch(...)
  {
    temp = WORKSTATION_VERSION;
  }
  m_supervisorProperties.dicos_general_equipment_module.SetSoftwareVersion(temp.toStdString().c_str());
  INFO_LOG( "Software Version:" << temp.toStdString().c_str());
}

/*!
* @fn       getSupervisorAgentProperties
* @param    None
* @return   analogic::nss::SupervisorAgentStaticProperties- properties
* @brief    gets Supervisor agent properties.
*/
analogic::nss::SupervisorAgentStaticProperties SupervisorConfig::getSupervisorAgentProperties()
{
  return m_supervisorProperties;
}
/*!
* @fn       getSupervisorListUpdateInterval
* @param    None
* @return   int - timeout
* @brief    time interval to check supervisor list update
*/
int SupervisorConfig::getSupervisorListUpdateInterval()
{
  return m_supervisorListUpdateInterval;
}

}  // end of namespace ws
}  // end of namespace analogic


