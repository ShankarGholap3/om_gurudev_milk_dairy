/*!
* @file     rerunconfig.cpp
* @author   Agiliad
* @brief    This file contains interface having configuration for rerun type workstation
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/rerun/rerunconfig.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
QString RerunConfig::m_playlistdir;
bool RerunConfig::m_isRemovableStorage;
QString RerunConfig::m_operatorResultPath;
QString RerunConfig::m_playlistDecisiondir;
int RerunConfig::m_defaultRequiredSpace;
analogic::nss::OsrAgentStaticProperties RerunConfig::m_osrProperties;
/*!
* @fn       RerunConfig
* @param    None
* @return   None
* @brief    Constructor for class RerunConfig.
*/
RerunConfig::RerunConfig()
{
    TRACE_LOG("");
    readRerunConfig(WorkstationConfig::getexecutablePath() + "/" + RERUN_INI_FILE_NAME);
}

/*!
* @fn       ~RerunConfig
* @param    None
* @return   None
* @brief    Destructor for class RerunConfig.
*/
RerunConfig::~RerunConfig()
{
    TRACE_LOG("");
}

/*!
* @fn       getOsrAgentProperties
* @param    None
* @return   analogic::nss::OsrAgentStaticProperties- properties
* @brief    gets osr agent properties.
*/
analogic::nss::OsrAgentStaticProperties RerunConfig::getOsrAgentProperties()
{
    return m_osrProperties;
}

/*!
* @fn       readRerunConfig
* @param    const std::string - config file
* @return   None
* @brief    initialization for configuration.
*/
void RerunConfig::readRerunConfig(const std::string ini_file)
{
    if (ini_file.length() > 0 && boost::filesystem::exists(ini_file))
    {
        INFO_LOG("ini file: " << ini_file);
        boost::property_tree::ptree ptr;
        boost::property_tree::ini_parser::read_ini(ini_file, ptr);
        QString temp;
        try
        {
            std::string ls;
            bool is_auto_clear = ptr.get<bool>("Storage.RemovableStorage");
            m_isRemovableStorage = is_auto_clear;
        }
        catch (...)
        {
            m_isRemovableStorage = false;
        }
        try
        {
            m_defaultRequiredSpace = ptr.get<int>("Storage.DefaultRequiredSpace");
        }
        catch (...)
        {
            m_defaultRequiredSpace = 520;
        }
        try
        {
            std::string ls = ptr.get<std::string>("Training.PlaylistPath");
            m_playlistdir  = ls.c_str();
        }
        catch (...)
        {
            m_playlistdir = DEFAULT_PLAYLIST_DIR;
        }
        DEBUG_LOG("Setting Playlist Path: " << m_playlistdir.toStdString());
        try
        {
            std::string ls = ptr.get<std::string>("Training.OperatorResult");
            m_operatorResultPath  = ls.c_str();
        }
        catch (...)
        {
            m_operatorResultPath = DEFAULT_OPERATOR_RESULT_DIR;
        }
        try
        {
            std::string ls = ptr.get<std::string>("Training.PlaylistDecision");
            m_playlistDecisiondir  = ls.c_str();
        }
        catch (...)
        {
            m_playlistDecisiondir = DEFAULT_PLAYLIST_DECISION_DIR;
        }
        DEBUG_LOG("Setting Operator Result Path: " << m_playlistDecisiondir.toStdString());
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
                m_osrProperties.dicos_general_equipment_module.SetSoftwareVersion(WORKSTATION_VERSION);
                INFO_LOG( "Software Version:" << WORKSTATION_VERSION);

    }
}

/*!
* @fn       isRemovableStorage
* @param    None
* @return   bool
* @brief    return removable storage flag
*/
bool RerunConfig::isRemovableStorage()
{
    return m_isRemovableStorage;;
}

/*!
* @fn       getDefaultRequiredSpace
* @param    None
* @return   int
* @brief    return Default available space value
*/
int RerunConfig::getDefaultRequiredSpace()
{
    return m_defaultRequiredSpace;
}

/*!
* @fn       getPlaylistPath
* @param    None
* @return   QString - Training Playlist path
* @brief    return training playlist location
*/
QString RerunConfig::getPlaylistPath()
{
    INFO_LOG("Getting Training Playlist Path:" << m_playlistdir.toStdString());
    return WorkstationConfig::getInstance()->getDataPath() + "/" +m_playlistdir;
}

QString RerunConfig::getPlaylistDecisionPath()
{
  INFO_LOG("Getting Training Playlist Decision Path:" << m_playlistDecisiondir.toStdString());
  return WorkstationConfig::getInstance()->getDataPath() + "/" +m_playlistDecisiondir;
}

/*!
* @fn       getOperatorResultPath
* @param    None
* @return   QString - Operator Result Path
* @brief    return Operator Result Location
*/
QString RerunConfig::getOperatorResultPath()
{
    INFO_LOG("Getting Operator Result Path:" << m_operatorResultPath.toStdString());
    return WorkstationConfig::getInstance()->getDataPath() + "/" + m_operatorResultPath;
}

/*!
* @fn       getReportResultPath
* @param    None
* @return   QString - Report Result Path
* @brief    return Report Result Location
*/
QString RerunConfig::getReportResultPath(QString m_userID)
{
  INFO_LOG("Getting Report Result Path:");
  return WorkstationConfig::getInstance()->getDataPath() + "/" + m_operatorResultPath+"/" + m_userID;

}
static analogic::nss::OsrAgentStaticProperties getOsrAgentProperties();
}  // end of namespace ws
}  // end of namespace analogic
