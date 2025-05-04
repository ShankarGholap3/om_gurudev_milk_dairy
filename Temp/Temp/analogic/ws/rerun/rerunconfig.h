/*!
* @file     rerunconfig.h
* @author   Agiliad
* @brief    This file contains interface having configuration for rerun type workstation
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_RERUN_RERUNCONFIG_H_
#define ANALOGIC_WS_RERUN_RERUNCONFIG_H_

#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/nss/agent/osr/OsrAgentStaticProperties.h>
#define DEFAULT_PLAYLIST_DIR "Playlist"
#define DEFAULT_OPERATOR_RESULT_DIR "OperatorResults"
#define DEFAULT_PLAYLIST_DECISION_DIR "PlaylistDecision"

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   RerunConfig
 * \brief   This class contains functionality related to Rerun Configurations
 */
class RerunConfig : public WorkstationConfig
{
public:
    /*!
    * @fn       RerunConfig
    * @param    None
    * @return   None
    * @brief    Constructor for class RerunConfig.
    */
    RerunConfig();
    /*!
    * @fn       ~RerunConfig
    * @param    None
    * @return   None
    * @brief    Destructor for class RerunConfig.
    */
    ~RerunConfig();
    /*!
    * @fn       readRerunConfig
    * @param    const std::string - config file
    * @return   None
    * @brief    initialization for configuration.
    */
    static void readRerunConfig(const std::string ini_file);

    /*!
    * @fn       isRemovableStorage
    * @param    None
    * @return   bool
    * @brief    return removable storage flag
    */
    static bool isRemovableStorage();

    /*!
    * @fn       getDefaultRequiredSpace
    * @param    None
    * @return   int
    * @brief    return Default available space value
    */
    static int getDefaultRequiredSpace();

    /*!
    * @fn       getPlaylistPath
    * @param    None
    * @return   QString - Training Playlist path
    * @brief    return training playlist location
    */
    static QString getPlaylistPath();

    /*!
    * @fn       getPlaylistDecisionPath
    * @param    None
    * @return   QString - Training Playlist Decision path
    * @brief    return training playlist Decision location
    */
    static QString getPlaylistDecisionPath();

    /*!
    * @fn       getOperatorResultPath
    * @param    None
    * @return   QString - Operator Result Path
    * @brief    return Operator Result Location
    */
    static QString getOperatorResultPath();

    /*!
    * @fn       getOsrAgentProperties
    * @param    None
    * @return   analogic::nss::OsrAgentStaticProperties- properties
    * @brief    gets osr agent properties.
    */
    static analogic::nss::OsrAgentStaticProperties getOsrAgentProperties();


    /*!
    * @fn       getReportResultPath
    * @param    None
    * @return   QString - Report Result Path
    * @brief    return Report Result Location
    */
    QString getReportResultPath(QString m_userID);


private:
    static analogic::nss::OsrAgentStaticProperties  m_osrProperties;  //!< OSR Propert
    static bool          m_isRemovableStorage;
    static QString       m_playlistdir;                     //!< Training Playlist Path String
    static QString       m_operatorResultPath;              //!< Operator Result Path
    static QString       m_playlistDecisiondir;             //!< Training Playlist Decision Path String
    static int           m_defaultRequiredSpace;            //!< Available space required to run bags    
    QString              m_userID;                          //!< Training Report Screen selected userID
};

}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_RERUN_RERUNCONFIG_H_
