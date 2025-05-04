/*!
* @file     modelmanagertipconfigadmin.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view for tip config admin.
* @date     Aug, 02 2018
*
(c) Copyright <2018-2019> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_UIHANDLER_MODELMANAGERTIPCONFIGADMIN_H_
#define ANALOGIC_WS_UIHANDLER_MODELMANAGERTIPCONFIGADMIN_H_

#include <QTimer>
#include <analogic/ws/uihandler/modelmanager.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{
namespace ws
{
class TaskManager;

/*!
 * \class   ModelManagerTipConfigAdmin
 * \brief   This class contains variable and function related to
 *          updating the data for current screen view of tip config admin.
 */

class ModelManagerTipConfigAdmin: public ModelManager
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelManagerTipConfigAdmin
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ModelManagerTipConfigAdmin.
    */
    explicit ModelManagerTipConfigAdmin(QObject *parent = NULL);

    /*!
    * @fn       ModelManagerTipConfigAdmin
    * @param    None
    * @return   None
    * @brief    Destructor for class ModelManagerTipConfigAdmin.
    */
    virtual ~ModelManagerTipConfigAdmin();

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
     * @fn       populateTIPInfo
     * @param    None
     * @return   None
     * @brief    Populate TIP info
     */
    void populateTIPInfo(QVariantList data);

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
     * @param   int - modelnum
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
    * @fn       ModelManagerTipConfigAdmin();
    * @param    ModelManagerTipConfigAdmin&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelManagerTipConfigAdmin(const ModelManagerTipConfigAdmin& modelManagerTipConfigAdmin);

    /*!
    * @fn       operator=
    * @param    ModelManagerTipConfigAdmin&
    * @return   ModelManagerTipConfigAdmin&
    * @brief    declaration for private assignment operator.
    */
    ModelManagerTipConfigAdmin& operator= (const ModelManagerTipConfigAdmin& modelManagerTipConfigAdmin);

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

    QMap<QMLEnums::ModelEnum, QString>      m_rrestmap;              //!< map for rest URL
    QMap<QMLEnums::ModelEnum, QByteArray>   m_rrestContentMap;       //!< map for rest content type
    TaskManager**                           m_rTaskStatusManager;    //!< handle for taskmanager
    QTimer*                                 m_networreconnecttimer;  //!< handle for reconnect with sever.
};
}  // end of namespace ws
}  // end of namespace analogic
#endif // MODELMANAGERTIPCONFIGADMIN_H
