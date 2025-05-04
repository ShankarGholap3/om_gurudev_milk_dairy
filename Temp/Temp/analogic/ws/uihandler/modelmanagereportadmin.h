/*!
* @file     modelmanagereportadmin.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view for Report Admin.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef MODELMANAGEREPORTADMIN_H
#define MODELMANAGEREPORTADMIN_H

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
 * \class   ModelManagerReportAdmin
 * \brief   This class contains variable and function related to
 *          updating the data for current screen view of report admin.
 */
class ModelManagerReportAdmin: public ModelManager
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelManagerReportAdmin
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ModelManagerReportAdmin.
    */
    explicit ModelManagerReportAdmin(QObject *parent = NULL);

    /*!
    * @fn       ModelManagerReportAdmin
    * @param    None
    * @return   None
    * @brief    Destructor for class ModelManagerReportAdmin.
    */
    virtual ~ModelManagerReportAdmin();

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
    * @fn       ModelManagerReportAdmin();
    * @param    ModelManagerReportAdmin&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelManagerReportAdmin(const ModelManagerReportAdmin& modelManagerReportAdmin);

    /*!
    * @fn       operator=
    * @param    ModelManagerReportAdmin&
    * @return   ModelManagerReportAdmin&
    * @brief    declaration for private assignment operator.
    */
    ModelManagerReportAdmin& operator= (const ModelManagerReportAdmin& modelManagerReportAdmin);

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

    TaskManager**                           m_rTaskStatusManager;    //!< handle for taskmanager
    QMap<QMLEnums::ModelEnum, QString>      m_rrestmap;              //!< map for rest URL
    QMap<QMLEnums::ModelEnum, QByteArray>   m_rrestContentMap;       //!< map for rest content type

};
}  // end of namespace ws
}  // end of namespace analogic


#endif // MODELMANAGEREPORTADMIN_H
