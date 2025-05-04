/*!
* @file     authenticationmodel.cpp
* @author   Agiliad
* @brief    This file contains functions related to Authenticationmodel
*           which handles authentication of models.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QFileInfo>
#include <QProcess>
#include <rial.h>
#include <utility.h>
#include <analogic/ws/uihandler/authenticationmodel.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#ifdef WORKSTATION
#include <analogic/ws/SSLErrorEnums.h>
#endif
namespace analogic
{
namespace ws
{
/*!
* @fn       Authenticationmodel
* @param    QMLEnums::ModelEnum - model number
* @param    QString - url
* @param    QByteArray contentType
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class Authenticationmodel.
*/
Authenticationmodel::Authenticationmodel(
        QMLEnums::ModelEnum modelnumber, QString strresturl, QByteArray contentType,
        QObject* parent):ModelUpdater(modelnumber, strresturl, contentType, parent)
{
}

/*!
* @fn       postrequest();
* @param    QString data
* @param    QString sUrl
* @return   None
* @brief    This function is responsible for posting model.
*/
void Authenticationmodel::postrequest(QString data, QString sUrl)
{
    DEBUG_LOG("posting request Url: "<<sUrl.toStdString());
    if(m_modelnumber < QMLEnums::ModelEnum::SCANNERADMIN_MODELS_END)
    {
        THROW_IF_FAILED((m_connectionAccessLayer == NULL ? Errors::E_POINTER : Errors::S_OK));
    }
    if (m_modelnumber == QMLEnums::ModelEnum::USERADMIN_AUTHENTICATION_MODEL)
    {

        QStringList credentialStringList = data.split(",");
        if (credentialStringList.size()==0)
        {
            emit authstatuschanged(Errors::E_FAIL, tr("Useradmin service Username and password is empty"), QMLEnums::ModelEnum::USERADMIN_AUTHENTICATION_MODEL);
            return;
        }

        QString username = credentialStringList.at(0);
        QString password = credentialStringList.at(1);

        QString credential = "{\"user\":\""+ username + "\",\"password\":\""
                + Utility::generateSha256(password)+ "\" }";
        ModelUpdater::postrequest(credential, sUrl);

    }
    else if(m_modelnumber == QMLEnums::ModelEnum::AUTHENTICATIN_MODEL)
    {
      #ifdef WORKSTATION
        Errors::RESULT res =  validateandStartServiceTool(data);
        if((res != Errors::S_OK) &&
                (res != Errors::E_POINTER) /*&& data.isEmpty()*/)
        {
            QString encryptedPassword = Utility::generateSha256(
                        (QString(DEFAULT_SERVICE_PASSWORD)).toLower());
            QString creden = Utility::createServiceLoginMsg(DEFAULT_SERVICE_USERNAME, encryptedPassword,
                                                            WorkstationConfig::getInstance()->getLanguageCode(), SERVICE_APPTYPE);
            ModelUpdater::postrequest(creden, sUrl);
        }

        else if (res == Errors::S_OK)
        {
            emit authstatuschanged(Errors::S_OK, tr("SDSW Client has been started."), QMLEnums::ModelEnum::AUTHENTICATIN_MODEL);
        }
        #endif
    }
    else
    {
      if (data.isEmpty())
      {
          QString encryptedPassword = Utility::generateSha256(
                     (QString(DEFAULT_SERVICE_PASSWORD)).toLower());
          QString creden = "{\"user\":\"" + QString(DEFAULT_SERVICE_USERNAME)
                  + "\",\"password\":\""+ encryptedPassword +"\"}";
          ModelUpdater::postrequest(creden, sUrl);
      }

      else
      {
          ModelUpdater::postrequest(data, sUrl);
      }
    }
}

#ifdef WORKSTATION
/*!
* @fn       validateandStartServiceTool
* @param    QString data
* @return   Errors::RESULT
* @brief    This function is responsible for validating user credential for service tool.
*/
Errors::RESULT Authenticationmodel::validateandStartServiceTool(QString data )
{
    DEBUG_LOG("Modelnumber: "<< QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber));
    QString username, password;
    QJsonDocument jsondoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jobj =  jsondoc.object();
    QVariantMap mapobj =  jobj.toVariantMap();

    username = mapobj.value("user").toString();
    password = mapobj.value("password").toString();

    if (data == WORKSTATION_APPTYPE)
    {
        username = DEFAULT_SERVICE_USERNAME;
        password = Utility::generateSha256((QString(DEFAULT_SERVICE_PASSWORD)).toLower());
    }

    if(Utility::isServiceToolCredentials(username, password))
    {
        INFO_LOG("Login with Service tool credentials");
        QString program = WorkstationConfig::getInstance()->getSDSWClientBinarypath();

        QFileInfo file(program);
        if (file.exists()) {
            QStringList arguments;
            arguments << username << password << WorkstationConfig::getInstance()->getLanguageCode();
            QProcess *process = new QProcess();
            connect(process, SIGNAL(finished(int)), this, SIGNAL(sdswClientExited(int )));
            process->setWorkingDirectory(file.absolutePath());
            INFO_LOG("Starting SDSWClient located at:" << file.absoluteFilePath().toStdString());
            process->start(program, arguments);
            return Errors::S_OK;
        } else {
            ERROR_LOG("SDSWClient file is not present at configured path: " << program.toStdString());
            emit authstatuschanged(Errors::FILE_NOT_PRESENT, tr("SDSWClient file is not present at configured path:"), m_modelnumber);
            return Errors::E_POINTER;
        }
    }
    else
    {
        INFO_LOG("Login with Workstation credentials");
        return Errors::E_FAIL;
    }
    return Errors::E_FAIL;
}

/*!
* @fn       onsslavailable():
* @param    int - Result
* @param    QString - Data
* @return   None
* @brief    This function is responsible handling ssl signal.
*/
void Authenticationmodel::onsslavailable(int hr, QString data)
{
    emit sslerrorreceived(hr, data, m_modelnumber);
}
#endif
/*!
* @fn       onmodelresponse():
* @param    int - Result
* @param    QString - Data
* @return   None
* @brief    This function is responsible for updating model.
*/
void Authenticationmodel::onmodelresponse(int hr, QString data)
{
    DEBUG_LOG("Modelnumber: "<< QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_modelnumber) <<" Data : "<< data.toStdString() << " hr response:" <<hr);
  if(hr == Errors::LOGIN_SUCCESS)
    {
        QByteArray rawData = data.toLocal8Bit();
        // Parse JSON document
        QJsonDocument doc(QJsonDocument::fromJson(rawData));
        // Get JSON object
        QJsonObject json = doc.object();

        QString strtoken = json["authToken"].toString();

        m_connectionAccessLayer->settoken(strtoken);

        if (!strtoken.isEmpty()){
            emit authstatuschanged(hr, strtoken, m_modelnumber);
        }
        else
        {
            ERROR_LOG("Login Failed. Token is Empty ");
            emit authstatuschanged(hr, tr("Error: Authentication failed. Received Token is empty."), m_modelnumber);
        }
    }
    else
    {
        emit authstatuschanged(hr, data, m_modelnumber);
    }

}
}  // end of namespace ws
}  // end of namespace analogic
