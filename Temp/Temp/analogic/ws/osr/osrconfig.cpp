/*!
* @file     osrconfig.cpp
* @author   Agiliad
* @brief    This file contains interface having configuration for screens of Primary Workstation.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/osr/osrconfig.h>


namespace analogic
{
namespace ws
{
int OsrConfig::m_autoclear_timeout;
bool OsrConfig::m_is_autoclear_enabled;
analogic::nss::OsrAgentStaticProperties OsrConfig::m_osrProperties;

/*!
* @fn       OsrConfig
* @param    None
* @return   None
* @brief    Constructor for class OsrConfig.
*/
OsrConfig::OsrConfig()
{
    readOsrConfig(WorkstationConfig::getInstance()->getexecutablePath() + "/" +
                  INI_FILE_NAME);
}

/*!
* @fn       ~OsrConfig
* @param    None
* @return   None
* @brief    Destructor for class OsrConfig.
*/
OsrConfig::~OsrConfig()
{
    TRACE_LOG("");
}

/*!
* @fn       readOsrConfig
* @param    const std::string - config file
* @return   None
* @brief    initialization for configuration.
*/
void OsrConfig::readOsrConfig(const std::string ini_file)
{
    if (ini_file.length() > 0 && boost::filesystem::exists(ini_file))
    {
        INFO_LOG( "ini file: " << ini_file);
        boost::property_tree::ptree ptr;
        boost::property_tree::ini_parser::read_ini(ini_file, ptr);

        try
        {
            bool is_auto_clear = ptr.get<bool>("AutoClear.Enabled");
            m_is_autoclear_enabled = is_auto_clear;
        }
        catch (...)
        {
            m_is_autoclear_enabled = DEFAULT_AUTOCLEAR_ENABLED;
        }
        DEBUG_LOG("Setting AutoClear to: " << m_is_autoclear_enabled);
        try
        {
            int auto_clear_timeout = ptr.get<int>("AutoClear.Timeout");
            m_autoclear_timeout = auto_clear_timeout;
        }
        catch (...)
        {
            m_autoclear_timeout = DEFAULT_AUTOCLEAR_TIMEOUT;
        }
        DEBUG_LOG("Setting AutoClear timeout: " << m_autoclear_timeout);

        try
        {

            std::string ls = ptr.get<std::string>("OsrConfiguration.TransferOption");
            m_osrProperties.transfer_options.push_back(analogic::nss::TransferOption(ls.c_str()));
        }
        catch (...)
        {
            m_osrProperties.transfer_options.push_back(analogic::nss::TransferOption("dummy,0,dummy"));
        }
        INFO_LOG( "Setting OSR static properties passphrase ");
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
    m_osrProperties.dicos_general_equipment_module.SetMachineID(temp.toStdString().c_str());
    INFO_LOG( "MachineId:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getWorkstationManufactId();
    } catch(...)
    {
        temp = DEFAULT_MANUFACTURER;
    }
    m_osrProperties.dicos_general_equipment_module.SetManufacturer(temp.toStdString().c_str());
    INFO_LOG( "Manufacturer:"<<temp.toStdString().c_str());

    try
    {
        temp =  WorkstationConfig::getInstance()->getManufacturerModelNumber();
    } catch(...)
    {
        temp = DEFAULT_MODEL_NUMBER;
    }
    m_osrProperties.dicos_general_equipment_module.SetManufacturerModelName(temp.toStdString().c_str());
    INFO_LOG( "Model Number:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getWorkstationSerialNumber();
    } catch(...)
    {
        temp = DEFAULT_SERIAL_NUMBER;
    }
    m_osrProperties.dicos_general_equipment_module.SetDeviceSerialNumber(temp.toStdString().c_str());
    INFO_LOG( "Workstation serial Number:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getMachineAddress();
    } catch(...)
    {
        temp = DEFAULT_SERIAL_NUMBER;
    }
    m_osrProperties.dicos_general_equipment_module.SetMachineAddress(temp.toStdString().c_str());
    m_osrProperties.dicos_general_equipment_module.SetMachineSubLocation(temp.toStdString().c_str());
    INFO_LOG( "Machine Address:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getMachineLocation();
    } catch(...)
    {
        temp = DEFAULT_SERIAL_NUMBER;
    }
    m_osrProperties.dicos_general_equipment_module.SetMachineLocation(temp.toStdString().c_str());

    INFO_LOG( "Machine Location:"<<temp.toStdString().c_str());
    try
    {
        temp = WorkstationConfig::getInstance()->getWsVersion();
    } catch(...)
    {
        temp = WORKSTATION_VERSION;
    }
    m_osrProperties.dicos_general_equipment_module.SetSoftwareVersion(temp.toStdString().c_str());
    INFO_LOG( "Software Version:" << temp.toStdString().c_str());}

/*!
* @fn       getOsrAgentProperties
* @param    None
* @return   analogic::nss::OsrAgentStaticProperties- properties
* @brief    gets osr agent properties.
*/
analogic::nss::OsrAgentStaticProperties OsrConfig::getOsrAgentProperties()
{
    return m_osrProperties;
}

/*!
* @fn       getAutoclearTimeout
* @param    None
* @return   int - timeout
* @brief    time out at which bag is to be ceared
*/
int OsrConfig::getAutoclearTimeout()
{
    return m_autoclear_timeout;
}

/*!
* @fn       setAutoclearTimeout
* @param    int autoclear_timeout
* @return   None
* @brief    time out at which bag is to be ceared
*/
void OsrConfig::setAutoclearTimeout(int autoclear_timeout)
{
    m_autoclear_timeout = autoclear_timeout;
}

/*!
* @fn       getIsAutoclearEnabled
* @param    None
* @return   bool
* @brief    check if auto clear enabled
*/
bool OsrConfig::getIsAutoclearEnabled()
{
    return m_is_autoclear_enabled;
}

/*!
* @fn       setIsAutoclearEnabled
* @param    bool is_autoclear_enabled
* @return   None
* @brief    sets auto clear enabled/ disabled
*/
void OsrConfig::setIsAutoclearEnabled(bool is_autoclear_enabled)
{
    m_is_autoclear_enabled = is_autoclear_enabled;
}
}  // end of namespace ws
}  // end of namespace analogic
