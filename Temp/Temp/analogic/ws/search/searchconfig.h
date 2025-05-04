/*!
* @file     searchconfig.h
* @author   Agiliad
* @brief    This file contains interface having configuration for screens of Search Workstation.
* @date     Jan, 24 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SEARCHCONFIG_H
#define SEARCHCONFIG_H

#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/nss/agent/search/SearchAgentStaticProperties.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SearchConfig
 * \brief   This class contains structure for search config
 */
class SearchConfig: public WorkstationConfig
{
public:
    /*!
    * @fn       SearchConfig
    * @param    None
    * @return   None
    * @brief    Constructor for class SearchConfig.
    */
    SearchConfig();

    /*!
    * @fn       ~SearchConfig
    * @param    None
    * @return   None
    * @brief    Destructor for class SearchConfig.
    */
    ~SearchConfig();

    /*!
    * @fn       readSearchConfig
    * @param    const std::string - config file
    * @return   None
    * @brief    initialization for configuration.
    */
    static void readSearchConfig(const std::string ini_file);
    /*!
    * @fn       getSearchAgentProperties
    * @param    None
    * @return   analogic::nss::SearchAgentStaticProperties- properties
    * @brief    gets Search agent properties.
    */
    static analogic::nss::SearchAgentStaticProperties getSearchAgentProperties();

    /*!
    * @fn       getSearchlistupdateInterval
    * @param    None
    * @return   int - timeout
    * @brief    time interval to check search list update
    */
    static int getSearchlistupdateInterval();


private:
    static analogic::nss::SearchAgentStaticProperties  m_searchProperties;  //!< Holds information about Search agent at registration-time.
    static int            m_searchListUpdateInterval;                 //!< NSS check interval
};
}  // end of namespace ws
}  // end of namespace analogic

#endif // SEARCHCONFIG_H
