/*!
* @file     modelmanagertiplibconfigadmin.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view for tip lib config admin.
* @date     Aug, 02 2018
*
(c) Copyright <2018-2019> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_UIHANDLER_MODELMANAGERTIPLIBCONFIGADMIN_H_
#define ANALOGIC_WS_UIHANDLER_MODELMANAGERTIPLIBCONFIGADMIN_H_

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
 * \class   ModelManagerTipLibConfigAdmin
 * \brief   This class contains variable and function related to
 *          updating the data for current screen view of tip lib config admin.
 */

class ModelManagerTipLibConfigAdmin: public ModelManager
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelManagerTipLibConfigAdmin
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ModelManagerTipLibConfigAdmin.
    */
    explicit ModelManagerTipLibConfigAdmin(QObject *parent = NULL);

    /*!
    * @fn       ModelManagerTipLibConfigAdmin
    * @param    None
    * @return   None
    * @brief    Destructor for class ModelManagerTipLibConfigAdmin.
    */
    virtual ~ModelManagerTipLibConfigAdmin();

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
    * @fn       ModelManagerTipLibConfigAdmin();
    * @param    ModelManagerTipLibConfigAdmin&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelManagerTipLibConfigAdmin(const ModelManagerTipLibConfigAdmin& modelManagerTipLibConfigAdmin);

    /*!
    * @fn       operator=
    * @param    ModelManagerTipLibConfigAdmin&
    * @return   ModelManagerTipLibConfigAdmin&
    * @brief    declaration for private assignment operator.
    */
    ModelManagerTipLibConfigAdmin& operator= (const ModelManagerTipLibConfigAdmin& modelManagerTipLibConfigAdmin);

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
#endif // MODELMANAGERTIPLIBCONFIGADMIN_H
