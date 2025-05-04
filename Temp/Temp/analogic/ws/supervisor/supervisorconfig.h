/*!
* @file     supervisorconfig.h
* @author   Agiliad
* @brief    This file contains interface having configuration for screens of Supervisor Workstation.
* @date     Jul, 11 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORCONFIG_H
#define SUPERVISORCONFIG_H

#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/nss/agent/supervisor/SupervisorAgentStaticProperties.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorConfig
 * \brief   This class contains structure for supervisor config
 */
class SupervisorConfig: public WorkstationConfig
{
public:
    /*!
    * @fn       SupervisorConfig
    * @param    None
    * @return   None
    * @brief    Constructor for class SupervisorConfig.
    */
    SupervisorConfig();

    /*!
    * @fn       ~SupervisorConfig
    * @param    None
    * @return   None
    * @brief    Destructor for class SupervisorConfig.
    */
    ~SupervisorConfig();

    /*!
    * @fn       readSupervisorConfig
    * @param    const std::string - config file
    * @return   None
    * @brief    initialization for configuration.
    */
    static void readSupervisorConfig(const std::string ini_file);
    /*!
    * @fn       getSupervisorAgentProperties
    * @param    None
    * @return   analogic::nss::SupervisorAgentStaticProperties- properties
    * @brief    gets Supervisor agent properties.
    */
    static analogic::nss::SupervisorAgentStaticProperties getSupervisorAgentProperties();

    /*!
    * @fn       getSupervisorListUpdateInterval
    * @param    None
    * @return   int - timeout
    * @brief    time interval to check Supervisor list update
    */
    static int getSupervisorListUpdateInterval();


private:
    static analogic::nss::SupervisorAgentStaticProperties  m_supervisorProperties;          //!< Holds information about supervisor agent at registration-time.
    static int                                             m_supervisorListUpdateInterval; //!< NSS check interval
};
}  // end of namespace ws
}  // end of namespace analogic

#endif // SUPERVISORCONFIG_H
