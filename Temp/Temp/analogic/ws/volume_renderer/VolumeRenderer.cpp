//------------------------------------------------------------------------------
// File: VolumeRenderer.cpp
// Description: Implemenatation of Volume Renderer
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
// Added to capture performance results
#include <QElapsedTimer>
#include <vtkCallbackCommand.h>
#include <vtkPolyData.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <vtkTransform.h>
#include <vtkPlanes.h>
#include <vtkDataSetMapper.h>

#include <ctime>
#include <cstdio>
#include <bitset>

#include "VolumeRenderer.h"

//------------------------------------
// ARO-SPEEDUP-DEBUGGING
int g_dbg_call_count = 0;
//------------------------------------

// constants
const int g_flipCopyBufferSize = 2048;

const bool g_left_color_all_machine_threats = true;

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
VolumeRenderer::VolumeRenderer(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow,
                               vtkSmartPointer<vtkRenderer> renderer,
                               double pixelSpacingX,
                               double pixelSpacingY,
                               double pixelSpacingZ,
                               int maxDimX,
                               int maxDimY,
                               int maxDimZ):
  VolumeRendererBase(renderWindow,
                     renderer,
                     pixelSpacingX,
                     pixelSpacingY,
                     pixelSpacingZ),
  m_initialBuildTime_ms(0),
  m_volumeRef(nullptr),
  m_rawClippedImage(nullptr),
  m_mtRender(false),
  m_renderMutex(nullptr),
  m_clip_box(nullptr),
  m_slabbing(false),
#ifdef MASK_AND_MULTI_CALL_SPEEDUP
  m_allowThreatMasking(false),
#else
  m_allowThreatMasking(true),
#endif
  m_slabbingthickness(DEFAULT_SLAB_THICKNESS),
  m_buttonWidget({0, 0, 0, 0, 0}),
  m_button_text_actor({0, 0, 0, 0, 0}),
  m_meashuredVolumeStartPoint(753),
  m_measuredVolumeActor(nullptr),
  m_measurementTextActor(nullptr),
  m_slabUnpackPlanes(nullptr),
  m_threats(nullptr),
  m_doReomveLaptop(false),
  m_volSliceWidth(maxDimX),
  m_volSliceHeight(maxDimY),
  m_volSliceDepth(maxDimZ)
{
  this->setRenderBackground();

  this->connect(this, &VolumeRenderer::loadComplete,
                this, &VolumeRenderer::onVolumeCreated);
  if (m_volSliceDepth == 1)
    m_volSliceDepth = m_volSliceDepth + 1;

  this->initUnpackAndSlabbing();
  this->initMeasuredVolumeText();
}

//------------------------------------------------------------------------------
VolumeRenderer::~VolumeRenderer()
{
}


//------------------------------------------------------------------------------
void VolumeRenderer::initVolumeImageAttributes(double pixelSpacingX,
                                               double pixelSpacingY,
                                               double pixelSpacingZ,
                                               int maxDimX,
                                               int maxDimY,
                                               int maxDimZ)
{
  // Base class

  m_pixelSpacingX = pixelSpacingX;
  m_pixelSpacingY = pixelSpacingY;
  m_pixelSpacingZ = pixelSpacingZ;


  // This class
  m_volSliceWidth  = maxDimX;
  m_volSliceHeight = maxDimY;
  m_volSliceDepth  = maxDimZ;

  if (maxDimZ == 1)
  {
    m_volSliceDepth = m_volSliceDepth + 1;
  }

  // Init the bounds here in case we don't get to later.
  double vBx = m_pixelSpacingX*m_volSliceWidth;
  double vBy = m_pixelSpacingY*m_volSliceHeight;
  double vBz = m_pixelSpacingZ*m_volSliceDepth;
  double bounds[6] = {0, vBx,
                      0, vBy,
                      0, vBz};
  this->setBounds(bounds);
}

//------------------------------------------------------------------------------
void VolumeRenderer::initUnpackAndSlabbing()
{
  m_slabUnpackPlanes = vtkSmartPointer<vtkPlanes>::New();
  m_clipCube         = vtkSmartPointer<vtkCubeSource>::New();
}

//------------------------------------------------------------------------------
void VolumeRenderer::memLoadVolData(uint16_t* buffer,
                                    int bufferSize,
                                    vtkSmartPointer<vtkVolume> volume)
{
  m_volumeSizeBytes = bufferSize;
  QString message;
  message = "*TTTTTT* - BEGIN Memload VolumeData.";
  LOG(INFO) << message.toLocal8Bit().data();

  // Memorty load timer
  QTime msMemLoadProcTimer = QTime::currentTime();


  // Create a transfer function mapping scalar value to opacity
  // and Color.
  vtkSmartPointer<vtkPiecewiseFunction>  opacityTransferFunction =
      vtkSmartPointer<vtkPiecewiseFunction>::New();

  vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  if ( (m_opasityList.size()  == 0) &&
       (m_hsvColorList.size() == 0) )
  {
    // Fall back if we don't have a default colormap and opacity map.
    this->generateSafeNormalTransferFunctions(opacityTransferFunction,
                                              colorTransferFunction);
  }
  else
  {
    this->initHSVColorTransferFunction(colorTransferFunction, m_hsvColorList);
    this->initOpacityTransferFunction(opacityTransferFunction, m_opasityList);
  }

  message = "Created Opacity and Color Tranfer Functions";
  this->sendDebugMessage(message);
  qDebug() << message;

  m_keptImageData = vtkSmartPointer<vtkStructuredPoints>::New();

  vtkSmartPointer<vtkImageClip> clip =
      vtkSmartPointer<vtkImageClip>::New();

  vtkSmartPointer<vtkVolumeProperty> property =
      vtkSmartPointer<vtkVolumeProperty>::New();

  m_rawProperty = property.GetPointer();

  vtkSmartPointer<vtkVolumeMapper> mapper;
  if (!m_useGPU)
  {
    mapper = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
  }
  else
  {
    mapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    vtkGPUVolumeRayCastMapper* rayCastMapper = vtkGPUVolumeRayCastMapper::SafeDownCast(mapper);
    rayCastMapper->SetMaxMemoryInBytes(GPU_RENDERER_MEMORY_SIZE);
    rayCastMapper->SetMaxMemoryFraction(GPU_RENDERER_MEMORY_FRACTION);
  }

  //----------------------------------------------------
  // Set at least one pixel to a threat level to thwart
  // a VTK optrimization that removes that color
  // from the colormap.

  int clipBounds[6];
  this->dataBoundsToClipBounds(m_imageDataBounds, clipBounds);

  int nSlices = this->getNumberOfSlices(m_volumeSizeBytes,
                                        m_volSliceWidth,
                                        m_volSliceHeight);
  clipBounds[5] = nSlices - 1;

  QVector3D pSetA(clipBounds[0],
      clipBounds[2],
      clipBounds[4]);

  QVector3D pSet = QVector3D( pSetA.x(),
                              pSetA.y(),
                              pSetA.z() );

  uint16_t setSigmax =  m_transtitonMap[SIG_THREAT_MAX];

  analogic::workstation::setCornerToPreventVTKOptimization(buffer,
                                                           m_volSliceWidth,
                                                           m_volSliceHeight,
                                                           pSet,
                                                           setSigmax);


  uint16_t clipVal  = m_transtitonMap[SIG_DEFAULT_NOISE_CLIP];
  uint16_t threatMax = m_transtitonMap[SIG_THREAT_MAX];
  nSlices = this->initVolumeFromBuffer(m_keptImageData,
                                       buffer,
                                       bufferSize,
                                       clipVal,
                                       threatMax);


  message  = "Begin Volume Creation # slices =" + QString::number(nSlices);
  LOG(INFO) << message.toLocal8Bit().data();

  clip->SetInputData(m_keptImageData);

  // Clip Bounds {xmin, xmax, ymin, ymax, zmin, zmax}
  clip->SetOutputWholeExtent(clipBounds[0],
      clipBounds[1],
      clipBounds[2],
      clipBounds[3],
      clipBounds[4],
      clipBounds[5]);

  clip->ClipDataOn();

  message = "Volume Creation Complete";
  LOG(INFO) << message.toLocal8Bit().data();

  message = "Begin Mapping with Opacity/Color Transfer Functions";
  LOG(INFO) << message.toLocal8Bit().data();

  //--------------------------------------------------------
  // Color abd OPacity Transfer Functions
  property->SetScalarOpacity(opacityTransferFunction);
  property->SetColor(colorTransferFunction);

  this->setDefaultLightingParams();
  //--------------------------

  //--------------------------------------------------------

  if (m_useShader)
  {
    property->ShadeOn();
  }
  else
  {
    property->ShadeOff();
  }


  // property->SetScalarOpacityUnitDistance(m_opacitySampleDistance);
  if (m_linearInterpolation)
  {
     this->setVolumeInerpolation(property, INTERPOLATE_LINEAR);
  }
  else
  {
      this->setVolumeInerpolation(property, INTERPOLATE_NEAREST);
  }

  mapper->SetBlendModeToComposite();
  mapper->SetInputConnection(clip->GetOutputPort() );




  m_rawMapper = reinterpret_cast<vtkFixedPointVolumeRayCastMapper*>(mapper.GetPointer());

  m_rawClippedImage = mapper->GetInput();

  volume->SetMapper(mapper);
  volume->SetProperty(property);

  volume->SetMapper(mapper);
  volume->SetProperty(property);

  volume->SetUserMatrix(m_flipYMatrix);
  //----------------------------------------------------------------


  emit this->loadComplete(m_eventTimer.elapsed());

  // *** ARO-NOTE: End Mapping
  message = "Mapping Complete.";
  LOG(INFO) << message.toLocal8Bit().data();

  int msInitmemLoad = msMemLoadProcTimer.elapsed();

  QString initMessage = "*TTTTTT* - Full Init using memLoadVolData() - ";
  initMessage += this->displayName();
  initMessage += " - run length (ms), " + QString::number(msInitmemLoad);
  emit this->debugMessage(initMessage);
  LOG(INFO) << initMessage.toLocal8Bit().data();

  //----------------------------------------------
  // ARO-NOTE: 4-4-2018
  // All threats are in the LEFT  Viewer now.
  double currentVolume = 0;
  double totalVolume   = 0;
  int    numThreats    = 0;
  if (this->displayName().contains("Left"))
  {
    if (m_threats)
    {
      if (m_threats->size() > 0 )
      {
        ThreatObject* thObj = m_threats->getThreatAtIndex(0);
        if (thObj)
        {
          currentVolume       = thObj->getVolumeSize();
        }
      }
      numThreats          = m_threats->size();
    }
    double pixelScalar = m_pixelSpacingX*m_pixelSpacingY*m_pixelSpacingZ;
    currentVolume *= pixelScalar;
    // Convert to liters from CUBIC MILLIMETERS.
    currentVolume *= 1.0E-6;

    double totalVolume = this->getVolumeMMCubed();
    // Convert to Liters
    totalVolume   *= 1.0E-6;
    VolumeRendererBase::setInitTime(msInitmemLoad);
    VolumeRendererBase::setInitThreatInfo(numThreats,
                                              currentVolume,
                                              totalVolume);
  }
  else
  {
    VolumeRendererBase::addInitTime(msInitmemLoad);
    double msTotal = VolumeRendererBase::getInitTime();
    msTotal /= 1000.0;
    VolumeRendererBase::getInitThreatInfo(numThreats,
                                          currentVolume,
                                          totalVolume);

    QString fullMessage = "*TTTTTT* - COMBINED-LEFT-RIGHT: ";
    fullMessage += "Init time(sec), # Threats, VOL-Threat (L), VOL-Total(L), ";
    fullMessage +=  QString::number(msTotal)           + ", ";
    fullMessage +=  QString::number(numThreats) + ", ";
    fullMessage +=  QString::number(currentVolume)     + ", ";
    fullMessage +=  QString::number(totalVolume)       + ", ";
    LOG(INFO) << fullMessage.toLocal8Bit().data();
  }

  this->addBuildTime(msInitmemLoad);
}

//------------------------------------------------------------------------------
void VolumeRenderer::addBuildTime(int msBuildIncrement)
{
  m_initialBuildTime_ms += msBuildIncrement;
  QString timeString = QString::number(msBuildIncrement);
  QString ouputStr ="Adding " + timeString + "  milliseconds to initial build.";
  DEBUG_LOG(ouputStr.toLocal8Bit().data());
}

//------------------------------------------------------------------------------
void VolumeRenderer::resetBuildTime()
{
  m_initialBuildTime_ms = 0;
}

//------------------------------------------------------------------------------
QString VolumeRenderer::currentThreatText()
{
  return m_threats->currentText();
}

//------------------------------------------------------------------------------
bool VolumeRenderer::initFromBuffer(uint16_t* buffer,
                                    int sizeBuffer,
                                    double* bounds)
{
  //------------------------------------------------------------
  // Data Format
  // Slice width: W
  // Slice height: H
  // Pixel Size/Spacing: px, py , pz
  //         where x = slice width,
  //         y = slice height and
  //         z = slice thickness,
  //         all measurements in millimeters.
  // Data: 2 byte (16-bit) unsigned, where only 12 bits are used,
  //         meaning the data range is 0 to 4095.
  // Little Endian byte order.
  // +++++++++++++++++++++++
  //
  // ARO-NOTE: (Threading issue and vtkSmartPointer Ownership).
  // Since the vtkvolume is being passed off renderer
  // see [ m_renderer->AddViewProp(volumeFinalRef); ]
  // we don't use a sart pointer here. We will take
  // the reference in VolumeRenderer::onVolumeCreated() with
  // a smart pointer.
  // see [ volumeFinalRef.TakeReference(m_volumeRef) ]

  m_volumeRef = vtkVolume::New();
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Setup bounds if we have valid data.
  // std::cout << "------------------------------------------------------------" << std::endl;
  // std::cout << "VolumeRenderer::initFromBuffer() set m_imageDataBounds[0..5]" << std::endl;
  for (int i = 0; i < 6; i++)
  {
    m_imageDataBounds[i] =  bounds[i];
    // std::cout << "m_imageDataBounds[i]: [" << i << "] : " << m_imageDataBounds[i] << std::endl;
  }
  // std::cout << "------------------------------------------------------------" << std::endl;
  //------------------------------------------------------------

  this->memLoadVolData(buffer, sizeBuffer, m_volumeRef);
  return true;
}

//------------------------------------------------------------------------------
void VolumeRenderer::setPixelSpacing(double psX,
                                     double psY,
                                     double psZ)
{
  m_pixelSpacingX = psX;
  m_pixelSpacingY = psY;
  m_pixelSpacingZ = psZ;
}

//------------------------------------------------------------------------------
void VolumeRenderer::getPixelSpacing(double& px, double& py, double& pz)
{
  px = m_pixelSpacingX;
  py = m_pixelSpacingY;
  pz = m_pixelSpacingZ;
}

//------------------------------------------------------------------------------
void VolumeRenderer::setMaterialColorize(bool doOrganic,
                                         bool doInOrganic,
                                         bool doMetal)
{
  m_doColorizeOrganic   =  doOrganic;
  m_doColorizeInorganic =  doInOrganic;
  m_doColorizeMetal     =  doMetal;
}


//------------------------------------------------------------------------------
void VolumeRenderer::setOpacitySampleDistance(double opSampledist)
{
  m_opacitySampleDistance = opSampledist;
  if (m_rawProperty != nullptr)
  {
    m_rawProperty->SetScalarOpacityUnitDistance(m_opacitySampleDistance);
  }
  this->showSettings();
}

//------------------------------------------------------------------------------
void VolumeRenderer::setInteractiveSampleDistince(double intSampleDist)
{
  m_interactiveSampleDistance = intSampleDist;
  if (m_rawMapper != nullptr)
  {
    m_rawMapper->SetInteractiveSampleDistance(m_interactiveSampleDistance);
  }
  this->showSettings();
}


//------------------------------------------------------------------------------
void VolumeRenderer::setAutoAdjustSampleDistance(bool adjVal)
{
  m_autoAdjustSampleDistance = adjVal;
  if(m_rawMapper != nullptr)
  {
    if (!m_autoAdjustSampleDistance)
    {
      m_rawMapper->AutoAdjustSampleDistancesOff();
      m_rawMapper->SetInteractiveSampleDistance(m_interactiveSampleDistance);
    }
    else
    {
      m_rawMapper->AutoAdjustSampleDistancesOn();
    }
  }
  this->showSettings();
}



//------------------------------------------------------------------------------
void VolumeRenderer::setBgNormal()
{
  m_inverseBackground = false;
  m_backgroundRed   = VOLUME_BG_DEFAULT_RED;
  m_backgroundGreen = VOLUME_BG_DEFAULT_GREEN;
  m_backgroundBlue  = VOLUME_BG_DEFAULT_BLUE;
  this->setRenderBackground();
}

//------------------------------------------------------------------------------
void VolumeRenderer::setBgInverse()
{
  m_inverseBackground = true;
  m_backgroundRed   = VOLUME_BG_INVERSE_RED;
  m_backgroundGreen = VOLUME_BG_INVERSE_GREEN;
  m_backgroundBlue  = VOLUME_BG_INVERSE_BLUE;
  this->setRenderBackground();
}

//----------------------------------------------------------------------------
bool VolumeRenderer::getBagInverse()
{
  return m_inverseBackground;
}

//----------------------------------------------------------------------------
// ** We Brighten the color by changing only the V component of the HSV part of
// the color transfer function.
//
// The volume renderer uses a color transfer function as part of its rendering
// algorithm. The transfer function is of the form:
// { {s0, HSV0}, {s1, HSV1}, {s2, HSV2}, ... } where s = signal [0 - 4095] and
// HSV = Hue, Saturation, & Value from the HSV color space.
//
// We brighten or darken the color by changing just the V component of the HSV
// color space.
//------------------------------------------------------------------------------
void VolumeRenderer::brightenColor(int percentSet)
{
  m_currentHSVColorList.clear();
  std::list<HSVColorSigVal> brightenList;
  std::list<HSVColorSigVal>::iterator itL;
  if (!m_rawProperty) return;
  if ( m_hsvColorList.size() !=0 )
  {
    for (itL = m_hsvColorList.begin(); itL !=m_hsvColorList.end(); itL++)
    {
      HSVColorSigVal svI = *itL;
      double vOrig = svI.m_v;

      double vNew = this->scaleSaturationValue(percentSet, vOrig);
      if (vNew < 0)  vNew = 0;
      if (vNew > 1 ) vNew = 1;
      HSVColorSigVal tmpI = svI;
      tmpI.m_v = vNew;
      brightenList.push_back(tmpI);
      m_currentHSVColorList.push_back(tmpI);
    }
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
        vtkSmartPointer<vtkColorTransferFunction>::New();
    this->initHSVColorTransferFunction(colorTransferFunction,
                                       brightenList);
    this->setPropertyColor(0, colorTransferFunction);
  }
  if( m_threatOffsetHsvMap.size() > 0 && percentSet != 50)
  {
    m_isBrightnessModified = true;
    m_threatOffsetHsvBrightnessMap.clear();
    HSVColorSigVal svI;
    double vNew = 0.0;
    std::map<double, std::list<HSVColorSigVal>>::iterator itMap;
    std::list<HSVColorSigVal> orgHsvList;
    for (itMap = m_threatOffsetHsvMap.begin(); itMap !=m_threatOffsetHsvMap.end(); itMap++)
    {
      std::list<HSVColorSigVal> brightenHsvList;
      orgHsvList = itMap->second;
      for (itL = orgHsvList.begin(); itL !=orgHsvList.end(); itL++)
      {
        svI = *itL;
        vNew = this->scaleSaturationValue(percentSet, svI.m_v);
        if (vNew < 0)  vNew = 0;
        if (vNew > 1 ) vNew = 1;
        svI.m_v = vNew;
        brightenHsvList.push_back(svI);
      }
      m_threatOffsetHsvBrightnessMap[itMap->first] = brightenHsvList;
    }

    if(m_threatOffsetHsvBrightnessMap.find(m_currThreatOffset) != m_threatOffsetHsvBrightnessMap.end())
    {
      vtkSmartPointer<vtkColorTransferFunction> threatColorTransferFunction =
          vtkSmartPointer<vtkColorTransferFunction>::New();
         this->initHSVColorTransferFunction(threatColorTransferFunction,
                                            m_threatOffsetHsvBrightnessMap[m_currThreatOffset]);
      this->setPropertyColor(1, threatColorTransferFunction);
    }
  }
}

//------------------------------------------------------------------------------
// ** Opacity level is defined by the signal level in the opacity Transfer
// Function.
//
// The volume renderer has an opacity transfer function which is a map of
// opacity values (op) from 0 - 1.0, and signal strength (s) from 0 - 4096.
//
// So the map is of the form:
// { {s0, op0}, {s1, op1}, {s2, op2}, ...}
// So we make the renderer more or less "opaque" by lowering or raising each
// signal value in the transfer function.
//
// So we Increase the thus defined "Opacity" by moving the transfer function LEFT.
// (i.e. each signal value in the TF is Lower).
//
// And correspondingly we Reduce the thus defined "Opacity" by moving the transfer
// function RIGHT (i.e. each signal value in the TF is Higher).
//------------------------------------------------------------------------------
void VolumeRenderer::setOpacitySignalLevel(int percentSet)
{
  m_currentOpacityList.clear();

  std::list<OpacitySigVal> opacityList;
  std::list<OpacitySigVal>::iterator itL;
  if ( (m_rawProperty !=  nullptr ) &&
       (m_opasityList.size() !=0)

       )
  {
    for (itL = m_opasityList.begin(); itL !=m_opasityList.end(); itL++)
    {
      OpacitySigVal opI = *itL;
      double vOrig = opI.m_signalVal;

      double vNew = this->scaleSignalValue(percentSet,
                                           vOrig);
      // double allowedMax = m_transtitonMap[SIG_METAL_MAX];
      // if (vNew > allowedMax ) vNew = allowedMax;

      OpacitySigVal tmpI  = opI;
      tmpI.m_signalVal = vNew;
      opacityList.push_back(tmpI);
      m_currentOpacityList.push_back(tmpI);
    }

    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
        vtkSmartPointer<vtkPiecewiseFunction>::New();

    this->initOpacityTransferFunction(opacityTransferFunction, opacityList);
    this->setPropertyScalarOpacity(0, opacityTransferFunction);
  }
}


//------------------------------------------------------------------------------
void VolumeRenderer::setTempTransferFunctions(std::list<OpacitySigVal>&  opacityList,
                             std::list<HSVColorSigVal>& colorList)
{
  vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
      vtkSmartPointer<vtkPiecewiseFunction>::New();

  this->initOpacityTransferFunction(opacityTransferFunction, opacityList);


  vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  this->initHSVColorTransferFunction(colorTransferFunction,
                                     colorList);

  this->setPropertyScalarOpacity(0, opacityTransferFunction);
  this->setPropertyColor(0, colorTransferFunction);
}


//------------------------------------------------------------------------------
// ** We change the Contrast by varying the Value part of the opacity transfer
// function.
// The volume renderer has an opacity transfer function which is a map of
// opacity values (op) from 0 - 1.0, and signal strength (s) from 0 - 4096.
//
// So the map is of the form:
// { {s0, op0}, {s1, op1}, {s2, op2}, ...}
//
// We change the contrast by changing the value part of the transfer function.
//
//------------------------------------------------------------------------------
void VolumeRenderer::setOpacityValueLevel(int percentSet)
{
  m_currentOpacityList.clear();
  std::list<OpacitySigVal> opacityList;
  std::list<OpacitySigVal>::iterator itL;

  if ( (m_rawProperty !=  nullptr ) &&
       (m_opasityList.size() !=0)
       )
  {
    for (itL = m_opasityList.begin(); itL !=m_opasityList.end(); itL++)
    {
      OpacitySigVal opI = *itL;
      double vOrig = opI.m_opacity;

      double vNew = this->scaleOpacityValue(percentSet, vOrig);


      OpacitySigVal tmpI  = opI;
      tmpI.m_opacity = vNew;
      opacityList.push_back(tmpI);
      m_currentOpacityList.push_back(tmpI);
    }

    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
        vtkSmartPointer<vtkPiecewiseFunction>::New();

    this->initOpacityTransferFunction(opacityTransferFunction,
                                      opacityList);
    this->setPropertyScalarOpacity(0, opacityTransferFunction);
  }
}


//------------------------------------------------------------------------------
void VolumeRenderer::setPositionForSlabOrUnpacking(double cutPosition,
                                                   VREnums::AXIS axis)
{
  if (!m_rawMapper) return;
  if (m_slabbing)
  {
    // std::cout << "%%%%%%%%%%%%%% SLAB POSITION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%: "
    //           << cutPosition << std::endl;
    this->setSlabPosition(cutPosition, axis);
  }
  else
  {
    // std::cout << "%%%%%%%%%%%%%% UNPACK POSITION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%: "
    //          << cutPosition << std::endl;
    this->setUnPackingPosition(cutPosition, axis);
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::setThreats(ThreatList* threats)
{
  m_threats = threats;
}

//------------------------------------------------------------------------------
QStringList VolumeRenderer::getThreatList()
{
  QStringList emptyList;
  if (!m_threats) return emptyList;
  return m_threats->getThreatIDs();
}

//------------------------------------------------------------------------------
std::list<ThreatObject*> VolumeRenderer::getThreatObjectList()
{
  std::list<ThreatObject*> emptyList;
  if (!m_threats) return emptyList;
  return m_threats->getThreatObjectList();
}

//------------------------------------------------------------------------------
int VolumeRenderer::threatListSize()
{
  if (!m_threats) return 0;
  return m_threats->size();
}

//------------------------------------------------------------------------------
void VolumeRenderer::clearAllThreats()
{
  if (!m_threats) return;
  m_threats->clearAllThreats();
}

//------------------------------------------------------------------------------
void VolumeRenderer::clearMachineThreat(int index)
{
  // std::cout << "**** --- clearMachineThreat(index): index= [ "
  //           << index << " ]" << std::endl;
  if (!m_threats) return;
  m_threats->clearThreatDirect(index);
  m_rawClippedImage->Modified();
}

//------------------------------------------------------------------------------
void VolumeRenderer::clearOperatorThreat(int index)
{
  // std::cout << "**** --- clearOperatorThreat(index): index= [ "
  //           << index << " ]" << std::endl;
  if (!m_threats) return;
  m_threats->clearThreatMoveOperators(index);
  m_rawClippedImage->Modified();
}

//------------------------------------------------------------------------------
void VolumeRenderer::resetTransferFunctions()
{
  // std::cout << "&&&&&&&&&&&&& resetTransferFunctions() &&&&&&&&&&&&&: "
  //           << this->displayName().toStdString()
  //           << std::endl;
  if ( (m_rawProperty !=  nullptr  ) &&
       (m_opasityList.size()  != 0 ) &&
       (m_hsvColorList.size() != 0 )
     )
  {
    m_isBrightnessModified = false;
    //-----------------------------------------------
    // Set non threat property colors
    vtkSmartPointer<vtkPiecewiseFunction>  opacityTransferFunction =
        vtkSmartPointer<vtkPiecewiseFunction>::New();

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
        vtkSmartPointer<vtkColorTransferFunction>::New();
    this->initHSVColorTransferFunction(colorTransferFunction,
                                       m_hsvColorList);
    this->setPropertyColor(0, colorTransferFunction);

    this->initOpacityTransferFunction(opacityTransferFunction,
                                      m_opasityList);

    this->setPropertyScalarOpacity(0, opacityTransferFunction);
    //------------------------------------------------

    this->setMutiThreatPropertyColors(m_threatOffsetHsvMap,
                                      m_threatOffsetOpacityMap,
                                      m_threatPropertyList);


    // Left color all machine trhreats
    if(m_colorizeThreat)
    {
       this->maskAllMachineThreats(m_threats,
                                   m_threatPropertyList);
    }
  }
}


//------------------------------------------------------------------------------
void VolumeRenderer::setCurrentThreat(int index)
{
  if (!m_threats)
  {
    qDebug() << "******* THREAT NOT DEFINED!!!!!!";
    return;
  }
  if (index >= 0)
  {
    m_threats->setCurrent(index);
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::selectThreat(int index)
{
  QString message = "*SSTTSS* - VolumeRenderer::selectThreat(index) : " + QString::number(index);
  LOG(INFO) << message.toLocal8Bit().data();

  if (!m_threats)
  {
    qDebug() << "******* THREAT NOT DEFINED!!!!!!";
    return;
  }
  if (index >= 0)
  {
    m_threats->setCurrent(index);
    this->displayThreatAndBorder(index);
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::hideLaptopThreats()
{
   m_threats->forceHideLaptops();
}

//------------------------------------------------------------------------------
void VolumeRenderer::unHideMachineThreats()
{
  m_threats->unHideMachineThreats();
}

//------------------------------------------------------------------------------
void VolumeRenderer::allowThreatMask(bool setVal)
{
  m_allowThreatMasking = setVal;
  if (!setVal)
  {
    g_dbg_call_count = 0;
  }
}

//-------------------------------------------------------------------------------
bool VolumeRenderer::isThreaMaskingAllowed()
{
  return m_allowThreatMasking;
}

//------------------------------------------------------------------------------
void VolumeRenderer::setVolumeMeasurementSetPoint(uint16_t setval)
{
  uint16_t compSeed = setval;
  if (compSeed < m_volumeComputeStartMin)
  {
    compSeed = m_volumeComputeStartMin;
  }
  if (compSeed > m_volumeComputeStartMax)
  {
    compSeed = m_volumeComputeStartMax;
  }
  m_meashuredVolumeStartPoint = compSeed;
}

//------------------------------------------------------------------------------
int VolumeRenderer::initVolumeFromBuffer(vtkSmartPointer<vtkImageData> imageData,
                                         uint16_t* buffer,
                                         int sizeBuffer,
                                         uint16_t clampVal,
                                         uint16_t maxThreat)
{
  m_volumeSizeBytes = sizeBuffer;
  QTime timerInitVolume = QTime::currentTime();

  this->showSettings();
  QTime timerClamp = QTime::currentTime();
  // CLAMPING DATA
  int nData = sizeBuffer/sizeof(uint16_t);
  for (int k = 0; k < nData; k++)
  {
    if ( buffer[k] > clampVal) buffer[k] = clampVal;
  }
  buffer[0] = maxThreat;

  int timeClampMs = timerClamp.elapsed();

  QString message = "**TT** Volume clamp time elapsed: " + QString::number(timeClampMs);
  LOG(INFO) << message.toLocal8Bit().data();
  qDebug() << message;
  message = "Buffer Size (Bytes): " + QString::number(sizeBuffer);
  qDebug() << message;
  this->debugMessage(message);
  LOG(INFO) << message.toLocal8Bit().data();


  // Each dada is 2 bytes long
  int nSlices = this->getNumberOfSlices(m_volumeSizeBytes,
                                        m_volSliceWidth,
                                        m_volSliceHeight);
  if (nSlices > m_volSliceDepth)
  {
    nSlices = m_volSliceDepth;
  }

  if(nSlices == 1)
  {
      nSlices = 2;
  }

  imageData->SetDimensions(m_volSliceWidth, m_volSliceHeight, nSlices);
  //----------------------
  imageData->SetSpacing(m_pixelSpacingX,
                        m_pixelSpacingY,
                        m_pixelSpacingZ);

  double computedWidth  =  m_pixelSpacingX*m_volSliceWidth;
  double computedHeight =  m_pixelSpacingY*m_volSliceHeight;
  double computedLength =  m_pixelSpacingZ*nSlices;

  emit this->readDataRange(computedWidth, computedHeight, computedLength);

  qDebug() << "Read Data Dimensions (Width,Height,Length):"
           << computedWidth << ", "
           << computedHeight << ", "
           << computedLength;

  double originX = 0;
  double originY = 0;
  double originZ = 0;

  message = "Volume Origin X = " + QString::number(originX) + "\n" +
      "Volume Origin Y = " + QString::number(originY) + "\n" +
      "Volume Origin Z = " + QString::number(originZ) + "\n";
  this->debugMessage(message);

  imageData->SetOrigin(originX,
                       originY,
                       originZ);

  int msIVBPartA = timerInitVolume.elapsed();
  message = "*TTTTTT* - initVolumeFromBuffer part A (ms): " + QString::number(msIVBPartA);
  LOG(INFO) << message.toLocal8Bit().data();

  timerInitVolume = QTime::currentTime();

  message = "Volume Renderer start bag Memory Copy";
  LOG(INFO) << message.toLocal8Bit().data();
  imageData->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

  uint16_t* rawImageBufferPointer = static_cast<uint16_t*>(imageData->GetScalarPointer());

  //-------------------------------------------------
  // ARO-Note: 5-28-2019
  // Not sure if we need this but it
  // probably doesn't hurt as is is only two slices.
  if (nSlices == 2)
  {
    memset(rawImageBufferPointer,
           0,
           sizeBuffer);
  }
  //--------------------------------------------------

  int msAllocate = timerInitVolume.elapsed();

  message = "*TTTTTT* - Allocate VTK memory (ms):" + QString::number(msAllocate);
  LOG(INFO) << message.toLocal8Bit().data();

  timerInitVolume = QTime::currentTime();

  message = "*TTTTTT* - BEGIN Copy.";
  LOG(INFO) << message.toLocal8Bit().data();

  memcpy(rawImageBufferPointer,
         buffer,
         sizeBuffer);

  int msFlip = timerInitVolume.elapsed();
  message = "*TTTTTT* - Copy Image (ms): " + QString::number(msFlip);

  LOG(INFO) << message.toLocal8Bit().data();

  return nSlices;
}

//------------------------------------------------------------------------------
int VolumeRenderer::readVolFile(vtkSmartPointer<vtkImageData> imageData,
                                const QString filename)
{
  this->showSettings();

  QString message =  "Begin Read From file";
  LOG(INFO)<< message.toLocal8Bit().data();

  int rowIndex   = 0;
  int sliceIndex = 0;
  double minVal =  1E6;
  double maxVal = -1E6;

  QFileInfo fInfo(filename);
  int sizeFile = fInfo.size();
  // open the file:
  std::ifstream file(filename.toLocal8Bit().data(),
                     std::ios::binary);
  std::string vtkFileName = filename.toLocal8Bit().data();

  message = "File Name: = ";
  message += vtkFileName.c_str();
  qDebug() << message;
  this->debugMessage(message);

  uint16_t* pReadBuffer = new uint16_t[g_flipCopyBufferSize];
  if(pReadBuffer == nullptr)
  {
    LOG(INFO) << "readVolFile: pReadBuffer memory allocation failed !! : "
              << g_flipCopyBufferSize;
    if(file.is_open())
      file.close();
    return 0;
  }

  message = "File Size (Bytes): " + QString::number(sizeFile);
  qDebug() << message;
  this->debugMessage(message);

  // Each dada is 2 bytes long
  int nData = sizeFile/sizeof(uint16_t);
  int nSlices = nData/(m_volSliceWidth*m_volSliceHeight);
  if (nSlices > m_volSliceDepth)
  {
    nSlices = m_volSliceDepth;
  }

  imageData->SetDimensions(m_volSliceWidth,
                           m_volSliceHeight,
                           nSlices);

  //----------------------
  imageData->SetSpacing(m_pixelSpacingX,
                        m_pixelSpacingY,
                        m_pixelSpacingZ);

  double computedWidth  =  m_pixelSpacingX*m_volSliceWidth;
  double computedHeight =  m_pixelSpacingY*m_volSliceHeight;
  double computedLength =  m_pixelSpacingZ*nSlices;

  emit this->readDataRange(computedWidth,
                           computedHeight,
                           computedLength);

  qDebug() << "Read Data Dimensions (Width,Height,Length):"
           << computedWidth << ", "
           << computedHeight << ", "
           << computedLength;

  double originX = 0;
  double originY = 0;
  double originZ = 0;

  double bounds[6] = {0, computedWidth,
                      0, computedHeight,
                      0, computedLength};

  this->setBounds(bounds);

  message = "Volume Origin X = " + QString::number(originX) + "\n" +
      "Volume Origin Y = " + QString::number(originY) + "\n" +
      "Volume Origin Z = " + QString::number(originZ) + "\n";
  this->debugMessage(message);

  imageData->SetOrigin(originX,
                       originY,
                       originZ);
  message = "Begin Memory Allocation...";
  LOG(INFO)<< message.toLocal8Bit().data();

  imageData->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

  message = "Memory Creation Complete";
  LOG(INFO)<< message.toLocal8Bit().data();

  // Seek to start of File
  file.seekg(0, std::ios::beg);
  // read the data:
  uint16_t iBuf;

  int updateIndex = 0;
  int updateDelta = nData/100;

  int nBuffers = nData/m_volSliceWidth;

  uint16_t* rawImageBufferPointer = static_cast<uint16_t*>(imageData->GetScalarPointer());

  int W = m_volSliceWidth;
  int H = m_volSliceHeight;

  uint16_t dataclipVal = (uint16_t)m_transtitonMap[SIG_DEFAULT_NOISE_CLIP];

  for (int i = 0; i < nBuffers; i++)
  {
    //-----------------------------------
    // ARO-NOTE: The Y axiz is flipped
    // in the raw data. So we have
    // [ y = (H - 1) - rowIndex; ]
    // Insead of [ y = rowIndex; ]

    int y = (H - 1) - rowIndex;
    //-----------------------------------

    int z = sliceIndex;

    file.read(reinterpret_cast<char*>(&pReadBuffer[0]), sizeof(uint16_t)*m_volSliceWidth);
    for (int j = 0; j < m_volSliceWidth; j++)
    {
      int colIndex = j;
      int x = colIndex;
      iBuf = pReadBuffer[j];

      uint16_t rawData;
      if ( ( x   < m_volSliceWidth  ) &&
           ( y   < m_volSliceHeight ) &&
           ( z   < nSlices          ) )
      {
        //-------------------------------------------------------
        // Use image loc to place value in imageData buffer.
        int imageLoc = (W*H)*z + (W*y) + x;

        rawData    = iBuf;
        if (rawData > dataclipVal )
        {
          rawData = dataclipVal;
        }
        rawImageBufferPointer[imageLoc] = rawData;

        if (rawData > maxVal)
        {
          maxVal = rawData;
        }
        if (rawData < minVal)
        {
          minVal = rawData;
        }
      }

      //------------------------
      // Updagte
      updateIndex++;
      if (updateIndex == updateDelta)
      {
        updateIndex = 0;
        // double dLoc  = i*m_volSliceWidth + j;

        // double progress = (dLoc*100.0)/nData;
        // emit this->readProgress(progress);
      }
    }
    rowIndex++;
    if (rowIndex == m_volSliceHeight)
    {
      rowIndex = 0;
      sliceIndex++;
    }
    if (sliceIndex > nSlices )
    {
      break;
    }
  }
  file.close();

  delete [] pReadBuffer;

  message = "Vol File Read Complete";
  LOG(INFO)<< message.toLocal8Bit().data();

  return nSlices;
}


//------------------------------------------------------------------------------
void VolumeRenderer::onVolumeCreated(int msLoad)
{
  vtkSmartPointer<vtkVolume> volumeFinalRef;
  volumeFinalRef.TakeReference(m_volumeRef);
  m_volumeRef = nullptr;

  //---------------------------------------------------
  // ARO-NOTE: While this update seems to
  // be unnecessary, it is. It actually saves time in overall
  // rendering in actual testing.
  // A. Oughton 6-12-2017
  volumeFinalRef->Update();
  // Do not remove.
  //---------------------------------------------------

  m_rendererRef->AddViewProp(volumeFinalRef);

  qDebug() << "*GGXXGG* Volume Renderer: - RenderWindow And Renderer: "
           << (uint64_t)m_renderWindowRef
           << (uint64_t)m_rendererRef;

  m_rawVolume = volumeFinalRef.GetPointer();

  double bounds[6];
  this->getBounds(bounds);
  qDebug() << "***** Resetting Camera Bounds On Volume Created";

  // Bounds: (xmin,xmax, ymin,ymax, zmin,zmax).
  this->resetCameraWithBounds(bounds);

  QString message = "::: Load Complete! in " + QString::number(msLoad) + " ms";
  this->debugMessage(message);

  double  bx0 = bounds[0];
  double  bx1 = bounds[1];
  double  by0 = bounds[2];
  double  by1 = bounds[3];
  double  bz0 = bounds[4];
  double  bz1 = bounds[5];

  QString threatMessage = "Volume Bounds P0(x,y,z)-P1(x,y,z): [(";
  threatMessage += QString::number(bx0) + ", ";
  threatMessage += QString::number(by0) + ", ";
  threatMessage += QString::number(bz0) + ") - (";
  threatMessage += QString::number(bx1) + ", ";
  threatMessage += QString::number(by1) + ", ";
  threatMessage += QString::number(bz1) + ")]";

  this->debugMessage(threatMessage);

  LOG(INFO) << threatMessage.toLocal8Bit().data();
  QTime firstrRenderTimer = QTime::currentTime();

  int msFirstRender = firstrRenderTimer.elapsed();


  this->setActualBounds(volumeFinalRef);

  bx0 = m_actual_bounds[0];
  bx1 = m_actual_bounds[1];
  by0 = m_actual_bounds[2];
  by1 = m_actual_bounds[3];
  bz0 = m_actual_bounds[4];
  bz1 = m_actual_bounds[5];


  QString actualBoundsString = "VTK Volume Reference Actual Bounds: P0(x,y,z)-P1(x,y,z): [(";
  actualBoundsString += QString::number(bx0) + ", ";
  actualBoundsString += QString::number(by0) + ", ";
  actualBoundsString += QString::number(bz0) + ") - (";
  actualBoundsString += QString::number(bx1) + ", ";
  actualBoundsString += QString::number(by1) + ", ";
  actualBoundsString += QString::number(bz1) + ")]";

  this->debugMessage(actualBoundsString);

  this->createAndSetMask();

  this->addBuildTime(msFirstRender);
}


//------------------------------------------------------------------------------
void VolumeRenderer::showSettings()
{
  QString strUseGPU;
  if (m_useGPU)
  {
    strUseGPU = "Using GPU: true \n";
  }
  else
  {
    strUseGPU = "Using GPU: false \n";
  }
  QString strAutoAdjust;
  if (m_autoAdjustSampleDistance)
  {
    strAutoAdjust = "Auto Adjust: true \n";
  }
  else
  {
    strAutoAdjust = "Auto Adjust: false \n";
  }

  QString strOpacitySampleDist =  "Opacity Sample distance: " +
      QString::number(m_opacitySampleDistance) + "\n";

  QString strIntractiveveSampleDist = "Interactive Sample Distance:" +
      QString::number(m_interactiveSampleDistance) + "\n";

  QString settingsMsg = "** Volume Renderer Settings: \n" +
      strUseGPU +
      strAutoAdjust +
      strOpacitySampleDist +
      strIntractiveveSampleDist;

  emit this->debugMessage(settingsMsg);
}

//------------------------------------------------------------------------------
void VolumeRenderer::showBitMapDensity(SDICOS::Bitmap* pBMP)
{
  QTime densityTimer = QTime::currentTime();
  uint32_t W;
  uint32_t H;
  uint32_t L;
  pBMP->GetDims(W, H, L);

  int sizeBmp    = W*H*L;
  int countZero  = 0;
  int countOne   = 0;
  double percentFull;
  for (uint32_t k = 0; k < L;  k++)
  {
    for (uint32_t j = 0; j < H; j++)
    {
      for (uint32_t i =0; i < W; i++)
      {
        if ( pBMP->GetBit(i, j, k) )
        {
          countOne++;
        }
        else
        {
          countZero++;
        }
      }
    }
  }
  int msDensityCalcTime =  densityTimer.elapsed();

  percentFull = (100.0*countOne)/(1.0*sizeBmp);
  std::cout << "SDICOS::Bitmap size [ W, H, L] : "
            <<  W << ", "
            <<  H << ", "
            <<  L << std::endl;

  std::cout << "SDICOS::Bitmap density timer (ms) : "
            <<  msDensityCalcTime
            << std::endl;
  std::cout << "SDICOS::Bitmap stats [#Zero, #One, Size, (#0 + #1), %Full]: "
            << countZero << ", "
            << countOne  << ", "
            << sizeBmp   << ", "
            << (countZero + countOne) << ", "
            << percentFull
            << std::endl;
}

//------------------------------------------------------------------------------
double VolumeRenderer::averageImageData(vtkImageData* imageData,
                                        QVector3D& P0,
                                        int W,
                                        int H,
                                        int L)
{
  double image_sum     = 0;
  double image_count   = W*H*L;
  double image_average = 0;
  int originX  = P0.x();
  int originY  = P0.y();
  int originZ  = P0.z();

  for (int z = 0; z < L; z++)
  {
    for (int y = 0; y < H; y++)
    {
      for (int x = 0; x < W; x++)
      {
        uint16_t* pixel = static_cast< uint16_t* >(imageData->GetScalarPointer(originX + x,
                                                                 originY + y,
                                                                 originZ + z));
        image_sum+=pixel[0];
      }
    }
  }
  std::cout << "Image Dims [W,H,L] : "
            << W << " , "
            << H << " , "
            << L << std::endl;

  std::cout << "Number of points   : "
            << image_count
            << std::endl;
  if (image_count > 0)
  {
    image_average = image_sum/image_count;
    std::cout << "Average value of Image = "
              << image_average
              << std::endl;
  }
  return image_average;
}

//------------------------------------------------------------------------------
void VolumeRenderer::saveImageData(std::string& filenameOutVol,
                                   std::string& filenameOutProp,
                                   vtkImageData* imageData,
                                   QVector3D& P0,
                                   int W,
                                   int H,
                                   int L)
{
  int originX  = P0.x();
  int originY  = P0.y();
  int originZ  = P0.z();

  std::ofstream outFileVol;
  std::ios_base::openmode mode = (std::ios_base::out | std::ios_base::binary);



  outFileVol.open(filenameOutVol, mode);
  if(outFileVol.is_open())
  {
    for (int z = 0; z < L; z++)
    {
      for (int y = 0; y < H; y++)
      {
        for (int x = 0; x < W; x++)
        {
          uint16_t* pixel = static_cast< uint16_t* >(imageData->GetScalarPointer(originX + x,
                                                                                 originY + y,
                                                                                 originZ + z));
          outFileVol.write( reinterpret_cast<char*>(&pixel[0]), sizeof(uint16_t));
        }
      }
    }
    outFileVol.close();
  }
  else
  {
    std::cout << "Error opening Vloume out File: "
              << filenameOutVol
              << std::endl;
    return;
  }

  std::ofstream outFileProp;
  mode = std::ios_base::out;
  outFileProp.open(filenameOutProp, mode);
  if (outFileProp.is_open())
  {
    std::string strPro = "WIDTH= " + std::to_string(W) + "\n";
    outFileProp.write(strPro.c_str(), strPro.length());

    strPro = "HEIGHT= " + std::to_string(H) + "\n";
    outFileProp.write(strPro.c_str(), strPro.length());

    strPro = "LENGTH= " + std::to_string(L) + "\n";
    outFileProp.write(strPro.c_str(), strPro.length());


    strPro = "BYTES_PER_PIXEL= " + std::to_string(sizeof(uint16_t)) + "\n";
    outFileProp.write(strPro.c_str(), strPro.length());
    outFileProp.close();
  }
  else
  {
    std::cout << "Error opening Properties out File: "
              << filenameOutProp
              << std::endl;
    return;
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::initMeasuredVolumeText()
{
  vtkRenderer*    renderer =  m_renderWindowRef->GetRenderers()->GetFirstRenderer();
  if (!renderer) return;

  m_measurementTextActor = vtkSmartPointer<vtkTextActor>::New();

  m_measurementTextActor->SetInput("");

  m_measurementTextActor->SetPosition(VOL_MEASURE_TEXT_POS_X,
                                      VOL_MEASURE_TEXT_POS_Y);

  m_measurementTextActor->GetTextProperty()->SetFontSize(VOL_MEASURE_TEXT_FONT_SIZE);
  m_measurementTextActor->GetTextProperty()->SetBold(1);
  m_measurementTextActor->GetTextProperty()->SetColor(VOL_MEASURE_TEXT_RED,
                                                      VOL_MEASURE_TEXT_GREEN,
                                                      VOL_MEASURE_TEXT_BLUE);
  renderer->AddActor2D(m_measurementTextActor);
}

//------------------------------------------------------------------------------
void VolumeRenderer::setMeasuredVolumeText(std::string& textStr)
{
    m_measurementTextActor->SetInput(textStr.c_str());
}

//------------------------------------------------------------------------------
void VolumeRenderer::sendDebugMessage(QString& message)
{
  emit this->debugMessage(message);
}

//------------------------------------------------------------------------------
double VolumeRenderer::getBMPixelVolumeNew(SDICOS::Bitmap* pBMP)
{
  if(pBMP == nullptr)
  {
     return 0;
  }
  double nPixels = 0;
  double px, py, pz;
  SDICOS::S_UINT32 nx, ny, nz;
  pBMP->GetDims(nx, ny, nz);
  this->getPixelSpacing(px, py, pz);
  QTime msBMPVolumeMeasureTimer = QTime::currentTime();

  const unsigned char* buf = pBMP->GetBitmap().GetData();

  for (size_t i = 0; i < pBMP->GetBitmap().GetSize(); ++i) {
      nPixels += std::bitset<8>(buf[i]).count();
  }

  int msVolMeasure = msBMPVolumeMeasureTimer.elapsed();
  QString measureTimer = "BMP based volume measure timer (ms) = " + QString::number(msVolMeasure);
  LOG(INFO) << measureTimer.toStdString();

  std::cout << measureTimer.toStdString()
            << std::endl;

  double retVal = nPixels*px*py*pz;
  return retVal;
}

//------------------------------------------------------------------------------
double VolumeRenderer::getBMPixelVolume(SDICOS::Bitmap* pBMP)
{
  if(pBMP == nullptr)
  {
     return 0;
  }
  double nPixels = 0;
  double px, py, pz;
  SDICOS::S_UINT32 nx, ny, nz;
  pBMP->GetDims(nx, ny, nz);
  this->getPixelSpacing(px, py, pz);

  QTime msBMPVolumeMeasureTimer = QTime::currentTime();
  for (SDICOS::S_UINT32 k = 0; k < nz; k++)
  {
    for (SDICOS::S_UINT32 j = 0; j < ny; j++ )
    {
      for (SDICOS::S_UINT32 i = 0; i < nx; i++)
      {
        if (pBMP->GetBit(i, j, k))
        {
          nPixels += 1;
        }
      }
    }
  }
  int msVolMeasure = msBMPVolumeMeasureTimer.elapsed();
  QString measureTimer = "BMP based volume measure timer (ms) = " + QString::number(msVolMeasure);
  LOG(INFO) << measureTimer.toStdString();

  std::cout << measureTimer.toStdString()
            << std::endl;

  double retVal = nPixels*px*py*pz;
  return retVal;
}


//------------------------------------------------------------------------------
void VolumeRenderer::setColorLevel(float colorLevel)
{
  vtkGPUVolumeRayCastMapper* gpuMapper =  vtkGPUVolumeRayCastMapper::SafeDownCast(m_rawMapper);
  if (gpuMapper)
  {
    gpuMapper->SetFinalColorLevel(colorLevel);
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::getImageBounds(double* bounds)
{
  if (!m_rawClippedImage)
  {
    for (int i = 0; i < 6; i++)
    {
      bounds[i] = 0;
    }
    return;
  }
  double tmpBounds[6];
  m_rawClippedImage->GetBounds(tmpBounds);
  for (int i = 0; i < 6; i++)
  {
    bounds[i] = tmpBounds[i];
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::setWindowLevel(float windowLevel)
{
  vtkGPUVolumeRayCastMapper* gpuMapper =  vtkGPUVolumeRayCastMapper::SafeDownCast(m_rawMapper);
  if (gpuMapper)
  {
    gpuMapper->SetFinalColorWindow(windowLevel);
  }
}

//------------------------------------------------------------------------------
ThreatList* VolumeRenderer::getThreats()
{
  return m_threats;
}

//------------------------------------------------------------------------------
void VolumeRenderer::writeThreatVolumeToFile(QString& volFile)
{
  if (!m_rawClippedImage) return;

  QString volFile2 = volFile.left(volFile.size()-3);
  volFile2 += "stash.vol";

  int theatIndex = m_threats->getCurrent();
  if (theatIndex == -1) return;

  std::vector<uint16_t>& threatStashBuffer = m_threats->getThreatStashBuffer();
  std::vector<uint16_t>& threatImageBuffer = m_threats->getThreatImageData();

  // Save Image Stash buffer
  std::string saveFile = volFile.toLocal8Bit().data();
  std::ofstream outFile;
  int sizeBuff = threatStashBuffer.size()*sizeof(uint16_t);
  outFile.open(saveFile, ios::out | ios::binary);
  outFile.write(reinterpret_cast<char*>(&threatStashBuffer[0]), sizeBuff);
  outFile.close();

  // Save Threat Image Buffer
  std::string savFileThreatImage = volFile2.toLocal8Bit().data();
  std::ofstream outFileThreatImage;
  int sizeBuff2 = threatImageBuffer.size()*sizeof(uint16_t);
  outFileThreatImage.open(savFileThreatImage, ios::out | ios::binary);
  outFileThreatImage.write(reinterpret_cast<char*>(&threatImageBuffer[0]), sizeBuff2);
  outFileThreatImage.close();

  ThreatObject* pCur = m_threats->getThreatAtIndex(theatIndex);
  ThreatVolume  curVol = pCur->getVolume();

  int W = curVol.p1().x() - curVol.p0().x();
  int H = curVol.p1().y() - curVol.p0().y();
  int L = curVol.p1().z() - curVol.p0().z();

  std::string savePropertiesFile = saveFile + ".properties";
  std::ofstream outFileProps;
  outFileProps.open(savePropertiesFile, ios::out);
  outFileProps << pCur->getText().toLocal8Bit().data() << std::endl;
  outFileProps << "W=" << W << std::endl;
  outFileProps << "H=" << H << std::endl;
  outFileProps << "L=" << L << std::endl;
  outFileProps.close();
}

//------------------------------------------------------------------------------
void VolumeRenderer::slabMaskNonthreatLaptop(vtkSmartPointer<vtkColorTransferFunction> normalColorTF,
                                             vtkSmartPointer<vtkPiecewiseFunction>  normalOpacityTF)
{
  this->removeLabelMaskColors();
  this->setMaskTypeToBinary();
  this->setMaskBitsToValue(1);
  this->setPropertyColor(0, normalColorTF);
  this->setPropertyScalarOpacity(0, normalOpacityTF);
  m_maskNeedsColorRemoval = false;
}


//------------------------------------------------------------------------------
void VolumeRenderer::setPrimaryColorAndOpacity(std::list<HSVColorSigVal>& normalHsvColorList,
                                               std::list<OpacitySigVal>&  normalOpacityList)
{
  std::list<OpacitySigVal> thinOpacityList;
  this->increaseOpacityOnThinSlice(normalOpacityList, thinOpacityList);

  vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
      vtkSmartPointer<vtkColorTransferFunction>::New();
  this->initHSVColorTransferFunction(colorTransferFunction,
                                     normalHsvColorList);

  vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
      vtkSmartPointer<vtkPiecewiseFunction>::New();

  this->initOpacityTransferFunction(opacityTransferFunction,
                                    thinOpacityList);

  this->setPropertyColor(0, colorTransferFunction);
  this->setPropertyScalarOpacity(0, opacityTransferFunction);
}

//------------------------------------------------------------------------------
void VolumeRenderer::setMutiThreatPropertyColors(std::map<double, std::list<HSVColorSigVal>>& hsvThreatMap,
                                                 std::map<double, std::list<OpacitySigVal>>&  opacityThreatMap,
                                                 std::list<ThretOffsetProperty>& threatPropertyList)
{
  std::list<ThretOffsetProperty>::iterator litt;
  for (litt = threatPropertyList.begin(); litt != threatPropertyList.end(); litt++)
  {
    int vtk_index    = litt->m_vtkIndex;
    uint16_t offVal  = litt->m_threatOffset;

    std::list<HSVColorSigVal>  mapThreatHsvList =  hsvThreatMap[offVal];
    std::list<OpacitySigVal>   mapThreatOpList  =  opacityThreatMap[offVal];
    std::list<OpacitySigVal>   thinThreatOpList;
    this->increaseOpacityOnThinSlice(mapThreatOpList, thinThreatOpList);

    vtkSmartPointer<vtkColorTransferFunction> mapColorTF =   vtkSmartPointer<vtkColorTransferFunction>::New();
    this->initHSVColorTransferFunction(mapColorTF,
                                       mapThreatHsvList,
                                       true);

    vtkSmartPointer<vtkPiecewiseFunction> opacityTF =   vtkSmartPointer<vtkPiecewiseFunction>::New();
    this->initOpacityTransferFunction(opacityTF,
                                      thinThreatOpList);

    //------------------------------------------------------------
    // ARO-TODO setPtoprtyColor( index -- 0,1,2,3)
    // this->m_rawProperty->SetColor(index, colorTF);
    // We want to test the VTK limitiation on color channel for
    // Volume rendering:
    //  From: vtkVolumeProperty.h
    //  /**
    //  * Set the color of a volume to an RGB transfer function
    //  * for the component indicated by index. This will set the
    //  * color channels for this component to 3.
    //  * This will also recompute the color channels
    //  */
    //  void SetColor(int index, vtkColorTransferFunction *function);
    //  void SetColor(vtkColorTransferFunction *function)
    //  { this->SetColor(0, function); }
    // ------------------------------------------------------------
    // std::cout << "setPropertyColor(index,mapColorTF) index ="
    //          << vtk_index
    //          << std::endl;
    //-------------------------------------------------------------

    this->setPropertyColor(vtk_index, mapColorTF);
    this->setPropertyScalarOpacity(vtk_index, opacityTF);
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::displayThreatAndBorder(int threatIndex)
{
  // std::cout << "displayThreatAndBorder(int threatIndex)"
  //           << std::endl;
  if (!m_rawClippedImage) return;

  int W, H, L;
  QVector3D P0;

  //-------------------------------------------------------
  m_threats->getThreatBoundaries(threatIndex,
                                 W, H, L, P0);

  //------------------------------------------------
  // ARO-SPEEDUP
  // Reset the mask only if the current index
  // and the last index wer different.
  int currentThreat = m_threats->getCurrent();
  if (currentThreat != threatIndex)
  {
    this->resetMask();
  }
  //-----------------------------------------------

  // Code added for performance optimization
  ThreatObject* pThr = m_threats->getThreatAtIndex(threatIndex);
  if(!pThr)
  {
    QString nullMessage = "VolumeRenderer::displayThreatAndBorder(): getThreatAtIndex(index) is NULL Pointer!";
    LOG(INFO) << nullMessage.toLocal8Bit().data();
    return;
  }
  m_currThreatOffset = pThr->getOffset();


  // Create and set threat Color
  if(m_isBrightnessModified)
  {
    if(m_threatOffsetHsvBrightnessMap.find(m_currThreatOffset) != m_threatOffsetHsvBrightnessMap.end())
    {
      m_currThreatHsvList = m_threatOffsetHsvBrightnessMap[m_currThreatOffset];
    }
  }
  else
  {
    if(m_threatOffsetHsvMap.find(m_currThreatOffset) != m_threatOffsetHsvMap.end())
    {
      m_currThreatHsvList = m_threatOffsetHsvMap[m_currThreatOffset];
    }
  }

  SDICOS::Bitmap* pBMP = m_threats->bmpAtIndex(threatIndex);

  bool isLaptop = m_threats->isLaptopAtIndex(threatIndex);
  bool bLaptopRemoval = false;
  if (isLaptop && m_doReomveLaptop)
  {
    this->setMaskTypeToBinary();
    this->setMaskBitsToValue(1);
    bLaptopRemoval = true;
  }

  bool retVal = true;

  if (!g_left_color_all_machine_threats)
  {
    //---------------------------
    // Left color only current machine threat
    vtkSmartPointer<vtkColorTransferFunction> threatColorTF =   vtkSmartPointer<vtkColorTransferFunction>::New();
    this->initHSVColorTransferFunction(threatColorTF,
                                       m_currThreatHsvList);

    this->setPropertyColor(1, threatColorTF);

    if (m_colorizeThreat || m_doReomveLaptop)
    {
      retVal = this->threatMaskSubVolume(W, H, L, P0,
                                         pBMP,
                                         m_rawClippedImage,
                                         m_currThreatOffset,
                                         m_mask,
                                         bLaptopRemoval);
    }
  }
  else
  {
    this->setMutiThreatPropertyColors(m_threatOffsetHsvMap,
                                      m_threatOffsetOpacityMap,
                                      m_threatPropertyList);

    // Left color all machine trhreats
    if (!isLaptop)
    {
      if(m_colorizeThreat)
      {
        retVal = this->maskAllMachineThreats(m_threats,
                                             m_threatPropertyList);
      }
    }
    else
    {
      if (m_doReomveLaptop)
      {
        retVal = this->threatMaskSubVolume(W, H, L, P0,
                                           pBMP,
                                           m_rawClippedImage,
                                           m_currThreatOffset,
                                           m_mask,
                                           bLaptopRemoval);
      }
    }
  }

  if (!retVal)
  {
    QString threatInitErrr = "*GGVVGG* Return value from threat threatMaskSubVolume() = false.";
    qDebug() << threatInitErrr;
    LOG(INFO) << threatInitErrr.toLocal8Bit().data();
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::setTransferfunctionsInderect(std::list<HSVColorSigVal>& newHsvColorList,
                                                 std::list<OpacitySigVal>&  newOpacityList)
{
  vtkSmartPointer<vtkColorTransferFunction> newColorTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  vtkSmartPointer<vtkPiecewiseFunction>  newOpacityTF =
      vtkSmartPointer<vtkPiecewiseFunction>::New();

  this->initHSVColorTransferFunction(newColorTF,
                                     newHsvColorList);

  this->initOpacityTransferFunction(newOpacityTF,
                                    newOpacityList);


  this->setPropertyScalarOpacity(0, newOpacityTF);
  this->setPropertyColor(0, newColorTF);
}


//------------------------------------------------------------------------------
void VolumeRenderer::setTransferfunctionsDirect(std::list<HSVColorSigVal>& newHsvColorList,
                                                 std::list<OpacitySigVal>&  newOpacityList)
{
  m_opasityList.clear();
  m_hsvColorList.clear();
  m_currentOpacityList.clear();
  m_currentHSVColorList.clear();
  std::list<HSVColorSigVal>::iterator itCol;
  std::list<OpacitySigVal>::iterator itOp;

  // std::cout << "setTransferfunctionsDirect: XXXXXXXXXXXXXXX setTransferfunctionsDirect" << std::endl;

  for (itCol = newHsvColorList.begin(); itCol != newHsvColorList.end(); itCol++)
  {
    HSVColorSigVal cVal = *itCol;
    m_hsvColorList.push_back(cVal);
    m_currentHSVColorList.push_back(cVal);
  }

  for (itOp = newOpacityList.begin(); itOp != newOpacityList.end(); itOp++)
  {
    OpacitySigVal opVal = *itOp;
    m_opasityList.push_back(opVal);
    m_currentOpacityList.push_back(opVal);
  }


  vtkSmartPointer<vtkColorTransferFunction> newColorTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  vtkSmartPointer<vtkPiecewiseFunction>  newOpacityTF =
      vtkSmartPointer<vtkPiecewiseFunction>::New();

  this->initHSVColorTransferFunction(newColorTF,
                                     newHsvColorList);

  this->initOpacityTransferFunction(newOpacityTF,
                                    newOpacityList);


  this->setPropertyScalarOpacity(0, newOpacityTF);
  this->setPropertyColor(0, newColorTF);
}

//------------------------------------------------------------------------------
void VolumeRenderer::displayOperatorThreatFromSibling(int W,
                                                      int H,
                                                      int L,
                                                      QVector3D P0,
                                                      double ambient,
                                                      double diffuse,
                                                      double specular,
                                                      std::list<HSVColorSigVal>& sibThreatHsvList,
                                                      std::list<OpacitySigVal>&  sibThreatOpacityList)
{
  if (!m_rawClippedImage) return;

  //-------------------------------------------
  // ARO-Note: timing remap method
  // m_eventTimer = QTime::currentTime();

  this->reMapFullVolume();


  // int remapTime = m_eventTimer.elapsed();
  // std::cout << "Time to Remap in MS: "
  //           <<  remapTime << std::endl;
  //------------------------------------------

  this->resetMask();

  this->setThreatViewOpacityAndColorTF(sibThreatOpacityList, sibThreatHsvList);

  vtkSmartPointer<vtkColorTransferFunction> threatColorTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  vtkSmartPointer<vtkPiecewiseFunction>  threatOpacityTF =
      vtkSmartPointer<vtkPiecewiseFunction>::New();

  this->initHSVColorTransferFunction(threatColorTF,
                                     sibThreatHsvList);

  this->initOpacityTransferFunction(threatOpacityTF,
                                    sibThreatOpacityList);

  this->setPropertyColor(1, threatColorTF);

  m_rawProperty->SetAmbient(ambient);
  m_rawProperty->SetDiffuse(diffuse);
  m_rawProperty->SetSpecular(specular);


  this->setMaskTypeToBinary();
  this->setMaskBitsToValue(0);
  this->setTempTransferFunctions(sibThreatOpacityList, sibThreatHsvList);

  this->applyOperatorRenderMask(W, H, L, P0, m_rawClippedImage, m_mask);

  m_mask->Modified();
}


//------------------------------------------------------------------------------
void VolumeRenderer::displayCroppedThreatFromSibling(int threatIndex,
                                                     int W,
                                                     int H,
                                                     int L,
                                                     QVector3D P0,
                                                     SDICOS::Bitmap* pBMP,
                                                     analogic::workstation::ThreatAlarmType threatType,
                                                     bool isLaptop,
                                                     double ambient,
                                                     double diffuse,
                                                     double specular,
                                                     std::list<HSVColorSigVal>& sibNormalHsvList,
                                                     std::list<OpacitySigVal>& sibNormalOpacityList,
                                                     std::list<HSVColorSigVal>& sibThreatHsvList,
                                                     std::list<OpacitySigVal>& sibThreatOpacityList)
{
  if (!m_rawClippedImage) return;
  this->removeCenteringSphereIfPresent();
  this->removeMeasuredVolume();
  this->removeMeasuredText();

  vtkSmartPointer<vtkColorTransferFunction> normalColorTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  vtkSmartPointer<vtkPiecewiseFunction>  normalOpacityTF =
      vtkSmartPointer<vtkPiecewiseFunction>::New();


  this->initHSVColorTransferFunction(normalColorTF,
                                     sibNormalHsvList);

  this->initOpacityTransferFunction(normalOpacityTF,
                                    sibNormalOpacityList);

  //--------------------------------------------

  vtkSmartPointer<vtkColorTransferFunction> threatColorTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  vtkSmartPointer<vtkPiecewiseFunction>  threatOpacityTF =
      vtkSmartPointer<vtkPiecewiseFunction>::New();

  this->initHSVColorTransferFunction(threatColorTF,
                                     sibThreatHsvList,
                                     true);

  this->initOpacityTransferFunction(threatOpacityTF,
                                    sibThreatOpacityList);

   bool doPI_Clamp = false;
   if ( (threatType == analogic::workstation::BLUNT) ||
        (threatType == analogic::workstation::SHARP) ||
        (threatType == analogic::workstation::GUN)   ||
        (threatType == analogic::workstation::SHIELD)
      )
   {
      doPI_Clamp = true;
   }
  //-----------------------------------------------------------------------------
  // HERE we clip the  Subvlume and give it to the volume mapper.
  // We now pass in THREAT INDEX so don't do CLIP if the threat is the same, as
  // the one jus clipped since we already have the data.
  if (threatIndex != m_currentClippedThreatIndex)
  {
    this->mapClippedSubVolume(P0, W, H, L, doPI_Clamp);
    m_currentClippedThreatIndex = threatIndex;
  }

  this->resetMaskWithResize(W, H, L);

  this->setMaskColors(isLaptop,
                      pBMP,
                      W, H, L,
                      normalColorTF,
                      normalOpacityTF,
                      threatColorTF,
                      threatOpacityTF);

  m_rawProperty->SetAmbient(ambient);
  m_rawProperty->SetDiffuse(diffuse);
  m_rawProperty->SetSpecular(specular);
  this->applyCroppedRenderMask(W, H, L, pBMP, m_mask);
  m_mask->Modified();
  //----------------------------------------
}

//-----------------------------------------------------------------------------
void VolumeRenderer::setMaskColors(bool isLaptop,
                                   SDICOS::Bitmap* pBMP,
                                   int W, int H, int L,
                                   vtkColorTransferFunction* normalColorTF,
                                   vtkPiecewiseFunction* normalOpacityTF,
                                   vtkColorTransferFunction* threatColorTF,
                                   vtkPiecewiseFunction*  threatOpacityTF)
{
  if (!isLaptop)
  {
    // BitmapOnly-ColorizeThreat [Truth Table]
    // 0-0:
    if ((!m_threatBMPOnly) && (!m_colorizeThreat))
    {
      this->setMaskTypeToLabel();
      this->setMaskBitsToValueBySize(0, W, H, L);
      this->setPropertyColor(0, normalColorTF);
      this->setPropertyColor(1, normalColorTF);
      this->setPropertyScalarOpacity(0, normalOpacityTF);
      this->setPropertyScalarOpacity(1, normalOpacityTF);
      m_maskNeedsColorRemoval = false;
    }
    // 0-1:
    if ((!m_threatBMPOnly)  &&  (m_colorizeThreat))
    {
      this->setMaskTypeToLabel();
      this->setMaskBitsToValueBySize(0, W, H, L);
      this->setPropertyColor(0, normalColorTF);
      this->setPropertyColor(1, threatColorTF);
      this->setPropertyScalarOpacity(0, normalOpacityTF);
      this->setPropertyScalarOpacity(1, threatOpacityTF);
      m_maskNeedsColorRemoval = false;
    }
    // 1-0:
    if ((m_threatBMPOnly)  &&  (!m_colorizeThreat))
    {
      if (!pBMP)
      {
        // For OPERATOR threats revert to:
        // BitmapOnly-ColorizeThreat [Truth Table]: 0-1
        this->setMaskTypeToLabel();
        this->setMaskBitsToValueBySize(0, W, H, L);
        this->setPropertyColor(0, normalColorTF);
        this->setPropertyColor(1, threatColorTF);
        this->setPropertyScalarOpacity(0, normalOpacityTF);
        this->setPropertyScalarOpacity(1, threatOpacityTF);
        m_maskNeedsColorRemoval = false;
      }
      else
      {
        if (m_maskNeedsColorRemoval)
        {
          this->removeLabelMaskColors();
          //------------------------------------------------------
          // Once that is done we can add back the original colors.
          // It's a hack but ist seems to work.
          this->setMaskTypeToBinary();
          this->setMaskBitsToValueBySize(0, W, H, L);
          this->setPropertyColor(0, normalColorTF);
          this->setPropertyScalarOpacity(0, normalOpacityTF);
          m_maskNeedsColorRemoval = false;
        }
        else
        {
          //---------------------------
          // No need to remove colors.
          this->setMaskTypeToBinary();
          this->setMaskBitsToValueBySize(0, W, H, L);
          this->setPropertyColor(0, normalColorTF);
          this->setPropertyScalarOpacity(0, normalOpacityTF);
          m_maskNeedsColorRemoval = false;
        }
      }
    }

    // 1-1:
    if ((m_threatBMPOnly) &&  (m_colorizeThreat))
    {
      if (pBMP)
      {
        m_maskNeedsColorRemoval = true;
        this->setMaskTypeToBinary();
        this->setMaskBitsToValueBySize(0, W, H, L);
        this->setPropertyColor(0, threatColorTF);
        this->setPropertyScalarOpacity(0, threatOpacityTF);
      }
      else
      {
        // For OPERATOR threats revert to:
        // BitmapOnly-ColorizeThreat [Truth Table]: 0-1
        this->setMaskTypeToLabel();
        this->setMaskBitsToValueBySize(0, W, H, L);
        this->setPropertyColor(0, normalColorTF);
        this->setPropertyColor(1, threatColorTF);
        this->setPropertyScalarOpacity(0, normalOpacityTF);
        this->setPropertyScalarOpacity(1, threatOpacityTF);
        m_maskNeedsColorRemoval = false;
      }
    }
  }
  else
  {
    this->setMaskTypeToLabel();
    this->setMaskBitsToValueBySize(0, W, H, L);
    this->setPropertyColor(0, normalColorTF);
    this->setPropertyScalarOpacity(0, normalOpacityTF);
    this->setPropertyColor(1, normalColorTF);
    this->setPropertyScalarOpacity(1, normalOpacityTF);
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::increaseOpacityOnThinSlice(std::list<OpacitySigVal>& normalOpacityList,
    std::list<OpacitySigVal>& thinOpacityList)
{
  std::list<OpacitySigVal>::iterator itOp;

  for (itOp = normalOpacityList.begin(); itOp != normalOpacityList.end(); itOp++)
  {
    if (m_slabbingthickness > THIN_SLAB_OPACITY_TRANSISTION_THICKNESS)
    {
      OpacitySigVal ov = *itOp;
      thinOpacityList.push_back(ov);
    }
    else
    {
      OpacitySigVal ov = *itOp;;
      double sigVal = ov.m_signalVal;
       if( (sigVal  >= m_transtitonMap[SIG_ORGANIC_MIN] ) &&
           (sigVal  <= m_transtitonMap[SIG_METAL_MAX]   )
         )
       {
         ov.m_opacity = THIN_SLAB_OPACITY_SET_VALUE;
       }
      thinOpacityList.push_back(ov);
    }
  }
}


//------------------------------------------------------------------------------
void VolumeRenderer::getSlabOpacityList(std::list<OpacitySigVal>& normalOpacityList,
                                        std::list<OpacitySigVal>& slabOpacityList)
{
  this->increaseOpacityOnThinSlice(normalOpacityList,
                                   slabOpacityList);
}

//------------------------------------------------------------------------------
void VolumeRenderer::directMapThreat(int W, int H, int L,
                     QVector3D P0,
                     SDICOS::Bitmap* pBMP,
                     bool  isLaptop,
                     std::list<HSVColorSigVal>& normalHsvList,
                     std::list<OpacitySigVal>& normalOpacityList,
                     std::list<HSVColorSigVal>& threatHsvList,
                     std::list<OpacitySigVal>& threatOpacityList)
{
  this->removeCenteringSphereIfPresent();
  this->removeMeasuredVolume();
  this->removeMeasuredText();


  //--------------------------------------------------------------
  //  Special case of THIN SLABS
  std::list<OpacitySigVal> applyNormalOpacityhList;
  std::list<OpacitySigVal> applyThreatOpacityhList;
  this->increaseOpacityOnThinSlice(normalOpacityList, applyNormalOpacityhList);
  this->increaseOpacityOnThinSlice(threatOpacityList, applyThreatOpacityhList);
  // std::cout << "%%%%%%%%%%%%% SLAB THICKNESS:"<< m_slabbingthickness << std::endl;
  //--------------------------------------------------------------

  vtkSmartPointer<vtkColorTransferFunction> normalColorTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  vtkSmartPointer<vtkPiecewiseFunction>  normalOpacityTF =
      vtkSmartPointer<vtkPiecewiseFunction>::New();

  this->initHSVColorTransferFunction(normalColorTF,
                                     normalHsvList);

  this->initOpacityTransferFunction(normalOpacityTF,
                                    applyNormalOpacityhList);



  vtkSmartPointer<vtkColorTransferFunction> threatColorTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  vtkSmartPointer<vtkPiecewiseFunction>  threatOpacityTF =
      vtkSmartPointer<vtkPiecewiseFunction>::New();



  this->initHSVColorTransferFunction(threatColorTF,
                                     threatHsvList);

  this->initOpacityTransferFunction(threatOpacityTF,
                                    applyThreatOpacityhList);



  if (pBMP && (!isLaptop))
  {
    this->setMaskTypeToLabel();
    this->setMaskBitsToValue(0);
    this->setPropertyColor(0, normalColorTF);
    this->setPropertyColor(1, threatColorTF);
    this->setPropertyScalarOpacity(0, normalOpacityTF);
    this->setPropertyScalarOpacity(1, threatOpacityTF);
    m_maskNeedsColorRemoval = false;
    this->applyDirectRenderMask(W, H, L, P0, pBMP, m_rawClippedImage, m_mask);
  }
  else
  {
    this->removeLabelMaskColors();
    this->setMaskTypeToBinary();
    this->setMaskBitsToValue(1);
    this->setPropertyColor(0, normalColorTF);
    this->setPropertyScalarOpacity(0, normalOpacityTF);
    m_maskNeedsColorRemoval = false;
  }
  m_mask->Modified();
}

//------------------------------------------------------------------------------
void VolumeRenderer::removeLabelMaskColors()
{
  //------------------------------------------------------
  // ARO-Note: 8-29-2019
  // This may be a worka round to a VTK bug - not sure.
  // When the previous Mask type setMaskTypeToBinary()
  // and the color and opacity were THREAT values we
  // need to first remove colors from the property
  // first.
  vtkColorTransferFunction* pTestfunc0 = m_rawProperty->GetRGBTransferFunction(0);
  vtkColorTransferFunction* pTestfunc1 = m_rawProperty->GetRGBTransferFunction(1);
  if (pTestfunc0)  pTestfunc0->RemoveAllPoints();
  if (pTestfunc1)  pTestfunc1->RemoveAllPoints();
}

//------------------------------------------------------------------------------
void VolumeRenderer::setUpForBagSlabbing()
{
  this->showSlabCube();
  if (m_maskIsClipped)
  {
    this->reMapFullVolume();
  }

  //--------------------------
  // Volume Measurement Removal
  this->removeMeasuredVolume();
  this->removeMeasuredText();
  this->removeCenteringSphereIfPresent();
  //---------------------------

  this->resetMask();
  // this->setMaskTypeToBinary();
  // this->setMaskBitsToValue(1);
  this->setMaskTypeToLabel();
  this->setMaskBitsToValue(0);

  m_mask->Modified();

  this->resetClipIndex();
}

//------------------------------------------------------------------------------
void VolumeRenderer::resetFromSiblingThreat()
{
  vtkSmartPointer<vtkColorTransferFunction> resetColorTF =
      vtkSmartPointer<vtkColorTransferFunction>::New();

  vtkSmartPointer<vtkPiecewiseFunction>  resetOpacityTF =
      vtkSmartPointer<vtkPiecewiseFunction>::New();



  this->initHSVColorTransferFunction(resetColorTF,
                             m_hsvColorList);

  this->initOpacityTransferFunction(resetOpacityTF,
                           m_opasityList);

  this->setPropertyColor(1, resetColorTF);

  m_rawProperty->SetAmbient(m_ambientLighting);
  m_rawProperty->SetDiffuse(m_diffuseLighting);
  m_rawProperty->SetSpecular(m_specularLighting);

  this->setPropertyScalarOpacity(0, resetOpacityTF);
  this->setPropertyColor(0, resetColorTF);

  if (m_maskIsClipped)
  {
    this->reMapFullVolume();
  }

  this->resetMask();

  this->setMaskTypeToBinary();
  this->setMaskBitsToValue(1);

  //--------------------------
  // Volume Measurement Removal
  this->removeMeasuredVolume();
  this->removeMeasuredText();
  this->removeCenteringSphereIfPresent();
  //---------------------------

  m_mask->Modified();
}


//------------------------------------------------------------------------------
bool VolumeRenderer::threatMaskSubVolume(int W,
                                         int H,
                                         int L,
                                         QVector3D p0,
                                         SDICOS::Bitmap* pBMP,
                                         vtkImageData* rawClippedImage,
                                         uint16_t threatMapIndex,
                                         vtkImageData* volumeMask,
                                         bool removeROI)
{
  // Set current threat offset
  m_currThreatOffset = threatMapIndex;

  double originX = p0.x();
  double originY = p0.y();
  double originZ = p0.z();

  //----------------------------------------------
  // ARO-ADDED check to bound origin index
  if (originX < 0) originX = 0;
  if (originY < 0) originY = 0;
  if (originZ < 0) originZ = 0;
  //----------------------------------------------

  int* imageDims = rawClippedImage->GetDimensions();
  int dimX = imageDims[0];
  int dimY = imageDims[1];

  // Get address to set mask bits corresponding to current threat in bag volume mask
  unsigned char* ptrMainLowerCornerMask =
      static_cast< unsigned char* >(volumeMask->GetScalarPointer( originX, originY, originZ  ));


  // Get address of location corresponding to threat lower corner in Main Bag volume
  uint16_t* ptrMainLowerCorner =
      static_cast< uint16_t* >(rawClippedImage->GetScalarPointer(originX,
                                                                 originY,
                                                                 originZ));

  if(!ptrMainLowerCorner)
  {
    QString nullMessage = "m_rawClippedImage->GetScalarPointer(originX, originY, originZ) ";
    nullMessage += "returns nullptr  in initFromThreatData! P1[originX, originY, originZ] (";
    nullMessage += QString::number(originX)  + ", "  +
        QString::number(originY)  + ", "  +
        QString::number(originZ)  + ")";
    LOG(INFO) << nullMessage.toLocal8Bit().data();
    qDebug()  << nullMessage;
    return false;
  }

  bool clearAlllOfLaptop = removeROI;
  //---------------------------------------
  // Itterate over threat to mask voxel appropriately.
  //---------------------------------------
  // ARO-SPEEDUP-FIX
  QTime msMaskTimer = QTime::currentTime();
#ifdef MASK_AND_MULTI_CALL_SPEEDUP
  // std::cout << "YES_DICOS_SPEEDUP" << std::endl;
  for (int k = 0; k < L; k++ )
  {
    for (int j = 0; j < H; j++ )
    {
      for(int i = 0; i < W;  i++)
      {
#else
  // std::cout << "NO_DICOS_SPEEDUP" << std::endl;
  for(int i = 0; i < W;  i++)
  {
    for (int j = 0; j < H; j++ )
    {
      for (int k = 0; k < L; k++ )
      {
#endif
        int bagOffset    = i + (j *dimX) + (k*dimX*dimY);

        // CLEAR ALL OF LAPTOPS
        if (clearAlllOfLaptop)
        {
          if (threatMapIndex !=0 )
          {
            if (!pBMP)
            {
              if (removeROI)
              {
                ptrMainLowerCornerMask[bagOffset]       = (uint8_t)0;
              }
              else
              {
                ptrMainLowerCornerMask[bagOffset]       = (uint8_t)0;
              }
            }
            else
            {
              bool bval =   pBMP->GetBit(i, j, k);

              if (removeROI)
              {
                ptrMainLowerCornerMask[bagOffset]      = (uint8_t)0;
              }
              else
              {
                if (bval)
                {
                  ptrMainLowerCornerMask[bagOffset]       = (uint8_t)1;
                }
                else
                {
                  ptrMainLowerCornerMask[bagOffset]       = (uint8_t)0;
                }
              }
            }
          }
        }
        else
        {
          // CLEAR LAPTOPS BY BMP
          if (threatMapIndex !=0 )
          {
            if(!pBMP || pBMP->GetBit(i, j, k))
            {
              if (removeROI)
              {
                ptrMainLowerCornerMask[bagOffset]      = (uint8_t)0;
              }
              else
              {
                ptrMainLowerCornerMask[bagOffset]       = (uint8_t)1;
              }
            }
          }
        }
      }
    }
  }
  volumeMask->Modified();
  int maskSetTime = msMaskTimer.elapsed();
  QString maskingDuration = "VolumeRenderer::threatMaskSubVolume() - maks set time (ms): "
                            + QString::number(maskSetTime);
  LOG(INFO) << maskingDuration.toStdString();

  // std::cout << maskingDuration.toStdString()
  //           << std::endl;
  return true;
}

//------------------------------------------------------------------------------
bool VolumeRenderer::maskAllMachineThreats(ThreatList *threats,
                                           std::list<ThretOffsetProperty>& propertyList)
{
  if (m_maskApplied) return true;
  // if (!threats->isMachineThreatsEnabled()) return false;
  // std::cout << "%%%%%%%% - VolumeRenderer::maskAllMachineThreats() - :"
  //           << this->displayName().toStdString()
  //           << std::endl;
  bool retVal = true;
  int* imageDims = m_rawClippedImage->GetDimensions();
  int dimX = imageDims[0];
  int dimY = imageDims[1];
  std::list<ThreatObject*> machineThreaObjects;

  QTime multiMaskTheatTimer = QTime::currentTime();

  for (int i = 0; i < threats->size(); i++)
  {
    ThreatObject* pThr =     threats->getThreatAtIndex(i);
    if ( pThr )
    {
      if ( (!pThr->isLapTop()) &&
           ( pThr->getState() != ThreatObject::cleared) &&
           ( pThr->getGenType() != analogic::workstation::OPERATOR_GEN)
         )
       {
          machineThreaObjects.push_back(pThr);
       }
    }
  }

  std::list<ThreatObject*>::iterator mt_iter;
  for (mt_iter = machineThreaObjects.begin(); mt_iter != machineThreaObjects.end();
       mt_iter++)
  {
    ThreatObject* pThr = *mt_iter;

    int threatMapoffset = pThr->getOffset();

    int vtkIndex  = this->vtkLabelIndexFromThreatOffset(threatMapoffset,
                                                        propertyList);

    ThreatVolume tv = pThr->getVolume();
    SDICOS::Bitmap* pBMP = pThr->getBMP();

    if (!pBMP)
    {
      LOG(INFO) << "DICOS BMP is NULLL for threat:" << pThr->getID().toStdString();
      // std::cout << "DICOS BMP is NULLL for threat:" << pThr->getID().toStdString()
      //           << " MAP-OFFSET: " << threatMapoffset
      //           << std::endl;
    }

    double originX = tv.p0().x();
    double originY = tv.p0().y();
    double originZ = tv.p0().z();

    int W = tv.p1().x() - tv.p0().x();
    int H = tv.p1().y() - tv.p0().y();
    int L = tv.p1().z() - tv.p0().z();

    //----------------------------------------------
    // ARO-ADDED check to bound origin index
    if (originX < 0) originX = 0;
    if (originY < 0) originY = 0;
    if (originZ < 0) originZ = 0;
    //----------------------------------------------


    // Get address to set mask bits corresponding to current threat in bag volume mask
    unsigned char* ptrMainLowerCornerMask =
        static_cast< unsigned char* >(m_mask->GetScalarPointer( originX, originY, originZ  ));

    //------------------------------
    // ARO: ANSS-1498
    if (!ptrMainLowerCornerMask)
    {
      LOG(INFO) << "** ERROR! The threat mask pointer ptrMainLowerCornerMask  is NULL.";
      return false;
    }
    //------------------------------

    // Get address of location corresponding to threat lower corner in Main Bag volume
    uint16_t* ptrMainLowerCorner =
        static_cast< uint16_t* >(m_rawClippedImage->GetScalarPointer(originX,
                                                                   originY,
                                                                   originZ));

    if(!ptrMainLowerCorner)
    {
      QString nullMessage = "m_rawClippedImage->GetScalarPointer(originX, originY, originZ) ";
      nullMessage += "returns nullptr  in initFromThreatData! P1[originX, originY, originZ] (";
      nullMessage += QString::number(originX)  + ", "  +
          QString::number(originY)  + ", "  +
          QString::number(originZ)  + ")";
      LOG(INFO) << nullMessage.toLocal8Bit().data();
      qDebug()  << nullMessage;
      retVal = false;
    }

    //---------------------------------------------------------------
    // if (this->displayName().contains("Left"))
    // {
    //   std::cout << "Viewer: MASK-THREAT: "
    //             << this->displayName().toStdString() << std::endl;
    //   std::cout << "Threat OFSET :"
    //             << threatMapoffset << std::endl;
    //   std::cout << "Threat ID :"
    //             << pThr->getID().toStdString() << std::endl;
    //   std::cout << "m_threatOffsetMap[SIG_THREAT_SET_A]: "
    //             << offsetMap[SIG_THREAT_SET_A] << std::endl;
    //   std::cout << "m_threatOffsetMap[SIG_THREAT_SET_B]: "
    //             << offsetMap[SIG_THREAT_SET_B] << std::endl;
    // }
    //---------------------------------------------------------------

    //---------------------------------------
    // Itterate over threat to mask voxel appropriately.
    //---------------------------------------
#ifdef MASK_AND_MULTI_CALL_SPEEDUP
    // std::cout << "YES_DICOS_SPEEDUP" << std::endl;
    for (int k = 0; k < L; k++ )
    {
      for (int j = 0; j < H; j++ )
      {
        for(int i = 0; i < W;  i++)
        {
#else
    // std::cout << "NO_DICOS_SPEEDUP" << std::endl;
    for(int i = 0; i < W;  i++)
    {
      for (int j = 0; j < H; j++ )
      {
        for (int k = 0; k < L; k++ )
       {
#endif
          int bagOffset    = i + (j *dimX) + (k*dimX*dimY);
          if (threatMapoffset !=0 )
          {
            if( pBMP->GetBit(i, j, k))
            {
              ptrMainLowerCornerMask[bagOffset]  = (uint8_t)vtkIndex;
            }
          }
        }
      }
    }
  }

  int msMultiMaskTime = multiMaskTheatTimer.elapsed();
  QString multi_mask_timeLog = "Left viewer multi threat mask time = " +
          QString::number(msMultiMaskTime);
  LOG (INFO)  << multi_mask_timeLog.toStdString();

  // std::cout << multi_mask_timeLog.toStdString()
  //           << std::endl;

  m_mask->Modified();
  m_maskApplied = true;
  return retVal;
}

//------------------------------------------------------------------------------
void VolumeRenderer::applyOperatorRenderMask(int W, int H, int L,
                                     QVector3D p0,
                                     vtkImageData* rawClippedImage,
                                     vtkImageData* volumeMask)
{
  double originX = p0.x();
  double originY = p0.y();
  double originZ = p0.z();


  if (originX < 0) originX = 0;
  if (originY < 0) originY = 0;
  if (originZ < 0) originZ = 0;

  // Get address to set mask bits corresponding to current threat in bag volume mask
  unsigned char* ptrMainLowerCornerMask =
      static_cast< unsigned char* >(volumeMask->GetScalarPointer( originX, originY, originZ  ));
  if (!ptrMainLowerCornerMask) return;

  int* imageDims = rawClippedImage->GetDimensions();
  int dimX = imageDims[0];
  int dimY = imageDims[1];

  //---------------------------------------
  // Mask the apropriate bits.
  //---------------------------------------
  for (int i = 0; i < W; i++ )
  {
    for (int j = 0; j < H; j++ )
    {
      for(int k = 0; k < L;  k++)
      {
        int bagOffset    = i + (j *dimX) + (k*dimX*dimY);
        ptrMainLowerCornerMask[bagOffset]       = (uint8_t)1;
      }
    }
  }
  volumeMask->Modified();
}

//------------------------------------------------------------------------------
void VolumeRenderer::applyDirectRenderMask(int W, int H, int L,
                                     QVector3D p0,
                                     SDICOS::Bitmap* pBMP,
                                     vtkImageData* rawClippedImage,
                                     vtkImageData* volumeMask)
{
  double originX = p0.x();
  double originY = p0.y();
  double originZ = p0.z();


  if (originX < 0) originX = 0;
  if (originY < 0) originY = 0;
  if (originZ < 0) originZ = 0;

  // Get address to set mask bits corresponding to current threat in bag volume mask
  unsigned char* ptrMainLowerCornerMask =
      static_cast< unsigned char* >(volumeMask->GetScalarPointer( originX, originY, originZ  ));
  if (!ptrMainLowerCornerMask) return;

  int bmpSize = 0;

  if (pBMP)
  {
    bmpSize = pBMP->GetSize();
    if (bmpSize ==0)
    {
      LOG(INFO) << "%%%% applyOperatorRenderMask() Threat BMP Size = 0!" << std::endl;
    }
  }

  int* imageDims = rawClippedImage->GetDimensions();
  int dimX = imageDims[0];
  int dimY = imageDims[1];

  //---------------------------------------
  // Mask the apropriate bits.
  //---------------------------------------

  QTime directRenderMaskTimer = QTime::currentTime();
#ifdef MASK_AND_MULTI_CALL_SPEEDUP
  // std::cout << "YES_DICOS_SPEEDUP" << std::endl;
  for (int k = 0; k < L; k++ )
  {
    for (int j = 0; j < H; j++ )
    {
      for(int i = 0; i < W;  i++)
      {
#else
  std::cout << "NO_DICOS_SPEEDUP" << std::endl;
  for(int i = 0; i < W;  i++)
  {
    for (int j = 0; j < H; j++ )
    {
      for (int k = 0; k < L; k++ )
      {
#endif
        int bagOffset    = i + (j *dimX) + (k*dimX*dimY);

        if (bmpSize == 0)
        {
          // Operator thereat
          ptrMainLowerCornerMask[bagOffset]       = (uint8_t)1;
        }
        else
        {
          //-----------------------------------------------------
          // NORMAL AND COLORED THREAT
          if( pBMP->GetBit(i, j, k) )
          {
            ptrMainLowerCornerMask[bagOffset]   =  (uint8_t)1;
          }
          else
          {
            ptrMainLowerCornerMask[bagOffset]   = (uint8_t)0;
          }
        }
      }
    }
  }
  volumeMask->Modified();
  int ms_directMaskInterval = directRenderMaskTimer.elapsed();
  QString directMaskLogInfo = "VolumeRenderer::applyDirectRenderMask() - interval (ms) ="
                              + QString::number(ms_directMaskInterval);
  LOG(INFO)  << directMaskLogInfo.toStdString();
  // std::cout << directMaskLogInfo.toStdString()
  //           << std::endl;
}


//------------------------------------------------------------------------------
void VolumeRenderer::applyCroppedRenderMask(int W,
                                            int H,
                                            int L,
                                            SDICOS::Bitmap* pBMP,
                                            vtkImageData* volumeMask)
{
  // std::cout << "%%%%%% applyCroppedRenderMask() on clipped sub-volume %%%%%%" << std::endl;
  // Since the mapped volume data is obtained from a clipped subsection of a bag,
  // the ofset PO is:
  //   QVector3D(0, 0, 0)
  //-----------------------------------------

  if (g_dbg_call_count == 0)
  {
    m_initRenderTimer = QTime::currentTime();
    // std::cout << "*** ARO-SPEEDUP TESTING start timer" << std::endl;
  }
  //================================
  // this->showBitMapDensity(pBMP);
  //================================
  g_dbg_call_count++;
  if (!m_allowThreatMasking) return;
  unsigned char* ptrMainLowerCornerMask =
      static_cast< unsigned char* >(volumeMask->GetScalarPointer(0, 0, 0));
  if (!ptrMainLowerCornerMask) return;
  if (!pBMP) return;

  int dimX = W;
  int dimY = H;

  //---------------------------------------
  // Set the approriate bits
  //---------------------------------------
  // ARO-SPEEDUP-FIX
  QTime timerCroppedMask = QTime::currentTime();
#ifdef MASK_AND_MULTI_CALL_SPEEDUP
  // std::cout << "YES_DICOS_SPEEDUP" << std::endl;
  for (int k = 0; k < L; k++ )
  {
    for (int j = 0; j < H; j++ )
    {
      for(int i = 0; i < W;  i++)
      {
#else
  // std::cout << "NO_DICOS_SPEEDUP" << std::endl;
  for(int i = 0; i < W;  i++)
  {
    for (int j = 0; j < H; j++ )
    {
      for (int k = 0; k < L; k++ )
      {
#endif
        int bagOffset    = i + (j *dimX) + (k*dimX*dimY);
        //-----------------------------------------------------
        // NORMAL AND COLORED THREAT
        if( pBMP->GetBit(i, j, k) )
        {
          ptrMainLowerCornerMask[bagOffset]   =  (uint8_t)1;
        }
        else
        {
          ptrMainLowerCornerMask[bagOffset]   = (uint8_t)0;
        }
      }
    }
  }
  volumeMask->Modified();
  int croppedMaskElapsesTime = timerCroppedMask.elapsed();
  QString croppedMaskTimer = "VolumeRenderer::applyCroppedRenderMask() -  timer  (ms) = " +
      QString::number(croppedMaskElapsesTime);
  LOG(INFO) << croppedMaskTimer.toStdString();
  // std::cout << croppedMaskTimer.toStdString() << std::endl;
  int fullMultiCallTime = m_initRenderTimer.elapsed();
  QString multiCallTimer = "Multi-call Timer (ms) = "
          + QString::number(fullMultiCallTime);
  LOG(INFO) << multiCallTimer.toStdString();
  // std::cout << multiCallTimer.toStdString() << std::endl;
}

//------------------------------------------------------------------------------
void VolumeRenderer::outputVolumeMeasurementResult(QString method,
                                                   int computeTime,
                                                   double computedVolume,
                                                   int W,
                                                   int H,
                                                   int L)
{
  double px, py, pz = 1.0;
  double dW = W;
  double dH = H;
  double dL = L;
  this->getPixelSpacing(px, py, pz);
  double boxVol = dW*dH*dL*px*py*pz*CUBIC_CM_PER_CUBIC_MM;
  QString timingMessage =  "%>>><<<% End Measure Volume by: ";
  timingMessage += method;
  timingMessage += " - Elapsed(ms): " + QString::number(computeTime);
  timingMessage += ", Threat BOX VOL (CM^3): "   + QString::number(boxVol);
  timingMessage += ", Computed VOL (CM^3): "     + QString::number(computedVolume);
  QString volMessage = "%>>><<<% Threat BOX Dimensions(mm) [W,H,L]: ";
  volMessage += QString::number(W) + " ,"
             +  QString::number(H) + ", "
             +  QString::number(L);
  LOG(INFO) << timingMessage.toStdString();
  LOG(INFO) << volMessage.toStdString();
  emit this->volumeMeasureComplete();
}

//------------------------------------------------------------------------------
void VolumeRenderer::measureVolumeByValueFlyingEdge(QVector3D threatP0,
                                           int W,
                                           int H,
                                           int L)
{
  QTime timeVolCompute = QTime::currentTime();
  QString timingMessage =   "%>>><<<% Begin Measure Volume by Value ...";
  LOG(INFO) << timingMessage.toStdString();

  vtkRenderer*    renderer =  m_renderWindowRef->GetRenderers()->GetFirstRenderer();
  if (!renderer) return;


  int dims[6];
  m_rawClippedImage->GetDimensions(dims);

  uint16_t valPick = m_meashuredVolumeStartPoint;

  vtkSmartPointer<vtkImageClip> imageClip = vtkSmartPointer<vtkImageClip>::New();

  int minx = threatP0.x();
  int maxx = threatP0.x() +  W;

  int miny = threatP0.y();
  int maxy = threatP0.y() + H;

  int minz = threatP0.z();
  int maxz = threatP0.z() +  L;

  imageClip->SetInputData(this->getImageData());
  imageClip->SetOutputWholeExtent(minx,
                                  maxx,
                                  miny,
                                  maxy,
                                  minz,
                                  maxz);
  imageClip->Update();

  vtkSmartPointer<vtkFlyingEdges3D> flyingEdges = vtkSmartPointer<vtkFlyingEdges3D>::New();
  flyingEdges->SetInputConnection(imageClip->GetOutputPort());
  flyingEdges->ComputeGradientsOff();
  flyingEdges->ComputeScalarsOff();
  flyingEdges->ComputeNormalsOff();
  flyingEdges->SetValue(5, valPick);


  vtkSmartPointer<vtkPolyDataConnectivityFilter> confilter =
      vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
  confilter->SetInputConnection(flyingEdges->GetOutputPort());
  confilter->SetExtractionModeToLargestRegion();


  vtkSmartPointer<vtkTransform> flipTransform =
      vtkSmartPointer<vtkTransform>::New();
  flipTransform->SetMatrix(m_flipYMatrix);

  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
      vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputConnection(confilter->GetOutputPort());
  transformFilter->SetTransform(flipTransform);
  transformFilter->Update();

  vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter =
      vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  smoothFilter->AddInputConnection(transformFilter->GetOutputPort());
  smoothFilter->BoundarySmoothingOn();
  smoothFilter->Update();

  vtkSmartPointer<vtkMassProperties> massProperties =
      vtkSmartPointer<vtkMassProperties>::New();

  massProperties->SetInputConnection(smoothFilter->GetOutputPort());
  massProperties->Update();


  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(smoothFilter->GetOutputPort());
  mapper->ScalarVisibilityOn();

  this->removeMeasuredVolume();

  // Volume Actor
  m_measuredVolumeActor = vtkSmartPointer<vtkActor>::New();
  m_measuredVolumeActor->GetProperty()->SetDiffuseColor(VOL_MEASURE_DIFFUSE_RED,
                                                        VOL_MEASURE_DIFFUSE_GREEN,
                                                        VOL_MEASURE_DIFFUSE_BLUE);

  m_measuredVolumeActor->GetProperty()->SetSpecularColor(VOL_MEASURE_SPECULAR_RED,
                                                         VOL_MEASURE_SPECULAR_GREEN,
                                                         VOL_MEASURE_SPECULAR_BLUE);

  m_measuredVolumeActor->GetProperty()->SetSpecularPower(VOL_MEASURE_SPECULAR_POWER);
  m_measuredVolumeActor->GetProperty()->SetSpecular(VOL_MEASURE_SPECULARITY);
  m_measuredVolumeActor->SetMapper(mapper);

  renderer->AddActor(m_measuredVolumeActor);

  //-----------------------------------------------
  // Measured Volume Text Value
  double mappPropVol_mm3  = massProperties->GetVolume();
  double mappPropVol_in3  = mappPropVol_mm3*CUBIC_INCH_PER_CUBIC_MM;
  double mappPropVol_cc   = mappPropVol_mm3*CUBIC_CM_PER_CUBIC_MM;

  std::string textVal;
  if (m_diplayUnit == VR_DISPLAY_CM)
  {
    textVal = QObject::tr("Vol (ml): ").toStdString() +
              to_string_with_precision(mappPropVol_cc,
              VOLUME_MEASUREMENT_PRECISION);
  }
  else
  {
    textVal = QObject::tr("Vol (in^3): ").toStdString() +
              to_string_with_precision(mappPropVol_in3,
              VOLUME_MEASUREMENT_PRECISION);
  }
  this->setMeasuredVolumeText(textVal);
  //-----------------------------------------------

  int compTime = timeVolCompute.elapsed();
  this->outputVolumeMeasurementResult("[FYING EDGES]",
                                      compTime,
                                      mappPropVol_cc,
                                      W,
                                      H,
                                      L);
}

//------------------------------------------------------------------------------
void VolumeRenderer::measureVolumeByValueMarchingCube(QVector3D threatP0,
                                           int W,
                                           int H,
                                           int L)
{
  QTime timeVolCompute = QTime::currentTime();
  QString timingMessage =   "%>>><<<% Begin Measure Volume by Value ...";
  LOG(INFO) << timingMessage.toStdString();

  vtkRenderer*    renderer =  m_renderWindowRef->GetRenderers()->GetFirstRenderer();
  if (!renderer) return;


  int dims[6];
  m_rawClippedImage->GetDimensions(dims);

  uint16_t valPick = m_meashuredVolumeStartPoint;

  vtkSmartPointer<vtkImageClip> imageClip = vtkSmartPointer<vtkImageClip>::New();

  int minx = threatP0.x();
  int maxx = threatP0.x() +  W;

  int miny = threatP0.y();
  int maxy = threatP0.y() + H;

  int minz = threatP0.z();
  int maxz = threatP0.z() +  L;

  imageClip->SetInputData(this->getImageData());
  imageClip->SetOutputWholeExtent(minx,
                                  maxx,
                                  miny,
                                  maxy,
                                  minz,
                                  maxz);
  imageClip->Update();

  vtkSmartPointer<vtkMarchingCubes> surface =
      vtkSmartPointer<vtkMarchingCubes>::New();
  surface->SetInputConnection(imageClip->GetOutputPort());
  surface->ComputeGradientsOff();
  surface->ComputeNormalsOff();
  surface->ComputeScalarsOff();

  surface->SetValue(0,  // index
                    valPick);
  surface->SetNumberOfContours(5);

  surface->Update();

  vtkSmartPointer<vtkTransform> flipTransform =
      vtkSmartPointer<vtkTransform>::New();
  flipTransform->SetMatrix(m_flipYMatrix);


  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
      vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputConnection(surface->GetOutputPort());
  transformFilter->SetTransform(flipTransform);
  transformFilter->Update();


  vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter =
      vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  smoothFilter->SetInputConnection(transformFilter->GetOutputPort());
  smoothFilter->SetNumberOfIterations(VOL_MEASURE_LAPLACIAN_SMOOTH_ITT);
  smoothFilter->BoundarySmoothingOn();
  smoothFilter->SetRelaxationFactor(VOL_MEASURE_RELAX_FACTOR);
  smoothFilter->Update();

  vtkSmartPointer<vtkMassProperties> massProperties =
      vtkSmartPointer<vtkMassProperties>::New();

  massProperties->SetInputConnection(smoothFilter->GetOutputPort());
  massProperties->Update();


  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(smoothFilter->GetOutputPort());
  mapper->ScalarVisibilityOn();
  // mapper->SetColorModeToMapScalars();
  mapper->Update();

  this->removeMeasuredVolume();

  // Volume Actor
  m_measuredVolumeActor = vtkSmartPointer<vtkActor>::New();
  m_measuredVolumeActor->GetProperty()->SetDiffuseColor(VOL_MEASURE_DIFFUSE_RED,
                                                        VOL_MEASURE_DIFFUSE_GREEN,
                                                        VOL_MEASURE_DIFFUSE_BLUE);

  m_measuredVolumeActor->GetProperty()->SetSpecularColor(VOL_MEASURE_SPECULAR_RED,
                                                         VOL_MEASURE_SPECULAR_GREEN,
                                                         VOL_MEASURE_SPECULAR_BLUE);

  m_measuredVolumeActor->GetProperty()->SetSpecularPower(VOL_MEASURE_SPECULAR_POWER);
  m_measuredVolumeActor->GetProperty()->SetSpecular(VOL_MEASURE_SPECULARITY);
  m_measuredVolumeActor->SetMapper(mapper);

  renderer->AddActor(m_measuredVolumeActor);

  //-----------------------------------------------
  // Measured Volume Text Value
  double mappPropVol_mm3  = massProperties->GetVolume();
  double mappPropVol_in3  = mappPropVol_mm3*CUBIC_INCH_PER_CUBIC_MM;
  double mappPropVol_cc   = mappPropVol_mm3*CUBIC_CM_PER_CUBIC_MM;

  std::string textVal;
  if (m_diplayUnit == VR_DISPLAY_CM)
  {
    textVal = QObject::tr("Vol (ml): ").toStdString() +
              to_string_with_precision(mappPropVol_cc,
              VOLUME_MEASUREMENT_PRECISION);
  }
  else
  {
    textVal = QObject::tr("Vol (in^3): ").toStdString() +
              to_string_with_precision(mappPropVol_in3,
              VOLUME_MEASUREMENT_PRECISION);
  }
  this->setMeasuredVolumeText(textVal);
  //-----------------------------------------------

  int compTime = timeVolCompute.elapsed();
  this->outputVolumeMeasurementResult("[MARCHING CUBES]",
                                      compTime,
                                      mappPropVol_cc,
                                      W,
                                      H,
                                      L);
}

//------------------------------------------------------------------------------
void VolumeRenderer::measureVolumeByDICOSBitmap(QVector3D threatP0,
                                                int W, int H, int L, SDICOS::Bitmap* pBMP)
{
  QTime timeVolCompute = QTime::currentTime();
  QString timingMessage =   "%>>><<<% Begin Measure Volume by DICOS BMP ...";
  LOG(INFO) << timingMessage.toStdString();

  vtkRenderer*    renderer =  m_renderWindowRef->GetRenderers()->GetFirstRenderer();
  if (!renderer) return;

  vtkSmartPointer<vtkImageData>   dicosMask;
  dicosMask = vtkSmartPointer<vtkImageData>::New();

  int dims[3] = {W, H, L};
  double px, py, pz = 1.0;
  this->getPixelSpacing(px, py, pz);

  double threatOrigin[3] = { threatP0.x()*px, threatP0.y()*py, threatP0.z()*pz };

  dicosMask->SetDimensions(dims);
  dicosMask->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  dicosMask->SetSpacing(m_rawClippedImage->GetSpacing());
  dicosMask->SetOrigin(threatOrigin);
  unsigned char* ptrMainLowerCornerMask = static_cast<unsigned char*>(dicosMask->GetScalarPointer(0, 0, 0));

  unsigned char  clamp_min  = VOL_MEASURE_DICOS_BMP_CLAMP_MIN;
  unsigned char  clamp_max  = VOL_MEASURE_DICOS_BMP_CLAMP_MAX;
  unsigned char  set_val    = VOL_MEASURE_DICOS_BMP_SET_VAL;

  // Set all the bits of mask to 0
  memset(ptrMainLowerCornerMask,  0 , dims[0]*dims[1]*dims[2]*sizeof(unsigned char));

  int dimX = W;
  int dimY = H;

  QTime dicosAccessTimer = QTime::currentTime();
  // Set Mask DICOS BMP bits to 1 where true:
#ifdef MASK_AND_MULTI_CALL_SPEEDUP
  // std::cout << "YES_DICOS_SPEEDUP" << std::endl;
  for (int k = 0; k < L; k++ )
  {
    for (int j = 0; j < H; j++ )
    {
      for(int i = 0; i < W;  i++)
      {
#else
  // std::cout << "NO_DICOS_SPEEDUP" << std::endl;
  for(int i = 0; i < W;  i++)
  {
    for (int j = 0; j < H; j++ )
    {
      for (int k = 0; k < L; k++ )
      {
#endif
        int bagOffset     = i + (j *dimX) + (k*dimX*dimY);
        bool bval         =   pBMP->GetBit(i, j, k);
        // uint16_t imageVal = ptrClippedImageBase[bagOffset];
        if (bval)
        {
          ptrMainLowerCornerMask[bagOffset]       = (unsigned char)set_val;
        }
      }
    }
  }
  int ms_dicosAccess = dicosAccessTimer.elapsed();
  QString dicosAccessLog = "VolumeRenderer::measureVolumeByDICOSBitmap() set mask access timer (ms) = "
      + QString::number(ms_dicosAccess);
  LOG(INFO) << dicosAccessLog.toStdString();
  std::cout << dicosAccessLog.toStdString() << std::endl;

  vtkSmartPointer<vtkImageThreshold> clamper =
      vtkSmartPointer<vtkImageThreshold>::New();
  clamper->SetInputData(dicosMask);
  clamper->ThresholdBetween(clamp_min,
                            clamp_max);
  clamper->ReplaceOutOn();
  clamper->SetOutValue(clamp_max);
  clamper->Update();

  vtkSmartPointer<vtkImageDataGeometryFilter> polyConvert
     = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
  polyConvert->SetInputConnection(clamper->GetOutputPort());

   //------------------------------------------------------
   // ARO-DEBUG:
   // vtkSmartPointer<vtkTransform> identityMatrix =
   //   vtkSmartPointer<vtkTransform>::New();
   // identityMatrix->Identity();
   //------------------------------------------------------

   vtkSmartPointer<vtkTransform> flipTransform =
      vtkSmartPointer<vtkTransform>::New();
   flipTransform->SetMatrix(m_flipYMatrix);


  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
      vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputConnection(polyConvert->GetOutputPort());
  transformFilter->SetTransform(flipTransform);
  transformFilter->Update();

  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(transformFilter->GetOutputPort());
  mapper->SetColorModeToMapScalars();
  mapper->ScalarVisibilityOn();
  mapper->Update();

  vtkSmartPointer<vtkLookupTable> myLookup = vtkSmartPointer<vtkLookupTable>::New();

  // Set up an HSV color map for pixels in DICOS BITMAP that are "ON".
  myLookup->SetTableRange(VOL_MEASURE_DICOS_BMP_CLAMP_MIN,
                          VOL_MEASURE_DICOS_BMP_CLAMP_MAX);

  myLookup->SetHueRange(VOL_MEASURE_DICOS_BMP_HSV_HUE_MIN,
                        VOL_MEASURE_DICOS_BMP_HSV_HUE_MAX);

  myLookup->SetSaturationRange(VOL_MEASURE_DICOS_BMP_HSV_SAT_MIN,
                               VOL_MEASURE_DICOS_BMP_HSV_SAT_MAX);

  myLookup->SetValueRange(VOL_MEASURE_DICOS_BMP_HSV_VAL_MIN,
                          VOL_MEASURE_DICOS_BMP_HSV_VAL_MAX);

  myLookup->Build();

  // Set  mapper lookupo table to the newly created table
  mapper->SetLookupTable(myLookup);

  int tableSize = myLookup->GetNumberOfAvailableColors();

  double tableval[4];

  for (int i=0; i < tableSize; i++)
  {
    myLookup->GetTableValue(i, tableval);
    // ARO-NOTE:
    // double  R = tableval[0];  - Red
    // double  G = tableval[1];  - Green
    // double  B = tableval[2];  - Blue
    // double  A = tableval[3];  - Alpha
    if (i < tableSize/2)
    {
       // Set Alpha to transparent for values below clamp.
       tableval[3] = 0;
       myLookup->SetTableValue(i, tableval);
    }
  }
  this->removeMeasuredVolume();

  // Volume Actor
  m_measuredVolumeActor = vtkSmartPointer<vtkActor>::New();
  m_measuredVolumeActor->GetProperty()->SetPointSize(VOL_MEASURE_DICOS_BMP_POINT_SZE);
  m_measuredVolumeActor->SetMapper(mapper);

  renderer->AddActor(m_measuredVolumeActor);

  //-----------------------------------------------
  // Measured Volume Text Value
  double mappPropVol_mm3  = this->getBMPixelVolumeNew(pBMP);
  double mappPropVol_in3  = mappPropVol_mm3*CUBIC_INCH_PER_CUBIC_MM;
  double mappPropVol_cc   = mappPropVol_mm3*CUBIC_CM_PER_CUBIC_MM;

  std::string textVal;
  if (m_diplayUnit == VR_DISPLAY_CM)
  {
    textVal = QObject::tr("Vol (ml): ").toStdString() +
              to_string_with_precision(mappPropVol_cc,
              VOLUME_MEASUREMENT_PRECISION);
  }
  else
  {
    textVal = QObject::tr("Vol (in^3): ").toStdString() +
              to_string_with_precision(mappPropVol_in3,
              VOLUME_MEASUREMENT_PRECISION);
  }
  this->setMeasuredVolumeText(textVal);
  //-----------------------------------------------

  int compTime = timeVolCompute.elapsed();
  this->outputVolumeMeasurementResult("[SDICOS BITMAP]",
                                      compTime,
                                      mappPropVol_cc,
                                      W,
                                      H,
                                      L);
}

//------------------------------------------------------------------------------
void VolumeRenderer::removeMeasuredVolume()
{
  vtkRenderer*    renderer =  m_renderWindowRef->GetRenderers()->GetFirstRenderer();
  if (!renderer) return;
  if (m_measuredVolumeActor)
  {
    renderer->RemoveActor(m_measuredVolumeActor);
    m_measuredVolumeActor = nullptr;
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::removeMeasuredText()
{
  std::string clearText = "";
  this->setMeasuredVolumeText(clearText);
}


//------------------------------------------------------------------------------
int VolumeRenderer::getRenderBuildTime()
{
  return m_initialBuildTime_ms;
}

//------------------------------------------------------------------------------
void VolumeRenderer::setRenderMutex(QMutex* aMutex)
{
  m_renderMutex = aMutex;
}

//------------------------------------------------------------------------------
// Set Actual bounds for volume
void VolumeRenderer::setActualBounds(vtkSmartPointer<vtkVolume> volume)
{
  double* boundsPointer = volume->GetBounds();
  if (boundsPointer)
  {
    for (int i = 0; i < 6; i++)
    {
      m_actual_bounds[i] = boundsPointer[i];
    }
  }
}


//------------------------------------------------------------------------------
void VolumeRenderer::mapClippedSubVolume(QVector3D P0,
                                         int W,
                                         int H,
                                         int L,
                                         bool doClamp)
{
  int clip_x_min = P0.x();
  int clip_x_max = P0.x() + W;

  int clip_y_min = P0.y();
  int clip_y_max = P0.y() + H;


  int clip_z_min = P0.z();
  int clip_z_max = P0.z() + L;

  QTime threatClipTime = QTime::currentTime();

  vtkSmartPointer<vtkImageClip> threatClip =
      vtkSmartPointer<vtkImageClip>::New();
  threatClip->SetInputData(m_rawClippedImage);
  threatClip->SetOutputWholeExtent(clip_x_min,
                                   clip_x_max,
                                   clip_y_min,
                                   clip_y_max,
                                   clip_z_min,
                                   clip_z_max);
  threatClip->ClipDataOn();
  if (doClamp)
  {
    //--------------------------------------------------------
    // Clamp threat data for PI threats
    uint16_t clamp_min = 0;
    uint16_t clamp_max = PI_CLAMP_MAX;

    vtkSmartPointer<vtkImageThreshold> clamper =
      vtkSmartPointer<vtkImageThreshold>::New();
    clamper->SetInputConnection(threatClip->GetOutputPort());
    clamper->ThresholdBetween(clamp_min,
                            clamp_max);
    clamper->ReplaceOutOn();
    clamper->SetOutValue(clamp_max);
    clamper->Update();

    int msClip = threatClipTime.elapsed();
    QString message = "*TTTTTT* - THREAT CLAMP CLIP time (ms):" + QString::number(msClip);
    LOG(INFO) << message.toStdString();
    // std::cout << message.toStdString()
    //          << std::endl;
    //--------------------------------------------------------
    m_rawMapper->SetInputConnection(clamper->GetOutputPort());
  }
  else
  {
    m_rawMapper->SetInputConnection(threatClip->GetOutputPort());
  }

  m_rawMapper->Update();
  m_maskIsClipped = true;
}

//------------------------------------------------------------------------------
void VolumeRenderer::reMapFullVolume()
{
  QTime remapTime = QTime::currentTime();
  int   timeInterval_ms = 0;
  QString remapTimeStr = "";
  int dims[3];
  m_rawClippedImage->GetDimensions(dims);
  m_mask->SetDimensions(dims);
  m_mask->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  m_mask->SetSpacing(m_rawClippedImage->GetSpacing());
  m_mask->SetOrigin(m_rawClippedImage->GetOrigin());
  this->setMaskBitsToValue(0);
  m_rawMapper->SetInputData(m_rawClippedImage);
  m_maskIsClipped = false;
  this->resetClipIndex();

  // Process time info
  timeInterval_ms = remapTime.elapsed();
  remapTimeStr = "#### REMAP - Full Volumne - RayCast Mapper nterval (ms): " + QString::number(timeInterval_ms);
  LOG(INFO) << remapTimeStr.toStdString();
  // std::cout << remapTimeStr.toStdString() << std::endl;
}


//------------------------------------------------------------------------------
// Create Image mask for 3D volume
void VolumeRenderer::createAndSetMask()
{
  m_mask = vtkSmartPointer<vtkImageData>::New();
  int dims[3];
  m_rawClippedImage->GetDimensions(dims);
  m_mask->SetDimensions(dims);
  m_mask->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  m_mask->SetSpacing(m_rawClippedImage->GetSpacing());
  m_mask->SetOrigin(m_rawClippedImage->GetOrigin());

  this->setMaskBitsToValue(0);

  this->setMaskTypeToLabel();
}


//------------------------------------------------------------------------------
void VolumeRenderer::setMaskBitsToValue(uint8_t val)
{
  if (!m_mask) return;

  int dims[3];
  m_mask->GetDimensions(dims);
  unsigned char* base = static_cast<unsigned char*>(m_mask->GetScalarPointer(0, 0, 0));

  // Set all the bits of mask to 0
  memset(base, val, dims[0]*dims[1]*dims[2]*sizeof(char));
}


//------------------------------------------------------------------------------
// Reset mask for 3D volume
void VolumeRenderer::resetMask()
{
  if(m_mask)
  {
    int dims[3];
    m_mask->GetDimensions(dims);
    unsigned char* base = static_cast<unsigned char*>(m_mask->GetScalarPointer(0, 0, 0));
    // Set all the bits of mask to 0
    memset(base, 0, dims[0]*dims[1]*dims[2]*sizeof(char));
  }
  m_maskApplied = false;
}

//------------------------------------------------------------------------------
void VolumeRenderer::resetMaskWithResize(int imageSizeX,
                                         int imageSizeY,
                                         int imageSizeZ)
{
  int dims[] = {imageSizeX, imageSizeY, imageSizeZ };
  m_mask->SetDimensions(dims);
  m_mask->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  m_mask->SetSpacing(m_rawClippedImage->GetSpacing());
  m_mask->SetOrigin(m_rawClippedImage->GetOrigin());
  this->setMaskBitsToValueBySize(0,
                                 imageSizeX,
                                 imageSizeY,
                                 imageSizeZ);
  this->setMaskTypeToLabel();
}

//------------------------------------------------------------------------------
void VolumeRenderer::setMaskBitsToValueBySize(uint8_t val,
                                              int sizeX,
                                              int sizeY,
                                              int sizeZ)
{
  if (!m_mask) return;
  unsigned char* base = static_cast<unsigned char*>(m_mask->GetScalarPointer(0, 0, 0));
  // Set all the bits of mask to 0
  memset(base, val, sizeX*sizeY*sizeZ*sizeof(char));
}



//------------------------------------------------------------------------------
void VolumeRenderer::resetAndUpdateMask()
{
  // std::cout << "&&&&&&&&&&&&& resetAndUpdateMask() &&&&&&&&&&&&&: " <<
  // this->displayName().toStdString() << std::endl;
  this->resetMask();
  m_mask->Modified();
}

//------------------------------------------------------------------------------
void VolumeRenderer::setMaskTypeToLabel()
{
  // std::cout << "Setting Mask Type top LABEL: "
  //           << this->displayName().toStdString()
  //           << std::endl;
  vtkGPUVolumeRayCastMapper* rayCastMapper = vtkGPUVolumeRayCastMapper::SafeDownCast(m_rawMapper);
  if(rayCastMapper)
  {
    rayCastMapper->SetMaskBlendFactor(1);
    rayCastMapper->SetMaskInput(m_mask);
    rayCastMapper->SetMaskTypeToLabelMap();
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::setMaskTypeToBinary()
{
  // std::cout << "Setting Mask Type top BINARY: "
  //           << this->displayName().toStdString()
  //           << std::endl;
  vtkGPUVolumeRayCastMapper* rayCastMapper = vtkGPUVolumeRayCastMapper::SafeDownCast(m_rawMapper);
  if(rayCastMapper)
  {
    rayCastMapper->SetMaskInput(m_mask);
    rayCastMapper->SetMaskTypeToBinary();
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::setRemoveLaptop(bool doRemove)
{
  m_doReomveLaptop = doRemove;
  this->resetMask();
  m_threats->setLaptopViewMode(doRemove);
  if (!m_doReomveLaptop)
  {
    this->setMaskTypeToLabel();
  }
  m_mask->Modified();
}

//------------------------------------------------------------------------------
vtkImageData* VolumeRenderer::getImageData()
{
  return m_rawClippedImage;
}

//------------------------------------------------------------------------------
uint16_t VolumeRenderer::getImageValueAt(QVector3D imagePoint)
{
  vtkImageData*  pRawImage = this->getImageData();
  if (!pRawImage) return -1;
  // Set up the list of point to examine.  And average about imagePoint
  // of N in size. (N = 0 to 4).
  // Range = {magePoimt.x() - W  to imagePoint.x()  + W } in x
  //         {magePoimt.y() - W  to imagePoint.y()  + w } in y
  //         {magePoimt.z() - W  to imagePoint.y()  + w } in z

  int    N   = VOL_MEASURE_MARCH_CUBE_AVG_WINDOW;
  double dR  = 1.0;
  double x0  = imagePoint.x() - N*dR;
  double y0  = imagePoint.y() - N*dR;
  double z0  = imagePoint.z() - N*dR;

  double x1  = imagePoint.x() + N*dR;
  double y1  = imagePoint.y() + N*dR;
  double z1  = imagePoint.z() + N*dR;

  double dx = (x1 - x0)/(2.0*N);
  double dy = (y1 - y0)/(2.0*N);
  double dz = (z1 - z0)/(2.0*N);

  int Nx = 2*N;
  int Ny = 2*N;
  int Nz = 2*N;

  // std::cout << "Start Point [x, y, z]: "
  //          << imagePoint.x()  << ", "
  //          << imagePoint.y()  << ", "
  //          << imagePoint.z()
  //         << std::endl;
  std::list<QVector3D>  pickPoints;
  for (int i = 0; i < Nx; i++)
  {
    double di = i;
    double x = x0 +  di*dx;
    for (int j = 0; j < Ny; j++)
    {
      double dj = j;
      double y = y0 + dj*dy;
      for (int k = 0; k < Nz; k++)
      {
        double dk = k;
        double z = z0 + dk*dz;
        QVector3D aPick(x, y, z);
        pickPoints.push_back(aPick);
      }
    }
  }
  // std::cout << "Size Search Window [N]:"  << pickPoints.size()
  //          << std::endl;
  bool havePoint  = false;
  double sum_Val  = 0;
  double numPoint = 0;

  std::list<QVector3D>::iterator vit;
  for (vit = pickPoints.begin(); vit !=  pickPoints.end(); vit++)
  {
      QVector3D listP = *vit;
      // Get the Scalar Pointer of the vtkImageData at the location passed in at imagePoint.
      uint16_t* ptrImageData =
      static_cast< uint16_t* >(pRawImage->GetScalarPointer(listP.x(),
                                                           listP.y(),
                                                           listP.z()));
      if (ptrImageData)
      {
        havePoint = true;
        sum_Val  += ptrImageData[0];
        numPoint++;
        // std::cout << "Valid Point [x,y,z]: "
        //          << listP.x() << ", "
        //          << listP.y() << ", "
        //          << listP.z() << std::endl;
      }
  }
  if(!havePoint)
  {
    QString nullMessage = "m_rawClippedImage->GetScalarPointer(x, y, z) ";
    nullMessage += "returns nullptr !(";
    nullMessage += QString::number(imagePoint.x())  + ", "  +
        QString::number(imagePoint.y())  + ", "  +
        QString::number(imagePoint.z())  + ")";
    LOG(INFO) << nullMessage.toLocal8Bit().data();
    qDebug()  << nullMessage;
    return -1;
  }
  double avgVal = sum_Val/numPoint;
  return static_cast<uint16_t>(avgVal);
}

//------------------------------------------------------------------------------
void VolumeRenderer::clearvolumeRegion(QVector3D p0,
                                           int W,
                                           int H,
                                           int L)
{
  //=========================================
  double originX = p0.x();
  double originY = p0.y();
  double originZ = p0.z();
  //----------------------------------------------
  // ARO-ADDED check to bound origin index
  if (originX < 0) originX = 0;
  if (originY < 0) originY = 0;
  if (originZ < 0) originZ = 0;
  //----------------------------------------------

  if (!m_rawClippedImage) return;

  int* imageDims = m_rawClippedImage->GetDimensions();
  int dimX = imageDims[0];
  int dimY = imageDims[1];


  // Get address of location corresponding to threat lower corner in Main Bag volume
  uint16_t* ptrMainLowerCorner =
      static_cast< uint16_t* >(m_rawClippedImage->GetScalarPointer(originX,
                                                           originY,
                                                           originZ));
  if(!ptrMainLowerCorner)
  {
    QString nullMessage = "m_rawClippedImage->GetScalarPointer(originX, originY, originZ) ";
    nullMessage += "returns nullptr  in initFromThreatData! P1[originX, originY, originZ] (";
    nullMessage += QString::number(originX)  + ", "  +
        QString::number(originY)  + ", "  +
        QString::number(originZ)  + ")";
    LOG(INFO) << nullMessage.toLocal8Bit().data();
    qDebug()  << nullMessage;
    return;
  }

  //---------------------------------------
  // Fill Threat renderer Volume with Data
  //---------------------------------------
  for (int i = 0; i < W; i++ )
  {
    for (int j = 0; j < H; j++ )
    {
      for(int k = 0; k < L;  k++)
      {
        int bagOffset    = i + (j *dimX) + (k*dimX*dimY);

        ptrMainLowerCorner[bagOffset] = 0;
      }
    }
  }
}


//------------------------------------------------------------------------------
// Unmask all bits of Volume
void VolumeRenderer::UnmaskVolume(vtkImageData* image)
{
  int dims[3];
  if(NULL != m_rawClippedImage)
  {
    m_rawClippedImage->GetDimensions(dims);
  }
  if(NULL != image)
  {
    unsigned char* base = static_cast<unsigned char*>(image->GetScalarPointer(0, 0, 0));
    for(int z = 0; z < dims[2]; z++)
    {
      for(int y = 0; y < dims[1]; y++)
      {
        for(int x = 0; x < dims[0]; x++)
        {
          unsigned char* pixel = (base + z * (dims[1] * dims[0] ) + y * (dims[0]) + x);
          pixel[0] = 255;
        }
      }
    }

    image->Modified();
  }
}


//------------------------------------------------------------------------------
void VolumeRenderer::ApplyBoundingCube(double *bounds)
{
  // std::cout << "%%%%%%%%%%% ApplyBoundingCube() %%%%%%%%%%%%%%%%%" << std::endl;
  // Create a cube that will be used as a clip box for both UNPACKING and SLABBING.
  // Create clip box once andd apply cube as needed.
  m_clipCube->SetBounds(bounds);
  if (!m_clip_box)
  {
    vtkSmartPointer<vtkDataSetMapper> mapper =
        vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputConnection(m_clipCube->GetOutputPort());

    m_clip_box = vtkSmartPointer<vtkActor>::New();
    m_clip_box->SetMapper(mapper);
    this->showSlabCube();

    // Add the "SLAB" clip box actor to the scene.
    m_rendererRef->AddActor(m_clip_box);
  }
  else
  {
    m_clip_box->GetMapper()->SetInputConnection(m_clipCube->GetOutputPort());
  }
}


//------------------------------------------------------------------------------
void VolumeRenderer::setSlabbing(bool flag, double thickness)
{
  this->setEnableSlabbing(flag);
  m_slabbingthickness = thickness;
}

//------------------------------------------------------------------------------
void VolumeRenderer::setEnableSlabbing(bool flag)
{
  m_slabbing = flag;
}


//------------------------------------------------------------------------------
void VolumeRenderer::hideSlabCube()
{
  if(!m_clip_box) return;
  m_clip_box->GetProperty()->EdgeVisibilityOff();
  m_clip_box->GetProperty()->SetOpacity(SLAB_HIDE_OPACITY);
}

//------------------------------------------------------------------------------
void VolumeRenderer::showSlabCube()
{
  if(!m_clip_box) return;
  m_clip_box->GetProperty()->EdgeVisibilityOn();
  m_clip_box->GetProperty()->SetOpacity(SLAB_SHOW_OPACITY);
}


//----------------------------------------------------------------------------
void VolumeRenderer::resetClippingPlanes()
{
  if (m_rawMapper)
  {
    m_rawMapper->RemoveAllClippingPlanes();
    m_rawMapper->Modified();
  }
}

//------------------------------------------------------------------------------
void VolumeRenderer::setSlabPosition(double slabPosition,
                                            VREnums::AXIS axis)
{
  double slabBounds[6];
  this->computeSlabBoundingBoxForBag(slabPosition,
                                     axis,
                                     slabBounds);

  // ARO-NOTE: If this is a Threat Window, should we do something else??
  //

  m_slabUnpackPlanes->SetBounds(slabBounds[1], slabBounds[0],
                                slabBounds[3], slabBounds[2],
                                slabBounds[5], slabBounds[4]);

  //----------------------------------------------------
  // The clipping is only applied to the RIGHT viewer!
  if(!this->displayName().contains("Left"))
  {
    if (m_rawMapper)
    {
      m_rawMapper->SetClippingPlanes(m_slabUnpackPlanes);
    }
  }
  this->ApplyBoundingCube(slabBounds);
  m_rawMapper->Modified();
}

//-------------------------------------------------------------------------------
void VolumeRenderer::setUnPackingPosition(double unpackPosition,
                                          VREnums::AXIS axis)
{
  double unpackBounds[6];
  this->computeUnpackingBondingBoxForBag(unpackPosition,
                                         axis,
                                         unpackBounds);

  // Set bounds for the clipping plane to clip the rendered volume
  m_slabUnpackPlanes->SetBounds(unpackBounds[1], unpackBounds[0],
                                unpackBounds[3], unpackBounds[2],
                                unpackBounds[5], unpackBounds[4]);

  if (m_rawMapper)
  {
    m_rawMapper->SetClippingPlanes(m_slabUnpackPlanes);
    m_rawMapper->Modified();
  }
}

//------------------------------------------------------------------------------
double VolumeRenderer::getSlabUnpackPositionFromPoint(QVector3D pCent,
                                                      VREnums::AXIS axis)
{
  double bounds[6];
  for (int i = 0; i < 6; i++)
  {
    bounds[i] = m_actual_bounds[i];
  }
  QVector3D boxLength((bounds[1] - bounds[0]),
                      (bounds[3] - bounds[2]),
                      (bounds[5] - bounds[4]));
  double slabOffset = 0;

  //---------------------------
  // X: Needs testing
  if (axis == VREnums::X)
  {
    slabOffset = 1 - pCent.x()/boxLength.x();
  }

  //---------------------------
  // Y: OK
  if (axis == VREnums::Y)
  {
    slabOffset = pCent.y()/boxLength.y();
  }

  //---------------------------
  // Z Needs Testiung
  if (axis == VREnums::Z)
  {
    slabOffset = 1- pCent.z()/boxLength.z();
  }
  return slabOffset;
}

//----------------------------------------------------------------------------
void VolumeRenderer::computeSlabBoundingBoxForBag(double slabPosition,
                                                  VREnums::AXIS axis,
                                                  double* boundingBox)
{
  double bounds[6];
  for (int i = 0; i < 6; i++)
  {
    bounds[i] = m_actual_bounds[i];
  }
  // Calculate length of box in X, Y and Z direction
  double c_length[3];
  c_length[0] = bounds[1] - bounds[0];
  c_length[1] = bounds[3] - bounds[2];
  c_length[2] = bounds[5] - bounds[4];

  double slabOffset = 1.0 - slabPosition;

  // Find bounds to be set for clipping planes
  if ((axis == VREnums::X))
  {
    double total_len = c_length[0];
    double target_length = total_len * slabOffset;
    double start_length = target_length - m_slabbingthickness/2;
    double end_length = target_length + m_slabbingthickness/2;
    bounds[1] = bounds[0] + end_length;
    bounds[0] = bounds[0] + start_length;

    // The value bounds[1] is slightly modified to prevent vtk
    // from rendering entire volume when bounds[1] = bounds[0]
    if(bounds[1] == bounds[0])
    {
      if(bounds[1] > 0.0)
      {
        bounds[1]-= 0.001;
      }
      else
      {
        bounds[1]+= 0.001;
      }
    }
  }
  else if ((axis == VREnums::Y))
  {
    double total_len = c_length[1];
    double target_length = total_len * slabOffset;
    double start_length = target_length - m_slabbingthickness/2;
    double end_length = target_length + m_slabbingthickness/2;
    bounds[3] = bounds[2] + end_length;
    bounds[2] = bounds[2] + start_length;

    // The value bounds[3] is slightly modified to prevent vtk
    // from rendering entire volume wsetSlabOrCutPositionhen bounds[3]=bounds[2]
    if(bounds[3] == bounds[2])
    {
      if(bounds[3] > 0.0)
      {
        bounds[3]-= 0.001;
      }
      else
      {
        bounds[3]+= 0.001;
      }
    }
  }
  else if ((axis == VREnums::Z))
  {
    double total_len = c_length[2];
    double target_length = total_len * slabOffset;
    double start_length = target_length - m_slabbingthickness/2;
    double end_length = target_length + m_slabbingthickness/2;
    bounds[5] = bounds[4] + end_length;
    bounds[4] = bounds[4] + start_length;

    // The value bounds[5] is slightly modified to prevent vtk
    // from rendering entire volume when bounds[5]=bounds[4]
    if(bounds[5] == bounds[4])
    {
      if(bounds[5] > 0.0)
      {
        bounds[5]-= 0.001;
      }
      else
      {
        bounds[5]+= 0.001;
      }
    }
  }
  else
  {
    LOG(INFO) << "Cutview volume data is not correct.Axis:" << axis << "Percentage:" << slabOffset;
    LOG(INFO) << "volume bounds: xmin:" << bounds[0]
              << " xmax:" << bounds[1]
              << " ymin:" << bounds[2]
              << " ymax:" << bounds[3]
              << " zmin:" << bounds[4]
              << " zmax:" << bounds[5];
    return;
  }
  for (int i = 0; i < 6; i++)
  {
    boundingBox[i] = bounds[i];
  }
}

//-----------------------------------------------------------------------------
void VolumeRenderer::computeUnpackingBondingBoxForBag(double unpackPosition,
                                   VREnums::AXIS axis,
                                   double* unpackBounds)
{
  double bounds[6];
  for (int i = 0; i < 6; i++)
  {
    bounds[i] = m_actual_bounds[i];
  }

  // Calculate length of box in X, Y and Z direction

  double c_length[3];
  c_length[0] = bounds[1] - bounds[0];
  c_length[1] = bounds[3] - bounds[2];
  c_length[2] = bounds[5] - bounds[4];

  // Find bounds to be set for clipping planes
  if ((axis == VREnums::X) && (bounds[1] - c_length[0]*unpackPosition) >= bounds[0])
  {
    bounds[1] = (bounds[1] - c_length[0]*unpackPosition);

    // The value bounds[1] is slightly modified to prevent vtk
    // from rendering entire volume when bounds[1] = bounds[0]
    if(bounds[1] == bounds[0])
    {
      if(bounds[1] > 0.0)
      {
        bounds[1]-= 0.001;
      }
      else
      {
        bounds[1]+= 0.001;
      }
    }
  }
  else if ((axis == VREnums::Y) && (bounds[3] - c_length[1]*unpackPosition) >= bounds[2])
  {
    bounds[3]=(bounds[3]-c_length[1]*unpackPosition);

    // The value bounds[3] is slightly modified to prevent vtk
    // from rendering entire volume when bounds[3]=bounds[2]
    if(bounds[3] == bounds[2])
    {
      if(bounds[3] > 0.0)
      {
        bounds[3]-= 0.001;
      }
      else
      {
        bounds[3]+= 0.001;
      }
    }
  }
  else if ((axis == VREnums::Z) && (bounds[5] - c_length[2]*unpackPosition) >= bounds[4])
  {
    bounds[5]=(bounds[5]-c_length[2]*unpackPosition);


    // The value bounds[5] is slightly modified to prevent vtk
    // from rendering entire volume when bounds[5]=bounds[4]
    if(bounds[5] == bounds[4])
    {
      if(bounds[5] > 0.0)
      {
        bounds[5]-= 0.001;
      }
      else
      {
        bounds[5]+= 0.001;
      }
    }
  }
  else
  {
    LOG(INFO) << "Cutview volume data is not correct.Axis:" << axis << "Percentage:" << unpackPosition;
    LOG(INFO) << "volume bounds: xmin:" << bounds[0]
              << " xmax:" << bounds[1]
              << " ymin:" << bounds[2]
              << " ymax:" << bounds[3]
              << " zmin:" << bounds[4]
              << " zmax:" << bounds[5];
    return;
  }
  for (int i = 0;  i < 6; i++)
  {
    unpackBounds[i]  = bounds[i];
  }
}
