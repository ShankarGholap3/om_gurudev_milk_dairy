/*!
* @file     volumedisplayuihelper.cpp
* @author   Agiliad
* @brief    Class for performing UI operations on the displayed volume.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/volumedisplayuihelper.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       VolumeDisplayUIHelper()
* @param    None
* @return   None
* @brief    Constructor.
*/
VolumeDisplayUIHelper::VolumeDisplayUIHelper():
  QObject(nullptr),
  m_inverse(DEFAULT_VALUE_INVERSE),
  m_currentColorMetal(DEFAULT_VALUE_COLOR_METAL),
  m_currentColorOrganic(DEFAULT_VALUE_COLOR_ORGANIC),
  m_currentColorInorganic(DEFAULT_VALUE_COLOR_INORGANIC),
  m_displayAlarmThreat(DEFAULT_VALUE_THREAT_REGION),
  m_enableThreatColorization(DEFAULT_VALUE_ENABLE_THREAT_COLORIZATION),
  m_currentOpacity(DEFAULT_VALUE_OPACITY),
  m_currentContrast(DEFAULT_VALUE_CONTRAST),
  m_currentCutPercentage(DEFAULT_VALUE_CUT_PERCENTAGE),
  m_currentZoomLevel(DEFAULT_VALUE_ZOOM),
  m_currentBrightness(DEFAULT_VALUE_BRIGHTNESS),
  m_parentWidgetRef(nullptr),
  m_printer(nullptr),
  m_qmlContainerRef(nullptr),
  m_vdRef(nullptr),
  m_vrwLeftRef(nullptr),
  m_vrwRightRef(nullptr),
  m_printDialog(nullptr),
  m_currentRenderWithColor(DEFAULT_VALUE_RENDER_WITH_COLOR),
  m_currentWithLowDensity(DEFAULT_VALUE_RENDER_WITH_LOW_DENSITY),
  m_axis(DEFAULT_VALUE_AXIS),
  m_slabaxis(DEFAULT_VALUE_AXIS),
  m_currentSlabbingPercentage(DEFAULT_VALUE_CUT_PERCENTAGE),
  m_slabtype(DEFAULT_VALUE_SLABTYPE)

{
  TRACE_LOG("");
}

/*!
* @fn       ~VolumeDisplayUIHelper()
* @param    None
* @return   None
* @brief    Destructor.
*/
VolumeDisplayUIHelper::~VolumeDisplayUIHelper()
{
  TRACE_LOG("");
  m_parentWidgetRef = NULL;
  SAFE_DELETE( m_printer);
  m_qmlContainerRef = NULL;
  m_vdRef = NULL;
  m_vrwLeftRef = NULL;
  m_vrwRightRef = NULL;
}

/*!
* @fn       initialize()
* @param    QQuickView *qmlContainer
* @param    QWidget *centralWidget
* @param    VolumeDisplay *vd
* @param    analogic::workstation::VolumeRendererWidget* left
* @param    analogic::workstation::VolumeRendererWidget* right
* @return   None
* @brief    Initialize UI helper. Provide required references.
*/
void VolumeDisplayUIHelper::initialize(QQuickView* qmlContainer,
                                       QWidget* vrTopWidget,
                                       VolumeDisplay* vd,
                                       analogic::workstation::VolumeRendererWidget* left,
                                       analogic::workstation::VolumeRendererWidget* right)
{
  TRACE_LOG("");
  m_parentWidgetRef = vrTopWidget;
  m_qmlContainerRef = qmlContainer;
  m_vdRef = vd;
  m_vrwLeftRef = left;
  m_vrwRightRef = right;
  connect(m_vrwRightRef, &analogic::workstation::VolumeRendererWidget::slabPositionChanged,
          this, &VolumeDisplayUIHelper::onSlabPositionChanged);
}

/*!
* @fn       uiCommand()
* @param    QMLEnums::ThreatEvalPanelCommand command
* @return   None
* @brief    Change image as per user inputs.
*/
void VolumeDisplayUIHelper::uiCommand(QMLEnums::ThreatEvalPanelCommand command)
{
  DEBUG_LOG("User Command: " << command);
  if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_ORGANIC ||
     command == QMLEnums::ThreatEvalPanelCommand::TEPC_INORGANIC ||
     command == QMLEnums::ThreatEvalPanelCommand::TEPC_METAL)
  {
    applyColorCheckboxOIM();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_CONTRAST)
  {
    applySliderContrast();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_BRIGHTNESS)
  {
    applySliderBrightness();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_OPACITY)
  {
    applySliderOpacity();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_ZOOMIN)
  {
    applyZoomIn();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_ZOOMOUT)
  {
    applyZoomOut();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_RELOAD)
  {
    QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
    if((currentscreen == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN) ||
       (currentscreen == QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN))
    {
      m_vdRef->setShowAllThreat(true);
      m_vdRef->updateLeftRightViewerForRecallBag(true);
      writeShowAllThreatStateChange();
    }
    else
    {
      applyReload();
    }
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_INVERTCOLORS)
  {
    applyPaintDrop();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_INVERSE)
  {
    applyImageInversion();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_REMOVELAPTOP)
  {
    applyFrontalViewButtonsAndBlankScreen();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRINT)
  {
    applyPrinter();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_DISPLAY_ALARM_THREAT_ONLY)
  {
    applyDisplayAlarmThreatonly();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_CUTVIEW_PERCENTAGE_CHANGED)
  {
    applyCutViewPercentageChange();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_CUTVIEW_AXIS_CHANGED)
  {
    applyAxisChange();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ADD)
  {
    pickboxOn();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_CANCEL)
  {
    pickboxCancel();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_DELETE)
  {
    pickboxDelete();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_COLOR_ON)
  {
    presetColorOn();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_METAL_ONLY)
  {
    presetMetalOnly();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_ORGANIC_ONLY)
  {
    presetOrganicOnly();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_INORGANIC_ONLY)
  {
    presetInorganicOnly();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_LOWDENSITY)
  {
    presetLowdensityOnly();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_GREYSCALE_ON)
  {
    presetGreyScale();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_INVERSE)
  {
    presetInverseImage();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_SHOWRIGHTIMAGE)
  {
    presetShowRightImage();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_ALLMATERIAL)
  {
    presetAllMaterials();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_VIEW_MODE)
  {
    SetRightViewerDisplayMode();
    m_currentOpacity = DEFAULT_VALUE_OPACITY;
    if (!m_vrwRightRef->isAThreatRenderer())
    {
      m_vrwRightRef->SetOpacity(m_currentOpacity);
    }
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_TOGGLE_THREAT_COLORIZATION)
  {
    applyToggleThreatColorization();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_SLABVIEW_PERCENTAGE_CHANGED)
  {
    // add slab view percentage change function
    applySlabPercentageChange();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_SLABVIEW_AXIS_CHANGED)
  {
    // add slab view axis change function
    applySlabAxisChange();
  }
  else if(command == QMLEnums::ThreatEvalPanelCommand::TEPC_SLABVIEW_TYPE_CHANGED)
  {
    // add slab view type change function
    applySlabTypeChange();
  }
}

/*!
* @fn       readColorCheckboxOIM()
* @param    None
* @return   bool - Tell is anything has changed
* @brief    Read values of organic, inorganic, metal checkboxes from UI.
*/
bool VolumeDisplayUIHelper::readColorCheckboxOIM()
{
  TRACE_LOG("");
  bool isChanged = false;

  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return isChanged;
  }


  QObject *qobj = rootObject->findChild<QObject*>("    Show Color    ");
  if(qobj)
  {
    bool colorMetal = false, colorOrganic = false, colorInorganic = false;
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

    QVariant returnedValue;
    QVariant msg = 0;
    QMetaObject::invokeMethod(p, "funcGetVal",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, msg));

    colorOrganic = returnedValue.toBool();

    msg = 1;
    QMetaObject::invokeMethod(p, "funcGetVal",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, msg));

    colorInorganic = returnedValue.toBool();

    msg = 2;
    QMetaObject::invokeMethod(p, "funcGetVal",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, msg));

    colorMetal = returnedValue.toBool();

    if (m_currentColorOrganic != colorOrganic ||
        m_currentColorInorganic != colorInorganic ||
        m_currentColorMetal != colorMetal)
    {
      isChanged = true;
      DEBUG_LOG("Color changed. organic from : "<< m_currentColorOrganic <<" to :"<< colorOrganic
                << "Inorganic from : "<< m_currentColorInorganic << " to : "<< colorInorganic
                << "Metal from : "<< m_currentColorMetal <<" to : "<< colorMetal);
      m_currentColorOrganic = colorOrganic;
      m_currentColorInorganic = colorInorganic;
      m_currentColorMetal = colorMetal;
    }
  }
  else
  {
    ERROR_LOG(" Failed to find child- Show Color");
  }
  return isChanged;
}

/*!
* @fn       pickboxDelete
* @param    None
* @return   None
* @brief    delete current pickbox
*/
void VolumeDisplayUIHelper::pickboxDelete()
{
  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->
      getBagbyState(BagData::RENDERING);
  if(bagData == NULL)
  {
    ERROR_LOG("Bag data is NULL. Can not delete pick box");
    return;
  }
  int currentThreat = bagData->getCurrrentThreat();
  m_vrwLeftRef->DeleteOperatorThreat(currentThreat);
  currentThreat = bagData->removePickBoxThreat(currentThreat);
  if (bagData->getCurrentVisibleEntityCount() > 0)
  {
    m_vdRef->setNextThreat();
  }
  if (bagData->getTotalVisibleAlarmThreats() == 0)
  {
    onViewChanged(VREnums::VIEW_AS_SURFACE);
    SetRightViewerDisplayMode();
  }
  m_vrwLeftRef->UpdateRenderer();
  m_vrwRightRef->UpdateRenderer();
}


/*!
* @fn       writeColorCheckboxOIM()
* @param    None
* @return   None
* @brief    Write values of organic, inorganic, metal checkboxes to UI.
*/
void VolumeDisplayUIHelper::writeColorCheckboxOIM()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }

  QObject *qobj = rootObject->findChild<QObject*>("    Show Color    ");
  if(qobj)
  {
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);
    if(p)
    {
      bool organic = m_currentColorOrganic;
      bool inorganic = m_currentColorInorganic;
      bool metal = m_currentColorMetal;

      QMetaObject::invokeMethod(p, "funcSetVal",
                                Q_ARG(QVariant, 0),
                                Q_ARG(QVariant, organic));

      QMetaObject::invokeMethod(p, "funcSetVal",
                                Q_ARG(QVariant, 1),
                                Q_ARG(QVariant, inorganic));

      QMetaObject::invokeMethod(p, "funcSetVal",
                                Q_ARG(QVariant, 2),
                                Q_ARG(QVariant, metal));
      DEBUG_LOG("Setting color values.")
    }
  }
  else
  {
    ERROR_LOG(" Failed to find child- Show Color");
  }
}

/*!
* @fn       writeQMLSliderValues()
* @param    None
* @return   None
* @brief    Write values of sliders to UI.
*/
void VolumeDisplayUIHelper::writeQMLSliderValues()
{
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }

  QObject* qobj = rootObject->findChild<QObject*>("threatevalpanel");
  if(qobj)
  {
    DEBUG_LOG("Setting values for contrast to : " << m_currentContrast
              << " opacity :" << m_currentOpacity
              << " and brightness :" << m_currentBrightness);
    QMetaObject::invokeMethod(qobj, "setModelContrast", Q_ARG(QVariant, m_currentContrast));
    QMetaObject::invokeMethod(qobj, "setModelOpacity", Q_ARG(QVariant, m_currentOpacity));
    QMetaObject::invokeMethod(qobj, "setModelBrightness", Q_ARG(QVariant, m_currentBrightness));
  }
  else
  {
    ERROR_LOG(" Failed to find child- threatevalpanel");
  }
}

/*!
* @fn       applyColorCheckboxOIM()
* @param    None
* @return   None
* @brief    Apply user changes in organic, inorganic, metal checkboxes to 3D image.
*/
void VolumeDisplayUIHelper::applyColorCheckboxOIM()
{
  TRACE_LOG("");
  // Apply changes only if some setting has changed.
  if(readColorCheckboxOIM())
  {
    DEBUG_LOG("Color settings changed so applying changes");
    std::vector<analogic::workstation::MaterialEnumeration> list;

    if(m_currentColorOrganic)
      list.push_back(analogic::workstation::ORGANIC);

    if(m_currentColorInorganic)
      list.push_back(analogic::workstation::INORGANIC);

    if(m_currentColorMetal)
      list.push_back(analogic::workstation::METAL);

    m_vrwLeftRef->SetHighlightedMaterials(list);
    m_vrwRightRef->SetHighlightedMaterials(list);
  }
  else
  {
    DEBUG_LOG("No change in settings.");
  }
}

/*!
* @fn       applyPaintDrop()
* @param    None
* @return   None
* @brief    Apply user press of paint drop to 3D image.
*/
void VolumeDisplayUIHelper::applyPaintDrop()
{
  TRACE_LOG("");
  m_currentRenderWithColor = !m_currentRenderWithColor;
  m_vrwLeftRef->RenderWithColor(m_currentRenderWithColor);
  m_vrwRightRef->RenderWithColor(m_currentRenderWithColor);
}

/*!
        * @fn       applySliderContrast()
        * @param    None
        * @return   None
        * @brief    Apply user changes in contrast slider to 3D image.
        */
void VolumeDisplayUIHelper::applySliderContrast()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  double uiContrast = 0;

  QObject *qobj = rootObject->findChild<QObject*>("sliderContrast");
  if(qobj)
  {
    uiContrast = qobj->property("value").toDouble();
  }
  else
  {
    ERROR_LOG(" Failed to find child- sliderContrast");
  }
  if(m_currentContrast != uiContrast)
  {
    DEBUG_LOG("Setting changed contrast to volume render widgets to apply changes and"
              " updating currentcontrast to : " << uiContrast);
    m_vrwLeftRef->SetContrast(uiContrast);
    m_vrwRightRef->SetContrast(uiContrast);

    m_currentContrast = uiContrast;
  }
}

/*!
        * @fn       writeCutViewAxis()
        * @param    None
        * @return   None
        * @brief    Write values of cut view axis
        */
void VolumeDisplayUIHelper::writeCutViewAxis()
{
  TRACE_LOG("");

  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject * qobj = rootObject->findChild<QObject*>("axismodel");
  if(qobj)
  {
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

    QVariant returnedValue;
    QVariant msg = m_axis;
    QMetaObject::invokeMethod(p, "funcSetSelectedAxis",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, msg));
  }
  else
  {
    ERROR_LOG(" Failed to find child- axismodel");
  }
}

/*!
        * @fn       writeSlabAxis()
        * @param    None
        * @return   None
        * @brief    Write values of cut view axis
        */
void VolumeDisplayUIHelper::writeSlabAxis()
{
  TRACE_LOG("");

  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject * qobj = rootObject->findChild<QObject*>("axisModelSlab");
  if(qobj)
  {
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

    QVariant returnedValue;
    QVariant msg = m_slabaxis;
    QMetaObject::invokeMethod(p, "funcSetSelectedAxis",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, msg));
  }
  else
  {
    ERROR_LOG("Failed to find child- axismodelSlab");
  }
}

/*!
* @fn       writeSlabType()
* @param    None
* @return   None
* @brief    Write values of slab type
*/
void VolumeDisplayUIHelper::writeSlabType()
{
  TRACE_LOG("");

  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject * qobj = rootObject->findChild<QObject*>("slabModel");
  if(qobj)
  {
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

    QVariant returnedValue;
    QVariant msg = m_slabtype;
    QMetaObject::invokeMethod(p, "funcSetSelectedSlabViewType",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, msg));
  }
  else
  {
    ERROR_LOG("Failed to find child- axismodelSlab");
  }
}

/*!
* @fn       onViewChanged
* @param    int - mode
* @return   None
* @brief    Write values of threat view mode
*/
void VolumeDisplayUIHelper::onViewChanged(VREnums::VRViewModeEnum aMode)
{
  TRACE_LOG("");
  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->
      getBagbyState(BagData::RENDERING);
  if(bagData == NULL)
  {
    ERROR_LOG("Bag data is NULL.Can not change view mode");
    return;
  }
  bagData->setCurrentVRViewmode(aMode);
  writeViewMode(aMode);
  DEBUG_LOG("View mode" << aMode);
}

/*!
* @fn       onSlabPositionChanged
* @param    double - slabOffset value
* @return   None
* @brief    Write slab percentage
*/
void VolumeDisplayUIHelper::onSlabPositionChanged(double slabOffset)
{
  // TO DO: Use Floating point comparision for values
  std::ostringstream streamObject;
  streamObject << std::fixed;
  streamObject << std::setprecision(3);
  streamObject << slabOffset;
  QString stringObject = QString::fromStdString(streamObject.str());
  slabOffset = stringObject.toDouble();

  if(m_currentSlabbingPercentage != slabOffset)
  {
    m_currentSlabbingPercentage = slabOffset;
    writeSlabbingPercentage();
  }
}

/*!
* @fn       writeViewMode
* @param    VREnums::VRViewModeEnum modeenum
* @return   None
* @brief    Write values of threat view mode
*/
void VolumeDisplayUIHelper::writeViewMode(VREnums::VRViewModeEnum modeenum)
{
  TRACE_LOG("");


  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject * qobj = rootObject->findChild<QObject*>("rightViewMode");
  if(qobj)
  {
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

    QVariant returnedValue;
    QVariant msg = modeenum;

    QMetaObject::invokeMethod(p, "funcSetSelectedViewMode",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, msg));
  }
  else
  {
    ERROR_LOG(" Failed to find child- view model");
  }
}




/*!
* @fn       writeCutViewPercentage()
* @param    None
* @return   None
* @brief    Write values of cut view percentage
*/
void VolumeDisplayUIHelper::writeCutViewPercentage()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject* qobj = rootObject->findChild<QObject*>("threatevalpanel");
  if(qobj)
  {
    DEBUG_LOG("Setting values for axis to : " << m_currentCutPercentage);
    QMetaObject::invokeMethod(qobj, "setModelCutPercentage", Q_ARG(QVariant, m_currentCutPercentage));
  }
  else
  {
    ERROR_LOG(" Failed to find child- threatevalpanel");
  }
}

/*!
* @fn       writeSlabbingPercentage()
* @param    None
* @return   None
* @brief    Write values of slab percentage to qml
*/
void VolumeDisplayUIHelper::writeSlabbingPercentage()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject* qobj = rootObject->findChild<QObject*>("threatevalpanel");
  if(qobj)
  {
    DEBUG_LOG("Setting values for axis to : " << m_currentSlabbingPercentage);
    QMetaObject::invokeMethod(qobj, "setModelSlabbingPercentage", Q_ARG(QVariant, m_currentSlabbingPercentage));
  }
  else
  {
    ERROR_LOG(" Failed to find child- threatevalpanel");
  }
}

/*!
* @fn       writeVariableDensityPercentage()
* @param    None
* @return   None
* @brief    Write values of variable density percentage
*/
void VolumeDisplayUIHelper::writeVariableDensityPercentage()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject* qobj = rootObject->findChild<QObject*>("threatevalpanel");
  if(qobj)
  {
    DEBUG_LOG("Setting values for variable density to : " << m_currentDensityPercentage);
    QMetaObject::invokeMethod(qobj, "setVariableDensitySliderValue", Q_ARG(QVariant, m_currentDensityPercentage));
  }
  else
  {
    ERROR_LOG(" Failed to find child- threatevalpanel");
  }
}

/*!
* @fn       writeShowAllThreatStateChange
* @param    None
* @return   None
* @brief    Write values of show all threat
*/
void VolumeDisplayUIHelper::writeShowAllThreatStateChange()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject* qobj = rootObject->findChild<QObject*>("threatevalpanel");
  if(qobj)
  {
    bool showAllThreat = m_vdRef->getShowAllThreat();
    DEBUG_LOG("Setting Show all threat value to : " << showAllThreat);
    QMetaObject::invokeMethod(qobj, "setShowAllThreat", Q_ARG(QVariant, showAllThreat));
  }
  else
  {
    ERROR_LOG(" Failed to find child- threatevalpanel");
  }
}

/*!
* @fn       applySlabAxisChange()
* @param    None
* @return   None
* @brief    Change value of slab axis
*/
void VolumeDisplayUIHelper::applySlabAxisChange()
{
  TRACE_LOG("");
  VREnums::AXIS axis = VREnums::Y;
  VREnums::SlabViewType slabtype = VREnums::BAG_VIEW;
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
  }
  else
  {
    QObject * qobj = rootObject->findChild<QObject*>("axisModelSlab");
    if(qobj)
    {
      QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

      QVariant returnedValue;
      QMetaObject::invokeMethod(p, "funcGetSelectedAxis",
                                Q_RETURN_ARG(QVariant, returnedValue));

      axis = (VREnums::AXIS)returnedValue.toInt();
    }
    else
    {
      ERROR_LOG(" Failed to find child- sliderSlabViewAxis");
    }

    if(m_vrwLeftRef && m_vrwRightRef)
    {
      INFO_LOG("Applying axis changes to volume : " << axis);
      INFO_LOG("Applying slab view type to volume : " << slabtype);
      // ARO-Note: Use Left widget only here.
      m_vrwLeftRef->SetCutViewAxis(axis);
      m_slabaxis = axis;
    }
    else
    {
      ERROR_LOG("Volume renderer widget is null");
    }
  }
}

/*!
* @fn       applySlabPercentageChange()
* @param    None
* @return   None
* @brief    Change value of slab percentage
*/
void VolumeDisplayUIHelper::applySlabPercentageChange()
{
  TRACE_LOG("");

  VREnums::AXIS axis = VREnums::Y;
  double cutpercentage = DEFAULT_VALUE_SLAB_PERCENTAGE;

  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
  }
  else
  {
    VREnums::SlabViewType slabtype = VREnums::BAG_VIEW;
    QObject *qobj = rootObject->findChild<QObject*>("sliderSlabView");
    if(qobj)
    {
      cutpercentage = qobj->property("value").toDouble();
      if(cutpercentage < 0.0 || cutpercentage > 1.0)
      {
        cutpercentage = DEFAULT_VALUE_SLAB_PERCENTAGE;
        ERROR_LOG("Incorrect value received. Setting cutpercentage to default value:" << cutpercentage);
      }
    }
    else
    {
      ERROR_LOG(" Failed to find child- sliderCutPlane");
    }

    qobj = rootObject->findChild<QObject*>("axisModelSlab");
    if(qobj)
    {
      QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

      QVariant returnedValue;
      QMetaObject::invokeMethod(p, "funcGetSelectedAxis",
                                Q_RETURN_ARG(QVariant, returnedValue));

      axis = (VREnums::AXIS)returnedValue.toInt();
    }
    else
    {
      ERROR_LOG(" Failed to find child- sliderCutPlane");
    }

    qobj = rootObject->findChild<QObject*>("slabModel");
    if(qobj)
    {
      QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

      QVariant returnedValue;
      QMetaObject::invokeMethod(p, "funcGetSelectedSlabViewType",
                                Q_RETURN_ARG(QVariant, returnedValue));

      slabtype = (VREnums::SlabViewType)returnedValue.toInt();
    }
    else
    {
      ERROR_LOG(" Failed to find child- sliderSlabViewAxis");
    }

    if ((m_currentSlabbingPercentage != cutpercentage) || (m_slabaxis != axis) || m_slabtype != slabtype)
    {
      DEBUG_LOG("Setting changed cut view to volume render widgets to apply changes and"
                " updating cut view to : " << cutpercentage);
      if(m_slabtype != slabtype)
      {
        m_slabtype = slabtype;
        m_vrwLeftRef->SetSlabViewType(m_slabtype);
        m_vrwRightRef->SetSlabViewType(m_slabtype);
      }

      if(m_currentSlabbingPercentage != cutpercentage)
      {
        m_currentSlabbingPercentage = cutpercentage;
        m_vrwLeftRef->SetSlabUnpackPosition(m_currentSlabbingPercentage, axis, true);
        m_vrwRightRef->SetSlabUnpackPosition(m_currentSlabbingPercentage, axis, true);
      }

      m_slabaxis = axis;
    }
  }
}

/*!
* @fn       applySlabTypeChange()
* @param    None
* @return   None
* @brief    Change value of slab type
*/
void VolumeDisplayUIHelper::applySlabTypeChange()
{
  TRACE_LOG("");
  VREnums::SlabViewType slabtype = VREnums::BAG_VIEW;
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject * qobj = rootObject->findChild<QObject*>("slabModel");
  if(qobj)
  {
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

    QVariant returnedValue;
    QMetaObject::invokeMethod(p, "funcGetSelectedSlabViewType",
                              Q_RETURN_ARG(QVariant, returnedValue));

    slabtype = (VREnums::SlabViewType)returnedValue.toInt();
  }
  else
  {
    ERROR_LOG(" Failed to find child- sliderSlabViewAxis");
  }
  if(m_vrwLeftRef && m_vrwRightRef)
  {
    INFO_LOG("Applying slab view type to volume : " << slabtype);

    boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->
        getBagbyState(BagData::RENDERING);
    if(bagData != nullptr)
    {
      bagData->setCurrentSlabViewType(slabtype);
      bagData->setCurrentThreat(DEFAULT_VALUE_CURRENT_THREAT);
      bagData->setCurrentVisibleEntityIndex(DEFAULT_VALUE_CURRENT_THREAT);
    }

    m_vrwLeftRef->SetSlabViewType(slabtype);
    m_vrwRightRef->SetSlabViewType(slabtype);
    m_vdRef->setNextThreat();
    m_slabtype = slabtype;
  }
}

/*!
* @fn       applyAxisChange()
* @param    None
* @return   None
* @brief    Apply user changes in axis to volume
*/
void VolumeDisplayUIHelper::applyAxisChange()
{
  TRACE_LOG("");
  VREnums::AXIS axis = VREnums::Y;

  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
  }
  else
  {
    QObject * qobj = rootObject->findChild<QObject*>("axismodel");
    if(qobj)
    {
      QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

      QVariant returnedValue;
      QMetaObject::invokeMethod(p, "funcGetSelectedAxis",
                                Q_RETURN_ARG(QVariant, returnedValue));

      axis = (VREnums::AXIS)returnedValue.toInt();
    }
    else
    {
      ERROR_LOG(" Failed to find child- sliderCutPlane");
    }

    if(m_vrwLeftRef && m_vrwRightRef)
    {
      m_axis = axis;
      INFO_LOG("Applying axis changes to volume : " << m_axis);
      m_vrwLeftRef->SetCutViewAxis(m_axis);
    }
    else
    {
      ERROR_LOG("Volume renderer widget is null");
    }
  }
}

/*!
* @fn       applyCutViewPercentageChange()
* @param    None
* @return   None
* @brief    Apply user changes in cut view slider on volume
*/
void VolumeDisplayUIHelper::applyCutViewPercentageChange()
{
  TRACE_LOG("");

  double cutpercentage = DEFAULT_VALUE_CUT_PERCENTAGE;
  VREnums::AXIS axis = VREnums::Y;

  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
  }
  else
  {
    QObject *qobj = rootObject->findChild<QObject*>("sliderCutPlane");
    if(qobj)
    {
      cutpercentage = qobj->property("value").toDouble();
      if(cutpercentage<0.0 || cutpercentage>1.0)
      {
        ERROR_LOG("Incorrect value received. Setting cutpercentage to Default value" << cutpercentage);
        cutpercentage = DEFAULT_VALUE_CUT_PERCENTAGE;
      }
    }
    else
    {
      ERROR_LOG(" Failed to find child- sliderCutPlane");
    }

    qobj = rootObject->findChild<QObject*>("axismodel");
    if(qobj)
    {
      QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

      QVariant returnedValue;
      QMetaObject::invokeMethod(p, "funcGetSelectedAxis",
                                Q_RETURN_ARG(QVariant, returnedValue));

      axis = (VREnums::AXIS)returnedValue.toInt();
    }
    else
    {
      ERROR_LOG(" Failed to find child- sliderCutPlane");
    }
    if ((m_currentCutPercentage != cutpercentage) || (m_axis != axis))
    {
      DEBUG_LOG("Setting changed cut view to volume render widgets to apply changes and"
                " updating cut view to : " << cutpercentage);
      m_axis = axis;
      m_currentCutPercentage = cutpercentage;
      m_vrwLeftRef->SetSlabUnpackPosition(m_currentCutPercentage, m_axis, false);
      m_vrwRightRef->SetSlabUnpackPosition(m_currentCutPercentage, m_axis, false);
    }
  }
}

/*!
* @fn       applySliderBrightness()
* @param    None
* @return   None
* @brief    Apply user changes in brightness slider to 3D image.
*/
void VolumeDisplayUIHelper::applySliderBrightness()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }

  double uiBrightness = 0;

  QObject *qobj = rootObject->findChild<QObject*>("sliderBrightness");
  if(qobj)
  {
    uiBrightness = qobj->property("value").toDouble();
  }
  else
  {
    ERROR_LOG(" Failed to find child- sliderBrightness");
  }

  if(m_currentBrightness != uiBrightness)
  {
    DEBUG_LOG("Setting changed brightness to volume render widgets to apply changes and"
              " updating currentbrightness to : " << uiBrightness);
    m_vrwLeftRef->SetBrightness(uiBrightness);
    m_vrwRightRef->SetBrightness(uiBrightness);

    m_currentBrightness = uiBrightness;
  }
}

/*!
* @fn       applySliderOpacity()
* @param    None
* @return   None
* @brief    Apply user changes in opacity slider to 3D image.
*/
void VolumeDisplayUIHelper::applySliderOpacity()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }

  double uiOpacity = 0;

  QObject *qobj = rootObject->findChild<QObject*>("sliderOpacity");
  if(qobj)
  {
    uiOpacity = qobj->property("value").toDouble();
  }
  else
  {
    ERROR_LOG(" Failed to find child- sliderOpacity");
  }
  if(m_currentOpacity != uiOpacity)
  {
    DEBUG_LOG("Setting changed opacity to volume render widgets to apply changes and"
              " updating currentopacity to : " << uiOpacity);
    m_vrwLeftRef->SetOpacity(uiOpacity);
    m_vrwRightRef->SetOpacity(uiOpacity);

    m_currentOpacity = uiOpacity;
  }
}


/*!
* @fn       applyZoomIn()
* @param    None
* @return   None
* @brief    Apply user press of zoomin button to 3D image.
*/
void VolumeDisplayUIHelper::applyZoomIn()
{
  TRACE_LOG("");
  m_currentZoomLevel *= 1.1;
  DEBUG_LOG("Setting changed ZoomIn values to volume render widgets to apply changes and"
            " updating currentzoomlevel to : " << m_currentZoomLevel);

  m_vrwLeftRef->SetZoom(m_currentZoomLevel);
  // ARO-NOTE: 6-21-2018
  // Lets perform the  zoom on the LEFT  Viewer ONLY and use sync siblings for
  // the right window if its a threat viewer  perform the zoom directly.
  // This reduces the chance of unintended redraws/paints.
  // *** DEPRIOCATED m_vrwRightRef->SetZoom(m_currentZoomLevel);
}

/*!
* @fn       applyDisplayAlarmThreatonly()
* @param    None
* @return   None
* @brief    Apply user press of display alarm threat only.
*/
void VolumeDisplayUIHelper::applyDisplayAlarmThreatonly()
{
  if((m_vrwLeftRef == NULL) ||
     (m_vrwRightRef == NULL))
  {
    ERROR_LOG("VolumeRendererWidget is NULL. returning.");
    return;
  }
  m_displayAlarmThreat = !m_displayAlarmThreat;
  //---------------------------------
  // ARO-NOTE:  6-25-2018
  // Call from LEFT viewer only the Volumer Renderer will know what to do.
  m_vrwLeftRef->DisplayThreatOnly(m_displayAlarmThreat);
  m_vrwRightRef->UpdateRenderer();
}

/*!
* @fn       applyToggleThreatColorization
* @param    None
* @return   None
* @brief    Toggles threat colorization.
*/
void VolumeDisplayUIHelper::applyToggleThreatColorization()
{
  if((m_vrwLeftRef == NULL) ||
     (m_vrwRightRef == NULL))
  {
    ERROR_LOG("VolumeRendererWidget is NULL. returning.");
    return;
  }

  m_enableThreatColorization = !m_enableThreatColorization;
  // ARO-Note 8-28-2019: We only need to call this on the Left Viewewr
  // just like DisplayThreatOnly()
  m_vrwLeftRef->SetToggleThreatColorization(m_enableThreatColorization);
  m_vrwLeftRef->RepaintViewers();
}

/*!
        * @fn       applyZoomOut()
        * @param    None
        * @return   None
        * @brief    Apply user press of zoomout button to 3D image.
        */
void VolumeDisplayUIHelper::applyZoomOut()
{
  TRACE_LOG("");
  m_currentZoomLevel /= 1.1;
  DEBUG_LOG("Setting changed ZoomOut values to volume render widgets to apply changes and"
            " updating currentzoomlevel to : " << m_currentZoomLevel);
  m_vrwLeftRef->SetZoom(m_currentZoomLevel);
  // ARO-NOTE: 6-21-2018
  // Lets perform the  zoom on the LEFT  Viewer ONLY and use sync siblings for
  // the right window if its a threat viewer  perform the zoom directly.
  // This reduces the chance of unintended redraws/paints.
  // *** DEPRIOCATED m_vrwRightRef->SetZoom(m_currentZoomLevel);
}

/*!
        * @fn       applyReload()
        * @param    None
        * @return   None
        * @brief    Apply user press of reload button to 3D image.
        */
void VolumeDisplayUIHelper::applyReload()
{
  TRACE_LOG("");
  DEBUG_LOG("Applying reload.");

  m_vrwRightRef->setCursor(Qt::BusyCursor);
  m_vrwLeftRef->setCursor(Qt::BusyCursor);

  m_axis = DEFAULT_VALUE_AXIS;
  writeCutViewAxis();
  m_slabaxis = DEFAULT_VALUE_AXIS;
  writeSlabAxis();

  m_currentCutPercentage = DEFAULT_VALUE_CUT_PERCENTAGE;
  writeCutViewPercentage();
  m_currentSlabbingPercentage = DEFAULT_VALUE_CUT_PERCENTAGE;
  writeSlabbingPercentage();

  m_slabtype = DEFAULT_VALUE_SLABTYPE;
  writeSlabType();

  m_currentDensityPercentage = DEFAULT_VALUE_VARIABLE_DENSITY;
  writeVariableDensityPercentage();

  m_currentColorMetal = DEFAULT_VALUE_COLOR_METAL;
  m_currentColorOrganic = DEFAULT_VALUE_COLOR_ORGANIC;
  m_currentColorInorganic = DEFAULT_VALUE_COLOR_INORGANIC;

  //---------------------
  // ARO-NOTE: Apply color before reset display so that if Splash Screen is
  // enabeled we get the propper splash.
  //---------------------
  DEBUG_LOG("Apply paint drop");
  m_currentRenderWithColor = DEFAULT_VALUE_RENDER_WITH_COLOR;
  applyPaintDrop();

  DEBUG_LOG("Apply Low Density");
  // Below is toggled value as internally presetLowdensityOnly button toggle status.
  m_currentWithLowDensity = !DEFAULT_VALUE_RENDER_WITH_LOW_DENSITY;
  presetLowdensityOnly();

  std::vector<analogic::workstation::MaterialEnumeration> list;
  if(m_currentColorOrganic)
    list.push_back(analogic::workstation::ORGANIC);

  if(m_currentColorInorganic)
    list.push_back(analogic::workstation::INORGANIC);

  if(m_currentColorMetal)
    list.push_back(analogic::workstation::METAL);

  m_vrwLeftRef->SetHighlightedMaterials(list);
  m_vrwRightRef->SetHighlightedMaterials(list);


  if(m_inverse != DEFAULT_VALUE_INVERSE)
  {
    DEBUG_LOG("Reset inverse to: " << DEFAULT_VALUE_INVERSE);
    m_inverse = DEFAULT_VALUE_INVERSE;
    m_vrwLeftRef->SetInverse(m_inverse);
    m_vrwRightRef->SetInverse(m_inverse);
  }

  if(m_currentOpacity != DEFAULT_VALUE_OPACITY)
  {
    DEBUG_LOG("Reset opacity to: " << DEFAULT_VALUE_OPACITY);
    m_currentOpacity = DEFAULT_VALUE_OPACITY;
    m_vrwLeftRef->SetOpacity(m_currentOpacity);
    m_vrwRightRef->SetOpacity(m_currentOpacity);
  }

  if(m_currentContrast != DEFAULT_VALUE_CONTRAST)
  {
    DEBUG_LOG("Reset contrast to: " << DEFAULT_VALUE_CONTRAST);
    m_currentContrast = DEFAULT_VALUE_CONTRAST;
    m_vrwLeftRef->SetContrast(m_currentContrast);
    m_vrwRightRef->SetContrast(m_currentContrast);
  }
  if(m_currentBrightness != DEFAULT_VALUE_BRIGHTNESS)
  {
    DEBUG_LOG("Reset Brightness to: " << DEFAULT_VALUE_BRIGHTNESS);
    m_currentBrightness = DEFAULT_VALUE_BRIGHTNESS;
    m_vrwLeftRef->SetBrightness(m_currentBrightness);
    m_vrwRightRef->SetBrightness(m_currentBrightness);
  }

  DEBUG_LOG("Reset left volume renderer widget parameters and display");
  m_vrwLeftRef->ResetParameters();
  m_vrwLeftRef->ResetDisplay(false);

  DEBUG_LOG("Reset right volume renderer widget parameters and display");
  m_vrwRightRef->ResetParameters();
  m_vrwRightRef->ResetDisplay(false);

  if(m_currentZoomLevel != DEFAULT_VALUE_ZOOM)
  {
    DEBUG_LOG("Reset Zoomlevel to: " << DEFAULT_VALUE_ZOOM);
    m_currentZoomLevel = DEFAULT_VALUE_ZOOM;
  }

  DEBUG_LOG("Write changed values of color to qml");
  writeColorCheckboxOIM();
  DEBUG_LOG("Write changed values of contrast, brightness and opacity to qml");
  writeQMLSliderValues();
  m_displayAlarmThreat = DEFAULT_VALUE_THREAT_REGION;
  m_enableThreatColorization = DEFAULT_VALUE_ENABLE_THREAT_COLORIZATION;
  applyDisplayAlarmThreatonly();
  applyToggleThreatColorization();

  resetMeasureBoxButtons();
  m_vrwRightRef->MeasureVolumeByValue(false);

  setDefaultVRMode();
  resetPresetButtons();
  m_vrwRightRef->setCursor(Qt::ArrowCursor);
  m_vrwLeftRef->setCursor(Qt::ArrowCursor);
}

/*!
* @fn       applyImageInversion()
* @param    None
* @return   None
* @brief    Apply user press of image inversion button to 3D image.
*/
void VolumeDisplayUIHelper::applyImageInversion()
{
  m_inverse =!m_inverse;
  DEBUG_LOG("Applying image inversion");
  m_vrwLeftRef->SetInverse(m_inverse);
  m_vrwRightRef->SetInverse(m_inverse);

  // TODO(Agiliad):
  // After SetInverse() is implemented, see if show(), render(), refresh() or some such function
  // Of the volume renderer interface needs to be called here.
}

/*!
* @fn       applyFrontalViewButtonsAndBlankScreen()
* @param    None
* @return   None
* @brief    Apply user press of frontal view button to 3D image.
*/
void VolumeDisplayUIHelper::applyFrontalViewButtonsAndBlankScreen()
{
}

/*!
* @fn       screenshot()
* @param    None
* @return   None
* @brief    Take screenshot.
*/
void VolumeDisplayUIHelper::screenshot()
{
  INFO_LOG("Taking screenshot");
  double pageWidth = m_printer->pageRect().width();
  double width = m_parentWidgetRef->width();
  double xscale = pageWidth / width;

  double pageHeight = m_printer->pageRect().height();
  double height = m_parentWidgetRef->height();
  double yscale = pageHeight / height;

  double scale = qMin(xscale, yscale);
  DEBUG_LOG("parameters used for screenshots are: "
            " pageWidth: "    << pageWidth
            << "pageHeight: " << pageHeight
            << " width: "     << width
            << " height: "    << height
            << " xscale: "    << xscale
            << " yscale: "    << yscale);
  QPainter painter;
  DEBUG_LOG("Begin painting.");
  painter.begin(m_printer);
  painter.scale(scale, scale);

  QWindow* mainWindowHandle = m_parentWidgetRef->windowHandle();
  if(mainWindowHandle)
  {
    QScreen* screenHandle = mainWindowHandle->screen();
    if(screenHandle)
    {
      DEBUG_LOG("Drawing the screen.");
      QPixmap pixmap = screenHandle->grabWindow(0);
      painter.drawPixmap(0, 0, pixmap);
    }
  }
  else
  {
    ERROR_LOG("handle to window is NULL.");
  }
  painter.end();
  DEBUG_LOG("End painting.");
  DEBUG_LOG("Releasing the printer resource.");
  mainWindowHandle = NULL;
  SAFE_DELETE( m_printer);
  m_printer = NULL;
}

/*!
* @fn       applyPrinter()
* @param    None
* @return   None
* @brief    Handle user press of print button.
*/
void VolumeDisplayUIHelper::applyPrinter()
{
  QString filename;
  DEBUG_LOG("User pressed printer button");
  if(!m_vdRef->isFront())
  {
    ERROR_LOG("Not front widget.");
    return;
  }
  SAFE_DELETE(m_printer);
  m_printer = new QPrinter();
  THROW_IF_FAILED(( m_printer == NULL )? Errors::E_OUTOFMEMORY : Errors::S_OK);
  DEBUG_LOG("Got printer resource handle.");
  m_printer->setPageOrientation(QPageLayout::Landscape);
  QDateTime time = QDateTime::currentDateTime();
  filename = QApplication::applicationDirPath() +
      QString("/") + QString("print_") +
      time.toString("MMddyy_hhmmss") + QString(".pdf");

  m_printer->setOutputFileName(filename);
  m_printer->setOutputFormat(QPrinter::PdfFormat);
  SAFE_DELETE(m_printDialog);
  m_printDialog = new QPrintDialog(m_printer, m_parentWidgetRef);
  THROW_IF_FAILED(( m_printDialog == NULL )? Errors::E_OUTOFMEMORY : Errors::S_OK);
  DEBUG_LOG("Got dialog handle.");
  m_printDialog->setWindowTitle("Print");
  //    m_printDialog->setConfirmOverwrite(false);
  if (m_printDialog->exec() != QDialog::Accepted)
  {
    DEBUG_LOG("Dialog rejected.");
    // SAFE_DELETE(m_printDialog);
    return;
  }
  DEBUG_LOG("Dialog accepted and releasing resource.");

  // A delay is required between dissapearing of the QPrintDialog and
  // call to QScreen::grabWindow().
  DEBUG_LOG("Calling function to take screen shot after QPrintDialog is accepted.");
  QTimer::singleShot(500, this, &VolumeDisplayUIHelper::screenshot);
}

/*!
* @fn       pickboxOn()
* @param    None
* @return   None
* @brief    Start pickbox instrumentation graphics
*/
void VolumeDisplayUIHelper::pickboxOn()
{
  m_vrwLeftRef->StartOperatorThreatGeneration();
  m_vrwRightRef->StartOperatorThreatGeneration();
}

/*!
* @fn       SetRightViewerDisplayMode()
* @param    None
* @return   None
* @brief    set right viewer display mode
*/
void VolumeDisplayUIHelper::SetRightViewerDisplayMode()
{
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject * qobj = rootObject->findChild<QObject*>("rightViewMode");
  if(qobj)
  {
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

    QVariant returnedValue;
    QMetaObject::invokeMethod(p, "funcGetSelectedViewMode",
                              Q_RETURN_ARG(QVariant, returnedValue));
    VREnums::VRViewModeEnum mode = ( VREnums::VRViewModeEnum )returnedValue.toInt();
    boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->
        getBagbyState(BagData::RENDERING);
    if(bagData == NULL)
    {
      ERROR_LOG("Bag data is NULL.Can not change view mode");
      return;
    }
    bagData->setCurrentVRViewmode(mode);
    bagData->setCurrentSlabViewType(VREnums::BAG_VIEW);
    m_vrwRightRef->SetRightViewerDisplayMode(mode);
    // Refresh threat count
    bagData->setCurrentThreat(DEFAULT_VALUE_CURRENT_THREAT);
    bagData->setCurrentVisibleEntityIndex(DEFAULT_VALUE_CURRENT_THREAT);
    if((mode == VREnums::VIEW_AS_THREAT)
       || (mode == VREnums::VIEW_AS_SURFACE)
       || (mode == VREnums::VIEW_AS_LAPTOP))
    {
      m_vdRef->setNextThreat();
      m_currentCutPercentage = DEFAULT_VALUE_CUT_PERCENTAGE;
      writeCutViewPercentage();

      m_axis = VREnums::Y;
      writeCutViewAxis();

      m_vrwLeftRef->SetEnableThreatRedering(true);
      m_vrwRightRef->SetEnableThreatRedering(true);

      m_vrwLeftRef->SetSlabUnpackPosition(m_currentCutPercentage, m_axis, false);
      m_vrwRightRef->SetSlabUnpackPosition(m_currentCutPercentage, m_axis, false);
      m_vrwLeftRef->SetCutViewAxis(m_axis);
    }
    else if(mode == VREnums::VIEW_AS_SLAB)
    {
      double slabThickness = WorkstationConfig::getInstance()->getSlabbingThickness();
      m_vrwLeftRef->SetSlabThickness(slabThickness);
      m_vrwRightRef->SetSlabThickness(slabThickness);
      m_currentSlabbingPercentage = DEFAULT_VALUE_SLAB_PERCENTAGE;
      writeSlabbingPercentage();

      m_slabaxis =  VREnums::Y;
      writeSlabAxis();

      m_vrwLeftRef->SetSlabUnpackPosition(m_currentSlabbingPercentage, m_slabaxis, true);
      m_vrwRightRef->SetSlabUnpackPosition(m_currentSlabbingPercentage, m_slabaxis, true);
      m_vrwLeftRef->SetSlabViewType(VREnums::BAG_VIEW);
      m_vrwLeftRef->SetCutViewAxis(m_slabaxis);
    }
  }
  else
  {
    ERROR_LOG(" Failed to find child- rightViewMode");
  }
}

/*!
* @fn       pickboxAccept
* @param    QMLEnums::ThreatEvalPanelCommand command
* @return   int - threat index
* @brief    Accept changes to pickbox instrumentation graphics
*/
int VolumeDisplayUIHelper::pickboxAccept(QMLEnums::ThreatEvalPanelCommand eThreatType)
{
  // Enquire bounds of pickbox
  QVector3D p0Ret;
  QVector3D extentRet;
  QString   strThreatType;

  if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_EXPLOSIVES)
  {
    strThreatType = "Explosive";
  }
  else if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_ANOMALY)
  {
    strThreatType = "Anomaly";
  }
  else if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_WEAPON)
  {
    strThreatType = "Weapon";
  }
  else if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_LIQUID)
  {
    strThreatType = "Liquid";
  }
  else if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_CONTRABAND)
  {
    strThreatType = "Contraband";
  }

  int threat_index = m_vrwLeftRef->RightAcceptOperatorGeneratedThreat(strThreatType,
                                                                      p0Ret,
                                                                      extentRet);

  if(threat_index == -1)
    return threat_index;

  boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->
      getBagbyState(BagData::RENDERING);
  if(bagData == NULL)
  {
    ERROR_LOG("Bag data is NULL.");
    return 0;
  }

  // TODO(amirkumar.umani@agiliad.com) Revisit when separating rerun and training #ifdef WORKSTATION
  // Create new threat item corresponding to given box
  // If this is first operator threat then add new threat module (TDR) in bagdata
  int ret = bagData->addPickBoxThreat(p0Ret, extentRet, eThreatType);
  if(ret < 0)
  {
    ERROR_LOG("Pickbox addition failed.");
  }

  // Set View mode to threat as threat is getting added
  if( bagData->getCurrentVRViewmode() != VREnums::VIEW_AS_THREAT)
  {
    writeViewMode(VREnums::VIEW_AS_THREAT);
    SetRightViewerDisplayMode();
  }

  QString userDescStr = tr("Threat Decision: ");
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  if((currentscreen == QMLEnums::BAGSCREENING_SCREEN) ||
     (currentscreen == QMLEnums::TRAINING_BAG_SCREEN) ||
     (currentscreen == QMLEnums::RERUNSELECTION_BAG_SCREEN))
  {
     userDescStr = "";
  }
  else
  {
     userDescStr += "Suspect";
  }
  m_vrwRightRef->ShowThreatUserDecision(userDescStr);

  return bagData->getTotalAvailableThreats()-1;
}

/*!
* @fn       pickboxCancel()
* @param    None
* @return   None
* @brief    Stop drawing pickbox instrumentation graphics
*/
void VolumeDisplayUIHelper::pickboxCancel()
{
  m_vrwLeftRef->CancelOperatorThreatGeneration();
  m_vrwRightRef->CancelOperatorThreatGeneration();
}

/*!
* @fn       resetPresetButtons()
* @param    None
* @return   None
* @brief    resetPresetButtons
*/
void VolumeDisplayUIHelper::resetPresetButtons()
{
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject * qobj = rootObject->findChild<QObject*>("presetModel");
  if(qobj)
  {
    QAbstractListModel *p = qobject_cast<QAbstractListModel*>(qobj);

    QVariant returnedValue;

    // reset all material.
    presetAllMaterials();

    QVariant command = QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_ALLMATERIAL;
    QVariant value = true;
    QMetaObject::invokeMethod(p, "funcSetSelectedPresetButton",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, command),
                              Q_ARG(QVariant, value));


    command = QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_METAL_ONLY;
    value = false;
    QMetaObject::invokeMethod(p, "funcSetSelectedPresetButton",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, command),
                              Q_ARG(QVariant, value));

    command = QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_ORGANIC_ONLY;
    value = false;
    QMetaObject::invokeMethod(p, "funcSetSelectedPresetButton",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, command),
                              Q_ARG(QVariant, value));

    command = QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_INORGANIC_ONLY;
    value = false;
    QMetaObject::invokeMethod(p, "funcSetSelectedPresetButton",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, command),
                              Q_ARG(QVariant, value));

    command = QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_GREYSCALE_ON;
    value = false;
    QMetaObject::invokeMethod(p, "funcSetSelectedPresetButton",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, command),
                              Q_ARG(QVariant, value));

    command = QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_INVERSE;
    value = false;
    QMetaObject::invokeMethod(p, "funcSetSelectedPresetButton",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, command),
                              Q_ARG(QVariant, value));

    command = QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_SHOWRIGHTIMAGE;
    VREnums::VRViewModeEnum dfmode = getDefaultVRMode();
    if(dfmode == VREnums::VIEW_AS_SURFACE_CAMERA)
    {
      value = true;
      m_currentOpacity = 0.9;
    }
    else
    {
      value = false;
      m_currentOpacity = DEFAULT_VALUE_OPACITY;
    }
    if (!m_vrwRightRef->isAThreatRenderer())
    {
      m_vrwRightRef->SetOpacity(m_currentOpacity);
    }
    QMetaObject::invokeMethod(p, "funcSetSelectedPresetButton",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, command),
                              Q_ARG(QVariant, value));

    command = QMLEnums::ThreatEvalPanelCommand::TEPC_PRESET_LOWDENSITY;
    value = false;
    QMetaObject::invokeMethod(p, "funcSetSelectedPresetButton",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, command),
                              Q_ARG(QVariant, value));
  }
  else
  {
    ERROR_LOG(" Failed to find child- preset model");
  }
}

/*!
* @fn       presetColorOn()
* @param    None
* @return   None
* @brief    Preset color on
*/
void VolumeDisplayUIHelper::presetColorOn()
{
  m_vrwLeftRef->RenderWithColor(true);
  m_vrwRightRef->RenderWithColor(true);
}

/*!
* @fn       presetMetalOnly()
* @param    None
* @return   None
* @brief    Preset Metal only display graphics
*/
void VolumeDisplayUIHelper::presetMetalOnly()
{
  TRACE_LOG("");
  std::vector<analogic::workstation::MaterialEnumeration> list;
  list.push_back(analogic::workstation::METAL);
  m_vrwLeftRef->SetHighlightedMaterials(list);
  m_vrwRightRef->SetHighlightedMaterials(list);
}

/*!
* @fn       presetOrganicOnly()
* @param    None
* @return   None
* @brief    Preset Organic only display graphics
*/
void VolumeDisplayUIHelper::presetOrganicOnly()
{
  TRACE_LOG("");
  std::vector<analogic::workstation::MaterialEnumeration> list;
  list.push_back(analogic::workstation::ORGANIC);
  m_vrwLeftRef->SetHighlightedMaterials(list);
  m_vrwRightRef->SetHighlightedMaterials(list);
}

/*!
* @fn       presetInorganicOnly()
* @param    None
* @return   None
* @brief    Preset Inorganic only display graphics
*/
void VolumeDisplayUIHelper::presetInorganicOnly()
{
  TRACE_LOG("");
  std::vector<analogic::workstation::MaterialEnumeration> list;
  list.push_back(analogic::workstation::INORGANIC);
  m_vrwLeftRef->SetHighlightedMaterials(list);
  m_vrwRightRef->SetHighlightedMaterials(list);
}

/*!
* @fn       presetLowdensityOnly()
* @param    None
* @return   None
* @brief    Preset Low Density only display graphics
*/
void VolumeDisplayUIHelper::presetLowdensityOnly()
{
  TRACE_LOG("");
  m_currentWithLowDensity = !m_currentWithLowDensity;
  m_vrwLeftRef->EnableLowDensity(m_currentWithLowDensity);
  m_vrwRightRef->EnableLowDensity(m_currentWithLowDensity);
}

/*!
* @fn       presetGreyScale()
* @param    None
* @return   None
* @brief    Preset Grey scale graphics
*/
void VolumeDisplayUIHelper::presetGreyScale()
{
  TRACE_LOG("");
  m_currentRenderWithColor = !m_currentRenderWithColor;
  m_vrwLeftRef->RenderWithColor(m_currentRenderWithColor);
  m_vrwRightRef->RenderWithColor(m_currentRenderWithColor);
  if (!m_vrwRightRef->isAThreatRenderer())
  {
    m_vrwRightRef->SetOpacity(m_currentOpacity);
  }
  m_vrwLeftRef->RepaintViewers();
}

/*!
* @fn       presetInverseImage()
* @param    None
* @return   None
* @brief    Preset Inverse image
*/
void VolumeDisplayUIHelper::presetInverseImage()
{
  applyImageInversion();

  if (!m_vrwRightRef->isAThreatRenderer())
  {
    m_vrwRightRef->SetOpacity(m_currentOpacity);
  }
}

/*!
* @fn       presetShowRightImage()
* @param    None
* @return   None
* @brief    Preset show right side image without transparency
*/
void VolumeDisplayUIHelper::presetShowRightImage()
{
  if(m_currentOpacity != DEFAULT_VALUE_OPACITY)
  {
    m_currentOpacity = DEFAULT_VALUE_OPACITY;
  }
  else
  {
    m_currentOpacity = PHOTOGRAPIC_OPACITY;
  }
  if (!m_vrwRightRef->isAThreatRenderer())
  {
    m_vrwRightRef->SetOpacity(m_currentOpacity);
  }
}

/*!
* @fn       presetAllMaterials()
* @param    None
* @return   None
* @brief    Preset to all materials display graphics
*/
void VolumeDisplayUIHelper::presetAllMaterials()
{
  std::vector<analogic::workstation::MaterialEnumeration> list;
  list.push_back(analogic::workstation::ORGANIC);
  list.push_back(analogic::workstation::INORGANIC);
  list.push_back(analogic::workstation::METAL);
  m_vrwLeftRef->SetHighlightedMaterials(list);
  m_vrwRightRef->SetHighlightedMaterials(list);
}

/*!
* @fn       resetMeasureBoxButtons
* @param    None
* @return   None
* @brief    Resets all measurebox buttons.
*/
void VolumeDisplayUIHelper::resetMeasureBoxButtons()
{
  TRACE_LOG("");
  QQuickItem *rootObject = m_qmlContainerRef->rootObject();
  if(!rootObject)
  {
    ERROR_LOG("rootObject is NULL.");
    return;
  }
  QObject* qobj = rootObject->findChild<QObject*>("threatevalpanel");
  if(qobj)
  {
    QMetaObject::invokeMethod(qobj, "resetMeasurementBoxButtons");
  }
  else
  {
    ERROR_LOG(" Failed to find child- threatevalpanel");
  }
}

/*!
* @fn       getEnableThreatColorization()
* @param    None
* @return   bool
* @brief    get threat colorization value
*/
bool VolumeDisplayUIHelper::getEnableThreatColorization() const
{
  return m_enableThreatColorization;
}

/*!
* @fn       getDisplayAlarmThreat
* @param    None
* @return   bool
* @brief    get display alarm threat
*/
bool VolumeDisplayUIHelper::getDisplayAlarmThreat() const
{
  return m_displayAlarmThreat;
}


/*!
* @fn       getDefaultVRMode
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    Get default right view mode
*/
VREnums::VRViewModeEnum VolumeDisplayUIHelper::getDefaultVRMode()
{
  UILayoutManager *m_uiLayoutManager = UILayoutManager::getUILayoutManagerInstance();
  VREnums::VRViewModeEnum viewmode = DEFAULT_VALUE_VR_VIEW_MODE;

  if(m_uiLayoutManager != NULL)
  {
    QMLEnums::WSType wsType = WorkstationConfig::getInstance()->getWorkstationNameEnum();

    if(wsType == QMLEnums::SEARCH_WORKSTATION || wsType == QMLEnums::SUPERVISOR_WORKSTATION)
    {
      if(m_uiLayoutManager->getScreenState() == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN)
      {
        viewmode = m_vdRef->getRecallDefaultRightviewModeType();
      }
      else if(m_uiLayoutManager->getScreenState() == QMLEnums::SEARCH_BAG_SCREEN)
      {
        viewmode = m_vdRef->getSearchDefaultRightviewModeType();
      }
    }
    else if(  wsType == QMLEnums::OSR_WORKSTATION &&
              (m_uiLayoutManager->getScreenState() == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN ||
               m_uiLayoutManager->getScreenState() == QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN))
    {
      viewmode = m_vdRef->getRecallDefaultRightviewModeType();
    }
  }
  return viewmode;
}

/*!
* @fn       setDefaultVRMode
* @param    None
* @return   None
* @brief    Set default right view mode
*/
void VolumeDisplayUIHelper::setDefaultVRMode()
{
  UILayoutManager *m_uiLayoutManager = UILayoutManager::getUILayoutManagerInstance();
  VREnums::VRViewModeEnum viewmode = DEFAULT_VALUE_VR_VIEW_MODE;

  if(m_uiLayoutManager != NULL)
  {
    QMLEnums::WSType wsType = WorkstationConfig::getInstance()->getWorkstationNameEnum();

    if(wsType == QMLEnums::SEARCH_WORKSTATION || wsType == QMLEnums::SUPERVISOR_WORKSTATION)
    {
      if(m_uiLayoutManager->getScreenState() == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN)
      {
        viewmode = m_vdRef->getRecallDefaultRightviewModeType();
      }
      else if(m_uiLayoutManager->getScreenState() == QMLEnums::SEARCH_BAG_SCREEN)
      {
        viewmode = m_vdRef->getSearchDefaultRightviewModeType();
      }
    }
    else if(wsType == QMLEnums::OSR_WORKSTATION &&
            ( m_uiLayoutManager->getScreenState() == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN ||
              m_uiLayoutManager->getScreenState() == QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN))
    {
      viewmode = m_vdRef->getRecallDefaultRightviewModeType();
    }
  }

  if(viewmode == VREnums::VIEW_AS_THREAT)
  {
    boost::shared_ptr<BagData> bagData = BagsDataPool::getInstance()->
        getBagbyState(BagData::RENDERING);
    if(bagData == nullptr)
    {
      ERROR_LOG("Bag data is NULL.Can not change view mode");
      return;
    }
    // Set temp VR View mode to calculate total visible entity
    bagData->setCurrentVRViewmode(viewmode);
    bagData->refreshAttributes();
    if (bagData->getTotalVisibleAlarmThreats() <= 0)
    {
      viewmode = VREnums::VIEW_AS_SURFACE;
    }
  }
  if(viewmode == VREnums::VIEW_AS_SURFACE_CAMERA)
    viewmode = VREnums::VIEW_AS_SURFACE;

  writeViewMode(viewmode);
  SetRightViewerDisplayMode();
}

}  // namespace ws
}  // namespace analogic
