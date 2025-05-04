/*!
* @file     downloader.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to Downloader
*           which is responding for downloading raw data file.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDir>
#include <analogic/ws/common/accesslayer/scanneradminaccessinterface.h>
#include <analogic/ws/common/accesslayer/reportadminaccessinterface.h>
#include <analogic/ws/common/accesslayer/useradminaccessinterface.h>
#include <analogic/ws/common/accesslayer/tipconfigadminaccessinterface.h>
#include <analogic/ws/common/accesslayer/tiplibconfigadminaccessinterface.h>
#include <analogic/ws/uihandler/downloader.h>
#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       Downloader
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class Downloader.
*/
Downloader::Downloader(QObject *parent) : QObject(parent)
{
    m_filepath =  QDir::tempPath()+ "/downloaded";
    m_phashFuncs.reset(new QCryptographicHash(  QCryptographicHash::Sha1));
    m_sDownloadError = "";
    m_taskStatus = NOT_STARTED;
    INFO_LOG("Default download path set to: "<<m_filepath.toStdString());
}

/*!
* @fn       setDownloadLink();
* @param    QString - download link
* @param    QString - checksum
* @return   None
* @brief    This function is responsible for setting download link.
*/
void Downloader::setDownloadLink(QString download, QString sbase16cryptoSha1hash)
{
    INFO_LOG("Download link is: "<< download.toStdString() << " base16crypto: "<<sbase16cryptoSha1hash.toStdString());
    m_downloadlink = download;
    m_base16Sha1CryptoHash = sbase16cryptoSha1hash;
}

/*!
* @fn       setSha1Base16Checksum();
* @param    QString - checksum
* @return   None
* @brief    This function is responsible for setting check sum.
*/
void Downloader::setSha1Base16Checksum(QString sbase16cryptoSha1hash)
{
    DEBUG_LOG("Setting check sum to: "<< sbase16cryptoSha1hash.toStdString());
    m_base16Sha1CryptoHash = sbase16cryptoSha1hash;
}

/*!
* @fn       getStatus
* @param    QString - task
* @return   TaskStatus
* @brief    gets status of download task
*/
TaskStatus Downloader::getStatus(QString task)
{
    if (task == m_strTask)
    {
        DEBUG_LOG("Task with id : "<< task.toStdString() <<" has status: "<< m_taskStatus);
        return m_taskStatus;
    }
    else
    {
        DEBUG_LOG("Task with id : "<< task.toStdString() <<" has status: NOT_STARTED");
        return NOT_STARTED;
    }
}

/*!
* @fn       setTaskId
* @param    QString - taskID
* @return   None
* @brief    set task id
*/
void Downloader::setTaskId(QString task)
{
    DEBUG_LOG("Setting Task with id : "<< task.toStdString() <<" and its status to: NOT_STARTED");
    m_strTask = task;
    m_taskStatus = NOT_STARTED;
}

/*!
* @fn       setFilePathToSave();
* @param    QString - sFilePath
* @return   None
* @brief    This function is responsible for setting file path to save.
*/
void Downloader::setFilePathToSave(QString sFilePath)
{
    QStringList pieces = sFilePath.split( "/" );
    QString filename = pieces.value( pieces.length() - 1 );
    m_filepath =  QDir::tempPath()+ "/" + filename;
    INFO_LOG("Writing filename :" << filename.toStdString() << " to path :"<< m_filepath.toStdString());
    if (m_fileptr.use_count() != 0)
    {
        m_fileptr->close();
    }

    m_fileptr.reset(new QFile(m_filepath));
    bool ret = m_fileptr->open( QFile::WriteOnly | QFile::Truncate );
    if (!ret)
    {
        ERROR_LOG("Failed to open file for writing");
    }
}

/*!
* @fn       start();
* @param    None
* @return   None
* @brief    This function is responsible for start download.
*/
void Downloader::start(QMLEnums::ModelEnum modelno)
{
    DEBUG_LOG("Task status is:" << m_taskStatus);
    if (m_taskStatus == NOT_STARTED)
    {
#ifdef WORKSTATION
        INFO_LOG("Starting file download");
        if ((modelno <= QMLEnums::SCANNERADMIN_MODELS_END) && (modelno >= QMLEnums::UPDATE_TASK_STATUS_MODEL))
        {
            saal = ScannerAdminAccessInterface::getInstance();
            THROW_IF_FAILED(((saal == NULL) ? Errors::E_POINTER : Errors::S_OK));
        }
        else if ((modelno <= QMLEnums::REPORT_ADMIN_MODELS_END) && (modelno >= QMLEnums::UPDATE_REPORT_TASK_STATUS_MODEL))
        {
            saal = ReportAdminAccessInterface::getInstance();
            THROW_IF_FAILED(((saal == NULL) ? Errors::E_POINTER : Errors::S_OK));
        }
        else
#endif
        if ((modelno <= QMLEnums::USERADMIN_MODELS_END) && (modelno >= QMLEnums::UPDATE_USERADMIN_TASK_STATUS_MODEL))
        {
            saal = UserAdminAccessInterface::getInstance();
            THROW_IF_FAILED(((saal == NULL) ? Errors::E_POINTER : Errors::S_OK));
        }
#ifdef WORKSTATION
        else if ((modelno <= QMLEnums::TIP_CONFIG_ADMIN_MODELS_END) && (modelno >= QMLEnums::UPDATE_TIP_CONFIG_TASK_STATUS_MODEL))
        {
            saal = TipConfigAdminAccessInterface::getInstance();
            THROW_IF_FAILED(((saal == NULL) ? Errors::E_POINTER : Errors::S_OK));
        }
        else if ((modelno <= QMLEnums::TIP_LIB_CONFIG_ADMIN_MODELS_END) && (modelno >= QMLEnums::UPDATE_TIP_LIB_CONFIG_TASK_STATUS_MODEL))
        {
            saal = TipLibConfigAdminAccessInterface::getInstance();
            THROW_IF_FAILED(((saal == NULL) ? Errors::E_POINTER : Errors::S_OK));
        }
#endif
        else
        {
            ERROR_LOG("File download request failed for Task id:" << m_taskStatus<<" model number"<< QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(modelno));
            return;
        }

        m_preply.reset(saal->getdwrequest(m_downloadlink));
        if ( !m_preply.isNull() )
        {
            connect(m_preply.data(), &RialReply::downladAvailable, this,
                    &Downloader::onDownladAvailable, Qt::QueuedConnection);
            connect(m_preply.data(), &RialReply::downloadFinished, this,
                    &Downloader::onDownloadFinished, Qt::QueuedConnection);
        }
        else
        {
            ERROR_LOG("NULL reply for GET " << m_downloadlink.toStdString().c_str());
        }

        m_taskStatus = STARTED;
    }
}

/*!
* @fn       ~Downloader();
* @param    None
* @return   None
* @brief    Destructor for class Downloader.
*/
Downloader::~Downloader()
{
    m_phashFuncs.reset();
    m_fileptr.reset();
}

/*!
* @fn       onDownladAvailable();
* @param    int - hr
* @param    QByteArray - data
* @return   None
* @brief    Function will call on dowload available.
*/
void Downloader::onDownladAvailable(int hr, QByteArray data)
{
    if (hr == 200)
    {
        INFO_LOG("File is available for download");
        if (!m_fileptr.use_count())
        {
            m_fileptr.reset(new QFile(m_filepath));
            INFO_LOG("Downloading file to: "<<m_filepath.toStdString());
            m_fileptr->open( QFile::WriteOnly | QFile::Truncate );
            m_phashFuncs->reset();
        }
        m_fileptr->write(data);
        m_phashFuncs->addData(data);
    }
    else
    {
        ERROR_LOG("File not available for download..."<< "response is : " <<data.toStdString());
        m_sDownloadError = data;
    }
}

/*!
* @fn       onDownloadFinished();
* @param    None
* @return   None
* @brief    Function will call on dowload finished.
*/
void Downloader::onDownloadFinished()
{
    if (m_fileptr.use_count() != 0 && m_sDownloadError == "")
    {
        QByteArray hash = m_phashFuncs->result();

        QString sHashHexencoding = QString(hash.toHex());

        if ( m_base16Sha1CryptoHash != "" &&
             sHashHexencoding == m_base16Sha1CryptoHash)
        {
            INFO_LOG("File download success..");
        }
        else
        {
            m_sDownloadError = tr("File download failed. SHA1 checksum match failed");
            INFO_LOG(m_sDownloadError.toStdString());
        }
        m_phashFuncs->reset();
        m_fileptr->close();
        INFO_LOG("File download task completed for file: "<< m_filepath.toStdString() <<" and error string is: "<< m_sDownloadError.toStdString());
        emit taskComplete(m_filepath, m_sDownloadError);
        m_sDownloadError = "";
        m_taskStatus = COMPLETED;
    }
    else
    {
        if(!m_fileptr)
            ERROR_LOG("Downloader: No file created.");

        if(m_sDownloadError != "")
        {
            emit taskComplete(m_filepath,m_sDownloadError);
            ERROR_LOG("Downloader: " << m_sDownloadError.toStdString().c_str());
        }
    }
    m_sDownloadError = "";
}
}  // end of namespace ws
}  // end of namespace analogic
