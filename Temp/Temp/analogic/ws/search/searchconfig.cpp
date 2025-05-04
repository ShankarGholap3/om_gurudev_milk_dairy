/*!
* @file     searchconfig.cpp
* @author   Agiliad
* @brief    This file contains interface having configuration for screens of Search Workstation.
* @date     Jan, 24 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/search/searchconfig.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
namespace analogic
{
namespace ws
{

int SearchConfig::m_searchListUpdateInterval;
analogic::nss::SearchAgentStaticProperties SearchConfig::m_searchProperties;

/*!
* @fn       SearchConfig
* @param    None
* @return   None
* @brief    Constructor for class SearchConfig.
*/
SearchConfig::SearchConfig()
{
    TRACE_LOG("");
    readSearchConfig(WorkstationConfig::getInstance()->getexecutablePath() + "/" + INI_FILE_NAME);
}

/*!
* @fn       ~SearchConfig
* @param    None
* @return   None
* @brief    Destructor for class SearchConfig.
*/
SearchConfig::~SearchConfig()
{

}

/*!
* @fn       readSearchConfig
* @param    const std::string - config file
* @return   None
* @brief    initialization for configuration.
*/
void SearchConfig::readSearchConfig(const std::string ini_file)
{
    if (ini_file.length() > 0 && boost::filesystem::exists(ini_file))
    {
        INFO_LOG( "ini file: " << ini_file);
        boost::property_tree::ptree ptr;
        boost::property_tree::ini_parser::read_ini(ini_file, ptr);


        try
        {

            std::string ls = ptr.get<std::string>("OsrConfiguration.TransferOption");
            m_searchProperties.transfer_options.push_back(analogic::nss::TransferOption(ls.c_str()));
        }
        catch (...)
        {
            m_searchProperties.transfer_options.push_back(analogic::nss::TransferOption("dummy,0,dummy"));
        }

        try
        {
            std::string ls = ptr.get<std::string>("SearchConfiguration.SearchListUpdateInterval");
            m_searchListUpdateInterval  = QString::fromStdString(ls).toInt();
        }
        catch (...)
        {
            m_searchListUpdateInterval = DEFAULT_TIMEOUT;
        }
        DEBUG_LOG("Setting time interval of checking search list update: " << m_searchListUpdateInterval);


        INFO_LOG("Setting SEARCH static properties passphrase ");
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
    m_searchProperties.dicos_general_equipment_module.SetMachineID(temp.toStdString().c_str());
    INFO_LOG( "MachineId:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getWorkstationManufactId();
    } catch(...)
    {
        temp = DEFAULT_MANUFACTURER;
    }
    m_searchProperties.dicos_general_equipment_module.SetManufacturer(temp.toStdString().c_str());
    INFO_LOG( "Manufacturer:"<<temp.toStdString().c_str());

    try
    {
        temp =  WorkstationConfig::getInstance()->getManufacturerModelNumber();
    } catch(...)
    {
        temp = DEFAULT_MODEL_NUMBER;
    }
    m_searchProperties.dicos_general_equipment_module.SetManufacturerModelName(temp.toStdString().c_str());
    INFO_LOG( "Model Number:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getWorkstationSerialNumber();
    } catch(...)
    {
        temp = DEFAULT_SERIAL_NUMBER;
    }
    m_searchProperties.dicos_general_equipment_module.SetDeviceSerialNumber(temp.toStdString().c_str());
    INFO_LOG( "Workstation serial Number:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getMachineAddress();
    } catch(...)
    {
        temp = DEFAULT_SERIAL_NUMBER;
    }
    m_searchProperties.dicos_general_equipment_module.SetMachineAddress(temp.toStdString().c_str());
    m_searchProperties.dicos_general_equipment_module.SetMachineSubLocation(temp.toStdString().c_str());
    INFO_LOG( "Machine Address:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getMachineLocation();
    } catch(...)
    {
        temp = DEFAULT_SERIAL_NUMBER;
    }
    m_searchProperties.dicos_general_equipment_module.SetMachineLocation(temp.toStdString().c_str());
    INFO_LOG( "Machine Location:"<<temp.toStdString().c_str());

    try
    {
        temp = WorkstationConfig::getInstance()->getWsVersion();
    } catch(...)
    {
        temp = WORKSTATION_VERSION;
    }
    m_searchProperties.dicos_general_equipment_module.SetSoftwareVersion(temp.toStdString().c_str());
    INFO_LOG( "Software Version:" << temp.toStdString().c_str());
}

/*!
* @fn       getSearchAgentProperties
* @param    None
* @return   analogic::nss::SearchAgentStaticProperties- properties
* @brief    gets Search agent properties.
*/
analogic::nss::SearchAgentStaticProperties SearchConfig::getSearchAgentProperties()
{
    return m_searchProperties;
}
/*!
* @fn       getSearchlistupdateInterval
* @param    None
* @return   int - timeout
* @brief    time interval to check search list update
*/
int SearchConfig::getSearchlistupdateInterval()
{
    return m_searchListUpdateInterval;
}


}  // end of namespace ws
}  // end of namespace analogic
