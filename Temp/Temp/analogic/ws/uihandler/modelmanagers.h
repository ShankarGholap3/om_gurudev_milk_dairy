/*!
* @file     modelmanagers.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles updating the data for current screen view.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_MODELMANAGERS_H_
#define ANALOGIC_WS_UIHANDLER_MODELMANAGERS_H_

#include <QObject>
#include <analogic/ws/common.h>
#include <semaphore.h>
//For updating QAbstractSeries i.e linechart
#include <QtCharts/QAbstractSeries>
QT_CHARTS_USE_NAMESPACE
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class ModelManagerScannerAdmin;
class ModelManagerUserAdmin;
class ModelManagerReportAdmin;
class ModelManagerTipConfigAdmin;
class ModelManagerTipLibConfigAdmin;
class ModelManagerAntiVirus;
class ModelManagerSupervisor;

/*!
 * \class   ModelManagers
 * \brief   This class contains variable and function related to
 *          updating the data for current screen view.
 */
class ModelManagers: public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelManagers
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ModelManagers.
    */
    explicit ModelManagers(QObject *parent = NULL);

    /*!
    * @fn       ~ModelManagers
    * @param    None
    * @return   None
    * @brief    Destructor for class ModelManagers.
    */
    virtual ~ModelManagers();

public slots:
    /*!
     * @fn       init
     * @param    None
     * @return   None
     * @brief    on thread start this function will initialize models.
     */
    void init();

    /*!
     * @fn       onCommandButtonEvent
     * @param    QMLEnums::ModelEnum  - modelname
     * @param    QString data to be post
     * @return   None
     * @brief    This slot will call on command button clicked.
     */
    void onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data);

    /*!
    * @fn      updateChartsCordinates
    * @param   QAbstractSeries
    * @return  None
    * @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
    */
    void onUpdateChartsCordinates(QAbstractSeries *series,int modelnum);

     /*!
     * @fn      onExit
     * @param   None
     * @return  None
     * @brief Function will call on exit of threads
     */
    void onExit();

    /*!
    * @fn      onGetSelectedGroup
    * @param   QString
    * @return  None
    * @brief   This function is responsible for getting selected group
    */
    void onGetSelectedGroup(QString selectedGroup);


    /*!
    * @fn      onGetSelectedGauge
    * @param   QString
    * @return  None
    * @brief   This function is responsible for getting selected gauge
    */
    void onGetSelectedGauge(int selectedGauge);

    /*!
     * @fn      getSupervisorModelManager
     * @param   None
     * @return  m_supervisorModelManager
     * @brief   Getter for charts model manager.
     */
    ModelManagerSupervisor* getSupervisorModelManager() { return m_supervisorModelManager; }


    /*!
     * @fn      getScannerAdminModelManager
     * @param   None
     * @return  m_ScannerAdminModelManager
     * @brief   Getter for scanner admin model manager.
     */
    ModelManagerScannerAdmin* getScannerAdminModelManager() { return m_ScannerAdminModelManager; }

    /*!
     * @fn      getUserAdminModelManager
     * @param   None
     * @return  m_userAdminModelManager
     * @brief   Getter for user admin model manager.
     */
    ModelManagerUserAdmin* getUserAdminModelManager() { return m_userAdminModelManager; }

    /*!
     * @fn      getReportAdminModelManager
     * @param   None
     * @return  m_reportAdminModelManager
     * @brief   Getter for report admin model manager.
     */
    ModelManagerReportAdmin* getReportAdminModelManager() { return m_reportAdminModelManager; }

    /*!
     * @fn      getTipConfigAdminModelManager
     * @param   None
     * @return  m_tipConfigAdminModelManager
     * @brief   Getter for tip config admin model manager.
     */
    ModelManagerTipConfigAdmin* getTipConfigAdminModelManager() { return m_tipConfigAdminModelManager; }

    /*!
     * @fn      getTipLibConfigAdminModelManager
     * @param   None
     * @return  m_tipLibConfigAdminModelManager
     * @brief   Getter for tip lib config admin model manager.
     */
    ModelManagerTipLibConfigAdmin* getTipLibConfigAdminModelManager() { return m_tipLibConfigAdminModelManager; }

    /*!
     * @fn      getAntiVirusModelManager
     * @param   None
     * @return  m_reportAdminModelManager
     * @brief   Getter for report admin model manager.
     */
    ModelManagerAntiVirus* getAntiVirusModelManager() { return m_antiVirusModelManager; }

private:
    /*!
    * @fn       ModelManagers();
    * @param    ModelManagers&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelManagers(const ModelManagers& modelManagers);

    /*!
    * @fn       operator=
    * @param    ModelManagers&
    * @return   ModelManagers&
    * @brief    declaration for private assignment operator.
    */
    ModelManagers& operator = (const ModelManagers& modelManager);

    ModelManagerTipLibConfigAdmin*      m_tipLibConfigAdminModelManager;    //!< Model Manager for tip lib config admin
    ModelManagerTipConfigAdmin*         m_tipConfigAdminModelManager;       //!< Model Manager for tip config admin
    ModelManagerScannerAdmin*           m_ScannerAdminModelManager;         //!< Model manager for scanner admin
    ModelManagerReportAdmin*            m_reportAdminModelManager;          //!< Model Manager for report admin
    ModelManagerUserAdmin*              m_userAdminModelManager;            //!< Model manager for user admin
    ModelManagerAntiVirus*              m_antiVirusModelManager;            //!< Model manager for user admin
    ModelManagerSupervisor*             m_supervisorModelManager;               //!< Model manager for charts
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_MODELMANAGERS_H_


