/*!
* @file     commandmodel.cpp
* @author   Agiliad
* @brief    This file contains functions related to Command Model
*           which is the part of control screen.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/commandmodel.h>


namespace analogic
{
namespace ws
{

/*!
* @fn       CommandModel
* @param    QMLEnums::ModelEnum - model number
* @param    QString - url
* @param    QByteArray contentType
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class CommandModel
*/
CommandModel::CommandModel(QMLEnums::ModelEnum modelnumber, QString strresturl,
                           QByteArray contentType, QObject* parent):
  ModelUpdater(modelnumber, strresturl, contentType, parent)
{
}

/*!
* @fn       postrequest();
* @param    QString data
* @param    QString sUrl
* @return   None
* @brief    This function is responsible for posting model.
*/
void CommandModel::postrequest(QString data, QString sUrl)
{
  QString sreqstr = "";
  if(data == "")
  {
    int hr = -1;
    QString strReplyData = tr("Input is Null For Sending Post Command");
    emit authstatuschanged(hr, strReplyData, m_modelnumber);
  }
  else
  {
    if ((m_modelnumber == QMLEnums::COMMAND_IMAGEQUALITY_TEST_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_RADIATIONSURVEY_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_RADIATIONSURVEY_CANCEL_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_IMAGEQUALITY_CANCEL_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_REPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_USER_DB_EXPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_USER_DB_IMPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_KEY_EXPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_KEY_IMPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_KEY_GENERATE_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_TIP_CONFIG_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_TIP_LIB_CONFIG_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_UPDATE_TIP_CONFIG_SEND_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_UPDATE_TIP_OBJECT_CONFIG_SEND_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_UPDATE_TIP_LIB_CONFIG_SEND_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_INSTALL_ALARM_LIB)
        )
    {
      sreqstr = data ;
    }
    else
    {
      sreqstr = "{\"action\":\"" + data + "\"}";
    }
    DEBUG_LOG("PostRequest with data: "<< sreqstr.toStdString() <<" Url: "<<sUrl.toStdString());
    ModelUpdater::postrequest(sreqstr, sUrl);
  }
}
#if defined WORKSTATION || defined RERUN
/*!
* @fn       onsslavailable
* @param    int - Return result
* @param    QString- strReplyData
* @return   None
* @brief    This slot will call when ssl signal is receieved.
*/
void CommandModel::onsslavailable(int hr, QString strReplyData)
{
  emit sslerrorreceived(hr, strReplyData, m_modelnumber);
}
#endif
/*!
* @fn       onModelResponse
* @param    int - Return result
* @param    QString- strReplyData
* @return   None
* @brief    This slot will call when model query response will be received from server.
*/
void CommandModel::onmodelresponse(int hr, QString strReplyData)
{
  if(hr == Errors::SUCCESS)
  {
    DEBUG_LOG("Command send succeeded for ModelNo:" << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber) << " ReplyData: " << strReplyData.toStdString());
    QJsonParseError jsonError;
    if ((m_modelnumber == QMLEnums::COMMAND_IMAGEQUALITY_TEST_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_RADIATIONSURVEY_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_RADIATIONSURVEY_CANCEL_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_REPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_TIP_CONFIG_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_TIP_LIB_CONFIG_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_USER_DB_EXPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_KEY_EXPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_KEY_IMPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_USER_DB_IMPORT_MODEL)
        || (m_modelnumber == QMLEnums::COMMAND_USERADMIN_KEY_GENERATE_MODEL)
        )
    {
      QJsonDocument jsondoc = QJsonDocument::fromJson(strReplyData.toUtf8(), &jsonError);
      if (jsonError.error == QJsonParseError::NoError)
      {
        QJsonObject jobj =  jsondoc.object();
        QVariantMap data =  jobj.toVariantMap();
        DEBUG_LOG("taskinfo received with data: " <<data.first().toString().toStdString());
        QVariantList listobj;
        listobj << data;
        emit taskInfoReceived(listobj,m_modelnumber);
        emit authstatuschanged(hr, strReplyData, m_modelnumber);
      }
      else
      {
        emit authstatuschanged(hr, strReplyData, m_modelnumber);
      }
    }
    else if(m_modelnumber == QMLEnums::COMMAND_GET_VERSION)
    {
      QJsonDocument jsondoc = QJsonDocument::fromJson(strReplyData.toUtf8(), &jsonError);
      QJsonObject jobj =  jsondoc.object();
      QVariantMap data =  jobj.toVariantMap();
      DEBUG_LOG("Antivirus latest version: " <<data.first().toString().toStdString());
      QString version = data.value("version").toString();
      emit authstatuschanged(hr, version, m_modelnumber);
    }
    else if(m_modelnumber ==QMLEnums::COMMAND_GET_VERSION_LIST)
    {
      QJsonDocument jsondoc = QJsonDocument::fromJson(strReplyData.toUtf8(), &jsonError);
      QJsonObject jobj =  jsondoc.object();
      QVariantMap data =  jobj.toVariantMap();
      DEBUG_LOG("Antivirus version list: " <<data.first().toString().toStdString());
      QVariantList listobj;
      listobj << data;
    }
    else
    {
      emit authstatuschanged(hr, strReplyData, m_modelnumber);
    }
  }
  else
  {
    emit authstatuschanged(hr, strReplyData, m_modelnumber);
    ERROR_LOG("Command send failed for ModelNo:" << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber) << " ReplyData: " << strReplyData.toStdString());
  }
}
}  // end of namespace ws
}  // end of namespace analogic
