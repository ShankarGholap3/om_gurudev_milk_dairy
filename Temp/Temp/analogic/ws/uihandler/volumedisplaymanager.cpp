/*!
* @file     volumedisplaymanager.cpp
* @author   Agiliad
* @brief    Class for managing the contents and visibility of the volumes.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <boost/shared_ptr.hpp>

#include <QDir>
#include <QUrl>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QProcess>
#include <QDateTime>
#include <QQmlEngine>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlContext>
#include <QVariantList>
#include <QApplication>
#include <QDesktopWidget>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QQmlApplicationEngine>

#include <Logger.h>
#include <errors.h>
#include <frameworkcommon.h>
#ifdef WORKSTATION
#include <analogic/ws/osr/osrconfig.h>
#endif
#include <analogic/ws/ulm/dataobject.h>
#include <analogic/ws/uihandler/view.h>
#include <analogic/ws/ulm/wsscreendao.h>
#include <analogic/ws/ulm/uilayoutmanager.h>
#include <analogic/ws/uihandler/volumedisplay.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>

#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/uihandler/volumedisplaymanager.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       VolumeDisplayManager()
* @param    View* parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
VolumeDisplayManager::VolumeDisplayManager(View *parent):
  QObject(parent),
  m_vtkScreenDisplayed(false),
  m_displayAreaWidth(1000),
  m_displayAreaHeight(700),
  m_renderMutex(nullptr),
  m_AutoClearTimer(nullptr),
  m_timerPollForBags(nullptr),
  m_topLeftCorner(0, 0),
  m_qmlContainerRef(nullptr),
  m_volumeDisplay(nullptr),
  m_isArchiveOrSearchScreenDisplayed(false)
{
  TRACE_LOG("");
  DEBUG_LOG("Creating instance of VolumeDisplay for front bag");
  m_volumeDisplay = new VolumeDisplay(parent);
  THROW_IF_FAILED((m_volumeDisplay == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);

  m_volumeDisplay->setFront(true);

  DEBUG_LOG("Connecting signals and their resp slots.");
  connect(m_volumeDisplay, SIGNAL(backgroundProcessFinished()),
          this, SLOT(volumeDisplayBackgroundProcessFinished()), Qt::QueuedConnection);

  connect(m_volumeDisplay, SIGNAL(updateScreeningPanelEnability(bool)),
          this, SIGNAL(updateScreeningPanelEnability(bool)), Qt::QueuedConnection);

  connect(m_volumeDisplay, &VolumeDisplay::bagRenderedComplete,
          this, &VolumeDisplayManager::bagRenderedComplete, Qt::QueuedConnection);
}

/*!
* @fn       VolumeDisplayManager()
* @param    View* parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
VolumeDisplayManager::VolumeDisplayManager(View *parent,
                                           VolumeDisplay* ptr):
  QObject(parent),
  m_vtkScreenDisplayed(false),
  m_displayAreaWidth(1000),
  m_displayAreaHeight(700),
  m_renderMutex(nullptr),
  m_AutoClearTimer(nullptr),
  m_timerPollForBags(nullptr),
  m_topLeftCorner(0, 0),
  m_qmlContainerRef(nullptr),
  m_volumeDisplay(nullptr),
  m_isArchiveOrSearchScreenDisplayed(false)
{
  TRACE_LOG("");
  DEBUG_LOG("Creating instance of VolumeDisplay for front bag");
  m_volumeDisplay = ptr;
  THROW_IF_FAILED((m_volumeDisplay == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);

  m_volumeDisplay->setFront(true);

  DEBUG_LOG("Connecting signals and their resp slots.");
  connect(m_volumeDisplay, SIGNAL(backgroundProcessFinished()),
          this, SLOT(volumeDisplayBackgroundProcessFinished()), Qt::QueuedConnection);

  connect(m_volumeDisplay, SIGNAL(updateScreeningPanelEnability(bool)),
          this, SIGNAL(updateScreeningPanelEnability(bool)), Qt::QueuedConnection);

  connect(m_volumeDisplay, &VolumeDisplay::bagRenderedComplete,
          this, &VolumeDisplayManager::bagRenderedComplete, Qt::QueuedConnection);
}

/*!
* @fn      ~VolumeDisplayManager()
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
VolumeDisplayManager::~VolumeDisplayManager()
{
  TRACE_LOG("");
  SAFE_DELETE(m_timerPollForBags);
  SAFE_DELETE(m_AutoClearTimer);
  SAFE_DELETE(m_volumeDisplay);
  SAFE_DELETE(m_renderMutex);
}

/*!
* @fn       initialize()
* @param    QQuickView *qmlContainer
* @param    QWidget *centralWidget
* @return   None
* @brief    initialize members of class View.
*/
void VolumeDisplayManager::initialize(QQuickView *qmlContainer, QWidget *centralWidget)
{
  TRACE_LOG("");
  DEBUG_LOG("Initalizing members.");

  m_renderMutex = new QMutex();

  m_qmlContainerRef = qmlContainer;
  DEBUG_LOG("Initializing Volume Rendering Widgets of front bag");
  m_volumeDisplay->initialize(qmlContainer, centralWidget, m_renderMutex);

  DEBUG_LOG("Creating timer for bag polling.");
  m_timerPollForBags = new QTimer(this);
  THROW_IF_FAILED((m_timerPollForBags == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  connect(m_timerPollForBags, SIGNAL(timeout()), this, SLOT(pollForBagsTimerSlot()));

  DEBUG_LOG("Creating timer for auto clear.");
  m_AutoClearTimer = new QTimer(this);
  THROW_IF_FAILED((m_AutoClearTimer == NULL)? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_AutoClearTimer->setSingleShot(true);

#ifdef WORKSTATION
  if((OsrConfig::getIsAutoclearEnabled() == true))
  {
    connect(m_AutoClearTimer, SIGNAL(timeout()), this, SLOT(autoClearBag()));
  }
#endif
  DEBUG_LOG("Starting polling timer.");
  m_timerPollForBags->start(1000);
  vtkSettingsChanged(QMLEnums::TEPC_DISPLAY_ALARM_THREAT_ONLY);
}

/*!
* @fn       bagDataReceived
* @param    boost::shared_ptr<BagData> bagdata
* @param    bool status
* @param    QString message
* @return   None
* @brief    This function will call on bag data received
*/
void VolumeDisplayManager::bagDataReceived(boost::shared_ptr<BagData> bagdata,
                                           bool status, QString message)
{
  Q_UNUSED(bagdata);
  Q_UNUSED(status);
  Q_UNUSED(message);
  pollForBagsTimerSlot();
}

/*!
* @fn       pollForBagsTimerSlot()
* @param    None
* @return   None
* @brief    Gets triggered at fixed time interval.
*/
void VolumeDisplayManager::pollForBagsTimerSlot()
{
  TRACE_LOG("");
  static bool timerReentryGuard = false;

  // safety to avoid messes caused by re-entry during processing.
  // Would come into picture when processing speed drops below timer frequency. (slower machines).
  if(timerReentryGuard)
    return;
  timerReentryGuard = true;

  if(!m_vtkScreenDisplayed && !m_isArchiveOrSearchScreenDisplayed)
  {
    DEBUG_LOG("Neither bag screening screen is displayed nor archive screen is displayed.");
    timerReentryGuard = false;
    return;
  }

  // Using only front widget for now. Rendering happens on the main thread.
  if(m_volumeDisplay->hasABag())
  {
    timerReentryGuard = false;
    DEBUG_LOG("front has a bag.");
    return;
  }

  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->getBagbyState(BagData::READYTORENDER);

  if(bagData == NULL)
  {
    DEBUG_LOG("Bag data is NULL.");
    timerReentryGuard = false;
    return;
  }

  // This condition should never be true.
  if(m_isArchiveOrSearchScreenDisplayed && m_volumeDisplay->hasABag())
  {
    FATAL_LOG("Attempting to view a local archive bag while in live bag screening state.");
    return;
  }


  bagData->setState(BagData::RENDERING);

  View* viewRef = static_cast<View*>(parent());
  if(bagData != NULL)
  {
#ifdef WORKSTATION
    if (bagData->getVolumeDimension().z() != bagData->getSliceCountReceived())
    {
      QString msg = tr("Received slice count is different than the slice_count property. Received Slices:") +
          QString::number(bagData->getSliceCountReceived()) + tr(", Slice Count Property:")
          +QString::number( bagData->getVolumeDimension().z());
      bagData->addErrorMsg(msg);
      bagData->setError(true);
    }
#endif

    //Populate TDR message from TDR.
    bagData->populateErrorMsgFromTDR();
    if (viewRef)
    {
      if (bagData->isBagError())
      {

        emit viewRef->authstatuschanged(Errors::E_FAIL, bagData->getErrorMsg(), QMLEnums::QML_MESSAGE_MODEL);

      }


#ifdef WORKSTATION
      if ((bagData->getLivebag() != NULL))
      {
        QString rfid = "";
        analogic::nss::NssBag *livebag = bagData->getLivebag();
        for (int i = 0; i < livebag->get_extended_property_count(); ++i)
        {
          const analogic::nss::Property& prop = livebag->get_extended_property(i);

          DEBUG_LOG("For bag " << livebag->get_transfer_bag_identifier() <<
                    " received extended property " << prop.key << "=" << prop.value);

          // If the ObviousThreatLatch is set, note this in the bag data
          // so the dialog is shown
          if (prop.key == analogic::nss::kExtendedPropertyObviousThreatLatch) {

            INFO_LOG("Found ObviousThreatLatch extended property; setting flag");
            bagData->setObviousThreatFlag(true, true);
            // RFID on image overlay for SEARCH, RECALL and PVS.
            if(bagData->getVolumeCtModule())
            {
              SDICOS::Array1D<SDICOS::ObjectOfInspectionModule::IdInfo>& alternateIDs =
                  bagData->getVolumeCtModule()->GetObjectOfInspection().GetAlternateIDs();
              int count_alt = alternateIDs.GetSize();
              for (int i = 0; i < count_alt; i++)
              {
                std::string signID   = alternateIDs[i].GetID().Get();
                if (alternateIDs[i].GetIdType() == SDICOS::ObjectOfInspectionModule::IdInfo::enumRFID )
                {
                  if(!(signID.empty()) && (signID.compare(bagData->getBagid()) != 0))
                  {
                    rfid += signID.c_str();
                  }
                }
              }
            }
            QString msg = WorkstationConfig::getInstance()->getHighThreatUpperText()
                + QString(" ")
                + WorkstationConfig::getInstance()->getHighThreatLowerText()
                + tr(" has been attached to this bag: ")
                + (rfid.isEmpty() ? bagData->getVolumeCtModule()->GetObjectOfInspection().GetID().GetID().Get() : rfid);//livebag->get_transfer_bag_identifier().c_str();
            emit viewRef->authstatuschanged(Errors::E_FAIL, msg, QMLEnums::QML_MESSAGE_MODEL);
          }
        }
      }
#endif
    }
  }

  if(!m_volumeDisplay->hasABag())
  {
    DEBUG_LOG("Bag Flow Event:: Rendering to front" << bagData->getBagid());

    //--------------------------------------
    // ARO-ADDED: 2-14-2020
//    m_enableVtkVolumeComputation = false;
    //--------------------------------------

    m_volumeDisplay->renderVolume(bagData);

    showBag();
  }

  timerReentryGuard = false;

}

/*!
* @fn       autoClearBag()
* @param    None
* @return   None
* @brief    Auto clear the bag after set period as per user config.
*/
void VolumeDisplayManager::autoClearBag()
{
  if(!m_AutoClearTimer)
    return;

  m_AutoClearTimer->stop();
  INFO_LOG("**YYEYY**: Stopping Auto Clear timer");
  setBagDecisionTime();
  applyDecision(QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG);
}

/*!
* @fn       screenChanged();
* @param    QMLEnums::ScreenEnum prevScreen
* @param    QMLEnums::ScreenEnum newScreen
* @return   None
* @brief    Handling of change in screen.
*/
void VolumeDisplayManager::screenChanged(QMLEnums::ScreenEnum prevScreen, QMLEnums::ScreenEnum newScreen)
{
  if(prevScreen == newScreen)
    return;

  if(prevScreen == QMLEnums::ScreenEnum::BAGSCREENING_SCREEN ||
     prevScreen == QMLEnums::ScreenEnum::LOCAL_ARCHIVE_BAG_SCREEN ||
     prevScreen == QMLEnums::ScreenEnum::REMOTE_ARCHIVE_BAG_SCREEN ||
     prevScreen == QMLEnums::ScreenEnum::SEARCH_BAG_SCREEN ||
     prevScreen == QMLEnums::ScreenEnum::RERUNSELECTION_BAG_SCREEN
     || (prevScreen == QMLEnums::ScreenEnum::TRAINING_BAG_SCREEN))
  {
    if(m_volumeDisplay->hasABag())
    {
      if(m_vtkScreenDisplayed)
      {
        FATAL_LOG("Leaving screening screen while bag is not cleared.");
      }

      m_volumeDisplay->hide();
    }

    m_AutoClearTimer->stop();
    m_vtkScreenDisplayed = false;
    m_isArchiveOrSearchScreenDisplayed = false;
  }

  if((newScreen == QMLEnums::ScreenEnum::BAGSCREENING_SCREEN)
     || (newScreen == QMLEnums::ScreenEnum::RERUNSELECTION_BAG_SCREEN)
     || (newScreen == QMLEnums::ScreenEnum::TRAINING_BAG_SCREEN))
  {
    m_vtkScreenDisplayed = true;
    m_isArchiveOrSearchScreenDisplayed = false;
    m_volumeDisplay->setArchive(false);
    showBag();
  }
  else if((newScreen == QMLEnums::ScreenEnum::LOCAL_ARCHIVE_BAG_SCREEN) ||
          (newScreen == QMLEnums::ScreenEnum::REMOTE_ARCHIVE_BAG_SCREEN) ||
          (newScreen == QMLEnums::ScreenEnum::SEARCH_BAG_SCREEN))
  {
    m_vtkScreenDisplayed = false;
    m_isArchiveOrSearchScreenDisplayed = true;
    if((newScreen == QMLEnums::ScreenEnum::LOCAL_ARCHIVE_BAG_SCREEN) ||
       (newScreen == QMLEnums::ScreenEnum::REMOTE_ARCHIVE_BAG_SCREEN)) {
      m_volumeDisplay->setArchive(true);
    }
    else {
      m_volumeDisplay->setArchive(false);
    }
    showBag();
  }
}

/*!
* @fn       vtkScreenLoaded()
* @param    None
* @return   None
* @brief    Called when QML screen is loaded. Shows the bag if already available and rendered.
*/
void VolumeDisplayManager::vtkScreenLoaded()
{
  if(m_qmlContainerRef)
  {
    QQuickItem *rootObject = m_qmlContainerRef->rootObject();
    if(rootObject)
    {
      QObject *displayArea = rootObject->findChild<QObject*>("nameRowDisplayArea");
      if(displayArea)
      {
        QQuickItem *displayAreaD = qobject_cast<QQuickItem*>(displayArea);
        if(displayAreaD && displayAreaD->isVisible())
        {
          m_displayAreaWidth = displayAreaD->property("width").toDouble();
          m_displayAreaHeight = displayAreaD->property("height").toDouble();
          m_topLeftCorner = displayAreaD->mapToGlobal(QPointF(0, 0));
        }
      }
    }
  }

  m_volumeDisplay->setGeometry(m_topLeftCorner, m_displayAreaWidth, m_displayAreaHeight);
  showBag();
}

/*!
* @fn       showBag()
* @param    None
* @return   None
* @brief    Make front widget visible if all conditions met.
*/
void VolumeDisplayManager::showBag()
{
  if(m_vtkScreenDisplayed || m_isArchiveOrSearchScreenDisplayed)
  {
    bool status = m_volumeDisplay->show();
    DEBUG_LOG("show bag status: "<<status);
#ifdef WORKSTATION

    if(status && m_vtkScreenDisplayed && OsrConfig::getIsAutoclearEnabled() && m_AutoClearTimer)
    {
      INFO_LOG("**YYYSYY** Starting Auto Clear timer");
      m_AutoClearTimer->start(OsrConfig::getAutoclearTimeout()*1000);
    }
#endif

  }
}

/*!
* @fn       showNextBag()
* @param    None
* @return   None
* @brief    Clear front bag.
*/
void VolumeDisplayManager::showNextBag()
{
  if(!m_vtkScreenDisplayed)
    return;

  DEBUG_LOG("Bag Flow Event:: Clearing front bag.");

  m_volumeDisplay->clearWidgets();
  showBag();
}

/*!
* @fn       vtkSettingsChanged
* @param    QMLEnums::ThreatEvalPanelCommand command
* @return   None
* @brief    Responsible for processing all user events.
*/
void VolumeDisplayManager::vtkSettingsChanged(QMLEnums::ThreatEvalPanelCommand command)
{
  DEBUG_LOG("vtkSettingsChanged " << command);
  if(command < 0 || command >= QMLEnums::ThreatEvalPanelCommand::TEPC_MAX)
  {
    ERROR_LOG("Wrong ThreatEvalPanelCommand " << command);
    return;
  }

  if ((command == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG)
      || (command == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG)
      || (command == QMLEnums::ThreatEvalPanelCommand::TEPC_TIMEOUT_BAG)
      || (command == QMLEnums::ThreatEvalPanelCommand::TEPC_RETAIN_BAG)
      )
  {
    applyDecision(command);
  }
  else if (command == QMLEnums::ThreatEvalPanelCommand::TEPC_NEXT_BAG)
  {
    showNextRerunBag();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_STOP_BAG)
  {    
    stopShowingRerunBag();
  }
  else
  {
    m_volumeDisplay->uiCommand(command);
  }
}

/*!
     * @fn       getBagRenderedtime
     * @param    None
     * @return   QDateTime
     * @brief    get time when bag rendering start
     */
QDateTime VolumeDisplayManager::getBagRenderedtime()
{
  return m_volumeDisplay->getBagRenderedtime();
}
/*!
* @fn       getTotalVisibleAlarmThreats
* @param    None
* @return   int
* @brief    get total visible alarm threat.
*/
int VolumeDisplayManager::getTotalVisibleAlarmThreats()
{
  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->getBagbyState(BagData::RENDERING);

  if(bagData == NULL)
  {
    DEBUG_LOG("Bag data is NULL.")
        return 0;
  }

  // For Live bag next threat add operator threrats
  int retVal = bagData->getTotalVisibleAlarmThreats();

  return retVal;
}

/*!
* @fn       isCurrentThreatOperator
* @param    None
* @return   bool
* @brief    return true if current threat is operator
*/
bool VolumeDisplayManager::isCurrentThreatOperator()
{
  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->getBagbyState(BagData::RENDERING);
  if(bagData == NULL)
  {
    DEBUG_LOG("Bag data is NULL.");
    return false;
  }
  // For Live bag next threat add operator threrats
  return bagData->isCurrentThreatOperator();
}

/*!
* @fn       getTotalNoAlarmThreats
* @param    None
* @return   int
* @brief    get total non alarm threat.
*/
int VolumeDisplayManager::getTotalNoAlarmThreats()
{
  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->getBagbyState(BagData::RENDERING);

  if(bagData == NULL)
  {
    DEBUG_LOG("Bag data is NULL.")
        return 0;
  }

  // For Live bag next threat add operator threrats
  int retVal =   bagData->getTotalNoAlarmThreats();

  return retVal;
}

/*!
* @fn       getOperatorAlarmThreat
* @param    None
* @return   int
* @brief    return operator alarm threats
*/
int VolumeDisplayManager::getOperatorAlarmThreat()
{
  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->getBagbyState(BagData::RENDERING);

  if(bagData == NULL)
  {
    DEBUG_LOG("Bag data is NULL.")
        return 0;
  }

  // return operator alarm threats
  int retVal =  bagData->getOperatorAlarmThreat();

  return retVal;
}

/*!
* @fn       isBagError
* @param    None
* @return   bool
* @brief    tells if bag has error.
*/
bool VolumeDisplayManager::isBagError()
{
  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->
      getBagbyState(BagData::RENDERING);
  if(bagData == NULL)
  {
    DEBUG_LOG("Bag data is NULL.")
        return 0;
  }
  return bagData->isBagError();
}

/*!
* @fn       flushBagDataPool
* @param    None
* @return   None
* @brief    flush BagDataPool as network is either reconnecting or unregistered
*/
void VolumeDisplayManager::flushBagDataPool()
{
  BagsDataPool* bagqueue = BagsDataPool::getInstance();
  for (int i=0; i<bagqueue->getBagQueueCount(); i++) {
    boost::shared_ptr<BagData> bagData = bagqueue->getBagbyState(
          BagData::READYTORENDER | BagData::RENDERING |
          BagData::CONSTRUCTION | BagData::INITIALISED);
    if(bagData != NULL) {
      bagData->setState(BagData::DISPOSED);
    }
  }
  m_volumeDisplay->clearWidgets();
}

/*!
* @fn       getIsAutoclearEnabled
* @param    None
* @return   bool
* @brief    check if auto clear enabled
*/
bool VolumeDisplayManager::getIsAutoclearEnabled()
{
  bool isAutclear = false;
#ifdef WORKSTATION
  isAutclear = OsrConfig::getIsAutoclearEnabled();
#endif
  return isAutclear;
}

/*!
* @fn       applyForward()
* @param    None
* @return   None
* @brief    Apply user press of forward button to 3D image.
*/
void VolumeDisplayManager::applyForward()
{
  m_vtkScreenDisplayed = false;
  m_isArchiveOrSearchScreenDisplayed = false;
  m_volumeDisplay->applyForward();
}

/*!
* @fn       applyDecision
* @param    QMLEnums::ThreatEvalPanelCommand
* @return   None
* @brief    Apply user decision
*/
void VolumeDisplayManager::applyDecision(QMLEnums::ThreatEvalPanelCommand decision)
{
  m_volumeDisplay->applyDecision(decision);

  if (m_vtkScreenDisplayed)
  {
    showNextBag();
  }

  if (m_isArchiveOrSearchScreenDisplayed)
  {
    changeBagScrnToBagListScrn();
  }

}

/*!
* @fn       showTipThreat
* @param    None
* @return   None
* @brief    Show Tip Threat
*/
void VolumeDisplayManager::showTipThreat(QMLEnums::ThreatEvalPanelCommand command)
{
  QMLEnums::ScreenEnum screen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  if (m_volumeDisplay->isTIPBag()
      && ((screen== QMLEnums::ScreenEnum::BAGSCREENING_SCREEN)
          || (screen== QMLEnums::ScreenEnum::TRAINING_BAG_SCREEN)))
  {
    m_volumeDisplay->showTIPThreat();
    QMLEnums::TIPResult tipResult;
    QString tipResultMsg = m_volumeDisplay->validateOperatorDecisionForTIP(command, tipResult);
    emit showTIPResult(tipResultMsg, tipResult);
  }
}

/*!
* @fn       validateUserDecisionForNonTIPBag
* @param    QMLEnums::ThreatEvalPanelCommand command
* @return   bool
* @brief    validate user decision for Non TIP Bag
*/
bool VolumeDisplayManager::validateUserDecisionForNonTIPBag(QMLEnums::ThreatEvalPanelCommand command)
{
  QString tipResultMsg;
  bool res = false;
  QMLEnums::ScreenEnum screen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  if (!m_volumeDisplay->isTIPBag()
      && (WorkstationConfig::getInstance()->getNonTipDialogEnable())
      && (screen== QMLEnums::ScreenEnum::BAGSCREENING_SCREEN))
  {
    QMLEnums::TIPResult tipResult;
    if ( (command != QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG)
         && (command != QMLEnums::ThreatEvalPanelCommand::TEPC_TIMEOUT_BAG)
         )
    {
      return res;
    }

    tipResultMsg = WorkstationConfig::getInstance()->getTipNONTIPMsgDialog();
    tipResult = QMLEnums::TIP_NONE;

    emit showTIPResult(tipResultMsg, tipResult);
    res = true;
  }
  return res;
}

/*!
* @fn       showNextRerunBag()
* @param    None
* @return   None
* @brief    Apply user press of NextBag button.
*/
void VolumeDisplayManager::showNextRerunBag()
{
  m_volumeDisplay->clearBag();
  View* viewRef = static_cast<View*>(parent());
  if (viewRef) {
    emit viewRef->notifytoShowNextRerunBag();
  }
}
/*!
* @fn       stopShowingRerunBag()
* @param    None
* @return   None
* @brief    Apply user press of StopBag button.
*/
void VolumeDisplayManager::stopShowingRerunBag()
{
  m_volumeDisplay->clearBag();
  changeBagScrnToBagListScrn();
}

/*!
* @fn       changeBagScrnToBagListScrn()
* @param    None
* @return   None
* @brief    change Screen To Recall Archive Or Search screen
*/
void VolumeDisplayManager::changeBagScrnToBagListScrn()
{
  QQuickItem *rootObject = (m_qmlContainerRef)->rootObject();

  if(!rootObject) {
    return;
  }
  QObject *qobj = rootObject->findChild<QObject*>("appContainer");
  if(!qobj) {
    return;
  }

  View* viewRef = static_cast<View*>(parent());
  if (viewRef)
  {
    if ((viewRef->getCurrentView() == QMLEnums::ScreenEnum::
         LOCAL_ARCHIVE_BAG_SCREEN) || (viewRef->getCurrentView() ==
                                       QMLEnums::ScreenEnum::REMOTE_ARCHIVE_BAG_SCREEN))
    {
      QMetaObject::invokeMethod(qobj, "invokeCentralscreenchange",
                                Q_ARG(QVariant, QMLEnums::ScreenEnum::RECALLBAG_SCREEN));
    }
    else if (viewRef->getCurrentView() == QMLEnums::ScreenEnum::
             SEARCH_BAG_SCREEN)
    {
      QMetaObject::invokeMethod(qobj, "invokeCentralscreenchange",
                                Q_ARG(QVariant, QMLEnums::ScreenEnum::SEARCH_SCREEN));
    }
    else if (viewRef->getCurrentView() == QMLEnums::ScreenEnum::
             RERUNSELECTION_BAG_SCREEN)
    {
      if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::RERUN_EMULATOR)
      {
        QMetaObject::invokeMethod(qobj, "invokeCentralscreenchange",
                                  Q_ARG(QVariant, QMLEnums::ScreenEnum::RERUNSELECTION_SCREEN));
      }
    }
    else if (viewRef->getCurrentView() == QMLEnums::ScreenEnum::
             TRAINING_BAG_SCREEN)
    {
      if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION) {
        QMetaObject::invokeMethod(qobj, "invokeCentralscreenchange",
                                  Q_ARG(QVariant, QMLEnums::ScreenEnum::TRAINING_VIEWER_SCREEN));
      }
    }
    else if (viewRef->getCurrentView() == QMLEnums::ScreenEnum::
             SUPERVISOR_SCREEN)
    {
      if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::SUPERVISOR_WORKSTATION) {
      QMetaObject::invokeMethod(qobj, "invokeCentralscreenchange",
                                Q_ARG(QVariant, QMLEnums::ScreenEnum::SUPERVISOR_SCREEN));
      }
    }

  }
}

/*!
* @fn       volumeDisplayBackgroundProcessFinished()
* @param    None
* @return   None
* @brief    Slot executes on main thread on completion of a background process. Update UI.
*/
void VolumeDisplayManager::volumeDisplayBackgroundProcessFinished()
{
  showBag();
}

/*!
* @fn       isFrontWidgetVolumeRendered();
* @param    None
* @return   bool
* @brief    Tell if the front widget volume is rendered.
*/
bool VolumeDisplayManager::isFrontWidgetVolumeRendered()
{
  if(m_volumeDisplay)
  {
    DEBUG_LOG("Return front widget render status");
    return m_volumeDisplay->isRendered();
  }
  return false;
}
#ifdef WORKSTATION
/*!
* @fn       setObviousThreatFlag
* @param    bool - flag
* @return   None
* @brief    set obvious threat flag
*/
void VolumeDisplayManager::setObviousThreatFlag(bool flag)
{
  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->getBagbyState(BagData::RENDERING);
  if(bagData == NULL)
  {
    DEBUG_LOG("Bag data is NULL.");
    return;
  }
  else
  {
    bagData->setObviousThreatFlag(flag);
  }
}

#endif

/*!
* @fn       getIsThreatOnlyMode
* @param    None
* @return   bool
* @brief    gets if threat only mode on threat volume rendered
*/
bool VolumeDisplayManager::getIsThreatOnlyMode()
{
  return m_volumeDisplay->getDisplayAlarmThreat();
}

/*!
* @fn       getEnableThreatColorization()
* @param    None
* @return   bool
* @brief    get threat colorization value
*/
bool VolumeDisplayManager::getEnableThreatColorization()
{
  return m_volumeDisplay->getEnableThreatColorization();
}

/*!
* @fn       getSliceCountReceived
* @return   int
* @brief    return number of slice count received
*/

int VolumeDisplayManager::getSliceCountReceived()
{
  return m_volumeDisplay->getSliceCountReceived();
}

/*!
* @fn       canApplyClearDecisionOnCurrentThreat
* @return   bool
* @brief    return if clear decision applied on current threat or not
*/
bool VolumeDisplayManager::canApplyClearDecisionOnCurrentThreat()
{
  return m_volumeDisplay->canApplyClearDecisionOnCurrentThreat();
}

/*!
* @fn       canApplyDecisionOnCurrentThreat
* @return   bool
* @brief    return if decision applied on current threat or not
*/
bool VolumeDisplayManager::canApplyDecisionOnCurrentThreat()
{
  return m_volumeDisplay->canApplyDecisionOnCurrentThreat();
}


/*!
* @fn       canTakeSuspectDecisionOnBag
* @return   bool
* @brief    return true if decsion is taken on all threat
*/
bool VolumeDisplayManager::canTakeSuspectDecisionOnBag()
{
  return m_volumeDisplay->canTakeSuspectDecisionOnBag();
}

/*!
* @fn       canTakeClearDecisionOnBag
* @return   bool
* @brief    return true if any threat marked as suspect
*/
bool VolumeDisplayManager::canTakeClearDecisionOnBag()
{
  return m_volumeDisplay->canTakeClearDecisionOnBag();
}

/*!
* @fn       setBagDecisionTime
* @return   None
* @brief    sets Bag Decision Time.
*/
void VolumeDisplayManager::setBagDecisionTime()
{
  m_volumeDisplay->setBagDecisionTime();
}

/*!
* @fn       getOperatorDecisiontime
* @param    None
* @return   float
* @brief    get time taken to take decision by operator for training simulator
*/
float VolumeDisplayManager::getOperatorDecisiontime()
{
  return m_volumeDisplay->getOperatorDecisiontime();
}

/*!
* @fn       isViewModeEnabled
* @return   bool
* @brief    return if view mode is enabled
*/
bool VolumeDisplayManager::isViewModeEnabled()
{
  return m_volumeDisplay->isViewModeEnabled();
}

/*!
* @fn       isVRModeVisible
* @return   bool
* @brief    return if vrmode argument needed visible on UI
*/
bool VolumeDisplayManager::isVRModeVisible(VREnums::VRViewModeEnum vrmode)
{
  return m_volumeDisplay->isVRModeVisible(vrmode);
}

/*!
* @fn       isNextThreatAvailable
* @return   bool
* @brief    return if next threat is available
*/
bool VolumeDisplayManager::isNextThreatAvailable()
{
  return m_volumeDisplay->isNextThreatAvailable();
}

/*!
* @fn       isCurrentSelectedEntityMachineThreat
* @return   bool
* @brief    return if selected entity is machine threat
*/
bool VolumeDisplayManager::isCurrentSelectedEntityMachineThreat()
{
  return m_volumeDisplay->isCurrentSelectedEntityMachineThreat();
}

/*!
* @fn       getCurrentVRViewmode()
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets current VR view mode(threat, surface or laptop)
*/
VREnums::VRViewModeEnum VolumeDisplayManager::getCurrentVRViewmode() const
{
  return m_volumeDisplay->getCurrentVRViewmode();
}

/*!
* @fn       getCurrentSlabViewType()
* @param    None
* @return   VREnums::SlabViewType
* @brief    gets current slab view type
*/
VREnums::SlabViewType VolumeDisplayManager::getCurrentSlabViewType() const
{
  return m_volumeDisplay->getCurrentSlabViewType();
}

/**
* @fn       addKeystroke()
* @param    keystroke String definition of the keystroke: 2 Character code
* @param    alarm_type If pressed while viewing a threat, the type of threat. Otherwise, blank.
* @brief    recorded keystroke for a bag.
*/
void VolumeDisplayManager::addKeystroke(QString keystroke, QString alarm_type)
{
  if(m_volumeDisplay)
  {
    if(alarm_type == "")
        {
          QString categoryDesc  = "";
          boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->getBagbyState(BagData::RENDERING);
          if(bagData == NULL)
          {
            DEBUG_LOG("Bag data is NULL.");
            alarm_type = categoryDesc;
          }
          else
          {

            int currentThreat = bagData->getCurrrentThreat();

            SDICOS::TDRModule* currenttdrModule =  bagData->getCurrenttdrModule();
            if (currenttdrModule == NULL)
            {
              ERROR_LOG("Error in setting next threat. TdrModule is NULL.");
              alarm_type = categoryDesc;
            }
            else
            {
              if(currentThreat == -1)
              {
                alarm_type = categoryDesc;
              }
              else
              {
                SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>&
                    threat_item_array = currenttdrModule->GetThreatSequence().GetPotentialThreatObject();

                SDICOS::TDRTypes::AssessmentSequence& assessment_sequence = threat_item_array[currentThreat].GetAssessment();

                SDICOS::TDRTypes::ThreatSequence::ThreatItem& thretItem = threat_item_array[currentThreat];

                analogic::workstation::ThreatAlarmType thType =
                    analogic::workstation::getThreatAlarmTypeFromTdrThreatObject(thretItem);

                categoryDesc    =  analogic::workstation::convertMapToString(thType);
                alarm_type = categoryDesc;

                DEBUG_LOG("The Alarmm type is" << categoryDesc.toStdString());
              }
            }
          }
        }
    m_volumeDisplay->addKeystroke(keystroke, alarm_type);
  }
}

/*!
* @fn       getBHSBagId()
* @param    None
* @return   string
* @brief    Get BHS bag id
*/
QString VolumeDisplayManager::getBHSBagId()
{
  if(m_volumeDisplay->getBHSBagId() != ""){
    std::string bhs = m_volumeDisplay->getBHSBagId();
    QString temp = bhs.c_str();
    return temp;
  }
  else
    return QString("");
}

/*!
* @fn       getSearchDefaultRightviewModeType
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets default mode type
*/
VREnums::VRViewModeEnum VolumeDisplayManager::getSearchDefaultRightviewModeType()
{
  return m_volumeDisplay->getSearchDefaultRightviewModeType();
}

/*!
* @fn       getRecallDefaultRightviewModeType
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets default mode type
*/
VREnums::VRViewModeEnum VolumeDisplayManager::getRecallDefaultRightviewModeType()
{
  return m_volumeDisplay->getRecallDefaultRightviewModeType();
}

/**
 * @brief isTIPBag
 * @return bool
 * @brief Identify if its current bag is TIP bag
 */
bool VolumeDisplayManager::isTIPBag()
{
  return m_volumeDisplay->isTIPBag();
}

/*!
* @fn       getCheckpointMinimumSlices
* @param    None
* @return   int
* @brief    get number of minimum slices required for checkpoint scanner
*/
int VolumeDisplayManager::getCheckpointMinimumSlices()
{
  return CHECKPOINT_MIN_NUMBER_SLICES;
}

/*!
* @fn       setEnableDistanceMeasurement
* @param    bool
* @return   None
* @brief    set distance Measurement mode.
*/
void VolumeDisplayManager::setEnableDistanceMeasurement(bool setVal)
{
  m_volumeDisplay->setEnableDistanceMeasurement(setVal);
}

/*!
* @fn       measureVolumeByValue
* @param    bool
* @return   None
* @brief    Display volume estimation of picked object.
*/
void VolumeDisplayManager::measureVolumeByValue(bool setVal)
{
  m_volumeDisplay->measureVolumeByValue(setVal);
}

/*!
* @fn       setMaterialFilterCutoff
* @param    double
* @return   None
* @brief    Sets matarial cutoff value.
*/
void VolumeDisplayManager::setMaterialFilterCutoff(double material_cutoff)
{
  m_volumeDisplay->setMaterialFilterCutoff(material_cutoff);
}

/*!
   * @fn       isSlabViewThreatModeEnable
   * @param    None
   * @return   bool
   * @brief    return Slab View Threat mode enable/disable status.
   */
bool VolumeDisplayManager::isSlabViewThreatModeEnable()
{
  if(m_volumeDisplay)
  {
    return m_volumeDisplay->isSlabViewThreatModeEnable();
  }
}

/*!
* @fn       isDetectionException
* @param    None
* @return   bool
* @brief    tells if bag has detection exception.
*/
bool VolumeDisplayManager::isDetectionException()
{
  return m_volumeDisplay->isDetectionException();
}

}  // namespace ws
}  // namespace analogic
