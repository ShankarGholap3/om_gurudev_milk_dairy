/*!
* @file     taskmanager.cpp
* @author   Agiliad
* @brief    This file contains functions related to TaskManager
*           which handles task status update and downloading of files.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include <analogic/ws/uihandler/taskmanager.h>
#include <analogic/ws/uihandler/taskinfomodel.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       TaskManager
* @param    None
* @return   None
* @brief    Constructor for class TaskManager.
*/
TaskManager::TaskManager(int taskrefreshinterval, int tasktimeoutcheckinterval, QMLEnums::ModelEnum updatetaskmodel): m_updatemodelnum(updatetaskmodel)
{
  m_taskrefreshinterval = taskrefreshinterval;
  m_tasktimeout = tasktimeoutcheckinterval;
  DEBUG_LOG("Creating instance of task info model and connect signals and resp slots");
  m_pTaskInfo.reset(new TaskInfoModel(m_updatemodelnum, "taskinfo", DEFAULT_CONTENT_TYPE));
  THROW_IF_FAILED((m_pTaskInfo.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  connect(m_pTaskInfo.get(), SIGNAL(updateModel(QVariantList, QMLEnums::ModelEnum)),
          this, SLOT(onUpdateModel(QVariantList, QMLEnums::ModelEnum)));

  connect(m_pTaskInfo.get(), SIGNAL(authstatuschanged(int, QString, int)),
          this, SLOT(onAuthStatusChanged(int, QString, int)));

  connect(&m_Downloader, &Downloader::taskComplete, this, &TaskManager::onTaskComplete);

  connect(m_pTaskInfo.get(), SIGNAL(taskInfoReceived(QVariantList, QMLEnums::ModelEnum)),
          this, SLOT(ontaskInfoReceived(QVariantList, QMLEnums::ModelEnum)));

  resetTimer();

  m_taskstatus = NOT_STARTED;
  m_SecondIQFile = "";
  m_ChecksumSecondIQFile = "";
  m_taskTimeoutTimer.setInterval(m_tasktimeout);
  connect(&m_taskTimeoutTimer, SIGNAL(timeout()), this, SLOT(onTaskTimeout()), Qt::QueuedConnection);
}

/*!
* @fn       onTaskTimeout
* @param    None
* @return   None
* @brief    slots check if task has got timeout.
*/
void TaskManager::onTaskTimeout()
{
  QDateTime currenttime = QDateTime::currentDateTime();
  long int timediff = currenttime.toMSecsSinceEpoch() - m_lastresponsetime.toMSecsSinceEpoch();
  if(timediff>m_tasktimeout)
  {
    m_taskstatus = TIMEOUT;
    QString templog = tr("Task with modelnumber: ") + QString("%1").arg((int)m_modelnum) + tr(" TaskId: ") + QString("%1").arg(m_taskId)
        + tr(" Error: Task Timeout ErrorCode: ") +QString("%1").arg(Errors::SERVICE_UNAVAILABLE);
    ERROR_LOG(templog.toStdString());
    resetTimer();
    m_taskTimeoutTimer.stop();
    m_taskId.clear();
    m_modelnum = m_updatemodelnum;
    emit authstatuschanged(Errors::SERVICE_UNAVAILABLE, templog,m_modelnum);
  }
}
/*!
* @fn       ~TaskManager
* @param    None
* @return   None
* @brief    Destructor for class TaskManager.
*/
TaskManager::~TaskManager()
{
  TRACE_LOG("");
  DEBUG_LOG("Stopping timer and destroying members");
  m_pStatusTimer->stop();
  m_pTaskInfo.reset();
  m_pStatusTimer.reset();
}

/*!
* @fn       resetTimer
* @param    None
* @return   None
* @brief    called when m_pStatusTime is to reset with new QTimer.
*/
void TaskManager::resetTimer()
{

  // Previously m_pStatusTimer was getting stopped when either task was erroneous or was complete
  // but by stopping, the event that were in queue would get executed before stopping, but this would not happen
  // as precision of server sending reply is more and m_pStatusTimer sends request at every 3 secs,
  // but on safer side, now, the m_pStatusTimer will be deleted and reset to new QTimer

  // by default status timer will be  in stopped state.
  m_pStatusTimer.reset(new QTimer(this));
  THROW_IF_FAILED((m_pStatusTimer.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  // setting up for status timer
  m_pStatusTimer->setInterval(m_taskrefreshinterval);
  // connecting timer events to proper slot
  connect(m_pStatusTimer.get(), SIGNAL(timeout()), this, SLOT(onprocessStatusRequest()), Qt::QueuedConnection);
}

/*!
* @fn       onprocessStatusRequest
* @param    None
* @return   None
* @brief    called when timer expires to get task status.
*/
void TaskManager::onprocessStatusRequest()
{
  if (!((m_taskstatus == COMPLETED)
        || (m_taskstatus == FAILED)
        || (m_taskstatus == CANCELED)
        || (m_taskstatus == TIMEOUT)))
  {
    DEBUG_LOG("Requesting task status for task id: "<<m_taskId.toStdString());
    m_pTaskInfo->setTaskID(m_taskId);
    m_pTaskInfo->getrequest();
  }
}

/*!
* @fn       ontaskInfoReceived
* @param    QVariantMap - json response
* @param    QMLEnums::ModelEnum - modelname
* @return   None
* @brief    called  when response is received on a post request special case: raw data auth status
*/
void TaskManager::ontaskInfoReceived(QVariantList commandreslist, QMLEnums::ModelEnum modelname)
{
  QVariantMap commandresmap;
  if(commandreslist.size()==0)
  {
    ERROR_LOG("invalid response received.");
    return;

  }
  commandresmap = commandreslist[0].toMap();
  DEBUG_LOG("Response received for task having modelname: "<< modelname);
  QString error = commandresmap["error"].toString();
  int  errorcode = commandresmap["ErrorCode"].toInt();
  QString status = commandresmap["status"].toString();
  if((error != "") && ((errorcode == Errors::Errors::AUTHENTICATION_ERROR)
                       ||(errorcode == Errors::Errors::INVALID_REQ)
                       ||(errorcode == Errors::Errors::NO_RESPONSE)
                       ||(errorcode == Errors::Errors::FILE_NOT_FOUND)
                       ||(errorcode == Errors::Errors::SERVICE_UNAVAILABLE)
                       ||(errorcode == Errors::Errors::UNKNOWN_NETWORK)
                       ||(errorcode == Errors::Errors::CONNECTION_ERROR)
                       || (errorcode == Errors::Errors::SERVER_ERROR)
                       || (errorcode == Errors::RPC_REPLY_TIMEOUT)
                       || (errorcode==Errors::RPC_REPLY_ERROR)
                       || (errorcode==Errors::TASK_CANCELED)
                       || (errorcode==Errors::METHOD_NOT_ALLOWED)
                       || (errorcode==Errors::NETWORK_SESSION_OUT)
                       || (status.contains("Failed"))
                       ))
  {
    INFO_LOG("Task failed.");
    m_taskstatus = FAILED;
    m_taskTimeoutTimer.stop();
    QString templog = tr("Task with modelnumber: ") + QString("%1").arg(static_cast<int>(m_modelnum)) + tr(" TaskId: ") + QString("%1").arg(m_taskId)
        + tr(" Error: ") + QString("%1").arg(error) + tr(" ErrorCode: ") +QString("%1").arg(errorcode);

    WARN_LOG(templog.toStdString());
    m_pStatusTimer->stop();
    emit authstatuschanged(Errors::httpErrorEnum::INVALID_REQ, templog, modelname);
    return;
  }
  if(((modelname == QMLEnums::UPDATE_TASK_STATUS_MODEL) ||
      (modelname == QMLEnums::UPDATE_REPORT_TASK_STATUS_MODEL) ||
      (modelname == QMLEnums::UPDATE_TIP_CONFIG_TASK_STATUS_MODEL) ||
      (modelname == QMLEnums::UPDATE_TIP_LIB_CONFIG_TASK_STATUS_MODEL) ||
      (modelname == QMLEnums::UPDATE_USERADMIN_TASK_STATUS_MODEL))
     && validateTaskMessage(commandresmap))
  {
    m_lastresponsetime = QDateTime::currentDateTime();
  }
  if((m_taskstatus == STARTED))
  {
    INFO_LOG("Task is already running");
    QString templog = QString(
          "Task with modelnumber: %1 TaskId: %2 is already running. Please let task finish or cancel.")
        .arg(static_cast<int>(m_modelnum)).arg(m_taskId);

    WARN_LOG(templog.toStdString());
    return;
  }

  if ((commandresmap.contains("taskId"))
      && (commandresmap.contains("status"))
      && (commandresmap["taskId"].toString() != ""))
  {
    if(commandresmap["taskId"].toString() == m_taskId)
    {
      DEBUG_LOG("Taskid is: "<< commandresmap["taskId"].toString().toStdString() << " and its status is: "<< commandresmap["status"].toString().toStdString());
      return;
    }

    DEBUG_LOG("Updating task id to: "<< commandresmap["taskId"].toString().toStdString() <<" and modelnum to: " <<modelname);
    INFO_LOG("Task started");
    m_taskstatus = STARTED;
    m_taskId = commandresmap["taskId"].toString();
    m_modelnum = modelname;
    m_pStatusTimer->start();
    m_taskTimeoutTimer.start();
  }
}

/*!
* @fn       validateTaskMessage
* @param    QVariantMap
* @return   bool - result
* @brief    validate message.
*/
bool TaskManager::validateTaskMessage(QVariantMap commandresmap)
{
  bool result = true;
  if ((commandresmap["taskId"].toString().isEmpty())||
      (commandresmap["status"].toString().isEmpty())||
      (commandresmap["progress"].toString().isEmpty())
      )
  {
    result = false;
  }
  return result;
}

/*!
* @fn       onUpdateModel
* @param    QVariantList - map
* @param    QMLEnums::ModelEnum - modelname
* @return   None
* @brief    This function is responsible for updating model and starting the file download task.
*/
void TaskManager::onUpdateModel(QVariantList list, QMLEnums::ModelEnum modelname)
{
  QVariantMap mapdata;
  QString taskid;
  QString error;
  if (list.size() > 0)
  {
    mapdata = list[0].toMap();
    taskid = mapdata["taskId"].toString();
    error = mapdata["error"].toString();
  }
  else
  {
    ERROR_LOG("Invalid data received.");
    return;
  }
  if(((modelname == QMLEnums::UPDATE_TASK_STATUS_MODEL) ||
      (modelname == QMLEnums::UPDATE_REPORT_TASK_STATUS_MODEL)||
      (modelname == QMLEnums::UPDATE_TIP_CONFIG_TASK_STATUS_MODEL)||
      (modelname == QMLEnums::UPDATE_TIP_LIB_CONFIG_TASK_STATUS_MODEL)||
      (modelname == QMLEnums::UPDATE_USERADMIN_TASK_STATUS_MODEL)
      )
     && validateTaskMessage(mapdata))
  {
    m_lastresponsetime = QDateTime::currentDateTime();
  }

  if(((m_modelnum == QMLEnums::COMMAND_RADIATIONSURVEY_MODEL)
      || (m_modelnum ==QMLEnums::COMMAND_RADIATIONSURVEY_CANCEL_MODEL)
      ) && (mapdata["progress"].toInt() == 100))
  {
    resetTimer();
    m_taskstatus = COMPLETED;
    m_taskTimeoutTimer.stop();
  }
  else if(((m_modelnum ==QMLEnums::COMMAND_USERADMIN_KEY_IMPORT_MODEL)
           || (m_modelnum ==QMLEnums::COMMAND_USERADMIN_USER_DB_IMPORT_MODEL)
           || (m_modelnum ==QMLEnums::COMMAND_USERADMIN_KEY_GENERATE_MODEL)
           ) && (mapdata["progress"].toInt() == 100))
  {
    resetTimer();
    m_taskstatus = COMPLETED;
    m_taskTimeoutTimer.stop();
    QString msg;
    if ((m_modelnum == QMLEnums::COMMAND_USERADMIN_KEY_IMPORT_MODEL)
        || (m_modelnum == QMLEnums::COMMAND_USERADMIN_USER_DB_IMPORT_MODEL))
    {
      msg = "{\"status\":\"File Imported Successfully\",\"progress\":\"100\",\"error\":\"""\"}";

    }

    if (m_modelnum == QMLEnums::COMMAND_USERADMIN_KEY_GENERATE_MODEL)
    {
      msg = "{\"status\":\"Key Generation Task Executed Successfully.\",\"progress\":\"100\",\"error\":\"""\"}";
    }
    std::string errorlog = error.toStdString();
    if((errorlog.empty()))
    {
       onAuthStatusChanged(Errors::S_OK, msg, QMLEnums::COMMAND_USERADMIN_FILE_UPLOAD_COMPLETE);
    }
  }
  else if ((m_taskId == taskid) && (mapdata.contains("progress")))
  {
    QString stepDescriptionData = mapdata["stepDescription"].toString().toUpper();
    QVariant errorData = mapdata["error"];
    QString statusData = mapdata["status"].toString().toUpper();
    QString step = mapdata["step"].toString().toUpper();
    if ((mapdata["progress"].toInt() == 100)  && (mapdata["result"].toString() != ""))
    {
      resetTimer();
      m_taskstatus = COMPLETED;
      if (m_Downloader.getStatus(taskid) == NOT_STARTED || (m_Downloader.getStatus(taskid) == COMPLETED))
      {
        if (mapdata.contains("file_name_2"))
        {
          //save data for second file of ImageQuality
          m_SecondIQFile = mapdata["file_name_2"].toString();
          m_ChecksumSecondIQFile = mapdata["checksum2"].toString();
        }
        // save data for first file and start downloading
        m_Downloader.setDownloadLink( mapdata["result"].toString());
        m_Downloader.setFilePathToSave(mapdata["result"].toString());
        m_Downloader.setSha1Base16Checksum(mapdata["checksum"].toString());
        m_Downloader.setTaskId(mapdata["taskId"].toString());
        m_Downloader.start(modelname);
        emit updateModels(list, modelname);
        if(step.contains("EMPTY") == true)
        {
          emit authstatuschanged(Errors::S_OK, step, QMLEnums::UPDATE_REPORT_TASK_STATUS_MODEL);
        }
      }
      else
        ERROR_LOG("Cannot download: " << mapdata["result"].toString().toStdString());
    }
    else if ((stepDescriptionData.contains("ERROR") == true) ||
             ((errorData != "") && (statusData.compare("ERROR") == 0)) ||
             (statusData.contains("FAILED"))
             )
    {
      INFO_LOG("Task " << taskid.toStdString() << " completed with error");
      resetTimer();
      m_taskstatus = FAILED;
      if ((m_modelnum ==QMLEnums::COMMAND_USERADMIN_KEY_IMPORT_MODEL)
          || (m_modelnum ==QMLEnums::COMMAND_USERADMIN_USER_DB_IMPORT_MODEL)
          || (m_modelnum ==QMLEnums::COMMAND_USERADMIN_KEY_GENERATE_MODEL))
      {

      }
      else
      {
        m_pTaskInfo->postrequest();
      }


    }
  }
}

/*!
* @fn       onAuthStatusChanged
* @param    int - error code
* @param    QString - status
* @param    int - modelno
* @return   None
* @brief    called on authentication status changed
*/
void TaskManager::onAuthStatusChanged(int hr, QString status, int modelnum)
{
  emit authstatuschanged(hr, status, modelnum);
}

/*!
* @fn       onTaskComplete
* @param    QString - filepath downloaded
* @param    QString - download error
* @return   None
* @brief    called on downloading task is finished.
*/
void TaskManager::onTaskComplete(QString filepath, QString sDownloadError)
{
  if(sDownloadError == "")
  {
    DEBUG_LOG("Updating task status to completed");
    m_taskstatus = COMPLETED;
    if (((m_modelnum == QMLEnums::COMMAND_IMAGEQUALITY_TEST_MODEL)
         || (m_modelnum == QMLEnums::PARAMETER_MODEL)
         || (m_modelnum == QMLEnums::COMMAND_REPORT_MODEL)
         || (m_modelnum == QMLEnums::COMMAND_TIP_CONFIG_MODEL)
         || (m_modelnum == QMLEnums::COMMAND_TIP_LIB_CONFIG_MODEL))
        && (m_SecondIQFile != "") && (m_ChecksumSecondIQFile != ""))
    {
      emit taskcomplete(filepath, sDownloadError, m_modelnum);
      // Download second file
      m_Downloader.setDownloadLink(m_SecondIQFile);
      m_Downloader.setFilePathToSave(m_SecondIQFile);
      m_Downloader.setSha1Base16Checksum(m_ChecksumSecondIQFile);
      m_Downloader.setTaskId(m_taskId);
      m_SecondIQFile = "";
      m_ChecksumSecondIQFile = "";
      m_Downloader.start(m_modelnum);
      return;
    }

  }
  else
  {
    DEBUG_LOG("Updating task status to failed");
    m_taskstatus = FAILED;
  }
  DEBUG_LOG("Sending acknowledgement request for the task.");
  m_pTaskInfo->postrequest();
  DEBUG_LOG("Sending taskcomplete signal to start file download of file: "<< filepath.toStdString() <<" for modelnum: "<< m_modelnum);
  emit taskcomplete(filepath, sDownloadError, m_modelnum);
  m_taskTimeoutTimer.stop();
}
/*!
* @fn       cancelTask
* @param    None
* @return   None
* @brief    Cancel Task
*/
void TaskManager::cancelTask()
{
  if (m_taskstatus == STARTED)
  {
    m_taskstatus = CANCELED;
    QString templog = tr("Task Canceled.");

    INFO_LOG(templog.toStdString());
    resetTimer();
    m_taskTimeoutTimer.stop();
    emit authstatuschanged(Errors::httpErrorEnum::TASK_CANCELED, templog, m_modelnum);
  }
}
}  // namespace ws
}  // namespace analogic

