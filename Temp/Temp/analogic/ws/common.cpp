/*!
* @file     common.cpp
* @author   Agiliad
* @brief    This file contains functions related to all classes.
* @date     Sep, 29 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common.h>
#include <QApplication>
#include <QFileInfo>
#include <QDir>

QMap<QMLEnums::WSType, std::string> QMLEnums::wsTypeEnumString;
QMap<QMLEnums::AppState, std::string> QMLEnums::appStateEnumString;
QMap<QMLEnums::ArchiveFilterUserDecision, std::string> QMLEnums::userDeciEnumString;
QMap<QMLEnums::ArchiveFilterMachineDecision, std::string> QMLEnums::machineDeciEnumString;
QMap<QMLEnums::WSBasicCommandResult, std::string> QMLEnums::wsBasicCmdRsltEnumString;
QMap<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG, QString> QMLEnums::m_threatDecisionString;

/*!
* @fn       getAppstateString
* @param    const QMLEnums::AppState &st
* @return   std::string
* @brief    gets the repspective string for enum
*/
std::string QMLEnums::getAppstateString(const QMLEnums::AppState &st)
{
  return appStateEnumString[st];
}

/*!
* @fn       getWSTypeEnum
* @param    const std::string
* @return   QMLEnums::WSType
* @brief    gets the repspective enum for string
*/
QMLEnums::WSType QMLEnums::getWSTypeEnum(const std::string st)
{
  return wsTypeEnumString.key(st);
}

/*!
* @fn       getUserDeciString
* @param    const QMLEnums::ArchiveFilterUserDecision
* @return   std::string
* @brief    gets the repspective string for enum
*/
std::string QMLEnums::getUserDeciString(const QMLEnums::ArchiveFilterUserDecision st)
{
  return userDeciEnumString[st];
}

/*!
* @fn       getWSBasicCmdResultString
* @param    const QMLEnums::WSBasicCommandResult
* @return   std::string
* @brief    gets the repspective string for enum
*/
std::string QMLEnums::getWSBasicCmdResultString(const QMLEnums::WSBasicCommandResult st)
{
  return wsBasicCmdRsltEnumString[st];
}

/*!
* @fn       getMachineDeciString
* @param    const QMLEnums::ArchiveFilterMachineDecision
* @return   std::string
* @brief    gets the repspective string for enum
*/
std::string QMLEnums::getMachineDeciString(const QMLEnums::ArchiveFilterMachineDecision st)
{
  return machineDeciEnumString[st];
}

/*!
 * @fn       getThreatDescriptionFromMap
 * @param    SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG
 * @return   QString
 * @brief    return String from Threat Decision Enum
 */
QString QMLEnums::getThreatDescriptionFromMap(
    SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG decision)
{
  return m_threatDecisionString[decision];
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    Initialization of the common memebers
*/
void QMLEnums::init()
{
  TRACE_LOG("");
  appStateEnumString[QMLEnums::INITIAL]                                       = "INITIAL";
  appStateEnumString[QMLEnums::SET_TO_OFFLINE]                                = "SET_TO_OFFLINE";
  appStateEnumString[QMLEnums::AppState::INPROGRESS]                          = "INPROGRESS";
  appStateEnumString[QMLEnums::AppState::ONLINE]                              = "ONLINE";
  appStateEnumString[QMLEnums::AppState::OFFLINE]                             = "OFFLINE";

  wsTypeEnumString[QMLEnums::WSType::OSR_WORKSTATION]                         = "OSR Workstation";
  wsTypeEnumString[QMLEnums::WSType::SEARCH_WORKSTATION]                      = "SEARCH Workstation";
  wsTypeEnumString[QMLEnums::WSType::TRAINING_WORKSTATION]                    = "Training Workstation";
  wsTypeEnumString[QMLEnums::WSType::RERUN_EMULATOR]                       = "Rerun Emulator";
  wsTypeEnumString[QMLEnums::WSType::SUPERVISOR_WORKSTATION]                      = "SUPERVISOR Workstation";

  userDeciEnumString[QMLEnums::ArchiveFilterUserDecision::UD_ALL]             = "All";
  userDeciEnumString[QMLEnums::ArchiveFilterUserDecision::UD_CLEAR]           = "Clear";
  userDeciEnumString[QMLEnums::ArchiveFilterUserDecision::UD_SUSPECT]         = "Suspect";
  userDeciEnumString[QMLEnums::ArchiveFilterUserDecision::UD_ABORT]         = "Unprocessed";

  machineDeciEnumString[QMLEnums::ArchiveFilterMachineDecision::MD_ALL]       = "All";
  machineDeciEnumString[QMLEnums::ArchiveFilterMachineDecision::MD_CLEAR]     = "Clear";
  machineDeciEnumString[QMLEnums::ArchiveFilterMachineDecision::MD_ALARM]     = "Alarm";
  machineDeciEnumString[QMLEnums::ArchiveFilterMachineDecision::MD_FAULT]     = "Fault";

  m_threatDecisionString[SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat]   = "Suspect";
  m_threatDecisionString[SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat] = "Clear";
  m_threatDecisionString[SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknown]  = "No Decision";

  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_SUCCESS]              = "Result Success";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_IO_ERROR]             = "Result IO Error";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_INVALID_REGISTRATION] = "Result Invalid Registration";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_INVALID_PERMISSION]   = "Result Invalid Permission";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_INVALID_ARGUMENTS]    = "Result Invalid Arguments";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_INVALID_STATE]        = "Result Invalid State";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_EXECUTION_ERROR]      = "Result Execution Error";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_INVALID_CREDENTIAL]   = "Result Invalid Credentials";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_INVALID_AUTH_METHOD]  = "Result Invalid Auth Method";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_DETAIL_RULE_VIOLATION]= "Result Detail Rule Violation:\nPassword should include uppercase,lowercase and special characters";
  wsBasicCmdRsltEnumString[QMLEnums::WSBASIC_COMMAND_RESULT_INVALID_OLD_DETAIL]   = "Old password is incorrect";
}

/*!
* @fn       fileExists
* @param    QString path
* @return   bool -
* @brief    check if file exist or not
*/
bool fileExists(QString path) {
  QFileInfo check_file(path);
  // check if file exists and if yes: Is it really a file and no directory?
  if (check_file.exists() && check_file.isFile()) {
    return true;
  } else {
    return false;
  }
}

/*!
* @fn       dirExists
* @param    QString path
* @return   bool -
* @brief    check if directory exist or not
*/
bool dirExists(QString path) {
  QFileInfo check_file(path);
  // check if file exists and if yes: Is it really a file and no directory?
  if (check_file.exists() && check_file.isDir()) {
    return true;
  } else {
    return false;
  }
}

/*!
* @fn       mapAlarmDescToDicosValue
* @param    bool machineDesc
* @param    int descision
* @return   SDICOS::TDRTypes::ThreatDetectionReport::ALARM_DECISION
* @brief    returns DICOS ALARM_DECISION mapped to QMLEnums ALARM_DECISION.
*/
SDICOS::TDRTypes::ThreatDetectionReport::ALARM_DECISION mapAlarmDescToDicosValue(bool machineDesc, int descision)
{
  SDICOS::TDRTypes::ThreatDetectionReport::ALARM_DECISION desc;
  if (machineDesc) {
    switch (descision) {
    case QMLEnums::MD_CLEAR:
      desc = SDICOS::TDRTypes::ThreatDetectionReport::enumClear;
      break;
    case QMLEnums::MD_ALARM:
      desc = SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm;
      break;
    case QMLEnums::MD_FAULT:
      desc = SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision;
      break;
    }
  }
  else
  {
    switch (descision) {
    case QMLEnums::UD_CLEAR:
      desc = SDICOS::TDRTypes::ThreatDetectionReport::enumClear;
      break;
    case QMLEnums::UD_SUSPECT:
      desc = SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm;
      break;
    case QMLEnums::UD_ABORT:
      desc = SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision;
      break;
    default:
      desc = SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision;
    }
  }
  return desc;
}
/*!
* @fn       executeCommand
* @param    QString cmd
* @return   int
* @brief    execute system command
*/
int executeCommand(QString url)
{
  TRACE_LOG("");
  DEBUG_LOG("Executing Command: " << url.toStdString());
  return  QProcess::execute(url);
}

/*!
* @fn      createAndGetDirPath
* @param   int modelindex
* @param   QString filename
* @return  QString
* @brief   This function is responsible for create and get dir.
*/
QString createAndGetDirPath(int modelindex, QString filename)
{
  QDir oDir;
  QString expectedlocation;
  switch(modelindex)
  {
  case QMLEnums::ERROR_LOG_SEARCH_MODEL:
    expectedlocation = QApplication::applicationDirPath() +
        (QString)FILE_ROOT_FOLDER +
        (QString)ERROR_LOG_FOLDER + "/";
    break;
  case QMLEnums::COMMAND_IMAGEQUALITY_TEST_MODEL:
    if ((filename.contains(analogic::ws::WorkstationConfig::getInstance()->getIQreportfileformat()))
            || (filename.contains(analogic::ws::WorkstationConfig::getInstance()->getIQNistfileformat())))
    {
      expectedlocation =  QApplication::applicationDirPath() +
          (QString)FILE_ROOT_FOLDER +
          (QString)IMAGE_QUALITY_REPORT + "/";
    }
    else
    {
      expectedlocation =  QApplication::applicationDirPath() +
          (QString)FILE_ROOT_FOLDER +
          (QString)IMAGE_QUALITY_BAG + "/";
    }
    break;
  case QMLEnums::COMMAND_REPORT_MODEL:
    expectedlocation = QApplication::applicationDirPath() +
        (QString)FILE_ROOT_FOLDER +
        (QString)FDRS + "/";
    break;
  case QMLEnums::PARAMETER_MODEL:
  case QMLEnums::PARAMETER_XSD_MODEL:
    expectedlocation = QApplication::applicationDirPath() +
        (QString)FILE_ROOT_FOLDER +
        (QString)SCANNER_PARAMETER + "/";
    break;

  case QMLEnums::COMMAND_USERADMIN_USER_DB_EXPORT_MODEL:
  case QMLEnums::COMMAND_USERADMIN_KEY_EXPORT_MODEL:
    expectedlocation = QApplication::applicationDirPath() +
        (QString)FILE_ROOT_FOLDER +
        (QString)DB_MNGMNT_FOLDER + "/";
    break;
  case QMLEnums::COMMAND_TIP_CONFIG_MODEL:
  case QMLEnums::COMMAND_TIP_LIB_CONFIG_MODEL:
    expectedlocation = QApplication::applicationDirPath() +
        (QString)FILE_ROOT_FOLDER +
        (QString)TIP_FOLDER + "/";
    break;
  default:
    break;
  }
  DEBUG_LOG("Creating directory: " << expectedlocation.toStdString());
  bool breturn = oDir.mkpath(expectedlocation);
  if (!breturn)
  {
    ERROR_LOG("Failed to create dir: " << expectedlocation.toStdString());
    expectedlocation = QDir::tempPath();
  }
  else
  {
    DEBUG_LOG("Expected directory/location set to: "
              << expectedlocation.toStdString());
    expectedlocation += filename;
  }
  return expectedlocation;
}

/*!
* @fn      moveFilePath
* @param   QString dstgetModelData
* @param   QString src
* @return  int
* @brief   Function will call on save file.
*/
int moveFilePath(QString dst , QString src)
{
  TRACE_LOG("");
  int hr = Errors::FILE_OPERATION_SUCCESS;
  bool status = true;
  INFO_LOG("Common::moveFile: <" << src.toStdString() << "> <" << dst.toStdString() << ">");
  if(src == dst)
  {
    INFO_LOG("Source-destination Path are same. Returning.");
    return 0;
  }

  QFile iFile(src);
  QFile oFile(dst);

  if(oFile.exists())
  {
    bool res = oFile.remove();
    if(!res)
    {
      status = false;
      ERROR_LOG("File Already Exist And Existing File removal failed. Possible Causes: "
                "Folder or File Permission." <<  dst.toStdString());
      hr = Errors::FILE_REMOVAL_FAILED;
    }
  }


  if(status == true)
  {
    if(iFile.exists())
    {
      bool res = iFile.copy(dst);
      if(!res)
      {
        ERROR_LOG("File Copy failed on the given path. Possible issue: Folder permission or "
                  "File Permission or File Path does not exist. Path: " <<  dst.toStdString());
        hr = Errors::FILE_PERMISSION_DENIED;
      }
    }
    else
    {
      ERROR_LOG("Src file doesn't exist. Possible issue: File is not received from server or"
                " file permission or folder permission. Path: " << src.toStdString());
      hr = Errors::FILE_NOT_PRESENT;
    }
    status = false;
  }

  bool res = iFile.remove();
  if (status == true)
  {
    if(!res)
    {
      ERROR_LOG("Deleting file from source path after copy failed. Path: " << src.toStdString());
      hr = Errors::FILE_PATH_NOT_FOUND;
    }
  }
  return hr;
}

/*!
* @fn      convertEnumToString
* @param   int modelno
* @return  QString
* @brief   Function to convert Model Enum to String.
*/
QString convertModelEnumToString(int modelno)
{
  return QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(modelno);
}
