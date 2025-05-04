/*!
* @file     workstationbuilder.cpp
* @author   Agiliad
* @brief    builder for workstation
* @date     Mar, 03 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/appbuilder/workstationbuilder.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       ~WorkstationBuilder
* @param    None
* @return   None
* @brief    destructor for the class.
*/
WorkstationBuilder::~WorkstationBuilder() {
  TRACE_LOG("");
}

/*!
* @fn       workstationTypeBasedQmlRegister
* @param    QMLEnums::WSType  - wsType
* @param    boost::shared_ptr<ViewManager> viewManager
* @param    boost::shared_ptr<WorkstationManager> workstationManager
* @return   None
* @brief    make connection based on type provided.
*/
void WorkstationBuilder::workstationTypeBasedQmlRegister(QMLEnums::WSType wsType,
                                                         boost::shared_ptr<ViewManager> viewManager,
                                                         boost::shared_ptr<WorkstationManager> workstationManager)
{
  INFO_LOG("Workstation type Received : " << wsType);
#ifdef WORKSTATION
  Q_UNUSED(viewManager);
  Q_UNUSED(workstationManager);
  if (wsType == QMLEnums::WSType::OSR_WORKSTATION)
  {
    DEBUG_LOG("Object register in qml for OSR workstation");
  }
  else if (wsType == QMLEnums::WSType::SEARCH_WORKSTATION || wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION)
  {
    DEBUG_LOG("Object register in qml for Search workstation / Supervisor workstation");
  }
#endif
#ifdef RERUN
  if (wsType == QMLEnums::WSType::RERUN_EMULATOR)
  {
    DEBUG_LOG("Object register in qml for Rerun workstation");
    viewManager->getViewObject()->registerUsbFSModel((dynamic_cast
                                                      <RerunManager*>(workstationManager.get()))->
                                                     getRerunViewerHandler()->getUsbFileSystemModel());
  }
  else if (wsType == QMLEnums::WSType::TRAINING_WORKSTATION)
  {
    DEBUG_LOG("Object register in qml for Training workstation");
    viewManager->getViewObject()->registerUsbFSModel((dynamic_cast
                                                      <TrainingManager*>(workstationManager.get()))->
                                                     getRerunViewerHandler()->getUsbFileSystemModel());
  }
#endif
  else
  {
    ERROR_LOG("Invalid WorkstationType; Check INI file");
  }
}

/*!
* @fn       workstationTypeBasedConnection
* @param    QMLEnums::WSType  - wsType
* @param    boost::shared_ptr<ViewManager> viewManager
* @param    boost::shared_ptr<WorkstationManager> workstationManager
* @return   None
* @brief    make connection based on type provided.
*/
void WorkstationBuilder::workstationTypeBasedConnection(QMLEnums::WSType wsType,
                                                        boost::shared_ptr<ViewManager> viewManager,
                                                        boost::shared_ptr<WorkstationManager> workstationManager,
                                                        boost::shared_ptr<ModelManagers> modalmanager,
                                                        UILayoutManager *uilayoutmanager) {
  Q_UNUSED(modalmanager);
#ifdef WORKSTATION
  if (wsType == QMLEnums::WSType::OSR_WORKSTATION)
  {
    DEBUG_LOG("Connect for OSR workstation");

    connect((dynamic_cast<OsrManager*>(
               workstationManager.get())), SIGNAL(networkStatus(QMLEnums::NetworkState)),
            viewManager.get(), SLOT(onNetworkStatus(QMLEnums::NetworkState)),
            Qt::QueuedConnection);

    connect(viewManager.get(), &ViewManager::setAuthenticatedUserDetailsToScanner,
            (dynamic_cast<OsrManager*>(workstationManager.get())), &OsrManager::setAuthenticatedUserDetailsToScanner);
    connect(workstationManager->getLiveControllerHnd().get(), &NssHandler::
            stateChanged, uilayoutmanager, &UILayoutManager::onStateChanged);
    connect(workstationManager.get(), &WorkstationManager::
            registScannerInDiscovered, viewManager->getViewObject().get(),
            &View::registScannerInDiscovered);
    connect((dynamic_cast<OsrManager*>(workstationManager.get())), &OsrManager::notifyWSConfigUpdated,
             viewManager.get(), &ViewManager::notifyWSConfigUpdated);

    connect((dynamic_cast<OsrHandler*>(
               workstationManager->getLiveControllerHnd().get())),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), viewManager.get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::QueuedConnection);

    connect(viewManager.get(), &ViewManager::bagListCommand,
            workstationManager.get(), &WorkstationManager::
            bagListCommand, Qt::QueuedConnection);

    connect(workstationManager.get(), SIGNAL(
              bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc,
                                    QMLEnums::BagsListAndBagDataCommands,
                                    QVariant)), viewManager.get(), SIGNAL(
              bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc,
                                    QMLEnums::BagsListAndBagDataCommands,
                                    QVariant)), Qt::QueuedConnection);
  }
  else if (wsType == QMLEnums::WSType::SEARCH_WORKSTATION)
  {
    DEBUG_LOG("Connect for  Search workstation");
    connect(viewManager.get(), &ViewManager::setAuthenticatedUserDetailsToScanner,
            (dynamic_cast<SearchManager*>(workstationManager.get())), &SearchManager::setAuthenticatedUserDetailsToScanner);
    connect(workstationManager->getLiveControllerHnd().get(), &NssHandler::
            stateChanged, uilayoutmanager, &UILayoutManager::onStateChanged);
    connect((dynamic_cast<SearchManager*>(
               workstationManager.get())), SIGNAL(networkStatus(QMLEnums::NetworkState)),
            viewManager.get(), SLOT(onNetworkStatus(QMLEnums::NetworkState)),
            Qt::QueuedConnection);
    connect(workstationManager.get(), &WorkstationManager::
            registScannerInDiscovered, viewManager->getViewObject().get(),
            &View::registScannerInDiscovered);
    connect((dynamic_cast<SearchManager*>(workstationManager.get())), &SearchManager::notifyWSConfigUpdated,
             viewManager.get(), &ViewManager::notifyWSConfigUpdated);
    connect((dynamic_cast<SearchHandler*>(
               workstationManager->getLiveControllerHnd().get())), &SearchHandler::bagRetain
            , viewManager.get(), &ViewManager::bagRetain, Qt::QueuedConnection);

    connect((dynamic_cast<SearchHandler*>(
               workstationManager->getLiveControllerHnd().get())),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), viewManager.get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::QueuedConnection);

    connect(viewManager.get(),
            SIGNAL(bagListCommand(QMLEnums::ArchiveFilterSrchLoc,
                                  QMLEnums::BagsListAndBagDataCommands, QVariant)), workstationManager.get(),
            SLOT(bagListCommand(QMLEnums::ArchiveFilterSrchLoc, QMLEnums::
                             BagsListAndBagDataCommands, QVariant)), Qt::QueuedConnection);

    connect(workstationManager.get(), SIGNAL(
              bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc,
                                    QMLEnums::BagsListAndBagDataCommands,
                                    QVariant)), viewManager.get(), SIGNAL(
              bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc,
                                    QMLEnums::BagsListAndBagDataCommands,
                                    QVariant)), Qt::QueuedConnection);
  }
  else if (wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION)
  {
    DEBUG_LOG("Connect for Supervisor workstation");
    connect(viewManager.get(), &ViewManager::setAuthenticatedUserDetailsToScanner,
            (dynamic_cast<SupervisorManager*>(workstationManager.get())), &SupervisorManager::setAuthenticatedUserDetailsToScanner);
    connect(workstationManager->getLiveControllerHnd().get(), &NssHandler::
            stateChanged, uilayoutmanager, &UILayoutManager::onStateChanged);
    connect((dynamic_cast<SupervisorManager*>(
               workstationManager.get())), SIGNAL(networkStatus(QMLEnums::NetworkState)),
            viewManager.get(), SLOT(onNetworkStatus(QMLEnums::NetworkState)),
            Qt::QueuedConnection);
    connect(workstationManager.get(), &WorkstationManager::
            registScannerInDiscovered, viewManager->getViewObject().get(),
            &View::registScannerInDiscovered);
    connect((dynamic_cast<SupervisorManager*>(workstationManager.get())), &SupervisorManager::notifyWSConfigUpdated,
             viewManager.get(), &ViewManager::notifyWSConfigUpdated);
    connect((dynamic_cast<SupervisorHandler*>(
               workstationManager->getLiveControllerHnd().get())), &SupervisorHandler::bagRetain
            , viewManager.get(), &ViewManager::bagRetain, Qt::QueuedConnection);

    connect((dynamic_cast<SupervisorHandler*>(
               workstationManager->getLiveControllerHnd().get())),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), viewManager.get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::QueuedConnection);

    connect(viewManager.get(),
            SIGNAL(bagListCommand(QMLEnums::ArchiveFilterSrchLoc,
                                  QMLEnums::BagsListAndBagDataCommands, QVariant)), workstationManager.get(),
            SLOT(bagListCommand(QMLEnums::ArchiveFilterSrchLoc, QMLEnums::
                             BagsListAndBagDataCommands, QVariant)), Qt::QueuedConnection);

    connect(workstationManager.get(), SIGNAL(
              bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc,
                                    QMLEnums::BagsListAndBagDataCommands,
                                    QVariant)), viewManager.get(), SIGNAL(
              bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc,
                                    QMLEnums::BagsListAndBagDataCommands,
                                    QVariant)), Qt::QueuedConnection);
  }
  else if (wsType == QMLEnums::WSType::TRAINING_WORKSTATION)
  {
    DEBUG_LOG("Connect for  Training workstation");
  }
#endif
#ifdef RERUN
  Q_UNUSED(uilayoutmanager);
  if (wsType == QMLEnums::WSType::RERUN_EMULATOR)
  {
    DEBUG_LOG("Connect for  Rerun workstation");
    // for rerun command
    connect(viewManager->getViewObject().get(), &View::rerunCommand,
            (dynamic_cast<RerunManager*>(workstationManager.get())) ,
            &RerunManager::onRerunCommand ,  Qt::QueuedConnection);

    // for rerun response status
    connect((dynamic_cast<RerunManager*>(
               workstationManager.get()))->getRerunHandler()->getConnectEmulator(),
            &ConnectEmulator::rerunResponse, viewManager.get()->getViewObject().get(),
            &View::displayRerunStatus, Qt::QueuedConnection);

    connect((dynamic_cast<RerunManager*>(
               workstationManager.get()))->getRerunHandler()->getUsbStorageService(),
            &UsbStorageSrv::usbMountevent, viewManager.get()->getViewObject().get(),
            &View::displayRerunStatus, Qt::QueuedConnection);

    connect((dynamic_cast<RerunManager*>(
               workstationManager.get()))->getRerunHandler()->getConnectEmulator(),
            SIGNAL(algListResponse(QStringList)), viewManager.get()->getViewObject().get(),
            SIGNAL(notifyAlgoList(QStringList)), Qt::QueuedConnection);    


    connect(viewManager->getViewObject().get(), &View::notifyViewSelectedBags,
            (dynamic_cast<RerunManager*>(workstationManager.get()))->
            getRerunDataAcqHandler().get() ,
            &RerunDataAcqHandler::onViewSelectedBags, Qt::QueuedConnection);

    connect(viewManager->getViewObject().get(), &View::notifyViewSelectedBags,
            (dynamic_cast<RerunManager*>(workstationManager.get()))->
            getRerunViewerHandler().get() ,
            &RerunViewerHandler::onViewSelectedBags, Qt::QueuedConnection);

    connect(viewManager->getViewObject().get(), &View::notifytoShowNextRerunBag,
            (dynamic_cast<RerunManager*>(workstationManager.get()))->
            getRerunViewerHandler().get() ,
            &RerunViewerHandler::showNextBag);

    connect(viewManager->getViewObject().get(), &View::notifyOperatorDecision,
            (dynamic_cast<RerunManager*>(workstationManager.get()))->
            getRerunViewerHandler().get() ,
            &RerunViewerHandler::saveOperatorDecision);


    connect((dynamic_cast<RerunManager*>(workstationManager.get()))->
            getRerunViewerHandler().get(), &RerunViewerHandler::
            notifyToStopShowingBag, viewManager->getViewObject().get(),
            &View::notifyToStopShowingBag,
            Qt::QueuedConnection);

    connect((dynamic_cast<RerunManager*>(workstationManager.get()))->
            getRerunViewerHandler().get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), viewManager.get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::QueuedConnection);

    connect((dynamic_cast<RerunManager*>(workstationManager.get()))->
            getRerunDataAcqHandler().get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), viewManager.get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::QueuedConnection);
  }
  else if(wsType == QMLEnums::WSType::TRAINING_WORKSTATION)
  {
   connect((dynamic_cast<TrainingManager*>(
                    workstationManager.get())), SIGNAL(networkStatus(QMLEnums::NetworkState)),
                 viewManager.get(), SLOT(onNetworkStatus(QMLEnums::NetworkState)),
                 Qt::QueuedConnection);

    connect(viewManager->getViewObject().get(), &View::notifyViewSelectedBags,
            (dynamic_cast<TrainingManager*>(workstationManager.get()))->
            getRerunDataAcqHandler().get() ,
            &RerunDataAcqHandler::onViewSelectedBags, Qt::QueuedConnection);

    connect(viewManager->getViewObject().get(), &View::notifyViewSelectedBags,
            (dynamic_cast<TrainingManager*>(workstationManager.get()))->
            getRerunViewerHandler().get() ,
            &RerunViewerHandler::onViewSelectedBags, Qt::QueuedConnection);

    connect((dynamic_cast<TrainingManager*>(workstationManager.get()))->
            getRerunViewerHandler().get(), &RerunViewerHandler::
            notifyToStopShowingBag, viewManager->getViewObject().get(),
            &View::notifyToStopShowingBag,
            Qt::QueuedConnection);

    connect((dynamic_cast<TrainingManager*>(workstationManager.get()))->
            getRerunViewerHandler().get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), viewManager.get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::QueuedConnection);

    connect((dynamic_cast<TrainingManager*>(workstationManager.get()))->
            getRerunDataAcqHandler().get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), viewManager.get(),
            SIGNAL(notifyToChangeOrNotScreen(bool, QString, int)), Qt::QueuedConnection);

    connect((dynamic_cast<TrainingManager*>(workstationManager.get()))->
            getRerunViewerHandler().get(), &RerunViewerHandler::
            notifyToStopShowingBag, viewManager->getViewObject().get(),
            &View::notifyToStopShowingBag,
            Qt::QueuedConnection);

    connect(viewManager->getViewObject().get(), &View::notifytoShowNextRerunBag,
            (dynamic_cast<TrainingManager*>(workstationManager.get()))->
            getRerunViewerHandler().get() ,
            &RerunViewerHandler::showNextBag);

    connect((dynamic_cast<TrainingManager*>(workstationManager.get())), &TrainingManager::notifyWSConfigUpdated,
             viewManager.get(), &ViewManager::notifyWSConfigUpdated);

    connect(viewManager->getViewObject().get(), &View::notifyOperatorDecision,
            (dynamic_cast<TrainingManager*>(workstationManager.get()))->
            getRerunViewerHandler().get() ,
            &RerunViewerHandler::saveOperatorDecision);
  }
#endif
  else
  {
    ERROR_LOG("Invalid WorkstationType; Check INI file");
  }
}
}  // end of namespace ws
}  // end of namespace analogic
