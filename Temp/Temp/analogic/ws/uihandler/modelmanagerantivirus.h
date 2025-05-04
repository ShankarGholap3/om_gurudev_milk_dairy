/*!
* @file     modelmanagerantivirus.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view for Antivirus.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

/*#ifndef MODELMANAGERANTIVIRUS_H
#define MODELMANAGERANTIVIRUS_H*/

#include <QTimer>
#include <analogic/ws/uihandler/modelmanager.h>
#include <analogic/ws/common.h>
#include <analogic/ws/uihandler/taskmanager.h>
#include <analogic/ws/uihandler/modelmanager.h>
#include <analogic/ws/uihandler/commandmodel.h>
#include <analogic/ws/uihandler/authenticationmodel.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <utility.h>
#include <analogic/ws/common/utility/usbstorageservice.h>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <analogic/ws/common/accesslayer/antivirusaccessinterface.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{
namespace ws
{
class TaskManager;

#define ANTIVIRUDS_UPLOAD_SERVELET "/nsswebservice/package"

/*!
 * \class   ModelManagerAntiVirus
 * \brief   This class contains variable and function related to
 *          updating the data for current screen view of antivirus.
 */
class ModelManagerAntiVirus: public ModelManager
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelManagerAntiVirus
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ModelManagerAntiVirus.
    */
    explicit ModelManagerAntiVirus(QObject *parent = NULL);

    /*!
    * @fn       ModelManagerAntiVirus
    * @param    None
    * @return   None
    * @brief    Destructor for class iModelManagerAntiVirus.
    */
    virtual ~ModelManagerAntiVirus();

    /*!
     * @fn       init
     * @param    None
     * @return   None
     * @brief    on thread start this function will initialize models.
     */
    void init();

    /*!
     * @fn      onExit
     * @param   None
     * @return  None
     * @brief Function will call on exit of threads
     */
    void onExit();
    /*!
     * @fn      setUploadReply
     * @param   QNetworkReply*
     * @return  None
     * @brief    set Network Reply instance
     */
    void setUploadReply(QNetworkReply *reply);
    /*!
     * @fn      getUploadReply
     * @param   None
     * @return  QNetworkReply*
     * @brief   get Network Reply instance
     */
    QNetworkReply* getUploadReply();

public slots:

    /*!
     * @fn       onUpdateModels
     * @param    QVariantList
     * @param    QMLEnums::ModelEnum model name
     * @return   void
     * @brief    This slot will call on data updation from rest.
     */
    void onUpdateModels(QVariantList map, QMLEnums::ModelEnum modelname);

    /*!
     * @fn      ontaskComplete
     * @param   QString downloadpath - information on finished download
     * @param   QString sDownloadError - Error occured during download
     * @param   QMLEnums::ModelEnum modelnum - modelnum for which file is downloaded
     * @return  None
     * @brief   slot call on task comlete
     */
    void ontaskComplete(QString downloadpath, QString sDownloadError, QMLEnums::ModelEnum modelnum);

    /*!
     * @fn      onauthstatuschanged
     * @param   int - error
     * @param   QString - status
     * @param   int - modelnum
     * @return  void
     * @brief   Function will call on authentication status changed
     */
    void onauthstatuschanged(int hr, QString status,int modelno);
#ifdef WORKSTATION
    /*!
     * @fn      onsslerrorreceived
     * @param   int - error
     * @param   QString - status
     * @param   int - modelno
     * @return  void
     * @brief   Function will call on onsslerrorreceived
     */
    void onsslerrorreceived(int hr, QString status,int modelno);
#endif

    /*!
    * @fn       onTaskTimeout
    * @param    None
    * @return   None
    * @brief    slots check if task has got timeout.
    */
    void onTaskTimeout();
    /*!
    * @fn       onCommandButtonEvent
    * @param    QMLEnums::ModelEnum  - modelname
    * @param    QString data to be post
    * @return   void
    * @brief    This slot will call on command button clicked.
    */
    void onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data);

    /*!
    * @fn       uploadDone
    * @param    None
    * @return   None
    * @brief    slot to call after upload success
    */
    void uploadDone();
    /*!
    * @fn       uploadProgress
    * @param    qint64 val1
    * @param    qint64 val2
    * @return   None
    * @brief    slot to call for file upload progress
    */
    void uploadProgress(qint64 val1, qint64 val2);

private:
    /*!
    * @fn       ModelManagerAntiVirus();
    * @param    ModelManagerReportAdmin&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelManagerAntiVirus(const ModelManagerAntiVirus& modelManagerAntiVirus);

    /*!
    * @fn       operator=
    * @param    ModelManagerAntiVirus&
    * @return   ModelManagerAntiVirus&
    * @brief    declaration for private assignment operator.
    */
    ModelManagerAntiVirus& operator= (const ModelManagerAntiVirus& modelManagerAntiVirus);

    /*!
     * @fn       initRestMap
     * @param    None
     * @return   None
     * @brief    Initialize model rest mapping.
     */
    void initRestMap();

    /*!
     * @fn       initRestContentMap();
     * @param    None
     * @return   None
     * @brief    Initialize model rest contentType mapping
     */
    void initRestContentMap();

    /*!
    * @fn      sendMultipartFile
    * @param   QString filePath
    * @param   QString urlPath
    * @return  None
    * @brief   Function will append data to file
    */
    void sendMultipartFile(QString , QString);

    //TaskManager**                           m_rTaskStatusManager;    //!< handle for taskmanager
    QMap<QMLEnums::ModelEnum, QString>      m_rrestmap;              //!< map for rest URL
    QMap<QMLEnums::ModelEnum, QByteArray>   m_rrestContentMap;       //!< map for rest content type
    QNetworkReply                           *m_uploadReply;         //!< handle for network reply
    QFile                                   *m_file;                //!< File pointer
    QNetworkAccessManager                   *m_networkManager;      //!< Handle For Network
    QHttpMultiPart                          *m_multiPart;           //!< http mulipart handler
};
}  // end of namespace ws
}  // end of namespace analogic


//#endif // MODELMANAGERANTIVIRUS_H
