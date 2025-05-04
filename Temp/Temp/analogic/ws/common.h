/*!
* @file     common.h
* @author   Agiliad
* @brief    This file contains functions related to all classes.
* @date     Sep, 29 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_H_
#define ANALOGIC_WS_COMMON_H_


#include <Logger.h>
#include <SDICOS/SdcsTDRSeries.h>
#include <SDICOS/SdcsAssessmentSequence.h>
#include <SDICOS/SdcsThreatDetectionReport.h>
#include <semaphore.h>
#include <frameworkcommon.h>
#include <boost/scoped_ptr.hpp>
#include <iostream>

#ifdef WORKSTATION
#include <rialcommon.h>
#include <analogic/nss/common/NssEvent.h>
#endif

#include <QProcess>
#include <QThread>

#include "VREnums.h"

#define WORKSTATION_VERSION "04.04"
#define RERUN_EMULATOR_VERSION "02.06.00"

/**
 * Macro for config file
 */
#define INI_FILE_NAME "Workstation.ini"
#define RERUN_INI_FILE_NAME "Emulator.ini"
#define WS_SCREEN_CONFIG_FILE_NAME "workstation_screen.xml"
#define ACCESS_SCREEN_CONFIG_FILE_NAME "access_screen_features.xml"
#define SCANNER_FAULT_MESSAGE_FILE_NAME "faultMessage.xml"
#define BHS_WARNING_MESSAGE_FILE_NAME "bhsPickupMessage.xml"

/**
 *Macro for conversionFactor
 */
#define CMTOINCH 0.39370079
#define INCHTOCM 2.54

/**
 * Macro for bag file list format
 */
#define BAGFILE_PROPERTY_FORMAT ".properties"
#define BAGFILE_VOLUME_FORMAT ".vol"
#define BAGFILE_JPEG_VOLUME_FORMAT ".jpeg_ls_vol"
#define BAGFILE_DICOSCT_FORMAT ".dicos_ct_vol"
#define BAGFILE_DICOSCT_PROJ00_FORMAT ".dicos_ct_proj00"
#define BAGFILE_DICOSCT_PROJ90_FORMAT ".dicos_ct_proj90"
#define BAGFILE_DICOSTDR_FORMAT ".dicos_tdr_"
#define BAGFILE_PROJ00_FORMAT ".proj00"
#define BAGFILE_PROJ90_FORMAT ".proj90"
#define BAGFILE_DICOSCT_DCS_PROJ00_FORMAT ".dicos_ct_proj00.dcs"
#define BAGFILE_DICOSCT_DCS_PROJ90_FORMAT ".dicos_ct_proj90.dcs"

/**
 * Macro for bag properties volume format.
 */
#define VOLUME_FORMAT_JPEG          "jpeg-ls"
#define VOLUME_FORMAT_UNCOMPRESSED  "uncompressed"

/**
 * Macro for TIP bag prefix appended while saving TIP bag.
 */
#define TIP_BAG_PREFIX  "tip_"

/**
 * Macro for temporary storage of bag file in case of overwrite
 */
#define TEMP_BAG_STORAGE "/tmp/BagData"

/**
 * Macro for No Decision String
 */
#define THREAT_NO_DECISION_STRING "No Decision"

/**
 * Macro's for AGENT TYPE
*/
#define SCANNER_AGENT   "Scanner"
#define PVS_AGENT       "PVS"
#define SVS_AGENT       "SVS"


/*
 *SUPERVISOR REST_APIs PARAMETERS
*/
//---------------------AGENTS----------------------------//
#define AGENTS_TYPE                 "type"
#define AGENTS_MACHINE_ID           "machine_id"
#define AGENTS_MANUFACTURER         "manufacturer"
#define AGENTS_REGISTERED           "registered"
#define AGENTS_READINESS            "readiness"
#define AGENTS_STATE                "state"
#define AGENTS_GROUPS_UUID          "uuid"
#define AGENTS_GROUPS               "groups"
#define AGENTS_STATUS               "status"
#define AGENTS_CREDENTIALS          "credentials"
#define AGENTS_USER                 "user"
#define AGENTS_LOCATION             "location"
//-------------------------------------------------------//

//---------------------GROUP OVERVIEW----------------------------//
#define GROUP_OVERVIEW_UUID                 "uuid"
#define GROUP_OVERVIEW_NAME                 "name"
#define GROUP_OVERVIEW_READINESS            "readiness"
#define GROUP_OVERVIEW_AGENTS               "agents"
#define GROUP_OVERVIEW_PINNED_EVENTS        "pinned_events"
//-------------------------------------------------------//

//---------------------PINNED EVENTS----------------------------//
#define PINNED_EVENTS_UUID                 "uuid"
#define PINNED_EVENTS_NAME                 "name"
#define PINNED_EVENTS_SOURCE_AGENT         "source_agent"
#define PINNED_EVENTS_CREATED_TIME         "created_time"
#define PINNED_EVENTS_PARAM                "param"
//-------------------------------------------------------//

//---------------------RECENT EVENTS----------------------------//
#define RECENT_EVENTS_UUID          "uuid"
#define RECENT_EVENTS_SOURCEAGENTS  "source_agent"
#define RECENT_EVENTS_EVENTNAME     "eventName"
#define RECENT_EVENTS_CREATEDTIME   "createdTime"
#define RECENT_EVENTS_PARAMS        "params"
//-------------------------------------------------------//

//---------------------SUPERVISOR GROUPS----------------------------//
#define SUPERVISOR_GROUPS_UUID          "uuid"
#define SUPERVISOR_GROUPS_TYPE          "type"
#define SUPERVISOR_GROUPS_NAME          "name"
//-------------------------------------------------------//

//---------------------SUPERVISOR UTILIZATION----------------------------//
#define SUPERVISOR_UTILIZATION_AGENTS_UUID           "agent"
#define SUPERVISOR_UTILIZATION_GROUPS_UUID           "group"
#define SUPERVISOR_UTILIZATION_DATA                  "data"
//-------------------------------------------------------//

//---------------------AGENT's (Scanner/PVS/SVS) DETAIL----------------------------//
#define AGENT_UUID                 "uuid"
#define AGENT_MACHINE_ID           "machine_id"
#define AGENT_LOCATION             "machine_location"
#define AGENT_SUBLOCATION          "machine_sublocation"
#define AGENT_ONLINETIME           "online_timestamp"
#define AGENT_UNLINK_RATE          "unlink_rate"
#define AGENT_TRAYS_SCREENED       "online_bags_scanned"
#define AGENT_TRAYS_SEARCHED       "online_bags_screened"
//-------------------------------------------------------//

/*!
  TRACE FUNCTION
  */
#ifndef NDEBUG
#define TRACE_LOG(msg) LOG(SEVERITY_LEVEL::TRACE) << __FILE__ << " -line-" << __LINE__ << " " << __PRETTY_FUNCTION__ << " : " << msg;
#else
#define TRACE_LOG(msg)
#endif  // ANALOGIC_WS_COMMON_H_

/*!
  DEBUG MACRO
  */
#ifndef NDEBUG
#define DEBUG_LOG(msg) LOG(SEVERITY_LEVEL::DEBUG) << __FILE__ << " -line-" << __LINE__ << " " << __PRETTY_FUNCTION__ << " : " << msg;
#else
#define DEBUG_LOG(msg)
#endif

/*!
  INFO MACRO
  */
#ifndef NDEBUG
#define INFO_LOG(msg) LOG(SEVERITY_LEVEL::INFO) << msg;
#else
#define INFO_LOG(msg)
#endif

/*!
  WARNING MACRO
  */
#ifndef NDEBUG
#define WARN_LOG(msg) LOG(SEVERITY_LEVEL::WARNING) << msg;
#else
#define WARN_LOG(msg)
#endif

/*!
  ERROR MACRO
  */
#ifndef NDEBUG
#define ERROR_LOG(msg) LOG(SEVERITY_LEVEL::ERROR) << __FILE__ << " -line-" << __LINE__ << " : " << msg;
#else
#define ERROR_LOG(msg)
#endif


/*!
  FATAL MACRO
  */
#ifndef NDEBUG
#define FATAL_LOG(msg) LOG(SEVERITY_LEVEL::FATAL) << __FILE__ << " -line-" << __LINE__ << " : " << msg;
#else
#define FATAL_LOG(msg)
#endif


/*!
 * \class   QMLEnums
 * \brief   This class contains variable and function related to
 *          QMLEnums.
 */
class QMLEnums:public QObject
{
  Q_OBJECT

public:
  /*!
    * @fn       QMLEnums();
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
  explicit QMLEnums(QObject *parent = NULL):QObject(parent) {}

  /**
     * workstation application Screen Name
     */
  enum  ScreenEnum
  {
    LOGIN_SCREEN, //0
    RELIEVE_USER_LOGIN_PANEL_SCREEN,
    OSR_SCREEN,
    BAGSCREENING_SCREEN,
    SCANNERADMIN_SCREEN,
    RECALLBAG_SCREEN,
    REPORTS_SCREEN,
    RERUNDETECION_SCREEN,
    SETTINGS_SCREEN,
    USERS_SCREEN,
    INFORMATION_SCREEN,
    SEARCH_SCREEN,
    LOCAL_ARCHIVE_BAG_SCREEN,
    REMOTE_ARCHIVE_BAG_SCREEN,
    SEARCH_BAG_SCREEN,
    RERUNSELECTION_SCREEN,
    RERUNSELECTION_BAG_SCREEN,
    PASSWORD_EXPIRE_SCREEN,
    PIN_EXPIRE_SCREEN,
    TRAINING_PLAYLIST_SCREEN,
    TRAINING_VIEWER_SCREEN,
    TRAINING_VIEWER_IMAGE_CATEGORIZATION_SCREEN,
    TRAINING_REPORT_SCREEN,
    TRAINING_BAG_SCREEN,
    TIP_MAIN_SCREEN,
    ANTIVIRUS_SCREEN,
    OS_ACCESS_SCREEN,
    SUPERVISOR_SCREEN
  };
  /**
     * TIP Result
     */

  enum TIPResult
  {
    TIP_NONE, TIP_HIT, TIP_MISS
  };

  /**
     * File Dialog Mode
     */

  enum FileDialogAcceptMode
  {
    AcceptOpen, AcceptSave
  };

  /**
     * Scanner Admin Screen Name
     */
  enum ScannerAdminScreenEnum
  {
    STATUS_SCREEN = 0,
    RADIATIONSURVEY_SCREEN,
    ERRORLOG_SCREEN,
    IMAGEQUALITY_SCREEN,
    PARAMETER_SCREEN,
    DETECTION_ALGORITHM_SCREEN
  };

  /**
     * User Admin Screen Name
     */
  enum UserAdminScreenEnum
  {
    USER_MANAGEMENT = 0,
    USER_ACCESS_LEVEL,
    USER_DB_MNGMNT,
    KEY_MNGMNT
  };
  /**
    * Supervisor Screen Name
    */
  enum SupervisorScreenEnum
  {
    DASHBOARD_SCREEN = 0,
    CONFIGURATION_SCREEN,
    EXPORT_REPORT_SCREEN
  };

  /**
   * Supervisor Grouping
   */
  enum SupervisorGroupingEnum
  {
    GROUPING_ELEMENT = 0,
    GROUPING_GROUP,
    GROUPING_ROLLUP_GROUP
  };

  /**
     * User Admin Screen Name
     */
  enum NssServiceScreenEnum
  {
    UPLOAD_PACKAGE = 0,
    DOWNLOAD_PACKAGE,
    GET_VERSION_LIST
  };
  /**
     * Tip Admin Screen Name
     */
  enum TipAdminScreenEnum
  {
    TIP_EMPTY_SCREEN = 0,
    TIP_CONFIG_SCREEN,
    TIP_OBJECT_LIB_SCREEN,
    TIP_OBJECT_CONFIG_SCREEN,
    TIP_CONFIG_MODEL
  };

  /**
     * Potential results of an authentication attempt.
     */
  enum WSAuthenticationResult
  {
    SUCCESS = 0,
    INVALID_LOGIN = 1,
    CONCURRENT_LOGIN_ERROR = 2,
    AUTHENTICATION_METHOD_EXPIRED = 3,
    LOCKED_OUT = 4,
    INACTIVE = 5,
    IO_ERROR = 6
  };

  /**
     * Potential results of an authentication attempt.
     */
  enum ScannerState
  {
    IDLE = 0,
    STANDBY = 1,
    SCAN = 2
  };

  /**
     * Application state
     */
  enum AppState
  {
    INITIAL,
    ONLINE,
    SET_TO_OFFLINE,
    OFFLINE,
    INPROGRESS
  };

  /**
     * Application models
     */
  enum ModelEnum
  {
    UPDATE_TASK_STATUS_MODEL,//0
    AUTHENTICATIN_MODEL,
    KEYCONSOLE_MODEL,
    SDSW_HEARTBEAT_MODEL,
    ACCESSPANEL_MODEL,
    BHSSTATUS_MODEL,
    CONVEYORANDGANTRY_MODEL,
    SAFETYCONTROLLERFAULTS_MODEL,
    ESTOPS_MODEL,
    MOTIONCONTROLDATA_MODEL,

    POWERLINKINVERTERSTATUS_MODEL,
    POWERLINKINVERTERDISCRETESTATUS_MODEL,
    POWERLINKINVERTERSERIALFAULTS_MODEL,
    POWERLINKINVERTERSERIALSTATUS_MODEL,
    POWERLINKRECTIFIERSTATUS_MODEL,

    RCBGANTRYSTATUS_MODEL,

    SCANNERBAGPROCESSEDMODEL,
    SCANNERCALIBRATION_MODEL,
    SCANNERTEMPHUMIDITY_MODEL,

    XRAYCOMMANDSTATUS_MODEL,
    XRAYDATA_MODEL,
    XRAYFAULTS_MODEL,
    XRAYTIME_MODEL,
    XRAYSTATUS_MODEL,

    IMAGE_QUALITY_MASS_VALUE_MODEL,

    MISCELLANEOUS_MODEL,

    ERROR_LOG_SEARCH_MODEL,

    PARAMETER_MODEL,
    PARAMETER_SEND_MODEL,
    PARAMETER_XSD_MODEL,

    COMMAND_RADIATIONSURVEY_MODEL,
    COMMAND_RADIATIONSURVEY_CANCEL_MODEL,
    COMMAND_IMAGEQUALITY_TEST_MODEL,
    COMMAND_IMAGEQUALITY_CANCEL_MODEL,
    COMMAND_MOTIONCONTROLCONVEYOR_MODEL,
    COMMAND_MOTIONCONTROLGANTRY_MODEL,
    COMMAND_SCANFLUSHCONTROL_MODEL,

    INSTALLED_ALARM_LIB_MODEL,
    INSTALLABLE_ALARM_LIB_MODEL,
    COMMAND_INSTALL_ALARM_LIB,

    SCANNERADMIN_MODELS_END,

    USERADMIN_AUTHENTICATION_MODEL,
    UPDATE_USERADMIN_TASK_STATUS_MODEL,
    GET_USERADMIN_USER_DB_IMPORT_ENABLED_MODEL,
    GET_USERADMIN_USER_DB_EXPORT_ENABLED_MODEL,
    GET_USERADMIN_KEY_IMPORT_ENABLED_MODEL,
    GET_USERADMIN_KEY_EXPORT_ENABLED_MODEL,
    GET_USERADMIN_KEY_GENERATE_ENABLED_MODEL,

    COMMAND_USERADMIN_USER_DB_IMPORT_MODEL,
    COMMAND_USERADMIN_USER_DB_EXPORT_MODEL,
    COMMAND_USERADMIN_KEY_IMPORT_MODEL,
    COMMAND_USERADMIN_KEY_EXPORT_MODEL,
    COMMAND_USERADMIN_KEY_GENERATE_MODEL,
    COMMAND_USERADMIN_FILE_UPLOAD,
    COMMAND_USERADMIN_FILE_UPLOAD_COMPLETE,
    USERADMIN_MODELS_END,

    REPORT_ADMIN_AUTHENTICATION_MODEL,
    UPDATE_REPORT_TASK_STATUS_MODEL,
    COMMAND_REPORT_MODEL,
    COMMAND_REPORT_SCANNER_MODEL,
    COMMAND_REPORT_OPERATOR_MODEL,
    REPORT_ADMIN_MODELS_END,

    UPDATE_TIP_CONFIG_TASK_STATUS_MODEL,
    TIP_CONFIG_ADMIN_AUTHENTICATION_MODEL,
    COMMAND_UPDATE_TIP_CONFIG_SEND_MODEL,
    COMMAND_UPDATE_TIP_OBJECT_CONFIG_SEND_MODEL,
    COMMAND_TIP_CONFIG_MODEL,
    TIP_CONFIG_ADMIN_MODELS_END,

    UPDATE_TIP_LIB_CONFIG_TASK_STATUS_MODEL,
    TIP_LIB_CONFIG_ADMIN_AUTHENTICATION_MODEL,
    COMMAND_UPDATE_TIP_LIB_CONFIG_SEND_MODEL,
    COMMAND_TIP_LIB_CONFIG_MODEL,
    TIP_LIB_CONFIG_ADMIN_MODELS_END,

    NETWORK_AUTHENTICATION_MODEL,
    NETWORK_CHANGE_PASSWORD_MODEL,
    NETWORK_MODELS_END,
    RERUN_EMULATOR_AUTHENTICATION_MODEL,
    RERUN_EMULATOR_MODELS_END,
    QML_MESSAGE_MODEL,
    BAGRECEIVED_STATUS_MODEL,
    WORKSTATION_CONFIG_MODEL,
    USER_ACCESS_RESET_MODEL,
    TRAINING_WORKSTATION_AUTHENTICATION_MODEL,

    ANTIVIRUS_AUTHENTICATION_MODEL,
    COMMAND_GET_VERSION,
    COMMAND_GET_VERSION_LIST,
    UPDATE_LATEST_PACKAGE,
    UPDATE_PACKAGE_WITH_VERSION,
    COMMAND_UPDATE_PACKAGE_STATUS,
    DOWNLOAD_LATEST_PACKAGE,
    DOWNLOAD_PACKAGE_WITH_VERSION,
    ANTIVIRUS_MODELS_END,

    SUPERVISOR_AUTHENTICATION_MODEL,//92
    BAGS_PER_HOUR,//93
    SUSPECT_RATE_PER_HOUR,//94
    RECENT_TIME_TO_DIVERT,//95
    SUPERVISOR_AGENTS,//96
    SUPERVISOR_PINNED_EVENTS,//97
    SUPERVISOR_GROUP_OVERVIEW,//98
    SUPERVISOR_API_VERSION,//99
    RECENT_SEARCH_LIST_LENGHT,//100
    RECENT_EVENTS,//101
    SUPERVISOR_GROUPS,//102
    RECENT_OPERATOR_DECISION_TIME,//103
    SUPERVISOR_SCANNER_UTILIZATION,//104
    SUPERVISOR_PVS_UTILIZATION,//105
    SUPERVISOR_SVS_UTILIZATION,//106
    SUPERVISOR_SCANNER_DETAIL,//107
    SUPERVISOR_PVS_DETAIL,//108
    SUPERVISOR_SVS_DETAIL,//109
    SUPERVISOR_MODEL_END//110
  };

  /**
     * Network state
     */
  enum NetworkState
  {
    UNREGISTERED = 1,
    REGISTERED = 2,
    RECONNECTING = 3
  };

  /**
     * Nss Agent API Error
     */
  enum NssAgentError
  {
    SUCCESSFUL = 0,
    CONN_AGENT_ERROR,
    AUTH_AGENT_ERROR,
    NSS_FAIL
  };

  /**
     * Modal Frame Component
     */
  enum ModalFrameComp
  {
    FAULT_PANEL = 0,
    NETWORK_STATUS_ERROR,
    RELIEVE_USER_LOGIN_PANEL,
    PASSWORD_EXPIRE
  };

  /**
     * @brief The StatusChangedFor enum
     */
  enum Servers
  {
    NW_SERVER = 1,
    SCANNER_SERVER = 2,
    USER_ADMIN_SERVER = 3,
    RERUN_SERVER = 4,
    REPORT_ADMIN_SERVER = 5,
    TIP_CONFIG_ADMIN_SERVER = 6,
    TIP_LIB_CONFIG_ADMIN_SERVER = 7,
    ANTIVIRUS_SERVER = 8,
    SUPERVISOR_SERVER = 9
  };

  /**
     * @brief UI command from QML to C++. Prepended TEPC_ to avoid bugs in future due to name
     * clashes. Important as when accessing in QML code, the enum name is dropped.
     */
  enum ThreatEvalPanelCommand
  {
    TEPC_CLEAR_BAG,
    TEPC_TIMEOUT_BAG,
    TEPC_SUSPECT_BAG,
    TEPC_RETAIN_BAG,
    TEPC_NEXT_BAG,
    TEPC_STOP_BAG,
    TEPC_ORGANIC,
    TEPC_INORGANIC,
    TEPC_METAL,
    TEPC_CONTRAST,
    TEPC_BRIGHTNESS,
    TEPC_OPACITY,
    TEPC_PRINT,
    TEPC_EXPORT,
    TEPC_REMOVELAPTOP,
    TEPC_ZOOMIN,
    TEPC_ZOOMOUT,
    TEPC_INVERSE,
    TEPC_INVERTCOLORS,
    TEPC_DISPLAY_ALARM_THREAT_ONLY,
    TEPC_NEXT_THREAT,
    TEPC_CLEAR_THREAT,
    TEPC_SUSPECT_THREAT,
    TEPC_RELOAD,
    TEPC_CUTVIEW_PERCENTAGE_CHANGED,
    TEPC_CUTVIEW_AXIS_CHANGED,
    TEPC_PICKBOX_ADD,
    TEPC_PICKBOX_ADD_ACCEPT,
    TEPC_PICKBOX_DELETE,
    TEPC_PICKBOX_CANCEL,
    TEPC_PRESET_COLOR_ON,
    TEPC_PRESET_METAL_ONLY,
    TEPC_PRESET_ORGANIC_ONLY,
    TEPC_PRESET_INORGANIC_ONLY,
    TEPC_PRESET_LOWDENSITY,
    TEPC_PRESET_GREYSCALE_ON,
    TEPC_PRESET_INVERSE,
    TEPC_PRESET_SHOWRIGHTIMAGE,
    TEPC_PRESET_ALLMATERIAL,
    TEPC_PICKBOX_ACCEPT_ANOMALY,
    TEPC_PICKBOX_ACCEPT_EXPLOSIVES,
    TEPC_PICKBOX_ACCEPT_LIQUID,
    TEPC_PICKBOX_ACCEPT_WEAPON,
    TEPC_PICKBOX_ACCEPT_CONTRABAND,
    TEPC_VIEW_MODE,
    TEPC_NOTIFY_READY_FOR_PUSH_TRANSFER,
    TEPC_OVERWRITE,
    TEPC_ERROR,
    TEPC_SHOW_TIP_THREAT,
    TEPC_UNKNOWN,
    TEPC_TOGGLE_THREAT_COLORIZATION,
    TEPC_SLABVIEW_PERCENTAGE_CHANGED,
    TEPC_SLABVIEW_AXIS_CHANGED,
    TEPC_SLABVIEW_TYPE_CHANGED,
    TEPC_SHOW_ALL_THREAT_TOGGLE,
    TEPC_MAX
  };


  /**
     * @brief Workstation type
     */
  enum WSType
  {
    OSR_WORKSTATION = 0,
    SEARCH_WORKSTATION = 1,
    TRAINING_WORKSTATION = 2,
    RERUN_EMULATOR = 3,
    SUPERVISOR_WORKSTATION = 4,
  };

  /**
     * @brief UI commands from View to Model, for Archive and Search functionalities.
     */
  enum BagsListAndBagDataCommands
  {
    BLBDC_START = 0,
    BLBDC_SHOW_BAGS_LIST,
    BLBDC_FETCH_BAGS_LIST,
    BLBDC_SORT_BAGS_LIST,
    BLBDC_SHOW_BAG,
    BLBDC_DELETE_BAG,
    BLBDC_ARCHIVE_BAG_TO_USB,
    BLBDC_ARCHIVE_BAG_TO_LOCAL_ARCHIVE,
    BLBDC_ARCHIVE_BAG_OVERWRITE,
    BLBDC_END
  };

  /**
     * @brief Archive Filter Machine Decision
     */
  enum ArchiveFilterMachineDecision
  {
    MD_ALL = 0,
    MD_CLEAR,
    MD_ALARM,
    MD_FAULT
  };

  /**
     * @brief Archive Filter User Decision
     */
  enum ArchiveFilterUserDecision
  {
    UD_ALL = 0,
    UD_CLEAR,
    UD_SUSPECT,
    UD_ABORT
  };

  /**
     * @brief Archive Filter User Decision
     */
  enum ArchiveFilterInterval
  {
    PR_HOUR = 0,
    PR_DAY,
    PR_WEEK,
    PR_MONTH
  };

  /**
     * @brief Archive Filter User Decision
     */
  enum ArchiveReportType
  {
    RT_BAG_REPORT = 0,
    RT_MACHINE_REPORT,
    RT_BAG_DETAIL_REPORT,
    RT_AUTHORIZED_USER_REPORT,
    RT_CALIBRATION_DATA_REPORT,
    RT_EVENT_DATA_REPORT,
    RT_ITEM_DATA_REPORT,
    RT_OPERATOR_REPORT,
    RT_INTL_OPERATOR_KEYSTROKE_REPORT,
    RT_TIPSCREENER_LOGIN_LOGOUT_REPORT,
    RT_TIP_SESSION_SUMMARY_REPORT,
    RT_TIP_SESSION_REPORT,
    RT_TIP_PERFORMANCE_REPORT,
    RT_EQUIPMENT_SUMMARY_REPORT,
    RT_TSA_OPERATOR_SUMMARY_REPORT,
    RT_OPERATOR_KEYSTROKE_REPORT,
    RT_LEVEL2_SUMMARY_REPORT,
    RT_LEVEL3_SUMMARY_REPORT

  };

  /**
     * @brief Archive Filter Search Location
     */
  enum ArchiveFilterSrchLoc
  {
    REMOTE_SERVER  = 0,
    LOCAL_SERVER   = 1,
    SEARCH_SERVER  = 2,
    BHS_SERVER     = 3,
    PRIMARY_SERVER = 4
  };

  /**
     * @brief Rerun command enums
     */
  enum RERUN_COMMANDS
  {
    RERUN_START,
    RERUN_CANCEL,
    RERUN_RESET,
    RERUN_RELEASE,
    RERUN_GET_ALGORITHM
  };

  /**
     * @brief Rerun response enums
     */
  enum RERUN_RESPONSES
  {
    RERUN_GET_ALGORITHMS_RSP,
    RERUN_RERUN_IN_PROGRESS_RSP,
    RERUN_RERUN_DONE_RSP,
    RERUN_RERUN_CANCEL_RSP,
    RERUN_RESET_RSP,
    RERUN_ERROR_RSP
  };

  /**
     * @brief Select rerun BagList
     */
  enum SelectBagListType
  {
    SELECT_BAGLIST_TYPE = 0,
    SELECT_EXISTING_BAGLIST = 1,
    CREATE_BAGLIST_FROM_INPUT_FOLDER = 2,
    CREATE_BATCHLIST_FROM_INPUT_FOLDER = 3,
    SELECT_EXISTING_BATCHLIST = 4
  };

  /**
     * @brief Buttons to be displayed in MessageBox
     */
  enum MessageBoxButtons
  {
    OK = 1,
    CANCEL = 2,
    LOGOFF = 4,
    SHUTDOWN = 8,
    RESTART = 16,
    SDSW_CLIENT = 32,
    WORK_STATION = 64
  };

  /**
     * @brief NSS BasicCommandResult maps to WS
     */
  enum WSBasicCommandResult
  {
    WSBASIC_COMMAND_RESULT_SUCCESS = 0,
    WSBASIC_COMMAND_RESULT_IO_ERROR = 1,
    WSBASIC_COMMAND_RESULT_INVALID_REGISTRATION = 2,
    WSBASIC_COMMAND_RESULT_INVALID_PERMISSION = 3,
    WSBASIC_COMMAND_RESULT_INVALID_ARGUMENTS = 4,
    WSBASIC_COMMAND_RESULT_INVALID_STATE = 5,
    WSBASIC_COMMAND_RESULT_EXECUTION_ERROR = 6,
    WSBASIC_COMMAND_RESULT_INVALID_CREDENTIAL = 7,
    WSBASIC_COMMAND_RESULT_INVALID_AUTH_METHOD = 8,
    WSBASIC_COMMAND_RESULT_DETAIL_RULE_VIOLATION = 9,
    WSBASIC_COMMAND_RESULT_INVALID_OLD_DETAIL = 10
  };

  /**
     * @brief XML data types
     */
  enum XMLDataTypes
  {
    STRINGTYPE = 0,
    BOOLEANTYPE,
    INTTYPE,
    LISTTYPE,
    ENUMSTYPE,
    DEPENDENCYTYPE,
    READONLY,
    NOTYPE,
    TEXTAREA
  };

  enum XMLValueTypes
  {
    NOVALUE = 0,
    STRINGVALUE,
    INTVALUE,
    POSITIVEINT,
    FLOATVALUE,
    IPADDRESSVALUE,
    ENUMVALUE
  };

  /**
     * @brief The msgBoxDecision enum
     */
  enum msgBoxTaskDecision
  {
    RELEASE_STOP_THE_CHECK_ACCEPT = 0,
    RELEASE_STOP_THE_CHECK_REJECT,
    TRAY_PICKUP_ACCEPT,
    TRAY_PICKUP_REJECT,
    DECISION_TIMEOUT_ACCEPT,
    DECISION_TIMEOUT_REJECT,
    TIP_DECISION_ACCEPT,
    TIP_DECISION_REJECT,
    NON_TIP_DECISION_ACCEPT,
    NON_TIP_DECISION_REJECT
  };

  /**
     * @brief The conversionEnum enum
     */
  enum conversionEnum
  {
    INCH_TO_CENTIMETER,
    CENTIMETER_TO_INCH
  };

  /**
     * @brief The BhsDiverterDecision enum
     */
  enum BhsDiverterDecision {
    BHS_DIVERTER_DECISION_NONE = 0,
    BHS_DIVERTER_DECISION_ACCEPT = 1,
    BHS_DIVERTER_DECISION_REJECT = 2
  };

  /**
     * @brief The AuthMethodType enum
     */
  enum AuthMethodType {
    AUTH_METHOD_UNDEFINED = 0,
    AUTH_METHOD_PASSWORD = 1,
    AUTH_METHOD_PIN = 2,
    AUTH_METHOD_CARD = 3,
    AUTH_METHOD_PIV = 4
  };

  /**
     * @brief The Nss Event enum
     */
  enum NssEventEnum {
    EVENT_SDSW_LOGIN = 0,
    EVENT_FDRS_DOWNLOAD,
    EVENT_WORKSTATION_SETTINGS_UPDATED
  };

  enum DashboardItemType {
    SCANNER = 0,
    PVS,
    SVS
  };
  Q_ENUM(RERUN_RESPONSES);
  Q_ENUM(RERUN_COMMANDS);
  Q_ENUM(NetworkState);
  Q_ENUM(AppState);
  Q_ENUM(ScreenEnum);
  Q_ENUM(ModelEnum);
  Q_ENUM(WSAuthenticationResult);
  Q_ENUM(ScannerState);
  Q_ENUM(NssAgentError);
  Q_ENUM(ScannerAdminScreenEnum);
  Q_ENUM(ModalFrameComp);
  Q_ENUM(UserAdminScreenEnum);
  Q_ENUM(Servers);
  Q_ENUM(ThreatEvalPanelCommand);
  Q_ENUM(WSType);
  Q_ENUM(FileDialogAcceptMode);
  Q_ENUM(BagsListAndBagDataCommands);
  Q_ENUM(ArchiveFilterMachineDecision);
  Q_ENUM(ArchiveFilterUserDecision);
  Q_ENUM(ArchiveFilterInterval);
  Q_ENUM(ArchiveReportType);
  Q_ENUM(ArchiveFilterSrchLoc);
  Q_ENUM(SelectBagListType);
  Q_ENUM(MessageBoxButtons);
  Q_ENUM(WSBasicCommandResult);
  Q_ENUM(XMLDataTypes);
  Q_ENUM(XMLValueTypes);
  Q_ENUM(msgBoxTaskDecision);
  Q_ENUM(conversionEnum);
  Q_ENUM(TipAdminScreenEnum);
  Q_ENUM(TIPResult);
  Q_ENUM(BhsDiverterDecision);
  Q_ENUM(AuthMethodType);
  Q_ENUM(NssEventEnum);
  Q_ENUM(SupervisorScreenEnum);
  Q_ENUM(DashboardItemType);
  Q_ENUM(SupervisorGroupingEnum)

  /*!
    * @fn       init
    * @param    None
    * @return   None
    * @brief    Initialization of the common memebers
    */
  static void init();

  /*!
    * @fn       getAppstateString
    * @param    const QMLEnums::AppState &st
    * @return   std::string
    * @brief    gets the repspective string for enum
    */
  static std::string getAppstateString(const QMLEnums::AppState &st);

  /*!
    * @fn       getWSTypeEnum
    * @param    const std::string
    * @return   QMLEnums::WSType
    * @brief    gets the repspective string for enum
    */
  Q_INVOKABLE static QMLEnums::WSType getWSTypeEnum(const std::string st);

  /*!
    * @fn       getUserDeciString
    * @param    const QMLEnums::ArchiveFilterUserDecision
    * @return   std::string
    * @brief    gets the repspective string for enum
    */
  std::string static getUserDeciString(const QMLEnums::ArchiveFilterUserDecision st);

  /*!
    * @fn       getMachineDeciString
    * @param    const QMLEnums::ArchiveFilterMachineDecision
    * @return   std::string
    * @brief    gets the repspective string for enum
    */
  std::string static getMachineDeciString(const QMLEnums::ArchiveFilterMachineDecision st);

  /*!
    * @fn       getWSBasicCmdResultString
    * @param    const QMLEnums::WSBasicCommandResult
    * @return   std::string
    * @brief    gets the repspective string for enum
    */
  std::string static getWSBasicCmdResultString(const QMLEnums::WSBasicCommandResult st);

  /*!
     * @fn       getThreatDescriptionFromMap
     * @param    SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG
     * @return   QString
     * @brief    return String from Threat Decision Enum
     */
  QString static getThreatDescriptionFromMap(
      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG decision);

  // ARO-NOTE: Keep Line line lengths under 120 chars if possible!
  //!< static instance of the enum string map
  static QMap<QMLEnums::WSType, std::string>                          wsTypeEnumString;

  //!< static instance of the enum string map
  static QMap<QMLEnums::AppState, std::string>                        appStateEnumString;

  //!< static instance of the enum string map
  static QMap<QMLEnums::ArchiveFilterUserDecision, std::string>       userDeciEnumString;

  //!< static instance of the enum string map
  static QMap<QMLEnums::ArchiveFilterMachineDecision, std::string>    machineDeciEnumString;

  //!< static instance of the enum string map
  static QMap<QMLEnums::WSBasicCommandResult, std::string>            wsBasicCmdRsltEnumString;

  //!< static instance of the enum string map
  static QMap<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG, QString> m_threatDecisionString;
};

typedef QList<QMLEnums::ModelEnum> ModelList;

Q_DECLARE_METATYPE(QMLEnums::AppState)
Q_DECLARE_METATYPE(QMLEnums::TIPResult)
Q_DECLARE_METATYPE(QMLEnums::ScreenEnum)
Q_DECLARE_METATYPE(QMLEnums::ThreatEvalPanelCommand)
Q_DECLARE_METATYPE(QMLEnums::BagsListAndBagDataCommands)
Q_DECLARE_METATYPE(QMLEnums::RERUN_COMMANDS)
Q_DECLARE_METATYPE(QMLEnums::RERUN_RESPONSES)
Q_DECLARE_METATYPE(QMLEnums::ArchiveFilterSrchLoc)
Q_DECLARE_METATYPE(QMLEnums::conversionEnum)
Q_DECLARE_METATYPE(QMLEnums::BhsDiverterDecision)
Q_DECLARE_METATYPE(std::vector<std::string>)
Q_DECLARE_METATYPE(QMLEnums::NssEventEnum)

#ifdef WORKSTATION
Q_DECLARE_METATYPE(analogic::nss::NssEvent);
#endif

/*!
* @fn       fileExists
* @param    QString path
* @return   bool -
* @brief    check if file exist or not
*/
bool fileExists(QString path);
/*!
* @fn       dirExists
* @param    QString path
* @return   bool -
* @brief    check if directory exist or not
*/
bool dirExists(QString path);
/*!
* @fn       executeCommand
* @param    QString cmd
* @return   int
* @brief    execute system command
*/
int executeCommand(QString url);
/*!
* @fn       mapAlarmDescToDicosValue
* @param    bool machineDesc
* @param    int descision
* @return   SDICOS::TDRTypes::ThreatDetectionReport::ALARM_DECISION
* @brief    returns DICOS ALARM_DECISION mapped to QMLEnums ALARM_DECISION.
*/
SDICOS::TDRTypes::ThreatDetectionReport::ALARM_DECISION mapAlarmDescToDicosValue(bool machineDesc, int descision);

/*!
* @fn      createAndGetDirPath
* @param   int modelindex
* @param   QString filename
* @return  QString
* @brief   This function is responsible for create and get dir.
*/
QString createAndGetDirPath(int modelindex, QString filename);

/*!
* @fn      moveFilePath
* @param   QString dst
* @param   QString src
* @return  int
* @brief   Function will call on move file.
*/
int moveFilePath(QString dst, QString src);

/*!
* @fn      convertEnumToString
* @param   int modelno
* @return  QString
* @brief   Function to convert Model Enum to String.
*/
QString convertModelEnumToString(int modelno);


//----------------------------------------
// PLACE HOLDER FOR KEY-STROKE DEFINITIONS:
// Events Volume Renderer needs to notify
// the Workstation code as a "Key-Stroke".
//
// -----------------------------
// Original list
// -----------------------------
// Threat Region          – TR
// Threat Color           – TC
// Next Threat            – NT
// Clear Threat           – CT
// Suspect Threat         – ST
// Image Reset            – IR
// Clear Bag              – CB
// Suspect Bag            – SB
// SUrface Mode           – SU
// Threat Mode            – TM
// Laptop Mode            – LM
// Slab Mode              – SM
// COlor                  - CO
// Metal Only             - MO
// Organic Only           - OO
// Inorganic Only         - IO
// GReyscale              - GR
// InVerse                - IV
// Low Density            - LD
// Pseudo Camera          – PC
// RUler                  – RU
// Drawbox Add            – DA
// Drawbox Remove         - DR
// Unpacking X            - UX
// Unpacking Y            - UY
// Unpacking Z            - UZ
// Unpacking Slider       – US
// Slab X                 - SX
// Slab Y                 - SY
// Slab Z                 - SZ
// Slab Slider            – SS
// EXplosive              - EX
// WeaPon                 - WP
// ConTraband             - CT
// ANomaly                - AN
// LiQuid                 - LQ
// Prohibited Items       - PI <-- not yet complete.
//--------------------------------------------------------------
// Newly implemented values
// Zoom In                – ZI
// Zoom Out               – ZO
// End Pan                - EP
// End Rotation           - ER
// Volumetric Measurement - VM
// Distance Measurement   - DM
//-----------------------------------------------------------------


#endif  // ANALOGIC_WS_COMMON_H_

