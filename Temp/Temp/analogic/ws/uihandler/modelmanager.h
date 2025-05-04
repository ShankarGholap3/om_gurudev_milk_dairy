/*!
* @file     modelmanager.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_MODELMANAGER_H_
#define ANALOGIC_WS_UIHANDLER_MODELMANAGER_H_

#include <QVariant>
#if defined WORKSTATION || defined RERUN
#include <rial.h>
#include <rialcommon.h>
#endif
#include <analogic/ws/common/utility/xmlservice.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class ModelUpdater;

#define MAX_PARALLEL_TASK 2
#define MAX_REPORT_PARALLEL_TASK 1

/*!
 * \class   ModelManager
 * \brief   This class contains variable and function related to
 *          updating the data for current screen view.
 */
class ModelManager: public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelManager
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ModelManager.
    */
    explicit ModelManager(QObject *parent = NULL);

    /*!
    * @fn       ModelManager
    * @param    None
    * @return   None
    * @brief    Destructor for class ModelManager.
    */
    virtual ~ModelManager();

    /*!
     * @fn       init
     * @param    None
     * @return   None
     * @brief    on thread start this function will initialize models.
     */
    virtual void init();

    /*!
     * @fn       onCommandButtonEvent
     * @param    QMLEnums::ModelEnum  - modelname
     * @param    QString data to be post
     * @return   None
     * @brief    This slot will call on command button clicked.
     */
    virtual void onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data);

    /*!
     * @fn      onExit
     * @param   None
     * @return  None
     * @brief Function will call on exit of threads
     */
    virtual void onExit();

signals:
    /*!
     * @fn       updateModels(QVariantMap data , QString sName) ;
     * @param    QVariantList - model data
     * @param    QMLEnums::ModelEnum - model name
     * @return   void
     * @brief    update model.
     */
    void updateModels(QVariantList data , QMLEnums::ModelEnum sName);

    /*!
     * @fn       dataTimerTick() ;
     * @param    None
     * @return   None
     * @brief    emitted when one round of data retrival loop finishes.
     */
    void dataTimerTick();

    /*!
     * @fn      authstatuschanged
     * @param   int - errorcode
     * @param   QString - status
     * @param   int - model
     * @return  void
     * @brief   signal for authentication status changed
     */
    void authstatuschanged(int hr, QString status, int model);

    /*!
     * @fn      sslerrorreceived
     * @param   int - errorcode
     * @param   QString - status
     * @param   int - model
     * @return  void
     * @brief   signal for sslerrorreceived
     */
    void sslerrorreceived(int hr, QString status, int model);

    /*!
     * @fn      fileDatadownloadFinished
     * @param   QString sDownloadpath - information on finished download
     * @param   QString sError - Error occured during download
     * @param   QMLEnums::ModelEnum modelnum - modelnum for which file is downloaded
     * @return  void
     * @brief   signal emitted when file download finishes
     */
    void fileDatadownloadFinished(QString sDownloadpath, QString sError, QMLEnums::ModelEnum modelnum);

public slots:
    /*!
     * @fn      onauthstatuschanged
     * @param   int - error
     * @param   QString - status
     * @param   int - modelno
     * @return  None
     * @brief   Function will call on authentication status changed
     */
    virtual void onauthstatuschanged(int hr, QString status, int modelno);

private:
    /*!
    * @fn       ModelManager();
    * @param    ModelManager&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelManager(const ModelManager& modelManager);

    /*!
    * @fn       operator=
    * @param    ModelManager&
    * @return   ModelManager&
    * @brief    declaration for private assignment operator.
    */
    ModelManager& operator= (const ModelManager& modelManager);

protected:
    /*!
    * @fn       createDirAndMoveFile
    * @param    QString sDownloadpath
    * @param    QMLEnums::modelnum
    * @return   QString
    * @brief    common function for create Dir and move file
    */
    QString createDirAndMoveFile(QString sDownloadpath, QMLEnums::ModelEnum modelnum);

    /*!
    * @fn       updateXMLFileData
    * @param    QString &data - return
    * @return   bool
    * @brief    common function for update xml
    */
    bool updateXMLFileData(QString &data);

    ModelUpdater**                          m_modelsUpdater;        //!< handle for array of models
    int                                     m_modelEnumOffset;      //!< Enums starting point.
    bool                                    m_XmlReceived;          //!< xml is received
    bool                                    m_XsdReceived;          //!< xsd is received
    QString                                 m_configXMLFile;        //!< received xml file
    QString                                 m_configXSDFile;        //!< received xsd file
    QString                                 m_configXMLData;        //!< received xml data
    QString                                 m_configXSDData;        //!< received xsd data
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_MODELMANAGER_H_


