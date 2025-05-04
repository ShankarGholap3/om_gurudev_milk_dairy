/*!
* @file     taskmanager.h
* @author   Agiliad
* @brief    This file contains functions related to TaskManager
*           which handles task status update and downloading of files.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_TASKMANAGER_H_
#define ANALOGIC_WS_UIHANDLER_TASKMANAGER_H_

#include <boost/shared_ptr.hpp>
#include <QVariant>
#include <QTimer>
#include <QDateTime>
#include <analogic/ws/common.h>
#include <errors.h>
#include <rialcommon.h>
#include <analogic/ws/uihandler/downloader.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class TaskInfoModel;

/*!
 * \class   TaskManager
 * \brief   This class contains variable and function related to
 *          getting task status and downloading of files.
 */
class TaskManager: public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       TaskManager
    * @param    None
    * @return   None
    * @brief    Constructor for class TaskManager.
    */
    TaskManager(int taskrefreshinterval, int tasktimeoutcheckinterval, QMLEnums::ModelEnum updatetaskmodel);

    /*!
    * @fn       ~TaskManager
    * @param    None
    * @return   None
    * @brief    Destructor for class TaskManager.
    */
    ~TaskManager();
    /*!
       * @fn       validateTaskMessage
       * @param    QVariantMap
       * @return   bool - result
       * @brief    validate message.
       */
    bool validateTaskMessage(QVariantMap commandresmap);
    /*!
       * @fn       cancelTask
       * @param    None
       * @return   None
       * @brief    Cancel Task
       */
    void cancelTask();

signals:

    /*!
    * @fn       updateModel();
    * @param    QVariantList - list
    * @param    QMLEnums::ModelEnum - m_modelname
    * @return   None
    * @brief    called on update models.
    */
    void updateModels(QVariantList list, QMLEnums::ModelEnum modelname);

    /*!
    * @fn       onauthstatuschanged();
    * @param    int - error code
    * @param    QString - status
    * @param    int - modelno
    * @return   None
    * @brief    called on authentication status changed
    */
    void authstatuschanged(int, QString, int);

    /*!
    * @fn       taskcomplete
    * @param    QString - filepath
    * @param    QString - sDownloadError
    * @param    QMLEnums::ModelEnum - modelnumber
    * @return   None
    * @brief    emitted  when file download task is completed.
    */
    void taskcomplete(QString filename, QString sDownloadError, QMLEnums::ModelEnum modelname);

public slots:

    /*!
    * @fn       onUpdateModel
    * @param    QVariantList - map
    * @param    QMLEnums::ModelEnum - modelname
    * @return   None
    * @brief    This function is responsible for updating model and starting the file download task.
    */
    void onUpdateModel(QVariantList map, QMLEnums::ModelEnum modelname);

    /*!
    * @fn       onAuthStatusChanged
    * @param    int - error code
    * @param    QString - status
    * @param    int - modelno
    * @return   None
    * @brief    called on authentication status changed
    */
    void onAuthStatusChanged(int hr, QString status, int modelno);

    /*!
    * @fn       onTaskComplete
    * @param    QString - filepath downloaded
    * @param    QString - download error
    * @return   None
    * @brief    called on downloading task is finished.
    */
    void onTaskComplete(QString filepath, QString sDownloadError);

    /*!
    * @fn       onprocessStatusRequest
    * @param    None
    * @return   None
    * @brief    called when timer expires to get task status.
    */
    void onprocessStatusRequest();

    /*!
    * @fn       ontaskInfoReceived
    * @param    QVariantList - json response
    * @param    QMLEnums::ModelEnum - modelnumber
    * @return   None
    * @brief    called  when response is received on a post request special case: raw data auth status
    */
    void ontaskInfoReceived(QVariantList commandreslist, QMLEnums::ModelEnum modelname);
    /*!
    * @fn       onTaskTimeout
    * @param    None
    * @return   None
    * @brief    slots check if task has got timeout.
    */
    void onTaskTimeout();
private:
    /*!
    * @fn       TaskManager();
    * @param    TaskManager&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    TaskManager(const TaskManager& taskManager);

    /*!
    * @fn       operator=
    * @param    TaskManager&
    * @return   TaskManager&
    * @brief    declaration for private assignment operator.
    */
    TaskManager& operator= (const TaskManager& taskManager);

    /*!
    * @fn       resetTimer
    * @param    None
    * @return   None
    * @brief    called when m_pStatusTime is to reset with new QTimer.
    */
    void resetTimer();

    QString                             m_taskId;               //!< current taskid
    QString                             m_SecondIQFile;         //!< handle for 2nd file in ImageQuality
    QString                             m_ChecksumSecondIQFile; //!< handle for checksum of 2nd file in ImageQuality
    Downloader                          m_Downloader;           //!< downloader instance
    TaskStatus                          m_taskstatus;
    QMLEnums::ModelEnum                 m_modelnum;             //!< current modelnum
    QMLEnums::ModelEnum                 m_updatemodelnum;       //!< current modelnum
    boost::shared_ptr<QTimer>           m_pStatusTimer;         //!< handle to timer
    boost::shared_ptr<TaskInfoModel>    m_pTaskInfo;            //!< handle to task info model
    QTimer                              m_taskTimeoutTimer;     //!< async task timer interval
    QDateTime                           m_lastresponsetime;     //!< last valid response time
    int                                 m_taskrefreshinterval;  //!< task refresh inteval
    int                                 m_tasktimeout;          //!< task timeout
};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_TASKMANAGER_H_

