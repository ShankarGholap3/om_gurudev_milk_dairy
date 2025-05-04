/*!
* @file     osrconfig.h
* @author   Agiliad
* @brief    This file contains interface having configuration for screens of Primary Workstation.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_OSR_OSRCONFIG_H_
#define ANALOGIC_WS_OSR_OSRCONFIG_H_


#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/nss/agent/osr/OsrAgentStaticProperties.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   OsrConfig
 * \brief   This class contains functionality related to OSR Configurations
 */
class OsrConfig : public WorkstationConfig
{
public:
    /*!
    * @fn       OsrConfig
    * @param    None
    * @return   None
    * @brief    Constructor for class OsrConfig.
    */
    OsrConfig();
    /*!
    * @fn       ~OsrConfig
    * @param    None
    * @return   None
    * @brief    Destructor for class OsrConfig.
    */
    ~OsrConfig();

    /*!
    * @fn       readOsrConfig
    * @param    const std::string - config file
    * @return   None
    * @brief    initialization for configuration.
    */
    static void readOsrConfig(const std::string ini_file);

    /*!
    * @fn       getOsrAgentProperties
    * @param    None
    * @return   analogic::nss::OsrAgentStaticProperties- properties
    * @brief    gets osr agent properties.
    */
    static analogic::nss::OsrAgentStaticProperties getOsrAgentProperties();

    /*!
    * @fn       setAutoclearTimeout
    * @param    int autoclear_timeout
    * @return   None
    * @brief    time out at which bag is to be ceared
    */
    static void setAutoclearTimeout(int autoclear_timeout);

    /*!
    * @fn       getAutoclearTimeout
    * @param    None
    * @return   int - timeout
    * @brief    time out at which bag is to be ceared
    */
    static int getAutoclearTimeout();

    /*!
    * @fn       getIsAutoclearEnabled
    * @param    None
    * @return   bool
    * @brief    check if auto clear enabled
    */
    static bool getIsAutoclearEnabled();

    /*!
    * @fn       setIsAutoclearEnabled
    * @param    bool is_autoclear_enabled
    * @return   None
    * @brief    sets auto clear enabled/ disabled
    */
    static void setIsAutoclearEnabled(bool is_autoclear_enabled);

private:
    static analogic::nss::OsrAgentStaticProperties  m_osrProperties;  //!< OSR Properties for OSR agent creation
    static bool           m_is_autoclear_enabled;             //!< Auto clear flag
    static int            m_autoclear_timeout;                //!< Autoclear timeout value
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_OSR_OSRCONFIG_H_

