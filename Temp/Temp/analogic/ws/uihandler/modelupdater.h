/*!
* @file     modelupdater.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelUpdater
*           which handles updating of models.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_MODELUPDATER_H_
#define ANALOGIC_WS_UIHANDLER_MODELUPDATER_H_

#include <QDebug>
#include <QString>
#include <QJsonValue>
#include <QJsonObject>
#include <QVariantMap>
#include <rialreply.h>
#include <QJsonDocument>
#include <QSharedPointer>
#include <QDesktopServices>
#include <analogic/ws/common.h>
#include <rialdownloader.h>
#include <QFile>




class Rial;

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   ModelUpdater
 * \brief   This class contains variable and function related to
 *          updating of models.
 */
class ModelUpdater : public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelUpdater
    * @param    QMLEnums::ModelEnum - model number
    * @param    QString - url
    * @param    QByteArray contentType
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor for class ModelUpdater.
    */
    explicit ModelUpdater(QMLEnums::ModelEnum modelnumber, QString strresturl,
                          QByteArray contentType = DEFAULT_CONTENT_TYPE, QObject* parent = NULL);

    /*!
    * @fn       ~ModelUpdater
    * @param    None
    * @return   None
    * @brief    Destructor for class ModelUpdater.
    */
    virtual ~ModelUpdater();

    /*!
    * @fn       getrequest
    * @param    QString sUrl
    * @return   None
    * @brief    This function is responsible for getting ModelUpdater.
    */
    virtual void getrequest(QString sUrl = "");

    /*!
    * @fn       getdwrequest
    * @param    QString sUrl
    * @return   None
    * @brief    This function is responsible for getting ModelUpdater.
    */
    virtual void getdwrequest(QString sUrl = "");

    /*!
    * @fn       postrequest
    * @param    QString data
    * @param    QString sUrl
    * @return   None
    * @brief    This function is responsible for posting ModelUpdater.
    */
    virtual void postrequest(QString data, QString sUrl = "");

    /*!
    * @fn       init
    * @param    None
    * @return   None
    * @brief    This function is responsible for initialization of rial class members.
    */
    void init();

    /*!
    * @fn       getModelNumber
    * @param    None
    * @return   QMLEnums::ModelEnum = model number
    * @brief    This function is responsible for getting model number.
    */
    QMLEnums::ModelEnum  getModelNumber() const;

    /*!
    * @fn       getUrl
    * @param    None
    * @return   QString
    * @brief    This function is responsible for getting url.
    */
    QString  getUrl(void) const;

    /*!
    * @fn       setUrl
    * @param    QString
    * @return   None
    * @brief    This function is responsible for setting url.
    */
    void  setUrl(QString url);

signals:

    /*!
    * @fn       updateModel();
    * @param    QVariantList - mapobj
    * @param    QMLEnums::ModelEnum - m_modelname
    * @return   None
    * @brief    called on update models.
    */
    void updateModel(QVariantList mapobj, QMLEnums::ModelEnum m_modelname);

    /*!
    * @fn       onauthstatuschanged();
    * @param    int - error code
    * @param    QString - status
    * @param    int - modelno
    * @return   None
    * @brief    called on authentication status changed
    */
    void authstatuschanged(int hr, QString status, int modelno);


    /*!
    * @fn       sslerrorreceived();
    * @param    int - error code
    * @param    QString - status
    * @param    int - modelno
    * @return   None
    * @brief    called on sslerrorreceived
    */
    void sslerrorreceived(int hr, QString status, int modelno);
#ifdef WORKSTATION
    /*!
    * @fn       sdswClientExited
    * @param    int - exit code
    * @return   None
    * @brief    called on sdswclient logout/exit
    */
    void sdswClientExited(int exitCode);
#endif

    /*!
    * @fn       taskInfoReceived();
    * @param    QVariantList - json response
    * @param    QMLEnums::ModelEnum - modelnumber
    * @return   None
    * @brief    called  when response is received on a post request special case: raw data auth status
    */
    void taskInfoReceived(QVariantList jsonResponse, QMLEnums::ModelEnum modelnumber);

public slots:

    /*!
    * @fn       onmodelresponse
    * @param    int - Result
    * @param    QString - replydata
    * @return   None
    * @brief    This function is responsible for updating model slot.
    */
    virtual void onmodelresponse(int hr, QString replydata);
#if defined WORKSTATION || defined RERUN
    /*!
    * @fn       onsslavailable
    * @param    int - Result
    * @param    QString - replydata
    * @return   None
    * @brief    This function handles ssl signal.
    */
    virtual void onsslavailable(int hr, QString replydata);
#endif
    /*!
    * @fn       onDownladAvailable();
    * @param    int - hr
    * @param    QByteArray - data
    * @return   None
    * @brief    Function will call on dowload available.
    */
    void onDwnldAvailable(int hr, QByteArray data);

    /*!
    * @fn       onDownloadFinished();
    * @param    None
    * @return   None
    * @brief    Function will call on dowload finished.
    */
    void onDwnldFinished();
private:
    /*!
    * @fn       ModelUpdater();
    * @param    ModelUpdater&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelUpdater(const ModelUpdater& modelUpdater);

    /*!
    * @fn       ModelUpdater();
    * @param    ModelUpdater&
    * @return   ModelUpdater&
    * @brief    declaration for private assignment operator.
    */
    ModelUpdater& operator=(const ModelUpdater& modelUpdater);

protected:
    QString                                         m_strUrl;                   //!< Rest URL for model
    QByteArray                                      m_contentType;              //!< Rest content type
    QMLEnums::ModelEnum                             m_modelnumber;              //!< model number
    Rial*                                           m_connectionAccessLayer;    //!< Handle for connection interface
    QSharedPointer<RialReply>                       m_reply;
    boost::shared_ptr<QFile>                        m_fileptr;
    QSharedPointer<RialDownloader>                  m_replydownload;
    QString                                         m_filepath;
    QString                                         m_sDownloadError;

};
}  // end of namespace ws
}  // end of namespace analogic


#endif  // ANALOGIC_WS_UIHANDLER_MODELUPDATER_H_

