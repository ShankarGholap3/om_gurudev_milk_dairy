/*!
* @file     modelupdater.cpp
* @author   Agiliad
* @brief    This file contains functions related to ModelUpdater
*           which handles updating of models.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/modelupdater.h>
#include <analogic/ws/common/utility/xmlservice.h>
#include <analogic/ws/common/accesslayer/useradminaccessinterface.h>
#include <analogic/ws/common/accesslayer/reportadminaccessinterface.h>
#include <analogic/ws/common/accesslayer/scanneradminaccessinterface.h>
#include <analogic/ws/common/accesslayer/tipconfigadminaccessinterface.h>
#include <analogic/ws/common/accesslayer/tiplibconfigadminaccessinterface.h>
#include <analogic/ws/common/accesslayer/antivirusaccessinterface.h>
#include <analogic/ws/common/accesslayer/supervisoraccessinterface.h>


namespace analogic
{
namespace ws
{
/*!
* @fn       ModelUpdater
* @param    QMLEnums::ModelEnum - model number
* @param    QString - url
* @param    QByteArray contentType
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class ModelUpdater.
*/
ModelUpdater::ModelUpdater(QMLEnums::ModelEnum modelnumber, QString strresturl,
                           QByteArray contentType, QObject* parent):QObject(parent)
{
  m_modelnumber = modelnumber;
  m_strUrl = strresturl;
  m_contentType = contentType;
  init();
}

/*!
* @fn       ~ModelUpdater
* @param    None
* @return   None
* @brief    Destructor for class ModelUpdater.
*/
ModelUpdater::~ModelUpdater()
{
  TRACE_LOG("");
  // Singleton instance would be deleted by ModelManager.
  m_connectionAccessLayer = NULL;
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    This function is responsible for initialization of rial class members.
*/
void ModelUpdater::init()
{
  DEBUG_LOG("Initializing modelupdater based on modelnumber");
#ifdef WORKSTATION
  if(m_modelnumber < QMLEnums::ModelEnum::SCANNERADMIN_MODELS_END)
  {
    DEBUG_LOG("Initializing connection access layer with Scanner admin interface for modelno: " << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
    m_connectionAccessLayer = ScannerAdminAccessInterface::getInstance();
  }
  if(m_modelnumber > QMLEnums::ModelEnum::ANTIVIRUS_MODELS_END &&
     m_modelnumber < QMLEnums::ModelEnum::SUPERVISOR_MODEL_END)
  {
    DEBUG_LOG("Initializing connection access layer with supervisor interface for modelno: " << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
    m_connectionAccessLayer = SupervisorAccessInterface::getInstance();
  }
#endif
  DEBUG_LOG("Initializing modelupdater USERADMIN_MODELS");
  if(m_modelnumber > QMLEnums::ModelEnum::SCANNERADMIN_MODELS_END &&
     m_modelnumber < QMLEnums::ModelEnum::USERADMIN_MODELS_END)
  {
    DEBUG_LOG("Initializing connection access layer with User admin interface for modelno: " << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
    m_connectionAccessLayer = UserAdminAccessInterface::getInstance();
  }
#ifdef WORKSTATION
  DEBUG_LOG("Initializing modelupdater REPORT_ADMIN_MODELS");
  if(m_modelnumber > QMLEnums::ModelEnum::USERADMIN_MODELS_END &&
     m_modelnumber < QMLEnums::ModelEnum::REPORT_ADMIN_MODELS_END)
  {
    DEBUG_LOG("Initializing connection access layer with Report admin interface for modelno: " << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
    m_connectionAccessLayer = ReportAdminAccessInterface::getInstance();
  }

  if(m_modelnumber > QMLEnums::ModelEnum::REPORT_ADMIN_MODELS_END &&
     m_modelnumber < QMLEnums::ModelEnum::TIP_CONFIG_ADMIN_MODELS_END)
  {
    DEBUG_LOG("Initializing connection access layer with tip config admin interface for modelno: " << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
    m_connectionAccessLayer = TipConfigAdminAccessInterface::getInstance();
  }

  if(m_modelnumber > QMLEnums::ModelEnum::TIP_CONFIG_ADMIN_MODELS_END &&
     m_modelnumber < QMLEnums::ModelEnum::TIP_LIB_CONFIG_ADMIN_MODELS_END)
  {
    DEBUG_LOG("Initializing connection access layer with tip lib config "
              "admin interface for modelno: " << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
    m_connectionAccessLayer = TipLibConfigAdminAccessInterface::getInstance();
  }
  if(m_modelnumber > QMLEnums::ModelEnum::USER_ACCESS_RESET_MODEL &&
     m_modelnumber < QMLEnums::ModelEnum::ANTIVIRUS_MODELS_END)
  {
    DEBUG_LOG("Initializing connection access layer with antivirus "
              " interface for modelno: " << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
    m_connectionAccessLayer = AntiVirusAccessInterface::getInstance();
  }
#endif
  THROW_IF_FAILED(((m_connectionAccessLayer == NULL) ? Errors::E_POINTER : Errors::S_OK));
}

/*!
* @fn       getModelNumber
* @param    None
* @return   QMLEnums::ModelEnum = model number
* @brief    This function is responsible for getting model number.
*/
QMLEnums::ModelEnum ModelUpdater::getModelNumber() const
{
  DEBUG_LOG("Modelnumber is: "<< QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
  return m_modelnumber;
}

/*!
* @fn       getUrl
* @param    None
* @return   QString
* @brief    This function is responsible for getting url.
*/
QString ModelUpdater::getUrl() const
{
  return m_strUrl;
}

/*!
* @fn       setUrl
* @param    QString
* @return   None
* @brief    This function is responsible for setting url.
*/
void ModelUpdater::setUrl(QString url)
{
  m_strUrl = url;
}

/*!
* @fn       getrequest();
* @param    QString - sUrl
* @return   None
* @brief    This function is responsible for updating model.
*/
void ModelUpdater::getrequest(QString sUrl)
{
  sUrl = sUrl == "" ? m_strUrl : sUrl;
  DEBUG_LOG("Sending getrequest using url:" << sUrl.toStdString());
  bool updateWithoutToken = false;
  if(m_modelnumber == QMLEnums::SDSW_HEARTBEAT_MODEL)
  {
    updateWithoutToken = true;
  }

  QString modelString = QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber);
  m_reply.reset( m_connectionAccessLayer->getrequest(sUrl, updateWithoutToken, m_contentType, modelString));

  if ( !m_reply.isNull() )
  {
    connect(m_reply.data(), &RialReply::dataAvailable, this, &ModelUpdater::onmodelresponse);
#if defined WORKSTATION || defined RERUN
    connect(m_reply.data(), &RialReply::sslAvailable, this, &ModelUpdater::onsslavailable);
#endif
  }
  else
  {
    ERROR_LOG("NULL reply for GET " << sUrl.toStdString());
  }
}

/*!
* @fn       getdwrequest();
* @param    QString - sUrl
* @return   None
* @brief    This function is responsible for updating model.
*/
void ModelUpdater::getdwrequest(QString sUrl)
{
  sUrl = sUrl == "" ? m_strUrl : sUrl;
  DEBUG_LOG("Sending getrequest using url:" << sUrl.toStdString());
  bool updateWithoutToken = false;
  m_reply.reset( m_connectionAccessLayer->getdwrequest(sUrl));
  if ( !m_reply.isNull() )
  {
    connect(m_reply.data(), &RialReply::downladAvailable, this,
            &ModelUpdater::onDwnldAvailable, Qt::QueuedConnection);
    connect(m_reply.data(), &RialReply::downloadFinished, this,
            &ModelUpdater::onDwnldFinished, Qt::QueuedConnection);
  }
  else
  {
    ERROR_LOG("NULL reply for GET " << sUrl.toStdString());
  }
}

/*!
* @fn       postrequest();
* @param    QString data
* @param    QString sUrl
* @return   None
* @brief    This function is responsible for posting model.
*/
void ModelUpdater::postrequest(QString data, QString sUrl){
  sUrl = sUrl == "" ? m_strUrl : sUrl;
  DEBUG_LOG("Sending postrequest using url:" << sUrl.toStdString());

  QString modelString = QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber);
  m_reply.reset( m_connectionAccessLayer->postrequest(sUrl, data, m_contentType, true, modelString));

  if ( !m_reply.isNull() )
  {
    connect(m_reply.data(), &RialReply::dataAvailable, this, &ModelUpdater::onmodelresponse);
#if defined WORKSTATION || defined RERUN
    connect(m_reply.data(), &RialReply::sslAvailable, this, &ModelUpdater::onsslavailable);
#endif
  }
  else
  {
    ERROR_LOG("NULL reply for GET " << sUrl.toStdString());
  }
}

#if defined WORKSTATION || defined RERUN
/*!
* @fn       onsslavailable
* @param    int - Result
* @param    QString - replydata
* @return   None
* @brief    This function handles ssl signal.
*/
void ModelUpdater::onsslavailable(int hr, QString replydata )
{
  emit sslerrorreceived(hr, replydata, m_modelnumber);
}
#endif
/*!
* @fn       onmodelresponse
* @param    int - Result
* @param    QString - replydata
* @return   None
* @brief    This function is responsible for updating model slot.
*/
void ModelUpdater::onmodelresponse(int hr, QString replydata )
{
  QString strReplyData = replydata;
  if(hr == Errors::SUCCESS)
  {
    // if ups status display request response than file open dialog should be opened.
    if(m_modelnumber == QMLEnums::SDSW_HEARTBEAT_MODEL)
    {
      emit authstatuschanged(hr, replydata, m_modelnumber);
    }
    else if (!strReplyData.isEmpty())
    {
      DEBUG_LOG("Model response for modelnumber: " << m_modelnumber << " hr: "
                << hr << " and reply string: " << strReplyData.toStdString());
      strReplyData = QString ("{\"data\":") + strReplyData + "}";

      QJsonDocument jsondoc = QJsonDocument::fromJson(strReplyData.toUtf8());

      QJsonObject jobj =  jsondoc.object();
      QVariantMap mapobj =  jobj.toVariantMap();
      QVariantList listobj;

      if ( mapobj["data"].type() == QVariant::List)
      {
        listobj = mapobj["data"].toList();
      }
      else if ( mapobj["data"].type() == QVariant::Map)
      {
        listobj <<  mapobj["data"].toMap();
      }
      else
      {
        ERROR_LOG("Data is not as per desing map,list." << replydata.toStdString() << " " << mapobj["data"].type());
        return;
      }

      if ((m_modelnumber == QMLEnums::IMAGE_QUALITY_MASS_VALUE_MODEL) ||
          (m_modelnumber == QMLEnums::COMMAND_REPORT_SCANNER_MODEL) ||
          (m_modelnumber == QMLEnums::COMMAND_REPORT_OPERATOR_MODEL) ||
          (m_modelnumber == QMLEnums::COMMAND_INSTALL_ALARM_LIB))
      {
        emit authstatuschanged(hr, replydata, m_modelnumber);
      }
      else
      {
        emit updateModel(listobj, m_modelnumber );
      }

      if ((m_modelnumber == QMLEnums::UPDATE_TASK_STATUS_MODEL)
          || (m_modelnumber == QMLEnums::UPDATE_REPORT_TASK_STATUS_MODEL)
          || (m_modelnumber == QMLEnums::UPDATE_USERADMIN_TASK_STATUS_MODEL)
          ||(m_modelnumber == QMLEnums::ERROR_LOG_SEARCH_MODEL)
          ||(m_modelnumber == QMLEnums::UPDATE_TIP_CONFIG_TASK_STATUS_MODEL)
          ||(m_modelnumber == QMLEnums::UPDATE_TIP_LIB_CONFIG_TASK_STATUS_MODEL)
          ||(m_modelnumber == QMLEnums::PARAMETER_XSD_MODEL)
          ||(m_modelnumber == QMLEnums::PARAMETER_MODEL)
          )
      {
        emit taskInfoReceived(listobj, m_modelnumber);
        emit authstatuschanged(hr, replydata, m_modelnumber);
      }
    }
  }
  else
  {
    if ((m_modelnumber == QMLEnums::UPDATE_TASK_STATUS_MODEL) ||
        (m_modelnumber == QMLEnums::UPDATE_REPORT_TASK_STATUS_MODEL) ||
        (m_modelnumber == QMLEnums::UPDATE_TIP_CONFIG_TASK_STATUS_MODEL) ||
        (m_modelnumber == QMLEnums::UPDATE_TIP_LIB_CONFIG_TASK_STATUS_MODEL) ||
        (m_modelnumber == QMLEnums::UPDATE_USERADMIN_TASK_STATUS_MODEL)
        )
    {
      DEBUG_LOG("response for update task model with hr: " <<hr << " and response string: " << replydata.toStdString());
      QVariantList listobj;
      QVariantMap mapobj;
      mapobj.insert("error", strReplyData);
      mapobj.insert("ErrorCode", hr);
      listobj <<  mapobj;
      emit taskInfoReceived(listobj, m_modelnumber);
    }
    emit authstatuschanged(hr, replydata, m_modelnumber);
  }
}
/*!
* @fn       onDownladAvailable();
* @param    int - hr
* @param    QByteArray - data
* @return   None
* @brief    Function will call on dowload available.
*/
void ModelUpdater::onDwnldAvailable(int hr, QByteArray data)
{
  if (hr == Errors::SUCCESS)
  {
    INFO_LOG("File is available for download");
    m_fileptr.reset(new QFile("/home/receive_file/clamAV.tgz"));
    INFO_LOG("Downloading file to: "<<m_filepath.toStdString());
    m_fileptr->open( QFile::WriteOnly | QFile::Truncate );
  }
  m_fileptr->write(data);

}

/*!
* @fn       onDownloadFinished();
* @param    None
* @return   None
* @brief    Function will call on dowload finished.
*/
void ModelUpdater::onDwnldFinished()
{
  if (m_fileptr.use_count() != 0 && m_sDownloadError == "")
  {
    m_fileptr->close();
    INFO_LOG("File download task completed for file: "<< m_filepath.toStdString() <<" and error string is: "<< m_sDownloadError.toStdString());
    m_sDownloadError = "";
  }
  else
  {
    if(!m_fileptr)
      ERROR_LOG("Downloader: No file created.");

    if(m_sDownloadError != "")
    {

      ERROR_LOG("Downloader: " << m_sDownloadError.toStdString().c_str());
    }
  }
  m_sDownloadError = "";
}
}  // end of namespace ws
}  // end of namespace analogic


