/*!
* @file     workstationconfig.h
* @author   Agiliad
* @brief    This file contains interface, responsible for managing UI configurations of system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_WSCONFIGURATION_WORKSTATIONCONFIG_H_
#define ANALOGIC_WS_WSCONFIGURATION_WORKSTATIONCONFIG_H_

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <QDir>
#include <QSizeF>
#include <QFile>
#include <analogic/ws/common/utility/xmlservice.h>
#include <QTextStream>
#ifdef WORKSTATION
#include <analogic/nss/agent/osr/OsrAgentStaticProperties.h>
#endif
#include <analogic/ws/common.h>
#include <analogic/ws/uihandler/configbaseelement.h>
#include <analogic/ws/ulm/uilayoutmanager.h>

namespace analogic
{
namespace ws
{

// Macros
/**
 * Macro for unzip command
 */
#define DEFAULT_UNZIP "unzip -o"
/**
 * Macro for time out for user inactivity of system
 */
#define DEFAULT_TIMEOUT 600

/**
 * Macro for IQ Bag File Format
 */
#define DEFAULT_IQBAG_FILEFORMAT ".proj0"

/**
 * Macro for IQ Report File Format
 */
#define DEFAULT_IQREPORT_FILEFORMAT ".txt"

/**
 * Macro for IQ Nist File Format
 */
#define DEFAULT_IQNIST_FILEFORMAT ".xml"

#define DEFAULT_NSS_IP_ADDRESS "localhost"
#define DEFAULT_NSS_PORT "5672"
#define DEFAULT_SCANNER_BAG_DATA_PORT "8282"
#define DEFAULT_SCANNER_BAG_DATA_IP_ADDRESS "localhost"
#define DEFAULT_WORKSTATION_NAME "OSR Workstation"
#define DEFAULT_SCANNERNAME ""
#define DEFAULT_AUTOCLEAR_ENABLED "false"
#define DEFAULT_AUTOCLEAR_TIMEOUT 5
#define DEFAULT_NETWORK_RECONNECT 10000
#define DEFAULT_NETWORK_TIMEOUT 10000
#define DEFAULT_TASK_TIMEOUT 3000
#define DEFAULT_BAG_TRANSFER_TIMEOUT_IN_SEC 120
#define DEFAULT_AUTOARCHIVE_ENABLED "false"
#define DEFAULT_CAN_CLEAR_ALARM_BAG true
#define DEFAULT_SHOW_USER_DECISION_TIMEOUT_CONFIRMATION true
#define DEFAULT_MACHINE_ALARM_DECISION true
#define DEFAULT_USER_DECISION_TIMEOUT 30000   // time in milisecond
#define DEFAULT_EXPORT_BAG_DATA_PATH "."
#define DEFAULT_EXPORT_BAG_DATA_MAX_COUNT 10
#define DEFAULT_WS_MACHINE_ID "OSR"
#define DEFAULT_SDSW_CLIENT_PATH "../../service/SDSWClient/dist/SDSWClient"
#define DEFAULT_AUTHENTICATE_WITH_NSS true
#define DEFAULT_SOAP_CLIENT_END_POINT "http://asd/"
#define DEFAULT_SOAP_IP ""
#define DEFAULT_SOAP_PORT 11005
#define DEFAULT_SWS_BHS_SERVER_IP_ADDR "127.0.0.1"
#define DEFAULT_SWS_BHS_SERVER_PORT 11005
#define DEFAULT_RFID_READER_IP_ADDR "192.168.1.200"
#define DEFAULT_RFID_READER_PORT 502
#define DEFAULT_PDF_VIEWER_PATH "/usr/bin/evince"
#define DEFAULT_RENDERPARAMETER_SURFACESHADING true
#define DEFAULT_RENDERPARAMETER_TRANSLUCENTSHADING false
#define DEFAULT_RENDERPARAMETER_SURFACE_INTERP_LINEAR          true
#define DEFAULT_RENDERPARAMETER_TRANSLUCENT_INTERP_LINEAR      false
#define DEFAULT_RENDERPARAMETER_SURFACE_ORGANICREMOVAL         false
#define DEFAULT_RENDERPARAMETER_SURFACE_INORGANICREMOVAL       false
#define DEFAULT_RENDERPARAMETER_SURFACE_METALREMOVAL           false
#define DEFAULT_RENDERPARAMETER_TRANSLUCENT_ORGANICREMOVAL     false
#define DEFAULT_RENDERPARAMETER_TRANSLUCENT_INORGANICREMOVAL   false
#define DEFAULT_RENDERPARAMETER_TRANSLUCENT_METALREMOVAL       false
#define DEFAULT_RENDERPARAMETER_SCANNED_DIR_LEFT_TO_RIGHT      true
#define DEFAULT_RENDERPARAMETER_ENABLE_THREAT_HANDLING         true
#define DEFAULT_RENDERPARAMETER_ENABLE_VTK_SHARPENED_SPLASH    false
#define DEFAULT_RENDERPARAMETER_ENABLE_VTK_DYNAMIC_SPLASH      false
#define DEFAULT_RENDERPARAMETER_VTK_VOL_DISPLAY_UNIT           "centimeter"
#define DEFAULT_RENDERPARAMETER_TRANSLUCENT_AMBIENT_LIGHTING   0.10
#define DEFAULT_RENDERPARAMETER_TRANSLUCENT_DIFFUSE_LIGHTING   0.70
#define DEFAULT_RENDERPARAMETER_TRANSLUCENT_SPECULAR_LIGHTING  0.20
#define DEFAULT_RENDERPARAMETER_SURFACE_AMBIENT_LIGHTING       0.10
#define DEFAULT_RENDERPARAMETER_SURFACE_DIFFUSE_LIGHTING       0.70
#define DEFAULT_RENDERPARAMETER_SURFACE_SPECULAR_LIGHTING      0.20
#define DEFAULT_RENDERPARAMETER_VOLUME_TOOL_MIN                753
#define DEFAULT_RENDERPARAMETER_VOLUME_TOOL_MAX                2201
#define DEFAULT_MANUFACTURER "TBD"
#define DEFAULT_MODEL_NUMBER "TBD"
#define DEFAULT_SERIAL_NUMBER "TBD"
#define DEFAULT_MACHINE_ID "TBD"
#define DEFAULT_WS_DISPLAY_UNIT_SYSTEM "METRIC"

#define DEFAULT_SLABBINGMODE        false
#define DEFAULT_PICKUP_ENABLED false
#define DEFAULT_SLABBING_THICKNESS      0.10

#define DEFAULT_BAG_WIDTH 630
#define DEFAULT_BAG_HEIGHT 430

#define DEFAULT_BAG_PRE_PROCESSING_THREAD 2
#define DEFAULT_MINIMUM_REQUIRED_SLICE_COUNT 600
#define DEFAULT_STRING_VALUE "TBD"
#define DEFAULT_NON_TIP_ENABLE true
#define DEFAULT_ORGANIZATION_NAME "TBD"
#define DEFAULT_OPERATOR_COMPANY_NAME "TBD"

/**
 * Macro for root folder
 */
#define FILE_ROOT_FOLDER "/ancp00"

/**
 * Macro for error log file
 */
#define ERROR_LOG_FOLDER "/ErrorLog"

/**
 * Macro for bag report file
 */
#define FDRS "/FDRS"

/**
 * Macro for tip config folder
 */
#define TIP_FOLDER          "/TIP"

/**
 * Macro for tip xml and xsd files
 */
#define TIP_CONFIG_XML_FILE "/TIPConfig.xml"
#define TIP_CONFIG_XSD_FILE "/TIPConfig.xsd"
#define TIP_OBJECT_CONFIG_XML_FILE "/TIPObjectConfig.xml"
#define TIP_OBJECT_CONFIG_XSD_FILE "/TIPObjectConfig.xsd"
#define TIP_OBJECT_LIB_XML_FILE "/TIPObjectLib.xml"
#define TIP_OBJECT_LIB_XSD_FILE "/TIPObjectLib.xsd"
/**
 * Macro for scanner parameter file
 */
#define SCANNER_PARAMETER "/ScannerParameter"


/**
 * Macro for scanner parameter file
 */
#define DB_MNGMNT_FOLDER "/DBmngmnt"


/**
 * Macro for image quality bag
 */
#define IMAGE_QUALITY_BAG "/ImageQuality"

/**
 * Macro for image quality report
 */
#define IMAGE_QUALITY_REPORT "/IQR"

/**
 * Macro for xml file name
 */
#define CONFIG_XML_FILE_NAME "workstation.xml"

/**
 * Macro for xsd file name
 */
#define CONFIG_XSD_FILE_NAME "workstation.xsd"

/**
 * Macro for training workstation xml file name
 */
#define EMULATOR_CONFIG_XML_FILE_NAME "emulator.xml"

/**
 * Macro for training workstation xsd file name
 */
#define EMULATOR_CONFIG_XSD_FILE_NAME "emulator.xsd"

/**
 * Macro for training workstation xml file name
 */
#define SIMULATOR_CONFIG_XML_FILE_NAME "simulator.xml"

/**
 * Macro for training workstation xsd file name
 */
#define SIMULATOR_CONFIG_XSD_FILE_NAME "simulator.xsd"

/**
 * Macro for info file path
 */
#define DEFAULT_INFO_FILEPATH ""
/**
 * Macro for Workstation version file path
 */
#define DEFAULT_WSVERSION_FILEPATH "../resources/viewing_station_version.txt"
/**
 * Macro for OS version file path
 */
#define DEFAULT_OSVERSION_FILEPATH "/etc/analogic_os_version.txt"
/**
 * Macro for Mount path
 */
#define DEFAULT_MOUNTPATH "/media"
/**
 * Macro for Mount path
 */
#define DEFAULT_DATA_PATH "/home/analogic"
/**
 * Macro for Logo Path
 */
#define DEFAULT_LOGOPATH "ancp00/Logo/analogic.png"
/**
 * Macro for Default language
 */
#define DEFAULT_LANGUAGE "English"
/**
 * Macro for Default language
 */
#define DEFAULT_SSI_BANNER false


/*!
 * \class   WorkstationConfig
 * \brief   This class contains variable and function related to
 *          managing configurations of system.
 */
class WorkstationConfig : public QObject
{
  Q_OBJECT
public:
  /*!
    * @fn       getInstance
    * @param    None
    * @return   WorkstationConfig
    * @brief    returns Instance of WorkstationConfig
    */
  static WorkstationConfig *getInstance();

  /*!
    * @fn       validateAndParseXml
    * @param    QString& error - return error msg
    * @param    const QString - xml file
    * @param    const QString - xsd file
    * @return   bool
    * @brief    parse xml file
    */
  bool validateAndParseXml(QString& error, const QString xmlfile, const QString xsdfile);

  /*!
    * @fn       validateAndParseIni
    * @param    const std::string - ini file
    * @return   bool
    * @brief    parse ini file
    */
  bool validateAndParseIni(const std::string ini_file);

  /*!
    * @fn       initXml
    * @param    None
    * @return   None
    * @brief    initialization for XML configuration.
    */
  void initXml();

  /*!
    * @fn       initIni
    * @param    None
    * @return   None
    * @brief    initialization for ini configuration.
    */
  void initIni();

  /*!
    * @fn       workstationVersion
    * @param    None
    * @return   QString
    * @brief    gets VersionNumber
    */
  QString workstationVersion();

  /*!
    * @fn       OSVersion
    * @param    None
    * @return   QString
    * @brief    get OSVersionNumber
    */
  QString OSVersion();
#ifdef WORKSTATION
  /*!
    * @fn       getscanneradminseveripaddress
    * @param    None
    * @return   string- ipadress
    * @brief    gets scanner admin sever ip address.
    */
  QString getscanneradminseveripaddress();

  /*!
    * @fn       getscanneradminserverportno
    * @param    None
    * @return   QString- portno
    * @brief    gets scanner admin server portno.
    */
  QString getscanneradminserverportno();

  /*!
    * @fn       getpassphrase
    * @param    None
    * @return   QString- password
    * @brief    gets pass phrase which will use to decrypt certificate.
    */
  QString getpassphrase();

  /*!
    * @fn       getsslauth
    * @param    None
    * @return   bool- server auth
    * @brief    to use server auth
    */
  bool getsslauth();

  /*!
    * @fn       getserverpassphrase
    * @param    None
    * @return   string- server password
    * @brief    gets server side pass phrase which will use to decrypt certificate.
    */
  QString getserverpassphrase();

  /*!
    * @fn       getservercertificate
    * @param    None
    * @return   string- server certificate
    * @brief    gets server side certificate.
    */
  QString getservercertificate();

  /*!
  * @fn       getnssservercertificate
  * @param    None
  * @return   string- certifcate
  * @brief    gets nss server certificate
  */
  QString getnssservercertificate();

#endif
  /*!
    * @fn       getinactivitytimeout
    * @param    None
    * @return   int - timeout
    * @brief    time out in case on keyboard or mouse inactivity
    */
  int getinactivitytimeout();

  /*!
    * @fn       getNssipaddress
    * @param    None
    * @return   QString - ip
    * @brief    config for nss server
    */
  QString getNssipaddress();

  /*!
    * @fn       getWorkstationLogoPath
    * @param    None
    * @return   QString - WorkstationLogoPath
    * @brief    config for workstation Logo Path
    */
  Q_INVOKABLE QString getWorkstationLogoPath();

  /*!
    * @fn       getCurrentLanguage
    * @param    None
    * @return   bool
    * @brief    Getting Current Language
    */
  Q_INVOKABLE QString getCurrentLanuage();

  /*!
    * @fn       getLanguageCode
    * @param    None
    * @return   string  - Language
    * @brief    gets Language Code
    */
  Q_INVOKABLE QString getLanguageCode();

  /*!
    * @fn       getCurrentLanguageLocale
    * @param    None
    * @return   string  - Language
    * @brief    gets Language locale
    */
  QString getCurrentLanguageLocale();

  /*!
    * @fn       setNssipaddress
    * @param    const QString - ip address
    * @return   None
    * @brief    config for scanner bag data server
    */
  void setNssipaddress(const QString &nssipaddress);

  /*!
    * @fn       setWorkstationName
    * @param    const QString - workstationName
    * @return   None
    * @brief    config for workstation name
    */
  void setWorkstationName(const QString &workstationName);

  /*!
    * @fn       getWorkstationName
    * @param    None
    * @return   QString - WorkstationName
    * @brief    config for workstation name
    */
  Q_INVOKABLE QString getWorkstationName();

  /*!
    * @fn       getWorkstationNameEnum
    * @param    None
    * @return   QMLEnums::WSType
    * @brief    config for workstation name
    */
  QMLEnums::WSType getWorkstationNameEnum();

  /*!
    * @fn       getNssportno
    * @param    None
    * @return   QString - port
    * @brief    config for scanner nss server port
    */
  QString getNssportno();

  /*!
    * @fn       setNssportno
    * @param    const QString - port
    * @return   None
    * @brief    config for nss server port
    */
  void setNssportno(const QString &nssportno);

  /*!
    * @fn       getNSSServerCheckInterval
    * @param    None
    * @return   int - timeout
    * @brief    time interval to check NSS server availaibility
    */
  int getNSSServerCheckInterval();

  /*!
    * @fn       getAutoArchiveEnabled
    * @param    None
    * @return   int - timeout
    * @brief    sets auto archive enabled/ disabled.
    */
  bool getAutoArchiveEnabled();

  /*!
    * @fn       getScannername
    * @param    None
    * @return   String - scannername
    * @brief    config for scanner name
    */
  QString getScannername();

  /*!
    * @fn       setScannername
    * @param    const QString &scannername
    * @return   None
    * @brief    config for scanner name
    */
  void setScannername(const QString &scannername);

  /**
     * @brief getCanUserClearAlarmBag
     * @return bool
     * @brief Get status of if user can clear alarm bag
   */
  Q_INVOKABLE bool getCanUserClearAlarmBag();


  /**
     * @brief getEnableThreathandling
     * @return
     * Get status of threat view Enable/Disable
     */
  bool getEnableThreathandling();

  /**
     * @brief setEnableThreatHandling
     * @param setVal
     * Enable/Disable the viewing of threats
     */
  void setEnableThreatHandling(bool setVal);

  /**
     * @brief getEnableVtkSharpenSplash
     * @return
     * Get status of sharpened VTK Splash Screen
     */
  bool getEnableVtkSharpenSplash();

  /**
     * @brief setEnableVtkSharpenSplash
     * @param doSharpen
     * Set status of sharpened VTK Splash Screen.
     */
  void setEnableVtkSharpenSplash(bool doSharpen);

  /**
     * @brief setEnableVtkDynamicSharpenSplash
     * @param doSharpen
     */
  void setEnableVtkDynamicSharpenSplash(bool doSharpen);

  /**
     * @brief getEnableVtkDynamicSharpenSplash
     * @return
     */
  bool getEnableVtkDynamicSharpenSplash();

  /*!
    * @fn       getWorkstationDisplayUnitSystem
    * @param    None
    * @return   VREnums::DispalyUnitSystemEnum
    * @brief    config for Workstation Display Unit System
    */
  VREnums::DispalyUnitSystemEnum getWorkstationDisplayUnitSystem();


  /*!
    * @fn       setWorkstationDisplayUnitSystem
    * @param    QString unitSystemStr
    * @return   None
    * @brief    set config for Workstation Display Unit System
    */
  void setWorkstationDisplayUnitSystem(QString unitSystemStr);

  /*!
    * @fn       getUseShadingforMainViewer
    * @param    None
    * @return   bool
    * @brief    config for Shading for MainViewer
    */
  bool getUseShadingforMainViewer();

  /*!
    * @fn       setUseShadingforMainViewer
    * @param    bool
    * @return   None
    * @brief    config for Shading for MainViewer
    */
  void setUseShadingforMainViewer(bool useShading);

  /*!
    * @fn       getUseShadingforRightViewer
    * @param    None
    * @return   bool
    * @brief    config for Shading for Right MainViewer
    */
  bool getUseShadingforRightViewer();

  /*!
    * @fn       setUseShadingforRightViewer
    * @param    bool useShading
    * @return   None
    * @brief    config for Shading for Right MainViewer
    */
  void setUseShadingforRightViewer(bool useShading);

  /**
     * @brief getTranslucentLinearInterpolation
     * @return
     */
  bool getTranslucentLinearInterpolation();

  /**
     * @brief setTranslucentLinearInterpolation
     * @param doLinear
     */
  void setTranslucentLinearInterpolation(bool doLinear);

  /**
     * @brief getSurfaceLinearInterpolation
     * @return
     */
  bool getSurfaceLinearInterpolation();

  /**
     * @brief setSurfaceLinearInterpolation
     * @param doLinear
     */
  void setSurfaceLinearInterpolation(bool doLinear);

  /*!
    * @brief getOrganicRemovableForRightViewer
    * @return if organics can be removed from right viewer
    */
  bool getOrganicRemovableForRightViewer();

  /*!
    * @brief setOrganicRemovableForRightViewer
    * @param enable/disble organic removal from right viewer
    */
  void setOrganicRemovableForRightViewer(bool organicRemovable);

  /**
     * @brief getInorganicRemovableForRightViewer
     * @return if in-organics can be removed from right viewer
     */
  bool getInorganicRemovableForRightViewer();

  /**
     * @brief setInorganicRemovableForRightViewer
     * @param enable/disable inorganic removal from right viewer
     */
  void setInorganicRemovableForRightViewer(bool inorganicRemovable);

  /**
     * @brief getMetalRemovableForRightViewer
     * @return bool is metal removable in surface viewer
     */
  bool getMetalRemovableForRightViewer();

  /**
     * @brief setMetalRemovableForRightViewer
     * @param enable/disable metal removal in right viewer
     */
  void setMetalRemovableForRightViewer(bool enableMetalremoval);

  /*!
    * @brief getOrganicRemovableForLeftViewer
    * @return if organics can be removed from left viewer
    */
  bool getOrganicRemovableForLeftViewer();

  /*!
    * @brief setOrganicRemovableForLeftViewer
    * @param enable/disable organic removal from left viewer
    */
  void setOrganicRemovableForLeftViewer(bool organicRemovable);

  /**
     * @brief getInorganicRemovableForLeftViewer
     * @return if in-organics can be removed from Left viewer
     */
  bool getInorganicRemovableForLeftViewer();

  /**
     * @brief setInorganicRemovableForLeftViewer
     * @param enable/disable inorganic removal from Left viewer
     */
  void setInorganicRemovableForLeftViewer(bool inorganicRemovable);

  /**
     * @brief getMetalRemovableForLeftViewer
     * @return bool is metal removable in left viewer
     */
  bool getMetalRemovableForLeftViewer();

  /**
     * @brief setMetalRemovableForLeftViewer
     * @param enable/disable metal removal in left viewer
     */
  void setMetalRemovableForLeftViewer(bool enableMetalremoval);

  /**
     * @brief getTranslucentAmbientLighting
     * @return
     */
  double getTranslucentAmbientLighting();

  /**
     * @brief setTranslucentAmbientLighting
     * @param ambientLight
     */
  void setTranslucentAmbientLighting(double ambientLight);

  /**
     * @brief getTranslucentDiffuseLighting
     * @return
     */
  double getTranslucentDiffuseLighting();

  /**
     * @brief setTranslucentDiffuseLighting
     * @param diffuseLight
     */
  void setTranslucentDiffuseLighting(double diffuseLight);

  /**
     * @brief getTranslucentSpecularLighting
     * @return
     */
  double getTranslucentSpecularLighting();

  /**
     * @brief setTranslucentSpecularLighting
     * @param specularLight
     */
  void setTranslucentSpecularLighting(double specularLight);

  /**
     * @brief getSurfaceAmbientLighting
     * @return
     */
  double getSurfaceAmbientLighting();

  /**
     * @brief setSurfaceAmbientLighting
     * @param ambientLight
     */
  void setSurfaceAmbientLighting(double ambientLight);

  /**
     * @brief getSurfaceDiffuseLighting
     * @return
     */
  double getSurfaceDiffuseLighting();

  /**
     * @brief setSurfaceDiffuseLighting
     * @param diffuseLight
     */
  void setSurfaceDiffuseLighting(double diffuseLight);

  /**
     * @brief getSurfaceSpecularLighting
     * @return
     */
  double getSurfaceSpecularLighting();

  /**
     * @brief setSurfaceSpecularLighting
     * @param specularLight
     */
  void setSurfaceSpecularLighting(double specularLight);


  //------------------------------------
  // Volume measurement start point

  /**
     * @brief getVolumeToolStartMin
     * @return Volume measurrement tool minimum start point.
     */
  uint16_t getVolumeToolStartMin();

  /**
     * @brief getVolumeToolStartMax
     * @return Volume measurrement tool maximum start point.
     */
  uint16_t getVolumeToolStartMax();

  /**
     * @brief setVolumeToolStartMin
     * @param setVal
     */
  void setVolumeToolStartMin(uint16_t setVal);

  /**
     * @brief setVolumeToolStartMax
     * @param setVal
     */
  void setVolumeToolStartMax(uint16_t setVal);

  //------------------------------------

  /**
     * @brief getScannerDirectionLeftToRight
     * See if the scanner is left to right for bag movement
     */
  bool getScannerDirectionLeftToRight();

  /**
     * @brief setScannerDirectionLeftToRight
     * @param dirLeftToRight
     * Set the scanner as left to right in bag movement
     */
  void setScannerDirectionLeftToRight(bool dirLeftToRight);

  /*!
    * @fn       getExportBagDataPath
    * @param    None
    * @return   QString - path
    * @brief    Folder location where exported bad data would be saved.
    */
  QString getExportBagDataPath();

  /*!
    * @fn       setExportBagDataPath
    * @param    None
    * @return   QString - path
    * @brief    Folder location where exported bad data would be saved.
    */
  void setExportBagDataPath(const QString& in);

  /*!
    * @fn       getExportBagDataMaxCount
    * @param    None
    * @return   int - max count
    * @brief    Maximum number of bags that should be saved locally.
    */
  int getExportBagDataMaxCount();

  /*!
    * @fn       getWorkstationMachineId
    * @param    None
    * @return   QString - Id
    * @brief    gets workstation machine ID
    */
  Q_INVOKABLE QString getWorkstationMachineId();

  /*!
    * @fn       setWorkstationMachineId
    * @param    QString - Id
    * @return   None
    * @brief    sets workstation machine ID
    */
  void setWorkstationMachineId(const QString &workstationMachineId);

  /*!
    * @fn       setexecutablePath
    * @param    std::string path
    * @return   None
    * @brief    sets the executable dir string
    */
  void setexecutablePath(std::string path);

  /*!
    * @fn       getexecutablePath
    * @param    None
    * @return   std::string
    * @brief    gets the executable dir string
    */
  std::string getexecutablePath();
#ifdef WORKSTATION
  /*!
    * @fn       getSDSWClientBinarypath
    * @param    None
    * @return   QString - path with filename
    * @brief    gets SDSW client binary path with binary. Example /opt/SDSWClient/dist/SDSWClient
    */
  QString getSDSWClientBinarypath();

  /*!
    * @fn       setSDSWClientBinarypath
    * @param    QString - path with filename
    * @return   None
    * @brief    sets SDSW client binary path with binary. Example /opt/SDSWClient/dist/SDSWClient
    */
  void setSDSWClientBinarypath(const QString &workstationMachineId);

#endif
  /*!
    * @fn       getuseradminseveripaddress
    * @param    None
    * @return   string- ipadress
    * @brief    gets user admin sever ip address.
    */
  QString getuseradminseveripaddress();

  /*!
    * @fn       getuseradminserverportno
    * @param    None
    * @return   QString- portno
    * @brief    gets user admin server portno.
    */
  QString getuseradminserverportno();

  /*!
    * @fn       getreportadminseveripaddress
    * @param    None
    * @return   string- ipadress
    * @brief    gets report admin sever ip address.
    */
  QString getreportadminseveripaddress();

  /*!
    * @fn       getreportadminserverportno
    * @param    None
    * @return   QString- portno
    * @brief    gets report admin server portno.
    */
  QString getreportadminserverportno();

  /*!
    * @fn       getantivirusseveripaddress
    * @param    None
    * @return   string- ipadress
    * @brief    gets antivirus sever ip address.
    */
  QString getantivirusseveripaddress();

  /*!
    * @fn       getantivirusserverportno
    * @param    None
    * @return   QString- portno
    * @brief    gets antivirus server portno.
    */
  QString getantivirusserverportno();
  /*!
  * @fn       getSupervisorSeverIPAddress
  * @param    None
  * @return   string- ipadress
  * @brief    gets user supervisor sever ip address.
  */
  QString getSupervisorSeverIPAddress();

  /*!
   * @fn       getSupervisorSeverPortNo
   * @param    None
   * @return   QString- portno
   * @brief    gets supervisor server portno.
   */
  QString getSupervisorSeverPortNo();
  /*!
    * @fn       getOperatorCompany
    * @param    None
    * @return   QString
    * @brief    gets operator organization name
    */
  QString getOperatorCompany();
  /*!
    * @fn       setOperatorCompany
    * @param    QString
    * @return   none
    * @brief    sets operator organization name
    */
  void setOperatorCompany(QString name);

  /*!
    * @fn       getOsVersion
    * @param    None
    * @return   QString-
    * @brief    get OsVersion
    */
  Q_INVOKABLE QString getOsVersion();

  /*!
    * @fn       getWsVersion
    * @param    None
    * @return   QString-
    * @brief    gets workstationVersion
    */
  Q_INVOKABLE QString getWsVersion();
  // TODO Revisit when separating rerun and training#endif

  /*!
    * @fn       getIQbagfileformat
    * @param    None
    * @return   String - Image Quality bagfileformat
    * @brief    config for Image Quality bagfileformat
    */
  Q_INVOKABLE QString getIQbagfileformat();

  /*!
    * @fn       getIQreportfileformat
    * @param    None
    * @return   String - Image Quality reportfileformat
    * @brief    config for Image Quality reportfileformat
    */
  Q_INVOKABLE QString getIQreportfileformat();

  /*!
    * @fn       getIQNistfileformat
    * @param    None
    * @return   String - Image Quality Nist file format
    * @brief    config for Image Quality Nist file format
    */
  Q_INVOKABLE QString getIQNistfileformat();

  /*!
    * @fn       getBagDataResolution
    * @param    None
    * @return   QSizeF
    * @brief    gets bag data resolution
    */
  QSizeF getBagDataResolution();

  int get_max_slices(){ return m_max_slices; }

  /*!
    * @fn       getBagTransferTimeoutInSec
    * @param    None
    * @return   int
    * @brief    gets bag transfer timeout which needs to be set to the Alog transfer library
    */
  int getBagTransferTimeoutInSec(){ return m_bagTransferTimeoutInSec; }

  /*!
    * @fn       setBagTransferTimeoutInSec
    * @param    None
    * @return   int
    * @brief    gets bag transfer timeout which needs to be set to the Alog transfer library
    */
  void setBagTransferTimeoutInSec(int bagTransferTimeout){ m_bagTransferTimeoutInSec = bagTransferTimeout; }


  void log_params();
  /*!
    * @fn       getScannerAdminAsyncTaskRefreshInterval
    * @param    None
    * @return   int - timeout
    * @brief    scanner admin service asynchronous task interval.
    */
  int getScannerAdminAsyncTaskRefreshInterval();

  /*!
    * @fn       getScannerAdminAsyncTaskTimeoutInterval
    * @param    None
    * @return   int - timeout
    * @brief    scanner admin service asynchronous task timeout interval.
    */
  int getScannerAdminAsyncTaskTimeoutInterval();

  /*!
    * @fn       getNetworkReconnectTime
    * @param    None
    * @return   int- Reconnect Time interval
    * @brief    gets reconnect time interval.
    */
  int getNetworkReconnectTime();

  /*!
    * @fn       getNetworkTimeout
    * @param    None
    * @return   int- Network Timeout
    * @brief    gets Network Timeout.
    */
  Q_INVOKABLE int getNetworkTimeout();

  /*!
    * @fn       getScannerConnectionCheckInterval
    * @param    None
    * @return   int - timeout
    * @brief    time interval to check scanner availability
    */
  int getScannerConnectionCheckInterval();

  /*!
    * @fn       getSoapClientEndPoint
    * @param    None
    * @return   std::string soap client end point
    * @brief    gets soap client end point
    */
  std::string getSoapClientEndPoint();

  /*!
    * @fn       getSoapServerPort
    * @param    None
    * @return   std::string soap server port
    * @brief    gets soap server port
    */
  uint16_t getSoapServerPort();

  /*!
    * @fn       getSwsBhsServerIpAddr
    * @param    None
    * @return   std::string sws bhs server ip address
    * @brief    gets sws bhs server ip address
    */
  std::string getSwsBhsServerIpAddr();

  /*!
    * @fn       getSwsBhsServerPort
    * @param    None
    * @return   uint16_t sws bhs server port
    * @brief    gets sws bhs server port
    */
  uint16_t getSwsBhsServerPort();

  /*!
    * @fn       getRfidReaderIpAddr
    * @param    None
    * @return   std::string rfid reader ip address
    * @brief    gets rfid reader ip address
    */
  std::string getRfidReaderIpAddr();

  /*!
    * @fn       getRfidReaderPort
    * @param    None
    * @return   uint16_t rfid reader port
    * @brief    gets rfid reader port
    */
  uint16_t getRfidReaderPort();

  /*!
    * @fn       getBhsType
    * @param    None
    * @return   QString - BHS type
    * @brief    config for BHS type
    */
  std::string getBhsType();

  /*!
    * @fn       getBhsBinType
    * @param    None
    * @return   QString - BHS bin type
    * @brief    config for BHS bin type
    */
  std::string getBhsBinType();

  /**
     * @brief getNoofBagPreprossingThreads
     * @return int
     * @brief get no of bag proccessing thread
     */
  int getNoofBagPreprocessingThreads();

  /**
     * @brief setNoofBagPreprossingThreads
     * @return int
     * @brief set no of bag proccessing thread
     */
  void setNoofBagPreprocessingThreads(int NoofBagPreprossingThreads);

  /**
     * @brief getMinimumRequiredSliceCount
     * @return int
     * @brief get number slice required to enable decision
     */
  int getMinimumRequiredSliceCount();

  /**
     * @brief setMinimumRequiredSliceCount
     * @return int
     * @brief set number slice required to enable decision
     */
  void setMinimumRequiredSliceCount(int minimumRequiredSliceCount);


  /**
     * @brief getBagDecisionTimeout
     * @return int
     * @brief get user decision timeout
     */
  Q_INVOKABLE int getBagDecisionTimeout();

  /**
     * @brief showDialogForUserDecisionTimeout
     * @return bool
     * @brief is popup needed for user decision timeout
     */
  Q_INVOKABLE bool showDialogForUserDecisionTimeout();

  /**
     * @brief getSlabbingThickness
     * @return
     */
  double getSlabbingThickness();

  /*!
    * @fn       updateWorkstationConfig
    * @param    ConfigBaseElement config
    * @return   None
    * @brief    update Workstation Config
    */
  void updateWorkstationConfig(ConfigBaseElement config);

  /*!
      * @fn       getWorkstationManufactId
      * @param    None
      * @return   QString
      * @brief    gets workstation Manufacturer Id
      */
  Q_INVOKABLE QString getWorkstationManufactId();

  /*!
    * @fn       getManufacturerModelNumber
    * @param    None
    * @return   QString
    * @brief    gets workstation Manufacturer model number
    */
  QString getManufacturerModelNumber();

  /*!
    * @fn       getWorkstationSerialNumber
    * @param    None
    * @return   QString
    * @brief    gets workstation serial number
    */
  QString getWorkstationSerialNumber();

  /*!
    * @fn       getConfigxmlfilename
    * @param    None
    * @return   QString
    * @brief    gets config xml file name
    */
  QString getConfigxmlfilename();

  /*!
    * @fn       setConfigxmlfilename
    * @param    QString
    * @return   None
    * @brief    sets config xml file name
    */
  void setConfigxmlfilename(const QString &configxmlfilename);

  /*!
    * @fn       getConfigxsdfilename
    * @param    None
    * @return   QString
    * @brief    gets config xsd file name
    */
  QString getConfigxsdfilename();

  /*!
    * @fn       setConfigxsdfilename
    * @param    QString
    * @return   None
    * @brief    sets config xsd file name
    */
  void setConfigxsdfilename(const QString &configxsdfilename);

  /*!
    * @fn       getConfigInifilename
    * @param    None
    * @return   QString
    * @brief    gets config ini file name
    */
  QString getConfigInifilename();

  /*!
    * @fn       setMountPath
    * @param    std::string mountpath
    * @return   void
    * @brief    return mountpath
    */
  void setMountPath(std::string mountpath);

  /*!
    * @fn       getUsbMountPath
    * @param    None
    * @return   QString
    * @brief    return mountpath
    */
  Q_INVOKABLE QString getUsbMountPath();

  /*!
    * @fn       setDefaultDataPath
    * @param    std::string defaultdatapath
    * @return   void
    * @brief    None
    */
  void setDefaultDataPath(std::string defaultmountpath);

  /*!
    * @fn       getDefaultDataPath
    * @param    None
    * @return   std::string
    * @brief    return defaultdatapath
    */
  Q_INVOKABLE QString getDefaultDataPath();

  /*!
    * @fn       setConfigInifilename
    * @param    QString
    * @return   None
    * @brief    sets config ini file name
    */
  void setConfigInifilename(const QString &configinifilename);

  /*!
    * @fn       getInfoFilePath
    * @param    None
    * @return   QString
    * @brief    gets info file path from ini
    */
  Q_INVOKABLE QString getInfoFilePath();

  /*!
    * @fn       getOrganizationName
    * @param    None
    * @return   QString
    * @brief    gets workstation organization name
    */
  QString getOrganizationName();

  /*!
    * @fn       getMachineLocation
    * @param    None
    * @return   QString
    * @brief    gets workstation Machie location
    */
  QString getMachineLocation();

  /*!
    * @fn       getMachineAddress
    * @param    None
    * @return   QString
    * @brief    gets workstation Machine address
    */
  QString getMachineAddress();

  /*!
    * @fn       isBagPickupEnabled
    * @param    None
    * @return   bool
    * @brief    gets pickup enable satus
    */
  Q_INVOKABLE bool isBagPickupEnabled();

  /*!
    * @fn       isStopTheCheckVisible
    * @param    None
    * @return   bool
    * @brief    Enable/Disable it to show/Hide
    */
  Q_INVOKABLE bool isStopTheCheckVisible();

  /*!
    * @fn       isDisplayOperatorClearedAlarmEnabled
    * @param    None
    * @return   bool
    * @brief    Enable/Disable it to show/Hide User Decision on SVS image overlay
    */
  bool isDisplayOperatorClearedAlarmEnabled();

  /*!
    * @fn       getSearchDefaultRightviewModeType
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    gets default mode type
    */
  VREnums::VRViewModeEnum getSearchDefaultRightviewModeType();

  /*!
    * @fn       getRecallDefaultRightviewModeType
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    gets default mode type
    */
  VREnums::VRViewModeEnum getRecallDefaultRightviewModeType();

  /*!
    * @fn       getTipConfigIpAddress
    * @param    None
    * @return   QString - ip
    * @brief    tip config server
    */
  QString getTipConfigIpAddress();

  /*!
    * @fn       getTipConfigPortNo
    * @param    None
    * @return   QString - port
    * @brief    Tip config server port
    */
  QString getTipConfigPortNo();

  /*!
    * @fn       getTipLibConfigIpAddress
    * @param    None
    * @return   QString - ip
    * @brief    tip lib server
    */
  QString getTipLibConfigIpAddress();

  /*!
    * @fn       getTipLibConfigPortNo
    * @param    None
    * @return   QString - port
    * @brief    Tip lib server port
    */
  QString getTipLibConfigPortNo();

  /*!
    * @fn       getTipInitialDecisionTimeout
    * @param    None
    * @return   int - timeout value
    * @brief    Tip Initial Decision Timeout value
    */
  Q_INVOKABLE int getTipInitialDecisionTimeout();

  /*!
    * @fn       getTipAnalysisTimeout
    * @param    None
    * @return   int - timeout value
    * @brief    Tip Analysis Timeout value
    */
  Q_INVOKABLE int getTipAnalysisTimeout();

  /*!
    * @fn       getTipHITMsgDialog
    * @param    None
    * @return   QString - message
    * @brief    Tip message for user decision
    */
  QString getTipHITMsgDialog();

  /*!
    * @fn       setTipHITMsgDialog
    * @param    QString - message
    * @return
    * @brief    set Tip message for user decision
    */
  void setTipHITMsgDialog(QString msg);

  /*!
    * @fn       getTipMISSMsgDialog
    * @param    None
    * @return   QString - message
    * @brief    Tip message for user decision
    */
  QString getTipMISSMsgDialog();

  /*!
    * @fn       setTipMISSMsgDialog
    * @param    QString - message
    * @return   None
    * @brief    set Tip message for user decision
    */
  void setTipMISSMsgDialog(QString msg);

  /*!
    * @fn       getTipNONTIPMsgDialg
    * @param    None
    * @return   QString - message
    * @brief    Tip message for user decision
    */
  QString getTipNONTIPMsgDialog();

  /*!
    * @fn       setTipNONTIPMsgDialog
    * @param    QString - message
    * @return   None
    * @brief    Tset ip message for user decision
    */
  void setTipNONTIPMsgDialog(QString msg);

  /*!
    * @fn       getHighThreatUpperText
    * @param    None
    * @return   QString
    * @brief    get high threat upper text value
    */
  Q_INVOKABLE QString getHighThreatUpperText();

  /*!
    * @fn       getHighThreatLowerText
    * @param    None
    * @return   QString
    * @brief    get high threat lower text value
    */
  Q_INVOKABLE QString getHighThreatLowerText();

  /*!
    * @fn       getNonTipDialogEnable
    * @param    None
    * @return   bool
    * @brief    get non tip enable text value
    */
  bool getNonTipDialogEnable();

  /*!
    * @fn       updateProperty
    * @param    QString name
    * @param    QString value
    * @param    QString type
    * @return   None
    * @brief    update properties in map
    */
  Q_INVOKABLE static void updateProperty(QString name, QString value , QString type);

  /*!
    * @fn       setDataPath
    * @param    QString
    * @return   None
    * @brief    set data path
    */
  void setDataPath(const QString dataPath);

  /*!
    * @fn       getDataPath
    * @param    None
    * @return   QString
    * @brief    get data path
    */
  QString getDataPath();

  /*!
    * @fn       clearWSConfigMap
    * @param    None
    * @return   None
    * @brief    clear map data
    */
  void clearWSConfigMap();

  /*!
    * @fn       getBhsNotificationTimeout
    * @param    None
    * @return   int
    * @brief    return bhs bag notification popup timeout.
    */
  Q_INVOKABLE int getBhsNotificationTimeout();

  /*!
    * @fn       getAuthenticateWithNss
    * @param    None
    * @return   bool
    * @brief    return SDSWCLient needs to be authenticated with NSS.
    */
  Q_INVOKABLE bool getAuthenticateWithNss() const;

  /*!
    * @brief showMachineAlarmDecision
    * @return bool
    * @brief is popup needed for machine alarm visiblity decision
    */
  bool showMachineAlarmDecision();

  /*!
    * @fn       getssibanner
    * @param    None
    * @return   bool- banner stauts
    * @brief    gets ssi banner status flag*/
  Q_INVOKABLE bool getssibanner();

protected:
  static QMap<QString, QVariant> m_WsConfigMap;

  /*!
    * @fn       WorkstationConfig
    * @param    None
    * @return   None
    * @brief    Constructor for class WorkstationConfig.
    */
  WorkstationConfig();

  /*!
    * @fn       ~WorkstationConfig
    * @param    None
    * @return   None
    * @brief    Destructor for class WorkstationConfig.
    */
  virtual ~WorkstationConfig();

private:
  /*!
    * @fn       WorkstationConfig();
    * @param    WorkstationConfig&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
  WorkstationConfig(const WorkstationConfig& workstationconfig);

  /*!
    * @fn       operator=
    * @param    WorkstationConfig&
    * @return   WorkstationConfig&
    * @brief    declaration for private assignment operator.
    */
  WorkstationConfig& operator=(const WorkstationConfig& workstationconfig);

  int            m_nssCheckInterval;                 //!< NSS check interval
  int            m_scannerCheckInterval;             //!< scanner connection check interval
  int            m_networkreconnecttimer;            //!< Timer For Reconnect With Server.
  int            m_networktimeout;                   //!< Timeout With Server.
  int            m_scanneradminasynctaskrefreshinterval;  //!< scanner admin async task refresh interval.
  int            m_scanneradminasynctasktimeoutinterval;  //!< scanner admin async task timeout interval.
  int            m_NoofBagPreprocessingThreads;      //!< specify no of bag processing thread
  int            m_minimumRequiredSliceCount;        //!< minimum required slice count to apply decision.
  int            m_max_slices;                       //!< max # of slices
  int            m_bagTransferTimeoutInSec;          //!< Workstation bag timeout in sec for the Alog transfer library
  bool           m_useshadingformainviewer;          //!< Shading for main flag
  bool           m_useshadingforrightviewer;         //!< Shading for right viewer flag
  bool           m_traslucentInterpolationLinear;    //!< Use linear interpolation for translucent viewer.
  //!< options: {Linear / Nearest}
  bool           m_surfaceInterpolationLinear;       //!< Use linear interpolation for surface viewer.
  //!< options: {Linear / Nearest}
  bool           m_surfaceOrganicRemovable;          //!< Allow Organic to be removed from surface view
  bool           m_surfaceInorganicRemovable;        //!< Allow Inorganic to be removed from surface view
  bool           m_surfaceMetalRemovable;            //!< Allow Metal to be removed from surface view
  bool           m_translucentOrganicRemovable;      //!< Allow Organic to be removed from translucent view
  bool           m_translucentInorganicRemovable;    //!< Allow Inorganic to be removed from translucent view
  bool           m_translucentMetalRemovable;        //!< Allow Metal to be removed from translucent view
  bool           m_enableThreatHandling;             //!< Allow Threat Handling
  bool           m_authenticateWithNss;              //!< Allow SDSWClient Authentication with nss
  double         m_translucentAmbientLighting;       //!< Set ambient lighting for the translucent viewer
  double         m_translucentDiffuseLighting;       //!< Set diffuse lighting for the translucent viewer
  double         m_translucentSpecularLighting;      //!< Set specular lighting for the translucent viewer
  double         m_surfaceAmbientLighting;           //!< Set ambient lighting for the surface viewer
  double         m_surfaceDiffuseLighting;           //!< Set diffuse lighting for the surface viewer
  double         m_surfaceSpecularLighting;          //!< Set specular lighting for the surface viewer
  uint16_t       m_volumeMeasuremtStartMin;          //!< Volume Measurent start minimum
  uint16_t       m_volumeMeasuremtStartMax;          //!< Volume Measurent start maximum
  QSizeF         m_bagdataresolution;                //!< ImageQuality Report File format
  QString        m_workstationName;                  //!< Workstation name / type
  QString        m_strunzipcommand;                  //!< Unzip command string
  QString        m_serviceclientbinarypath;          //!< service tool binary path
  QString        m_iqBagFileFormat;                  //!< ImageQuality Bag File format
  QString        m_iqReportFileFormat;               //!< ImageQuality Report File format
  QString        m_iqNistFileFormat;                 //!< ImageQuality Nist Report File format
  QString        m_configxmlfilename;                //!< Config xml file name
  QString        m_configxsdfilename;                //!< Config xsd file name
  QString        m_configinifilename;                //!< Config ini file name
  QString        m_workstationVersion;               //!< workstation version
  QString        m_OSVersion;                        //!< OS version
  QString        m_WSVersionFilePath;                //!< workstation version file path
  QString        m_OSVersionFilePath;                //!< OS version file path
  QString        m_infoFilePath;                     //!< software information file path
  QString        m_workstationLogoPath;              //!< logo path
  QString        m_tipHITMsg;                        //!< tip hit message
  QString        m_tipMISSMsg;                       //!< tip miss message
  QString        m_tipNonTIPMsg;                     //!< tip miss message
  QString        m_dataPath;                         //!< Data Path
  QString        m_currentLanguage;                  //!< Current language
  bool           m_strsslauth;                       //!< SSL auth
  QString        m_strserverpassphrase;              //!< Server passphrase
  QString        m_strservercertificate;             //!< SDSW Server Certifcate
  QString        m_strnssservercertificate;          //!< NSS Server Certifcate
  QString        m_operatorCompany;                  //!< Operator company from nss
  std::string    m_mountPath;                        //!< mountpath of usb
  std::string    m_defaultdatapath;                  //!< default mountpath
  std::string    m_executablePath;                   //!< Workstation executable path
  bool           m_ssibanner;                        //!< SSI Banner
  VREnums::DispalyUnitEnum m_displayUnuits;          //!< Display Units for Volume
  VREnums::DispalyUnitSystemEnum m_workstationDisplayUnitSystem;   //!< Workstation Display Unit System
  static WorkstationConfig     *m_workstationConfig;               //!< Holds Instance of Workstation Config
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_WSCONFIGURATION_WORKSTATIONCONFIG_H_
