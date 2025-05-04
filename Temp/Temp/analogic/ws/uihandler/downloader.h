/*!
* @file     downloader.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to Downloader
*           which is responding for downloading raw data file.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_DOWNLOADER_H_
#define ANALOGIC_WS_UIHANDLER_DOWNLOADER_H_

#include <boost/shared_array.hpp>
#include <QObject>
#include <analogic/ws/common.h>
#include <QCryptographicHash>
#include <rial.h>
#include <QFile>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
enum TaskStatus
{
    NOT_STARTED,
    STARTED,
    PAUSED,
    COMPLETED,
    FAILED,
    TIMEOUT,
    CANCELED
};

/*!
 * \class   Downloader
 * \brief   This class contains variable and function related to
 *          Downloader.
 */

class Downloader : public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       Downloader
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor for class Downloader.
    */
    explicit Downloader(QObject *parent = 0);

    /*!
    * @fn       setDownloadLink();
    * @param    QString - download link
    * @param    QString - checksum
    * @return   None
    * @brief    This function is responsible for setting download link.
    */
    void setDownloadLink(QString download , QString sbase64cryptoSha1hash = "");

    /*!
    * @fn       setFilePathToSave();
    * @param    QString - sFilePath
    * @return   None
    * @brief    This function is responsible for setting file path to save.
    */
    void setFilePathToSave(QString sFilePath);

    /*!
    * @fn       start();
    * @param    None
    * @return   None
    * @brief    This function is responsible for start download.
    */
    void start(QMLEnums::ModelEnum modelno);

    /*!
    * @fn       ~Downloader();
    * @param    None
    * @return   None
    * @brief    Destructor for class Downloader.
    */
    virtual ~Downloader();

    /*!
    * @fn       setSha1Base16Checksum();
    * @param    QString - sbase64cryptoSha1hash
    * @return   None
    * @brief    This function is responsible for setting check sum.
    */
    void setSha1Base16Checksum(QString sbase64cryptoSha1hash);

    /*!
    * @fn       getStatus
    * @param    QString - task
    * @return   TaskStatus
    * @brief    gets status of download task
    */
    TaskStatus getStatus(QString task);

    /*!
    * @fn       setTaskId
    * @param    QString - taskID
    * @return   None
    * @brief    set task id
    */
    void setTaskId(QString task);

signals:

    /*!
    * @fn       downloadPrctComplete();
    * @param    int - prct
    * @return   None
    * @brief    Function will call on download complete.
    */
    void downloadPrctComplete(int prct);

    /*!
    * @fn       taskComplete();
    * @param    QString - filepath
    * @param    QString - sDownloadError
    * @return   None
    * @brief    Function will call on task complete.
    */
    void taskComplete(QString filepath, QString sDownloadError);

public slots:
    /*!
    * @fn       onDownladAvailable();
    * @param    int - hr
    * @param    QByteArray - data
    * @return   None
    * @brief    Function will call on dowload available.
    */
    void onDownladAvailable(int hr, QByteArray data);

    /*!
    * @fn       onDownloadFinished();
    * @param    None
    * @return   None
    * @brief    Function will call on dowload finished.
    */
    void onDownloadFinished();

private:
    QString                                 m_strTask;
    QString                                 m_filepath;
    QString                                 m_downloadlink;
    QString                                 m_sDownloadError;
    QString                                 m_base16Sha1CryptoHash;
    TaskStatus                              m_taskStatus;
    Rial                                    *saal;
    boost::shared_ptr<QFile>                m_fileptr;
    boost::shared_ptr<QCryptographicHash>   m_phashFuncs;
    QSharedPointer<RialDownloader>           m_preply;
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_DOWNLOADER_H_

