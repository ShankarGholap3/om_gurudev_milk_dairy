/*!
* @file     volumedisplay.cpp
* @author   Agiliad
* @brief    Class for displaying volume.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#include <analogic/ws/uihandler/volumedisplay.h>
#include <analogic/nss/agent/search/SearchBag.h>
namespace analogic
{
namespace ws
{
/*!
* @fn       VolumeDisplay()
* @param    None
* @return   None
* @brief    Constructor.
*/
VolumeDisplay::VolumeDisplay(View* parent):
  QObject(parent),
  m_viewRef(parent),
  m_vrwLeftInitialized(false),
  m_vrwRightInitialized(false),
  m_rendered(false),
  m_renderingInProgress(false),
  m_front(false),
  m_uiOperationInProgress(false),
  m_hasABag(false),
  m_qmlContainerRef(nullptr),
  m_uiHelper(nullptr),
  m_fbagData(nullptr),
  m_vrwLeft(nullptr),
  m_vrwRight(nullptr),
  m_vrcLeft(nullptr),
  m_vrcRight(nullptr)
{
  DEBUG_LOG("Creating instance of VolumeDisplayUIHelper");
  m_uiHelper = new VolumeDisplayUIHelper();
  m_viewID = "";
}

/*!
* @fn       VolumeDisplay()
* @param    None
* @return   None
* @brief    Constructor.
*/
VolumeDisplay::VolumeDisplay(View* parent,
                             VolumeDisplayUIHelper* ptr):
  QObject(parent),
  m_viewRef(parent),
  m_vrwLeftInitialized(false),
  m_vrwRightInitialized(false),
  m_rendered(false),
  m_renderingInProgress(false),
  m_front(false),
  m_uiOperationInProgress(false),
  m_hasABag(false),
  m_qmlContainerRef(nullptr),
  m_uiHelper(nullptr),
  m_fbagData(nullptr),
  m_vrwLeft(nullptr),
  m_vrwRight(nullptr),
  m_vrcLeft(nullptr),
  m_vrcRight(nullptr)
{
  DEBUG_LOG("Creating instance of VolumeDisplayUIHelper");
  m_uiHelper = ptr;
  m_viewID = "";
}

/*!
* @fn       ~VolumeDisplay()
* @param    None
* @return   None
* @brief    Destructor.
*/
VolumeDisplay::~VolumeDisplay()
{
  TRACE_LOG("");

  m_qmlContainerRef = NULL;
  m_viewRef = NULL;

  m_fbagData.reset();
  SAFE_DELETE( m_vrwLeft );
  SAFE_DELETE( m_vrwRight );
  SAFE_DELETE( m_vrcLeft );
  SAFE_DELETE( m_vrcRight );
  SAFE_DELETE( m_uiHelper );
}

/*!
* @fn       initialize()
* @param    QQuickView *qmlContainer
* @param    QWidget *topWidget
* @param    QThread* m_backgroundThread
* @param    QMutex* renderMutex
* @return   None
* @brief    Create the volume rendering widgets.
*/
void VolumeDisplay::initialize(QQuickView *qmlContainer, QWidget *topWidget, QMutex* renderMutex)
{
  TRACE_LOG("");
  m_qmlContainerRef = qmlContainer;

  m_volumerenderTopWidget = topWidget;
  THROW_IF_FAILED((m_volumerenderTopWidget == NULL)? Errors::E_OUTOFMEMORY : Errors::S_OK);
  m_volumerenderTopWidget->hide();

  m_renderviewLayout = new QGridLayout(m_volumerenderTopWidget);
  THROW_IF_FAILED((m_renderviewLayout == NULL)? Errors::E_OUTOFMEMORY : Errors::S_OK);
  DEBUG_LOG("Created Grid layout.");

  m_renderviewLayout->setMargin(VTK_LAYOUT_MARGIN);
  m_renderviewLayout->setSpacing(VTK_LAYOUT_SPACING);
  m_volumerenderTopWidget->setLayout(m_renderviewLayout);
  m_volumerenderTopWidget->setMaximumHeight(VTK_WIDGET_HEIGHT);


  connect(this, SIGNAL(renderVolumeBackgroundThreadSignal()),
          this, SLOT(renderVolumeBackgroundThread()),
          Qt::QueuedConnection);

  connect(this, SIGNAL(uiCommandBackgroundThreadSignal(QMLEnums::ThreatEvalPanelCommand)),
          this, SLOT(uiCommandBackgroundThread(QMLEnums::ThreatEvalPanelCommand)),
          Qt::QueuedConnection);

  initializeVRW(m_renderviewLayout, m_vrcLeft, m_vrwLeft, false);
  initializeVRW(m_renderviewLayout, m_vrcRight, m_vrwRight, true);

  std::list<analogic::workstation::VolumeRendererWidget*> sibRight;
  std::list<analogic::workstation::VolumeRendererWidget*> sibLeft;
  sibRight.push_back(m_vrwLeft);
  sibLeft.push_back(m_vrwRight);

  m_vrwLeft->SetLinkSiblingsList(sibLeft);
  m_vrwRight->SetLinkSiblingsList(sibRight);

  // If and when there is a requirement for a GUI setting binary switch in the GUI,
  // use that value here instead of permanently setting to true.
  m_vrwLeft->SetSyncEnabled(true);
  m_vrwRight->SetSyncEnabled(true);

  // Using the same mutex for all 4 widgets: left/right of front/back.
  m_vrwLeft->setMutexForRendering(renderMutex);
  m_vrwRight->setMutexForRendering(renderMutex);

  DEBUG_LOG("Initialize VolumeDisplayUIHelper");
  m_uiHelper->initialize(m_qmlContainerRef, topWidget, this, m_vrwLeft, m_vrwRight);
}

/*!
* @fn       initializeVRW
* @param    analogic::workstation::VolumeRendererConfig*& vrc
* @param    analogic::workstation::VolumeRendererWidget*& vrw
* @param    bool right
* @return   None
* @brief    initialize members of class View.
*/
void VolumeDisplay::initializeVRW(QGridLayout*& vtkLayout,
                                  analogic::workstation::VolumeRendererConfig*& vrc,
                                  analogic::workstation::VolumeRendererWidget*& vrw,
                                  bool right)
{
  TRACE_LOG("");
  std::string settingsDir = WorkstationConfig::getInstance()->getexecutablePath() + "/../resources/volume_renderer/";
  std::vector<std::string> fileOpacityList;
  std::vector<std::string> fileHSVList;
  if ( right )
  {
    // More files will be added to these arrays in the future.
    fileOpacityList  = {"SurfaceOpacity.opmap",
                        "LowDensitySurfaceOpacity.opmap",
                        "Threat_A_SurfaceOpacity.opmap",
                        "Threat_B_SurfaceOpacity.opmap",
                        "Threat_C_SurfaceOpacity.opmap",
                       };

    fileHSVList      = {"SurfaceHSVColor.colmap",
                        "LowDensitySurfaceHSVColor.colmap",
                        "Threat_A_SurfaceHSVColor.colmap",
                        "Threat_B_SurfaceHSVColor.colmap",
                        "Threat_C_SurfaceHSVColor.colmap",
                       };
  }
  else
  {
    // More files will be added to these arrays in the future.
    fileOpacityList  = {"NormalOpacity.opmap",
                        "LowDensityNormalOpacity.opmap",
                        "Threat_A_NormalOpacity.opmap",
                        "Threat_B_NormalOpacity.opmap",
                        "Threat_C_NormalOpacity.opmap",
                       };

    fileHSVList      = {"NormalHSVColor.colmap",
                        "LowDensityNormalHSVColor.colmap",
                        "Threat_A_NormalHSVColor.colmap",
                        "Threat_B_NormalHSVColor.colmap",
                        "Threat_C_NormalHSVColor.colmap",
                       };
  }
  DEBUG_LOG("Settings dir is: " << settingsDir);
  DEBUG_LOG("File opacity is: " << fileOpacityList.at(0));
  DEBUG_LOG("File HSV is:     " << fileHSVList.at(0));

  bool useShading            = true;
  bool allowOrganicReomval   = true;
  bool allowInorganicremoval = true;
  bool allowMetalRemoval     = true;
  bool interpolateLinear     = false;
  bool showThreats           = true;
  bool showLabel             = true;
  bool showThreatWidget      = true;
  bool mainAlignLeft         = true;

  double lightingAmbient  = 0;
  double lightingDiffuse  = 0;
  double lightingSpecular = 0;

  uint16_t volMeasureStartMin = WorkstationConfig::getInstance()->getVolumeToolStartMin();
  uint16_t volMeasureStartMax = WorkstationConfig::getInstance()->getVolumeToolStartMax();
  bool vtkSplashSharpen       = WorkstationConfig::getInstance()->getEnableVtkSharpenSplash();
  bool vtkDynamicSplash       = WorkstationConfig::getInstance()->getEnableVtkDynamicSharpenSplash();
  VREnums::DispalyUnitSystemEnum unitSystem = WorkstationConfig::getInstance()->getWorkstationDisplayUnitSystem();

  if(right)
  {
    useShading            = WorkstationConfig::getInstance()->getUseShadingforRightViewer();
    allowOrganicReomval   = WorkstationConfig::getInstance()->getOrganicRemovableForRightViewer();
    allowInorganicremoval = WorkstationConfig::getInstance()->getInorganicRemovableForRightViewer();
    allowMetalRemoval     = WorkstationConfig::getInstance()->getMetalRemovableForRightViewer();
    interpolateLinear     = WorkstationConfig::getInstance()->getSurfaceLinearInterpolation();
    showThreats           = ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)?
                               WorkstationConfig::getInstance()->showMachineAlarmDecision():
                               WorkstationConfig::getInstance()->getEnableThreathandling());
    lightingAmbient       = WorkstationConfig::getInstance()->getSurfaceAmbientLighting();
    lightingDiffuse       = WorkstationConfig::getInstance()->getSurfaceDiffuseLighting();
    lightingSpecular      = WorkstationConfig::getInstance()->getSurfaceSpecularLighting();
    showLabel             = false;
    showThreatWidget      = false;
    mainAlignLeft         = false;
    m_viewID              = "Right View";
  }
  else
  {
    useShading            = WorkstationConfig::getInstance()->getUseShadingforMainViewer();
    allowOrganicReomval   = WorkstationConfig::getInstance()->getOrganicRemovableForLeftViewer();
    allowInorganicremoval = WorkstationConfig::getInstance()->getInorganicRemovableForLeftViewer();
    allowMetalRemoval     = WorkstationConfig::getInstance()->getMetalRemovableForLeftViewer();
    interpolateLinear     = WorkstationConfig::getInstance()->getTranslucentLinearInterpolation();
    showThreats           = ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)?
                               WorkstationConfig::getInstance()->showMachineAlarmDecision():
                               WorkstationConfig::getInstance()->getEnableThreathandling());
    lightingAmbient       = WorkstationConfig::getInstance()->getTranslucentAmbientLighting();
    lightingDiffuse       = WorkstationConfig::getInstance()->getTranslucentDiffuseLighting();
    lightingSpecular      = WorkstationConfig::getInstance()->getTranslucentSpecularLighting();

    showLabel             =  true;
    showThreatWidget      =  true;
    mainAlignLeft         =  true;
    m_viewID              = "Left View";
  }

  vrc = new analogic::workstation::VolumeRendererConfig(settingsDir,
                                                        fileOpacityList,
                                                        fileHSVList,
                                                        settingsDir + "GrayscaleValuesAndOpacities.txt",
                                                        settingsDir + "MaterialTransition.txt",
                                                        settingsDir + "InternalVolumeSettings.txt",
                                                        showThreats,
                                                        useShading,
                                                        interpolateLinear,
                                                        allowOrganicReomval,
                                                        allowInorganicremoval,
                                                        allowMetalRemoval);

  bool bScanGoesFromLeftoRight = WorkstationConfig::getInstance()->getScannerDirectionLeftToRight();

  THROW_IF_FAILED((vrc == NULL)? Errors::E_OUTOFMEMORY : Errors::S_OK);
  DEBUG_LOG("Created render configuration handle.");
  QLabel* vrLabel = new QLabel("", nullptr);
  vrw = new analogic::workstation::VolumeRendererWidget(vrLabel);
  if (mainAlignLeft)
  {
    vtkLayout->addWidget(vrLabel, 0, 0, 0, 0, Qt::AlignLeft | Qt::AlignBottom );
  }
  else
  {
    vtkLayout->addWidget(vrLabel, 0, 0, 0, 1, Qt::AlignRight | Qt::AlignBottom );
  }


  if (vrw)
  {
    vrw->SetDisplayLabel(m_viewID);
    vrw->InitPickBoxWidget();
    vrw->ShowThreatView(showThreatWidget);
    vrw->ShowViewLabel(showLabel);
    vrw->SetEnableSplashScreen(vtkSplashSharpen);
    vrw->SetEnableDynamicSplash(vtkDynamicSplash);
    vrw->SetLightingParameters(lightingAmbient,
                               lightingDiffuse,
                               lightingSpecular);
    vrw->SetVTKDisplayUnitSystem(unitSystem);
    vrw->SetVoumeMeasurementStartRange(volMeasureStartMin,
                                       volMeasureStartMax);
  }

  if (right)
  {
    vrw->SetAsThreatRenderer(true);
  }
  vrw->SetScannerDirectionLeftToRight(bScanGoesFromLeftoRight);
  THROW_IF_FAILED((vrw == NULL)? Errors::E_OUTOFMEMORY : Errors::S_OK);
  DEBUG_LOG("Created render widget.");
  connect(vrw, SIGNAL(initComplete()), this, SLOT(vrwInitialized()));
  connect(vrw, SIGNAL(loadUpdate(int)), this, SLOT(vrwLoadPercent(int)));
  connect(vrw, SIGNAL(userThreatBoxDrawingCompleted()), this, SLOT(UserThreatBoxDrawn()));
  connect(vrw, SIGNAL(operatorThreatSelected(bool)), this, SLOT(OperatorThreatIsActive(bool)));
}

/*!
* @fn       vrwInitialized()
* @param    None
* @return   None
* @brief    Slot that executes on completion of volume rendering.
*/
void VolumeDisplay::vrwInitialized()
{
  widgetclearcount++;
  if(m_renderingInProgress && widgetclearcount >= MAX_WIDGET_COUNT)
  {
    DEBUG_LOG("Rendering done for both Widget.Notify background process is finished.");
    m_renderingInProgress = false;
    m_rendered = true;
    m_fbagData->refreshAttributes();
    emit backgroundProcessFinished();
    m_fbagData->setCurrentThreat(DEFAULT_VALUE_CURRENT_THREAT);
    m_fbagData->setCurrentVisibleEntityIndex(DEFAULT_VALUE_CURRENT_THREAT);
    if ( m_viewRef
         && ((m_viewRef->getCurrentView() == QMLEnums::BAGSCREENING_SCREEN)
             ||(m_viewRef->getCurrentView() == QMLEnums::TRAINING_BAG_SCREEN)
             ||(m_viewRef->getCurrentView() == QMLEnums::SEARCH_BAG_SCREEN)
             ))

    {
      m_fbagData->initializeOperatorTDR();
    }
    setNextThreat();

    //------------------------------------
    // Log bag ready for user interaction
    LOG(INFO) << "VolumeDisplay - VOLUME_VTK_RENDER_END";
    //-------------------------------------

    m_fbagData->setBagRenderedTime(QDateTime::currentDateTime());
    emit bagRenderedComplete();
    if (m_fbagData->isDetectionException() == true)
    {
      m_vrwLeft->NotifyDetectionException();
      m_vrwRight->NotifyDetectionException();
    }
    autoArchiveBag();
  }
}

/*!
* @fn       vrwLoadPercent()
* @param    int percent
* @return   None
* @brief    Slot that executes periodically and informs rendering completion percentage.
*/
void VolumeDisplay::vrwLoadPercent(int percent)
{
  DEBUG_LOG("Volume renderer Load percent :: " << percent);
}

/*!
* @fn       renderVolume()
* @param    boost::shared_ptr<BagData> bagData
* @return   None
* @brief    Render both left and right widgets with provided bag data.
*/
void VolumeDisplay::renderVolume(boost::shared_ptr<BagData> bagData)
{
  TRACE_LOG("");
  if(!bagData.get())
  {
    ERROR_LOG("No Bag data for rendering.");
    return;
  }

  m_vrwLeft->SetScannerDirectionLeftToRight(WorkstationConfig::getInstance()->getScannerDirectionLeftToRight());
  m_vrwRight->SetScannerDirectionLeftToRight(WorkstationConfig::getInstance()->getScannerDirectionLeftToRight());

  m_renderingInProgress = true;
  widgetclearcount = 0;
  m_hasABag = true;
  m_fbagData = bagData;

  //-----------------------
  // ARO-ADDED: 3-16-2018

  VREnums::DispalyUnitSystemEnum unitSystem = WorkstationConfig::getInstance()->getWorkstationDisplayUnitSystem();
  m_vrwLeft->SetVTKDisplayUnitSystem(unitSystem);
  m_vrwRight->SetVTKDisplayUnitSystem(unitSystem);

  //-----------------------
  if (bagData->isTIPBag() &&
      ((m_viewRef->getCurrentView() == QMLEnums::BAGSCREENING_SCREEN)
       || (m_viewRef->getCurrentView() == QMLEnums::TRAINING_BAG_SCREEN)
       ))
  {
    bagData->setTIPPhase(TIPPhase::TIP_INITIAL_DECISION_PHASE);
  }
  else
  {
    bagData->setTIPPhase(TIPPhase::TIP_NONE_PHASE);
  }
  DEBUG_LOG("Rendering bag data in widgets.");
  renderVolumeBackgroundThread();
}

/*!
* @fn       renderVolumeBackgroundThread()
* @param    None
* @return   None
* @brief    Render on background thread.
*/
void VolumeDisplay::renderVolumeBackgroundThread()
{
  UILayoutManager* ulm = UILayoutManager::getUILayoutManagerInstance();
  if(ulm)
  {
    QMLEnums::ScreenEnum screenstate = ulm->getScreenState();
    if((screenstate == QMLEnums::BAGSCREENING_SCREEN) ||
       (screenstate == QMLEnums::SEARCH_BAG_SCREEN) ||
       (screenstate == QMLEnums::RERUNSELECTION_BAG_SCREEN)
       || (screenstate == QMLEnums::TRAINING_BAG_SCREEN)
       )
    {
      if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)
      {
        m_vrcLeft->m_enableThreatHandling  =  WorkstationConfig::getInstance()->showMachineAlarmDecision();
        m_vrcRight->m_enableThreatHandling =  WorkstationConfig::getInstance()->showMachineAlarmDecision();
      }
      else
      {
        m_vrcLeft->m_enableThreatHandling  = WorkstationConfig::getInstance()->getEnableThreathandling();
        m_vrcRight->m_enableThreatHandling = WorkstationConfig::getInstance()->getEnableThreathandling();
      }
    }
    else
    {
      m_vrcLeft->m_enableThreatHandling = true;
      m_vrcRight->m_enableThreatHandling = true;
    }
  }
  DEBUG_LOG("In background thread rendering bag data in left widgets.");
  renderVolumeSingle(m_vrcLeft, m_vrwLeft, m_vrwLeftInitialized, m_fbagData);
  DEBUG_LOG("In background thread rendering bag data in right widgets.");
  renderVolumeSingle(m_vrcRight, m_vrwRight, m_vrwRightInitialized, m_fbagData);
  m_uiHelper->applyReload();
}

/*!
* @fn       renderVolumeSingle
* @param    analogic::workstation::VolumeRendererConfig* vrc
* @param    analogic::workstation::VolumeRendererWidget* vrw
* @param    bool& isVolumePresentInThisWidget
* @param    boost::shared_ptr<BagData>
* @return   None
* @brief    Render volume in the widget.
*/
void VolumeDisplay::renderVolumeSingle(analogic::workstation::VolumeRendererConfig* vrc,
                                       analogic::workstation::VolumeRendererWidget* vrw,
                                       bool& isVolumePresentInThisWidget,
                                       boost::shared_ptr<BagData> bagData)
{
  TRACE_LOG("");
  DEBUG_LOG("Bag Flow Event:: Rendering "<< bagData->getBagid());

  if(!isVolumePresentInThisWidget)
  {
    DEBUG_LOG("Volume is not rendered in the widget. So rendering it.");
    uint16_t* pBuffer = reinterpret_cast<uint16_t*> (const_cast<unsigned char*>(bagData->getvolumedata()));
    QVector3D dimention =  bagData->getVolumeDimension();
    QVector3D roistart  =  bagData->getvolumeStartROI();
    QVector3D roiend    =  bagData->getvolumeEndROI();

    std::vector<SDICOS::TDRModule*> tdrModules = bagData->getTdrModules();
    SDICOS::Array1D<SDICOS::CTModule> ctModules = bagData->getCtModules();

    if(ctModules.GetSize() <= 0)
    {
      ERROR_LOG("CTModule is not valid. Cannot render volume.");
      vrwInitialized();
    }
    else
    {
      if (bagData->getTIPPhase() == TIP_ANALYSIS_PHASE)
      {
        vrc->m_showTIP = true;
      }
      else
      {
        vrc->m_showTIP = false;
      }
      vrc->m_isTIPBag = bagData->isTIPBag();
      QString bufferAddress = "*MMMMMMM* Volume Buffer address: ";
      bufferAddress += QString::number((uint64_t)pBuffer);
      INFO_LOG(bufferAddress.toLocal8Bit().data());

      std::vector< std::pair<int, std::string> >  vecDisplayThreatIdentifier = bagData->identifyDisplayableThreat();
      vrw->Init(&ctModules[0],
          pBuffer,
          dimention,
          roistart,
          roiend,
          tdrModules,
          vecDisplayThreatIdentifier,
          vrc);
    }
#ifdef WORKSTATION
    QStringList list;
    QMLEnums::ArchiveFilterSrchLoc location = bagData->getBagReqLoc();
    INFO_LOG("Location::" << location);

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
          // ie. SEARCH_SERVER, REMOTE_SERVER, LOCAL_SERVER, PVS
          if(!(signID.empty()) && (signID.compare(bagData->getBagid()) != 0))
          {
            signID.erase(0, signID.find_first_not_of('0'));
            QString rfidStr = tr("RFID: ");
            rfidStr += signID.c_str();

            list.append(rfidStr);
            DEBUG_LOG(rfidStr.toStdString().c_str());
          }
        }
      }
    }

    if ( (location == QMLEnums::SEARCH_SERVER) ||
         (location == QMLEnums::BHS_SERVER) ||
         (location == QMLEnums::REMOTE_SERVER) ||
         (location == QMLEnums::LOCAL_SERVER))
    {
      std::string searchReason = "";
      std::string diverterReason = "";
      analogic::nss::NssBag* nssBag = static_cast<analogic::nss::NssBag*>(bagData->getLivebag());
      if (nssBag)
      {
        int count = nssBag->get_extended_property_count();
        for (int c = 0; c < count; c++)
        {
          const analogic::nss::Property& extProp  = nssBag->get_extended_property(c);
          if (extProp.key.compare("reason_for_search") == 0)
          {
            DEBUG_LOG("Reason for Search:" << extProp.value);
            searchReason += extProp.value;
          }
          else if(extProp.key.compare("bhs_diverter_decision_string") == 0)
          {
            DEBUG_LOG("Diverter Reason :" << extProp.value);
            diverterReason += extProp.value;
          }
        }
      }
      else
      {
        DEBUG_LOG("Failed reinterpret cast");
        searchReason.append(bagData->getSearchReason());
      }
      if(searchReason.length() > 0)
      {
        searchReason = tr("Search Reason: ").toStdString() + searchReason;
        list.append(searchReason.c_str());
      }
      if (bagData && (bagData->getUserDecision().length() > 0))
      {
        QString userDecision = tr("User Decision: ");
        userDecision += bagData->getUserDecision();
        list.append(userDecision);
      }
      if(diverterReason.length() > 0)
      {
        diverterReason = tr("Diverter Reason: ").toStdString() + diverterReason;
        list.append(diverterReason.c_str());
      }
      DEBUG_LOG("BHS Server - "<< searchReason);
    }
    else if ((location == QMLEnums::PRIMARY_SERVER) &&
             (bagData->getBagTypeInfo().length() > 0))
    {
      QString baginfotype = tr("InfoType: ");
      baginfotype.append(bagData->getBagTypeInfo());
      list.append(baginfotype);
    }
    vrw->AppendDisplayText(list);

#endif
    isVolumePresentInThisWidget = true;
  }
  else
  {
    DEBUG_LOG("Volume is already rendered in the widget. So notify its status.");
    vrwInitialized();
  }
}

/*!
* @fn       setGeometry()
* @param    QPointF& topLeftCorner
* @param    double totalWidth
* @param    double totalHeight
* @return   None
* @brief    Set the position and size of container widgets.
*/
void VolumeDisplay::setGeometry(QPointF& topLeftCorner, double totalWidth, double totalHeight)
{
  m_volumerenderTopWidget->setGeometry(QRect(topLeftCorner.toPoint(), QSize(totalWidth, totalHeight)));
  DEBUG_LOG("Render layout geometry"
            << topLeftCorner.toPoint().x()
            << topLeftCorner.toPoint().y()
            << totalWidth
            << totalHeight);
}

/*!
* @fn       isRendered()
* @param    None
* @return   bool
* @brief    Tell if this volume is rendered.
*/
bool VolumeDisplay::isRendered()
{
  TRACE_LOG("");
  return m_rendered;
}

/*!
* @fn       hasABag()
* @param    None
* @return   bool
* @brief    Tell if this widget has taken a bag, and it's not yet disposed.
*/
bool VolumeDisplay::hasABag()
{
  TRACE_LOG("");
  return m_hasABag;
}

/*!
* @fn       show()
* @param    None
* @return   bool - Operation successful
* @brief    Make widgets visible.
*/
bool VolumeDisplay::show()
{
  TRACE_LOG("");
  bool retval = false;

  if(isRendered() && m_front)
  {
    DEBUG_LOG("Bag Flow Event:: Showing Bag "<< m_fbagData->getBagid());
    m_volumerenderTopWidget->show();
    retval = true;
  }
  DEBUG_LOG("Update UI controls visibility");
  updateUIControlsVisibility();
  return retval;
}

/*!
* @fn       hide()
* @param    None
* @return   None
* @brief    Set visibility of the widgets to false.
*/
void VolumeDisplay::hide()
{
  m_volumerenderTopWidget->hide();
  DEBUG_LOG("Update UI controls visibility");
  updateUIControlsVisibility();
}

/*!
* @fn       isVisible()
* @param    None
* @return   bool
* @brief    Tell if this volume is currently visible.
*/
bool VolumeDisplay::isVisible()
{
  if(!m_volumerenderTopWidget || !m_volumerenderTopWidget->isVisible())
  {
    DEBUG_LOG("Volume is currently not visible.");
    return false;
  }
  return true;
}

/*!
* @fn       clearWidgets()
* @param    None
* @return   None
* @brief    Clear the volume and hide widgets.
*/
void VolumeDisplay::clearWidgets()
{
  if(!isRendered())
  {
    DEBUG_LOG("Volume is not rendered.");
    return;
  }
  DEBUG_LOG("Volume is rendered. So remove bag from left and right volume remder widget.");
  m_vrwLeft->RemoveCurrentBag();
  m_vrwRight->RemoveCurrentBag();

  DEBUG_LOG("Reset flags to false.");
  m_vrwLeftInitialized = false;
  m_vrwRightInitialized = false;
  widgetclearcount = 0;
  m_rendered = false;
  m_renderingInProgress = false;
  DEBUG_LOG("Set visibility of widgets to false.");
  hide();
  m_hasABag = false;
}

/*!
* @fn       uiCommand
* @param    QMLEnums::ThreatEvalPanelCommand command
* @return   None
* @brief    Responsible for handling user events.
*/
void VolumeDisplay::uiCommand(QMLEnums::ThreatEvalPanelCommand command)
{
  if(m_uiOperationInProgress)
  {
    DEBUG_LOG("UI operation in progress.");
    return;
  }
  DEBUG_LOG("Set UI operation flag to in progress and update UI control visiblity.");
  m_uiOperationInProgress = true;
  updateUIControlsVisibility();
  if ((command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_EXPLOSIVES) ||
      (command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_ANOMALY) ||
      (command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_WEAPON) ||
      (command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_LIQUID) ||
      (command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_CONTRABAND) ||
      (command == QMLEnums::ThreatEvalPanelCommand::TEPC_NOTIFY_READY_FOR_PUSH_TRANSFER))
  {
    uiCommandBackgroundThread(command);
  }

  else
  {
    emit uiCommandBackgroundThreadSignal(command);
  }
}

/*!
* @fn       uiCommandBackgroundThread()
* @param    QMLEnums::ThreatEvalPanelCommand command
* @return   None
* @brief    Process a UI command on background thread.
*/
void VolumeDisplay::uiCommandBackgroundThread(QMLEnums::ThreatEvalPanelCommand command)
{
  // Process UI commands that need bag data, or any other commands that
  // are not simply volume image operations, over here, and pass on rest of
  // the commands to uiHelper.
  DEBUG_LOG("Process UI  command in background: "<< command);
  if (command == QMLEnums::ThreatEvalPanelCommand::TEPC_EXPORT)
  {
    DEBUG_LOG("Process Command to export to drive.");
    QString dest =  WorkstationConfig::getInstance()->getExportBagDataPath() + "/"
        + m_fbagData->getBagFileName().c_str() + BAGFILE_VOLUME_FORMAT;
    QString result;
    int hr = Utility::checkFileFolderPermission(dest, result);
    if(hr != Errors::FILE_OPERATION_SUCCESS)
    {
      UILayoutManager::getUILayoutManagerInstance()->displayMsg(hr, result, QMLEnums::BAGSCREENING_SCREEN);
    }
    else
    {
      applyExportToDrive();
    }
  }
  else if  (command == QMLEnums::ThreatEvalPanelCommand::TEPC_OVERWRITE)
  {
    applyExportToDrive();
  }
  else if (command == QMLEnums::ThreatEvalPanelCommand::TEPC_NEXT_THREAT)
  {
    setNextThreat();
  }
  else if ((command == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_THREAT) ||
           (command == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_THREAT))
  {
    applyDecisionOnThreat(command);
  }
  else if (command == QMLEnums::ThreatEvalPanelCommand::TEPC_NOTIFY_READY_FOR_PUSH_TRANSFER)
  {
    if(m_fbagData != NULL)
    {
      m_fbagData->notifyReadyforPushTransfer();
    }
  }
  else if ((m_uiHelper != NULL) &&
           ((command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_EXPLOSIVES) ||
            (command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_ANOMALY) ||
            (command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_WEAPON) ||
            (command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_LIQUID) ||
            (command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_CONTRABAND))
           )
  {
    int ret = m_uiHelper->pickboxAccept(command);
    if ((ret != -1) && (m_fbagData != NULL))
    {
      m_fbagData->setCurrentThreat(ret);
      m_fbagData->setCurrentVisibleEntityIndex(m_fbagData->getCurrentVisibleEntityCount()-1);

      SDICOS::TDRModule *tdrModule = m_fbagData->getOperatorTdrModule();
      if ((tdrModule != NULL) && (m_vrwLeft != NULL))
      {
        m_fbagData->setCurrenttdrModule(tdrModule);

        int visibleEntityIndex  = m_fbagData->getCurrentVisibleEntityIndex();;
        int totalVisibleEntityCount = m_fbagData->getCurrentVisibleEntityCount();
        m_vrwLeft->SetThreatVisibility(tdrModule,
                                       m_fbagData->getCurrrentThreat(),
                                       true,
                                       false,
                                       visibleEntityIndex,
                                       totalVisibleEntityCount);

        m_vrwLeft->RepaintViewers();
      }
    }
  }
  else if(command ==QMLEnums::TEPC_SHOW_ALL_THREAT_TOGGLE)
  {
    m_fbagData->toggleShowAllThreat();
    updateLeftRightViewerForRecallBag();
  }
  else
  {
    DEBUG_LOG("Processing UI command.");
    m_uiHelper->uiCommand(command);
  }

  m_uiOperationInProgress = false;
  DEBUG_LOG("Setting UI operation in progress flag to false and finished background process.");
  emit backgroundProcessFinished();
}

/*!
* @fn       setNextThreat()
* @param    None
* @return   None
* @brief    Set Next threat to highlight.
*/
void VolumeDisplay::setNextThreat()
{
  TRACE_LOG("");
  if(m_fbagData != NULL)
  {
    bool res = m_fbagData->setNextThreat();
    if(res)
    {
      int currentThreat = m_fbagData->getCurrrentThreat();
      SDICOS::TDRModule* currenttdrModule =  m_fbagData->getCurrenttdrModule();
      if (currenttdrModule == NULL)
      {
        ERROR_LOG("Error in setting next threat. TdrModule is NULL.");
        return;
      }

      SDICOS::TDRTypes::ThreatDetectionReport& threat_report = currenttdrModule->GetThreatDetectionReport();
      SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();

      DEBUG_LOG("Current Threat number:" << currentThreat<< "Tdr Type:" << tdr_type);
      int visibleEntityIndex = m_fbagData->getCurrentVisibleEntityIndex();
      int totalVisibleEntity = m_fbagData->getCurrentVisibleEntityCount();

      DEBUG_LOG("visibleEntityIndex: " << visibleEntityIndex);
      DEBUG_LOG("totalVisibleEntity: " << totalVisibleEntity);

      if((getCurrentVRViewmode() == VREnums::VIEW_AS_THREAT) ||
         (getCurrentVRViewmode() == VREnums::VIEW_AS_SLAB))
      {
        QString decision;
        QMLEnums::ScreenEnum currentscreen  = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
        QString userDescStr = tr("Threat Decision: ");
        if((currentscreen == QMLEnums::BAGSCREENING_SCREEN)
           || (currentscreen == QMLEnums::TRAINING_BAG_SCREEN))
        {
          decision = QMLEnums::getThreatDescriptionFromMap(m_fbagData->getUserDecisionfromOperatorTdr(currentThreat));
        }
        else
        {
          decision = QMLEnums::getThreatDescriptionFromMap(m_fbagData->getUserDecisionfromPrimaryTdr(currentThreat));
        }
        //ANSS-1470
        if((currentscreen == QMLEnums::BAGSCREENING_SCREEN) ||
           (currentscreen == QMLEnums::TRAINING_BAG_SCREEN) ||
           (currentscreen == QMLEnums::RERUNSELECTION_BAG_SCREEN))
        {
          userDescStr = "";
        }
        else
        {
          if(decision.isEmpty())
          {
            userDescStr += THREAT_NO_DECISION_STRING;
          }
          else
          {
            userDescStr += decision;
          }
        }
        m_vrwRight->ShowThreatUserDecision(userDescStr);
      }
      m_vrwLeft->SetThreatVisibility(currenttdrModule,
                                     currentThreat,
                                     true,
                                     false,
                                     visibleEntityIndex,
                                     totalVisibleEntity);
      m_vrwLeft->RepaintViewers();
    }
  }
}

/*!
* @fn       applyDecisionOnThreat
* @param    QMLEnums::ThreatEvalPanelCommand - bag decision
* @brief    This function will populate bag decision on particular threats.
*/
void VolumeDisplay::applyDecisionOnThreat(QMLEnums::ThreatEvalPanelCommand decision)
{
  TRACE_LOG("");
  if(m_fbagData != nullptr)
  {
    //---------------------------------------------------------------
    // Below code intensionally promoted with comment
    if(decision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_THREAT)
    {
      m_vrwLeft->SuspectCurrentThreat(m_fbagData->getCurrrentThreat(),
                                      m_fbagData->getCurrentVisibleEntityCount());
    }
    else if(decision == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_THREAT)
    {
      m_vrwLeft->ClearCurrentThreat();
      m_fbagData->applyDecisionOnThreat();
    }
    //---------------------------------------------------------------
    m_fbagData->populateDecisionOnParticularThreat(decision);
    setNextThreat();
    m_vrwLeft->RepaintViewers();
    m_vrwRight->RepaintViewers();
    if ((m_fbagData->getTotalVisibleAlarmThreats() == 0)
        && m_uiHelper)
    {
      m_uiHelper->onViewChanged(VREnums::VIEW_AS_SURFACE);
      m_uiHelper->SetRightViewerDisplayMode();
    }
  }
}

/*!
* @fn       setBagDecisionTime
* @return   None
* @brief    sets Bag Decision Time.
*/
void VolumeDisplay::setBagDecisionTime()
{
  if(m_fbagData != nullptr)
  {
    m_fbagData->setBagDecisionTime();
  }
}

/*!
* @fn       getOperatorDecisiontime
* @param    None
* @return   float
* @brief    get time taken to take decision by operator for training simulator
*/
float VolumeDisplay::getOperatorDecisiontime()
{
  if(m_fbagData != nullptr)
  {
    return  m_fbagData->getOperatorDecisiontime();
  }
}

/*!
* @fn       autoArchiveBag()
* @param    None
* @return   None
* @brief    Archive the bag after set period as per user config.
*/
void VolumeDisplay::autoArchiveBag()
{
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  if((WorkstationConfig::getInstance()->getAutoArchiveEnabled() == true)
     && currentscreen == QMLEnums::BAGSCREENING_SCREEN)
  {
    applyExportToDrive();
  }
}



/*!
* @fn       canTakeSuspectDecisionOnBag
* @return   bool
* @brief    return true if decsion is taken on all threat
*/
bool VolumeDisplay::canTakeSuspectDecisionOnBag()
{
  if(m_fbagData != nullptr && m_rendered)
  {
    return m_fbagData->canTakeSuspectDecisionOnBag();
  }
  return false;
}

/*!
* @fn       canTakeClearDecisionOnBag
* @return   bool
* @brief    return true if any threat marked as suspect
*/
bool VolumeDisplay::canTakeClearDecisionOnBag()
{
  if(m_fbagData != nullptr && m_rendered)
  {
    return m_fbagData->canTakeClearDecisionOnBag();
  }
  return true;
}

/*!
* @fn       applyForward()
* @param    None
* @return   None
* @brief    Apply user press of forward button to 3D image.
*/
void VolumeDisplay::applyForward()
{
  TRACE_LOG("");
  clearWidgets();
}

/*!
* @fn       setFront()
* @param    bool in
* @return   None
* @brief    Set value of front widget indicator.
*/
void VolumeDisplay::setFront(bool in)
{
  DEBUG_LOG("Setting front widget indicator to : " << in);
  m_front = in;
}

/*!
* @fn       setArchive()
* @param    bool in
* @return   None
* @brief    Set value of archive widget indicator.
*/
void VolumeDisplay::setArchive(bool in)
{
  DEBUG_LOG("Setting archive widget indicator to : " << in);
  m_isArchive = in;
}

/*!
* @fn       isFront()
* @param    None
* @return   bool - Is this the front widget.
* @brief    Tell if this is the front widget.
*/
bool VolumeDisplay::isFront()
{
  TRACE_LOG("");
  return m_front;
}

/*!
* @fn       applyDecision
* @param    QMLEnums::ThreatEvalPanelCommand
* @return   None
* @brief    Apply user decision
*/
void VolumeDisplay::applyDecision(QMLEnums::ThreatEvalPanelCommand decision)
{
  if(!isVisible() || !isRendered())
  {
    DEBUG_LOG("Bag data is i.e not rendered or not visible.");
    return;
  }

  m_fbagData->applyDecision(decision);

  DEBUG_LOG("Bag is rendered and visible.");
  clearWidgets();

  DEBUG_LOG("Setting hasAbag flag to false.");
  m_hasABag = false;

  DEBUG_LOG("Bag Flow Event:: Descision Executed for bag: "<< m_fbagData->getBagid());
}

/*!
* @fn       clearBag()
* @param    None
* @return   None
* @brief    Clears the current viewed bag.
*/
void VolumeDisplay::clearBag()
{
  if(!isVisible() || !isRendered())
  {
    DEBUG_LOG("Bag data is i.e not rendered or not visible.");
    return;
  }
  DEBUG_LOG("Bag is rendered and visible.");
  clearWidgets();

  m_fbagData->setState(BagData::DISPOSED);

  DEBUG_LOG("Setting hasAbag flag to false.");
  m_hasABag = false;

  DEBUG_LOG("Bag Flow Event:: Descision Executed for bag: "<< m_fbagData->getBagid());
}

/*!
* @fn       applyExportToDrive()
* @param    None
* @return   None
* @brief    Handle user press of export button.
*/
void VolumeDisplay::applyExportToDrive()
{
  TRACE_LOG("");
  if(!isRendered())
  {
    DEBUG_LOG("Bag data is i.e not rendered or not visible.");
    return;
  }

  QString defaultDataPath = WorkstationConfig::getInstance()->getExportBagDataPath();
  QDir dir(defaultDataPath);
  if(dir.exists())
  {
    if(m_fbagData != NULL && m_viewRef != NULL)
    {
      DEBUG_LOG("Request to export bag: "<<  m_fbagData->getBagid());
      QVariant bagData;
      bagData.setValue(m_fbagData.get());
      emit m_viewRef->bagListCommand(QMLEnums::ArchiveFilterSrchLoc::LOCAL_SERVER,
                                     QMLEnums::BagsListAndBagDataCommands::BLBDC_ARCHIVE_BAG_TO_LOCAL_ARCHIVE,
                                     bagData);
    }
  }
  else
  {
    emit UILayoutManager::getUILayoutManagerInstance()->displayMsg(Errors::FILE_OPERATION_SUCCESS,
                                                                   tr("Directory does not Exists."),
                                                                   QMLEnums::BAGSCREENING_SCREEN);
  }
}


/*!
* @fn       updateUIControlsVisibility()
* @param    None
* @return   None
* @brief    Enable or disable UI controls applicable to the displayed volume.
*/
void VolumeDisplay::updateUIControlsVisibility()
{
  if(!m_front)
  {
    DEBUG_LOG("Not front widget.");
    return;
  }

  bool enable = true;

  if(!isRendered() || !isVisible() || m_uiOperationInProgress)
  {
    enable = false;
  }
  DEBUG_LOG("Qml Component enable status should be set to : " << enable);
  DEBUG_LOG("Request qml update of ScreeningPanelEnability.");
  emit updateScreeningPanelEnability(enable);
}

/*!
 * @fn       updateLeftRightViewerForRecallBag
 * @param    bool
 * @return   None
 * @brief    updates left and right bag viewer according to show all threat status.
 */
void VolumeDisplay::updateLeftRightViewerForRecallBag(bool setDefaultRecallVrMode)
{    
  if((getCurrentVRViewmode() == VREnums::VIEW_AS_THREAT) ||
     (getCurrentVRViewmode() == VREnums::VIEW_AS_SLAB))
  {
    QString decision;
    int currentThreat = m_fbagData->getCurrrentThreat();
    QString userDescStr = tr("Threat Decision: ");

    decision = QMLEnums::getThreatDescriptionFromMap(m_fbagData->getUserDecisionfromPrimaryTdr(currentThreat));

    QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
    if((currentscreen == QMLEnums::BAGSCREENING_SCREEN) ||
       (currentscreen == QMLEnums::TRAINING_BAG_SCREEN) ||
       (currentscreen == QMLEnums::RERUNSELECTION_BAG_SCREEN))
    {
      userDescStr = "";
    }
    else
    {
      if(decision.isEmpty())
      {
        userDescStr += THREAT_NO_DECISION_STRING;
      }
      else
      {
        userDescStr += decision;
      }
    }
    m_vrwRight->ShowThreatUserDecision(userDescStr);
  }

  m_vrwLeftInitialized = false;
  m_vrwLeft->RemoveCurrentBag();
  renderVolumeSingle(m_vrcLeft, m_vrwLeft, m_vrwLeftInitialized, m_fbagData);
  m_uiHelper->applyReload();

  if(!setDefaultRecallVrMode)
  {
    if(m_fbagData->getCurrentVRViewmode() != VREnums::VIEW_AS_THREAT)
    {
      if(m_fbagData->getTotalVisibleAlarmThreats() > 0)
      {
        m_uiHelper->onViewChanged(VREnums::VIEW_AS_THREAT);
        m_uiHelper->SetRightViewerDisplayMode();
      }
    }
    else
    {
      if(m_fbagData->getTotalVisibleAlarmThreats() == 0)
      {
        m_uiHelper->onViewChanged(VREnums::VIEW_AS_SURFACE);
        m_uiHelper->SetRightViewerDisplayMode();
      }
    }
  }
}

/*!
   * @fn       isSlabViewThreatModeEnable
   * @param    None
   * @return   bool
   * @brief    return Slab View Threat mode enable/disable status.
   */
bool VolumeDisplay::isSlabViewThreatModeEnable()
{
  if(m_fbagData)
  {
    return m_fbagData->isSlabViewThreatModeEnable();
  }
}

/*!
* @fn       getViewID()
* @param    None
* @return   QString - View ID
* @brief    Tell view ID
*/
QString VolumeDisplay::getViewID()
{
  return m_viewID;
}

/*!
* @fn       UserThreatBoxDrawn()
* @param    None
* @return   None
* @brief    Slot function that gets called when user threat box is drawn on screen
*/
void VolumeDisplay::UserThreatBoxDrawn()
{
  // notify VolumeDisplaymanager about same
  // emit operatorThreatBoxDrawn();
  UILayoutManager* ulm = UILayoutManager::getUILayoutManagerInstance();
  if(ulm) {
    emit ulm->pickboxDrawn();
  }
}

/*!
* @fn       updatePickboxButtonsForTimeout()
* @param    None
* @return   None
* @brief    Function to update Pickbox UI when user timeout takes place
*/
void VolumeDisplay::updatePickboxButtonsForTimeout()
{
  //    if ((m_uiHelper != NULL) || (m_viewRef != NULL))
  //    {
  //        m_uiHelper->uiCommand(QMLEnums::TEPC_PICKBOX_ACCEPT_ANOMALY);
  //        m_viewRef->showHideOperatorThreatOptions(false);
  //    }
}

/*!
* @fn       getDisplayAlarmThreat
* @param    None
* @return   bool
* @brief    get display alarm threat
*/
bool VolumeDisplay::getDisplayAlarmThreat() const
{
  bool res = false;
  if (m_uiHelper)
  {
    res = m_uiHelper->getDisplayAlarmThreat();
  }
  return res;
}

/*!
* @fn       getEnableThreatColorization()
* @param    None
* @return   bool
* @brief    get threat colorization value
*/
bool VolumeDisplay::getEnableThreatColorization() const
{
  bool res = false;
  if (m_uiHelper)
  {
    res = m_uiHelper->getEnableThreatColorization();
  }
  return res;
}

/*!
* @fn       OperatorThreatIsActive()
* @param    None
* @return   None
* @brief    Update UI for current threat type as operator threat
*/
void VolumeDisplay::OperatorThreatIsActive(bool flag)
{
  // notify VolumeDisplaymanager about same
  UILayoutManager* ulm = UILayoutManager::getUILayoutManagerInstance();
  if(ulm) {
    emit ulm->operatorThreatSelected(flag);
  }
}

/**
* @fn       addKeystroke()
* @param    keystroke String definition of the keystroke: 2 Character code
* @param    alarm_type If pressed while viewing a threat, the type of threat. Otherwise, blank.
* @brief    recorded keystroke for a bag.
*/
void VolumeDisplay::addKeystroke(QString keystroke, QString alarm_type)
{
  if(m_fbagData)
  {
    m_fbagData->addKeystroke(keystroke, alarm_type);
  }
}

/*!
* @fn       isViewModeEnabled
* @return   bool
* @brief    return if view mode is enabled
*/
bool VolumeDisplay::isViewModeEnabled()
{
  bool res = false;
  if (m_fbagData)
  {
    res = m_fbagData->isViewModeEnabled();
  }
  return res;
}

/*!
* @fn       isVRModeVisible
* @return   bool
* @brief    return if vrmode argument needed visible on UI
*/
bool VolumeDisplay::isVRModeVisible(VREnums::VRViewModeEnum vrmode)
{
  bool ret = false;
  if (m_fbagData)
  {
    ret = m_fbagData->isVRModeVisible(vrmode);
  }
  return ret;
}


/*!
* @fn       isNextThreatAvailable
* @return   bool
* @brief    return if next threat is available
*/
bool VolumeDisplay::isNextThreatAvailable()
{
  bool ret = false;
  if (m_fbagData)
  {
    ret = m_fbagData->isNextThreatAvailable();
  }
  return ret;
}

/*!
* @fn       getSliceCountReceived
* @return   int
* @brief    return number of slice count received
*/
int VolumeDisplay::getSliceCountReceived()
{
  int ret = 0;
  if (m_fbagData && m_rendered)
  {
    ret = m_fbagData->getSliceCountReceived();
  }
  return ret;
}

/*!
* @fn       canApplyClearDecisionOnCurrentThreat
* @return   bool
* @brief    return if clear decision applied on current threat or not
*/
bool VolumeDisplay::canApplyClearDecisionOnCurrentThreat()
{
  bool ret = false;
  if (m_fbagData && m_rendered)
  {
    ret = m_fbagData->canApplyClearDecisionOnCurrentThreat();
  }
  return ret;
}


/*!
* @fn       canApplyDecisionOnCurrentThreat
* @return   bool
* @brief    return if decision applied on current threat or not
*/
bool VolumeDisplay::canApplyDecisionOnCurrentThreat()
{
  bool ret = false;
  if (m_fbagData && m_rendered)
  {
    ret = m_fbagData->canApplyDecisionOnCurrentThreat();
  }
  return ret;
}

/*!
* @fn       isCurrentSelectedEntityMachineThreat
* @return   bool
* @brief    return if selected entity is machine threat
*/
bool VolumeDisplay::isCurrentSelectedEntityMachineThreat()
{
  bool ret = false;
  if (m_fbagData)
  {
    ret = m_fbagData->isCurrentSelectedEntityMachineThreat();
  }
  return ret;
}

/*!
* @fn       getCurrentVRViewmode()
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets current VR view mode(threat, surface or laptop)
*/
VREnums::VRViewModeEnum VolumeDisplay::getCurrentVRViewmode() const
{
  VREnums::VRViewModeEnum ret = VREnums::VRViewModeEnum::VIEW_AS_SURFACE;
  if (m_fbagData)
  {
    ret = m_fbagData->getCurrentVRViewmode();
  }
  return ret;
}

/*!
* @fn       getCurrentSlabViewType()
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets current VR view mode(threat, surface or laptop)
*/
VREnums::SlabViewType VolumeDisplay::getCurrentSlabViewType() const
{
  VREnums::SlabViewType ret = VREnums::SlabViewType::THREAT_VIEW;
  if (m_fbagData)
  {
    ret = m_fbagData->getCurrentSlabViewType();
  }
  return ret;
}

/*!
* @fn       gettingBHSBagId()
* @param    None
* @return   string
* @brief    Get BHS bag id
*/
std::string VolumeDisplay::getBHSBagId()
{
  boost::shared_ptr<BagData> bhsbag = BagsDataPool::getInstance()->getBagbyState(BagData::RENDERING);
  if(bhsbag != NULL)
    return bhsbag->getBHSBagId();
  else
    return std::string();
}

/*!
* @fn       getSearchDefaultRightviewModeType
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets default mode type
*/
VREnums::VRViewModeEnum VolumeDisplay::getSearchDefaultRightviewModeType()
{
  VREnums::VRViewModeEnum viewmode;
  if(m_fbagData->isBagError())
    viewmode = VREnums::VIEW_AS_SURFACE;
  else
    viewmode = WorkstationConfig::getInstance()->getSearchDefaultRightviewModeType();
  return viewmode;
}

/*!
* @fn       getRecallDefaultRightviewModeType
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets default mode type
*/
VREnums::VRViewModeEnum VolumeDisplay::getRecallDefaultRightviewModeType()
{
  VREnums::VRViewModeEnum viewmode;
  if(m_fbagData->isBagError())
    viewmode = VREnums::VIEW_AS_SURFACE;
  else
    viewmode = WorkstationConfig::getInstance()->getRecallDefaultRightviewModeType();
  return viewmode;
}

/*!
* @fn       MockMe()
* @param
* @return   bool flag
* @brief    Use this function only for mocking
*/

void VolumeDisplay::MockMe(analogic::workstation::VolumeRendererWidget* vLeft,
                           analogic::workstation::VolumeRendererWidget* vRight,
                           QMutex* renderMutex)
{
  TRACE_LOG("");
  SAFE_DELETE(m_vrwLeft);
  SAFE_DELETE(m_vrwRight);
  m_vrwLeft = vLeft;
  m_vrwRight = vRight;

  std::list<analogic::workstation::VolumeRendererWidget*> sibRight;
  std::list<analogic::workstation::VolumeRendererWidget*> sibLeft;
  sibRight.push_back(m_vrwLeft);
  sibLeft.push_back(m_vrwRight);

  m_vrwLeft->SetLinkSiblingsList(sibLeft);
  m_vrwRight->SetLinkSiblingsList(sibRight);

  // If and when there is a requirement for a GUI setting binary switch in the GUI,
  // use that value here instead of permanently setting to true.
  m_vrwLeft->SetSyncEnabled(true);
  m_vrwRight->SetSyncEnabled(true);

  // Using the same mutex for all 4 widgets: left/right of front/back.
  m_vrwLeft->setMutexForRendering(renderMutex);
  m_vrwRight->setMutexForRendering(renderMutex);

  DEBUG_LOG("Initialize VolumeDisplayUIHelper");

  m_uiHelper->initialize(m_qmlContainerRef,
                         m_volumerenderTopWidget,
                         this,
                         m_vrwLeft,
                         m_vrwRight);
}

/**
 * @brief isTIPBag
 * @return bool
 * @brief Identify if its current bag is TIP bag
 */
bool VolumeDisplay::isTIPBag()
{
  if(m_fbagData)
  {
    return m_fbagData->isTIPBag();
  }
  return false;
}

/*!
* @fn       validateOperatorDecisionForTIP
* @param    decision - suspect, clear, timeout
* @param    QMLEnums::TIPResult& tipResult
* @return   bool - success
* @brief    validate operator decision for TIP
*/
QString  VolumeDisplay::validateOperatorDecisionForTIP(QMLEnums::ThreatEvalPanelCommand userDecision,
                                                       QMLEnums::TIPResult& tipResult)
{
  if(m_fbagData)
  {
    return m_fbagData->validateOperatorDecisionForTIP(userDecision, tipResult);
  }
  return "";
}

/*!
* @fn       showTIPThreat
* @param    decision - suspect, clear, timeout
* @return   bool - success
* @brief    show TIP Bag
*/
bool VolumeDisplay::showTIPThreat()
{
  //------------------------------------------------
  // ARO - Added : 4-17-2019
  m_vrwLeft->SuspectBag();
  TRACE_LOG("");
  if(m_fbagData != NULL)
  {
    bool res = m_fbagData->updateTIPThreat();
    if(res)
    {
      int currentThreat = m_fbagData->getCurrrentThreat();
      SDICOS::TDRModule* currenttdrModule =  m_fbagData->getCurrenttdrModule();
      if (currenttdrModule == NULL)
      {
        ERROR_LOG("Error in setting next threat. TdrModule is NULL.");
        return false;
      }

      SDICOS::TDRTypes::ThreatDetectionReport& threat_report = currenttdrModule->GetThreatDetectionReport();
      SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();

      DEBUG_LOG("Current Threat number:" << currentThreat<< "Tdr Type:" << tdr_type);
      int visibleEntityIndex = m_fbagData->getCurrentVisibleEntityIndex();
      int totalVisibleEntity = m_fbagData->getCurrentVisibleEntityCount();
      m_vrwRight->SetRightViewerDisplayMode(VREnums::VIEW_AS_THREAT);
      m_vrwLeft->SetThreatVisibility(currenttdrModule,
                                     currentThreat,
                                     true,
                                     false,
                                     visibleEntityIndex,
                                     totalVisibleEntity);
    }
    m_vrwLeft->RepaintViewers();
  }
  return true;
}

/*!
* @fn       isDetectionException
* @return   bool
* @brief    tells if detection exception for bag
*/
bool VolumeDisplay::isDetectionException()
{
  if (m_fbagData != nullptr)
    return m_fbagData->isDetectionException();
  else
    return false;
}

/*!
* @fn       setEnableDistanceMeasurement
* @param    bool
* @return   None
* @brief    set distance Measurement mode.
*/
void VolumeDisplay::setEnableDistanceMeasurement(bool setVal)
{
  m_vrwLeft->SetEnableDistanceMeasurement(setVal);
  m_vrwRight->SetEnableDistanceMeasurement(setVal);
}

/*!
* @fn       measureVolumeByValue
* @param    bool
* @return   None
* @brief    Display volume estimation of picked object.
*/
void VolumeDisplay::measureVolumeByValue(bool setVal)
{
  m_vrwRight->MeasureVolumeByValue(setVal);
}

/*!
   * @fn       setMaterialFilterCutoff
   * @param    double
   * @return   None
   * @brief    Sets matarial cutoff value.
   */
void VolumeDisplay::setMaterialFilterCutoff(double material_cutoff)
{
  m_vrwLeft->SetMaterialFilterCutoff(material_cutoff);
}

/*!
   * @fn       setShowAllThreat
   * @param    bool
   * @return   None
   * @brief    Sets show all threat.
   */
void VolumeDisplay::setShowAllThreat(bool showAllThreat)
{
  m_fbagData->setShowAllThreat(showAllThreat);
}

/*!
   * @fn       getShowAllThreat
   * @param    None
   * @return   bool
   * @brief    returns show all threat.
   */
bool VolumeDisplay::getShowAllThreat() const
{
  return m_fbagData->getShowAllThreat();
}

/*!
     * @fn       getBagRenderedtime
     * @param    None
     * @return   QDateTime
     * @brief    get time when bag rendering start on viewer screen
     */
QDateTime VolumeDisplay::getBagRenderedtime()
{
  if(m_fbagData != nullptr)
  {
    return  m_fbagData->getBagRenderTimeOnTraining();//getBagReadytoRenderTime();
  }
}

}  // namespace ws
}  // namespace analogic
