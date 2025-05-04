/*!
* @file     taskinfomodel.cpp
* @author   Agiliad
* @brief    This file contains functions related to TaskInfoModel
*           which handles Task info.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/taskinfomodel.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       TaskInfoModel
* @param    QMLEnums::ModelEnum - model number
* @param    QString - url
* @param    QByteArray contentType
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class CommandModel.
*/
TaskInfoModel::TaskInfoModel(QMLEnums::ModelEnum modelnumber, QString strresturl,
                             QByteArray contentType, QObject* parent ) :
    ModelUpdater(modelnumber, strresturl, contentType, parent)
{
}

/*!
* @fn       setTaskID
* @param    QString - preplysTaskID
* @return   None
* @brief    This function is responsible for setting TaskID.
*/
void TaskInfoModel::setTaskID(QString sTaskID)
{
    DEBUG_LOG("Setting TaskID to: "<<sTaskID.toStdString());
    m_sTaskID = sTaskID;
}

/*!
* @fn       getTaskID
* @param    None
* @return   QString - sTaskID
* @brief    This function is responsible for getting TaskID.
*/
QString TaskInfoModel::getTaskID()
{
    DEBUG_LOG("Current TaskID is: "<<m_sTaskID.toStdString());
    return m_sTaskID;
}

/*!
* @fn       getrequest();
* @param    QString - sUrl
* @return   None
* @brief    This function is responsible for updating model.
*/
void TaskInfoModel::getrequest(QString sUrl)
{
    DEBUG_LOG("GetRequest for task status...");
    sUrl = m_strUrl + "/" + m_sTaskID;
    ModelUpdater::getrequest(sUrl);
}

/*!
* @fn       postrequest();
* @param    None
* @return   None
* @brief    This function is responsible for posting model.
*/
void TaskInfoModel::postrequest()
{
    DEBUG_LOG("PostRequest for task acknowledgement...");
    QString data = "{ \"action\": \"Ack\"}";
    QString sUrl = m_strUrl + "/" + m_sTaskID;
    ModelUpdater::postrequest(data, sUrl);
}
}  // namespace ws
}  // namespace analogic
