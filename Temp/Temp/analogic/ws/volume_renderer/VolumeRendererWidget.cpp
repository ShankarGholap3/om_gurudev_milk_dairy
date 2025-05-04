//------------------------------------------------------------------------------
// File: VolumeRendererWidget.cpp
// Description: Implementation of the volume renderer widget class
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "VolumeRendererWidget.h"
#include "VRUtils.h"

QString g_cycleLoadLogFile = "/home/analogic/junk/CycleLoadLog.csv";
QString g_bagSizeInfoLog   = "/home/analogic/junk/BagSizeInfoLog.csv";

//------------------------------------------------------------------------------
// begin analogic::workstation namespace
//------------------------------------------------------------------------------
namespace analogic
{

namespace workstation
{

std::map<int, QString> g_genericVRMessageMap =
{
  {VOLUME_RENDERER_CAMERA_MOVED_MESSAGE_ID, "Main renderer camera moved"},
  {THREAT_RENDERER_CAMERA_MOVED_MESSAGE_ID, "Threat renderer camera moved"},
  {VOLUME_RENDERER_ZOOM_MESSAGE_ID,         "Camera zoom on renderer"},
  {THREAT_RENDERER_ZOOM_MESSAGE_ID,         "Camera zoom on threat renderer"},
  {VOLUME_RENDERER_INIT_ORIENT_MESSAGE_ID,  "Orienting the render window [x,y,z] axis"},
  {THREAT_RENDERER_INIT_ORIENT_MESSAGE_ID,  "Orienting the threat render window [x,y,z] axis"},
  {VOLUME_PICK_POINT_ID,                    "Main renderer pick point"},
  {THREAT_PICK_POINT_ID,                    "Threat renderer pick point"},

  {THREAT_STASH_SAVE_TIMER_ID,              "Stash save elapsed time (ms)"},
  {THREAT_STASH_RETRIEVE_TIMER_ID,          "Stash retrieve elapsed time (ms)"},
  {THREAT_VTK_DISPLAY_TIMER_ID,             "Threat VTK display time (ms)"},
  {MAIN_VTK_DISPLAY_TIMER_ID,               "Main VTK display time (ms)"},

  {VOL_RENDER_BASE_INITIAL_RENDER_ID,       "Volume Renderer Base initial render"},
};

const int GENERIC_SCANNER_MAX_X_DIM = 500;
const int GENERIC_SCANNER_MAX_Y_DIM = 500;
const int GENERIC_SCANNER_MAX_Z_DIM = 500;


const double GENERIC_SCANNER_PIXEL_SPACING_X =  1.0;     // MM per Pixel
const double GENERIC_SCANNER_PIXEL_SPACING_Y =  1.0;     // MM per Pixel
const double GENERIC_SCANNER_PIXEL_SPACING_Z =  0.96;    // MM per Pixel

//------------------------------------------------------------------------------



const double  VOL_MAXX = GENERIC_SCANNER_MAX_X_DIM*GENERIC_SCANNER_PIXEL_SPACING_X;
const double  VOL_MAXY = GENERIC_SCANNER_MAX_X_DIM*GENERIC_SCANNER_PIXEL_SPACING_Y;
const double  VOL_MAXZ = GENERIC_SCANNER_MAX_X_DIM*GENERIC_SCANNER_PIXEL_SPACING_Y;


const int g_vtkThreatWinWidth    = 300;
const int g_vtkThreatWinHeight   = 300;

int g_mainViewLabelX       = DEFAULT_VIEW_LABEL_POSITION_X;
int g_mainViewLabelY       = DEFAULT_VIEW_LABEL_POSITION_Y;
const int g_viewFontSize   = VIEW_TEXT_FONT_HEIGHT;

const QPoint g_viewLabelPos2(1, 1);
const QPoint g_viewLabelDisplayPos(g_mainViewLabelX,
                                   g_mainViewLabelY);

int g_rendererMarginRed   = 0;
int g_rendererMarginGreen = 0;
int g_rendererMarginBlue  = 0;


const int    g_threatFontSize = 16;
const QPoint g_threatLabelPos2(1, 1);

const int g_defaultPickVolWidth  = 250;
const int g_defaultPickVolHeight = 250;
const int g_defaultPickVolLength = 250;

//------------------------
// view-port for normal axes


double g_axesViewportMinXNormal = 0.00;
double g_axesViewportMaxXNormal = 0.12;

double g_axesViewportMinYNormal = 0.00;
double g_axesViewportMaxYNormal = 0.12;

//------------------------
// view-port for threat axes

double g_axesViewportMinXThreat = 0.65;
double g_axesViewportMaxXThreat = 0.95;

double g_axesViewportMinYThreat = 0.65;
double g_axesViewportMaxYThreat = 0.95;

double g_mainViewerInitialZoom  = 1.20;
//------------------------


//-----------------------
// Computed Zoom Scalar
double g_computedZoomScalar = 15.0;
double g_directionScaler    = 0.75;

//--------------------------
// Switch where we go from square threat to
// a more rectangular one.
double g_pickBoxApectMin  = 0.5;


// Milliseconds to redraw the threat window
int g_threatInitRedrawTimeout = 100;


// Milliseconds to redraw Splash screen on mouse/touch interaction end
int g_interactionEndSplahTimeout = 10;

// Milliseconds to redraw when the cut view changed.
int g_cutViewTimeout             = 10;



// Reduce degrading for rendering with large number of threats.
// Note: default is 15.
double g_desiredUpdateRate = 1.5;


VolumeComputeMethod g_operatorThreatVolumeMethod  =  MARCHING_CUBES;

VolumeRendererWidget* VolumeRendererWidget::s_curretnThreatPicker = nullptr;


bool VolumeRendererWidget::s_pickerActive = false;

VREnums::VRViewModeEnum VolumeRendererWidget::s_rightViewerViewMode = VREnums::VIEW_AS_THREAT;


//------------------------------------------------------------------------------
void myCallbackFunction(vtkObject* pObject,
                        uint64_t eid,
                        void* clientData,
                        void* callData)
{
  Q_UNUSED(pObject);
  Q_UNUSED(eid);
  Q_UNUSED(clientData);
  Q_UNUSED(callData);
  std::cout << "Measure Tool Callback ^^^^^ Left Mouse button press event ^^^^^^^^" << std::endl;
}


//------------------------------------------------------------------------------
VolumeRendererWidget::VolumeRendererWidget(QLabel* parent):
  OpenGLVolumeWidget(parent),
  m_parentWidget(parent),
  m_displayName(""),
  m_userDecision(""),
  m_textLabel(nullptr),
  m_viewAxisWidget(nullptr),
  m_viewRuler(nullptr),
  m_useGPU(true),
  m_enableThreats(true),
  m_enableThreatViewing(true),
  m_showThreatMass(true),
  m_threatOnlyViewStatus(false),
  m_dynamicSplash(ENABLE_DYNAMIC_SPLASH),
  m_isATipBag(false),
  m_isSlabbing(false),
  m_slabViewType(VREnums::BAG_VIEW),
  m_currentCutAxis(VREnums::AXIS::Y),
  m_slabThickness(1.0),
  m_threatList(nullptr),
  m_threatRawIndex(-1),
  m_threatTypeIndex(-1),
  m_threatTypeTotal(0),
  m_doInternalCropping(false),
  m_displayUnitSystem(VREnums::METRIC),
  m_contrastSliderVal(DEFAULT_VALUE_CONTRAST),
  m_brightnessSliderVal(DEFAULT_VALUE_BRIGHTNESS),
  m_currentOpacityValue(DEFAULT_VALUE_OPACITY),
  m_windowLevelSliderVal(0.5),
  m_colorLevelSliderVal(0.5),
  m_currentZoom(1.0),
  m_lightingAmbient(0.1),
  m_lightingDiffuse(0.7),
  m_lightingSpecular(0.2),
  m_cutPercentageY(0.0),
  m_volumeRenderer(nullptr),
  m_syncActive(false),
  m_scannerDirLefToRight(true),
  m_widgetRenderer(nullptr),
  m_imageSplashRef(nullptr),
  m_disabledViewerSplashRef(nullptr),
  m_stashCamera(nullptr),
  m_isThreatRenderer(false),
  m_isInSlabbingMode(false),
  m_dbgBagCount(0),
  m_memoryUseLogged(false),
  m_removeTimerStarted(false),
  m_debugWriteBuffer(nullptr),
  m_debugImageWidth(0),
  m_debugImageHeight(0),
  m_debugSliceCount(0),
  m_lastRightPickPoint(0, 0, 0),
  m_distanceWidget(nullptr),
  m_distanceRep(nullptr),
  m_distacneMeasurementComplete(false),
  m_sliderThrottleOn(false),
  m_prop_bagId(""),
  m_prop_DateTime(""),
  m_prop_ThreatType(""),
  m_prop_ThreatId(""),
  m_prop_RFID("")
{
  this->createVTKWindow();

  //---------------------------------------------
  // Setup pickers after we  create vtk rendererer and
  // render window.
  this->setupPointPicker();
  this->setupRubberBandPicker();


  m_stashCamera = vtkSmartPointer<vtkCamera>::New();


  this->createDistanceMeasurer();
}

//------------------------------------------------------------------------------
VolumeRendererWidget::~VolumeRendererWidget()
{
  // Clear the maps
  m_threatOffsetHsvMap.clear();
  m_threatOffsetOpacityMap.clear();
}



//------------------------------------------------------------------------------
void VolumeRendererWidget::Init(SDICOS::CTModule* ctModule,
                                uint16_t* buffer,
                                const QVector3D& volDimensions,
                                const QVector3D& roiStart,
                                const QVector3D& roiEnd,
                                std::vector<SDICOS::TDRModule*> tdrs,
                                const std::vector<std::pair<int, std::string>>& vecDisplayThreatIdentifier,
                                analogic::workstation::VolumeRendererConfig* configObject)
{
  // std::cout << "&&&&&&&&&&&&& Init &&&&&&&&&&&&&" << std::endl;

  m_dbgBagCount++;
  //------------------------------------------
  // ARO-DEBUG: More leak testing
  // This code will run the workstation without VTK rendering.
  //============================
  // emit this->initComplete();
  // return;
  //============================
  //------------------------------------------

  //-------------------------------------------------------
  // ARO-ADDED 6-27-2018 in situ testing for memory leaks.
  // this->systemMemoryUsageToLog();
  //-------------------------------------------------------

  //---------------------------------------
  // Adding Logging for timimg TDR parsing
  LOG(INFO) << "VolumeRendererWidget - VOLUME_TDR_RECEIVED - " << this->displayLabel().toStdString();
  //---------------------------------------

  //------------------------
  m_VR_ProcessFromInit = QTime::currentTime();
  //------------------------
  //------------------------
  int width     = volDimensions.x();
  int height    = volDimensions.y();
  int numSlices = volDimensions.z();

  //------------------------------------------------
  // ARO-DEBUGGING ONLY
  m_debugImageHeight = height;
  m_debugImageWidth  = width;
  //------------------------------------------------

  QVector3D c0(roiStart);
  QVector3D c1(roiEnd);
  CropCorners volCrop;
  volCrop.setBounds(c0, c1);

  this->setCropCorners(volCrop);

  QString strMessage = "**HH** Init() DIMENSION [ W x H x L]= ";
  strMessage += QString::number(width) + " x ";
  strMessage += QString::number(height) + " x ";
  strMessage += QString::number(numSlices);
  LOG(INFO) << strMessage.toLocal8Bit().data();

  strMessage = "**HH** Init() ROI START (x, y, z)= ";
  strMessage += QString::number(roiStart.x()) + ", ";
  strMessage += QString::number(roiStart.y()) + ", ";
  strMessage += QString::number(roiStart.z());
  LOG(INFO) << strMessage.toLocal8Bit().data();

  strMessage = "**HH** Init() ROI END (x, y, z)= ";
  strMessage += QString::number(roiEnd.x()) + ", ";
  strMessage += QString::number(roiEnd.y()) + ", ";
  strMessage += QString::number(roiEnd.z());
  LOG(INFO) << strMessage.toLocal8Bit().data();

  //----------------------------
  // given:
  // SDICOS::CTModule* ctBAG ID
  // Get the volume details from the CTModule
  // The 6 parameters we car about at this point:
  //
  //  1. Volume Width
  //  2. Volume Height
  //  3. Volume Depth <-????
  //
  //  4. Pixel Size X
  //  5. Pixel Size Y
  //  6. Pixel Size Z

  int imageSizeX = ctModule->GetCTImage().GetColumns();
  int imageSizeY = ctModule->GetCTImage().GetRows();

  double pixelSizeX =
      ctModule->GetMultiframeFunctionalGroups().GetSharedFunctionalGroups()->GetPixelMeasurement()->GetColumnSpacing();
  double pixelSizeY =
      ctModule->GetMultiframeFunctionalGroups().GetSharedFunctionalGroups()->GetPixelMeasurement()->GetRowSpacing();
  double pixelSizeZ =
      ctModule->GetMultiframeFunctionalGroups().GetSharedFunctionalGroups()->GetPixelMeasurement()->GetSliceThickness();

  LOG(INFO) << "Image attribute imageSize X: " << imageSizeX;
  LOG(INFO) << "Image attribute imageSize Y: " << imageSizeY;
  LOG(INFO) << "Image attribute imageSize Z: " << numSlices;

  LOG(INFO) << "Image attribute pixelSpacing X (mm): " << pixelSizeX;
  LOG(INFO) << "Image attribute pixelSpacing Y (mm): " << pixelSizeY;
  LOG(INFO) << "Image attribute pixelSpacing Z (mm): " << pixelSizeZ;

  m_volumeRenderer->setPixelSpacing(pixelSizeX,
                                    pixelSizeY,
                                    pixelSizeZ);

  //  std::cout << "VolumeRendererWidget::Init() set pixel spacing ---- " << std::endl;
  //  std::cout << "PIXEL_SPACING_X: " << pixelSizeX << ", "
  //            << "PIXEL_SPACING_Y: " << pixelSizeY << ", "
  //            << "PIXEL_SPACING_Z: " << pixelSizeZ
  //            << std::endl;
  //------------------------------------------------
  // ARO-DEBUGGING ONLY
  m_debugWriteBuffer = buffer;
  m_debugSliceCount  = numSlices;
  // this->writeVolume();
  //------------------------------------------------

  this->setGrayscaleSettings(configObject->m_grayScaleSettings);
  QTime initStartTimer = QTime::currentTime();
  this->setMaterialTransitions(configObject->m_materialTransitionMap);

  QVector3D pMin = m_cropBox.m_pMin;
  QVector3D pMax = m_cropBox.m_pMax;
  QVector3D pixelSpacing = QVector3D(pixelSizeX,
                                     pixelSizeY,
                                     pixelSizeZ);

  m_isATipBag = configObject->m_isTIPBag;


  m_enableThreats = configObject->m_isTIPBag ?
        (configObject->m_enableThreatHandling && configObject->m_showTIP)
      :configObject->m_enableThreatHandling;

  m_showThreatMass   = configObject->m_showThreatMass;

  this->initThreatList();


  if ( ( tdrs.size() > 0 ) &&
       ( !this->isAThreatRenderer() )
       )
  {
    int num_TDRmodules = tdrs.size();
    for (int  i = 0; i < num_TDRmodules; i++)
    {
      SDICOS::TDRModule* pTdr = tdrs.at(i);
      int generator_type;
      if (pTdr)
      {
        //=============================
        // Set TDR Type from tdr module
        SDICOS::TDRTypes::ThreatDetectionReport& threat_report = pTdr->GetThreatDetectionReport();
        SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();
        QString genTypeStr = "UNKNOWN";
        if (tdr_type== SDICOS::TDRTypes::ThreatDetectionReport::enumMachine)
        {
          generator_type = ATR_GEN;
          genTypeStr     = "MACHINE_GENERATED";
        }
        else if (tdr_type == SDICOS::TDRTypes::ThreatDetectionReport::enumOperator)
        {
          generator_type = OPERATOR_GEN;
          genTypeStr     = "OPERATOR_GENERATED";
        }
        else
        {
          generator_type = UNKNOWN_GEN;
          genTypeStr     = "UNLNOWN_GENERATED";
        }
        //=============================

        QString tdrLogStr = "VolumeRenderer::Init() TDR INDEX :[ ";
        tdrLogStr += QString::number(i);
        tdrLogStr += " ] - Generation Type: ";
        tdrLogStr += genTypeStr;
        LOG(INFO) << tdrLogStr.toStdString();

        int prev_size = m_threatList->size();
        this->initThreatsFromTDR(m_threatList,
                                 pTdr,
                                 pMin,
                                 pMax,
                                 pixelSpacing,
                                 vecDisplayThreatIdentifier,
                                 m_threatSignalOffsetMap,
                                 m_showThreatMass);


        // Set Threat object generator types
        int curr_size = m_threatList->size();
        for(int k = prev_size; k < curr_size; ++k)
        {
          ThreatObject* pThr = m_threatList->getThreatAtIndex(k);
          analogic::workstation::ThreatGenerationType aGenType = ATR_GEN;
          if (generator_type == 1)
          {
            aGenType = OPERATOR_GEN;
          }
          if(pThr)
          {
            pThr->setGenType(aGenType);
          }
          else
          {
            LOG(ERROR) << "ERROR m_threatList->getThreatAtIndex(k, ...) is NULL! k= " << k;
          }
        }
      }
    }
  }

  //-----------------------------------------------
  // TDR parsing completed begin VTK Rendering
  LOG(INFO) << "VolumeRendererWidget - VOLUME_VTK_RENDER_START - " << this->displayLabel().toStdString();
  //-----------------------------------------------
  m_threatList->setMachineThreatsEndbled(m_enableThreats);

  if (m_isATipBag)
  {
    LOG(INFO) << "Reciecved a TIP bag.";
    m_threatList->setAsTip();
  }

  this->setVolumeTransferFunctions(configObject->m_volumeHsvList,
                                   configObject->m_volumeOpacityList);

  this->setThreatTransferFunctions(configObject->m_volumeHsvList,
                                   configObject->m_volumeOpacityList);

  this->setLowDensityThransferFunctions(configObject->m_lowDensityHsvList,
                                        configObject->m_lowDensityOpacityList);

  // Performance Optimization related changes
  m_threatOffsetHsvMap = configObject->m_threatOffsetHsvMap;
  m_threatOffsetOpacityMap = configObject->m_threatOffsetOpacityMap;

  m_useGPU = configObject->m_useGPU;

  // We only show thrat boxes in the Left viewer
  if (this->displayLabel().contains("Left"))
  {
    this->createVTKThreatActors();
  }
  if (!m_enableThreats)
  {
    m_threatList->hideMachineThreats();
  }

  this->getTouchHandler()->adjustThreshod(configObject->m_pinchThreshold,
                                          configObject->m_spreadThreshold,
                                          configObject->m_moveThreshold);

  m_volumeRenderer->setShaderStatus(configObject->m_useShading);
  m_volumeRenderer->setLinearInterpolation(configObject->m_useLinearInterpolation);
  m_volumeRenderer->setOrganicRemovable(configObject->m_allowOrganicReomval);
  m_volumeRenderer->setInorganicRemovable(configObject->m_allowInorganicReomval);
  m_volumeRenderer->setMetalRemovable(configObject->m_allowMetalReomval);
  m_volumeRenderer->initVolumeImageAttributes(pixelSizeX,
                                              pixelSizeY,
                                              pixelSizeZ,
                                              imageSizeX,
                                              imageSizeY,
                                              numSlices);

  m_volumeRenderer->setOCBModifyScalar(configObject->m_opacityModifyScalar,
                                       configObject->m_contrastModifyScalar,
                                       configObject->m_brightnessModifyScalar);

  m_volumeRenderer->resetBuildTime();

  this->initRendererThreatOffsetMap();

  // Label info
  // 7. Bag ID
  // 8. Machine ID
  // 9. Bag Date/Time

  SDICOS::DcsString dicosBagID     = ctModule->GetObjectOfInspection().GetID().GetID();
  SDICOS::DcsString dicosMachineID = ctModule->GetGeneralEquipment().GetMachineID();
  SDICOS::DcsString dicosDate      = ctModule->GetSeries().GetDate();
  SDICOS::DcsString dicosTime      = ctModule->GetSeries().GetTime();

  // Format the Date
  std::string tmpDate = dicosDate.Get();
  std::string formattedYear  = tmpDate.substr(0, 4);
  std::string formattedMonth = tmpDate.substr(4, 2);
  std::string formattedDay   = tmpDate.substr(6, 2);
  std::string formatDate     = formattedMonth + "/" + formattedDay + "/" + formattedYear;

  // Format the TimeBAG ID
  std::string tmpTime = dicosTime.Get();
  std::string formattedHour    = tmpTime.substr(0, 2);
  std::string formattedMinute  = tmpTime.substr(2, 2);
  std::string formattedSecond  = tmpTime.substr(4, 2);
  std::string formatedTime     = formattedHour + ":" + formattedMinute + ":" + formattedSecond;
  //----------------------
  // Properties the get exposed to QT Objects;

  QString propBagID = dicosBagID.Get();
  this->setProperty(VTK_RENDERED_BAG_ID, QVariant(propBagID));
  m_prop_bagId    = propBagID;

  m_prop_DateTime =  "Date:";
  m_prop_DateTime += formatDate.c_str();
  m_prop_DateTime +=  " - ";
  m_prop_DateTime += formatedTime.c_str();
  this->setProperty(VTK_RENDERED_DATE_TIME, QVariant(m_prop_DateTime));

  QString machineSubLocation = ctModule->GetGeneralEquipment().GetMachineSubLocation().Get();

  //----------------------
  QString bagLabel = tr("Bag ID: ");
  bagLabel += propBagID;
  bagLabel += tr("\nDate: ");
  bagLabel += formatDate.c_str();
  bagLabel += tr(" - ");
  bagLabel += formatedTime.c_str();
  bagLabel += "\n";
  bagLabel += tr("Location: ");
  bagLabel += machineSubLocation;
  bagLabel += "\n";


  LOG(INFO) << "**YYBYY** BAG ID     :" << dicosBagID.Get();
  LOG(INFO) << "**YYBYY** MACHINE ID :" << dicosMachineID.Get();
  LOG(INFO) << "**YYBYY** BAG DATE   :" << dicosDate.Get();
  LOG(INFO) << "**YYBYY** BAG TIME   :" << dicosTime.Get();
  LOG(INFO) << "**YYBYY** LANE LOC   :" << machineSubLocation.toStdString();

  m_textLabel->setLabel(bagLabel);

  if (analogic::ws::WorkstationConfig::getInstance()->getWorkstationNameEnum()
      == QMLEnums::RERUN_EMULATOR)
  {
    this->update();
    QStringList rerunList;
    QString rerunbagLabel = tr("File: ");
    QString algVersion = tr("Alarm version:");
    boost::shared_ptr<analogic::ws::BagData> bagData = analogic::ws::BagsDataPool::getInstance()->
        getBagbyState(analogic::ws::BagData::RENDERING);
    if(bagData != NULL)
    {
      rerunbagLabel += bagData->getRerunBagFilename();
      algVersion += bagData->getCurrentThreatAlgorithm();
    }
    rerunList.append(rerunbagLabel);
    rerunList.append(algVersion);
    m_textLabel->moveDisplayPositionInY(-1);
    m_textLabel->moveDisplayPositionInY(-1);
    m_textLabel->insertNewText(rerunList);
  }
  this->setupVolumRendererParams();
  double volBounds[6];

  if (!m_doInternalCropping)
  {
    QVector3D p0, p1;
    p0 = QVector3D(0, 0, 0);

    double vx = pixelSizeX*imageSizeX;
    double vy = pixelSizeY*imageSizeY;
    double vz = pixelSizeZ*numSlices;

    p1 = QVector3D(vx,
                   vy,
                   vz);
    volBounds[0]= p0.x();
    volBounds[1]= p1.x();
    volBounds[2]= p0.y();
    volBounds[3]= p1.y();
    volBounds[4]= p0.z();
    volBounds[5]= p1.z();
  }
  else
  {
    QVector3D p0, p1;
    p0 = m_cropBox.m_pMin;
    p1 = m_cropBox.m_pMax;

    volBounds[0]= p0.x();
    volBounds[1]= p1.x();
    volBounds[2]= p0.y();
    volBounds[3]= p1.y();
    volBounds[4]= p0.z();
    volBounds[5]= p1.z();
  }

  int sizeBuffer = imageSizeX*imageSizeY*sizeof(uint16_t)*numSlices;
  int msInitStart = initStartTimer.elapsed();

  QString message  = "*TTTTTT* - Init Start:[Set TFs, get DICOS params, set Bounds]:" + QString::number(msInitStart);
  LOG(INFO) <<  message.toLocal8Bit().data();

  if (m_threatList)
  {
    m_volumeRenderer->setThreats(m_threatList);
  }

  m_volumeRenderer->initFromBuffer(buffer, sizeBuffer, volBounds);

  this->UpdateRenderer();

  //---------------------------------------------
  // If the TDRs only has non-threat laptops make
  // the Right Viewer a Surface Viewer and remove
  // any threat text.
  if ( m_enableThreats &&  ( !this->isAThreatRenderer() ) )
  {
    int ucM = m_threatList->unClearedMachineThreats();
    if (ucM == 0)
    {
      VolumeRendererWidget* leftViewer;
      VolumeRendererWidget* rightViewer;
      this->getLeftAndRightViewers(&leftViewer,
                                   &rightViewer);
      m_threatList->forceHideLaptops();
      rightViewer->SetAsThreatRenderer(false);
      rightViewer->removeText();
    }
  }
  //---------------------------------------------

  //---------------------------------------------
  // ARO-DEBUGGING Susopecting a Non threat TDR!
  // SDICOS::TDRModule* testTDR = tdrs.at(0);
  // this->suspectNonThreatTDR(testTDR, ctModule);
  //---------------------------------------------


  //--------------------------------------------------
  // ARO-DEBUGGING Transfer Functions
  // this->saveTransferFunctions();
  // this->saveFullMaps();
  //--------------------------------------------------
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SetSlabUnpackPosition(double value,
                                                 VREnums::AXIS axis,
                                                 bool isSlab)
{
  // std::cout << "VolumeRendererWidget::SetSlabUnpackPosition(value, axis, isSlabbing): value="
  //           << value << ", "
  //           << " axis = "
  //           << axis << ", "
  //           << " isSlabing= "
  //           << isSlab
  //           << " Viewer= "
  //           << this->displayLabel().toStdString()
  //           << std::endl;
  //------------------------------------------------------------------------
  m_isSlabbing = isSlab;
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  // * <<<=== ARO-TEST BELOW
  if (m_currentCutAxis != axis)
  {
    if(!isSlab)
    {
      this->reorientBagForCut(axis);
    }
    else
    {
      this->reorientBagForSlab(axis);
    }
    m_currentCutAxis = axis;
    // was this->forceRedraw();
    //  emit this->inputRedrawNeeded();
  }
  //-------------------------------------------
  m_volumeRenderer->setEnableSlabbing(isSlab);
  m_volumeRenderer->setPositionForSlabOrUnpacking(value, axis);
  //------------------------------------------------
  // Since we will use the Y value when picking a
  // sub-volume for a Operator threat we keep track
  // of it here.
  if (axis == VREnums::Y )
  {
    m_cutPercentageY = value;
  }
  else
  {
    m_cutPercentageY = 0.0;
  }
  //-----------------------------------------
  emit this->inputRedrawNeeded();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetSlabViewType(VREnums::SlabViewType newSetSlabType)
{
  if (this->displayLabel().contains("Right"))
  {
    // std::cout  << "****** Right Virewer returning *******"
    //            << std::endl;
    return;
  }
  if (!m_isSlabbing) return;
  m_threatList->setSurfaceViewMode(false);
  VREnums::SlabViewType oldSlabViewType = m_slabViewType;
  m_slabViewType = newSetSlabType;
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();

  //------------------------------------------
  // FIX for ANSS-1027.
  // Make sure the right viewer in in
  // the correct state first
  if(!rightViewer->isAThreatRenderer())
  {
    rightViewer->initAsThreatrenderer();
  }
  //------------------------------------------

  QString strSlabType = "SLAB VIEW set to type:";

  if (newSetSlabType == VREnums::BAG_VIEW)
  {
    strSlabType += " ** BAG ** = ";
  }
  else if (newSetSlabType == VREnums::THREAT_VIEW)
  {
    strSlabType += " ** THREAT ** = ";
  }
  else
  {
    strSlabType += " ** LAPTOP ** = ";
  }
  strSlabType += QString::number(newSetSlabType);
  LOG(INFO) << strSlabType.toStdString();
  // std::cout << strSlabType.toStdString() << std::endl;

  if (newSetSlabType == VREnums::BAG_VIEW)
  {
    m_threatList->forceHideLaptops();
    m_threatList->setSlabingLaptop(false);
    rightViewer->reorientBagForSlab(m_currentCutAxis);
    if (rightRenderer->isMsskClipped())
    {
      rightRenderer->reMapFullVolume();
      double volBounds[6];
      rightRenderer->getBounds(volBounds);
      rightRenderer->resetCameraWithBounds(volBounds);
    }
    if (oldSlabViewType == VREnums::LAPTOP_VIEW)
    {
      // std::cout << "===== SLAB-LAPTOP ==> SLAB-BAG Transition ====="
      //          << std::endl;
      ThreatObject* pThreatAvail =  m_threatList->findNextAvailbleThreatIndex();
      if (pThreatAvail)
      {
        int threatIndex = pThreatAvail->getIndex();
        m_threatList->setCurrent(threatIndex);
        boost::shared_ptr<analogic::ws::BagData> bagData = analogic::ws::BagsDataPool::getInstance()->
            getBagbyState(analogic::ws::BagData::RENDERING);
        if(bagData != NULL)
        {
          if (m_threatList->unClearedMachineThreats() > 1)
            bagData->setCurrentThreat(threatIndex-1);
          else
            bagData->setCurrentThreat(threatIndex);
        }
      }
      else
      {
       QString sError = "*** SetSlabViewType() - ERROR: m_threatList->findNextAvailbleThreatIndex(); == NULL";
       LOG(INFO) << sError.toStdString();
      }
    }
    if (rightViewer->isAThreatRenderer())
    {
      leftViewer->reDisplayCurrentThreat();
    }
    rightViewer->forceRedraw();
  }
  else if(newSetSlabType == VREnums::THREAT_VIEW)
  {
    if (oldSlabViewType == VREnums::LAPTOP_VIEW)
    {
      // std::cout << "===== SLAB-LAPTOP ==> SLAB-THREAT Transition ====="
      //            << std::endl;
      ThreatObject* pThreatAvail =  m_threatList->findNextAvailbleThreatIndex();
      if (pThreatAvail)
      {
        int threatIndex = pThreatAvail->getIndex();
        m_threatList->setCurrent(threatIndex);
        boost::shared_ptr<analogic::ws::BagData> bagData = analogic::ws::BagsDataPool::getInstance()->
            getBagbyState(analogic::ws::BagData::RENDERING);
        if(bagData != NULL)
        {
          bagData->setCurrentThreat(threatIndex);
        }
      }
      else
      {
        QString sError = "*** SetSlabViewType() - ERROR: m_threatList->findNextAvailbleThreatIndex(); == NULL";
        LOG(INFO) << sError.toStdString();
      }
    }

    leftViewer->hideLaptopROI();
    // Remove laptop masks
    if (rightRenderer->isMsskClipped())
    {
      // std::cout << "Remap Right Viewer Full Volume"
      //           << std::endl;
      rightRenderer->reMapFullVolume();
      double volBounds[6];
      rightRenderer->getBounds(volBounds);
      rightRenderer->resetCameraWithBounds(volBounds);
    }

    // set next available non laptop threat.
    rightRenderer->setCurrentThreat(DEFAULT_VALUE_CURRENT_THREAT);
    ThreatObject* pThreatAvail =  m_threatList->findNextAvailbleThreatIndex();
    if (pThreatAvail)
    {
      int threatIndex = pThreatAvail->getIndex();
      boost::shared_ptr<analogic::ws::BagData> bagData = analogic::ws::BagsDataPool::getInstance()->
          getBagbyState(analogic::ws::BagData::RENDERING);
      if(bagData != NULL)
      {
        bagData->setCurrentSlabViewType(VREnums::THREAT_VIEW);
        // Workaround to match threat mode sequence:  threatIndex-1
        if (m_threatList->unClearedMachineThreats() > 1)
          bagData->setCurrentThreat(threatIndex-1);
        else
          bagData->setCurrentThreat(threatIndex);
        m_threatList->setSlabingLaptop(false);
        rightRenderer->setCurrentThreat(threatIndex);
      }
    }
    // refresh view
    if (rightViewer->isAThreatRenderer())
    {
      leftViewer->reDisplayCurrentThreat();
    }
    rightViewer->forceRedraw();
  }

  if (this->displayLabel().contains("Left"))
  {
    if (leftViewer->CurrentThreat() >= 0)
    {
      if (m_slabViewType == VREnums::THREAT_VIEW)
      {
        leftViewer->reDisplayCurrentThreat();
      }
      if (m_slabViewType == VREnums::LAPTOP_VIEW)
      {
        m_threatList->hideMachineThreats();
        ThreatObject* pLaptop =   m_threatList->findFirstNonThreatLaptop();
        if (!pLaptop)
        {
          std::cout << "No LAPTOP FOUND" << std::endl;
          return;
        }
        //------------------------------------------
        // We need to center the threat also.
        rightViewer->slabModeRenderThreat(pLaptop, false);

        //------------------------------------------
        int threatIndex = pLaptop->getIndex();
        boost::shared_ptr<analogic::ws::BagData> bagData = analogic::ws::BagsDataPool::getInstance()->
            getBagbyState(analogic::ws::BagData::RENDERING);
        if(bagData != NULL)
        {
          bagData->setCurrentSlabViewType(VREnums::LAPTOP_VIEW);
          // Workaround to match laptop mode sequence:  threatIndex-1
          bagData->setCurrentThreat(threatIndex-1);
          rightRenderer->setCurrentThreat(threatIndex);
        }
        this->SlabNonThreatLaptop(threatIndex);
      }
    }
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetCutViewAxis(VREnums::AXIS axis)
{
  if (this->displayLabel().contains("Right")) return;
  QString msgString = "%%%%%% SetCutViewAxis(VREnums::AXIS axis)";
  if (axis == VREnums::X)
  {
    msgString += "For: - 'X AXIS'";
  }
  if (axis == VREnums::Y)
  {
    msgString += "For: - 'Y AXIS'";
  }
  if (axis == VREnums::Z)
  {
    msgString += "For: - 'Z AXIS'";
  }
  msgString += "- On Left Viewer - ";
  if (m_isSlabbing)
  {
    msgString += "- In SLAB mode";
  }
  else
  {
    msgString += "- In UNPACK mode";
  }
  LOG(INFO) << msgString.toStdString();
  // std::cout << msgString.toStdString() << std::endl;
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  VolumeRenderer* leftRenderer  = leftViewer->getVolumeRenderer();
  VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();

  leftViewer->directUpdateCutViewAxis(axis);
  rightViewer->directUpdateCutViewAxis(axis);
  if (m_imageSplashRef)
  {
    leftViewer->removeSplash();
    rightViewer->removeSplash();
  }

  if (m_isSlabbing)
  {
    leftViewer->reorientBagForSlab(axis);
    rightViewer->reorientBagForSlab(axis);
    leftRenderer->setPositionForSlabOrUnpacking(0, axis);
    rightRenderer->setPositionForSlabOrUnpacking(0, axis);
    leftViewer->forceRedraw();
    rightViewer->forceRedraw();
    this->reDisplayCurrentThreat();
  }
  else
  {
    leftViewer->reorientBagForCut(axis);
    rightViewer->reorientBagForCut(VREnums::Y);
    leftRenderer->setPositionForSlabOrUnpacking(0, axis);
    rightRenderer->setPositionForSlabOrUnpacking(0, VREnums::Y);
    leftViewer->forceRedraw();
    rightViewer->forceRedraw();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::directUpdateCutViewAxis(VREnums::AXIS axis)
{
  m_currentCutAxis = axis;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::reorientBagForCut(VREnums::AXIS axis)
{
  QString strLogMsg = "%%%% reorientBagForCut(VREnums::AXIS axis) - axis: ";
  if (axis == VREnums::X)
  {
    strLogMsg += " - X";
  }
  else if (axis == VREnums::Y)
  {
    strLogMsg += " - Y";
  }
  else if (axis == VREnums::Z)
  {
    strLogMsg += " - Z";
  }
  // std::cout << strLogMsg.toStdString() << std::endl;
  LOG(INFO) << strLogMsg.toStdString();

  if (this->isAThreatRenderer())
  {
    return;
  }

  vtkCamera* thisRenderCamera  = this->getRenderCamera();
  if (thisRenderCamera == NULL)
  {
    ERROR_LOG("Volume renderer camera is null.");
    return;
  }
  if (axis == VREnums::X)
  {
    // Camera in X so it display yz planes.
    thisRenderCamera->SetPosition(1, 0, 0);

    // Up direction is the z not the y.
    thisRenderCamera->SetViewUp(0, 1, 0);
  }
  else if (axis == VREnums::Y)
  {
    // Camera in Y so it display xz planes.
    thisRenderCamera->SetPosition(0, 1, 0);
    if (m_scannerDirLefToRight)
    {
      // Up direction is the x not the z.
      thisRenderCamera->SetViewUp(-1, 0, 0);
    }
    else
    {
      // Up direction is the x not the z.
      thisRenderCamera->SetViewUp(1, 0, 0);
    }
  }
  else if (axis == VREnums::Z)
  {
    // Camera in Z so it display xy planes.
    thisRenderCamera->SetPosition(0, 0, 1);

    // Up direction is the y not the x.
    thisRenderCamera->SetViewUp(0, 1, 0);
  }

  thisRenderCamera->SetFocalPoint(0, 0, 0);
  m_widgetRenderer->ResetCamera();
  this->initZoomIndex();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::reorientBagForSlab(VREnums::AXIS axis)
{
  vtkCamera* thisRenderCamera  = this->getRenderCamera();
  if (!thisRenderCamera)
  {
    QString errorMsg = "Error! vtkCamera is null pointer.";
    ERROR_LOG(errorMsg.toStdString());
    std::cout << errorMsg.toStdString() << std::endl;
    return;
  }
  if (this->displayLabel().contains("Left"))
  {
    // LEFT SLAB VIEW
    QString strLogMsg = "%%%%%%%%%%%%%% Reorienting viewer for SLABBIBNG on LEFT";
    if (axis == VREnums::X)
    {
      strLogMsg += " - X";
    }
    else if (axis == VREnums::Y)
    {
      strLogMsg += " - Y";
    }
    else if (axis == VREnums::Z)
    {
      strLogMsg += " - Z";
    }
    // std::cout << strLogMsg.toStdString() << std::endl;
    LOG(INFO) << strLogMsg.toStdString();
    if (axis == VREnums::Y)
    {
      if (m_scannerDirLefToRight)
      {
        thisRenderCamera->SetPosition(1, 0, 0);
        thisRenderCamera->SetViewUp(0, 1, 0);
      }
      else
      {
        // BAG SLAB LEFT VIEW OK
        thisRenderCamera->SetPosition(0, 1,  0);
        thisRenderCamera->SetFocalPoint(0, 0, 0);
        thisRenderCamera->SetViewUp(1, 0, 0);
      }
    }
    else if (axis == VREnums::Z)
    {
      // Camera in Y so it display xz planes.
      thisRenderCamera->SetPosition(0, 1, 0);
      if (m_scannerDirLefToRight)
      {
        // Up direction is the x not the z.
        thisRenderCamera->SetViewUp(-1, 0, 0);
      }
      else
      {
        // Up direction is the x not the z.
        thisRenderCamera->SetViewUp(1, 0, 0);
      }
    }
    else if (axis == VREnums::X)
    {
      // Camera in Z so it display xy planes.
      thisRenderCamera->SetPosition(0, 0, -1);

      // Up direction is the y not the x.
      thisRenderCamera->SetViewUp(0, 1, 0);
    }
  }
  else
  {
    // RIGHT SLAB VIEW AS BAG
    // std::cout << "%%%%%%%%%%%%%% Reorienting viewer for SLABBIBNG on RIGHT" << std::endl;
    if (axis == VREnums::X)
    {
      // Camera in X so it display yz planes.
      thisRenderCamera->SetPosition(1, 0, 0);

      // Up direction is the z not the y.
      thisRenderCamera->SetViewUp(0, 1, 0);
      // std::cout << "%%%%%%%%%%%%%% AXIS == X RIGHT" << std::endl;
    }
    else if (axis == VREnums::Y)
    {
      // Camera in Y so it display xz planes.
      if (m_scannerDirLefToRight)
      {
        // Up direction is the x not the z.
        thisRenderCamera->SetPosition(0, 1, 0);
        thisRenderCamera->SetViewUp(-1, 0, 0);
      }
      else
      {
        // BAG SLAB RIGHT VIEW OK
        thisRenderCamera->SetFocalPoint(0, 0, 0);
        thisRenderCamera->SetPosition(0,   1, 0);
        thisRenderCamera->SetViewUp(1,     0, 0);
      }
    }
    else if (axis == VREnums::Z)
    {
      // Camera in Z so it display xy planes.
      thisRenderCamera->SetPosition(0, 0, 1);

      // Up direction is the y not the x.
      thisRenderCamera->SetViewUp(0, 1, 0);
    }
  }

  thisRenderCamera->SetFocalPoint(0, 0, 0);
  m_widgetRenderer->ResetCamera();
  this->initZoomIndex();
}



//------------------------------------------------------------------------------
void VolumeRendererWidget::SetRightViewerDisplayMode(VREnums::VRViewModeEnum aMode)
{
  // std::cout << "**** SetRightViewerDisplayMode : "
  //           << this->displayModeString(aMode).toStdString()
  //           << std::endl;
  VREnums::VRViewModeEnum last_mode = s_rightViewerViewMode;
  if (this->displayLabel().contains("Right"))
  {
    s_rightViewerViewMode = aMode;

    VolumeRendererWidget* leftViewer;
    VolumeRendererWidget* rightViewer;
    this->getLeftAndRightViewers(&leftViewer,
                                 &rightViewer);
    VolumeRenderer* leftRenderer  = leftViewer->getVolumeRenderer();
    VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();
    this->removeSplash();

    //============================
    // SURFACE:
    if (aMode == VREnums::VIEW_AS_SURFACE)
    {
      if (rightRenderer->isLowDensityActive())
      {
        rightRenderer->setTransferfunctionsDirect(rightViewer->getNormalHsvColorList(),
                                                  rightViewer->getNormalOpacityList());
      }
      leftViewer->setSurfaceROIEnable(true);
      QTime surfaceResetTimer = QTime::currentTime();
      this->SetAsThreatRenderer(false);
      leftViewer->SetEnableLaptopRemoval(false);
      this->revertRightViewerFormThreatSettings();
      if (leftRenderer)
      {
        leftViewer->hideLaptopROI();
      }
      m_textLabel->setLabel("");
      int msSurfaceModeReset = surfaceResetTimer.elapsed();

      if (last_mode == VREnums::VIEW_AS_LAPTOP)
      {
        int curIndex = m_threatList->getCurrent();
        if (curIndex == -1)
        {
          leftRenderer->unHideMachineThreats();
        }
        // std::cout << "Make sure all threats borderrs are visible - Current = "
        //           << curIndex
        //           << std::endl;
      }

      QString message  = "*TTTTTT* - Surface Mode Image Remap timer: " + QString::number(msSurfaceModeReset);
      LOG(INFO) <<  message.toStdString();
      // std::cout << message.toStdString() << std::endl;

      emit this->viewModeEnabled(VREnums::VIEW_AS_SURFACE);
    }

    //============================
    // THREAT:
    if (aMode == VREnums::VIEW_AS_THREAT )
    {
      leftViewer->setLaptopSlabMode(false);
      rightViewer->SetAsThreatRenderer(true);
      if (last_mode == VREnums::VIEW_AS_SLAB)
      {
        m_currentCutAxis = VREnums::AXIS::Y;
        rightViewer->hardResetRendreYView();
      }
      if (leftRenderer)
      {
        leftRenderer->hideLaptopThreats();
      }
      if (leftViewer)
      {
        leftViewer->SetEnableLaptopRemoval(false);
      }
    }
    //============================
    // LAPTOP:
    if ( aMode == VREnums::VIEW_AS_LAPTOP )
    {
      rightViewer->SetAsThreatRenderer(true);
      leftViewer->SetEnableLaptopRemoval(true);
      rightRenderer->resetAndUpdateMask();
      rightViewer->clearUserDecision();
      leftViewer->disableSlabing();
    }
    //=============================
    // SLABBING:
    if (aMode == VREnums::VIEW_AS_SLAB)
    {
      // std::cout << "%%%%%%%%%%%% Mode Set to SLAB Viewer = "
      //           << this->displayLabel().toStdString()
      //           << " %%%%%%%%%%%%"
      //           << std::endl;

      // ARO-TEST:
      // Look at SLAB transition time.
      QTime slabResetTimer = QTime::currentTime();
      this->reorientBagForSlab(m_currentCutAxis);
      rightViewer->slabResetView();
      rightViewer->SetSyncEnabled(false);
      leftViewer->SetSyncEnabled(false);
      // Agiliad: Set laptop removal false for slab mode
      leftViewer->SetEnableLaptopRemoval(false);
      rightRenderer->setUpForBagSlabbing();
      leftRenderer->showSlabCube();

      //-------------------------------------
      // ARO: Note if we enter the slab mode
      // after unpacking we need to reset the
      // unpacking position back to zero.
      // Here we just use the defaults.
      VREnums::AXIS axis = VREnums::Y;
      leftViewer->SetSlabUnpackPosition(0, axis, false);
      //-------------------------------------

      int msSlabModeReset = slabResetTimer.elapsed();

      QString message  = "*TTTTTT* - Slab Mode Image Remap timer: " + QString::number(msSlabModeReset);
      LOG(INFO) <<  message.toStdString();
      // std::cout << message.toStdString() << std::endl;

      //-----------------------------------
      // ARO-NOTE: 11-7-2019
      // Make this call so the transition
      // from LAPTOP view to SLAB view
      // works as expected.
      int cleared = m_threatList->clearedMachineThreats();

      if ( (cleared== 0)||
           (m_threatList->size() == 0)
           )
      {
        rightRenderer->removeLabelMaskColors();

        std::list<HSVColorSigVal> currentHSV;
        std::list<OpacitySigVal>  currentOpacity;
        leftViewer->getCurrentTransferFunctions(currentHSV, currentOpacity);

        std::list<OpacitySigVal>  applyOpacityList;
        rightRenderer->increaseOpacityOnThinSlice(currentOpacity, applyOpacityList);
        rightRenderer->setTransferfunctionsInderect(currentHSV,
                                                    applyOpacityList);
        double ambient;
        double diffuse;
        double specular;
        leftRenderer->getLightingParams(ambient,
                                        diffuse,
                                        specular);

        rightRenderer->setLightingParams(ambient,
                                         diffuse,
                                         specular);

        //----------------------------------------------------------------------
        // Special case where we were in SLAB-LAPTOP mode
        if (last_mode == VREnums::VIEW_AS_LAPTOP)
        {
          ThreatList* threatList = leftRenderer->getThreats();
          ThreatObject* pThreatAvail =  threatList->findNextAvailbleThreatIndex();
          if (pThreatAvail)
          {
            int threatIndex = pThreatAvail->getIndex();
            threatList->setCurrent(threatIndex);
            boost::shared_ptr<analogic::ws::BagData> bagData =
                analogic::ws::BagsDataPool::getInstance()->getBagbyState(analogic::ws::BagData::RENDERING);
            if(bagData != NULL)
            {
              bagData->setCurrentThreat(threatIndex);
            }
          }
          // refresh view
          if (rightViewer->isAThreatRenderer())
          {
            leftViewer->reDisplayCurrentThreat();
          }
          rightViewer->forceRedraw();
        }
        //----------------------------------------------------------------------
      }
      else
      {
        rightRenderer->setMaskTypeToLabel();
        rightRenderer->setMaskBitsToValue(0);
      }
      m_isInSlabbingMode = true;
    }
    else
    {
      rightRenderer->hideSlabCube();
      leftRenderer->hideSlabCube();

      m_isInSlabbingMode = false;
    }
    //----------------------------------------------
    // Swapping modes might change the transfer
    //   function of the left viewer. For example
    //   LAPTOP to SLAB.  Here we reset it.
    leftViewer->resetRendererTransferFunctions();
    //---------------------------
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SetVTKDisplayUnit(VREnums::DispalyUnitEnum dispUnit)
{
  Q_UNUSED(dispUnit);
  // std::cout << "&&&&&&&&&&&&& SetVTKDisplayUnit &&&&&&&&&&&&&" << std::endl;
  // No longer used.
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetVTKDisplayUnitSystem(VREnums::DispalyUnitSystemEnum unitSystem)
{
  m_displayUnitSystem  = unitSystem;
  if (  m_displayUnitSystem  == VREnums::METRIC)
  {
    m_viewRuler->setRulerUnitToCentimeter();
    m_volumeRenderer->setDisplayUnitToCentimeter();
    this->setDistanceMeasureUnit(true);
  }
  else
  {
    m_viewRuler->setRulerUnitToInch();
    m_volumeRenderer->setDisplayUnitToInch();
    this->setDistanceMeasureUnit(false);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::DeleteOperatorThreat(int threatIndex)
{
  if (this->displayLabel().contains("Right")) return;
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  if (!m_threatList) return;
  if (threatIndex == -1)
  {
    // Bug-fix: workaround - ANSS-678.
    // The DeletOperatorThreat "Button" should be disabled until the treat TYPE
    // is fully selected.
    return;
  }
  // std::cout << "*** DeleteOperatorThreat(index) - index[ "
  //           << threatIndex
  //           << " ] using clearOperatorThreat(index)" << std::endl;

  m_volumeRenderer->clearOperatorThreat(threatIndex);
  if (this->displayLabel().contains("Left"))
  {
    emit this->interactorActivityEnd();
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::RepaintViewers()
{
  //------------------------------------------------------------
  // std::cout << "&&&&&&&&&&&&& RepaintViewers &&&&&&&&&&&&&"
  //           << std::endl;
  // VolumeRendererWidget* leftViewer;
  // VolumeRendererWidget* rightViewer;
  // this->getLeftAndRightViewers(&leftViewer,
  //                             &rightViewer);
  // emit leftViewer->inputRedrawNeeded();
  // emit rightViewer->inputRedrawNeeded();
  //-------------------------------------------------------------
  if (!m_volumeRenderer->isThreaMaskingAllowed()) return;
  QTimer::singleShot(g_interactionEndSplahTimeout,
                     this,
                     &VolumeRendererWidget::onIntractionEndTimeout);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetContrast(float value)
{
  //  std::cout << "&&&&&&&&&&&&& SetContrast &&&&&&&&&&&&&" << std::endl;
  if ( (value != DEFAULT_VALUE_CONTRAST) &&
       (m_imageSplashRef)
       )
  {
    this->removeSplash();
  }

  m_contrastSliderVal = value;
  m_volumeRenderer->setOpacityValueLevel(m_contrastSliderVal*100);
  this->update();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SetBrightness(float value)
{
  // std::cout << "&&&&&&&&&&&&& SetBrightness &&&&&&&&&&&&&" << std::endl;
  if ( (value != DEFAULT_VALUE_BRIGHTNESS) &&
       (m_imageSplashRef)
       )
  {
    this->removeSplash();
  }
  m_brightnessSliderVal = value;
  m_volumeRenderer->brightenColor(m_brightnessSliderVal*100);
  this->update();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SetOpacity(float value)
{
  // std::cout << "&&&&&&&&&&&&& SetOpacity &&&&&&&&&&&&&" << std::endl;
  if ( (value != m_currentOpacityValue) &&
       (m_imageSplashRef)
       )
  {
    this->removeSplash();
  }

  m_currentOpacityValue = value;
  m_volumeRenderer->setOpacitySignalLevel(m_currentOpacityValue*100);
  this->forceRedraw();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SetInverse(bool value)
{
  // std::cout << "&&&&&&&&&&&&& SetInverse &&&&&&&&&&&&&" << std::endl;
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  if (value)
  {
    m_volumeRenderer->setBgInverse();
    if (!this->isAThreatRenderer())
    {
      m_volumeRenderer->resetTransferFunctions();
    }
  }
  else
  {
    m_volumeRenderer->setBgNormal();
    if (!this->isAThreatRenderer())
    {
      m_volumeRenderer->resetTransferFunctions();
    }
  }
  this->forceRedraw();
  this->onIntractionEndTimeout();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::resetRendererTransferFunctions()
{
  if (!this->isAThreatRenderer())
  {
    m_volumeRenderer->resetTransferFunctions();
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::RenderWithColor(bool doColor)
{
  // std::cout << "&&&&&&&&&&&&& RenderWithColor(doColor) - doColor = "
  //           << doColor
  //           << " &&&&&&&&&&&&&"
  //           << std::endl;
  if ( m_imageSplashRef )
  {
    this->removeSplash();
  }
  m_volumeRenderer->setRenderWithColor(doColor);

  if (!this->isAThreatRenderer())
  {
    m_volumeRenderer->resetTransferFunctions();
  }
  if (this->isAThreatRenderer())
  {
    VolumeRendererWidget* leftViewer;
    VolumeRendererWidget* rightViewer;
    this->getLeftAndRightViewers(&leftViewer,
                                 &rightViewer);

    int lv_unclear = leftViewer->unclearedMachineThreats();
    if( lv_unclear > 0)
    {
      leftViewer->reDisplayCurrentThreat();
    }
    else
    {
      if (m_isSlabbing)
      {
        std::list<OpacitySigVal>  applyOpacityList;
        VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();
        rightRenderer->increaseOpacityOnThinSlice(leftViewer->getLowDensityOpacityList(), applyOpacityList);
        m_volumeRenderer->setTransferfunctionsInderect(leftViewer->getLowDensityHsvColorList(),
                                                       applyOpacityList);
      }
    }
  }
  this->forceRedraw();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetHighlightedMaterials(std::vector<MaterialEnumeration> list)
{
  // std::cout << "&&&&&&&&&&&&& SetHighlightedMaterials &&&&&&&&&&&&&" << std::endl;
  if ( m_imageSplashRef )
  {
    this->removeSplash();
  }
  std::vector<MaterialEnumeration>::iterator iter;
  bool doOrganic   = false;
  bool doInOrganic = false;
  bool doMetal     = false;
  for (iter = list.begin();  iter != list.end(); iter++)
  {
    MaterialEnumeration me = *iter;
    if (me == ORGANIC)   doOrganic   = true;
    if (me == INORGANIC) doInOrganic = true;
    if (me == METAL)     doMetal     = true;
  }
  m_volumeRenderer->setMaterialColorize(doOrganic, doInOrganic, doMetal);

  if (!this->isAThreatRenderer())
  {
    m_volumeRenderer->updateTransferFunctions();
  }
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  if(m_displayName.contains("Right"))
  {
    if (rightViewer->isAThreatRenderer())
    {
      int uc_mach = leftViewer->unclearedMachineThreats();
      if (uc_mach > 0)
      {
        leftViewer->reDisplayCurrentThreat();
      }
      else
      {
        if (m_isSlabbing)
        {
          std::list<OpacitySigVal>  applyOpacityList;
          VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();
          rightRenderer->increaseOpacityOnThinSlice(leftViewer->getLowDensityOpacityList(), applyOpacityList);
          m_volumeRenderer->setTransferfunctionsInderect(leftViewer->getLowDensityHsvColorList(),
                                                         applyOpacityList);
        }
      }
    }
  }
  emit this->inputRedrawNeeded();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetMaterialFilterCutoff(double material_cutoff)
{
  if (m_sliderThrottleOn)
  {
    std::cout << "%%%%%% SetMaterialFilterCutoff() Throttle ON returning ..."
              << std::endl;
    return;
  }

  m_sliderThrottleOn = true;
  QTime timeDensity = QTime::currentTime();

  QString timeStr  = QString::number(timeDensity.msecsSinceStartOfDay());

  if ( m_imageSplashRef )
  {
    this->removeSplash();
  }
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  VolumeRenderer* rightVR = rightViewer->getVolumeRenderer();
  VolumeRenderer* leftVR  = leftViewer->getVolumeRenderer();

  leftVR->setMaterialHighPassCutoff(material_cutoff);
  leftVR->updateTransferFunctions();
  rightVR->setMaterialHighPassCutoff(material_cutoff);
  int unCleared = m_threatList->unClearedMachineThreats();

  if ( (m_isSlabbing) && (unCleared == 0 ) )
  {
    std::list<HSVColorSigVal> curHSVList;
    std::list<OpacitySigVal>  curOpacityList;

    leftViewer->getCurrentTransferFunctions(curHSVList,
                                            curOpacityList);
    std::list<OpacitySigVal> applyNormalOpacityhList;
    rightVR->increaseOpacityOnThinSlice(curOpacityList,
                                        applyNormalOpacityhList);

    rightVR->updateRelativeTransferFunctions(curHSVList,
                                             applyNormalOpacityhList);
  }
  else
  {
    rightVR->updateTransferFunctions();
  }
  if (rightViewer->isAThreatRenderer())
  {
    leftViewer->reDisplayCurrentThreat();
  }
  leftViewer->forceRedraw();
  rightViewer->forceRedraw();

  LOG(INFO) << "%>>><<<% SetMaterialFilterCutoff() Start Time [ms from MIDNIGHT]:["
            << timeStr.toStdString()
            << "] Elapsed (ms):["
            << timeDensity.elapsed()
            << "] Set Val:["
            << material_cutoff
            << "]";

  m_sliderThrottleOn = false;
}

//------------------------------------------------------------------------------
bool VolumeRendererWidget::GetMaterialFilterActivityStatus()
{
  return m_sliderThrottleOn;
}

//------------------------------------------------------------------------------`
void VolumeRendererWidget::SetMaterialFilterActivityStatus(bool setVal)
{
  m_sliderThrottleOn = setVal;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::EnableLowDensity(bool setVal)
{
  // std::cout << "&&&&&&&&&&&&& EnableLowDensity(setVall) - setVl = "
  //           << setVal
  //           << " &&&&&&&&&&&&&"
  //           << std::endl;
  if ( m_imageSplashRef )
  {
    this->removeSplash();
  }
  if (m_displayName.contains("Left"))
  {
    if (setVal)
    {
      m_volumeRenderer->setLowDensityActive(setVal);
      m_volumeRenderer->setTransferfunctionsDirect(m_colorMapLowDensity,
                                                   m_opacityMapLowDensityl);
    }
    else
    {
      m_volumeRenderer->setLowDensityActive(setVal);
      m_volumeRenderer->setTransferfunctionsDirect(m_colorMapNormal,
                                                   m_opacityMapNormal);
    }
    this->forceRedraw();
  }
  else
  {
    this->rightAapplyLowDensity(setVal);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetZoom(float value)
{
  // std::cout << "&&&&&&&&&&&&& SetZoom:[" << value << "] &&&&&&&&&&&&&" << std::endl;
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  this->localZoom( value);
  // SYNC Zoom with sibling.
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);

  //----------------------------------------------
  // ARO -  6-3-2019.
  // Since all QML calls
  // to this come through the LEFT Viewer we need
  // to clear the splash if present in the right
  // viewer.
  rightViewer->removeSplash();
  //----------------------------------------------

  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
  if (s_rightViewerViewMode == VREnums::VRViewModeEnum::VIEW_AS_SURFACE)
  {
    // If right viewer is a Surface then zoom by cloning the camera
    this->syncSibling();
    firstSib->siblingSetZoomValue(value);
  }
  else
  {
    firstSib->localZoom(value);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetThreatVisibility(SDICOS::TDRModule *pTdr,
                                               int  rawIndex,
                                               bool visible,
                                               bool keepOrientation,
                                               int  threatTypeIndex,
                                               int  threatTypeTotal)
{
  QTime threatDisplayTimer = QTime::currentTime();
  Q_UNUSED(pTdr);
  Q_UNUSED(visible);
  m_volumeRenderer->removeCenteringSphereIfPresent();
  if (!m_volumeInitialized)
  {
    std::cout << "%>>>><<<<% SetThreatVisibility()  called before volume initialized" << std::endl;
    return;
  }
  //---------------------------------------------------
  // ARO-XXXX Start from here!
  // TODO(aoughton) Dec-13-2018 USER DECISION IN VTK WINDOW!
  //
  //---------------------------------------------------
  // std::cout << "*** SetThreatVisibility(tdr,rawIndex,visible,keepOrioentation,theratIndex,threatTypeTotal)"
  //           << std::endl;
  // std::cout << "Display Label: "
  //           << this->displayLabel().toStdString()
  //           << std::endl;
  // std::cout << "pTdr            : "  << (unsigned long)pTdr << std::endl;
  // std::cout << "rawIndex        : "  << rawIndex            << std::endl;
  // std::cout << "visiblle        : "  << visible             << std::endl;
  // std::cout << "keepOrientation : "  << keepOrientation     << std::endl;
  // std::cout << "threatTypeIndex : "  << threatTypeIndex     << std::endl;
  // std::cout << "threatTypeTotal : "  << threatTypeTotal     << std::endl;
  // std::cout << "## m_isSlabbing : "  << m_isSlabbing        << std::endl;
  //------------------------------------------------------------------------
  // ARO-NOTE Jan-2018: Right Widget Remove splash
  if ( m_imageSplashRef)
  {
    this->removeSplash();
  }
  //------------------------------------------------------------------------
  if (!m_threatList)
  {
    QString nullListMsg = "m_threatList is a NULL Pointer - SetThreatVisibility()";
    LOG(ERROR) << nullListMsg.toLocal8Bit().data();
    std::cout  << nullListMsg.toLocal8Bit().data() << std::endl;
    return;
  }

  if (m_threatList->size() == 0)
  {
    QString emptyListMsg = "Note: m_threatList is EMPTY!";
    LOG(INFO) << emptyListMsg.toLocal8Bit().data();
    // std::cout << emptyListMsg.toLocal8Bit().data() << std::endl;
    return;
  }

  m_threatRawIndex  = rawIndex;
  m_threatTypeIndex = threatTypeIndex;
  m_threatTypeTotal = threatTypeTotal;

  ThreatObject* pThr = m_threatList->getThreatAtIndex(rawIndex);
  if(!pThr)
  {
    LOG(INFO) << "NULL threat found at index: " << rawIndex;

    // std::cout << "NULL threat found at index: "
    //           << rawIndex
    //           << std::endl;
    return;
  }
  //-----------------------------------------------------------------------------
  // ARO-DEBUG
  //   ThreatVolume tv = pThr->getVolume();
  //   std::cout << "SetThreatVisibility () Threat Volume P0[x,y,z] - P1[x,y,z] :["
  //            << tv.p0().x() << ", "
  //            << tv.p0().y() << ", "
  //            << tv.p0().z() << "] - ["
  //            << tv.p1().x() << ", "
  //            << tv.p1().y() << ", "
  //            << tv.p1().z() << "]";
  //-----------------------------------------------------------------------------


  //----------------------------------------------------------
  // If threat is of type OPERATOR Generated
  // we need to turn on the colors as we won't have a bitmap.
  bool operator_generated = false;
  if (pThr->getGenType() == OPERATOR_GEN)
  {
    operator_generated = true;
  }
  else
  {
    operator_generated = false;
  }
  emit operatorThreatSelected(operator_generated);

  SDICOS::Bitmap* pBMP = pThr->getBMP();


  //------------------------------------------------------
  // ARO-NOTE Jan-2018: Now we use only the right viewer
  // to show the threat.
  //
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);

  //------------------------------------------------------
  if (m_isATipBag)
  {
    m_threatList->setMachineThreatsEndbled(true);
  }
  //------------------------------------------------------

  if (rightViewer->isAThreatRenderer())
  {
    this->showThreatByMode(pThr,
                           pBMP,
                           rawIndex,
                           threatTypeIndex,
                           threatTypeTotal,
                           operator_generated,
                           keepOrientation);
  }
  //------------------------------------------------------
  // Show the Threat in the Left Viewer
  this->leftViewerShowThreatAtIndex(rawIndex);
  //------------------------------------------------------

  QString currentThreatText = m_volumeRenderer->currentThreatText();
  this->setProperty(VTK_RENDERED_THREAT_TEXT, QVariant(currentThreatText));
  m_prop_ThreatType = currentThreatText;

  QString threatDispalyTimeStr = "%>>><<<% Display time for SetThreatVisibility() - (ms):"
      + QString::number(threatDisplayTimer.elapsed());
  // std::cout << threatDispalyTimeStr.toStdString() << std::endl;
  LOG(INFO) << threatDispalyTimeStr.toStdString();

  //-----------------------------------------------
  // !!!!!! ONLY FOR TESTING !!!!!
  // COMMENT FOR RELEASED CODE!!!!!!
  // ARO-NOTE: if we want to properly catch
  //  threat data in any memory logging we should
  //  log the memory here instead of in the maine
  //  VolumeRendererWidget::Init() method.
  //
  //  Also since this methid gets called ALLOT we should
  //  set a flag that it has been called that gets cleared
  //  when we clear the bag.
  // if  (!m_memoryUseLogged)
  // {
  //   this->logMemoryUsage();
  //   m_memoryUseLogged = true;
  // }
  //
  //-------------------------------------------------
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::ResetParameters()
{
  // std::cout << "&&&&&&&&&&&&& ResetParameters &&&&&&&&&&&&&" << std::endl;
  m_volumeRenderer->resetRenderSettings();
  this->update();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::ResetDisplay(bool dosync)
{
  // std::cout << "&&&&&&&&&&&&& ResetDisplay &&&&&&&&&&&&&" << std::endl;
  //---------------------------
  // ARO-DEBUG:
  // this->showThreatInfo();
  //---------------------------
  Q_UNUSED(dosync);
  this->resetMainRendererViewAlignment();
  m_currentZoom = VTK_DEFAULT_ZOOM;
  if  ( this->isSplashEenabled() )
  {
    this->showVtkSplashScreenNew();
  }
  //----------------------------------------
  // ARO-Added 3-5-2020
  // Since the distance measuremt tool is
  // disabled here we need to keep synched.
  this->disableMeasurementWidget();
  //----------------------------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::ClearCurrentThreat()
{
  // std::cout << "&&&&&&&&&&&&& ClearCurrentThreat &&&&&&&&&&&&&" << std::endl;
  //----------------------------------------
  // Clear splash screen if present.
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  rightViewer->RemoveSplashIfPresent();
  leftViewer->RemoveSplashIfPresent();
  //-----------------------------------------

  if (m_threatRawIndex < 0) return;

  //--------------------------------------------------------
  // ARO-NOTE: The m_threatList is passed as a pointer to
  // m_volumeRenderer no need to modify it here directly on
  // clearMachineThreat().
  m_volumeRenderer->clearMachineThreat(m_threatRawIndex);
  //--------------------------------------------------------

  m_threatRawIndex = -1;

  //---------------------------------
  // Reset mask and redraw threat
  QTime clearRedrawTimer = QTime::currentTime();
  VolumeRenderer* leftRenderer = leftViewer->getVolumeRenderer();
  leftRenderer->resetAndUpdateMask();
  leftViewer->reDisplayCurrentThreat();
  int clrRedrawTime_ms = clearRedrawTimer.elapsed();
  QString message = "*** CLEAR-REDRAW Elapsed time (ms): "  + QString::number(clrRedrawTime_ms);
  LOG(INFO) << message.toStdString();
  // std::cout << message.toStdString()
  //           << std::endl;
  //---------------------------------

  int cleared = m_threatList->clearedMachineThreats();
  if (cleared == m_threatList->size())
  {
    m_volumeRenderer->resetAndUpdateMask();
    this->forceRedraw();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::RemoveCurrentBag()
{
  // std::cout << "*** RemoveCurrentBag()" << std::endl;
  this->internalClearCurrentThreat();
  m_volumeRenderer->clearAllThreats();
  m_volumeRenderer->setRemoveLaptop(false);
  m_volumeRenderer->removeVolume();
  m_volumeRenderer->setEnableSlabbing(false);
  m_volumeRenderer->allowThreatMask(false);
  // --------------------
  // Remove volume measurement related items if present.
  m_volumeRenderer->removeCenteringSphereIfPresent();
  m_volumeRenderer->removeMeasuredVolume();
  m_volumeRenderer->removeMeasuredText();
  //--------------------------------------------------------------------

  m_volumeRenderer->resetClipIndex();
  m_isSlabbing       = false;
  m_isInSlabbingMode = false;


  if (this->displayLabel().contains("Right"))
  {
    this->SetAsThreatRenderer(true);
  }

  this->disableMeasurementWidget();
  //  std::cout << "&&&&&&&&&&&&& DISARMING VOLUME MEASUREMENT &&&&&&&&&&&&&" << std::endl;
  this->armForVolumeMeasurement(false);


  m_textLabel->setLabel("");
  m_volumeRenderer->setShowThreatOnlyValue(false);
  this->clearPaintCount();
  this->setVolumeInitialized(false);
  VolumeRendererWidget::setPickerActive(false);
  this->removeDisabledSplash();
  this->setEnableInteraction(true);
  m_memoryUseLogged = false;
  m_threatList->setAsTipFalse();
  m_threatList->setSlabingLaptop(false);
  m_VR_ProcessTimerFullLoad =  QTime::currentTime();
  m_removeTimerStarted = true;
  LOG(INFO) << "REMOVE_CURRENT_BAG";
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::RemoveSplashIfPresent()
{
  // std::cout << "&&&&&&&&&&&&& RemoveSplashIfPresent &&&&&&&&&&&&&" << std::endl;
  if(m_imageSplashRef)
  {
    this->removeSplash();
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SuspectCurrentThreat(int threatTypeIndex,
                                                int threatTypeTotal)
{
  // std::cout << "&&&&&&&&&&&&& SuspectCurrentThreat &&&&&&&&&&&&&" << std::endl;
  ThreatObject* pThreat = m_threatList->getThreatAtIndex(threatTypeIndex);
  if (pThreat)
  {
    vtkActor* threatActor  =  pThreat->getActor();
    if (threatActor)
    {
      threatActor->GetProperty()->SetColor(SUSPECTED_MACHINE_THREAT_RED,
                                           SUSPECTED_MACHINE_THREAT_GREEN,
                                           SUSPECTED_MACHINE_THREAT_BLUE);
    }
  }

  m_threatList->suspectThreatAt(threatTypeIndex);
  this->SetThreatVisibility(nullptr,
                            threatTypeIndex,
                            true,
                            true,
                            threatTypeIndex,
                            threatTypeTotal);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SetLinkSiblingsList(std::list<VolumeRendererWidget*> sibList)
{
  // std::cout << "&&&&&&&&&&&&& SetLinkSiblingsList &&&&&&&&&&&&&" << std::endl;
  std::list<VolumeRendererWidget*>::iterator itL;
  for (itL = sibList.begin(); itL != sibList.end();  itL++)
  {
    m_renderSiblingsList.push_back(*itL);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetSyncEnabled(bool enableStatus)
{
  // std::cout << "&&&&&&&&&&&&& SetSyncEnabled &&&&&&&&&&&&&" << std::endl;
  m_syncActive =  enableStatus;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetScannerDirectionLeftToRight(bool scanLeftToRight)
{
  m_scannerDirLefToRight = scanLeftToRight;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::DisplayThreatOnly(bool  showBMPOnly)
{
  if (!m_volumeInitialized) return;
  // std::cout << "&&&&&&&&&&&&& DisplayThreatOnly &&&&&&&&&&&&&: " << this->displayLabel().toStdString() << std::endl;
  // Note: on threat only
  // 1. Should be called from Left Viewer ONLY.
  // 2. m_threatList is a valid pointer
  // 2. The Left viewer must have #threats  > 0.
  if (this->displayLabel().contains("Right")) return;
  if (!m_threatList) return;

  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  int   numThreats = leftViewer->ThreatCount();
  if (numThreats == 0) return;

  if (m_imageSplashRef)
  {
    this->removeSplash();
  }

  int curThreat = leftViewer->CurrentThreat();

  // If the values are the same do nothing, just re-render
  if (showBMPOnly == m_threatOnlyViewStatus)
  {
    if (curThreat >= 0 )
    {
      // ARO-TODO:-Stash relative INDEX from Set Tnhreat Visibility
      // std::cout << " DisplayThreatOnly calls SetThreatVisibility() - BITMAP ONLY" << std::endl;
      this->SetThreatVisibility(nullptr,
                                curThreat,
                                true,
                                true,
                                m_threatTypeIndex,
                                m_threatTypeTotal);
    }
  }
  else
  {
    // OK the values are different change the internal one and re-render the scene.
    VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();
    m_threatOnlyViewStatus = showBMPOnly;
    m_volumeRenderer->setShowThreatOnlyValue(showBMPOnly);
    rightRenderer->setShowThreatOnlyValue(showBMPOnly);
    if (curThreat >= 0 )
    {
      // ARO-TODO:-Stash relative INDEX from Set Tnhreat Visibility
      // std::cout << " &&&&&& DisplayThreatOnly calls SetThreatVisibility() - BITMAP AND NORMAL " << std::endl;
      leftViewer->SetThreatVisibility(nullptr,
                                      curThreat,
                                      true,
                                      true,
                                      m_threatTypeIndex,
                                      m_threatTypeTotal);
    }
  }
  this->update();
}



//----------------------------------------------------------------------
void VolumeRendererWidget::StartOperatorThreatGeneration()
{
  // std::cout << "&&&&&&&&&&&&& StartOperatorThreatGeneration &&&&&&&&&&&&&" << std::endl;
  if (this->unclearedMachineThreats() > 0) return;
  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
  this->armRubberBand(true);
  firstSib->armRubberBand(true);
}

//------------------------------------------------------------------------------
int VolumeRendererWidget::RightAcceptOperatorGeneratedThreat(QString& threatType,
                                                             QVector3D& p0Ret,
                                                             QVector3D& extentRet)
{
  // std::cout << "&&&&&&&&&&&&& RightAcceptOperatorGeneratedThreat &&&&&&&&&&&&&" << std::endl;
  QTime acceptOT_Timer = QTime::currentTime();

  // First we needto make sure we usethe Left viewer to drive the change.
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);

  VolumeRenderer* leftRenderer = leftViewer->getVolumeRenderer();

  //-------------------------------------------
  // if pick Box not selected return cleanly
  bool pickSelectTest = VolumeRendererWidget::pickerActive();
  if (!pickSelectTest)
  {
    //-------------------------------
    // Disarm pick widget.
    leftViewer->armRubberBand(false);
    rightViewer->armRubberBand(false);
    return -1;
  }

  //------------------------------------------------------
  // ARO-NOTE Jan-2018: I need to remove splash of right
  // viewer if present.
  rightViewer->RemoveSplashIfPresent();
  leftViewer->RemoveSplashIfPresent();
  //------------------------------------------------------

  analogic::workstation::VolumeRendererWidget* activeWidget =
      analogic::workstation::VolumeRendererWidget::currentThreatPicker();
  if (activeWidget == nullptr)
  {
    qDebug() << "ERROR ACTIVE THREAT PICKING WIDGET IS NULL!!!";
    activeWidget = this;
  }
  QVector3D imageP0;
  QVector3D imageP1;
  activeWidget->getUserThreatRange(imageP0, imageP1);
  QVector3D p0;
  QVector3D p1;

  this->imageToWorldVolumeRange(imageP0,
                                imageP1,
                                p0,
                                p1);





  double tx = p1.x() - p0.x();
  double ty = p1.y() - p0.y();
  double tz = p1.z() - p0.z();

  double W = fabs(tx);
  double H = fabs(ty);
  double L = fabs(tz);

  //-----------------------------------------------------
  // If cut plane slider near end of travel
  if (W < OPERATOR_THREAT_MIN_SIZE_PIXELS )
  {
    W = OPERATOR_THREAT_MIN_SIZE_PIXELS;
    p1.setX(p0.x() + W);
  }
  if (H < OPERATOR_THREAT_MIN_SIZE_PIXELS )
  {
    std::cout << "P0 Y=" << p0.y() << std::endl;
    H = OPERATOR_THREAT_MIN_SIZE_PIXELS;
    p0.setY(p0.y() - H);
    p1.setY(p0.y() + H);
  }
  if (L < OPERATOR_THREAT_MIN_SIZE_PIXELS  )
  {
    L = OPERATOR_THREAT_MIN_SIZE_PIXELS;
    p1.setZ(p0.z() + L);
  }
  //-----------------------------------------------------

  leftViewer->setPickerActive(false);
  rightViewer->setPickerActive(false);

  leftViewer->hideSubVolume();
  rightViewer->hideSubVolume();


  //---------------------------------------------------
  // ARO-Added 5-30-2029 - Final check of Z for thin
  // bags - (bags with few slices):

  double imageRange[6];

  // Order of bounds index:
  // (xmin,xmax, ymin,ymax, zmin,zmax).
  leftRenderer->getImageBounds(imageRange);
  double rangeL = imageRange[5] - imageRange[4];


  if (L > rangeL)
  {
    L = 0.8*rangeL;
    p0.setX(imageRange[0]);
    p0.setY(imageRange[2]);
    p0.setZ(imageRange[4]);
    p1.setX(p0.x() + W);
    p1.setY(p0.y() + H);
    p1.setZ(p0.z() + L);
  }
  //---------------------------------------------------


  ThreatVolume newTV(p0,
                     p1);

  //-----------------------------------------------------
  // ARO-NOTE: The difference between RetP0 (RetP0 - extent/2) is the,
  // m_cropBox.m_pMin
  // This is because the return values assume the data is not flipped (shifted in Y)
  // but it is for rendering.
  int threatIndex = leftViewer->addOperatorThreatObject(newTV,
                                                        threatType);
  leftViewer->vtkWidgetUpdate();

  int threatCount = leftViewer->ThreatCount();

  std::list<OpacitySigVal>  curOpacityList;
  std::list<HSVColorSigVal> curHsvColorList;

  leftViewer->getNormalOpacityTransferFunction(curOpacityList);
  leftViewer->getNormalHSVTransferFunction(curHsvColorList);

  double ambient;
  double diffuse;
  double specular;
  leftRenderer->getLightingParams(ambient, diffuse, specular);

  leftViewer->SetSyncEnabled(false);
  rightViewer->SetSyncEnabled(false);

  // SDICOS::Bitmap* pBMP = nullptr;




  QVector3D volExtent(W, H, L);

  // Now the Right Viewer is a Threat Renderer
  rightViewer->SetAsThreatRenderer(true);
  rightViewer->setOperatorThreatLabel(threatIndex,
                                      threatCount,
                                      threatType);

  //-------------------------------
  // Disarm pick widget.
  leftViewer->armRubberBand(false);
  rightViewer->armRubberBand(false);

  QVector3D compP0  = p0;
  compP0.setX(compP0.x() + m_cropBox.m_pMin.x() );
  compP0.setY(compP0.y() + m_cropBox.m_pMin.y());
  compP0.setZ(compP0.z() + m_cropBox.m_pMin.z() );

  QVector3D  compExtent;

  compExtent = QVector3D(volExtent.x(),
                         volExtent.y(),
                         volExtent.z());

  //----------------------------------------------------------
  // Finally return the  p0Ret and extentRet values to the caller
  // of this method.
  p0Ret      = compP0;
  extentRet  = compExtent;
  //  std::cout << "========================================================" << std::endl;
  //  std::cout << "OPERATOR THREAT CREATION RETURN VALUES : P0 and EXTENT" << std::endl;
  //  std::cout << "P0 :     X= " <<  p0Ret.x() <<
  //               ", Y= "        <<  p0Ret.y() <<
  //               ", Z= "        <<  p0Ret.z() << std::endl;

  //  std::cout << "EXTENT : X= " <<  extentRet.x() <<
  //               ", Y= "        <<  extentRet.y() <<
  //               ", Z= "        <<  extentRet.z() << std::endl;
  //  std::cout << "========================================================" << std::endl;
  //-----------------------------------------------------------

  //-----------------------------------------
  int timeElapsed = acceptOT_Timer.elapsed();
  QString operatorThreatAcceptStr =  "*TTTTTT* Completed Operator Generated Threat selection elapsed (ms) :" +
      QString::number(timeElapsed);
  LOG(INFO) << operatorThreatAcceptStr.toStdString();
  // std::cout << operatorThreatAcceptStr.toStdString() << std::endl;
  //-----------------------------------------
  return threatIndex;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::slabModeRenderThreat(ThreatObject* pThr,
                                                bool centerOnThreat)
{
  // std::cout << "*** slabModeRenderThreat(ThreatObject* pThr, bool centerOnThreat) viewer: "
  //           << this->displayLabel().toStdString()
  //           << std::endl;
  if (!pThr)
  {
    LOG(ERROR)  << "SLAB Threat Render ThreatObject* is a NULL pointer!";
    return;
  }
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);


  VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();
  VolumeRenderer* leftRenderer  = leftViewer->getVolumeRenderer();

  ThreatVolume newTV = pThr->getVolume();

  QVector3D P0 = newTV.p0();

  QVector3D thrExtent((newTV.p1().x() - newTV.p0().x()),
                      (newTV.p1().y() - newTV.p0().y()),
                      (newTV.p1().z() - newTV.p0().z()));

  SDICOS::Bitmap* pBMP = pThr->getBMP();
  bool isLaptop = pThr->isLapTop();


  int W = thrExtent.x();
  int H = thrExtent.y();
  int L = thrExtent.z();


  std::list<OpacitySigVal> thinNormalOpacityList;
  std::list<OpacitySigVal> thinThreatOpacityList;
  std::list<OpacitySigVal>  normalOpacityList;
  std::list<HSVColorSigVal> normalHsvColorList;

  bool isLowDensity = leftRenderer->isLowDensityActive();
  if(isLowDensity)
  {
    normalOpacityList = leftViewer->getLowDensityOpacityList();
  }
  else
  {
    leftViewer->getNormalOpacityTransferFunction(normalOpacityList);
  }

  leftViewer->getNormalHSVTransferFunction(normalHsvColorList);

  std::list<HSVColorSigVal> threatHsvList;
  std::list<OpacitySigVal>  threatOpList;



  leftViewer->getThreatHSVTransferFunction(pThr->getOffset(),
                                           threatHsvList);
  leftViewer->getThreatOpacityTransferFunction(pThr->getOffset(),
                                               threatOpList);



  //--------------------------------------------------------------
  //  Check for special case of THIN SLABS
  leftRenderer->getSlabOpacityList(normalOpacityList,
                                   thinNormalOpacityList);
  leftRenderer->getSlabOpacityList(threatOpList,
                                   thinThreatOpacityList);
  //--------------------------------------------------------------

  QVector3D threatCenter((newTV.p0().x() + newTV.p1().x())/2.0,
                         (newTV.p0().y() + newTV.p1().y())/2.0,
                         (newTV.p0().z() + newTV.p1().z())/2.0);
  double newSlabOffset = rightRenderer->getSlabUnpackPositionFromPoint(threatCenter,
                                                                       m_currentCutAxis);
  leftViewer->SetSlabUnpackPosition(newSlabOffset,
                                    m_currentCutAxis,
                                    m_isSlabbing);
  rightViewer->SetSlabUnpackPosition(newSlabOffset,
                                     m_currentCutAxis,
                                     m_isSlabbing);


  std::map<double, std::list<HSVColorSigVal>>& leftThreatHsvOffsetMap =
      leftRenderer->getThreatOffsetHsvMap();

  std::map<double, std::list<OpacitySigVal>>&  leftThreatOpacityOffsetMap =
      leftRenderer->getThraOffsetOpacityMap();


  std::list<ThretOffsetProperty>&  lerftPropertyList =
      leftRenderer->getThreatPropertyList();




  if (!isLaptop)
  {
    rightRenderer->setPrimaryColorAndOpacity(normalHsvColorList,
                                             thinNormalOpacityList);

    rightRenderer->setMutiThreatPropertyColors(leftThreatHsvOffsetMap,
                                               leftThreatOpacityOffsetMap,
                                               lerftPropertyList);
  }
  else
  {
    vtkSmartPointer<vtkColorTransferFunction> threatColorTF =
        vtkSmartPointer<vtkColorTransferFunction>::New();

    vtkSmartPointer<vtkPiecewiseFunction>  threatOpacityTF =
        vtkSmartPointer<vtkPiecewiseFunction>::New();



    leftRenderer->initHSVColorTransferFunction(threatColorTF,
                                               threatHsvList);

    leftRenderer->initOpacityTransferFunction(threatOpacityTF,
                                              thinThreatOpacityList);

    rightRenderer->slabMaskNonthreatLaptop(threatColorTF,
                                           threatOpacityTF);
  }


  ThreatList* threatList = leftRenderer->getThreats();
  if (!isLaptop)
  {
    if (rightRenderer->isThreatColorized())
    {
      rightRenderer->maskAllMachineThreats(threatList, lerftPropertyList);
    }
  }
  else
  {
    rightRenderer->directMapThreat(W, H, L,
                                   P0,
                                   pBMP,
                                   isLaptop,
                                   normalHsvColorList,
                                   thinNormalOpacityList,
                                   threatHsvList,
                                   thinThreatOpacityList);
  }

  if (centerOnThreat)
  {
    int W = thrExtent.x();
    int H = thrExtent.y();
    int L = thrExtent.z();

    QVector3D extent(W,
                     H,
                     L);
    rightViewer->centerWindowOnPointAndExtentNew(P0,
                                                 extent);
  }

  // Notify Workstation of a slab position chgange.
  emit this->slabPositionChanged(newSlabOffset);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::CancelOperatorThreatGeneration()
{
  // std::cout << "&&&&&&&&&&&&& CancelOperatorThreatGeneration &&&&&&&&&&&&&" << std::endl;
  this->setPickerActive(false);
  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
  firstSib->setPickerActive(false);
  this->hideSubVolume();
  this->armRubberBand(false);
  if (firstSib)
  {
    firstSib->hideSubVolume();
    firstSib->armRubberBand(false);
  }
  LOG(INFO) << "Canecl Operator Threat Generation Not Currently Implemenmted.";
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::ExpandOperatorThreatVolume()
{
  // std::cout << "&&&&&&&&&&&&& ExpandOperatorThreatVolume &&&&&&&&&&&&&" << std::endl;
  VolumeRendererWidget* currentPicker = VolumeRendererWidget::currentThreatPicker();
  if (currentPicker)
  {
    currentPicker->increaseOperatorThreatSize();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::ShrinkOperatorThreatVolume()
{
  // std::cout << "&&&&&&&&&&&&& ShrinkOperatorThreatVolume &&&&&&&&&&&&&" << std::endl;
  VolumeRendererWidget* currentPicker = VolumeRendererWidget::currentThreatPicker();
  if (currentPicker)
  {
    currentPicker->decreaseOperatorThreatSize();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::syncSibling()
{
  if ((!m_syncActive) || (m_renderSiblingsList.size() == 0)) return;

  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
  vtkCamera* thisRenderCamera  = this->getRenderCamera();
  vtkCamera* sibCamera = firstSib->getRenderCamera();
  sibCamera->DeepCopy(thisRenderCamera);
  firstSib->removeSplash();
  firstSib->forceUpdate();
  firstSib->syncZoomText();
}

//------------------------------------------------------------------------------
int VolumeRendererWidget::ThreatCount()
{
  return m_threatList->size();
}


//------------------------------------------------------------------------------
int VolumeRendererWidget::CurrentThreat()
{
  return m_threatList->getCurrent();
}

//------------------------------------------------------------------------------
int VolumeRendererWidget::NextThreat(int index)
{
  Q_UNUSED(index);
  // ARO-TODO: This method is not properly defined!
  int iNext = 0;
  return iNext;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetDisplayLabel(const QString displayLab)
{
  m_displayName = displayLab;
  this->setRendererDisplayName();

  QString rulerName = "";
  if( m_displayName.contains("Left") )
  {
    rulerName = LEFT_RULER_NAME;
  }
  else
  {
    rulerName = RIGHT_RULER_NAME;
  }
  m_viewRuler->setName(rulerName);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::ShowViewLabel(bool doShow)
{
  m_textLabel->enableShowText(doShow);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::ShowThreatView(bool doShow)
{
  m_enableThreatViewing = doShow;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetEnableSplashScreen(bool doSplash)
{
  if (this)
  {
    this->enableSplash(doSplash);
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SetEnableDynamicSplash(bool doDynamicSplash)
{
  m_dynamicSplash = doDynamicSplash;
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SetLightingParameters(double sceneAmbient,
                                                 double sceneDiffuse,
                                                 double sceneSpecular)
{
  //  std::cout << "SetLightingParameters(ambient, diffuse, specular)"
  //            << " AMBI: " << sceneAmbient
  //            << " DIFF: " << sceneDiffuse
  //            << " SPEC: " << sceneSpecular
  //            << std::endl;

  m_lightingAmbient  = sceneAmbient;
  m_lightingDiffuse  = sceneDiffuse;
  m_lightingSpecular = sceneSpecular;
  if (m_volumeRenderer)
  {
    m_volumeRenderer->setLightingParams(m_lightingAmbient,
                                        m_lightingDiffuse,
                                        m_lightingSpecular);

    this->update();
  }
}

//-----------------------------------------------------------------------------
void VolumeRendererWidget::AppendDisplayText(QStringList& list)
{
  QString diverterVal = "";
  for (int i = 0; i < list.size();  i++)
  {
    QString testStr     = list.at(i);
    // std::cout << "******" << testStr.toStdString() << "******" << std::endl;
    QString diverterTag = tr("Diverter Reason: ");
    if (testStr.contains(diverterTag))
    {
      int blankPos = testStr.lastIndexOf(' ');
      diverterVal  = testStr.right(testStr.length() - (blankPos + 1));
    }
  }

  diverterVal.replace("ALARM",       tr("Suspect"));
  diverterVal.replace("CLEAR",       tr("Clear"));
  diverterVal.replace("UNPROCESSED", tr("Unprocessed"));
  diverterVal.replace("REJECT",      tr("Reject"));
  diverterVal.replace("TIMEOUT",     tr("Timeout"));
  diverterVal.replace("UNLINKED",    tr("Unlinked"));


  QStringList prunedList;
  bool foundSearch = false;
  for (int i = 0; i < list.size();  i++)
  {
    bool addItem = true;
    QString testStr     = list.at(i);
    LOG(INFO) << "*CHRIS-G** AppendDisplayText(LIST)  " << testStr.toStdString() << " at index: " << i;

    testStr.replace("ALARM",      tr("Suspect"),      Qt ::CaseInsensitive);
    testStr.replace("CLEAR",       tr("Clear"),       Qt ::CaseInsensitive);
    testStr.replace("UNPROCESSED", tr("Unprocessed"), Qt ::CaseInsensitive);
    testStr.replace("REJECT",      tr("Reject"),      Qt ::CaseInsensitive);
    testStr.replace("TIMEOUT",     tr("Timeout"),     Qt ::CaseInsensitive);
    testStr.replace("UNLINKED",    tr("Unlinked"),    Qt ::CaseInsensitive);

    QString diverterTag = tr("Diverter Reason: ");
    if (testStr.contains(diverterTag))
    {
      addItem = false;
    }
    QString searchTag = tr("Search Reason: ");
    if (testStr.contains(searchTag))
    {
      foundSearch = true;
      int blankPos = testStr.lastIndexOf(' ');
      QString searchVal = testStr.right(testStr.length() - (blankPos + 1));
      if (!diverterVal.isEmpty())
      {
        testStr.replace(searchVal, diverterVal);
      }
    }
    if (addItem)
    {
      prunedList.push_back(testStr);
    }
  }
  if (!foundSearch)
  {
    if (!diverterVal.isEmpty())
    {
      QString searchReason = tr("Search Reason: ") +  diverterVal;
      prunedList.insert(1, searchReason);
    }
  }
  if(
     (!this->displayLabel().contains("Right"))
     )
  {
    for (int i = 0; i < prunedList.size(); i++)
    {
      if (prunedList.at(i).contains(VTK_RFID_LABEL_TAG))
      {
        QString oldRfidString = prunedList.at(i);
        QString newRFidString = this->stripRFIDLeadingZeros(oldRfidString);
        LOG(INFO) << newRFidString.toStdString().data();
        m_prop_RFID = newRFidString;
        prunedList[i] = newRFidString;
        this->setProperty(VTK_RENDERED_RFID,
                          QVariant(m_prop_RFID));
      }
    }
    m_textLabel->insertNewText(prunedList);

    this->update();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::ShowThreatUserDecision(QString& userDecision)
{
  m_userDecision = userDecision;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetEnableLaptopRemoval(bool doEnable)
{
  if (m_threatList->size() == 0 )
  {
    m_volumeRenderer->hideLaptopThreats();
    m_volumeRenderer->resetAndUpdateMask();
  }
  if( doEnable )
  {
    emit this->viewModeEnabled(VREnums::VIEW_AS_LAPTOP);
  }
  if (m_threatList->size() == 0) return;
  m_volumeRenderer->setRemoveLaptop(doEnable);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetSlabThickness(double thickness)
{
  QString thkMessage = "%%%% Setting SLAB thickmess : "
      + QString::number(thickness);

  LOG(INFO) << thkMessage.toStdString();

  m_slabThickness = thickness;
  m_volumeRenderer->setSlabbing(m_isSlabbing, m_slabThickness);
}

//------------------------------------------------------------------------------
VolumeRendererWidget* VolumeRendererWidget::getSibling()
{
  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
  return firstSib;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::syncUserPickPpint()
{
  if (m_renderSiblingsList.size() == 0) return;
  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
  if (firstSib)
  {
    firstSib->setLastRightPickPoint(m_lastRightPickPoint);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setActiveCamera(vtkCamera* setCam)
{
  m_widgetRenderer->SetActiveCamera(setCam);
}



//------------------------------------------------------------------------------
vtkCamera* VolumeRendererWidget::getRenderCamera()
{
  return m_widgetRenderer->GetActiveCamera();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::deepCopyCameraAndUpdate(vtkCamera* aCamera)
{
  vtkCamera* activeCamera = this->getRenderCamera();
  activeCamera->DeepCopy(aCamera);
  this->update();
  this->onEndInputInteraction();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::removeText()
{
  m_textLabel->setLabel("");
}



//------------------------------------------------------------------------------
void VolumeRendererWidget::localZoom(float value)
{
  if (m_currentZoom > value)
  {
    this->triggeredActionZoomOut();
  }
  else if (m_currentZoom < value)
  {
    this->triggeredActionZoomIn();
  }
  m_currentZoom = value;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::siblingSetZoomValue(double newVal)
{
  m_currentZoom = newVal;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setOperatorThreatLabel(int index,
                                                  int size,
                                                  QString& operatorThreatType)
{
  m_textLabel->enableShowText(true);
  QString threatLab = analogic::workstation::threatTypeLab();
  threatLab += operatorThreatType;

  // Dynamicaly show the current adn total threats
  QString threatNumberOf  =  tr("\nThreat ID: ") + QString::number(index + 1) + tr(" of ");
  threatNumberOf          += QString::number(size);

  threatLab += threatNumberOf;

  this->setRawThreatLabel(threatLab);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setRawThreatLabel(QString& rawLabel)
{
  QString fullLabel = rawLabel;
  if (!m_userDecision.isEmpty())
  {
    fullLabel += "\n";
    fullLabel += m_userDecision;
    m_textLabel->moveDisplayPositionInY( -1.0*VIEW_TEXT_FONT_HEIGHT );
  }
  m_textLabel->setLabel(fullLabel);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::clearThreatLabel()
{
  m_textLabel->setLabel("");
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::vtkWidgetUpdate()
{
  this->update();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::enableThreatText()
{
  m_textLabel->enableShowText(true);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::forceUpdate()
{
  this->forceRedraw();
  m_viewRuler->setRulerSize();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::forceRedraw()
{
  bool renderRet = this->renderVTK();
  if (renderRet)
  {
    this->paintGL();
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::InitPickBoxWidget()
{
  m_volumeRenderer->createSubVolumeWidget(this);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::selectSubVolume()
{
  QVector3D pCenter = m_lastRightPickPoint;
  QVector3D pRange  = QVector3D(g_defaultPickVolWidth,
                                g_defaultPickVolHeight,
                                g_defaultPickVolLength);

  m_volumeRenderer->showSubVolume(pCenter, pRange);
  QString strWinID = "*TTTT* - Creating User Threat Viewable on:" + this->displayLabel();

  qDebug() << strWinID;
  VolumeRendererWidget::setCurrentThreatPicker(this);
}


//-----------------------------------------------------------------------------
void VolumeRendererWidget::hideSubVolume()
{
  this->syncUserPickPpint();
  m_volumeRenderer->hideSubVolume();
}


//-----------------------------------------------------------------------------
void VolumeRendererWidget::hideLaptopROI()
{
  m_threatList->forceHideLaptops();
}

//-----------------------------------------------------------------------------
int VolumeRendererWidget::addOperatorThreatObject(ThreatVolume& newTV, QString& threatTypeText)
{
  int ID = m_threatList->getNewOperatorThreatID();

  QString threatID = QString::number(ID);

  QString threatText = analogic::workstation::constructRawOperatorThreatText(threatTypeText);

  ThreatObject* pThreat = new ThreatObject(ID,
                                           newTV,
                                           threatID,
                                           threatText,
                                           nullptr);

  analogic::workstation::ThreatGenerationType aGenType = OPERATOR_GEN;
  analogic::workstation::ThreatAlarmType thType = EXPLOSIVE;

  uint16_t threatOffset = 0;
  // Old code  m_threatSignalOffsetMap[thType];

  pThreat->setGenType(aGenType);
  pThreat->setType(thType);
  pThreat->setMass(0);
  pThreat->setOffset(threatOffset);

  QVector3D imageP0;
  QVector3D imageP1;
  this->worldToImageVolumeRange(newTV.p0(),
                                newTV.p1(),
                                imageP0,
                                imageP1);

  ThreatVolume imageVol(imageP0, imageP1);

  //-----------------------------------
  // Gett the color for operator threat:
  QColor opeThreatcolor = analogic::workstation::getOperatorthreatBoxColor();
  double opRed   = static_cast<double>(opeThreatcolor.red()   )/255.0;
  double opGreen = static_cast<double>(opeThreatcolor.green() )/255.0;
  double opBlue  = static_cast<double>(opeThreatcolor.blue()  )/255.0;
  //-----------------------------------
  vtkSmartPointer<vtkActor> anActor =  m_volumeRenderer->createThreatVisual(imageVol,
                                                                            opRed,
                                                                            opGreen,
                                                                            opBlue,
                                                                            false);
  pThreat->setActor(anActor);
  m_threatList->addThreatObject(pThreat);
  if (ID == 0) m_threatList->setCurrent(ID);
  return ID;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::getUserThreatRange(QVector3D& p0,
                                              QVector3D& p1)
{
  m_volumeRenderer->getSubWidgetBounds(p0, p1);
}


//------------------------------------------------------------------------------
bool VolumeRendererWidget::getCurrentThreatVolume(QVector3D& p0,
                                                  QVector3D& p1)
{
  if (!m_threatList)
  {
    p0 = QVector3D(0, 0, 0);
    p1 = QVector3D(0, 0, 0);
    return false;
  }
  if (m_threatList->size() == 0)
  {
    p0 = QVector3D(0, 0, 0);
    p1 = QVector3D(0, 0, 0);
    return false;
  }
  int curThreat = m_threatList->getCurrent();
  if (curThreat == -1)
  {
    p0 = QVector3D(0, 0, 0);
    p1 = QVector3D(0, 0, 0);
    return false;
  }
  ThreatObject* pThr = m_threatList->getThreatAtIndex(curThreat);
  ThreatVolume pVol = pThr->getVolume();
  p0 = pVol.p0();
  p1 = pVol.p1();
  return true;
}

//------------------------------------------------------------------------------
ThreatObject* VolumeRendererWidget::getCurrentThreatObject()
{
  if (!m_threatList)
  {
    return nullptr;
  }
  if (m_threatList->size() == 0)
  {
    return nullptr;
  }
  int curThreat = m_threatList->getCurrent();
  if (curThreat == -1)
  {
    return nullptr;
  }
  ThreatObject* pThr = m_threatList->getThreatAtIndex(curThreat);
  return pThr;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setSurfaceROIEnable(bool setVal)
{
   m_threatList->setSurfaceViewMode(setVal);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::setMutexForRendering(QMutex* aMutex)
{
  m_volumeRenderer->setRenderMutex(aMutex);
}

//------------------------------------------------------------------------------
int VolumeRendererWidget::firstVolumeRenderTime()
{
  return m_volumeRenderer->getRenderBuildTime();
}


//------------------------------------------------------------------------------
QString VolumeRendererWidget::getGenericMessageStrig(int msgID)
{
  if (g_genericVRMessageMap.find(msgID) == g_genericVRMessageMap.end())
  {
    return tr("UNKNOWN MESSAGE");
  }
  else
  {
    return g_genericVRMessageMap[msgID];
  }
}

//-----------------------------------------------------------------------------
int VolumeRendererWidget::unclearedThreats()
{
  if (m_threatList)
  {
    return m_threatList->unClearedThreats();
  }
  else
  {
    return 0;
  }
}

//------------------------------------------------------------------------------
int VolumeRendererWidget::unclearedMachineThreats()
{
  if (m_threatList)
  {
    return m_threatList->unClearedMachineThreats();
  }
  else
  {
    return 0;
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::clearAllThreats()
{
  if (!m_threatList) return;
  m_threatList->clear();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::disarmOperatorThreatPick()
{
  m_volumeRenderer->hideSubVolume();
}

//-------------------------------------------------------------------------------
void VolumeRendererWidget::clearTheratData()
{
  m_threatList->clearData();
}

//-------------------------------------------------------------------------------
void VolumeRendererWidget::armRubberBand(bool armVal)
{
  this->armForOperatorRubberBandSelect(armVal);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setCurrentThreatPicker(VolumeRendererWidget* pVRWidget)
{
  VolumeRendererWidget::s_curretnThreatPicker  = pVRWidget;
}


//------------------------------------------------------------------------------
VolumeRendererWidget* VolumeRendererWidget::currentThreatPicker()
{
  return VolumeRendererWidget::s_curretnThreatPicker;
}


//------------------------------------------------------------------------------
bool VolumeRendererWidget::pickerActive()
{
  return VolumeRendererWidget::s_pickerActive;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setPickerActive(bool setVal)
{
  VolumeRendererWidget::s_pickerActive = setVal;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetAsThreatRenderer(bool setVal)
{
  // std::cout <<  "&&&&&& SetAsThreatRenderer - called setVal= "
  //           <<  setVal << " viewer: "
  //           <<  this->displayLabel().toStdString()
  //           <<  std::endl;
  m_isThreatRenderer = setVal;
  this->setAsThreatViewer(setVal);
  if (setVal)
  {
    m_textLabel->moveDisplayPositionInY( -1.0*VIEW_TEXT_FONT_HEIGHT );
  }
  else
  {
    m_textLabel->moveDisplayPositionInY(0);
  }
  if (setVal)
  {
    emit this->viewModeEnabled(VREnums::VIEW_AS_THREAT);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::initAsThreatrenderer()
{
  // First we needto make sure we use the Left viewer to drive the change.
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);

  VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();
  VolumeRenderer* leftgRenderer = leftViewer->getVolumeRenderer();
  double leftSceneAmbient;
  double leftSceneDiffuse;
  double leftSceneSpecular;


  leftgRenderer->getLightingParams(leftSceneAmbient,
                                   leftSceneDiffuse,
                                   leftSceneSpecular);
  rightViewer->SetAsThreatRenderer(true);

  if (rightRenderer)
  {
    rightRenderer->setLightingParams(leftSceneAmbient,
                                     leftSceneDiffuse,
                                     leftSceneDiffuse);

    this->update();
  }
}



//------------------------------------------------------------------------------
bool VolumeRendererWidget::isAThreatRenderer()
{
  return m_isThreatRenderer;
}

//------------------------------------------------------------------------------
bool VolumeRendererWidget::isInSlabMode()
{
  return m_isInSlabbingMode;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::revertRightViewerFormThreatSettings()
{
  // std::cout << "%%%%%%%%%%%%%%%%%%%%%%% revertRightViewerFormThreatSettings %%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }

  // First we needto make sure we use the Left viewer to drive the change.
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  VolumeRenderer* rightVR = rightViewer->getVolumeRenderer();

  rightVR->resetFromSiblingThreat();
  rightVR->resetClipIndex();

  leftViewer->SetSyncEnabled(true);
  rightViewer->SetSyncEnabled(true);
  rightViewer->removeText();
  rightViewer->SetAsThreatRenderer(false);
  this->ResetDisplay(true);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::getCurrentTransferFunctions(std::list<HSVColorSigVal>& retListHSV,
                                                       std::list<OpacitySigVal>&  retListOpcity)
{
  this->getNormalHSVTransferFunction(retListHSV);
  this->getNormalOpacityTransferFunction(retListOpcity);
}



//------------------------------------------------------------------------------
void VolumeRendererWidget::getNormalHSVTransferFunction(std::list<HSVColorSigVal>& retList)
{
  std::list<HSVColorSigVal>::iterator itL;

  if ( m_colorMapNormal.size() == 0 )
  {
    return;
  }
  for (itL = m_colorMapNormal.begin(); itL != m_colorMapNormal.end(); itL++)
  {
    HSVColorSigVal svI = *itL;
    retList.push_back(svI);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::getNormalOpacityTransferFunction(std::list<OpacitySigVal>& retList)
{
  std::list<OpacitySigVal>::iterator itL;
  if ( m_opacityMapNormal.size() == 0 )
  {
    return;
  }
  for (itL = m_opacityMapNormal.begin(); itL != m_opacityMapNormal.end(); itL++)
  {
    OpacitySigVal opI = *itL;
    retList.push_back(opI);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::getThreatOpacityTransferFunction(uint16_t threatOffset,
                                                            std::list<OpacitySigVal>& retOpList)
{
  if (m_threatOffsetOpacityMap.find(threatOffset) != m_threatOffsetOpacityMap.end())
  {
    retOpList = m_threatOffsetOpacityMap[threatOffset];
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::getThreatHSVTransferFunction(uint16_t threatOffset,
                                                        std::list<HSVColorSigVal>& retHsvList)
{
  if(m_threatOffsetHsvMap.find(threatOffset) != m_threatOffsetHsvMap.end())
  {
    retHsvList = m_threatOffsetHsvMap[threatOffset];
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::triggerThreatInitTimer()
{
  QTimer::singleShot(g_threatInitRedrawTimeout, this,
                     &VolumeRendererWidget::onThreatInitTimerTimeout);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::onThreatInitTimerTimeout()
{
  this->resetThreatRendererViewAlignment();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::onIntractionEndTimeout()
{
  if (VolumeRendererWidget::pickerActive()) return;
  if (m_distMeasureStatus == DISTANCE_MEASURE_INACTIVE)
  {
     // this->distanceMeasurementComplete();
    //  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
    // firstSib->distanceMeasurementComplete();
  }

  if  ( this->isSplashEenabled() && m_dynamicSplash)
  {
    VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
    this->showVtkSplashScreenNew();
    firstSib->showVtkSplashScreenNew();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onPercentSliderUpdate()
{
  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
  this->immediateRender();
  firstSib->immediateRender();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::immediateRender()
{
  vtkRenderWindow* renderWin = this->GetRenderWindow();
  renderWin->Render();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::reDisplayCurrentThreat()
{
  if (!m_volumeRenderer->isThreaMaskingAllowed()) return;
  // std::cout << "VolumeRendererWidget::reDisplayCurrentThreat()" << std::endl;
  QTime timeRedisplayThreat = QTime::currentTime();

  int currentThreat = this->CurrentThreat();
  if (currentThreat < 0)
  {
    QString errMsg = "Error - reDisplayCurrentThreat() fails bacause current threat index = -1 ";
    LOG(ERROR) << errMsg.toStdString();
  }
  if (currentThreat >= 0)
  {
    bool bKeepOrientation = true;
    if (m_isSlabbing)
    {
      bKeepOrientation = false;
    }

    this->SetThreatVisibility(nullptr,
                              currentThreat,
                              true,
                              bKeepOrientation,
                              m_threatTypeIndex,
                              m_threatTypeTotal);
  }
  int timeRedisp_ms = timeRedisplayThreat.elapsed();
  QString redisplayTime = "*TTTTTT* VolumeRendererWidget::reDisplayCurrentThreat() - elsaped time (ms): " +
      QString::number(timeRedisp_ms);
  LOG(INFO) << redisplayTime.toStdString();
  // std::cout << redisplayTime.toStdString() << std::endl;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::internalClearCurrentThreat()
{
  // std::cout << "&&&&&&&&&&&&& ClearCurrentThreat &&&&&&&&&&&&&" << std::endl;
  //----------------------------------------
  // Clear splash screen if present.
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  rightViewer->RemoveSplashIfPresent();
  leftViewer->RemoveSplashIfPresent();
  //-----------------------------------------

  if (m_threatRawIndex < 0) return;

  //--------------------------------------------------------
  // ARO-NOTE: The m_threatList is passed as a pointer to
  // m_volumeRenderer no need to modify it here directly on
  // clearMachineThreat().
  //--------------------------------------------------------

  m_threatRawIndex = -1;

  //---------------------------------
  // Reset mask and redraw threat
  QTime clearRedrawTimer = QTime::currentTime();
  VolumeRenderer* leftRenderer = leftViewer->getVolumeRenderer();
  leftRenderer->resetAndUpdateMask();
  int clrRedrawTime_ms = clearRedrawTimer.elapsed();
  QString message = "*** CLEAR-REDRAW Elapsed time (ms): "  + QString::number(clrRedrawTime_ms);
  LOG(INFO) << message.toStdString();
  // std::cout << message.toStdString()
  //           << std::endl;
  //---------------------------------

  int cleared = m_threatList->clearedMachineThreats();

  if (cleared == m_threatList->size())
  {
    m_volumeRenderer->resetAndUpdateMask();
    this->forceRedraw();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::triggeredActionZoomIn()
{
  if (this->displayLabel().contains("Left"))
  {
    // std::cout << "*** triggered LEFT ZOOM IN - zoomCount: "
    //           << s_zoomCount
    //           << "  ***"
    //           << std::endl;
    if ( (!m_isThreatRenderer) ||
         (m_slabViewType == VREnums::BAG_VIEW))
    {
      if (s_zoomCount < VTK_ZOOM_IN_LIMIT )
      {
        this->performZoom(DEF_ZOOM_WHEEL_DELTA);
        this->forceRedraw();
        if (this->displayLabel().contains("Left"))
        {
          s_zoomCount++;
        }
      }
    }
  }
  else
  {
    // std::cout << "*** RIGHT THREAT ZOOM IN - s_zoomCountThreat: "
    //           << s_zoomCountThreat
    //           << "   ,VTK_THREAT_ZOOM_IN_LIMIT: "
    //           << VTK_THREAT_ZOOM_IN_LIMIT
    //           << std::endl;
    if (s_zoomCountThreat < VTK_THREAT_ZOOM_IN_LIMIT )
    {
      this->performZoom(DEF_ZOOM_WHEEL_DELTA);
      this->forceRedraw();
      s_zoomCountThreat++;
    }
  }
  emit this->interactorActivityEnd();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::triggeredActionZoomOut()
{
  if (this->displayLabel().contains("Left"))
  {
    // std::cout << "*** triggered ZOOM OUT - zoomCount: "
    //           << s_zoomCount
    //           << "  ***"
    //           << std::endl;
    if ( (!m_isThreatRenderer) ||
         (m_slabViewType == VREnums::BAG_VIEW)
         )
    {
      if (s_zoomCount > VTK_ZOOM_OUT_LIMIT )
      {
        this->performZoom(-1*DEF_ZOOM_WHEEL_DELTA);
        this->forceRedraw();
        if (this->displayLabel().contains("Left"))
        {
          s_zoomCount--;
        }
      }
    }
  }
  else
  {
    // std::cout << "*** RIGHT THREAT ZOOM OUT - s_zoomCountThreat: "
    //           << s_zoomCountThreat
    //           << "   ,VTK_THREAT_ZOOM_OUT_LIMIT: "
    //           << VTK_THREAT_ZOOM_OUT_LIMIT
    //           << std::endl;
    if (s_zoomCountThreat > VTK_THREAT_ZOOM_OUT_LIMIT )
    {
      this->performZoom(-1*DEF_ZOOM_WHEEL_DELTA);
      this->forceRedraw();
      s_zoomCountThreat--;
    }
  }
  emit this->interactorActivityEnd();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::UpdateRenderer()
{
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  m_renderWindowRef->Render();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SuspectBag()
{
  if (m_threatList)
  {
    m_threatList->suspectBag();
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::NotifyDetectionException()
{
  this->disableVolume();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetToggleThreatColorization(bool setValue)
{
  // std::cout << "&&&&&&&&&&&&& SetToggleThreatColorization &&&&&&&&&&&&&"
  //           << this->displayLabel().toStdString() << std::endl;
  if (this->displayLabel().contains("Right")) return;
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  VolumeRenderer* leftRenderer   = leftViewer->getVolumeRenderer();
  VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();


  rightRenderer->setColorizeThreat(setValue);
  leftRenderer->setColorizeThreat(setValue);

  //--------------------------------------------------

  if (!setValue)
  {
    leftRenderer->resetAndUpdateMask();
    rightRenderer->resetTransferFunctions();
  }

  int curThreat = leftViewer->CurrentThreat();
  if (curThreat >= 0)
  {
    leftViewer->SetThreatVisibility(nullptr,
                                    curThreat,
                                    true,
                                    true,
                                    m_threatTypeIndex,
                                    m_threatTypeTotal);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::MeasureVolumeByValue(bool setVal)
{
  QString logMsg =  "%%%% MeasureVolumeByValue(setVal) %%%% new Value = ";
  if (setVal)
  {
    logMsg +=  "TRUE";
  }
  else
  {
    logMsg += "FALSE";
  }
  LOG(INFO) << logMsg.toStdString();
  // std::cout << logMsg.toStdString() << std::endl;

  this->armForVolumeMeasurement(setVal);
  if (!setVal)
  {
    this->removeSplash();
    m_volumeRenderer->removeMeasuredVolume();
    m_volumeRenderer->removeMeasuredText();
    emit this->interactorActivityEnd();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetEnableDistanceMeasurement(bool setVal)
{
  // std::cout << "%%%%%%%% SetEnableDistanceMeasurement() called set value :"
  //          << setVal << std::endl;
  m_distanceMeasureToolActive = setVal;
  if (setVal)
  {
    this->setDistanceMeasureStatus(DISTANCE_MEASURE_START);
    m_distanceWidget->SetWidgetStateToManipulate();
    // this->showDebugDistanceMeasure("ouside-call->SetEnableDistanceMeasurement()");
  }
  if (!setVal)
  {
    m_distanceWidget->Off();
    this->setDistanceMeasureStatus(DISTANCE_MEASURE_INACTIVE);
    this->onEndInputInteraction();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetVoumeMeasurementStartRange(uint16_t minVal,
                                                         uint16_t maxVal)
{
  m_volumeRenderer->setVolumeComputeStartRange(minVal,
                                               maxVal);
  QString logString = "%>>><<<% Volume Measure start range clamp [min, max]:";
  logString += QString::number(minVal);
  logString += ", " + QString::number(maxVal);
  LOG(INFO) << logString.toStdString();
  // std:cout << logString.toStdString() << std::endl;
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::SlabNonThreatLaptop(int laptopID)
{
  QString logMessage = "*** Slabbing Current non-threat LAPTOP - index = " +
      QString::number(laptopID);
  LOG(INFO) << logMessage.toStdString();
  // std::cout << logMessage.toStdString()
  //           << std::endl;

  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  VolumeRenderer* rightRenderer  = rightViewer->getVolumeRenderer();
  VolumeRenderer* leftRenderer = leftViewer->getVolumeRenderer();

  ThreatObject* pThr = m_threatList->getThreatAtIndex(laptopID);
  if (!pThr) return;

  boost::shared_ptr<analogic::ws::BagData> bagData = analogic::ws::BagsDataPool::getInstance()->
      getBagbyState(analogic::ws::BagData::RENDERING);
  if(bagData != NULL)
  {
     int visibleEntityIndex       = bagData->getCurrentVisibleEntityIndex();
     int totalVisibleEntityCount  = bagData->getCurrentVisibleEntityCount();
     // std::cout << "------------------------------------------------------" << std::endl;
     // std::cout << "ARO-DEBUG  BBB        : "                               << std::endl;
     // std::cout << "Threat Index:         : " <<  pThr->getIndex()          << std::endl;
     // std::cout << "laptop ID             : " <<  laptopID                  << std::endl;
     // std::cout << "visibilityIndex       : " <<  visibleEntityIndex        << std::endl;
     // std::cout << "visibilityEntityCount : " <<  totalVisibleEntityCount   << std::endl;
     // std::cout << "------------------------------------------------------" << std::endl;
     if (visibleEntityIndex < 0)
     {
       visibleEntityIndex = totalVisibleEntityCount -1;
     }
     this->showThreatText(pThr,
                          visibleEntityIndex,
                          totalVisibleEntityCount);
  }

  m_threatList->setSlabingLaptop(true);
  m_threatList->setCurrent(pThr->getIndex());
  QVector3D P0 = pThr->getVolume().p0();
  QVector3D P1 = pThr->getVolume().p1();
  double W = P1.x() - P0.x();
  double H = P1.y() - P0.y();
  double L = P1.z() - P0.z();

  QVector3D extent(W,
                   H,
                   L);

  std::list<OpacitySigVal>  normalOpacityList;
  std::list<HSVColorSigVal> normalHsvColorList;
  std::list<OpacitySigVal> thinNormalOpacityList;



  leftViewer->getNormalOpacityTransferFunction(normalOpacityList);
  leftViewer->getNormalHSVTransferFunction(normalHsvColorList);
  //----------------------------------------------------------------
  //  Check for special case of THIN SLABS
  leftRenderer->getSlabOpacityList(normalOpacityList,
                                   thinNormalOpacityList);
  rightRenderer->removeLabelMaskColors();
  rightRenderer->setPrimaryColorAndOpacity(normalHsvColorList,
                                           thinNormalOpacityList);
  //----------------------------------------------------------------

  rightRenderer->mapClippedSubVolume(P0, W, H, L, false);
  rightRenderer->resetMaskWithResize(W, H, L);
  rightRenderer->setMaskBitsToValue(0);
  rightRenderer->resetMask();
  rightViewer->centerCroppedThreatWindow(P0, extent);
  emit rightViewer->interactorActivityEnd();
}

//------------------------------------------------------------------------------
QString VolumeRendererWidget::displayLabel()
{
  return m_displayName;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::SetEnableThreatRedering(bool setVal)
{
  m_volumeRenderer->allowThreatMask(setVal);
}


//------------------------------------------------------------------------------
QString VolumeRendererWidget::displayModeString(VREnums::VRViewModeEnum aMode)
{
  if (aMode == VREnums::VIEW_AS_THREAT)  return "VIEW_AS_THREAT";
  if (aMode == VREnums::VIEW_AS_SURFACE) return "VIEW_AS_SURFACE";
  if (aMode == VREnums::VIEW_AS_LAPTOP)  return "VIEW_AS_LAPTOP";
  if (aMode == VREnums::VIEW_AS_SURFACE_CAMERA) return "VIEW_AS_SURFACE_CAMERA";
  if (aMode == VREnums::VIEW_AS_SLAB)    return "VIEW_AS_SLAB";
  return "UNKNOWN_VIEW_MODE";
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::enableVolumeCropping(bool doCrop)
{
  m_doInternalCropping = doCrop;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setCropCorners(CropCorners& corners)
{
  m_cropBox = corners;
}

//------------------------------------------------------------------------------
QStringList VolumeRendererWidget::getThreatList()
{
  return m_volumeRenderer->getThreatList();
}

//-----------------------------------------------------------------------------
std::list<ThreatObject*> VolumeRendererWidget::getThreatObjectList()
{
  return m_volumeRenderer->getThreatObjectList();
}

//-----------------------------------------------------------------------------
void VolumeRendererWidget::saveVTKTranferFunctions()
{
  QString grayHsvFile = "/home/analogic/junk/GrayScaleHSV_TF.csv";
  // std::list<OpacitySigVal>  curOpacityList;
  std::list<HSVColorSigVal> curHsvColorList;

  // leftRenderer->getNormalOpacityTransferFunction(curOpacityList);
  this->getNormalHSVTransferFunction(curHsvColorList);
  vtkSmartPointer<vtkColorTransferFunction> grayscaleTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();
  m_volumeRenderer->initBWTransferFunction(grayscaleTF,
                                           curHsvColorList);

  m_volumeRenderer->saveVTKColorMapToFile(grayscaleTF, grayHsvFile);
}

//------------------------------------------------------------------------------
QVector3D VolumeRendererWidget::getLastRightPickPoint()
{
  return m_lastRightPickPoint;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setLastRightPickPoint(QVector3D& pickPoint)
{
  m_lastRightPickPoint = pickPoint;
}

//------------------------------------------------------------------------------
vtkCamera* VolumeRendererWidget::getMainCamera()
{
  return m_widgetRenderer->GetActiveCamera();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::updateThreatRuler()
{
}

//------------------------------------------------------------------------------
int VolumeRendererWidget::getDbgBagCount()
{
  return m_dbgBagCount;
}

//------------------------------------------------------------------------------
double VolumeRendererWidget::getLastThreatVolumeSize()
{
  double retVal  = 0;
  return retVal;
}

//------------------------------------------------------------------------------
vtkCamera* VolumeRendererWidget::getThreatCamera()
{
  return nullptr;
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::createVTKWindow()
{
  // Step 1. Create main vtk renderer
  // VTK Renderer
  m_widgetRenderer = vtkSmartPointer<vtkRenderer>::New();



  // Show with QVTKOpenGLWidget
  m_renderWindowRef = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  m_renderWindowRef->AddRenderer(m_widgetRenderer);


  this->SetRenderWindow(m_renderWindowRef.GetPointer());
  // this->setMinimumWidth(VR_WIDGET_DEF_WIDTH);
  // widget->setMinimumHeight(VR_WIDGET_DEF_HEIGHT);


  m_widgetRenderer->SetBackground(VOLUME_BG_DEFAULT_RED,
                                  VOLUME_BG_DEFAULT_GREEN,
                                  VOLUME_BG_DEFAULT_BLUE);



  // Note: VolumeRenderWidget is now subclassed from
  // OpenGLVolumeWidget so the QT-VTK links is now
  // made during construction -[Qt and VTK are wedded].


  this->setVTKRenderer(m_widgetRenderer);
  this->setMinimumWidth(VTK_MAIN_VIEWER_WIN_WIDTH);
  this->setMinimumHeight(VTK_MAIN_VIEWER_WIN_HEIGHT);

  m_parentWidget->setEnabled(true);
  m_parentWidget->setText("");
  m_parentWidget->setMinimumWidth(VTK_MAIN_VIEWER_WIN_WIDTH);
  m_parentWidget->setMinimumHeight(VTK_MAIN_VIEWER_WIN_HEIGHT);

  m_parentWidget->show();
  this->show();


  // Step 2. Create Volume renderer.
  m_volumeRenderer = new VolumeRenderer(m_renderWindowRef,
                                        m_widgetRenderer,
                                        GENERIC_SCANNER_PIXEL_SPACING_X,
                                        GENERIC_SCANNER_PIXEL_SPACING_Y,
                                        GENERIC_SCANNER_PIXEL_SPACING_Z,
                                        GENERIC_SCANNER_MAX_X_DIM,
                                        GENERIC_SCANNER_MAX_Y_DIM,
                                        GENERIC_SCANNER_MAX_Z_DIM);

  // Threat Processing Now handeled by right viewer.
  m_volumeRenderer->setBgNormal();



  // Step 3. Set Desired Update Rate to reduce volume renderer degradation with
  // large number of threats.
  this->setDesiredUpdateRate(g_desiredUpdateRate);
  m_volumeRenderer->resetCamera();

  //-------------------------------------------
  // Setup Connections

  this->connect(m_volumeRenderer, &VolumeRenderer::readDataRange,
                this, &VolumeRendererWidget::onReadDataRange);

  this->connect(m_volumeRenderer, &VolumeRenderer::loadComplete,
                this,  &VolumeRendererWidget::onImageLoadComplete);

  this->connect(m_volumeRenderer, &VolumeRenderer::volumeMeasureComplete,
                this, &VolumeRendererWidget::onVolumeMeasurementComplete);

  this->connect(m_volumeRenderer, &VolumeRenderer::debugMessage,
                this, &VolumeRendererWidget::onVolumeRenderDBGUpdate);

  this->connect(m_volumeRenderer, &VolumeRenderer::genericVRBaseMessage,
                this, &VolumeRendererWidget::onVolumeRenderGenericMessage);

  this->connect(m_volumeRenderer, &VolumeRenderer::timerVRBaseMessage,
                this, &VolumeRendererWidget::onVolumeRenderTimerMessage);

  this->connect(m_volumeRenderer, &VolumeRenderer::memAllocationRequest,
                this, &VolumeRendererWidget::onVolumeRenderMemRequest);

  this->connect(this, &VolumeRendererWidget::inputRedrawNeeded,
                this, &VolumeRendererWidget::onSliderRedraw);

  this->connect(this, &OpenGLVolumeWidget::cameraPositionModified,
                this, &VolumeRendererWidget::onCameraPositionChanged);

  this->connect(this, &OpenGLVolumeWidget::volumepointPickEvent,
                this, &VolumeRendererWidget::onMainVolumePick);

  this->connect(this, &OpenGLVolumeWidget::volumeMeasurePickPoint,
                this, &VolumeRendererWidget::onVolumeMeasurementByPick);

  this->connect(this, &OpenGLVolumeWidget::beginInteraction,
                this, &VolumeRendererWidget::onBeginInteractiveRender);

  this->connect(this, &OpenGLVolumeWidget::splashReady,
                this, &VolumeRendererWidget::onPaintComplete);

  this->connect(this, &OpenGLVolumeWidget::rubberBandSelected,
                this, &VolumeRendererWidget::onRubberBandSelected);

  this->connect(this, &OpenGLVolumeWidget::interactorActivityEnd,
                this, &VolumeRendererWidget::onEndInputInteraction);


  //-----------------------------------------------------------
  // (X,Y,Z) Axis widget.

  m_viewAxisWidget = new AxisWidget(m_renderWindowRef,
                                    g_axesViewportMinXNormal,
                                    g_axesViewportMinYNormal,
                                    g_axesViewportMaxXNormal,
                                    g_axesViewportMaxYNormal);


  //-----------------------------------------------------------
  // Ruler Widget for measuring distances.
  m_viewRuler = new RulerWidget(m_widgetRenderer, false, "");

  this->setRuler(m_viewRuler);



  // Label Widget for drawing the label.
  // ARO-NOTE: This is now dependant on which window!
  m_textLabel = new ViewLabelWidget(m_renderWindowRef,
                                    m_widgetRenderer,
                                    g_viewLabelDisplayPos,
                                    g_viewLabelPos2,
                                    "",
                                    false);

  m_textLabel->setTextSize(g_viewFontSize);
  this->setViewLabel(m_textLabel);

  this->initViewAlignment();
  this->update();

  //-------------------------------------------
  vtkRenderWindowInteractor* renderWindowInteractor = m_renderWindowRef->GetInteractor();

  vtkSmartPointer<vtkCallbackCommand> mainCameraMoveCallbackCmd =
      vtkSmartPointer<vtkCallbackCommand>::New();

  mainCameraMoveCallbackCmd->SetCallback(mainRendererCameraMoveCallback);
  mainCameraMoveCallbackCmd->SetClientData(this);
  renderWindowInteractor->AddObserver(vtkCommand::InteractionEvent,
                                      mainCameraMoveCallbackCmd);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setDistanceMeasureUnit(bool asMetric)
{
  if (asMetric)
  {
    vtkDistanceRepresentation* toolRep =
        static_cast<vtkDistanceRepresentation *>(m_distanceWidget->GetRepresentation());
    toolRep->SetLabelFormat("%-#6.3g cm");
    toolRep->SetScale(0.1);
  }
  else
  {
    vtkDistanceRepresentation* toolRep =
        static_cast<vtkDistanceRepresentation *>(m_distanceWidget->GetRepresentation());
    toolRep->SetLabelFormat("%-#6.3g in");
    toolRep->SetScale(0.03937);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::createDistanceMeasurer()
{
  m_distanceRep = vtkSmartPointer<vtkDistanceRepresentation3D>::New();


  m_distanceRep->GetLineProperty()->SetColor(MEASURE_RULER_LINE_COLOR_RED,
                                             MEASURE_RULER_LINE_COLOR_GREEN,
                                             MEASURE_RULER_LINE_COLOR_BLUE);

  m_distanceRep->GetLineProperty()->SetLineWidth(3);

  bool handleIsSphere = false;
  if (handleIsSphere)
  {
    vtkSmartPointer<vtkSphereHandleRepresentation> handleRep =
        vtkSmartPointer<vtkSphereHandleRepresentation>::New();
    handleRep->GetProperty()->SetColor(MEASURE_RULER_HANDLE_COLOR_RED,
                                       MEASURE_RULER_HANDLE_COLOR_GREEN,
                                       MEASURE_RULER_HANDLE_COLOR_BLUE);
    handleRep->SetHandleSize(MEASURE_RULER_HANDEL_SIZE_SPHERE);
    m_distanceRep->SetHandleRepresentation(handleRep);
  }
  else
  {
    vtkSmartPointer<vtkPointHandleRepresentation3D> handleRep =
        vtkSmartPointer<vtkPointHandleRepresentation3D>::New();
    handleRep->GetProperty()->SetColor(MEASURE_RULER_HANDLE_COLOR_RED,
                                       MEASURE_RULER_HANDLE_COLOR_GREEN,
                                       MEASURE_RULER_HANDLE_COLOR_BLUE);
    handleRep->SetHandleSize(MEASURE_RULER_HANDEL_SIZE_POINT);
    handleRep->SetHotSpotSize(300);
    m_distanceRep->SetHandleRepresentation(handleRep);
  }

  // The Glyphs are the tick marks.
  m_distanceRep->GetGlyphActor()->GetProperty()->SetColor(MEASURE_RULER_GLYPH_COLOR_RED,
                                                          MEASURE_RULER_GLYPH_COLOR_GREEN,
                                                          MEASURE_RULER_GLYPH_COLOR_BLUE);



  //-------------------------------------------------------------
  // Ruler Text Properites
  vtkAxisActor* testAct = reinterpret_cast<vtkAxisActor*>(m_distanceRep->GetLabelActor());
  if (testAct)
  {
    testAct->GetProperty()->SetColor(MEASURE_RULER_TEXT_COLOR_RED,
                                     MEASURE_RULER_TEXT_COLOR_GREEN,
                                     MEASURE_RULER_TEXT_COLOR_BLUE);
  }
  //-------------------------------------------------------------
  // ARO-REMOVE LATER
  // double lablePos = m_distanceRep->GetLabelPosition();
  // std::cout << "LABEL Position:" << lablePos << std::endl;
  //-------------------------------------------------------------
  m_distanceRep->SetLabelPosition(1.0);


  //---------------------------------------------------------------------------
  // ARO-PERHAPS REMOVE LATER
  // vtkSmartPointer<vtkCallbackCommand> myCommandCallback =
  // vtkSmartPointer<vtkCallbackCommand>::New();
  // myCommandCallback->SetCallback(myCallbackFunction);
  //---------------------------------------------------------------------------

  m_distanceWidget = vtkSmartPointer<TouchDistanceWidget>::New();

  vtkRenderWindow* t_renWin = this->GetRenderWindow();

  vtkRenderWindowInteractor* t_interactor = t_renWin->GetInteractor();

  m_distanceWidget->SetInteractor(t_interactor);

  m_distanceWidget->SetRepresentation(m_distanceRep);
  m_distanceWidget->SetWidgetStateToManipulate();
  // m_distanceWidget->SetDebug(1);

  //---------------------------------------------------------------------------
  // ARO-PERHAPS REMOVE LATER
  // t_interactor->AddObserver(vtkCommand::LeftButtonPressEvent, myCommandCallback);
  // t_interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, myCommandCallback);
  // m_distanceWidget->AddObserver(vtkWidgetEvent::AddPoint, myCommandCallback);
  //---------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setMaterialTransitions(std::map<MaterialTransition_E,
                                                  double>&
                                                  transitionMap)
{
  m_materialTransitionMap.clear();
  std::map<MaterialTransition_E, double>::iterator iter;
  for (iter = transitionMap.begin(); iter != transitionMap.end(); iter++)
  {
    MaterialTransition_E enV = iter->first;
    double               tV  = iter->second;
    m_materialTransitionMap[enV] = tV;
  }
  m_threatSignalOffsetMap[EXPLOSIVE]  = m_materialTransitionMap[SIG_THREAT_SET_A];
  m_threatSignalOffsetMap[LAPTOP]     = m_materialTransitionMap[SIG_THREAT_SET_A];
  m_threatSignalOffsetMap[SHIELD]     = m_materialTransitionMap[SIG_THREAT_SET_B];
  m_threatSignalOffsetMap[LIQUID]     = m_materialTransitionMap[SIG_THREAT_SET_A];
  m_threatSignalOffsetMap[SHARP]      = m_materialTransitionMap[SIG_THREAT_SET_C];
  m_threatSignalOffsetMap[BLUNT]      = m_materialTransitionMap[SIG_THREAT_SET_C];
  m_threatSignalOffsetMap[GUN]        = m_materialTransitionMap[SIG_THREAT_SET_C];
  m_threatSignalOffsetMap[WEAPON]     = m_materialTransitionMap[SIG_THREAT_SET_D];
  m_threatSignalOffsetMap[CONTRABAND] = m_materialTransitionMap[SIG_THREAT_SET_E];
  m_threatSignalOffsetMap[ANOMALY]    = m_materialTransitionMap[SIG_THREAT_SET_F];
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::setVolumeTransferFunctions(std::list<HSVColorSigVal>& colorList,
                                                      std::list<OpacitySigVal>&  opacityList)
{
  m_opacityMapNormal.clear();
  m_colorMapNormal.clear();
  std::list<OpacitySigVal>::iterator  opIt;
  std::list<HSVColorSigVal>::iterator colIt;
  for (opIt = opacityList.begin(); opIt!= opacityList.end(); opIt++)
  {
    OpacitySigVal opVal = *opIt;
    m_opacityMapNormal.push_back(opVal);
  }

  for (colIt = colorList.begin(); colIt!= colorList.end(); colIt++)
  {
    HSVColorSigVal colVal = *colIt;
    m_colorMapNormal.push_back(colVal);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setLowDensityThransferFunctions(std::list<HSVColorSigVal>& colorList,
                                                           std::list<OpacitySigVal>& opacityList)
{
  m_opacityMapLowDensityl.clear();
  m_colorMapLowDensity.clear();
  std::list<OpacitySigVal>::iterator  opIt;
  std::list<HSVColorSigVal>::iterator colIt;
  for (opIt = opacityList.begin(); opIt!= opacityList.end(); opIt++)
  {
    OpacitySigVal opVal = *opIt;
    m_opacityMapLowDensityl.push_back(opVal);
  }

  for (colIt = colorList.begin(); colIt!= colorList.end(); colIt++)
  {
    HSVColorSigVal colVal = *colIt;
    m_colorMapLowDensity.push_back(colVal);
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::setThreatTransferFunctions(std::list<HSVColorSigVal>& colorList,
                                                      std::list<OpacitySigVal>&  opacityList)
{
  m_opacityMapThreat.clear();
  m_colorMapThreat.clear();

  std::list<OpacitySigVal>::iterator  opIt;
  std::list<HSVColorSigVal>::iterator colIt;
  for (opIt = opacityList.begin(); opIt!= opacityList.end(); opIt++)
  {
    OpacitySigVal opVal = *opIt;
    m_opacityMapThreat.push_back(opVal);
  }

  for (colIt = colorList.begin(); colIt!= colorList.end(); colIt++)
  {
    HSVColorSigVal colVal = *colIt;
    m_colorMapThreat.push_back(colVal);
  }
}



//------------------------------------------------------------------------------
void VolumeRendererWidget::setupVolumRendererParams()
{
  m_volumeRenderer->setTransferFunctionValues(m_opacityMapNormal,
                                              m_colorMapNormal,
                                              m_materialTransitionMap,
                                              m_threatOffsetHsvMap,
                                              m_threatOffsetOpacityMap);

  m_volumeRenderer->setGPUStatus(m_useGPU);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::syncZoomText()
{
  double rZoom = m_viewRuler->getZoom();
  m_textLabel->setZoomValue(rZoom);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::syncZoomCount(int& zoomCount,
                                         int& threatZoomCount)
{
  this->setZoomCount(zoomCount,
                     threatZoomCount);
}



//------------------------------------------------------------------------------
void VolumeRendererWidget::initViewAlignment()
{
  this->resetMainRendererViewAlignment();
  this->resetThreatRendererViewAlignment();
  this->update();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::centerWindowOnPointAndExtent(QVector3D& p0,
                                                        QVector3D& extent,
                                                        bool useY)
{
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  //---------------------------------------------------------------
  // ARO-TODO: Get Better Scaling function.
  double bagBounds[6];
  m_volumeRenderer->getBounds(bagBounds);

  QVector3D bagBoundVec(fabs(bagBounds[1] - bagBounds[0]),
      fabs(bagBounds[3] - bagBounds[2]),
      fabs(bagBounds[5] - bagBounds[4]));

  if (extent.x() == 0) extent.setX(0.01);
  if (extent.y() == 0) extent.setY(0.01);
  if (extent.z() == 0) extent.setZ(0.01);



  QVector3D  ratioVec(bagBoundVec.x()/extent.x(),
                      bagBoundVec.y()/extent.y(),
                      bagBoundVec.z()/extent.z());


  double smallest = ratioVec.x();

  // Note: opertator threats dont use Y to determine zoom as it is ALWAYS the
  // size of the volume.
  if(useY)
  {
    if (smallest > ratioVec.y()) smallest = ratioVec.y();
  }

  if (smallest > ratioVec.z()) smallest = ratioVec.z();


  double zoomFactor = smallest;

  if (zoomFactor > RIGHT_THREAT_MAX_ZOOM)
  {
    zoomFactor = RIGHT_THREAT_MAX_ZOOM;
  }
  //---------------------------------------------------------------


  QVector3D pointZero(0, 0, 0);
  QVector3D volCenter = QVector3D((bagBounds[0] + bagBounds[1])/2.0,
      (bagBounds[2] + bagBounds[3])/2.0,
      (bagBounds[4] + bagBounds[5])/2.0);

  QVector3D focalPoint(p0.x() + extent.x()/2,
                       p0.y() + extent.y()/2,
                       p0.z() + extent.z()/2);

  QVector3D p1(p0.x() + extent.x(),
               p0.y() + extent.y(),
               p1.z() + extent.z());

  QVector3D flipP0;
  QVector3D flipP1;
  m_volumeRenderer->flipBoxInY(p0,
                               p1,
                               flipP0,
                               flipP1);

  QVector3D flipPointZero;
  QVector3D flipvolCenter;

  m_volumeRenderer->flipBoxInY(pointZero,
                               volCenter,
                               flipPointZero,
                               flipvolCenter);


  double flipCenterY = (flipP0.y() + flipP1.y())/2;
  focalPoint.setY(flipCenterY);


  double avgExtent = (extent.x() + extent.y() + extent.z())/3.0;

  vtkRenderer* renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  if (!renderer) return;

  //--------------------------------------------------------------------
  // ARO-DEBUG Create Green Sphere that lines up with center of volume.
  // m_volumeRenderer->removeCenteringSphereIfPresent();
  // m_volumeRenderer->createCenteringSphere(renderer, focalPoint);
  //--------------------------------------------------------------------
  vtkCamera* camera     = renderer->GetActiveCamera();
  if (!camera) return;
  camera->SetFocalPoint(focalPoint.x(),
                        focalPoint.y(),
                        focalPoint.z());


  QVector3D camPos(focalPoint.x(),
                   focalPoint.y() + avgExtent/500,
                   focalPoint.z());

  camera->SetPosition(camPos.x(), camPos.y(), camPos.z());
  m_volumeRenderer->resetCamera();
  camera->Zoom(zoomFactor);

  // Because we've just zoomed we need to re-set the focal point.
  camera->SetFocalPoint(focalPoint.x(),
                        focalPoint.y(),
                        focalPoint.z());

  this->repaint();

  //-------------------
  m_viewRuler->setRulerSize();
  m_viewRuler->initZoom();
  this->initThreatZoomIndex();
  //-------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::centerWindowOnPointAndExtentNew(QVector3D& p0,
                                                           QVector3D& extent)
{
  // std::cout << "%%%%%%%%%%%%%%% SLAB MODE Center Threat %%%%%%%%%%%%%%%" << std::endl;
  if (this->displayLabel().contains("Left"))
  {
    return;
  }
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }

  double characteristicLength = extent.x();
  if (extent.y() > characteristicLength)
  {
    characteristicLength = extent.y();
  }
  if (extent.z() > characteristicLength)
  {
    characteristicLength = extent.z();
  }

  QVector3D rawThreatCenter(p0.x() + extent.x()/2.0,
                            p0.y() + extent.y()/2.0,
                            p0.z() + extent.z()/2.0);

  QVector3D flipThreatCenter;
  m_volumeRenderer->flipPointInY(rawThreatCenter,
                                 flipThreatCenter);

  vtkRenderer* renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  if (!renderer)
  {
    std::string strError =
        "this->GetRenderWindow()->GetRenderers()->GetFirstRenderer() == nullptr";
    LOG(WARNING) << strError;
    return;
  }

  //--------------------------------------------------------------------
  // ARO-DEBUG Create Green Sphere that lines up with center of volume.
  // m_volumeRenderer->removeCenteringSphereIfPresent();
  // m_volumeRenderer->createCenteringSphere(renderer, flipThreatCenter);
  //---------------------------------------------------------------------
  vtkCamera* camera  = renderer->GetActiveCamera();

  camera->SetFocalPoint(flipThreatCenter.x(),
                        flipThreatCenter.y(),
                        flipThreatCenter.z());


  double boundWidth = characteristicLength/2.0;

  double bounds[6] = {(flipThreatCenter.x() - boundWidth),
                      (flipThreatCenter.x() + boundWidth),
                      (flipThreatCenter.y() - boundWidth),
                      (flipThreatCenter.y() + boundWidth),
                      (flipThreatCenter.z() - boundWidth),
                      (flipThreatCenter.z() + boundWidth)};

  renderer->ResetCamera(bounds);


  if (m_currentCutAxis == VREnums::X)
  {
    camera->SetViewUp(0, 1, 0);
    camera->SetPosition(flipThreatCenter.x() + RIGHT_SLAB_THREAT_ZOOM*characteristicLength,
                        flipThreatCenter.y(),
                        flipThreatCenter.z());
  }

  if (m_currentCutAxis == VREnums::Y)
  {
    camera->SetPosition(flipThreatCenter.x(),
                        flipThreatCenter.y() + RIGHT_SLAB_THREAT_ZOOM*characteristicLength,
                        flipThreatCenter.z());
  }

  if (m_currentCutAxis == VREnums::Z)
  {
    camera->SetViewUp(0, 1, 0);
    camera->SetPosition(flipThreatCenter.x(),
                        flipThreatCenter.y(),
                        flipThreatCenter.z() + RIGHT_SLAB_THREAT_ZOOM*characteristicLength);
  }

  camera->SetFocalPoint(flipThreatCenter.x(),
                        flipThreatCenter.y(),
                        flipThreatCenter.z());

  renderer->ResetCameraClippingRange(bounds);

  this->repaint();

  //-------------------
  m_viewRuler->setRulerSize();
  m_viewRuler->initZoom();
  this->initThreatZoomIndex();
  //-------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::slabCenterThreat(QVector3D& p0,
                                            QVector3D& extent)
{
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  vtkRenderer* renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

  if (!renderer) return;
  vtkCamera* camera     = renderer->GetActiveCamera();
  double bagBounds[6];
  m_volumeRenderer->getBounds(bagBounds);

  double avgExtent = (extent.x() + extent.y() + extent.z() )/3.0;

  QVector3D threatCenter( (p0.x() + extent.x()/2),
                          (p0.y() + extent.y()/2),
                          (p0.z() + extent.z()/2));

  QVector3D threatTop(threatCenter.x(),
                      threatCenter.y() + avgExtent/2,
                      threatCenter.z());

  QVector3D threatBottom(threatCenter.x(),
                         threatCenter.y() - avgExtent/2,
                         threatCenter.x());

  QVector3D flipThreatCenter;
  m_volumeRenderer->flipPointInY(threatCenter,
                                 flipThreatCenter);

  QVector3D flipThreatTop;
  QVector3D flipThreatBottom;

  m_volumeRenderer->flipPointInY(threatTop,
                                 flipThreatTop);

  m_volumeRenderer->flipPointInY(threatBottom,
                                 flipThreatBottom);

  if (m_currentCutAxis == VREnums::X)
  {
    QVector3D normY(0, 1, 0);
    double frustrumLength = 2.0*(bagBounds[1] - bagBounds[0]);
    double boundWidth = avgExtent/2.0;

    double viewUp[3]     = {normY.x(),
                            normY.y(),
                            normY.z()};

    double position[3]   = {flipThreatCenter.x(),
                            flipThreatCenter.y(),
                            flipThreatCenter.z()};

    double focus[3]      = {flipThreatCenter.x(),
                            flipThreatCenter.y(),
                            flipThreatCenter.z()};

    double testBounds[6] = {threatCenter.x() - boundWidth,
                            threatCenter.x() + boundWidth,
                            flipThreatBottom.y(),
                            flipThreatTop.y(),
                            threatCenter.z() - boundWidth,
                            threatCenter.z() + boundWidth};


    //---------------------------------------------------------
    // ARO ANSS-1028 fix.
    position[0] = frustrumLength -  flipThreatCenter.x();
    //---------------------------------------------------------
    camera->SetPosition(position);
    camera->SetViewUp(viewUp);
    camera->SetFocalPoint(focus);
    m_widgetRenderer->ResetCamera(testBounds);
    // std::cout << "%%%%%% SLAB THREAT RESIZE AXIS = X %%%%%%" << std::endl;
  }
  if (m_currentCutAxis == VREnums::Y)
  {
    // ARO bug-fix ANSS-1028
    QVector3D normMinusX(-1, 0, 0);
    double frustrumLength = 2.0*(bagBounds[3] - bagBounds[2]);

    double viewUp[3]     = {normMinusX.x(),
                            normMinusX.y(),
                            normMinusX.z()};

    double position[3]   = {flipThreatCenter.x(),
                            flipThreatCenter.y(),
                            flipThreatCenter.z()};

    double focus[3]      = {flipThreatCenter.x(),
                            flipThreatCenter.y(),
                            flipThreatCenter.z()};

    double testBounds[6] = {threatCenter.x() - avgExtent,
                            threatCenter.x() + avgExtent,
                            flipThreatBottom.y(),
                            flipThreatTop.y(),
                            threatCenter.z() - avgExtent,
                            threatCenter.z() + avgExtent};
    // ARO bug-fix ANSS-1028
    position[1] =  frustrumLength - flipThreatCenter.y();
    camera->SetPosition(position);
    camera->SetViewUp(viewUp);
    camera->SetFocalPoint(focus);
    m_widgetRenderer->ResetCamera(testBounds);
    // std::cout << "%%%%%% SLAB THREAT RESIZE AXIS = Y %%%%%%" << std::endl;
  }
  if (m_currentCutAxis == VREnums::Z)
  {
    QVector3D normY(0, 1, 0);
    double frustrumLength = 2.0*(bagBounds[5] - bagBounds[4]);

    double viewUp[3]     = {normY.x(),
                            normY.y(),
                            normY.z()};

    double position[3]   = {flipThreatCenter.x(),
                            flipThreatCenter.y(),
                            flipThreatCenter.z()};

    double focus[3]      = {flipThreatCenter.x(),
                            flipThreatCenter.y(),
                            flipThreatCenter.z()};

    double testBounds[6] = {threatCenter.x() - avgExtent,
                            threatCenter.x() + avgExtent,
                            flipThreatBottom.y(),
                            flipThreatTop.y(),
                            threatCenter.z() - avgExtent,
                            threatCenter.z() + avgExtent};

    // ARO bug-fix ANSS-1028
    position[2] =  frustrumLength - flipThreatCenter.z();
    camera->SetPosition(position);
    camera->SetViewUp(viewUp);
    camera->SetFocalPoint(focus);
    m_widgetRenderer->ResetCamera(testBounds);
    // std::cout << "%%%%%% SLAB THREAT RESIZE AXIS = Z %%%%%%" << std::endl;
  }
  //-------------------
  this->forceRedraw();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::centerCroppedThreatWindow(QVector3D& p0,
                                                     QVector3D& extent)
{
  //--------------------------------------------------------
  // std::cout << "%%%% centerCroppedThreatWindow("
  //           << "QVector3D& p0,QVector3D& extent)"
  //           << std::endl;
  // std::cout << "P0[x,y,z]: "
  //           << p0.x() << ", "
  //           << p0.y() << ", "
  //           << p0.z() << std::endl;
  // std::cout << "Extent [W,H,L]: "
  //           << extent.x() << ", "
  //           << extent.y() << ", "
  //           << extent.z()
  //           << std::endl;
  //--------------------------------------------------------
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }

  double volBounds[6];
  //------------------------------------------------
  // Bug fix: ANSS-1013
  // m_volumeRenderer->getBounds(volBounds);
  //
  m_volumeRenderer->getVolumeBounds(volBounds);
  //------------------------------------------------

  //------------------------------------------------------------------
  // std::cout << "%%%% m_volumeRenderer->getVolumeBounds(volBounds)"
  //           << std::endl;
  // std::cout << "volBounds[x0 , x1]: "
  //           << volBounds[0] <<  " , " << volBounds[1]
  //           << std::endl;
  // std::cout << "volBounds[y0 , y1]: "
  //           << volBounds[2] <<  " , " << volBounds[3]
  //           << std::endl;
  // std::cout << "volBounds[z0 , z1]: "
  //           << volBounds[4] <<  " , " << volBounds[5]
  //           << std::endl;
  //------------------------------------------------------------------

  QVector3D pointZero(0, 0, 0);
  QVector3D volCenter = QVector3D((volBounds[0] + volBounds[1])/2.0,
      (volBounds[2] + volBounds[3])/2.0,
      (volBounds[4] + volBounds[5])/2.0);

  QVector3D focalPoint(p0.x() + extent.x()/2,
                       p0.y() + extent.y()/2,
                       p0.z() + extent.z()/2);

  QVector3D p1(p0.x() + extent.x(),
               p0.y() + extent.y(),
               p1.z() + extent.z());

  QVector3D flipP0;
  QVector3D flipP1;
  m_volumeRenderer->flipBoxInY(p0,
                               p1,
                               flipP0,
                               flipP1);

  QVector3D flipPointZero;
  QVector3D flipvolCenter;

  m_volumeRenderer->flipBoxInY(pointZero,
                               volCenter,
                               flipPointZero,
                               flipvolCenter);


  double flipCenterY = (flipP0.y() + flipP1.y())/2;
  focalPoint.setY(flipCenterY);


  double offsetScale  = RIGHT_THREAT_VIEW_OFFSET_SCALE;
  double avgExtent    = (extent.x() + extent.y() + extent.z())/3.0;
  double cameraOffset = avgExtent*offsetScale;

  // std::cout << "%%%%%%%%%%% offsetScale: " << offsetScale << std::endl;

  vtkRenderer* renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  if (!renderer) return;

  double newBounds[6] = {(volCenter.x() - avgExtent),
                         (volCenter.x() + avgExtent),
                         (volCenter.y() - avgExtent),
                         (volCenter.y() + avgExtent),
                         (volCenter.z() - avgExtent),
                         (volCenter.z() + avgExtent)};

  //--------------------------------------------------------------------
  // ARO-DEBUG Create Green Sphere that lines up with center of volume.
  // m_volumeRenderer->removeCenteringSphereIfPresent();
  // m_volumeRenderer->createCenteringSphere(renderer, focalPoint);
  //--------------------------------------------------------------------
  vtkCamera* camera     = renderer->GetActiveCamera();
  if (!camera) return;
  if (!m_isSlabbing)
  {
    if (m_scannerDirLefToRight)
    {
      camera->SetViewUp(-1, 0, 0);
    }
    else
    {
      camera->SetViewUp(1, 0, 0);
    }
    renderer->ResetCamera(newBounds);

    camera->SetFocalPoint(focalPoint.x(),
                          focalPoint.y(),
                          focalPoint.z());

    QVector3D camPos(focalPoint.x(),
                     focalPoint.y() + cameraOffset,
                     focalPoint.z());

    camera->SetPosition(camPos.x(), camPos.y(), camPos.z());
    renderer->ResetCameraClippingRange(newBounds);
  }
  else
  {
    // ARO-NOTE: this case is for For LAPTOP_SALB VIEW ONLY
    // Since it combines a cropped and slabbed threat.
    if ( m_currentCutAxis == VREnums::Y )
    {
      if (m_scannerDirLefToRight)
      {
        camera->SetViewUp(-1, 0, 0);
      }
      else
      {
        camera->SetViewUp(1, 0, 0);
      }
      renderer->ResetCamera(newBounds);

      camera->SetFocalPoint(focalPoint.x(),
                            focalPoint.y(),
                            focalPoint.z());

      QVector3D camPos(focalPoint.x(),
                       focalPoint.y() + cameraOffset,
                       focalPoint.z());

      camera->SetPosition(camPos.x(), camPos.y(), camPos.z());
      renderer->ResetCameraClippingRange(newBounds);
    }
    else if (m_currentCutAxis == VREnums::X)
    {
      camera->SetViewUp(0, 1, 0);
      renderer->ResetCamera(newBounds);
      camera->SetFocalPoint(focalPoint.x(),
                            focalPoint.y(),
                            focalPoint.z());

      QVector3D camPos(focalPoint.x() + cameraOffset,
                       focalPoint.y(),
                       focalPoint.z());

      camera->SetPosition(camPos.x(), camPos.y(), camPos.z());
      renderer->ResetCameraClippingRange(newBounds);
    }
    else if (m_currentCutAxis == VREnums::Z)
    {
      camera->SetViewUp(0, 1, 0);
      renderer->ResetCamera(newBounds);
      camera->SetFocalPoint(focalPoint.x(),
                            focalPoint.y(),
                            focalPoint.z());

      QVector3D camPos(focalPoint.x(),
                       focalPoint.y(),
                       focalPoint.z() + cameraOffset);

      camera->SetPosition(camPos.x(), camPos.y(), camPos.z());
      renderer->ResetCameraClippingRange(newBounds);
    }
  }

  this->update();

  //-------------------
  m_viewRuler->setRulerSize();
  m_viewRuler->initZoom();
  this->initThreatZoomIndex();
  //-------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::centerCroppedThreatWindowNew(QVector3D& p0,
                                                        QVector3D& extent)
{
  Q_UNUSED(p0);
  // std::cout << "XXXXXXXXXX centerCroppedThreatWindowNew() : XXXXXXXXXXXXXX" << std::endl;

  if (this->displayLabel().contains("Left"))
  {
    return;
  }
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }

  double avgExtent = (extent.x() + extent.y() + extent.z())/3.0;

  double cropVolBounds[6];
  m_volumeRenderer->getVolumeBounds(cropVolBounds);

  QVector3D cropVolCenter((cropVolBounds[0] + cropVolBounds[1])/2.0,
      (cropVolBounds[2] + cropVolBounds[3])/2.0,
      (cropVolBounds[4] + cropVolBounds[5])/2.0);

  double extendenBounds[6] = { (cropVolCenter.x() - avgExtent ),
                               (cropVolCenter.x() + avgExtent ),
                               (cropVolCenter.y() - avgExtent ),
                               (cropVolCenter.y() + avgExtent ),
                               (cropVolCenter.z() - avgExtent ),
                               (cropVolCenter.z() + avgExtent )};

  vtkRenderer* renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  if (!renderer)
  {
    std::string strError =
        "this->GetRenderWindow()->GetRenderers()->GetFirstRenderer() == nullptr";
    LOG(WARNING) << strError;
    return;
  }
  //--------------------------------------------------------------------
  // ARO-DEBUG Create Green Sphere that lines up with center of volume.
  // m_volumeRenderer->removeCenteringSphereIfPresent();
  // m_volumeRenderer->createCenteringSphere(renderer, cropVolCenter);
  //---------------------------------------------------------------------
  vtkCamera* camera  = renderer->GetActiveCamera();

  double cameraOffset = 8*avgExtent;

  camera->SetFocalPoint(cropVolCenter.x(),
                        cropVolCenter.y(),
                        cropVolCenter.z());

  renderer->ResetCamera(extendenBounds);

  camera->SetFocalPoint(cropVolCenter.x(),
                        cropVolCenter.y(),
                        cropVolCenter.z());


  camera->SetPosition(cropVolCenter.x(),
                      cropVolCenter.y() - cameraOffset,
                      cropVolCenter.z());


  renderer->ResetCameraClippingRange(extendenBounds);

  this->repaint();

  //-------------------
  m_viewRuler->setRulerSize();
  m_viewRuler->initZoom();
  this->initThreatZoomIndex();
  //-------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::resetMainRendererViewAlignment()
{
  // std::cout << "****** resetMainRendererViewAlignment() *******"
  //           << std::endl;

  if (this->isAThreatRenderer())
  {
    this->resetThereatViewAlignment();
    return;
  }
  m_volumeRenderer->resetCamera();

  double fp[3];
  double pos[3];
  fp[0] = m_widgetRenderer->GetActiveCamera()->GetFocalPoint()[0];
  fp[1] = m_widgetRenderer->GetActiveCamera()->GetFocalPoint()[1];
  fp[2] = m_widgetRenderer->GetActiveCamera()->GetFocalPoint()[2];
  pos[0] = m_widgetRenderer->GetActiveCamera()->GetPosition()[0];
  pos[1] = m_widgetRenderer->GetActiveCamera()->GetPosition()[1];
  pos[2] = m_widgetRenderer->GetActiveCamera()->GetPosition()[2];

  double dist = std::sqrt( (pos[0]- fp[0])*(pos[0] - fp[0]) +
      (pos[1]- fp[1])*(pos[1] - fp[1]) +
      (pos[2]- fp[2])*(pos[2] - fp[2])  );

  // std::cout << "=================================================================" << std::endl;
  // std::cout << "camera pos [x, y, z]: " << pos[0] << ", " << pos[1]  << ", " << pos[2] << std::endl;
  // std::cout << "focal pos  [x, y, z]: " <<  fp[0] << ", " <<  fp[1]  << ", " <<  fp[2] << std::endl;
  // std::cout << "=================================================================" << std::endl;

  m_widgetRenderer->GetActiveCamera()->SetPosition(fp[0], fp[1], fp[2] + dist);
  m_widgetRenderer->GetActiveCamera()->SetViewUp(0.0, 1.0, 0.0);


  // Angles here are defined in Degrees.
  double rollAngle = 0;
  double yawAngle  = 0;
  if (m_scannerDirLefToRight)
  {
    rollAngle = -90.0;
    yawAngle  = -90.0;
  }
  else
  {
    rollAngle = 90.0;
    yawAngle  = 90.0;
  }
  this->yawView(m_widgetRenderer,  yawAngle);
  this->rollView(m_widgetRenderer, rollAngle);
  m_volumeRenderer->resetCamera();
  m_widgetRenderer->GetActiveCamera()->Zoom(g_mainViewerInitialZoom);

  //----------------------------
  // Finally set zoom settings.
  if (this->isVolumeInitialized())
  {
    m_viewRuler->setRulerSize();
    m_viewRuler->initZoom();
    this->initZoomIndex();
  }
  //----------------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::slabResetView()
{
  // Bounds: (xmin,xmax, ymin,ymax, zmin,zmax).
  double bounds[6];
  m_volumeRenderer->getBounds(bounds);

  m_volumeRenderer->resetCameraWithBounds(bounds);
  m_widgetRenderer->GetActiveCamera()->Zoom(g_mainViewerInitialZoom);

  // Finally set zoom settings.
  if (this->isVolumeInitialized())
  {
    m_viewRuler->setRulerSize();
    m_viewRuler->initZoom();
    this->initZoomIndex();
  }
}

//-------------------------------------------------------------------
void VolumeRendererWidget::hardResetRendreYView()
{
  double volBounds[6];
  m_volumeRenderer->getBounds(volBounds);
  double H = volBounds[3] - volBounds[2];

  QVector3D focalPoint((volBounds[0] + volBounds[1])/2.0,
      (volBounds[2] + volBounds[3])/2.0,
      (volBounds[4] + volBounds[5])/2.0);

  QVector3D cameraPosition(focalPoint.x(),
                           focalPoint.y() + H ,
                           focalPoint.z());

  vtkCamera* thisRenderCamera  = this->getRenderCamera();
  vtkRenderer* renderer       = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

  thisRenderCamera->SetViewUp(0, 0, -1);


  thisRenderCamera->SetFocalPoint(focalPoint.x(),
                                  focalPoint.y(),
                                  focalPoint.z());

  thisRenderCamera->SetPosition(cameraPosition.x(),
                                cameraPosition.y(),
                                cameraPosition.z());

  renderer->ResetCamera(volBounds);


  renderer->ResetCameraClippingRange(volBounds);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::resetThreatRendererViewAlignment()
{
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::resetThereatViewAlignment()
{
  // ARO-How to Reset Threat Window???
  //

  VolumeRendererWidget* leftViewer = nullptr;
  VolumeRendererWidget* rightViewer = nullptr;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);

  if (!leftViewer) return;
  if (leftViewer->ThreatCount() == 0) return;
  ThreatObject* pThr = leftViewer->getCurrentThreatObject();
  if (!pThr) return;


  analogic::workstation::ThreatGenerationType gen_type = pThr->getGenType();
  ThreatVolume tv = pThr->getVolume();

  QVector3D p0 = tv.p0();
  QVector3D p1 = tv.p1();

  QVector3D extent(p1.x() - p0.x(),
                   p1.y() - p0.y(),
                   p1.z() - p0.z());

  vtkCamera*      camera = m_widgetRenderer->GetActiveCamera();
  if (!camera) return;
  double renderFocus[3];
  camera->GetFocalPoint(renderFocus);

  double fp[3];
  double pos[3];
  fp[0]  = m_widgetRenderer->GetActiveCamera()->GetFocalPoint()[0];
  fp[1]  = m_widgetRenderer->GetActiveCamera()->GetFocalPoint()[1];
  fp[2]  = m_widgetRenderer->GetActiveCamera()->GetFocalPoint()[2];
  pos[0] = m_widgetRenderer->GetActiveCamera()->GetPosition()[0];
  pos[1] = m_widgetRenderer->GetActiveCamera()->GetPosition()[1];
  pos[2] = m_widgetRenderer->GetActiveCamera()->GetPosition()[2];

  double dist = std::sqrt((pos[0]- fp[0])*(pos[0] - fp[0]) +
      (pos[1]- fp[1])*(pos[1] - fp[1]) +
      (pos[2]- fp[2])*(pos[2] - fp[2]) );



  m_widgetRenderer->GetActiveCamera()->SetPosition(fp[0], fp[1], fp[2] + dist);
  camera->SetViewUp(0.0, 1.0, 0.0);


  // Angles here are defined in Degrees.
  double rollAngle = 0;
  double yawAngle  = 0;
  if (m_scannerDirLefToRight)
  {
    rollAngle = -90.0;
    yawAngle  = -90.0;
  }
  else
  {
    rollAngle = 90.0;
    yawAngle  = 90.0;
  }
  this->yawView(m_widgetRenderer,  yawAngle);
  this->rollView(m_widgetRenderer, rollAngle);
  bool useY = true;
  if (gen_type == analogic::workstation::OPERATOR_GEN)
  {
    useY = false;
  }
  rightViewer->centerWindowOnPointAndExtent(p0,
                                            extent,
                                            useY);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::initThreatList()
{
  double psX, psY, psZ;
  m_volumeRenderer->getPixelSpacing(psX, psY, psZ);
  QVector3D imageOffset(0, 0, 0);
  if (!m_threatList)
  {
    m_threatList = new ThreatList(psX,
                                  psY,
                                  psZ,
                                  imageOffset,
                                  m_enableThreatViewing);

    m_threatList->setVolumeRenderer(m_volumeRenderer);
  }
  m_threatList->clear();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::initRendererThreatOffsetMap()
{
  // use std::set to avoid repeats.
  std::set<uint16_t> offsets_Set;
  std::set<uint16_t> test_Set;
  for (int i = 0; i < m_threatList->size(); i++)
  {
    ThreatObject* pThr = m_threatList->getThreatAtIndex(i);
    if (pThr)
    {
      uint16_t offVal = pThr->getOffset();
      if ( (offVal == m_materialTransitionMap[SIG_THREAT_SET_A]) ||
           (offVal == m_materialTransitionMap[SIG_THREAT_SET_B]) ||
           (offVal == m_materialTransitionMap[SIG_THREAT_SET_C])
           )
      {
        offsets_Set.insert(offVal);
        test_Set.insert(offVal);
      }
    }
  }
  int  vtk_index = 1;
  std::list<ThretOffsetProperty> threatPropertyList;
  if (offsets_Set.size() > 0)
  {
    std::set<uint16_t>::iterator itt;
    for (itt = offsets_Set.begin();
         itt != offsets_Set.end();
         itt++)
    {
      uint16_t offVal = *itt;
      ThretOffsetProperty prop(offVal, vtk_index);
      threatPropertyList.push_back(prop);
      vtk_index++;
    }
  }
  /*
  std::cout << "***** ThreatOffsetProperty list size ="
            << threatPropertyList.size()
            << std::endl;

  std::cout << "**** Test SET zize = "
            << test_Set.size()
            << std::endl;

  std::cout << "***** Threat List size ="
            << m_threatList->size()
            << std::endl;
 */
  m_volumeRenderer->setThreatOffsetPropertyList(threatPropertyList);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::rightAapplyLowDensity(bool setVal)
{
  // This is the right viewer.
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  if (m_isSlabbing || m_isThreatRenderer)
  {
    m_volumeRenderer->setLowDensityActive(setVal);
    if (setVal)
    {
      if (m_isSlabbing)
      {
        std::list<OpacitySigVal>  applyOpacityList;
        VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();
        rightRenderer->increaseOpacityOnThinSlice(leftViewer->getLowDensityOpacityList(), applyOpacityList);
        m_volumeRenderer->setTransferfunctionsInderect(leftViewer->getLowDensityHsvColorList(),
                                                       applyOpacityList);
      }
      else
      {
        m_volumeRenderer->setTransferfunctionsInderect(leftViewer->getLowDensityHsvColorList(),
                                                       leftViewer->getLowDensityOpacityList());
      }
    }
    else
    {
      if (m_isSlabbing)
      {
        std::list<OpacitySigVal>  applyOpacityList;
        VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();
        rightRenderer->increaseOpacityOnThinSlice(leftViewer->getNormalOpacityList(), applyOpacityList);
        m_volumeRenderer->setTransferfunctionsInderect(leftViewer->getNormalHsvColorList(),
                                                       applyOpacityList);
      }
      else
      {
        m_volumeRenderer->setTransferfunctionsInderect(leftViewer->getNormalHsvColorList(),
                                                       leftViewer->getNormalOpacityList());
      }
    }
    leftViewer->reDisplayCurrentThreat();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::worldToImageVolumeRange(QVector3D  p0wld,
                                                   QVector3D  p1wld,
                                                   QVector3D& p0img,
                                                   QVector3D& p1img)
{
  double psX, psY, psZ;
  m_volumeRenderer->getPixelSpacing(psX, psY, psZ);

  QVector3D P0 = p0wld;
  QVector3D P1 = p1wld;

  double x0 = P0.x();
  double x1 = P1.x();

  double y0 = P0.y();
  double y1 = P1.y();

  double z0 = P0.z();
  double z1 = P1.z();


  P0.setX(x0*psX);
  P0.setY(y0*psY);
  P0.setZ(z0*psZ);


  P1.setX(x1*psX);
  P1.setY(y1*psY);
  P1.setZ(z1*psZ);

  p0img = P0;
  p1img = P1;
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::imageToWorldVolumeRange(QVector3D   p0img,
                                                   QVector3D  p1img,
                                                   QVector3D& p0wld,
                                                   QVector3D& p1wld)
{
  double psX, psY, psZ;
  m_volumeRenderer->getPixelSpacing(psX, psY, psZ);

  QVector3D P0 = p0img;
  QVector3D P1 = p1img;

  double x0 = P0.x();
  double x1 = P1.x();

  double y0 = P0.y();
  double y1 = P1.y();

  double z0 = P0.z();
  double z1 = P1.z();


  P0.setX(x0/psX);
  P0.setY(y0/psY);
  P0.setZ(z0/psZ);


  P1.setX(x1/psX);
  P1.setY(y1/psY);
  P1.setZ(z1/psZ);

  p0wld = P0;
  p1wld = P1;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::createVTKThreatActors()
{
  int numThreats = m_threatList->size();

  ThreatBoxTypeColorMap threatBoxColorMap = analogic::workstation::getThreatBoxColorMap();

  for (int i = 0; i < numThreats; i++)
  {
    ThreatObject* pThr = m_threatList->getThreatAtIndex(i);
    if (pThr)
    {
      ThreatVolume aVol = pThr->getVolume();
      //---------------------------------
      // Convert from World to Pixel Scaling
      QVector3D pImg0;
      QVector3D pImg1;
      this->worldToImageVolumeRange(aVol.p0(),
                                    aVol.p1(),
                                    pImg0,
                                    pImg1);

      ThreatVolume newVol(pImg0, pImg1);
      //-----------------------------------
      double boundsRed;
      double boundsGreen;
      double boundsBlue;


      analogic::workstation::ThreatGenerationType genType = pThr->getGenType();
      if (genType == ThreatGenerationType::ATR_GEN)
      {
        analogic::workstation::ThreatAlarmType alarmType = pThr->getType();

        QColor threatBoxColor = threatBoxColorMap[alarmType];
        boundsRed   = static_cast<double>(threatBoxColor.red()    )/255.0;
        boundsGreen = static_cast<double>(threatBoxColor.green()  )/255.0;
        boundsBlue  = static_cast<double>(threatBoxColor.blue()   )/255.0;
      }
      else
      {
        QColor threatBoxColor = analogic::workstation::getOperatorthreatBoxColor();
        boundsRed   = static_cast<double>(threatBoxColor.red()    )/255.0;
        boundsGreen = static_cast<double>(threatBoxColor.green()  )/255.0;
        boundsBlue  = static_cast<double>(threatBoxColor.blue()   )/255.0;
      }
      bool bIsShield = false;
      if (pThr->getType() == analogic::workstation::ThreatAlarmType::SHIELD)
      {
        bIsShield = true;
      }

      vtkSmartPointer<vtkActor> anActor =  m_volumeRenderer->createThreatVisual(newVol,
                                                                                boundsRed,
                                                                                boundsGreen,
                                                                                boundsBlue,
                                                                                bIsShield);

      pThr->setActor(anActor);
    }
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onVolumeRenderDBGUpdate(QString message)
{
  qDebug() <<  message;
  if ( message.contains("Stop Event Timer:", Qt::CaseSensitive) )
  {
    // Some Timer message! Log it .
    LOG(INFO) << message.toLocal8Bit().data();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onVolumeRenderGenericMessage(int& msgID)
{
  emit this->genericVRWidgetMessage(msgID);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onVolumeRenderTimerMessage(int& msgID,
                                                      int& timeVal)
{
  emit this->timerVRWidgetMessage(msgID, timeVal);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::onThreatRenderTimerMessage(int& msgID,
                                                      int& timeVal)
{
  emit this->timerVRWidgetMessage(msgID, timeVal);
}




//------------------------------------------------------------------------------
void VolumeRendererWidget::onReadUpdate(double updateVal)
{
  qDebug() << "Read Update % : " << updateVal;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onImageLoadComplete(int msLoad)
{
  Q_UNUSED(msLoad);
  m_volumeRenderer->setOpacityValueLevel(m_contrastSliderVal*100);
  m_volumeRenderer->brightenColor(m_brightnessSliderVal*100);
  m_volumeRenderer->setOpacitySignalLevel(m_currentOpacityValue*100);


  m_volumeRenderer->setLightingParams(m_lightingAmbient,
                                      m_lightingDiffuse,
                                      m_lightingSpecular);

  int msBuild = m_volumeRenderer->getRenderBuildTime();
  QString strMessageBld =  "HHHHHHH, Render Build Time (ms),";
  strMessageBld += QString::number(msBuild);
  qDebug() << strMessageBld;

  this->initViewAlignment();


  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);


  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();
  this->hideSubVolume();
  firstSib->hideSubVolume();

  //----------------------------------------------------------
  // Special case. Only Laptops and no other machine threats.
  if (this->displayLabel().contains("Right")  &&
      (leftViewer->ThreatCount() == 0))
  {
    QString threatLabel = "";
    this->setRawThreatLabel(threatLabel);
    this->SetAsThreatRenderer(false);
    this->SetSyncEnabled(true);

    rightViewer->SetRightViewerDisplayMode(VREnums::VIEW_AS_SURFACE);
    VolumeRenderer* leftRenderer = leftViewer->getVolumeRenderer();
    if (leftRenderer)
    {
      leftRenderer->hideLaptopThreats();
    }
    if (leftViewer->isVolumeInitialized())
    {
      LOG(INFO) << "**TST-SYNC** - Only LAPTOPS and No MACHINE Threats Special Sync Siblings ... " << std::endl;
      //---------------------------------------------
      // ARO-NOTE: This is causng the Integrated
      // Volume Renderer to crash Qt App
      // this->syncSibling();
      // We might need to re-test Workstation.
      //---------------------------------------------
    }
  }
  //----------------------------------------------------------
  if (this->displayLabel().contains("Left"))
  {
    this->DisplayThreatOnly(m_threatOnlyViewStatus);
  }
  this->setVolumeInitialized(true);
  emit this->initComplete();
  this->stopLoadTimers();
  LOG(INFO) << "IMAGE_LOAD_COMPLETE";
  //--------------------------------------------
  // ARO-DEBUGGING ONLY:
  // this->logBagSizeInfo();
  //--------------------------------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onVolumeMeasurementComplete()
{
  this->setCursor(Qt::ArrowCursor);
  this->forceRedraw();
  this->logKeyStrokeEvent("VM", "");
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onReadDataRange(double rangeX,
                                           double rangeY,
                                           double rangeZ)
{
  QVector3D p0(0, 0, 0);
  QVector3D p1(rangeX,
               rangeY,
               rangeZ);
  qDebug() << "Range P0:" << p0;
  qDebug() << "Range P1:" << p1;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onThreatLoadComplete()
{
  this->triggerThreatInitTimer();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::onVolumeRenderMemRequest(uint64_t memRequested,
                                                    uint64_t memAvailable)
{
  QString message = "Memory requested: " +
      QString::number(memRequested/(1024 * 1024)) + " MBytes \nMemory Availiable: " +
      QString::number(memAvailable/(1024 * 1024)) + " MBytes";

  qDebug() << message;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onSliderRedraw()
{
  QTimer::singleShot(g_cutViewTimeout,
                     this,
                     &VolumeRendererWidget::onPercentSliderUpdate);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onCameraPositionChanged()
{
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  this->syncSibling();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onThreatCameraPositionChanged()
{
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onMainVolumePick(QVector3D& aPoint)
{
  bool isPickerPicking = VolumeRendererWidget::pickerActive();
  if (isPickerPicking) return;
  int id  = VOLUME_PICK_POINT_ID;
  emit this->positionVRWidgetMessage(id, aPoint);
  m_lastRightPickPoint = aPoint;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::measureItemVolumeByValue()
{
  if (this->isAThreatRenderer())
  {
    VolumeRendererWidget* leftViewer;
    VolumeRendererWidget* rightViewer;
    this->getLeftAndRightViewers(&leftViewer,
                                 &rightViewer);

    ThreatObject* pThreat = leftViewer->getCurrentThreatObject();
    if (!pThreat)
    {
      return;
    }
    ThreatVolume tv = pThreat->getVolume();
    QVector3D thrp0 = tv.p0();

    int W = tv.p1().x() - tv.p0().x();
    int H = tv.p1().y() - tv.p0().y();
    int L = tv.p1().z() - tv.p0().z();

    analogic::workstation::ThreatGenerationType genType =
        pThreat->getGenType();

    if (genType != analogic::workstation::OPERATOR_GEN )
    {
      SDICOS::Bitmap* pBMP = pThreat->getBMP();
      m_volumeRenderer->measureVolumeByDICOSBitmap(thrp0,
                                                   W,
                                                   H,
                                                   L,
                                                   pBMP);
    }
    else
    {
      if (g_operatorThreatVolumeMethod == FLYING_EDGES)
      {
        m_volumeRenderer->measureVolumeByValueFlyingEdge(thrp0,
                                                         W,
                                                         H,
                                                         L);
      }
      else
      {
        m_volumeRenderer->measureVolumeByValueMarchingCube(thrp0,
                                                           W,
                                                           H,
                                                           L);
      }
    }
    this->forceRedraw();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onVolumeMeasurementByPick(QVector3D&  worldPoint)
{
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);

  VolumeRenderer* leftRenderer = leftViewer->getVolumeRenderer();

  QVector3D rawPoint;
  leftRenderer->flipPointInY(worldPoint,
                             rawPoint);

  double pix_x, pix_y, pix_z;
  leftRenderer->getPixelSpacing(pix_x,
                                pix_y,
                                pix_z);

  QVector3D imagePoint(rawPoint.x()/pix_x,
                       rawPoint.y()/pix_y,
                       rawPoint.z()/pix_z);

  uint16_t startPoint = leftRenderer->getImageValueAt(imagePoint);

  //--------------------------------------------------
  // LOG start point and start value.
  QString logMessage = "Volume Measurement - Image voxel index   [x,y,z]: ";
  logMessage += QString::number(imagePoint.x()) + ", ";
  logMessage += QString::number(imagePoint.y()) + ", ";
  logMessage += QString::number(imagePoint.z());
  LOG(INFO) << logMessage.toStdString();
  // std::cout << logMessage.toStdString() << std::endl;

  logMessage = "Volume Measurement - Computation start value    : ";
  logMessage +=  QString::number(startPoint);
  LOG(INFO) << logMessage.toStdString();
  // std::cout << logMessage.toStdString() << std::endl;
  //--------------------------------------------------
  this->setCursor(Qt::BusyCursor);
  m_volumeRenderer->setVolumeMeasurementSetPoint(startPoint);
  this->measureItemVolumeByValue();
  this->setCursor(Qt::ArrowCursor);
  //--------------------------------------------------------------------
  // ARO-DEBUG Create Green Sphere that lines up with world point input
  // vtkRenderer* renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  // if (!renderer)return;
  // m_volumeRenderer->removeCenteringSphereIfPresent();
  // m_volumeRenderer->createCenteringSphere(renderer, worldPoint);
  //--------------------------------------------------------------------
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::disableDirectDistanceMeasurement()
{
  m_distanceMeasureToolActive = false;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::distanceMeasurementReset()
{
  m_distanceWidget->SetWidgetStateToStart();
  this->setDistanceMeasureStatus(DISTANCE_MEASURE_START);
}

//------------------------------------------------------------------------------
bool VolumeRendererWidget::isDistanceMeasurementComplete()
{
  return m_distacneMeasurementComplete;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::distanceMeasurementEndInteraction()
{
  double measureP0[3];
  double measureP1[3];
  m_distanceRep->GetPoint1WorldPosition(measureP0);
  m_distanceRep->GetPoint2WorldPosition(measureP1);
  QVector3D distCur((measureP1[0] - measureP0[0]),
      (measureP1[1] - measureP0[1]),
      (measureP1[2] - measureP0[2]));

  // std::cout << "[:^)][:^)][:^)]  Current Measured length = "
  //           << distCur.length()
  //           << std::endl;

  if (distCur.length() < 0.3)
  {
    m_distanceRep->SetPoint1WorldPosition(measureP0);
    m_distanceRep->SetPoint2WorldPosition(measureP0);
    this->setDistanceMeasureStatus(DISTANCE_MEASURE_START);
    // std::cout << "[:^)][:^)][:^)]  *** TOUCH RESET LENGTH ****"
    //           << std::endl;
  }
  else
  {
    m_distanceWidget->StopInteraction();
  }
  m_distacneMeasurementComplete = true;
  //------------------------------
  this->logKeyStrokeEvent("DM", "");
  //------------------------------
}

//------------------------------------------------------------------------------
double VolumeRendererWidget::distanceMeasurementCurrentLength()
{
  double measureP0[3];
  double measureP1[3];
  m_distanceRep->GetPoint1WorldPosition(measureP0);
  m_distanceRep->GetPoint2WorldPosition(measureP1);
  QVector3D distCur((measureP1[0] - measureP0[0]),
      (measureP1[1] - measureP0[1]),
      (measureP1[2] - measureP0[2]));
  return distCur.length();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::measurePickPointFromSibling(QVector3D& worldPoint,
                                                       bool bFirst)
{
  QString measureFromStr = "measurePickPointFromSibling First Point: ";
  if (bFirst)
  {
    measureFromStr += "TRUE";
  }
  else
  {
    measureFromStr += "FALSE";
  }

  measureFromStr += "- World Point [x, y, z]:";
  measureFromStr += QString::number(worldPoint.x()) + ", ";
  measureFromStr += QString::number(worldPoint.y()) + ", ";
  measureFromStr += QString::number(worldPoint.x());

  std::cout <<  measureFromStr.toStdString() << std::endl;
  if(bFirst)
  {
    m_distanceWidget->On();
    m_distanceWidget->SetWidgetStateToStart();


    double arrayP0[3] = {worldPoint.x(), worldPoint.y(), worldPoint.z()};
    m_distanceRep->SetPoint1WorldPosition(arrayP0);
    m_distanceWidget->On();
  }
  else
  {
    double arrayP1[3] = {worldPoint.x(), worldPoint.y(), worldPoint.z()};
    m_distanceRep->SetPoint2WorldPosition(arrayP1);
  }
}


//------------------------------------------------------------------------------
std::list<OpacitySigVal>&  VolumeRendererWidget::getNormalOpacityList()
{
  return  m_opacityMapNormal;
}


//------------------------------------------------------------------------------
std::list<HSVColorSigVal>& VolumeRendererWidget::getNormalHsvColorList()
{
  return m_colorMapNormal;
}

//------------------------------------------------------------------------------
std::list<OpacitySigVal>&  VolumeRendererWidget::getLowDensityOpacityList()
{
  return m_opacityMapLowDensityl;
}

//------------------------------------------------------------------------------
std::list<HSVColorSigVal>& VolumeRendererWidget::getLowDensityHsvColorList()
{
  return m_colorMapLowDensity;
}

//------------------------------------------------------------------------------
bool VolumeRendererWidget::distanceMeasurePickPoint(QVector3D& worldPoint,
                                                    bool bFirst)
{
  bool retVal = false;
  if(bFirst)
  {
    // Chek if we need  to reset it. if too far away.
    double currentP0[3];
    double currentP1[3];
    m_distanceRep->GetPoint1WorldPosition(currentP0);
    m_distanceRep->GetPoint2WorldPosition(currentP1);

    QVector3D meanCurrent((currentP1[0] + currentP0[0])/2.0,
        (currentP1[1] + currentP0[1])/2.0,
        (currentP1[2] + currentP0[2])/2.0);

    QVector3D lengthCur(fabs(currentP1[0] - currentP0[0]),
        fabs(currentP1[1] - currentP0[1]),
        fabs(currentP1[2] - currentP0[2]));

    QVector3D distFromCurrent( (meanCurrent.x() - worldPoint.x()),
                               (meanCurrent.y() - worldPoint.y()),
                               (meanCurrent.z() - worldPoint.z()));
    if ( (lengthCur.length() < TOUCH_MEASURE_RESET_SEPERATION_MINIMUM) &&
         (distFromCurrent .length() > TOUCH_MEASURE_RESET_DISTANCE )
         )
    {
      // std::cout << "Length Current: " << lengthCur.length()
      //          << " - Dist from current:  " << distFromCurrent.length()
      //          << std::endl;
      retVal = true;
    }
    double arrayP0[3] = {worldPoint.x(), worldPoint.y(), worldPoint.z()};
    m_distanceRep->SetPoint1WorldPosition(arrayP0);
    m_distanceRep->SetPoint2WorldPosition(arrayP0);
    m_distanceWidget->On();
    m_distanceWidget->SetWidgetStateToStart();
    m_distanceWidget->GrabFocus(nullptr, nullptr);
  }
  else
  {
    // std::cout << "Second Point: m_distanceRep->SetPoint2WorldPosition(arrayP1)"
    //           << std::endl;

    double arrayP1[3] = {worldPoint.x(), worldPoint.y(), worldPoint.z()};
    m_distanceRep->SetPoint2WorldPosition(arrayP1);
  }
  return retVal;
}


void VolumeRendererWidget::distanceMeasurePickPointTouch(QVector3D& worldPoint)
{
  double arrayP0[3] = {worldPoint.x(), worldPoint.y(), worldPoint.z()};
  m_distanceRep->SetPoint1WorldPosition(arrayP0);
  m_distanceRep->SetPoint2WorldPosition(arrayP0);
  m_distanceWidget->On();
  m_distanceWidget->SetWidgetStateToStart();
  m_distanceWidget->GrabFocus(nullptr, nullptr);
}



//------------------------------------------------------------------------------
void VolumeRendererWidget::distancMeasureZeroLengthAt(QVector2D& screenPoint)
{
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      this->GetInteractor();
  if (renderWindowInteractor)
  {
    double picked[3];
    renderWindowInteractor->GetPicker()->Pick(screenPoint.x(),
                                              screenPoint.y(),
                                              0,  // always zero.
          renderWindowInteractor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
    renderWindowInteractor->GetPicker()->GetPickPosition(picked);

    m_distanceRep->SetPoint1WorldPosition(picked);
    m_distanceRep->SetPoint2WorldPosition(picked);
    m_distacneMeasurementComplete = false;
  }
}

//------------------------------------------------------------------------------
bool VolumeRendererWidget::isDistancToolStateManipulate()
{
  int  widgetState = m_distanceWidget->GetWidgetState();

  std::cout << "Get Widget State for Measureing tool =" << widgetState << std::endl;
  if (widgetState == 2)
  {
    return true;
  }
  return false;
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::getViewCenter(QVector3D& viewCenter)
{
  double volBounds[6];
  // (xmin, xmax, ymin, ymax, zmin, zmax)
  m_volumeRenderer->getVolumeBounds(volBounds);
  viewCenter.setX((volBounds[0] + volBounds[1])/2.0);
  viewCenter.setY((volBounds[2] + volBounds[3])/2.0);
  viewCenter.setZ((volBounds[4] + volBounds[5])/2.0);
}


//------------------------------------------------------------------------------
bool VolumeRendererWidget::isToucPointNearHandle(QPoint touchPoint,
                                                 QPoint& retPoint)
{
  bool retVal = false;
  double P0[3], P1[3];
  m_distanceRep->GetPoint1DisplayPosition(P0);
  m_distanceRep->GetPoint2DisplayPosition(P1);

  QPoint handleP0(P0[0], P0[1]);
  QPoint handleP1(P1[0], P1[1]);

  QVector3D distP0( (handleP0.rx() - touchPoint.rx()),
                    (handleP0.ry() - touchPoint.ry()),
                    0);

  QVector3D distP1( (handleP1.rx() - touchPoint.rx()),
                    (handleP1.ry() - touchPoint.ry()),
                    0);

  vtkPointHandleRepresentation3D* handle2Rep =
      reinterpret_cast<vtkPointHandleRepresentation3D*>(m_distanceRep->GetPoint2Representation());
  vtkPointHandleRepresentation3D* handle1Rep =
      reinterpret_cast<vtkPointHandleRepresentation3D*>(m_distanceRep->GetPoint1Representation());

  if (distP0.length() < TOUCH_MEASURE_HANDLE_PROXIMITY)
  {
    if (handle1Rep && handle2Rep)
    {
      handle1Rep->Highlight(1);
      handle2Rep->Highlight(0);
      m_distanceWidget->RestartMeasurement();
    }
    // std::cout << "VolumeRendererWidget::isToucPointNearHandle() Handle 0 distance from touch: "
    //           << distP0.length() << std::endl;
    retPoint.setX(handleP0.rx());
    retPoint.setY(handleP0.ry());
    retVal = true;
  }
  if (distP1.length() < TOUCH_MEASURE_HANDLE_PROXIMITY)
  {
    if (handle1Rep && handle2Rep)
    {
      handle1Rep->Highlight(0);
      handle2Rep->Highlight(1);
      m_distanceWidget->RestartMeasurement();
    }
    // std::cout << "VolumeRendererWidget::isToucPointNearHandle() Handle 1 distance from touch: "
    //           << distP0.length() << std::endl;
    retPoint.setX(handleP1.rx());
    retPoint.setY(handleP1.ry());
    retVal = true;
  }
  return retVal;
}

//------------------------------------------------------------------------------
void  VolumeRendererWidget::logKeyStrokeEvent(QString keyStroke, QString alarm_type)
{
  //--------------------------------------------
  // ARO-DEBUG:
  // std::cout <<  "Log key-stroke event: "
  //           <<  keyStroke.toStdString()
  //           <<  "  - Alarm Type: "
  //           <<  alarm_type.toStdString()
  //           <<  std::endl;
  //--------------------------------------------

  boost::shared_ptr<analogic::ws::BagData> bagData = analogic::ws::BagsDataPool::getInstance()->
      getBagbyState(analogic::ws::BagData::RENDERING);
  if (bagData)
  {
     bagData->addKeystroke(keyStroke, alarm_type);
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::disableMeasurementWidget()
{
  m_distanceWidget->Off();
  m_distanceMeasureToolActive = false;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onBeginInteractiveRender()
{
  this->removeSplash();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onPaintComplete()
{
  if ( this->isSplashEenabled() )
  {
    this->showVtkSplashScreenNew();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onEndInputInteraction()
{
  if (!m_volumeRenderer->isThreaMaskingAllowed()) return;
  QTimer::singleShot(g_interactionEndSplahTimeout,
                     this,
                     &VolumeRendererWidget::onIntractionEndTimeout);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::onThreatVolumePick(QVector3D& aPoint)
{
  int id  = THREAT_PICK_POINT_ID;
  emit this->positionVRWidgetMessage(id, aPoint);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::onRubberBandSelected(QVector2D& p0, QVector2D& p1)
{
  bool isActive = VolumeRendererWidget::pickerActive();
  if (isActive) return;
  this->setPickerActive(true);

  double bandWidth   = fabs(p1.x() - p0.x());
  double bandHeight  = fabs(p1.y() - p0.y());
  double aspectRatio;
  if ((bandWidth != 0 ) && (bandHeight != 0))
  {
    if (bandWidth <= bandHeight)
    {
      aspectRatio = bandWidth/bandHeight;
    }
    else
    {
      aspectRatio = bandHeight/bandWidth;
    }
  }
  else
  {
    this->setPickerActive(false);
    return;
  }

  if ( aspectRatio >  g_pickBoxApectMin  )
  {
    this->generateCubeOperatorThreat(p0, p1);
  }
  else
  {
    this->generateElogatedOperatorThreat(p0, p1);
  }
  emit userThreatBoxDrawingCompleted();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::ClampPickBoxToVolumeBounds(QVector3D& pCenter,
                                                      QVector3D& pRange)
{
  //  std::cout << "============================================================" << std::endl;
  //  std::cout << "DEBUG pCenter & pRange Initial Values" << std::endl;
  //  std::cout << "pCenter [x, y, z] = "
  //            << pCenter.x() << ","
  //            << pCenter.y() << ","
  //            << pCenter.z() << std::endl;

  //  std::cout << "pRange [x, y, z] = "
  //            << pRange.x() << ","
  //            << pRange.y() << ","
  //            << pRange.z() << std::endl;
  //  std::cout << "============================================================" << std::endl;

  double volumeBounds[6];
  m_volumeRenderer->getVolumeBounds(volumeBounds);

  double widthX = volumeBounds[1] - volumeBounds[0];
  double widthZ = volumeBounds[5] - volumeBounds[4];

  if (pRange.x() > widthX) pRange.setX(widthX);
  if (pRange.z() > widthZ) pRange.setZ(widthZ);


  if (pCenter.x() < volumeBounds[0])
  {
    pCenter.setX(volumeBounds[0] + pRange.x()/2.05);
  }
  if (pCenter.x() > volumeBounds[1])
  {
    pCenter.setX(volumeBounds[1] - pRange.x()/2.05);
  }

  if (pCenter.z() < volumeBounds[4])
  {
    pCenter.setZ( volumeBounds[4] + pRange.z()/2.05);
  }
  if (pCenter.z() > volumeBounds[5])
  {
    pCenter.setZ( volumeBounds[5] - pRange.x()/2.05);
  }
  //  std::cout << "============================================================" << std::endl;
  //  std::cout << "DEBUG pCenter & pRange AFTER CLAMP" << std::endl;
  //  std::cout << "pCenter [x, y, z] = "
  //            << pCenter.x() << ","
  //            << pCenter.y() << ","
  //            << pCenter.z() << std::endl;

  //  std::cout << "pRange [x, y, z] = "
  //            << pRange.x() << ","
  //            << pRange.y() << ","
  //            << pRange.z() << std::endl;
  //  std::cout << "============================================================" << std::endl;
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::generateCubeOperatorThreat(QVector2D& p0, QVector2D& p1)
{
  this->armRubberBand(false);
  if (m_renderSiblingsList.size() == 0) return;
  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();

  if (firstSib) firstSib->armRubberBand(false);

  double zoom = this->estimetedZoom();

  //-----------------------
  // Here we try to get the Image position from pick points.
  vtkCamera* camera = m_widgetRenderer->GetActiveCamera();
  double vu_x, vu_y, vu_z;
  double pd_x, pd_y, pd_z;

  camera->GetViewUp(vu_x,
                    vu_y,
                    vu_z);
  camera->GetDirectionOfProjection(pd_x,
                                   pd_y,
                                   pd_z);


  vtkSmartPointer<vtkRenderWindow>   renderWin = this->GetRenderWindow();

  vtkSmartPointer<vtkCoordinate> coordinate =
      vtkSmartPointer<vtkCoordinate>::New();
  coordinate->SetCoordinateSystemToDisplay();

  std::vector<QVector2D> qpointCorners = {p0,
                                          QVector2D(p1.x(), p0.y()),
                                          p1,
                                          QVector2D(p0.x(), p1.y())};

  std::vector<QVector3D> boxCorners;
  if (renderWin)
  {
    for (int i = 0; i < 4; i++)
    {
      int x = qpointCorners[i].x();
      int y = qpointCorners[i].y();
      coordinate->SetValue(x, y, 0);
      double* world = coordinate->GetComputedWorldValue(renderWin->GetRenderers()->GetFirstRenderer());
      QVector3D aCorner(world[0],
          world[1],
          world[2]);
      boxCorners.push_back(aCorner);
    }
  }

  QVector3D bc0 = boxCorners.at(0);
  QVector3D bc1 = boxCorners.at(2);

  double width  = (abs(bc1.x() - bc0.x()))*zoom;

  double height = (abs(bc1.y() - bc0.y()))*zoom;

  double length = (abs(bc1.z() - bc0.z()))*zoom;

  double avgLength = (width + height + length)/3;

  QVector2D simCenter = QVector2D((p0.x() + p1.x())/2.0,
                                  (p0.y() + p1.y())/2.0);
  QVector3D simulatedPick;

  bool gotPick = this->simulateVolumePick(simCenter,
                                          simulatedPick);
  Q_UNUSED(gotPick);

  QVector3D pRange  = QVector3D(avgLength,
                                avgLength,
                                avgLength);

  QVector3D pCenter = simulatedPick;

  this->finalizePickBoxHeight(pCenter, pRange);
}





//------------------------------------------------------------------------------
void VolumeRendererWidget::generateElogatedOperatorThreat(QVector2D& p0, QVector2D& p1)
{
  this->armRubberBand(false);
  if (m_renderSiblingsList.size() == 0) return;
  VolumeRendererWidget* firstSib = m_renderSiblingsList.front();

  if (firstSib) firstSib->armRubberBand(false);

  double zoom = this->estimetedZoom();

  //-----------------------
  // Here we try to get the Image position from pick points.

  vtkSmartPointer<vtkRenderWindow>   renderWin = this->GetRenderWindow();

  vtkSmartPointer<vtkCoordinate> coordinate =
      vtkSmartPointer<vtkCoordinate>::New();
  coordinate->SetCoordinateSystemToDisplay();

  std::vector<QVector2D> qpointCorners = {p0,
                                          QVector2D(p1.x(), p0.y()),
                                          p1,
                                          QVector2D(p0.x(), p1.y())};

  std::vector<QVector3D> boxCorners;
  if (renderWin)
  {
    for (int i = 0; i < 4; i++)
    {
      int x = qpointCorners[i].x();
      int y = qpointCorners[i].y();
      coordinate->SetValue(x, y, 0);
      double* world = coordinate->GetComputedWorldValue(renderWin->GetRenderers()->GetFirstRenderer());
      QVector3D aCorner(world[0],
          world[1],
          world[2]);
      boxCorners.push_back(aCorner);
    }
  }

  QVector3D VecU0((boxCorners[0].x() + boxCorners[1].x())/2.0,
      (boxCorners[0].y() + boxCorners[1].y())/2.0,
      (boxCorners[0].z() + boxCorners[1].z())/2.0);

  QVector3D VecU1((boxCorners[2].x() + boxCorners[3].x())/2.0,
      (boxCorners[2].y() + boxCorners[3].y())/2.0,
      (boxCorners[2].z() + boxCorners[3].z())/2.0);

  QVector3D VecU((VecU1.x() - VecU0.x()),
                 (VecU1.y() - VecU0.y()),
                 (VecU1.z() - VecU0.z()));


  QVector3D VecV0((boxCorners[0].x() + boxCorners[3].x())/2.0,
      (boxCorners[0].y() + boxCorners[3].y())/2.0,
      (boxCorners[0].z() + boxCorners[3].z())/2.0);

  QVector3D VecV1((boxCorners[1].x() + boxCorners[2].x())/2.0,
      (boxCorners[1].y() + boxCorners[2].y())/2.0,
      (boxCorners[1].z() + boxCorners[2].z())/2.0);


  QVector3D VecV((VecV1.x() - VecV0.x()),
                 (VecV1.y() - VecV0.y()),
                 (VecV1.z() - VecV0.z()));

  QVector3D vecDirBox;
  QVector3D vecDirCross;

  if (VecV.length() > VecU.length())
  {
    vecDirBox = VecV;
    vecDirCross = VecU;
  }
  else
  {
    vecDirBox = VecU;
    vecDirCross = VecV;
  }
  vecDirBox = QVector3D(vecDirBox.x()*zoom*g_directionScaler,
                        vecDirBox.y()*zoom*g_directionScaler,
                        vecDirBox.z()*zoom*g_directionScaler);

  vecDirCross = QVector3D(vecDirCross.x()*zoom*g_directionScaler,
                          vecDirCross.y()*zoom*g_directionScaler,
                          vecDirCross.z()*zoom*g_directionScaler);


  double imageBounds[6];
  m_volumeRenderer->getImageBounds(imageBounds);

  QVector3D dirX(1, 0, 0);
  QVector3D dirY(0, 1, 0);
  QVector3D dirZ(0, 0, 1);

  QVector3D vecDirNorm      = vecDirBox.normalized();
  QVector3D vecDirCrossNorm = vecDirCross.normalized();

  double dirDotX = fabs(QVector3D::dotProduct(vecDirNorm, dirX));
  double dirDotY = fabs(QVector3D::dotProduct(vecDirNorm, dirY));
  double dirDotZ = fabs(QVector3D::dotProduct(vecDirNorm, dirZ));

  QVector2D simCenter = QVector2D((p0.x() + p1.x())/2.0,
                                  (p0.y() + p1.y())/2.0);
  QVector3D simulatedPick;

  bool simPick = this->simulateVolumePick(simCenter,
                                          simulatedPick);
  Q_UNUSED(simPick);

  QVector3D pRange;

  if ((dirDotX > dirDotY) &&
      (dirDotX > dirDotZ) )
  {
    // x wins
    pRange.setX(vecDirBox.length());
    double crossDotY = fabs(QVector3D::dotProduct(vecDirCrossNorm, dirY));
    double crossDotZ = fabs(QVector3D::dotProduct(vecDirCrossNorm, dirZ));
    // The in plane direction uses the whole volume.
    if (crossDotY > crossDotZ)
    {
      pRange.setY(vecDirCross.length());
      pRange.setZ(fabs(imageBounds[5] - imageBounds[4]));
    }
    else
    {
      pRange.setZ(vecDirCross.length());
      pRange.setY(fabs(imageBounds[3] - imageBounds[2]));
    }
  }
  else if  ((dirDotY > dirDotX) &&
            (dirDotY > dirDotZ) )
  {
    // y wins
    pRange.setY(vecDirBox.length());
    double crossDotX = fabs(QVector3D::dotProduct(vecDirCrossNorm, dirX));
    double crossDotZ = fabs(QVector3D::dotProduct(vecDirCrossNorm, dirZ));
    if (crossDotX > crossDotZ)
    {
      pRange.setX(vecDirCross.length());
      pRange.setZ(fabs(imageBounds[5] - imageBounds[4]));
    }
    else
    {
      pRange.setZ(vecDirCross.length());
      pRange.setX(fabs(imageBounds[1] - imageBounds[0]));
    }
  }
  else if  ((dirDotZ > dirDotX) &&
            (dirDotZ > dirDotY) )
  {
    // z wins
    pRange.setZ(vecDirBox.length());
    double crossDotX = fabs(QVector3D::dotProduct(vecDirCrossNorm, dirX));
    double crossDotY = fabs(QVector3D::dotProduct(vecDirCrossNorm, dirY));
    if (crossDotX > crossDotY)
    {
      pRange.setX(vecDirCross.length());
      pRange.setY(fabs(imageBounds[3] - imageBounds[2]));
    }
    else
    {
      pRange.setY(vecDirCross.length());
      pRange.setX(fabs(imageBounds[1] - imageBounds[0]));
    }
  }
  else
  {
    QVector3D bc0 = boxCorners.at(0);
    QVector3D bc1 = boxCorners.at(2);

    double width  = (abs(bc1.x() - bc0.x()))*zoom;
    double height = (abs(bc1.y() - bc0.y()))*zoom;
    double length = (abs(bc1.z() - bc0.z()))*zoom;

    double avgLength = (width + height + length)/3;

    pRange = QVector3D(avgLength,
                       avgLength,
                       avgLength);
  }

  QVector3D pCenter = simulatedPick;
  this->finalizePickBoxHeight(pCenter, pRange);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::finalizePickBoxHeight(QVector3D& pCenter,
                                                 QVector3D& pRange)
{
  double heightCropScalar = 1.0 - m_cutPercentageY;
  double volumeBounds[6];
  // (xmin, xmax, ymin, ymax, zmin, zmax)
  m_volumeRenderer->getVolumeBounds(volumeBounds);

  double volY0 = volumeBounds[2];
  double volY1 = volumeBounds[3];

  double rangeY          = (fabs(volY1 - volY0));
  double fullHeight      = 2.0*rangeY;
  double cropHeight      = 2.0*heightCropScalar*rangeY;
  double deltaheight     = fullHeight - cropHeight;
  double deltaCenter     = deltaheight/4.0;


  double computedCenterY  = (volY0 + volY1)/2.0 - deltaCenter;

  pRange.setY(cropHeight);
  pCenter.setY(computedCenterY);

  //------------------------------------------------
  // FINAL STEP:
  // Keep the new Operator threat range INSDIDE the
  // Volume Range owned by m_volumeRenderer;
  this->ClampPickBoxToVolumeBounds(pCenter, pRange);
  //------------------------------------------------

  //-----------------------------------------------
  // ARO-Added: 5/31/2019 For min slice bags.
  double imageL    = volumeBounds[5] - volumeBounds[4];
  if ( imageL <= CHECKPOINT_MIN_NUMBER_SLICES)
  {
    double volZ0     = volumeBounds[4];
    double volZ1     = volumeBounds[5];
    double volZCent  = (volZ0 + volZ1)/2.0;

    double boxZ0     = pCenter.z() - pRange.z()/2;
    double boxZ1     = pCenter.z() + pRange.z()/2;
    double boxZCent  = (boxZ0 + boxZ1)/2.0;

    double deltaZCenter = volZCent - boxZCent;

    pCenter.setZ(pCenter.z() + deltaZCenter);
  }

  //-----------------------------------------------


  m_volumeRenderer->showSubVolume(pCenter, pRange);
  VolumeRendererWidget::setCurrentThreatPicker(this);

  //  //------------------------------------------------------------------------------------
  //  QVector3D tmpP0( (pCenter.x() - pRange.x()/2.0),
  //                   (pCenter.y() - pRange.y()/2.0),
  //                   (pCenter.z() - pRange.z()/2.0));

  //  QVector3D tmpP1( (pCenter.x() + pRange.x()/2.0),
  //                   (pCenter.y() + pRange.y()/2.0),
  //                   (pCenter.z() + pRange.z()/2.0));
  //  std::cout << "========================================================" << std::endl;
  //  std::cout << "PICK BOX TREAT CREATION   : P0 and P1 & EXTENT"  << std::endl;
  //  std::cout << "P0 :     X= " <<  tmpP0.x() <<
  //               ", Y= "        <<  tmpP0.y() <<
  //               ", Z= "        <<  tmpP0.z() << std::endl;

  //  std::cout << "P1 :     X= " <<  tmpP1.x() <<
  //               ", Y= "        <<  tmpP1.y() <<
  //               ", Z= "        <<  tmpP1.z() << std::endl;

  //  std::cout << "EXTENT:  X= " <<  pRange.x() <<
  //               ", Y= "        <<  pRange.y() <<
  //               ", Z= "        <<  pRange.z() << std::endl;
  //  std::cout << "========================================================" << std::endl;
  //  //------------------------------------------------------------------------------------
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::yawView(vtkSmartPointer<vtkRenderer> aRenderer,
                                   double angle)
{
  vtkSmartPointer<vtkMatrix4x4> extrinsicVTK = vtkSmartPointer<vtkMatrix4x4>::New();
  extrinsicVTK->DeepCopy(aRenderer->GetActiveCamera()->GetViewTransformMatrix());

  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->SetMatrix(extrinsicVTK);
  transform->RotateWXYZ(angle, 0, 1, 0);
  transform->Update();
  vtkSmartPointer<vtkMatrix4x4> rotExtrinsicVTK = vtkSmartPointer<vtkMatrix4x4>::New();
  rotExtrinsicVTK->DeepCopy(transform->GetMatrix());

  vtkSmartPointer<vtkMatrix4x4>  newMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4>  extrinsicRealInvert = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(rotExtrinsicVTK, extrinsicRealInvert);
  vtkMatrix4x4::Multiply4x4(extrinsicRealInvert,
                            extrinsicVTK,
                            newMatrix);

  transform->SetMatrix(newMatrix);
  transform->Update();
  aRenderer->GetActiveCamera()->ApplyTransform(transform);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::rollView(vtkSmartPointer<vtkRenderer> aRenderer,
                                    double angle)
{
  vtkSmartPointer<vtkMatrix4x4> extrinsicVTK = vtkSmartPointer<vtkMatrix4x4>::New();
  extrinsicVTK->DeepCopy(aRenderer->GetActiveCamera()->GetViewTransformMatrix());

  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->SetMatrix(extrinsicVTK);
  transform->RotateWXYZ(angle, 0, 0, 1);
  transform->Update();
  vtkSmartPointer<vtkMatrix4x4> rotExtrinsicVTK = vtkSmartPointer<vtkMatrix4x4>::New();
  rotExtrinsicVTK->DeepCopy(transform->GetMatrix());

  vtkSmartPointer<vtkMatrix4x4>  newMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4>  extrinsicRealInvert = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(rotExtrinsicVTK, extrinsicRealInvert);
  vtkMatrix4x4::Multiply4x4(extrinsicRealInvert, extrinsicVTK, newMatrix);

  transform->SetMatrix(newMatrix);
  transform->Update();
  aRenderer->GetActiveCamera()->ApplyTransform(transform);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::pitchView(vtkSmartPointer<vtkRenderer> aRenderer,
                                     double angle)
{
  vtkSmartPointer<vtkMatrix4x4> extrinsicVTK = vtkSmartPointer<vtkMatrix4x4>::New();
  extrinsicVTK->DeepCopy(aRenderer->GetActiveCamera()->GetViewTransformMatrix());

  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->SetMatrix(extrinsicVTK);
  transform->RotateWXYZ(angle, 1, 0, 0);
  transform->Update();
  vtkSmartPointer<vtkMatrix4x4> rotExtrinsicVTK = vtkSmartPointer<vtkMatrix4x4>::New();
  rotExtrinsicVTK->DeepCopy(transform->GetMatrix());

  vtkSmartPointer<vtkMatrix4x4>  newMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4>  extrinsicRealInvert = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(rotExtrinsicVTK, extrinsicRealInvert);
  vtkMatrix4x4::Multiply4x4(extrinsicRealInvert, extrinsicVTK, newMatrix);

  transform->SetMatrix(newMatrix);
  transform->Update();
  aRenderer->GetActiveCamera()->ApplyTransform(transform);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::systemMemoryUsageToLog()
{
  //-------------------------------------------------
  // Send memory usage/free in Megabytes to log file
  uint64_t memTotal;
  uint64_t memFree;

  MemUtils::getTotalAndFreeMemoryBytes(memTotal, memFree);
  memTotal /= (1024*1024);
  memFree  /= (1024*1024);

  QString fullMessage = "*MMMMMM* - MEMORY_USAGE (Megabytes) - Total, Free : ";
  fullMessage +=  QString::number(memTotal) +  ", ";
  fullMessage +=  QString::number(memFree);
  LOG(INFO) << fullMessage.toLocal8Bit().data();
}

//-------------------------------------------------------------------------------
void VolumeRendererWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    if (m_imageSplashRef)
    {
      this->removeSplash();
    }
  }
  OpenGLVolumeWidget::mouseMoveEvent(event);
}

//-------------------------------------------------------------------------------
void VolumeRendererWidget::wheelEvent(QWheelEvent* event)
{
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  OpenGLVolumeWidget::wheelEvent(event);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::stashCurrentCamera()
{
  vtkCamera* curCamera = this->getRenderCamera();
  m_stashCamera->DeepCopy(curCamera);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::restoreStashedCamera()
{
  this->deepCopyCameraAndUpdate(m_stashCamera);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::showThreatText(ThreatObject* pThr,
                                          int threatTypeIndex,
                                          int threatTypeTotal)
{
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);

  QString threatLabel = pThr->getText();

  QString idPrefix = tr("\nThreat ID: ");
  if (pThr->isLapTop())
  {
    idPrefix = tr("\nLaptop ID: ");
  }

  QString threatNumberOf   =  idPrefix +
      QString::number(threatTypeIndex + 1) + tr(" of ");
  threatNumberOf   += QString::number(threatTypeTotal);

  threatLabel += threatNumberOf;

  rightViewer->setRawThreatLabel(threatLabel);

  this->setProperty(VTK_RENDERED_THREAT_ID,  QVariant(threatNumberOf));
  m_prop_ThreatId = threatNumberOf;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::showThreatByMode(ThreatObject* pThr,
                                            SDICOS::Bitmap* pBMP,
                                            int rawIndex,
                                            int threatTypeIndex,
                                            int threatTypeTotal,
                                            bool operatorGenerated,
                                            bool keepOrientation)
{
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);

  if (!rightViewer->isInSlabMode())
  {
    if(keepOrientation)
    {
      rightViewer->stashCurrentCamera();
    }
    std::list<OpacitySigVal>  curOpacityList;
    std::list<HSVColorSigVal> curHsvColorList;
    VolumeRenderer* leftRenderer = leftViewer->getVolumeRenderer();

    //-------------------------------------------------------
    // Allow low density in threat viewer too.
    bool isLD = leftRenderer->isLowDensityActive();
    if (!isLD)
    {
      leftViewer->getNormalOpacityTransferFunction(curOpacityList);
      leftViewer->getNormalHSVTransferFunction(curHsvColorList);
    }
    else
    {
      curOpacityList  = leftViewer->getLowDensityOpacityList();
      curHsvColorList = leftViewer->getLowDensityHsvColorList();
    }
    //-------------------------------------------------------

    VolumeRenderer* rightRenderer = rightViewer->getVolumeRenderer();

    double ambient;
    double diffuse;
    double specular;
    leftRenderer->getLightingParams(ambient, diffuse, specular);

    leftViewer->SetSyncEnabled(false);
    rightViewer->SetSyncEnabled(false);

    ThreatVolume threatVol = pThr->getVolume();
    QVector3D threatLocationP0 = threatVol.p0();
    QVector3D threatLocationP1 = threatVol.p1();

    int W  = ( threatLocationP1.x() - threatLocationP0.x() );
    int H  = ( threatLocationP1.y() - threatLocationP0.y() );
    int L  = ( threatLocationP1.z() - threatLocationP0.z() );

    QVector3D volExtent(W, H, L);


    //--------------------
    // ARO-DEBUG - Test code to get average of threat volume and
    //   output the threat data to a file.
    /*
    std::cout << "Average Threat Volume - computing average: "
              << std::endl;
    vtkImageData* ptrImage = rightRenderer->getImageData();
    double imageAvrage = rightRenderer->averageImageData(ptrImage,
                                                         threatLocationP0,
                                                         W, H, L);
    std::string outVol   = DEFAULT_VOL_IMAGE_FILE;
    std::string outProp  = DEFAULT_VOL_PROP_FILE;
    rightRenderer->saveImageData(outVol,
                                 outProp,
                                 ptrImage,
                                 threatLocationP0,
                                 W, H, L);

    std::cout << "Croppeed Imagemage average: "
              << imageAvrage
              << std::endl;
    */
    //--------------------

    //------------------------------------------------------
    // Now the Right Viewer is a Threat Renderer
    rightViewer->SetAsThreatRenderer(true);
    rightViewer->enableThreatText();
    rightViewer->vtkWidgetUpdate();
    // std::cout << "------------------------------------------------------" << std::endl;
    // std::cout << "NORMAL THREAT VIEW    : "                               << std::endl;
    // std::cout << "Threat Index          : " <<  pThr->getIndex()          << std::endl;
    // std::cout << "threatTypeIndex       : " <<  threatTypeIndex           << std::endl;
    // std::cout << "threatTypeTotal       : " <<  threatTypeTotal           << std::endl;
    // std::cout << "------------------------------------------------------" << std::endl;
    this->showThreatText(pThr,
                         threatTypeIndex,
                         threatTypeTotal);

    uint16_t threatOffset = pThr->getOffset();
    std::list<HSVColorSigVal> threatHsvList = this->m_threatOffsetHsvMap[threatOffset];
    std::list<OpacitySigVal>  threatOpList  = this->m_threatOffsetOpacityMap[threatOffset];

    if (!operatorGenerated)
    {
      // Machine Generated Threats
      rightRenderer->displayCroppedThreatFromSibling(rawIndex,
                                                     W, H, L,
                                                     threatLocationP0,
                                                     pBMP,
                                                     pThr->getType(),
                                                     pThr->isLapTop(),
                                                     ambient,
                                                     diffuse,
                                                     specular,
                                                     curHsvColorList,
                                                     curOpacityList,
                                                     threatHsvList,
                                                     threatOpList);
      // Use this to center a CROPPED Volume
      rightViewer->centerCroppedThreatWindow(threatLocationP0,
                                             volExtent);
      rightViewer->forceRedraw();
    }
    else
    {
      // Operator Generated Threats
      analogic::workstation::ThreatAlarmType threatType = analogic::workstation::UNDEFINED;
      rightRenderer->displayCroppedThreatFromSibling(rawIndex,
                                                     W, H, L,
                                                     threatLocationP0,
                                                     nullptr,
                                                     threatType,
                                                     pThr->isLapTop(),
                                                     ambient,
                                                     diffuse,
                                                     specular,
                                                     curHsvColorList,
                                                     curOpacityList,
                                                     threatHsvList,
                                                     threatOpList);

      rightViewer->centerCroppedThreatWindow(threatLocationP0,
                                             volExtent);
    }
  }
  else
  {
    if (m_slabViewType == VREnums::BAG_VIEW)
    {
      if(keepOrientation)
      {
        rightViewer->stashCurrentCamera();
      }
      rightViewer->slabModeRenderThreat(pThr, false);
      rightViewer->clearThreatLabel();
    }
    else if (m_slabViewType == VREnums::THREAT_VIEW)
    {
      if(keepOrientation)
      {
        rightViewer->stashCurrentCamera();
      }
      rightViewer->slabModeRenderThreat(pThr, true);
      // std::cout << "------------------------------------------------------" << std::endl;
      // std::cout << "SLAB-THREAT VIEW      : "                               << std::endl;
      // std::cout << "Threat Index          : " <<  pThr->getIndex()          << std::endl;
      // std::cout << "threatTypeIndex       : " <<  threatTypeIndex           << std::endl;
      // std::cout << "threatTypeTotal       : " <<  threatTypeTotal           << std::endl;
      // std::cout << "------------------------------------------------------" << std::endl;
      this->showThreatText(pThr,
                           threatTypeIndex,
                           threatTypeTotal);
    }
    else if (m_slabViewType == VREnums::LAPTOP_VIEW)
    {
      if(keepOrientation)
      {
        rightViewer->stashCurrentCamera();
      }
      rightViewer->slabModeRenderThreat(pThr, false);
      // std::cout << "------------------------------------------------------" << std::endl;
      // std::cout << "SLAB-LAPTOP VIEW      : "                               << std::endl;
      // std::cout << "Threat Index          : " <<  pThr->getIndex()          << std::endl;
      // std::cout << "threatTypeIndex       : " <<  threatTypeIndex           << std::endl;
      // std::cout << "threatTypeTotal       : " <<  threatTypeTotal           << std::endl;
      // std::cout << "------------------------------------------------------" << std::endl;
      this->showThreatText(pThr,
                           threatTypeIndex,
                           threatTypeTotal);
      this->SlabNonThreatLaptop(pThr->getIndex());
    }
  }

  if (keepOrientation)
  {
    rightViewer->restoreStashedCamera();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::leftViewerShowThreatAtIndex(int rawIndex)
{
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  this->getLeftAndRightViewers(&leftViewer,
                               &rightViewer);
  VolumeRenderer* leftRenderer = leftViewer->getVolumeRenderer();
  if (rawIndex >= 0)
  {
    bool isSlabbingLaptop = false;
    ThreatObject* pThr = m_threatList->getThreatAtIndex(rawIndex);
    if (pThr->isLapTop() && m_isSlabbing)
    {
      isSlabbingLaptop = true;
    }
    leftRenderer->setCurrentThreat(rawIndex);
    if ( !isSlabbingLaptop)
    {
      m_volumeRenderer->selectThreat(rawIndex);
    }
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::mainRendererCameraMoveCallback(vtkObject* caller,
                                                          vtk_callback_ulong eventId,
                                                          void* clientData,
                                                          void* callData )
{
  Q_UNUSED(caller);
  Q_UNUSED(eventId);
  Q_UNUSED(callData);
  VolumeRendererWidget* volumeRenderWidget = reinterpret_cast<VolumeRendererWidget*>(clientData);
  volumeRenderWidget->hadleVTKAutoCameraMove();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::hadleVTKAutoCameraMove()
{
  double viewZoom = m_viewRuler->getZoom();
  m_viewRuler->setRulerSize();
  m_textLabel->setZoomValue(viewZoom);
  this->syncSibling();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::threatRendererCameraMoveCallback(vtkObject* caller,
                                                            vtk_callback_ulong eventId,
                                                            void* clientData,
                                                            void* callData)
{
  Q_UNUSED(caller);
  Q_UNUSED(eventId);
  Q_UNUSED(clientData);
  Q_UNUSED(callData);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::threatWidowPrintSelf(QString& strSelf)
{
  strSelf = "";
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::showCameraAndSceneSettings()
{
}




//------------------------------------------------------------------------------
void VolumeRendererWidget::setRendererDisplayName()
{
  // Setup display name.
  QString mainDisplayName = this->displayLabel();
  mainDisplayName += "-Main";
  m_volumeRenderer->setDisplayName(mainDisplayName);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::writeVolume()
{
  std::string saveFile = "/home/analogic/junk/WorkstationVolume.vol";
  std::ofstream outFile;
  int sizeWrite = m_debugImageWidth*m_debugImageHeight*m_debugSliceCount*sizeof(uint16_t);
  outFile.open(saveFile, ios::out | ios::binary);
  outFile.write(reinterpret_cast<char*>(m_debugWriteBuffer), sizeWrite);
  outFile.close();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::saveTransferFunctions()
{
  if (m_displayName.compare("Left View") == 0)
  {
    std::list<HSVColorSigVal> currentHSV;
    std::list<OpacitySigVal>  currentOpacity;

    this->getCurrentTransferFunctions(currentHSV, currentOpacity);

    QString opacityFile =  "./CurrentOpacity.csv";
    QString hsvFile     =  "./CurrentHSV.csv";

    analogic::workstation::saveTransferFunctions(hsvFile,
                                                 currentHSV,
                                                 opacityFile,
                                                 currentOpacity);
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::logMemoryUsage()
{
  // ARO-DEBUGGING ONLY!
  if (m_displayName.contains("Left"))
  {
    if (m_dbgBagCount == 1)
    {
      this->cycleLoadWriteHeader();
    }
    else
    {
      this->cycleLoadWriteData();
    }
  }
}




//------------------------------------------------------------------------------
void VolumeRendererWidget::cycleLoadWriteHeader()
{
  QDateTime dt = QDateTime::currentDateTime();
  QString message = "START CYCLE TEST:" + dt.toString() + "\n";
  this->appenStringToFile(g_cycleLoadLogFile, message);
  message = "Bag,Used (MB),Free (MB),Threat Size (MB)\n";
  this->appenStringToFile(g_cycleLoadLogFile, message);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::cycleLoadWriteData()
{
  uint64_t memTotal;
  uint64_t memFree;

  double  threatSizeMB =  0;
  int currentThreat = this->CurrentThreat();
  if (currentThreat >= 0)
  {
    ThreatObject* pThr = m_threatList->getThreatAtIndex(currentThreat);
    if (pThr)
    {
      int thrVol = pThr->getVolumeSize();
      size_t threatSize = thrVol*sizeof(uint16_t);
      threatSizeMB = threatSize/(1024*1024);
    }
  }

  MemUtils::getTotalAndFreeMemoryBytes(memTotal, memFree);
  memTotal /= (1024*1024);
  memFree  /= (1024*1024);

  int bagCount = this->getDbgBagCount();
  QString message = QString::number(bagCount)             + ", " +
      QString::number(memTotal - memFree)   + ", " +
      QString::number(memFree)              + ", " +
      QString::number(threatSizeMB)         + "\n";

  this->appenStringToFile(g_cycleLoadLogFile, message);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::logBagSizeInfo()
{
  // ARO-DEBUGGING ONLY!
  if (m_displayName.contains("Left"))
  {
    if (m_dbgBagCount == 1)
    {
      this->sizeInfoWriteHeader();
    }
    else
    {
      this->sizeInfoWriteData();
    }
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::sizeInfoWriteHeader()
{
  QDateTime dt = QDateTime::currentDateTime();
  QString message = "START BG SIZE INFO LOG:" + dt.toString() + "\n";
  this->appenStringToFile(g_bagSizeInfoLog, message);

  message  = "Bag,W,H,L,crop_x0,crop_y0,crop_z0,crop_xMax,crop_yMax,crop_zMax,";
  message += "TV0_x,TV0_y,TV0_z,TV0_W,TV0_H,TV0_L,[TY0-Crop_Y0]\n";
  this->appenStringToFile(g_bagSizeInfoLog, message);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::sizeInfoWriteData()
{
  int bagCount = this->getDbgBagCount();
  double W, H, L;
  double crop_X0, crop_Y0, crop_Z0;
  double crop_X1, crop_Y1, crop_Z1;
  crop_X0  = m_cropBox.m_pMin.x();
  crop_Y0  = m_cropBox.m_pMin.y();
  crop_Z0  = m_cropBox.m_pMin.z();
  crop_X1  = m_cropBox.m_pMax.x();
  crop_Y1  = m_cropBox.m_pMax.y();
  crop_Z1  = m_cropBox.m_pMax.z();

  double volBounds[6];
  m_volumeRenderer->getVolumeBounds(volBounds);
  W = volBounds[1] - volBounds[0];
  H = volBounds[3] - volBounds[2];
  L = volBounds[5] - volBounds[4];

  double threatVolume_0_x  = 0;
  double threatVolume_0_y  = 0;
  double threatVolume_0_z  = 0;

  double threatVolume_0_W  = 0;
  double threatVolume_0_H  = 0;
  double threatVolume_0_L  = 0;

  if (m_threatList->size() > 0)
  {
    ThreatObject* pThObj =  m_threatList->getThreatAtIndex(0);
    ThreatVolume  TV = pThObj->getVolume();
    threatVolume_0_x = TV.p0().x();
    threatVolume_0_y = TV.p0().y();
    threatVolume_0_z = TV.p0().z();

    threatVolume_0_W = (TV.p1().x() - TV.p0().x());
    threatVolume_0_H = (TV.p1().y() - TV.p0().y());
    threatVolume_0_L = (TV.p1().z() - TV.p0().z());
  }

  double threat_Y_minusCrop_Y = crop_X0 - threatVolume_0_y;
  QString message = QString::number(bagCount)  + ", " +
      QString::number(W)                    + ", " +
      QString::number(H)                    + ", " +
      QString::number(L)                    + ", " +
      QString::number(crop_X0)              + ", " +
      QString::number(crop_Y0)              + ", " +
      QString::number(crop_Z0)              + ", " +
      QString::number(crop_X1)              + ", " +
      QString::number(crop_Y1)              + ", " +
      QString::number(crop_Z1)              + ", " +
      QString::number(threatVolume_0_x)     + ", " +
      QString::number(threatVolume_0_y)     + ", " +
      QString::number(threatVolume_0_z)     + ", " +
      QString::number(threatVolume_0_W)     + ", " +
      QString::number(threatVolume_0_H)     + ", " +
      QString::number(threatVolume_0_L)     + ", " +
      QString::number(threat_Y_minusCrop_Y) + "\n";
  this->appenStringToFile(g_bagSizeInfoLog, message);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::appenStringToFile(QString& filename,
                                             QString& str)
{
  std::ofstream ofs;
  ofs.open (filename.toLocal8Bit().data(), std::ofstream::out | std::ofstream::app);
  ofs << str.toLocal8Bit().data();
  ofs.close();
}

//------------------------------------------------------------------------------
QString VolumeRendererWidget::stripRFIDLeadingZeros(QString& fullRFIDStr)
{
  int lenTail = fullRFIDStr.length() - QString(VTK_RFID_LABEL_TAG).length();
  QString rfidTail = fullRFIDStr.right(lenTail);
  std::string clipTail = rfidTail.toStdString();
  clipTail.erase(0, clipTail.find_first_not_of('0'));
  QString retStr = VTK_RFID_LABEL_TAG;
  retStr += clipTail.c_str();
  return retStr;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::clearUserDecision()
{
  // std::cout << "Clear User Decision:"
  //          <<  this->displayLabel().toStdString()
  //          << std::endl;
  m_userDecision = "";
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::disableSlabing()
{
  m_isSlabbing = false;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::stopLoadTimers()
{
  int fullLoadTimer = m_VR_ProcessTimerFullLoad.elapsed();
  QString viewLabel;
  if (this->displayLabel().contains("Left"))
  {
    viewLabel = "LEFT_VIEW,";
  }
  else
  {
    viewLabel = "RIGHT_VIEW,";
  }
  if (m_removeTimerStarted)
  {
     QString fullTimeString = viewLabel +"BAG_CLEAR_SUSPECT_TO_RENDER_COMPLETE (ms),"
             + QString::number(fullLoadTimer);
     LOG(INFO) << fullTimeString.toStdString();
     // std:: cout << fullTimeString.toStdString()
     //           << std::endl;

     int clearToInit = m_VR_ProcessFromInit.elapsed();
     QString clearToInitString = viewLabel +
              "VOL_REND_INIT_TO_RENDER_COMPLETE (ms)," + QString::number(clearToInit);
     LOG(INFO) <<  clearToInitString.toStdString();
     // std::cout << clearToInitString.toStdString()
     //           << std::endl;
     //----------------------------
     // ARO Debugging only
     // QTimer::singleShot(1.5*1000, this, SLOT(OnTimerClearFirstThreat()));
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::saveFullMaps()
{
  if (m_displayName.compare("Left View") == 0)
  {
    uint16_t maxSignal = 12288;
    std::list<HSVColorSigVal> currentHSV;
    std::list<OpacitySigVal>  currentOpacity;

    this->getCurrentTransferFunctions(currentHSV, currentOpacity);


    QString fullmapFile =  "./FullHsvOpacityMap.csv";


    analogic::workstation::saveFullMaps(fullmapFile,
                                        currentHSV,
                                        currentOpacity,
                                        maxSignal);
  }
}

//-----------------------------------------------------------------------------
void VolumeRendererWidget::suspectNonThreatTDR(SDICOS::TDRModule *pTdr,
                                               SDICOS::CTModule* ctModule)
{
  SDICOS::TDRTypes::ThreatDetectionReport& threat_report = pTdr->GetThreatDetectionReport();
  threat_report.SetNumberOfAlarmObjects(0);
  threat_report.SetNumberOfTotalObjects(0);
  threat_report.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm);

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array =
      pTdr->GetThreatSequence().GetPotentialThreatObject();

  SDICOS::TDRTypes::ThreatSequence::ThreatItem pickbox_threat_item;

  pickbox_threat_item.EnableBaggageTDR(true);
  pickbox_threat_item.SetID(0);



  SDICOS::TDRTypes::AssessmentSequence& assessment_sequence = pickbox_threat_item.GetAssessment();
  assessment_sequence.GetAssessment().SetSize(1);
  assessment_sequence.GetAssessment()[0].SetThreatCategory(
        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumExplosive);
  assessment_sequence.GetAssessment()[0].SetThreatCategoryDescription("BULK");
  assessment_sequence.GetAssessment()[0].SetAbilityAssessment(
        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoInterference);

  // assessment_sequence.GetAssessment()[0].SetAbilityAssessment(
  //     SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumShield);

  assessment_sequence.GetAssessment()[0].SetAssessmentFlag(
        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat);


  SDICOS::TDRTypes::BaggageSpecificTDR& baggage_tdr = pickbox_threat_item.GetBaggageTDR();

  // bdavidson: added for DICOS v2.0a
  SDICOS::Array1D<SDICOS::TDRTypes::BaggageSpecificTDR::PTORepresentationSequenceItem>& pto_array =
      baggage_tdr.GetPTORepresentationSequenceItems();
  pto_array.SetSize(1);
  SDICOS::TDRTypes::BaggageSpecificTDR::PTORepresentationSequenceItem& pto = pto_array[0];

  // bdavidson: modified for DICOS v2.0a
  SDICOS::Array1D<SDICOS::SopInstanceReference> & sop_sequence = pto.GetReferencedInstance();
  sop_sequence.SetSize(1);
  sop_sequence[0].SetClassUID(SDICOS::SOPClassUID::GetCT());
  sop_sequence[0].SetInstanceUID(ctModule->GetSopCommon().GetSopInstanceUID());


  int xmin = 10;
  int ymin = 10;
  int zmin = 10;
  int size_x = 10;
  int size_y = 10;
  int size_z = 10;

  // bdavidson: modified for DICOS v2.0a
  SDICOS::TDRTypes::CommonSpecificTDR::ThreatROI& threat_roi = pto.GetThreatROI();
  threat_roi.SetThreatRoiBase(xmin, ymin, zmin);
  threat_roi.SetThreatRoiExtents(size_x, size_y, size_z);
  threat_item_array.Add(pickbox_threat_item);


  SDICOS::ErrorLog errorlog;
  if (pTdr->IsValid(errorlog))
  {
    LOG(INFO) << "Non-threat suspect Bag TDR is valid";
    std::cout << "Non-threat suspect Bag TDR is valid" << std::endl;
  }
  else
  {
    LOG(INFO) << "Non-threat suspect bag TDR is invalid";
    LOG(INFO) <<  errorlog.GetError(0).Get();
    std::cout  << "Non-threat suspect bag TDR is invalid" << std::endl;
  }
}


//-----------------------------------------------------------------------------
void VolumeRendererWidget::disableVolume()
{
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  vtkRenderWindow* renderWin = this->GetRenderWindow();
  vtkSmartPointer<vtkTextActor> textActor =
      vtkSmartPointer<vtkTextActor>::New();
  textActor->SetInput(DISABLE_VOLUME_SPLASH_TEXT);
  textActor->SetPosition(DISABLE_VOLUME_SPLASH_POS_X,
                         DISABLE_VOLUME_SPLASH_POS_Y);
  textActor->GetTextProperty()->SetFontSize(DISABLE_VOLUME_SPLASH_HEIGHT);
  textActor->GetTextProperty()->SetColor(DISABLE_VOLUME_SPLASH_COLOR_RED,
                                         DISABLE_VOLUME_SPLASH_COLOR_GREEN,
                                         DISABLE_VOLUME_SPLASH_COLOR_BLUE);

  m_disabledViewerSplashRef = textActor.GetPointer();

  m_widgetRenderer->AddActor(textActor);

  renderWin->Render();
  this->setEnableInteraction(false);
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::removeDisabledSplash()
{
  if (m_disabledViewerSplashRef)
  {
    m_widgetRenderer->RemoveActor2D(m_disabledViewerSplashRef);
    m_disabledViewerSplashRef = nullptr;
  }
}


//-----------------------------------------------------------------------------
void VolumeRendererWidget::showVtkSplashScreenNew()
{
  if (!this->isSplashAllowed()) return;
  if (m_disabledViewerSplashRef) return;
  if (!m_volumeInitialized) return;
  if (!m_volumeRenderer->isThreaMaskingAllowed()) return;
  // std::cout << "Show VTK Splash Screen - " << this->displayLabel().toStdString()
  //          << std::endl;
  this->removeSplash();
  vtkRenderWindow* renderWin = this->GetRenderWindow();


  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
      vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renderWin);

  // Record the alpha (transparency) channel
  windowToImageFilter->SetInputBufferTypeToRGBA();

  // Read from the back buffer
  windowToImageFilter->ReadFrontBufferOff();

  windowToImageFilter->Update();


  int marginY    = VTK_SPLASH_MARGIN_Y;
  int clip_x_min = 0;
  int clip_x_max = VTK_MAIN_VIEWER_WIN_WIDTH;
  int clip_y_min = marginY;
  int clip_y_max = 0;

  //----------------------------------------------------------
  // The inverse screen (black background) is
  // harder to splash with text so we
  // change the  border in Y
  if (m_volumeRenderer->getBagInverse())
  {
    clip_y_max = VTK_MAIN_VIEWER_WIN_HEIGHT -  marginY;
  }
  else
  {
    clip_y_max = VTK_MAIN_VIEWER_WIN_HEIGHT;
  }
  //----------------------------------------------------------

  int clip_z_min = 0;
  int clip_z_max = 0;

  int filter_color_window = 180;
  int filter_color_level  = 125;

  vtkSmartPointer<vtkImageClip> bagClip = vtkSmartPointer<vtkImageClip>::New();
  bagClip->SetInputConnection(windowToImageFilter->GetOutputPort());
  bagClip->SetOutputWholeExtent(clip_x_min,
                                clip_x_max,
                                clip_y_min,
                                clip_y_max,
                                clip_z_min,
                                clip_z_max);
  bagClip->Update();


  //=====================================
  // Sharpening FILTER
  vtkSmartPointer<vtkImageGaussianSmooth> imageSmoothFilter =
      vtkSmartPointer<vtkImageGaussianSmooth>::New();
  imageSmoothFilter->SetInputConnection(bagClip->GetOutputPort());
  imageSmoothFilter->Update();

  vtkSmartPointer<vtkImageShiftScale> imageScale =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScale->SetInputConnection(imageSmoothFilter->GetOutputPort());
  imageScale->SetScale(DYNAMIC_SPLASH_FILTER_SCALE);
  imageScale->Update();

  vtkSmartPointer<vtkImageMathematics> imageDiffFilter =
      vtkSmartPointer<vtkImageMathematics>::New();
  imageDiffFilter->SetOperationToSubtract();
  imageDiffFilter->SetInput1Data(bagClip->GetOutput());
  imageDiffFilter->SetInput2Data(imageScale->GetOutput());

  imageDiffFilter->Update();
  //=====================================

  //=====================================
  // Mapper

  vtkSmartPointer<vtkImageMapper> imageMapper = vtkSmartPointer<vtkImageMapper>::New();

  imageMapper->SetInputData(imageDiffFilter->GetOutput());
  imageMapper->SetColorWindow(filter_color_window);
  imageMapper->SetColorLevel(filter_color_level);


  vtkSmartPointer<vtkActor2D> imageActor = vtkSmartPointer<vtkActor2D>::New();
  imageActor->SetMapper(imageMapper);
  imageActor->SetPosition(0, 0);
  m_imageSplashRef = imageActor.GetPointer();


  m_widgetRenderer->AddActor(imageActor);

  renderWin->Render();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::removeSplash()
{
  if(m_imageSplashRef)
  {
    m_widgetRenderer->RemoveActor2D(m_imageSplashRef);
    m_imageSplashRef = nullptr;
  }
}

//------------------------------------------------------------------------------
double VolumeRendererWidget::estimetedZoom()
{
  vtkCamera* curCamera =  m_widgetRenderer->GetActiveCamera();

  double distance = curCamera->GetDistance();

  double imageBounds[6];
  m_volumeRenderer->getImageBounds(imageBounds);

  double Wx = abs(imageBounds[1] - imageBounds[0]);
  double Wy = abs(imageBounds[3] - imageBounds[2]);
  double Wz = abs(imageBounds[5] - imageBounds[4]);
  double avgWidth = (Wx + Wy + Wz)/3.0;

  double estZoom = g_computedZoomScalar*avgWidth/distance;

  return estZoom;
}

//------------------------------------------------------------------------------
VolumeRenderer* VolumeRendererWidget::getVolumeRenderer()
{
  return m_volumeRenderer;
}

//------------------------------------------------------------------------------
vtkRenderer* VolumeRendererWidget::getVTKRenderer()
{
  return m_widgetRenderer;
}



void VolumeRendererWidget::getLeftAndRightViewers(VolumeRendererWidget** ppLeftViewer,
                                                  VolumeRendererWidget** ppRightViewer)
{
  // First we needto make sure we usethe Left viewer to driver the change.
  VolumeRendererWidget* leftViewer;
  VolumeRendererWidget* rightViewer;
  if (this->displayLabel().contains("Left"))
  {
    leftViewer = this;
    rightViewer = m_renderSiblingsList.front();
  }
  else
  {
    leftViewer = m_renderSiblingsList.front();
    rightViewer = this;
  }
  *ppLeftViewer  = leftViewer;
  *ppRightViewer = rightViewer;
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::showLeaks()
{
  if (this->displayLabel().compare("Left View") == 0)
  {
    int testBagCount = m_dbgBagCount % 5;
    if (testBagCount ==0)
    {
      // Show Memory Leak every 5 Bags.
      if (vtkDebugLeaks::PrintCurrentLeaks() == 1)
      {
        QString leakMessage = "There were VTK leaks found in VolumeRenderer Application";
        LOG(INFO) << leakMessage.toLocal8Bit().data();
        qDebug() << leakMessage;
      }
    }
  }
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::showThreatInfo()
{
  std::cout << "--------------- **** Threat INFO **** ---------------------------" << std::endl;
  if (this->displayLabel().contains("Right")) return;
  for (int i = 0; i < m_threatList->size(); i++)
  {
    ThreatObject* pThr = m_threatList->getThreatAtIndex(i);
    if (pThr)
    {
      std::cout << "Index= "      <<  pThr->getIndex()
                << ", ID= "       <<  pThr->getID().toStdString()
                << ", Gen Type= " <<  pThr->getGenType()
                << ", Offset= "   <<  pThr->getOffset()
                << std::endl;
    }
  }
  std::cout << "----------------------------------------------------------------" << std::endl;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::exportThreatData(QString& filenameExport)
{
  m_volumeRenderer->writeThreatVolumeToFile(filenameExport);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setColorLevel(float colorLevel)
{
  m_volumeRenderer->setColorLevel(colorLevel);
  this->update();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setWindowLevel(float windowLevel)
{
  m_volumeRenderer->setWindowLevel(windowLevel);
  this->update();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setGrayscaleSettings(GrayscaleValueOpacityStruct& grayScaleStruct)
{
  m_volumeRenderer->setGrayscaleSettings(grayScaleStruct);
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::hideLabelsAndAxes()
{
  m_viewRuler->hide();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::showLabelsAndAxes()
{
  m_viewRuler->show();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::testImageOverlay(QWidget* mainWin)
{
  Q_UNUSED(mainWin);
  //----------------------------------------
  // ARO-NOTE:
  // This grabs a screen-shot of the MainWindow but VTK
  // objects don't get captured.

  // QPixmap windowPixmap = QPixmap::grabWidget(mainWin);
  // QFile file("/home/analogic/junk/tesImage.png");
  // file.open(QIODevice::WriteOnly);
  // windowPixmap.save(&file,"PNG");
  // file.close();
  //---------------------------------------

  vtkSmartPointer<vtkRenderWindow>  renderWin = this->GetRenderWindow();


  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
      vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renderWin);
  // Record the alpha (transparency) channel
  windowToImageFilter->SetInputBufferTypeToRGBA();
  // Read from the back buffer
  windowToImageFilter->ReadFrontBufferOff();
  windowToImageFilter->Update();

  vtkSmartPointer<vtkPNGWriter> writer =
      vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName("/home/analogic/junk/tesImage.png");
  writer->SetInputConnection(windowToImageFilter->GetOutputPort());
  writer->Write();

  // vtkSmartPointer<vtkImageReader> rawReader =   vtkSmartPointer<vtkImageReader>::New();
  // rawReader->SetD
  // windowToImageFilter->GetOutput()
  //+++++++++++++++++++++++++++++++++++++++++
  vtkSmartPointer<vtkImageGaussianSmooth> imageSmoothFilter =
      vtkSmartPointer<vtkImageGaussianSmooth>::New();
  imageSmoothFilter->SetInputConnection(windowToImageFilter->GetOutputPort());
  imageSmoothFilter->Update();

  vtkSmartPointer<vtkImageShiftScale> imageScale =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScale->SetInputConnection(imageSmoothFilter->GetOutputPort());
  imageScale->SetScale(0.15);
  imageScale->Update();

  vtkSmartPointer<vtkImageMathematics> imageDiffFilter =
      vtkSmartPointer<vtkImageMathematics>::New();
  imageDiffFilter->SetOperationToSubtract();
  imageDiffFilter->SetInput1Data(windowToImageFilter->GetOutput());
  imageDiffFilter->SetInput2Data(imageScale->GetOutput());

  imageDiffFilter->Update();

  //++++++++++++++++++++++++++++++++++++++++++

  vtkSmartPointer<vtkImageMapper> imageMapper = vtkSmartPointer<vtkImageMapper>::New();

  imageMapper->SetInputData(imageDiffFilter->GetOutput());
  imageMapper->SetColorWindow(180);
  imageMapper->SetColorLevel(125);

  vtkSmartPointer<vtkActor2D> imageActor = vtkSmartPointer<vtkActor2D>::New();
  imageActor->SetMapper(imageMapper);
  imageActor->SetPosition(0, 0);
  m_imageSplashRef = imageActor.GetPointer();


  m_widgetRenderer->AddActor(imageActor);
  this->update();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::initThreatsFromTDR(ThreatList* threats,
                                              SDICOS::TDRModule* pTdr,
                                              QVector3D& volMin,
                                              QVector3D& volMax,
                                              QVector3D& pixelSpacing,
                                              const std::vector<std::pair<int, std::string>>&
                                              vecDisplayThreatIdentifier,
                                              std::map<ThreatAlarmType, uint16_t>& treatOffsetMap,
                                              bool doMass)
{
  // std::cout << "%%%%  -  initThreatsFromTDR() - view: "
  //          << this->displayLabel().toStdString() << std::endl;
  // Read existing number of threats present in ThreatList
  int threatIndex = threats->size();

  // Read input threat module and threat objects preent inside it
  SDICOS::TDRTypes::ThreatSequence &ts = pTdr->GetThreatSequence();
  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects =
      ts.GetPotentialThreatObject();

  SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type =
      pTdr->GetThreatDetectionReport().GetTdrType();

  // print threats objects in input TDR module
  QString countMessage;
  int tdrThreatSize = threatObjects.GetSize();
  countMessage = "Threat Objects Liat size: " + QString::number(tdrThreatSize);
  qDebug() << countMessage;


  qDebug() << "About to read SDICONS::TDRModule ";


  // ---------------------------------------------------------------
  // Read all treat objects from input threat module ,
  // For each threat object in module, Create a new threat object,
  // Copy details from threat object from module
  // and Append new threat object in Input LIST
  // ---------------------------------------------------------------

  std::vector<bool> hideVec;
  hideVec.resize(tdrThreatSize);
  for (int ia = 0; ia < tdrThreatSize; ia++)
  {
    hideVec[ia] = true;
  }
  std::list<int> displayList;

  // Use the vecDisplayThreatIdentifier to decide which threats to display
  // Hide the ones missing from the vector.
  for (unsigned int ib = 0;  ib < vecDisplayThreatIdentifier.size(); ib++ )
  {
    uint16_t       testIndex  = vecDisplayThreatIdentifier.at(ib).first;
    // std::cout << "vecDisplayThreatIdentifier at[" << ib << "] = " << testIndex << std::endl;
    displayList.push_back(testIndex);
  }

  for (int i = 0; i < tdrThreatSize; i++)
  {
    bool bIsThreat = false;
    bool bIsLaptop = false;

    // Ignore threats that are reference threat in tdr module.
    SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object;
    threatObjects[i].GetReferencedPotentialThreatObject(ref_object);
    if (ref_object.GetReferencedPotentialThreatObject().GetSize() > 0)
    {
      continue;
    }

    SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem>& assmentItems =
        threatObjects[i].GetAssessment().GetAssessment();
    if (assmentItems.GetSize() > 0)
    {
      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG aFlag =
          assmentItems[0].GetAssessmentFlag();
      if (aFlag == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat)
      {
        bIsThreat = true;
      }
    }


    //----------------------------------------
    // TDR contains non threat LAPTOP item
    SDICOS::DcsString dcsDescription = assmentItems[0].GetThreatCategoryDescription();
    QString threatDescrioption = dcsDescription.Get();
    QString threatDiscriptionName = analogic::workstation::threatTypeName(LAPTOP);
    if ( threatDescrioption.contains(threatDiscriptionName,
                                     Qt::CaseInsensitive) && (!bIsThreat) )
    {
      bIsLaptop = true;
    }
    //----------------------------------------


    if ( (bIsThreat) || (bIsLaptop))
    {
      float xb, yb, zb;
      float xt, yt, zt;

      float vmx, vmy, vmz;

      vmx = volMin.x();
      vmy = volMin.y();
      vmz = volMin.z();

      // bdavidson: modified for DICOS v2.0a
      // Careful: I did not check to make sure there is an index 0 in the array
      threatObjects[i].GetBaggageTDR().GetPTORepresentationSequenceItems()[0].GetThreatROI().GetThreatRoiBase(xb,
                                                                                                              yb,
                                                                                                              zb);
      threatObjects[i].GetBaggageTDR().GetPTORepresentationSequenceItems()[0].GetThreatROI().GetThreatRoiExtents(xt,
                                                                                                                 yt,
                                                                                                                 zt);

      QString threatROIBaseAndExt =  " - Point 0 (x,y,z) [" +
          QString::number(xb) + ", " +
          QString::number(yb) + ", " +
          QString::number(zb) + "] - Extent [w,h,l]:" +
          QString::number(xt) + ", " +
          QString::number(yt) + ", " +
          QString::number(zt) + "]";

      QString threatROIInfo = "Threat INFO: " + threatDescrioption + threatROIBaseAndExt;

      // std::cout << threatROIInfo.toStdString() << std::endl;
      LOG(INFO) << threatROIInfo.toStdString();

      QVector3D p0 = QVector3D(xb - vmx,
                               yb - vmy,
                               zb - vmz );

      QVector3D p1 = QVector3D(xb + xt - vmx,
                               yb + yt - vmy,
                               zb + zt - vmz );




      ThreatVolume tv(p0, p1);

      analogic::workstation::clipVolumeRange(volMin, volMax, p0, p1);


      // Mass
      double massG = threatObjects[i].GetBaggageTDR().GetMass();

      // Bitmap
      // bdavidson: modified for DICOS v2.0a
      // Careful: I did not check to make sure the array had index 0
      SDICOS::Bitmap& bmpRef =
          threatObjects[i].GetBaggageTDR().GetPTORepresentationSequenceItems()[0].GetThreatROI().GetThreatRoiBitmap();
      SDICOS::Bitmap* pBmp = &bmpRef;

      // Threat Item

      SDICOS::TDRTypes::ThreatSequence::ThreatItem& thretItem = threatObjects[i];

      analogic::workstation::ThreatAlarmType thType =
          getThreatAlarmTypeFromTdrThreatObject(thretItem);


      uint16_t threatOffset = treatOffsetMap[thType];


      QString threatText;
      if ( (pBmp->GetSize() > 0) &&
           (tdr_type != SDICOS::TDRTypes::ThreatDetectionReport::enumOperator) )
      {
        threatText = this->constructMachineThreatText(thType,
                                                      massG,
                                                      doMass);
      }
      else
      {
        threatText = constructOperatorThreatTextFromType(thType);
      }

      QString threatID = "Threat-" + QString::number(threatIndex);

      ThreatObject* pThreat = new ThreatObject(threatIndex,
                                               tv,
                                               threatID,
                                               threatText,
                                               pBmp);


      //----------------------------------------------------------------
      // QString threatTypename = analogic::workstation::threatTypeName(thType);
      // std::cout << "Created Threat index[ "
      //          << threatIndex
      //          << " ] TYPE=  "
      //          << threatTypename.toStdString()
      //          << std::endl;
      //----------------------------------------------------------------

      hideVec[threatIndex] = false;
      pThreat->setType(thType);
      pThreat->setMass(massG);
      pThreat->setOffset(threatOffset);
      threats->addThreatObject(pThreat);

      if (bIsLaptop)
      {
        pThreat->setAsLaptop(true);
      }

      //--------------------------------------
      //  Log the threat creation information
      QString message = "**HH** Created ThreatObject:Vol[P0-P1](";
      message += QString::number(tv.p0().x()) + ", ";
      message += QString::number(tv.p0().y()) + ", ";
      message += QString::number(tv.p0().z()) + ")-(";

      message += QString::number(tv.p1().x()) + ", ";
      message += QString::number(tv.p1().y()) + ", ";
      message += QString::number(tv.p1().z()) + ") ID:[";
      message += threatID   + "] Text[";
      message += threatText;
      message += "] ";

      LOG(INFO) << message.toStdString();
      qDebug()  << message;
      //-----------------------------------------
    }
    threatIndex++;
  }

  // Setup the threat display list
  for (size_t i = 0; i < hideVec.size(); i++)
  {
    std::list<int>::iterator itL;
    for (itL = displayList.begin(); itL != displayList.end();  itL++)
    {
      size_t pI = *itL;
      if (pI == i) hideVec[i] =false;
    }
  }

  // Add new threat count in debug log
  int finalThreatCount = threats->size();
  countMessage = "**VV** Threat Count from SDICOS::TDRModule : " + QString::number(finalThreatCount);
  LOG(INFO) << countMessage.toLocal8Bit().data();
  qDebug() << countMessage;

  //------------------------------
  // Hide threats not in the display list.
  for (size_t i = 0; i < hideVec.size(); i++)
  {
    if (hideVec[i])
    {
      m_threatList->hideThreatAt(i);
    }
  }
}



//------------------------------------------------------------------------------
void VolumeRendererWidget::increaseOperatorThreatSize()
{
  m_volumeRenderer->increaseOperatorThreatsize();
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::decreaseOperatorThreatSize()
{
  m_volumeRenderer->decreaseOperatorThreatsize();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::setLaptopSlabMode(bool slabVal)
{
  m_threatList->setSlabingLaptop(slabVal);
}

//------------------------------------------------------------------------------
double VolumeRendererWidget::getDesiredUpdateRate()
{
  double updateRate = DEFAULT_VTK_UPDATE_RATE;
  vtkRenderWindowInteractor* rwi = m_renderWindowRef->GetInteractor();
  if (rwi)
  {
    updateRate = rwi->GetDesiredUpdateRate();
  }
  return updateRate;
}

//------------------------------------------------------------------------------
void  VolumeRendererWidget::setDesiredUpdateRate(double updateRate)
{
  vtkRenderWindowInteractor* rwi = m_renderWindowRef->GetInteractor();
  if (rwi)
  {
    rwi->SetDesiredUpdateRate(updateRate);
  }
}


//------------------------------------------------------------------------------
void VolumeRendererWidget::createColorImage(vtkImageData* image)
{
  unsigned int dim = 600;

  image->SetDimensions(dim, dim, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  for(unsigned int x = 0; x < dim; x++)
  {
    for(unsigned int y = 0; y < dim; y++)
    {
      unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      if(x < dim/2)
      {
        pixel[0] = 255;
        pixel[1] = 0;
      }
      else
      {
        pixel[0] = 0;
        pixel[1] = 255;
      }
      pixel[2] = 0;
    }
  }

  image->Modified();
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::distanceMeasurementComplete()
{
  // std::cout << "########## distanceMeasurementComplete() #################"
  //           <<  std::endl;
  this->setDistanceMeasureStatus(DISTANCE_MEASURE_START);
}

//------------------------------------------------------------------------------
QString VolumeRendererWidget::constructMachineThreatText(ThreatAlarmType aType,
                                                         double massG,
                                                         bool doMass)
{
  QString typeText    =  analogic::workstation::convertMapToString(aType);
  QString threatText  =  QObject::tr("Type: ");
  threatText         +=  typeText;

  if (doMass)
  {
    if (m_displayUnitSystem == VREnums::METRIC)
    {
      threatText += QObject::tr("\nMass: ") +
          QString::number(massG, 'd', 2) +
          QObject::tr(" g");
    }
    else
    {
      double massOZ = massG*OUNCES_PER_GRANM;
      threatText += QObject::tr("\nMass: ") +
          QString::number(massOZ, 'd', 2) +
          QObject::tr(" oz");
    }
  }
  return threatText;
}

//------------------------------------------------------------------------------
void VolumeRendererWidget::showWindowCenterInfo(QVector3D& p0,
                                                QVector3D& extent,
                                                const std::string strCaller)
{
  std::cout << "Window enter info - Caller = "
            << strCaller
            << std::endl;

  std::cout << "Threat P0[x,y,z]: " << p0.x() << ", "
            << p0.y() << ", "
            << p0.z() << std::endl;

  std::cout << "Threat Ext[W,H,L]: " << extent.x() << ", "
            << extent.y() << ", "
            << extent.z() << std::endl;
}

//------------------------------------------------------------------------------
}  // namespace workstation

}  // namespace analogic
//------------------------------------------------------------------------------
