/*!
* @file     appcontroller.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to AppController
*           which handles initialization of all managers and controllers.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_APPCONTROLLER_H_
#define ANALOGIC_WS_APPCONTROLLER_H_

#include <semaphore.h>
#include <analogic/ws/common.h>
#include <rial.h>
#ifdef WORKSTATION
#include <analogic/ws/osr/osrmanager.h>
#endif
#include <analogic/ws/workstationmanager.h>
#include <analogic/ws/ulm/uilayoutmanager.h>
#include <analogic/ws/uihandler/modelmanagers.h>
#include <analogic/ws/appbuilder/workstationfactory.h>
#include <analogic/ws/appbuilder/workstationbuilder.h>
#include <analogic/ws/common/bagdata/dataprocessingservice.h>
#include <analogic/ws/uihandler/supervisoragentsdatamodel.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class ViewManager;
#define MAX_THREAD 3
/*!
 * \class   AppController
 * \brief   This class contains variable and function related to
 *          initialization of all managers and controllers .
 */
class AppController:public QObject
{
  Q_OBJECT
public:
  /*!
    * @fn       AppController
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class AppController.
    */
  explicit AppController( QObject *parent = 0);

  /*!
    * @fn       ~AppController
    * @param    None
    * @return   None
    * @brief    Destructor for class AppController.
    */
  ~AppController();

  /*!
    * @fn       initialize()
    * @param    None
    * @return   None
    * @brief    Function responsible for initialization of members.
    */
  int initialize();

  /*!
    * @fn       getViewManagerInstance()
    * @param    None
    * @return   boost::shared_ptr<ViewManager> - view manager
    * @brief    Function returns view manager.
    */
  boost::shared_ptr<ViewManager> getViewManagerInstance() const;

  /*!
    * @fn       destroy
    * @param    None
    * @return   None
    * @brief    Function destroys members of class AppController.
    */
  void destroy();

signals:
  /*!
    * @fn       exit()
    * @param    None
    * @return   Noe
    * @brief    Signal call on exit
    */
  void exit();
  /*!
    * @fn       init()
    * @param    None
    * @return   Noe
    * @brief    Signal call on init
    */
  void  init();


public slots:
  /*!
    * @fn       onExit()
    * @param    None
    * @return   Noe
    * @brief    Slot call on exit
    */
  void onExit();

private:
  /*!
    * @fn       AppController();
    * @param    AppController&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
  AppController(const AppController& appController);

  /*!
    * @fn       operator=
    * @param    AppController&
    * @return   AppController&
    * @brief    declaration for private assignment operator.
    */
  AppController& operator=(const AppController& appController);

  UILayoutManager*                              m_uiLayoutManager;      //!< handle to ULM
  SupervisorAgentsDataModel*                    m_sadmManager;          //!< handle to SupervisorAgentsDataModel
  boost::shared_ptr<QThread>                    m_pThread;              //!< modelupdater and nss handler thread
  boost::shared_ptr<ViewManager>                m_viewManager;          //!< handle to view manager
  boost::shared_ptr<ModelManagers>              m_modelManagers;        //!< handle to model managers
  boost::shared_ptr<WorkstationManager>         m_workstationManager;   //!< handle to workstation manager
  Rial*                                         m_hrial;                //!< handle for rail
#ifdef WORKSTATION
  boost::shared_ptr<ScannerFaultHandler>        m_scannerfaulthandler;  //!< handle for scanner fault handler
  boost::shared_ptr<QThread>                    m_pThreadForBagData;    //!< handle for bag data acquisition thread
 #endif
  boost::shared_ptr<AbstractWorkStationFatcory> m_workstationFactory;   //!< handle to workstation factory instance
  boost::shared_ptr<QThread>                    m_compressionThread;    //!< handle to compression thread
  boost::shared_ptr<DataProcessingService>      m_dataCompressionSrv;      //!< handle to compression class
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_APPCONTROLLER_H_

