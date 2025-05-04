/*!
* @file     modelmanagerscanneradmin.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view of scanner admin.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_MODELMANAGERSCANNERADMIN_H_
#define ANALOGIC_WS_UIHANDLER_MODELMANAGERSCANNERADMIN_H_

#include <QTimer>
#include <analogic/ws/uihandler/modelmanager.h>
#include <analogic/ws/common/utility/xmlservice.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class TaskManager;

/*!
 * \class   ModelManagerScannerAdmin
 * \brief   This class contains variable and function related to
 *          updating the data for current screen view of scanner admin.
 */
class ModelManagerScannerAdmin: public ModelManager
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelManagerScannerAdmin
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ModelManagerScannerAdmin.
    */
    explicit ModelManagerScannerAdmin(QObject *parent = NULL);

    /*!
    * @fn       ModelManagerScannerAdmin
    * @param    None
    * @return   None
    * @brief    Destructor for class ModelManagerScannerAdmin.
    */
    virtual ~ModelManagerScannerAdmin();

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

signals:

    /*!
    * @fn       sdswClientExited
    * @param    int - exit code
    * @return   None
    * @brief    called on sdswclient logout/exit
    */
    void sdswClientExited(int exitCode);

public slots:    
    /*!
     * @fn       onScreenModelChanged(ModelList oModels)
     * @param    ModelList
     * @return   None
     * @brief    slot call on screen model changed
     */
    void onScreenModelChanged(ModelList oModels);

    /*!
     * @fn       onprocessUpdateModels
     * @param    None
     * @return   None
     * @brief    process models.
     */
    void onprocessUpdateModels();

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
     * @param   int - modelno
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

private:
    /*!
    * @fn       ModelManagerScannerAdmin();
    * @param    ModelManagerScannerAdmin&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelManagerScannerAdmin(const ModelManagerScannerAdmin& modelManagerScannerAdmin);

    /*!
    * @fn       operator=
    * @param    ModelManagerScannerAdmin&
    * @return   ModelManagerScannerAdmin&
    * @brief    declaration for private assignment operator.
    */
    ModelManagerScannerAdmin& operator= (const ModelManagerScannerAdmin& modelManagerScannerAdmin);

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

    bool                                    m_bpaused;              //!< boolean for pause model update
    ModelList                               m_scrmodels;            //!< handle for model list per active screen
    TaskManager**                           m_pTaskStatusManager;   //!< handle for taskmanager
    boost::shared_ptr<QTimer>               m_pUpdateTimer;         //!< handle for timer
    QTimer*                                 m_networreconnecttimer; //!< handle for reconnect with sever.
    QMap<QMLEnums::ModelEnum, QString>      m_restmap;              //!< map for rest URL
    QMap<QMLEnums::ModelEnum, QByteArray>   m_restContentMap;       //!< map for rest content type
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_MODELMANAGERSCANNERADMIN_H_


