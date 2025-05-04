/*!
* @file     appcontroller.cpp
* @author   Agiliad
* @brief    This file contains functions related to AppController
*           which handles initialization of all managers and controllers.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QApplication>
#include <analogic/ws/appcontroller.h>
#include <analogic/ws/uihandler/view.h>
#include <analogic/ws/uihandler/viewmanager.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/uihandler/modelmanagerscanneradmin.h>
#include <analogic/ws/uihandler/modelmanagereportadmin.h>
#include <analogic/ws/uihandler/modelmanageruseradmin.h>
#include <analogic/ws/uihandler/modelmanagertipconfigadmin.h>
#include <analogic/ws/uihandler/modelmanagertiplibconfigadmin.h>
#include <analogic/ws/uihandler/modelmanagerantivirus.h>
#include <analogic/ws/uihandler/modelmanagersupervisor.h>
#include <analogic/ws/uihandler/supervisoragents.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       AppController(RESULT &hr, QObject* parent = NULL);
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class AppController.
*/
AppController::AppController(QObject* parent):QObject(parent)
{
  TRACE_LOG("");
}

/*!
* @fn       ~AppController
* @param    None
* @return   None
* @brief    Destructor for class AppController.
*/
AppController::~AppController()
{
  TRACE_LOG("");
  //    destroy();
}

/*!
* @fn       destroy
* @param    None
* @return   None
* @brief    Function destroys members of class AppController.
*/
void AppController::destroy()
{
  TRACE_LOG("");
  INFO_LOG("Destroying AppController members");
  INFO_LOG("Waiting for child threads to cleanup.");

  Semaphore::getInstance()->resetSemCount(MAX_THREAD);
  Semaphore::getInstance()->acquire(MAX_THREAD);
  DEBUG_LOG("Notifying application exit.");

  // Exit will clean exit model manager
  emit exit();

  while(Semaphore::getInstance()->available() < MAX_THREAD);
  if (NULL != m_pThread)
  {
    DEBUG_LOG("Exiting m_pThread");
    m_pThread->exit();
    m_pThread->wait();
    DEBUG_LOG("Deleting m_pThread which is managing modelupdater and nss handler thread");
    m_pThread.reset();
  }

  if (NULL != m_compressionThread)
  {
    DEBUG_LOG("Exiting m_compressionThread");
    m_compressionThread->exit();
    m_compressionThread->wait();
    DEBUG_LOG("Deleting m_pThread which is managing modelupdater and nss handler thread");
    m_compressionThread.reset();
  }
#ifdef WORKSTATION
  if (NULL != m_pThreadForBagData)
  {
    DEBUG_LOG("Exiting m_pThreadForBagData");
    m_pThreadForBagData->exit();
    m_pThreadForBagData->wait();
    DEBUG_LOG("Deleting m_pThreadForBagData which is managing BagData handler thread");
    m_pThreadForBagData.reset();
  }
#endif
  DEBUG_LOG("Deleting m_viewManager viewmanager handle");
  m_viewManager.reset();

  DEBUG_LOG("Deleting UILayoutManager handle");
  UILayoutManager::cleanupUILayoutManagerInstance();
  m_uiLayoutManager = NULL;

  DEBUG_LOG("Deleting m_workstationFactory,m_modelManagers and m_workstationManager handle");
  m_workstationFactory.reset();
  m_modelManagers.reset();
  m_workstationManager.reset();

#ifdef WORKSTATION
  DEBUG_LOG("Deleting m_scannerfaulthandler handle");
  m_scannerfaulthandler.reset();
#endif
  Semaphore::destroyInstance();
  DEBUG_LOG("Exiting current thread");
  QThread::currentThread()->exit();
  DEBUG_LOG("Quiting Qapplication");
  //  QApplication::quit();
  QMLEnums::WSType wsType = WorkstationConfig::getInstance()->
      getWorkstationNameEnum();
  if (wsType == QMLEnums::WSType::RERUN_EMULATOR) {
    system("shutdown -P now");
  }
}

/*!
* @fn       initialize
* @param    None
* @return   None
* @brief    Responsible for intialization of managers and controllers.
*/
int AppController::initialize()
{
  TRACE_LOG("");

  Semaphore::getInstance()->resetSemCount(MAX_THREAD);
  Semaphore::getInstance()->acquire(MAX_THREAD);

  // creating a new thread object
  m_pThread.reset(new QThread());
  THROW_IF_FAILED((m_pThread.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_pThread->start();

  DEBUG_LOG("Create workstation instance from factory");
  // Create the workstation instance from factory based on the
  // workstationManagerkstation type.
  m_workstationFactory.reset(WorkStationFactory::
                             getWorkstationFactoryInstance());


  QMLEnums::WSType wsType = WorkstationConfig::getInstance()->
      getWorkstationNameEnum();
  m_workstationManager.reset(m_workstationFactory->
                             createWorkstationInstance(wsType));
  THROW_IF_FAILED((m_workstationManager.use_count() == 0)?
                    Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_workstationManager->moveToThread(m_pThread.get());


#ifdef WORKSTATION
  //  Please dont remove below code as it will require to initialize at proper place.
  NSSAgentAccessInterface* instance = NSSAgentAccessInterface::getInstance();
  Q_UNUSED(instance);
#endif


#ifdef WORKSTATION
  if ((wsType == QMLEnums::WSType::OSR_WORKSTATION) ||
      (wsType == QMLEnums::WSType::SEARCH_WORKSTATION) ||
      (wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION))
  {
    m_pThreadForBagData.reset(new QThread());
    THROW_IF_FAILED((m_pThreadForBagData.use_count() == 0)
                    ?Errors::E_OUTOFMEMORY:Errors::S_OK);
    m_workstationManager->setBagDataHandlerThread(m_pThreadForBagData);
  }
#endif
  connect(this, SIGNAL(init()), m_workstationManager.get(),
          SLOT(init()), Qt::QueuedConnection);

  // intialize ULM
  DEBUG_LOG("Initialize ULM");
  m_uiLayoutManager = UILayoutManager::getUILayoutManagerInstance();
  THROW_IF_FAILED((m_uiLayoutManager == NULL) ?
                    Errors::E_OUTOFMEMORY : Errors::S_OK);
  m_uiLayoutManager->initialize();
#ifdef WORKSTATION
  m_hrial =  Rial::getinstance();
  THROW_IF_FAILED((m_hrial == NULL)?Errors::E_POINTER:Errors::S_OK);
  m_hrial->setserversslauth(WorkstationConfig::getInstance()->getsslauth());
  m_hrial->setpassphrase(WorkstationConfig::getInstance()->getpassphrase());
  m_hrial->loadpfxcertificate(DEFAULT_CERTIFICATE);
  if(WorkstationConfig::getInstance()->getsslauth())
  {
    m_hrial->setserverpassphrase(WorkstationConfig::getInstance()->getserverpassphrase());
    m_hrial->loadsrvrpfxcertificate(WorkstationConfig::getInstance()->getnssservercertificate());
  }

#endif
  // intialize view manager
  DEBUG_LOG("Create and initialize ViewManager");
  m_viewManager.reset(new ViewManager());
  THROW_IF_FAILED((m_viewManager.use_count() == 0)?
                    Errors::E_OUTOFMEMORY:Errors::S_OK);

  // Register ulmobject in qml
  m_viewManager->getViewObject()->registerULMcontext(m_uiLayoutManager);
  m_viewManager->initialize();


  connect(m_uiLayoutManager, SIGNAL(updateModelsig(QList<QObject*>)),
          m_viewManager.get(), SLOT(onUpdateModelsig(QList<QObject*>)),
          Qt::QueuedConnection);
#ifdef WORKSTATION
  if ((wsType == QMLEnums::WSType::OSR_WORKSTATION) ||
      (wsType == QMLEnums::WSType::SEARCH_WORKSTATION) ||
      (wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION)) {
    // intialize model Manager
    DEBUG_LOG("Create and initialize model manager");
    m_modelManagers.reset(new ModelManagers());
    THROW_IF_FAILED((m_modelManagers.use_count() == 0)?
                      Errors::E_OUTOFMEMORY:Errors::S_OK);

    // move current object to p_thread
    m_modelManagers->moveToThread(m_pThread.get());
    connect(this, &AppController::init, m_modelManagers.get(),
            &ModelManagers::init, Qt::QueuedConnection);

    DEBUG_LOG("Initialize workstation instance and modelManagers");
  }
#endif

  m_compressionThread.reset(new QThread());
  THROW_IF_FAILED((m_compressionThread.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_compressionThread->start();

  m_dataCompressionSrv.reset(new DataProcessingService());
  THROW_IF_FAILED((m_dataCompressionSrv.use_count() == 0)?
                    Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_dataCompressionSrv->moveToThread(m_compressionThread.get());

  connect(this, &AppController::init, m_dataCompressionSrv.get(),
          &DataProcessingService::init, Qt::QueuedConnection);

  emit init();

  INFO_LOG("Waiting for child threads to initialize.");
  //  Wait untill all object gets initialize
  while(Semaphore::getInstance()->available() < MAX_THREAD);

  for(int i = 0; i < MAX_BAG_ALLOWED; i++)
  {
    BagData *bagData = BagsDataPool::getInstance()->getBagbyIndex(i).get();
    connect(bagData, &BagData::compressedSliceReceived,
            m_dataCompressionSrv.get(), &DataProcessingService::consumeQueue, Qt::QueuedConnection);
  }
  // Register object (which are allocated in init of manager) in qml
  WorkstationBuilder::workstationTypeBasedQmlRegister(wsType, m_viewManager, m_workstationManager);

  DEBUG_LOG("Connecting signals and slots");
  // add connects for workstationManager signals
  connect(m_viewManager.get(), SIGNAL(commandButtonEvent(
                                        QMLEnums::ModelEnum, QString)), m_workstationManager.get(),
          SLOT(onCommandButtonEvent(QMLEnums::ModelEnum, QString)),
          Qt::QueuedConnection);

  connect(m_workstationManager.get(), &WorkstationManager::authstatuschanged,
          m_viewManager.get(), &ViewManager::onauthstatuschanged,
          Qt::QueuedConnection);

  connect(m_viewManager.get(), SIGNAL(deAuthenticate()),
          m_workstationManager.get(), SIGNAL(deAuthenticate()),
          Qt::QueuedConnection);

  connect(m_viewManager.get(), &ViewManager::exit, this,
          &AppController::onExit, Qt::QueuedConnection);

  connect(this, &AppController::exit, m_workstationManager.get(),
          &WorkstationManager::onExit, Qt::QueuedConnection);

  // Connection when appstate is about to change
  connect(m_uiLayoutManager, &UILayoutManager::updateState,
          m_viewManager.get()->getViewObject().get(), &View::onUpdateState,
          Qt::QueuedConnection);

  connect(m_uiLayoutManager, &UILayoutManager::updateState,
          m_workstationManager.get(), &WorkstationManager::onUpdateState,
          Qt::QueuedConnection);

  connect(m_workstationManager.get(), &WorkstationManager::stateChanged,
          m_uiLayoutManager, &UILayoutManager::onStateChanged);

  connect(this, &AppController::exit, m_dataCompressionSrv.get(),
          &DataProcessingService::onExit, Qt::QueuedConnection);

  connect(m_workstationManager.get(),
          SIGNAL(updateModels(QVariantList, QMLEnums::ModelEnum)),
          m_viewManager.get(),
          SLOT(onModelDataChanged(QVariantList, QMLEnums::ModelEnum)),
          Qt::QueuedConnection);

  connect(m_viewManager.get(),
            SIGNAL(processUpdateModels()),
            m_modelManagers->getSupervisorModelManager(),
            SLOT(onProcessUpdateModels()),
            Qt::QueuedConnection);

  connect(m_modelManagers->getSupervisorModelManager(), SIGNAL(chartsAverageData(QVariantList)),
         m_viewManager->getViewObject().get(),SIGNAL(chartsAverageData(QVariantList)), Qt::QueuedConnection);


#ifdef WORKSTATION
  if ((wsType == QMLEnums::WSType::OSR_WORKSTATION) ||
      (wsType == QMLEnums::WSType::SEARCH_WORKSTATION) ||
      (wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION))
  {
    m_scannerfaulthandler = m_workstationManager->getScannerFaultHandle();

    connect(m_scannerfaulthandler.get(), &ScannerFaultHandler::
            scannerFaultReceived, m_viewManager->getViewObject().get(),
            &View::scannerFaultReceived, Qt::QueuedConnection);

    connect(m_scannerfaulthandler.get(), &ScannerFaultHandler::
            operatorAbsentFromMat, m_viewManager->getViewObject().get(),
            &View::operatorAbsentFromMat, Qt::QueuedConnection);

    connect(m_workstationManager.get(), &WorkstationManager::requestBagPickupsig,
            m_viewManager.get(), &ViewManager::onRequestBagPickup,
            Qt::QueuedConnection);

    connect(m_viewManager.get(), &ViewManager::notifyBagIsPickedUpsig,
            m_workstationManager.get(), &WorkstationManager::notifyBagIsPickedUpsig,
            Qt::QueuedConnection);

    connect(m_viewManager->getViewObject().get(), &View::scannerStateChanged,
            m_scannerfaulthandler.get(), &ScannerFaultHandler::
            onScannerStateChanged, Qt::QueuedConnection);

    connect(m_viewManager.get(), SIGNAL(commandButtonEvent(
                                          QMLEnums::ModelEnum, QString)), m_modelManagers.get(), SLOT(
              onCommandButtonEvent(QMLEnums::ModelEnum, QString)),
            Qt::QueuedConnection);

    //For updating the received supervisor webservice data to the lineseries(QAbstractSeries), this is done as in qml it was consuming
    //a lot of time to diplay the data into chart
    //Flow: DashBoardSupervisor.qml->view.cpp->viewmanager.cpp->modelmanagers.cpp->modelmanagersupervisor.cpp->modelupdater.cpp->rial
    connect(m_viewManager.get(), SIGNAL(updateChartsCordinates(QAbstractSeries*,int)), m_modelManagers.get(), SLOT(
              onUpdateChartsCordinates(QAbstractSeries*,int)),
            Qt::QueuedConnection);

   connect(this, &AppController::exit, m_modelManagers.get(),
            &ModelManagers::onExit, Qt::QueuedConnection);

    connect(m_uiLayoutManager, &UILayoutManager::updateState,
            m_workstationManager->getBagDataAcqHnd().get(),
            &BagDataAcqHandler::onUpdateState);

    //  Connection when appstate is changed
    connect(m_workstationManager->getBagDataAcqHnd().get(), &BagDataAcqHandler
            ::stateChanged, m_uiLayoutManager, &UILayoutManager::
            onStateChanged);

    connect(m_workstationManager.get(),
            SIGNAL(updateModels(QVariantList, QMLEnums::ModelEnum)),
            m_viewManager.get(),
            SLOT(onModelDataChanged(QVariantList, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

     //  Connection for ChartsModelManager
     connect(m_viewManager.get(), SIGNAL(screenChanged(ModelList)),
            m_modelManagers->getSupervisorModelManager (),
            SLOT(onScreenModelChanged(ModelList)), Qt::QueuedConnection);

    connect(m_modelManagers->getSupervisorModelManager(), SIGNAL(
              dataTimerTick()), m_viewManager.get(), SLOT(onDataTimerTick()),
            Qt::QueuedConnection);

    connect(m_modelManagers->getSupervisorModelManager(),
            SIGNAL(authstatuschanged(int, QString, int)), m_viewManager.get(),
            SLOT(onauthstatuschanged(int, QString, int)),
            Qt::QueuedConnection);

    connect(m_modelManagers->getSupervisorModelManager(),
            SIGNAL(sslerrorreceived(int, QString, int)), m_viewManager.get(),
            SLOT(onsslerrorreceived(int, QString, int)),
            Qt::QueuedConnection);

        //  Connection for ScannerAdminModelManager
    connect(m_modelManagers->getScannerAdminModelManager(),
            SIGNAL(updateModels(QVariantList, QMLEnums::ModelEnum)),
            m_viewManager.get(),
            SLOT(onModelDataChanged(QVariantList, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

    connect(m_viewManager.get(), SIGNAL(screenChanged(ModelList)),
            m_modelManagers->getScannerAdminModelManager (),
            SLOT(onScreenModelChanged(ModelList)), Qt::QueuedConnection);

    connect(m_modelManagers->getScannerAdminModelManager(), SIGNAL(
              dataTimerTick()), m_viewManager.get(), SLOT(onDataTimerTick()),
            Qt::QueuedConnection);
    connect(m_modelManagers->getScannerAdminModelManager(),
            SIGNAL(authstatuschanged(int, QString, int)), m_viewManager.get(),
            SLOT(onauthstatuschanged(int, QString, int)),
            Qt::QueuedConnection);
    connect(m_modelManagers->getScannerAdminModelManager(),
            SIGNAL(sslerrorreceived(int, QString, int)), m_viewManager.get(),
            SLOT(onsslerrorreceived(int, QString, int)),
            Qt::QueuedConnection);
    connect(m_modelManagers->getScannerAdminModelManager(),
            SIGNAL(sdswClientExited(int)), m_viewManager.get(),
            SLOT(onSdswClientExited(int)),
            Qt::QueuedConnection);
    //        connect(m_modelManagers->getScannerAdminModelManager(), SIGNAL(
    //                    updateState(QMLEnums::AppState)), m_uiLayoutManager, SLOT(
    //                    onStateChanged(QMLEnums::AppState)), Qt::QueuedConnection);

    connect(m_modelManagers->getScannerAdminModelManager(),
            SIGNAL(fileDatadownloadFinished(QString, QString,
                                            QMLEnums::ModelEnum)), m_viewManager.get(), SLOT(
              onfileDatadownloadFinished(QString, QString, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

    //  Connection for ReportAdminModelManager

    connect(m_modelManagers->getReportAdminModelManager(), SIGNAL(
              authstatuschanged(int, QString, int)), m_viewManager.get(), SLOT(
              onauthstatuschanged(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getReportAdminModelManager(), SIGNAL(
              sslerrorreceived(int, QString, int)), m_viewManager.get(), SLOT(
              onsslerrorreceived(int, QString, int)),Qt::QueuedConnection);

    connect(m_modelManagers->getReportAdminModelManager(),
            SIGNAL(fileDatadownloadFinished(QString, QString,
                                            QMLEnums::ModelEnum)), m_viewManager.get(), SLOT(
              onfileDatadownloadFinished(QString, QString, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

    //  Connection for UserAdminModelManager

    connect(m_modelManagers->getUserAdminModelManager(), SIGNAL(
              authstatuschanged(int, QString, int)), m_viewManager.get(), SLOT(
              onauthstatuschanged(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getUserAdminModelManager(), SIGNAL(
              sslerrorreceived(int, QString, int)), m_viewManager.get(), SLOT(
              onsslerrorreceived(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getUserAdminModelManager(),
            SIGNAL(fileDatadownloadFinished(QString, QString,
                                            QMLEnums::ModelEnum)), m_viewManager.get(), SLOT(
              onfileDatadownloadFinished(QString, QString, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

    //  Connection for TipAdminModelManager

    connect(m_modelManagers->getTipConfigAdminModelManager(),
            SIGNAL(updateModels(QVariantList, QMLEnums::ModelEnum)),
            m_viewManager.get(),
            SLOT(onModelDataChanged(QVariantList, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

    connect(m_modelManagers->getTipConfigAdminModelManager(), SIGNAL(
              authstatuschanged(int, QString, int)), m_viewManager.get(), SLOT(
              onauthstatuschanged(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getTipConfigAdminModelManager(), SIGNAL(
              sslerrorreceived(int, QString, int)), m_viewManager.get(), SLOT(
              onsslerrorreceived(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getTipConfigAdminModelManager(),
            SIGNAL(fileDatadownloadFinished(QString, QString,
                                            QMLEnums::ModelEnum)), m_viewManager.get(), SLOT(
              onfileDatadownloadFinished(QString, QString, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

    //  Connection for TipLibAdminModelManager
    connect(m_modelManagers->getTipLibConfigAdminModelManager(),
            SIGNAL(updateModels(QVariantList, QMLEnums::ModelEnum)),
            m_viewManager.get(),
            SLOT(onModelDataChanged(QVariantList, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);
    connect(m_modelManagers->getTipLibConfigAdminModelManager(), SIGNAL(
              authstatuschanged(int, QString, int)), m_viewManager.get(), SLOT(
              onauthstatuschanged(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getTipLibConfigAdminModelManager(), SIGNAL(
              sslerrorreceived(int, QString, int)), m_viewManager.get(), SLOT(
              onsslerrorreceived(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getTipLibConfigAdminModelManager(),
            SIGNAL(fileDatadownloadFinished(QString, QString,
                                            QMLEnums::ModelEnum)), m_viewManager.get(), SLOT(
              onfileDatadownloadFinished(QString, QString, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

    //Connection for AntiVirus
    connect(m_modelManagers->getAntiVirusModelManager(), SIGNAL(
              authstatuschanged(int, QString, int)), m_viewManager.get(), SLOT(
              onauthstatuschanged(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getAntiVirusModelManager(), SIGNAL(
              sslerrorreceived(int, QString, int)), m_viewManager.get(), SLOT(
              onsslerrorreceived(int, QString, int)), Qt::QueuedConnection);

    connect(m_modelManagers->getAntiVirusModelManager(),
            SIGNAL(fileDatadownloadFinished(QString, QString,
                                            QMLEnums::ModelEnum)), m_viewManager.get(), SLOT(
              onfileDatadownloadFinished(QString, QString, QMLEnums::ModelEnum)),
            Qt::QueuedConnection);

    connect(m_viewManager.get(), SIGNAL(getSelectedGroup(QString)), m_modelManagers.get(), SLOT(
              onGetSelectedGroup(QString)), Qt::QueuedConnection);

     connect(m_viewManager.get(), SIGNAL(getSelectedGauge(int)), m_modelManagers.get(), SLOT(
              onGetSelectedGauge(int)), Qt::QueuedConnection);

     connect(m_viewManager->getViewObject().get(), SIGNAL(currentViewChanged(int)), m_modelManagers->getSupervisorModelManager(), SLOT(
              onCurrentScreenChanged(int)), Qt::QueuedConnection);

  }

#endif
  // Workstation based connection
  WorkstationBuilder::workstationTypeBasedConnection(wsType, m_viewManager,
                                                     m_workstationManager, m_modelManagers, m_uiLayoutManager);
  return Errors::S_OK;
}

/*!
* @fn      getViewManagerInstance()
* @param   None
* @return  boost::shared_ptr<ViewManager> - view manager
* @brief   Function returns view manager.
*/
boost::shared_ptr<ViewManager> AppController::getViewManagerInstance() const
{
  DEBUG_LOG("Returning handle to view manager.");
  return m_viewManager;
}

/*!
* @fn       onExit()
* @param    None
* @return   Noe
* @brief    Slot call on exit
*/
void AppController::onExit()
{
  TRACE_LOG("");
  system("systemctl poweroff -i");
  destroy();
}
}  // end of namespace ws
}  // end of namespace analogic
