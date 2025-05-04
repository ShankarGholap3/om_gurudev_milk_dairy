//------------------------------------------------------------------------------
// File:  VolumeRendererBase.cpp
// Description: Implementation of Volume Renderer base class
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "VolumeRendererBase.h"

const double g_threatTFSharpness   = 1.0;
const double g_threatTFMidopoint   = 1.0;


double g_UserPickZoomInScale     = 2.25;
double g_UserPickZoomOutScale    = 1.75;


double g_pickBox_x0 = 0;
double g_pickBox_x1 = 50;

double g_pickBox_y0 = 0;
double g_pickBox_y1 = 50;

double g_pickBox_z0 = 0;
double g_pickBox_z1 = 50;


//--------------------------------------------------------------
// Matrix axis elements used for flipping in Y all Actors.
//--------------------------------------------------------------
static const double g_flipMatrixAxialElements[16] =
{
    1,  0, 0,  0,
    0, -1, 0,  0,
    0,  0, 1,  0,
    0,  0, 0,  1
};

//-------------------------------------------------


//------------------------------------------------------------------------------
VolumeRendererBase::VolumeRendererBase(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow,
                                       vtkSmartPointer<vtkRenderer> renderer,
                         double pixelSpacingX,
                         double pixelSpacingY,
                         double pixelSpacingZ):
 m_sphereActor(nullptr),
 m_volumeSizeBytes(0),
 m_keptImageData(nullptr),
 m_currentClippedThreatIndex(-1),
 m_maskIsClipped(false),
 m_displayName(""),
 m_renderWindowRef(renderWindow.Get()),
 m_rendererRef(renderer.Get()),
 m_subVolumeBoxWidget(nullptr),
 m_rawMapper(nullptr),
 m_rawProperty(nullptr),
 m_rawVolume(nullptr),
 m_viewLabel(nullptr),
 m_diplayUnit(VR_DISPLAY_CM),
 m_volumeComputeStartMin(0),
 m_volumeComputeStartMax(1),
 m_pixelSpacingX(pixelSpacingX),
 m_pixelSpacingY(pixelSpacingY),
 m_pixelSpacingZ(pixelSpacingZ),
 m_doColorizeOrganic(true),
 m_doColorizeInorganic(true),
 m_doColorizeMetal(true),
 m_doColorizeThreat(true),
 m_threatBMPOnly(false),
 m_maskNeedsColorRemoval(false),
 m_doInverseColor(false),
 m_opacitySampleDistance(DEFAULT_OPACITY_SAMPLE_DISTANCE),
 m_interactiveSampleDistance(DEFAULT_INTERACTIVE_SAMPLE_DISTANCE),
 m_autoAdjustSampleDistance(false),
 m_useShader(true),
 m_linearInterpolation(true),
 m_renderWithColor(true),
 m_organicRemovable(false),
 m_inorganicRemovable(false),
 m_metalRemovable(false),
 m_useGPU(true),
 m_inverseBackground(false),
 m_lowDensityActive(false),
 m_backgroundRed(VOLUME_BG_DEFAULT_RED),
 m_backgroundGreen(VOLUME_BG_DEFAULT_GREEN),
 m_backgroundBlue(VOLUME_BG_DEFAULT_BLUE),
 m_opacityModifyScalar(1.0),
 m_contrastModifyScalar(1.0),
 m_brightnessModifyScalar(4.0),
 m_ambientLighting(0.1),
 m_diffuseLighting(0.7),
 m_specularLighting(0.2),
 m_flipYMatrix(nullptr),
 m_currThreatOffset(0),
 m_materialDisplayCutoff(0),
 m_colorizeThreat(true),
 m_mask(nullptr),
 m_maskApplied(false),
 m_isBrightnessModified(false)
{
  m_imageDataBounds[0] = 0.0;
  m_imageDataBounds[1] = 1.0;
  m_imageDataBounds[2] = 0.0;
  m_imageDataBounds[3] = 1.0;
  m_imageDataBounds[4] = 0.0;
  m_imageDataBounds[5] = 1.0;
  this->setupYFlipMatrix();
}

//------------------------------------------------------------------------------
VolumeRendererBase::~VolumeRendererBase()
{
    this->removeVolume();
    // Clear the maps
    m_threatOffsetHsvMap.clear();
    m_threatOffsetOpacityMap.clear();
}

int VolumeRendererBase::s_initTime             = 0;
int VolumeRendererBase::s_numThreats           = 0;
double VolumeRendererBase::s_startThreatVolume = 0;
double VolumeRendererBase::s_fullVolume         = 0;


//------------------------------------------------------------------------------
void VolumeRendererBase::setInitTime(int msInit)
{
  s_initTime = msInit;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::addInitTime(int msInit)
{
  s_initTime += msInit;
}

//------------------------------------------------------------------------------
int  VolumeRendererBase::getInitTime()
{
  return s_initTime;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setInitThreatInfo(int numThreats,
                                           double startThreatVolume,
                                           double fullVolunme)
{
  s_numThreats         = numThreats;
  s_startThreatVolume  = startThreatVolume;
  s_fullVolume         = fullVolunme;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::getInitThreatInfo(int& numThreats,
                                           double& startThreatVolume,
                                           double& fullVolunme)
{
  numThreats        = s_numThreats;
  startThreatVolume = s_startThreatVolume;
  fullVolunme       = s_fullVolume;
}



//------------------------------------------------------------------------------
void VolumeRendererBase::setDisplayName(QString& aName)
{
  m_displayName = aName;
}


//------------------------------------------------------------------------------
QString VolumeRendererBase::displayName()
{
  return m_displayName;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setDisplayUnitToCentimeter()
{
  m_diplayUnit = VR_DISPLAY_CM;
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setDisplayUnitToInch()
{
  m_diplayUnit = VR_DISPLAY_INCH;
}


//------------------------------------------------------------------------------
double VolumeRendererBase::getVolumeSizeMB()
{
  int w     = m_imageDataBounds[1] - m_imageDataBounds[0];
  int h     = m_imageDataBounds[3] - m_imageDataBounds[2];
  int l     = m_imageDataBounds[5] - m_imageDataBounds[4];
  double VOL   = w*h*l*sizeof(uint16_t);
  double volMB = VOL/(1024.0*1024.0);
  return volMB;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setVolumeComputeStartRange(uint16_t startMin,
                                                    uint16_t startMax)
{
  m_volumeComputeStartMin = startMin;
  m_volumeComputeStartMax = startMax;
}


//-----------------------------------------------------------------------------
double VolumeRendererBase::getVolumeMMCubed()
{
  double w = m_imageDataBounds[1] - m_imageDataBounds[0];
  double h = m_imageDataBounds[3] - m_imageDataBounds[2];
  double l = m_imageDataBounds[5] - m_imageDataBounds[4];
  double volSize = w*h*l*m_pixelSpacingX*m_pixelSpacingY*m_pixelSpacingZ;

  return volSize;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::getBounds(double* bounds)
{
  for (int i = 0; i < 6; i++)
  {
    bounds[i] = m_imageDataBounds[i];
  }
}


//------------------------------------------------------------------------------'
void VolumeRendererBase::getVolumeBounds(double*  volBounds)
{
  if (m_rawVolume)
  {
    m_rawVolume->GetBounds(volBounds);
  }
  else
  {
    for (int i = 0; i < 6; i++)
    {
      volBounds[i] = 0;
    }
  }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setBounds(double* bounds)
{
  // std::cout << "----------------------------------------------------" << std::endl;
  // std::cout << "VolumeRendererBase::setBounds() - Setting m_imageDataBound[0..5]" << std::endl;
  for (int i = 0; i < 6; i++)
  {
    m_imageDataBounds[i] = bounds[i];
    // std::cout << "m_imageDataBounds[i]: [" << i << "] : " << m_imageDataBounds[i] << std::endl;
  }
  // std::cout << "----------------------------------------------------" << std::endl;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setupYFlipMatrix()
{
  m_flipYMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  m_flipYMatrix->DeepCopy(g_flipMatrixAxialElements);
}


//------------------------------------------------------------------------------
vtkSmartPointer<vtkMatrix4x4> VolumeRendererBase::getYFlipMatrix()
{
  return m_flipYMatrix;
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setRenderBackground()
{
  m_rendererRef->SetBackground(m_backgroundRed,
                            m_backgroundGreen,
                            m_backgroundBlue);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setRenderWithColor(bool doColor)
{
  m_renderWithColor = doColor;
}

//------------------------------------------------------------------------------
bool VolumeRendererBase::isRenderingWithColor()
{
  return m_renderWithColor;
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setGrayscaleSettings(GrayscaleValueOpacityStruct& grayStruct)
{
  m_grayscaleStruct = grayStruct;
}

//------------------------------------------------------------------------------
GrayscaleValueOpacityStruct& VolumeRendererBase::getGrayScaleSettings()
{
  return m_grayscaleStruct;
}

//------------------------------------------------------------------------------
int VolumeRendererBase::vtkLabelIndexFromThreatOffset(uint16_t threataOffset,
    std::list<ThretOffsetProperty>& properyList)
{
  // Default to zero offset not found.
  int retVal = 0;

  std::list<ThretOffsetProperty>::iterator listIt;
  for ( listIt = properyList.begin();
        listIt != properyList.end();
        listIt++ )
  {
      uint16_t offVal    = listIt->m_threatOffset;
      uint16_t vtk_index = listIt->m_vtkIndex;
      if (offVal == threataOffset)
      {
         retVal = vtk_index;
         break;
      }
  }
  return retVal;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::getPixelSpacing(double& psX,
                     double& psY,
                     double& psZ)
{
  psX = m_pixelSpacingX;
  psY = m_pixelSpacingY;
  psZ = m_pixelSpacingZ;
}

//-----------------------------------------------------------------------------
int VolumeRendererBase::getNumberOfSlices(size_t sizeBuffer, int W, int H)
{
  // Each dada is 2 bytes long
  int nData = sizeBuffer/sizeof(uint16_t);
  int nSlices = nData/(W*H);
  if(nSlices == 1)
  {
    nSlices = 2;
  }
  return nSlices;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setLinearInterpolation(bool doLinear)
{
  m_linearInterpolation = doLinear;
}

//-------------------------------------------------------------------------------
bool VolumeRendererBase::isUsingLinearInterpolation()
{
  return m_linearInterpolation;
}

//-------------------------------------------------------------------------------
bool VolumeRendererBase::isUsingShading()
{
 return m_useShader;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::resetCamera()
{
  m_rendererRef->ResetCamera();
}

//------------------------------------------------------------------------------
void VolumeRendererBase::resetCameraWithBounds(double* bounds)
{
  m_rendererRef->ResetCamera(bounds);
  // std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  // std::cout << "*** Resetting Camera With Bounds ****** " << std::endl;
  // std::cout << "[x0, x1] : " << bounds[0] << "," << bounds[1]  << std::endl;
  // std::cout << "[y0, y1] : " << bounds[2] << "," << bounds[3]  << std::endl;
  // std::cout << "[z0, z1] : " << bounds[4] << "," << bounds[5]  << std::endl;
  // std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  // qDebug() << "***** Resetting Camera With Bounds";
  // qDebug() << "x0,x1: " << bounds[0] << "," << bounds[1];
  // qDebug() << "y0,y1: " << bounds[2] << "," << bounds[3];
  // qDebug() << "z0,z1: " << bounds[4] << "," << bounds[5];
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setViewLabel(ViewLabelWidget* viewLabel)
{
  m_viewLabel = viewLabel;
}

//------------------------------------------------------------------------------
ViewLabelWidget* VolumeRendererBase::getViewLabel()
{
  return m_viewLabel;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::dataBoundsToClipBounds(double* dataBounds,
                                            int* clipBounds)
{
  clipBounds[0] = static_cast<int>(dataBounds[0]/m_pixelSpacingX);
  clipBounds[1] = static_cast<int>(dataBounds[1]/m_pixelSpacingX);

  clipBounds[2] = static_cast<int>(dataBounds[2]/m_pixelSpacingY);
  clipBounds[3] = static_cast<int>(dataBounds[3]/m_pixelSpacingY);

  clipBounds[4] = static_cast<int>(dataBounds[4]/m_pixelSpacingZ);
  clipBounds[5] = static_cast<int>(dataBounds[5]/m_pixelSpacingZ);
}


void VolumeRendererBase::setVolumeInerpolation(vtkSmartPointer<vtkVolumeProperty> property,
                           vtkInterpolationType interpType)
{
  switch(interpType)
  {
  case  INTERPOLATE_NEAREST:
    property->SetInterpolationTypeToNearest();
    break;
  case   INTERPOLATE_LINEAR:
    property->SetInterpolationTypeToLinear();
    break;
  case INTERPOLATE_CUBIC:
    break;
    property->SetInterpolationType(VTK_CUBIC_INTERPOLATION);
  default:
    property->SetInterpolationTypeToNearest();
    break;
  }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setGPUStatus(bool useGPU)
{
  m_useGPU = useGPU;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::removeVolume()
{
   if (m_rawVolume)
   {
      m_rendererRef->RemoveViewProp(m_rawVolume);
      m_rendererRef->Render();
      m_rawProperty = nullptr;
      m_rawMapper   = nullptr;
      m_rawVolume   = nullptr;
      m_keptImageData->ReleaseData();
      m_currentClippedThreatIndex = -1;
   }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::resetClipIndex()
{
  m_currentClippedThreatIndex = -1;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setThreatOffsetPropertyList(std::list<ThretOffsetProperty>& propertyList)
{
  m_threatPropertyList.clear();
  m_threatPropertyList = propertyList;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setShaderStatus(bool shaderStat)
{
  m_useShader = shaderStat;
}

//------------------------------------------------------------------------------
bool VolumeRendererBase::isBgInverse()
{
  return m_inverseBackground;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::resetRenderSettings()
{
  m_isBrightnessModified = false;
  m_currentOpacityList.clear();
  m_currentHSVColorList.clear();
  std::list<OpacitySigVal>::iterator itL;
  if ( (m_opasityList.size() !=0)       )
  {
    for (itL = m_opasityList.begin(); itL !=m_opasityList.end(); itL++)
    {
      OpacitySigVal opI = *itL;
      m_currentOpacityList.push_back(opI);
    }
  }
  std::list<HSVColorSigVal>::iterator itHsv;
  if ( (m_hsvColorList.size() !=0)       )
  {
    for (itHsv = m_hsvColorList.begin(); itHsv !=m_hsvColorList.end(); itHsv++)
    {
      HSVColorSigVal hsvI = *itHsv;
      m_currentHSVColorList.push_back(hsvI);
    }
  }
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setIverseBgValue(bool bgInverseVal)
{
  m_inverseBackground = bgInverseVal;
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setLowDensityActive(bool setVal)
{
   m_lowDensityActive = setVal;
}


//------------------------------------------------------------------------------
bool VolumeRendererBase::isLowDensityActive()
{
  return m_lowDensityActive;
}


 //------------------------------------------------------------------------------
 void VolumeRendererBase::setOrganicRemovable(bool organicRemovable)
 {
  m_organicRemovable =  organicRemovable;
 }

 //------------------------------------------------------------------------------
 void VolumeRendererBase::setInorganicRemovable(bool inorganicRemovable)
 {
  m_inorganicRemovable =  inorganicRemovable;
 }


 //------------------------------------------------------------------------------
 void VolumeRendererBase::setMetalRemovable(bool metalRemovable)
 {
  m_metalRemovable =  metalRemovable;
 }

 //------------------------------------------------------------------------------
 void VolumeRendererBase::removeThreatVisual(vtkActor* pActor)
 {
    m_rendererRef->RemoveActor(pActor);
 }


//------------------------------------------------------------------------------
void VolumeRendererBase::setTransferFunctionValues(std::list<OpacitySigVal>&  opacityList,
    std::list<HSVColorSigVal>& colorList,
    std::map<MaterialTransition_E,
             double>& transitionMap, std::map<double,
             std::list<HSVColorSigVal>>& threatOffsetHsvMap,
             std::map<double, std::list<OpacitySigVal>>&  threatOffsetOpacityMap
)
{
  m_opasityList.clear();
  m_hsvColorList.clear();
  std::list<OpacitySigVal>::iterator  opIt;
  std::list<HSVColorSigVal>::iterator colIt;
  for (opIt = opacityList.begin(); opIt!= opacityList.end(); opIt++)
  {
    OpacitySigVal opVal = *opIt;
    m_opasityList.push_back(opVal);
  }

  for (colIt = colorList.begin(); colIt!= colorList.end(); colIt++)
  {
    HSVColorSigVal colVal = *colIt;
    m_hsvColorList.push_back(colVal);
  }

  std::map<MaterialTransition_E, double>::iterator iter;
  for (iter = transitionMap.begin(); iter != transitionMap.end(); iter++)
  {
    MaterialTransition_E enV = iter->first;
    double               tV  = iter->second;
    m_transtitonMap[enV] = tV;
  }

  m_threatOffsetHsvMap = threatOffsetHsvMap;
  m_threatOffsetOpacityMap = threatOffsetOpacityMap;
  m_materialDisplayCutoff = m_transtitonMap[SIG_ORGANIC_MIN];
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setOCBModifyScalar(double scaleValOpacity,
                                            double scaleValContrast,
                                            double scaleValBrightness)
{
  m_opacityModifyScalar    = scaleValOpacity;
  m_contrastModifyScalar   = scaleValContrast;
  m_brightnessModifyScalar = scaleValBrightness;
}



//------------------------------------------------------------------------------
vtkSmartPointer<vtkActor>
VolumeRendererBase::createThreatVisual(ThreatVolume& aVolume,
                                       double createColorRed,
                                       double createColorGreen,
                                       double createColorBlue,
                                       bool bIsShield)
{
  // Create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  this->generatePolyDataFromPoints(aVolume.p0(),
                                   aVolume.p1(),
                                   polyData,
                                   bIsShield);

  // Flip the points in the Poly Data
  vtkSmartPointer<vtkTransform> flipTransForm = vtkSmartPointer<vtkTransform>::New();
  flipTransForm->SetMatrix(m_flipYMatrix);

  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =    vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  transformFilter->SetInputData(polyData);
  transformFilter->SetTransform(flipTransForm);
  transformFilter->Update();


  // Setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(transformFilter->GetOutputPort());


  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(createColorRed,
                                 createColorGreen,
                                 createColorBlue);

  actor->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY);
  // std::cout << "threatActor->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY); == 05" << std::endl;
  actor->GetProperty()->SetLineWidth(THREAT_BOUNDS_LINE_WIDTH);

  m_rendererRef->AddActor(actor);

  return actor;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::createCenteringSphere(vtkRenderer*  renderer,
                                               QVector3D& center)
{
  vtkSmartPointer<vtkSphereSource> sphereSource =
      vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->SetCenter(center.x(),
                          center.y(),
                          center.z());
  sphereSource->SetRadius(2.0);

  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  m_sphereActor =  vtkSmartPointer<vtkActor>::New();
  m_sphereActor->SetMapper(mapper);
  m_sphereActor->GetProperty()->SetColor(0.7, 0.2, 0.2);


  renderer->AddActor(m_sphereActor);

  m_renderWindowRef->Render();
}


//------------------------------------------------------------------------------
void VolumeRendererBase::removeCenteringSphereIfPresent()
{
  if (m_sphereActor)
  {
    m_rendererRef->RemoveActor(m_sphereActor);
    m_sphereActor = nullptr;
  }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::centerScene(QVector3D rotPoint)
{
  vtkRenderer*    renderer =  m_renderWindowRef->GetRenderers()->GetFirstRenderer();
  if (!renderer) return;

  double displayRotPoint[4] = {0, 0, 0, 0};

  this->computeWorldToDisplay(renderer,
                              rotPoint.x(),
                              rotPoint.y(),
                              rotPoint.z(),
                              displayRotPoint);

  double renderFocus[3] = {0, 0, 0};
  vtkCamera*      camera = renderer->GetActiveCamera();
  camera->SetFocalPoint(rotPoint.x(),
                        rotPoint.y(),
                        rotPoint.z());

  camera->GetFocalPoint(renderFocus);


  double displayFocus[4] = {0, 0, 0, 0};
  this->computeWorldToDisplay(renderer,
                              renderFocus[0],
                              renderFocus[1],
                              renderFocus[2],
                              displayFocus);

  double windowCenter[3] = {0, 0};

  camera->GetWindowCenter(windowCenter);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::panScene(int deltaX, int deltyaY)
{
  int lastEventPosX = 500;
  int lastEventPosY = 500;
  int currentEventPiosX = 500 + deltaX;
  int currentEventPiosY = 500 + deltyaY;
  vtkRenderWindowInteractor* rwi = m_renderWindowRef->GetInteractor();

  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];

  vtkRenderer*    renderer =  m_renderWindowRef->GetRenderers()->GetFirstRenderer();
  vtkCamera*      camera = renderer->GetActiveCamera();
  camera->GetFocalPoint(viewFocus);
  this->computeWorldToDisplay(renderer,
                              viewFocus[0],
                              viewFocus[1],
                              viewFocus[2],
                              viewFocus);
  focalDepth = viewFocus[2];

  this->computeDisplayToWorld(renderer,
                              currentEventPiosX,
                              currentEventPiosY,
                              focalDepth,
                              newPickPoint);

  this->computeDisplayToWorld(renderer,
                              lastEventPosX,
                              lastEventPosY,
                              focalDepth,
                              oldPickPoint);

  motionVector[0] = oldPickPoint[0] - newPickPoint[0];
  motionVector[1] = oldPickPoint[1] - newPickPoint[1];
  motionVector[2] = oldPickPoint[2] - newPickPoint[2];

  camera->GetFocalPoint(viewFocus);
  camera->GetPosition(viewPoint);
  camera->SetFocalPoint(motionVector[0] + viewFocus[0],
      motionVector[1] + viewFocus[1],
      motionVector[2] + viewFocus[2]);

  camera->SetPosition(motionVector[0] + viewPoint[0],
      motionVector[1] + viewPoint[1],
      motionVector[2] + viewPoint[2]);

  if (rwi->GetLightFollowCamera())
  {
    renderer->UpdateLightsGeometryToFollowCamera();
  }
  rwi->Render();
}


//------------------------------------------------------------------------------
void VolumeRendererBase::computeWorldToDisplay(vtkRenderer* ren,
                                         double x,
                                         double y,
                                         double z,
                                         double* displayPt)
{
  ren->SetWorldPoint(x, y, z, 1.0);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(displayPt);
}
//------------------------------------------------------------------------------
void VolumeRendererBase::computeDisplayToWorld(vtkRenderer* ren,
                                         double x,
                                         double y,
                                         double z,
                                         double* worldPt)
{
  ren->SetDisplayPoint(x, y, z);
  ren->DisplayToWorld();
  ren->GetWorldPoint(worldPt);
  if (worldPt[3])
  {
    worldPt[0] /= worldPt[3];
    worldPt[1] /= worldPt[3];
    worldPt[2] /= worldPt[3];
    worldPt[3] = 1.0;
  }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setMapperModified()
{
  if (m_rawMapper)
  {
    m_rawMapper->Modified();
  }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::showColorTransferFunction(vtkColorTransferFunction* colorTF)
{
  std::stringstream ss;
  colorTF->PrintSelf(ss, vtkIndent(2));
  std::string strOut = ss.str();
  std::cout << "**********************  COLOR TRANSFER FUNCTION  **********************" << std::endl;
  std::cout << "%%% Display Name :" << this->displayName().toStdString() << std::endl;
  std::cout << strOut << std::endl;
  std::cout << "***********************************************************************" << std::endl;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::showOpacityTansferFunction(vtkPiecewiseFunction* opacityTF)
{
  std::stringstream ss;
  opacityTF->PrintSelf(ss, vtkIndent(2));
  std::string strOut = ss.str();
  std::cout << "********************** OPACITY TRANSFER FUNCTION **********************" << std::endl;
  std::cout << strOut << std::endl;
  std::cout << "***********************************************************************" << std::endl;
}

//------------------------------------------------------------------------------
std::map<double, std::list<HSVColorSigVal>>& VolumeRendererBase::getThreatOffsetHsvMap()
{
  return m_threatOffsetHsvMap;
}

//------------------------------------------------------------------------------
std::map<double, std::list<OpacitySigVal>>& VolumeRendererBase::getThraOffsetOpacityMap()
{
  return m_threatOffsetOpacityMap;
}

//------------------------------------------------------------------------------
std::list<ThretOffsetProperty>& VolumeRendererBase::getThreatPropertyList()
{
  return m_threatPropertyList;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::generateSafeNormalTransferFunctions(
    vtkSmartPointer<vtkPiecewiseFunction> opacityTF,
    vtkSmartPointer<vtkColorTransferFunction> colorTF)
{
  opacityTF->AddPoint(0, 0);
  opacityTF->AddPoint(200, 0);
  opacityTF->AddPoint(200.1, 0);
  opacityTF->AddPoint(1250, 0.04);
  opacityTF->AddPoint(1250.1, 0);
  opacityTF->AddPoint(2400, 0.05);
  opacityTF->AddPoint(2400.1, 0.14);
  opacityTF->AddPoint(3300, 0.14);
  opacityTF->AddPoint(3300.1, 0.14);
  opacityTF->AddPoint(4095, 0.14);
  opacityTF->AddPoint(5010, 0.15);
  opacityTF->AddPoint(5050, 0.15);


  colorTF->AddHSVPoint(0, 0.666, 0, 1);
  colorTF->AddHSVPoint(200, 0.666, 0, 1);
  colorTF->AddHSVPoint(200.1, 0.0714, 1, 1);
  colorTF->AddHSVPoint(1250, 0.0938, 1, 0.8);
  colorTF->AddHSVPoint(1250.1, 0.3667, 0.5, 0.8);
  colorTF->AddHSVPoint(2400, 0.4667, 1, 0.5);
  colorTF->AddHSVPoint(2400.1, 0.5667, 1, 0.7698);
  colorTF->AddHSVPoint(3300, 0.6667, 1, 0.625);
  colorTF->AddHSVPoint(3300.1, 0.6667, 1, 0.625);
  colorTF->AddHSVPoint(4095, 0.6667, 1, 0.625);
  colorTF->AddHSVPoint(5010, 0.0, 1, 0.75);
  colorTF->AddHSVPoint(5050, 0.0, 1, 0.75);
}


//------------------------------------------------------------------------------
void VolumeRendererBase::generateTestOpacityTF(vtkSmartPointer<vtkPiecewiseFunction>&
                                               testOpacityTF)
{
  testOpacityTF->RemoveAllPoints();
  testOpacityTF->AddPoint(1,         0);
  testOpacityTF->AddPoint(621,   0.001);
  testOpacityTF->AddPoint(622,   0.001);
  testOpacityTF->AddPoint(4095,  0.001);
  testOpacityTF->AddPoint(4096,      0);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::generateTestColorTF(vtkSmartPointer<vtkColorTransferFunction>&
                                             testColorTF)
{
  testColorTF->RemoveAllPoints();
  testColorTF->AddHSVPoint(1,    0,         0,        0);
  testColorTF->AddHSVPoint(621,  0,         0,        0);
  testColorTF->AddHSVPoint(622,  0.29,  0.941,    0.689);
  testColorTF->AddHSVPoint(4095, 0.29,  0.941,    0.689);
  testColorTF->AddHSVPoint(4096, 1,         0,        1);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::resetTransferFunctions()
{
}


//------------------------------------------------------------------------------
void VolumeRendererBase::updateTransferFunctions()
{
  if ( (m_rawProperty !=  nullptr ) &&
       (m_opasityList.size()  != 0 ) &&
       (m_hsvColorList.size() != 0 )
       )
  {
    vtkSmartPointer<vtkPiecewiseFunction>  opacityTransferFunction =
        vtkSmartPointer<vtkPiecewiseFunction>::New();

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
        vtkSmartPointer<vtkColorTransferFunction>::New();
    this->initHSVColorTransferFunction(colorTransferFunction,
                                              m_currentHSVColorList);


    this->initOpacityTransferFunction(opacityTransferFunction,
                                      m_currentOpacityList);


    this->setPropertyScalarOpacity(0, opacityTransferFunction);
    this->setPropertyColor(0, colorTransferFunction);
    if(m_isBrightnessModified)
    {
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
}

//------------------------------------------------------------------------------
void VolumeRendererBase::updateRelativeTransferFunctions(std::list<HSVColorSigVal>& curHSVList,
                                                         std::list<OpacitySigVal>& curOpacityList)
{
  if ( (m_rawProperty !=  nullptr ) &&
       (curOpacityList.size()  != 0 ) &&
       (curHSVList.size() != 0 )
       )
  {
    vtkSmartPointer<vtkPiecewiseFunction>  opacityTransferFunction =
        vtkSmartPointer<vtkPiecewiseFunction>::New();

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
        vtkSmartPointer<vtkColorTransferFunction>::New();
    this->initHSVColorTransferFunction(colorTransferFunction,
                                              curHSVList);


    this->initOpacityTransferFunction(opacityTransferFunction,
                                      curOpacityList);


    this->setPropertyScalarOpacity(0, opacityTransferFunction);
    this->setPropertyColor(0, colorTransferFunction);
    if(m_isBrightnessModified)
    {
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
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setMaterialHighPassCutoff(double setValue)
{
  //---------------------------
  // Linear map  Y = Mx + B;
  double M = THRESHOLD_FILTER_MAP_SCALAR*(m_transtitonMap[SIG_METAL_MAX]
                                          - m_transtitonMap[SIG_ORGANIC_MIN]);
  double B =  m_transtitonMap[SIG_ORGANIC_MIN];

  m_materialDisplayCutoff = static_cast<uint16_t>(M*setValue + B);

  //------------------------------------------
  // std::cout << "highPasSignalCropMapper(double setValue): setValue : "
  //           << setValue
  //           << " - output = "
  //           << m_materialDisplayCutoff
  //           << std::endl;
  //------------------------------------------
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setThreatViewOpacityAndColorTF(std::list<OpacitySigVal>&  opacityList,
                                    std::list<HSVColorSigVal>& colorList)
{
  m_viewAsThreatrOpacityList.clear();
  m_vierwAsThreatHsvColorList.clear();

  std::list<OpacitySigVal>::iterator itOp;
  for (itOp = opacityList.begin();   itOp != opacityList.end(); itOp++)
  {
      OpacitySigVal ov = *itOp;
      m_viewAsThreatrOpacityList.push_back(ov);
  }

  std::list<HSVColorSigVal>::iterator itCol;
  for (itCol = colorList.begin();   itCol != colorList.end(); itCol++)
  {
      HSVColorSigVal cv = *itCol;
      m_vierwAsThreatHsvColorList.push_back(cv);
  }
}

//------------------------------------------------------------------------------
// ARO-NOTE: Method to be deprecated.
// Performance Optimization related changes
// Create opacity transfer function for threats
//------------------------------------------------------------------------------
void VolumeRendererBase::initThreatOpacityTF(vtkSmartPointer<vtkPiecewiseFunction> &threatOpacityTF,
                                             std::list<OpacitySigVal>& opasityLis)

{
    threatOpacityTF->RemoveAllPoints();
    std::list<OpacitySigVal>::iterator itL1;
    for (itL1 = opasityLis.begin(); itL1 !=opasityLis.end(); itL1++)
    {
        threatOpacityTF->AddPoint(itL1->m_signalVal, itL1->m_opacity);
    }
}

//------------------------------------------------------------------------------
// ARO-NOTE:  Method to be deprecated.
// Performance Optimization related changes
// Create color transfer function for threats
//------------------------------------------------------------------------------
void VolumeRendererBase::initThreatHSVColorTF(vtkSmartPointer<vtkColorTransferFunction> &threatColorTF,
                                              std::list<HSVColorSigVal>& hsvColorList)

{
    if (!m_renderWithColor)
    {
        this->initThreatBWTF(threatColorTF, hsvColorList);
        return;
    }

    threatColorTF->RemoveAllPoints();

    std::list<HSVColorSigVal>::iterator itL;
    HSVColorSigVal colVal;

    if(m_doColorizeThreat)
    {
        for (itL = hsvColorList.begin(); itL !=hsvColorList.end(); itL++)
        {
            colVal = *itL;
            threatColorTF->AddHSVPoint(colVal.m_signalVal, colVal.m_h, colVal.m_s, colVal.m_v,
                                       g_threatTFMidopoint, g_threatTFSharpness);
        }
    }
    else
    {
        for (itL = hsvColorList.begin(); itL !=hsvColorList.end(); itL++)
        {
            colVal = *itL;
            // Set value to 0
            threatColorTF->AddHSVPoint(colVal.m_signalVal, colVal.m_h, colVal.m_s, 0,
                                       g_threatTFMidopoint, g_threatTFSharpness);
        }
    }
}

//------------------------------------------------------------------------------
// ARO-NOTE: Method to be deprecated.
// Performance Optimization related changes
// Create BW transfer function for threats
//------------------------------------------------------------------------------
void VolumeRendererBase::initThreatBWTF(vtkSmartPointer<vtkColorTransferFunction> &threatColorTF,
                                              std::list<HSVColorSigVal>& hsvColorList)

{
    threatColorTF->RemoveAllPoints();

    std::list<HSVColorSigVal>::iterator itL;
    HSVColorSigVal colVal;
    double sigVal;
    double h, s, v;

    if(m_doColorizeThreat)
    {
        for (itL = hsvColorList.begin(); itL !=hsvColorList.end(); itL++)
        {
            colVal = *itL;
            sigVal = colVal.m_signalVal;
            h = colVal.m_h;
            s = colVal.m_s;
            v = colVal.m_v;

            // Added offset to signal value to get darker grey color
            // This is to get same effect as adding offset to intensity values
            double signalWithOffset = sigVal + m_currThreatOffset;
            this->hsvOfMetalGray(signalWithOffset, h, s, v);

            threatColorTF->AddHSVPoint(sigVal, h, s, v,
                                       g_threatTFMidopoint,
                                       g_threatTFSharpness);
        }
    }
    else
    {
        for (itL = hsvColorList.begin(); itL !=hsvColorList.end(); itL++)
        {
            colVal = *itL;
            sigVal = colVal.m_signalVal;
            v = 0;
            h = colVal.m_h;
            s = colVal.m_s;

            // Added offset to signal value to get darker grey color
            // This is to get same effect as adding offset to intensity values
            double signalWithOffset = sigVal + m_currThreatOffset;
            this->hsvOfMetalGray(signalWithOffset, h, s, v);

            threatColorTF->AddHSVPoint(sigVal, h, s, v,
                                       g_threatTFMidopoint,
                                       g_threatTFSharpness);
        }
    }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::initOpacityTransferFunction(vtkSmartPointer<vtkPiecewiseFunction>
                                                           opacityTF,
                                                           std::list<OpacitySigVal>& opasityLis)
{
  std::list<OpacitySigVal>::iterator  opIt;
  for (opIt = opasityLis.begin(); opIt != opasityLis.end(); opIt++)
  {
    OpacitySigVal opVal = *opIt;
    double sigVal       = opVal.m_signalVal;
    double opacity      = 1;



    //-----------------------------------------------
    // Set organic min based on the low density view
    // status.
    double currentOrganicMin = m_transtitonMap[SIG_ORGANIC_MIN];
    if (m_lowDensityActive)
    {
      currentOrganicMin = m_transtitonMap[SIG_LOW_DENSITY_MIN];
    }
    //-----------------------------------------------

    //-----------------------
    // BELOW  ORGANIC
    if ( sigVal <= currentOrganicMin )
    {
        opacity = opVal.m_opacity;
        opacityTF->AddPoint(sigVal, opacity);
    }
    //-----------------------

    //-----------------------
    // ORGANIC
    if (
        ( sigVal >= currentOrganicMin ) &&
        ( sigVal <= m_transtitonMap[SIG_ORGANIC_MAX] )
        )
    {
      if ( ((m_organicRemovable) && (!m_doColorizeOrganic)) ||
           (sigVal <= m_materialDisplayCutoff)
         )
      {
        opacity = 0;
        opacityTF->AddPoint(sigVal, opacity);
      }
      else
      {
        opacity = opVal.m_opacity;
        opacityTF->AddPoint(sigVal, opacity);
      }
    }
    //-----------------------

    //-----------------------
    // INORGANIC
    if (
        ( sigVal >= m_transtitonMap[SIG_INORGANIC_MIN] ) &&
        ( sigVal <= m_transtitonMap[SIG_INORGANIC_MAX] )
        )
    {
       if ( ((m_inorganicRemovable) && (!m_doColorizeInorganic) ) ||
            ( sigVal <= m_materialDisplayCutoff)
          )
       {
         opacity = 0;
         opacityTF->AddPoint(sigVal, opacity);
       }
       else
       {
        opacity = opVal.m_opacity;
        opacityTF->AddPoint(sigVal, opacity);
       }
    }

    //-----------------------

    //-----------------------
    // METAL
    if (
         ( sigVal >= m_transtitonMap[SIG_METAL_MIN] ) &&
         ( sigVal <= m_transtitonMap[SIG_METAL_MAX] )
       )
    {
      if( ((m_metalRemovable) && (!m_doColorizeMetal) ) ||
           ( sigVal <= m_materialDisplayCutoff)
         )
      {
        opacity = 0;
        opacityTF->AddPoint(sigVal, opacity);
      }
      else
      {
       opacity = opVal.m_opacity;
       opacityTF->AddPoint(sigVal, opacity);
      }
    }
    //-----------------------

    //-----------------------
    // Threats
    if (( sigVal >= m_transtitonMap[SIG_THREAT_MIN] ) &&
        ( sigVal <= m_transtitonMap[SIG_THREAT_MAX] )
        )
    {
          opacity = opVal.m_opacity;
          opacityTF->AddPoint(sigVal, opacity);
    }
    //---------------------
  }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::initHSVColorTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTF,
                                                      std::list<HSVColorSigVal>& hsvColorList,
                                                      bool isThreat)
{
  if (!m_renderWithColor)
  {
    this->initBWTransferFunction(colorTF, hsvColorList, isThreat);
    return;
  }

  double currentOrganicMin = m_transtitonMap[SIG_ORGANIC_MIN];
  if (m_lowDensityActive)
  {
    currentOrganicMin = m_transtitonMap[SIG_LOW_DENSITY_MIN];
  }

  std::list<HSVColorSigVal>::iterator colIt;
  for (colIt = hsvColorList.begin(); colIt!= hsvColorList.end(); colIt++)
  {
    HSVColorSigVal colVal = *colIt;
    double sigVal  = colVal.m_signalVal;
    double tmapOrgMin = m_transtitonMap[SIG_ORGANIC_MAX];
    double h, s, v;

    v = 0;


    //-----------------------
    // BWELOW  ORGANIC
    if (sigVal <= currentOrganicMin)
    {
      h = colVal.m_h;
      s = colVal.m_s;
      v = colVal.m_v;
      colorTF->AddHSVPoint(sigVal, h, s, v);
    }
    //-----------------------

    //-----------------------
    // ORGANIC
    if ( (sigVal >= currentOrganicMin) &&
         (sigVal <= m_transtitonMap[SIG_ORGANIC_MAX]) &&
         (sigVal >= m_materialDisplayCutoff)
       )
    {
      if (m_doColorizeOrganic)
      {
          h = colVal.m_h;
          s = colVal.m_s;
          v = colVal.m_v;
      }
      else
      {
        this->hsvOfOrganicGray(sigVal, h, s, v);
      }
      colorTF->AddHSVPoint(sigVal, h, s, v);
    }
    //-----------------------

    //-----------------------
    // INORGANIC
    if ((sigVal >= m_transtitonMap[SIG_INORGANIC_MIN]) &&
        (sigVal <= m_transtitonMap[SIG_INORGANIC_MAX]) &&
        (sigVal >= m_materialDisplayCutoff)
        )
    {
      if (m_doColorizeInorganic)
      {
        h = colVal.m_h;
        s = colVal.m_s;
        v = colVal.m_v;
      }
      else
      {
        this->hsvOfInorganicGray(sigVal, h, s, v);
      }
      colorTF->AddHSVPoint(sigVal, h, s, v);
    }

    //-----------------------

    //-----------------------
    // METAL
    if ((sigVal >= m_transtitonMap[SIG_METAL_MIN]) &&
        (sigVal <= m_transtitonMap[SIG_METAL_MAX]) &&
        (sigVal >= m_materialDisplayCutoff)
        )
    {
      if (m_doColorizeMetal)
      {
        h = colVal.m_h;
        s = colVal.m_s;
        v = colVal.m_v;
      }
      else
      {
        this->hsvOfMetalGray(sigVal, h, s, v);
      }
      colorTF->AddHSVPoint(sigVal, h, s, v, g_threatTFMidopoint, g_threatTFSharpness);
    }

    //-----------------------

    //------------------------
    // Threats
    if ( (sigVal >= m_transtitonMap[SIG_THREAT_MIN]) &&
         (sigVal <= m_transtitonMap[SIG_THREAT_MAX])
         )
    {
      if (m_doColorizeThreat)
      {
        h = colVal.m_h;
        s = colVal.m_s;
        v = colVal.m_v;
      }
      colorTF->AddHSVPoint(sigVal, h, s, v, g_threatTFMidopoint, g_threatTFSharpness);
    }
    //-------------------------
  }
}

//------------------------------------------------------------------------------
void VolumeRendererBase::initBWTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTF,
                                         std::list<HSVColorSigVal>& hsvColorList,
                                                bool isThreat)
{
  std::list<HSVColorSigVal>::iterator colIt;
  for (colIt = hsvColorList.begin(); colIt!= hsvColorList.end(); colIt++)
  {
    HSVColorSigVal colVal = *colIt;
    double sigVal  = colVal.m_signalVal;
    double h, s, v;

    v = 0;
    double threatDefV = GRAY_SCALE_DEF_THREAT_V;


    double currentOrganicMin = m_transtitonMap[SIG_ORGANIC_MIN];
    if (m_lowDensityActive)
    {
      currentOrganicMin = m_transtitonMap[SIG_LOW_DENSITY_MIN];
    }

    //-----------------------
    // BWELOW  ORGANIC
    if (sigVal <= currentOrganicMin)
    {
      h = colVal.m_h;
      s = colVal.m_s;
      v = colVal.m_v;
      if (!isThreat)
      {
        this->hsvOfOrganicGray(sigVal, h, s, v);
        colorTF->AddHSVPoint(sigVal, h, s, v);
       }
       else
      {
        colorTF->AddHSVPoint(sigVal, 0, 0, threatDefV);
      }
    }
    //-----------------------

    //-----------------------
    // ORGANIC
    if ( (sigVal >= currentOrganicMin) &&
         (sigVal <= m_transtitonMap[SIG_ORGANIC_MAX]) &&
         (sigVal >  m_materialDisplayCutoff)
       )
    {
      if (m_doColorizeOrganic)
      {
        if (!isThreat)
        {
          this->hsvOfOrganicGray(sigVal, h, s, v);
          colorTF->AddHSVPoint(sigVal, h, s, v);
        }
        else
        {
          colorTF->AddHSVPoint(sigVal, 0, 0, threatDefV);
        }
      }
    }
    //-----------------------

    //-----------------------
    // INORGANIC
    if ((sigVal >= m_transtitonMap[SIG_INORGANIC_MIN]) &&
        (sigVal <= m_transtitonMap[SIG_INORGANIC_MAX]) &&
        (sigVal >  m_materialDisplayCutoff)
        )
    {
      if (m_doColorizeInorganic)
      {
        if (!isThreat)
        {
          this->hsvOfInorganicGray(sigVal, h, s, v);
          colorTF->AddHSVPoint(sigVal, h, s, v);
        }
        else
        {
          colorTF->AddHSVPoint(sigVal, 0, 0, threatDefV);
        }
      }
    }

    //-----------------------

    //-----------------------
    // METAL
    if ((sigVal >= m_transtitonMap[SIG_METAL_MIN] ) &&
        (sigVal <= m_transtitonMap[SIG_THREAT_MAX]) &&
        (sigVal >  m_materialDisplayCutoff)
        )
    {
      if (m_doColorizeMetal)
      {
        if(!isThreat)
        {
          this->hsvOfMetalGray(sigVal, h, s, v);
          colorTF->AddHSVPoint(sigVal, h, s, v);
        }
        else
        {
          colorTF->AddHSVPoint(sigVal, 0, 0, threatDefV);
        }
      }
    }
    //-----------------------
    //------------------------
    // Threats
    if ( (sigVal >= m_transtitonMap[SIG_THREAT_MIN]) &&
         (sigVal <= m_transtitonMap[SIG_THREAT_MAX])
         )
    {
      if (m_doColorizeThreat)
      {
        h = colVal.m_h;
        s = colVal.m_s;
        v = colVal.m_v;
      }
      if (!isThreat)
      {
        this->hsvOfMetalGray(sigVal, h, s, v);
        colorTF->AddHSVPoint(sigVal, h, s, v, g_threatTFMidopoint, g_threatTFSharpness);
      }
      else
      {
        colorTF->AddHSVPoint(sigVal, 0, 0, threatDefV);
      }
    }
    //-------------------------
  }

  //-------------------------------------------------------------
  // ARO-DEBUGGING
  // Uncomment to save the grayscale Transferfunction to a file
  //
  //  QString sFile = "/home/analogic/junk/LeftViewGrayscaleTF.csv";
  //  if (this->displayName().contains("Left View"))
  //  {
  //     this->saveVTKColorMapToFile(colorTF,sFile);
  //  }
  //-------------------------------------------------------------
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setColorizeThreat(bool setVal)
{
  m_colorizeThreat = setVal;
}

//------------------------------------------------------------------------------
bool VolumeRendererBase::isThreatColorized()
{
  return m_colorizeThreat;
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setLightingParams(double ambient,
                                           double diffuse,
                                           double specular)
{
  if (!m_rawProperty) return;
  m_rawProperty->SetAmbient(ambient);
  m_rawProperty->SetDiffuse(diffuse);
  m_rawProperty->SetSpecular(specular);
}

//------------------------------------------------------------------------------
bool VolumeRendererBase::getLightingParams(double& ambient,
                                           double& diffuse,
                                           double& specular)
{
  if (!m_rawProperty)
  {
    ambient  = 0;
    diffuse  = 0;
    specular = 0;
    return false;
  }

  ambient  = m_rawProperty->GetAmbient();
  diffuse  = m_rawProperty->GetDiffuse();
  specular = m_rawProperty->GetSpecular();
  return true;
}

//------------------------------------------------------------------------------
bool VolumeRendererBase::isMsskClipped()
{
  return m_maskIsClipped;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setDefaultLightingParams()
{
  m_rawProperty->SetAmbient(m_ambientLighting);
  m_rawProperty->SetDiffuse(m_diffuseLighting);
  m_rawProperty->SetSpecular(m_specularLighting);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::hsvToRgb(double h, double s, double v,
              double& r, double& g, double& b)
{
  // The value of s, v, and a must all be in the range 0-255;
  // the value of h must be in the range 0-359.
  int qt_h = 359.0*h;
  int qt_s = 255.0*s;
  int qt_v = 255.0*v;
  QColor hsvColor = QColor::fromHsv(qt_h, qt_s, qt_v);
  r = hsvColor.red()/255.0;
  g = hsvColor.green()/255.0;
  b = hsvColor.blue()/255.0;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::rgbToHsv(double dR, double dG, double dB,
                                  double& dH,  double& dS, double& dV)
{
  QColor rgbColor(dR*255,
                  dG*255,
                  dB*255);

   QColor hsvColor = rgbColor.toHsv();

   int h = hsvColor.hue();
   int s = hsvColor.saturation();
   int v = hsvColor.value();

   dH = 1.0*h/359.0;
   dS = 1.0*s/255.0;
   dV = 1.0*v/255.0;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::createSubVolumeWidget(OpenGLVolumeWidget* qvtkWidget)
{
    Q_UNUSED(qvtkWidget);
  m_subVolumeBoxWidget = vtkSmartPointer<CenterSelectBoxWidget>::New();
  m_subVolumeBoxWidget->SetTranslationEnabled(1);
  vtkRenderWindowInteractor* rwi =  m_renderWindowRef->GetInteractor();
  if (rwi)
  {
    m_subVolumeBoxWidget->SetInteractor(rwi);
  }
  m_subVolumeBoxWidget->OutlineCursorWiresOff();
  m_subVolumeBoxWidget->ScalingEnabledOff();
  m_subVolumeBoxWidget->RotationEnabledOff();
  m_subVolumeBoxWidget->GetOutlineProperty()->SetColor(OPERATOR_THRERAT_INITIAL_RED,
                                      OPERATOR_THRERAT_INITIAL_GREEN,
                                      OPERATOR_THRERAT_INITIAL_BLUE);


  m_subVolumeBoxWidget->GetOutlineProperty()->SetLineWidth(THREAT_BOUNDS_LINE_WIDTH);

  double bounds[6] = {g_pickBox_x0,
                      g_pickBox_x1,
                      g_pickBox_y0,
                      g_pickBox_y1,
                      g_pickBox_z0,
                      g_pickBox_z1};

  m_subVolumeBoxWidget->PlaceWidget(bounds);
}


//------------------------------------------------------------------------------
void VolumeRendererBase::showSubVolume(QVector3D& pCenter,
                                    QVector3D& pRange)
{
  if (!m_subVolumeBoxWidget) return;

  double bounds[6] = {(pCenter.x() - pRange.x()/2),
                      (pCenter.x() + pRange.x()/2),

                      (pCenter.y() - pRange.y()/2),
                      (pCenter.y() + pRange.y()/2),

                      (pCenter.z() - pRange.z()/2),
                      (pCenter.z() + pRange.z()/2)};

  m_subVolumeBoxWidget->PlaceWidget(bounds);
  m_subVolumeBoxWidget->On();
}


//----------------------------------------------------------------------------------------
void VolumeRendererBase::setShowThreatOnlyValue(bool threatOnly)
{
  m_threatBMPOnly = threatOnly;
}

//----------------------------------------------------------------------------------------
bool VolumeRendererBase::getThreatOnlyViewStatus()
{
 return  m_threatBMPOnly;
}


//------------------------------------------------------------------------------
void VolumeRendererBase::hideSubVolume()
{
  if (!m_subVolumeBoxWidget) return;
  m_subVolumeBoxWidget->Off();
}


//------------------------------------------------------------------------------
void VolumeRendererBase::increaseOperatorThreatsize()
{
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();

   m_subVolumeBoxWidget->GetPolyData(polyData.GetPointer());

  double oldBounds[6];

  polyData->GetBounds(oldBounds);

  QVector3D P0 = QVector3D(oldBounds[0],
                           oldBounds[2],
                           oldBounds[4]);

  QVector3D P1 = QVector3D(oldBounds[1],
                           oldBounds[3],
                           oldBounds[5]);


  QVector3D center =  {(P0.x() + P1.x())/2,
                       (P0.y() + P1.y())/2,
                       (P0.z() + P1.z())/2};

  double width  =   (P1.x() - P0.x());
  double height =   (P1.y() - P0.y());
  double length =   (P1.z() - P0.z());


  // std::cout << "FFF: width computed:" << width << std::endl;

  double newWidth  = width*g_UserPickZoomInScale;
  double newHeight = height*g_UserPickZoomInScale;
  double newLength = length*g_UserPickZoomInScale;


  double newBounds[6] =
    {  (center.x() - newWidth/2.0),
       (center.x() + newWidth/2.0),

       (center.y() - newHeight/2.0),
       (center.y() + newHeight/2.0),

       (center.z() - newLength/2.0),
       (center.z() + newLength/2.0)
    };

  m_subVolumeBoxWidget->PlaceWidget(newBounds);
  m_renderWindowRef->Render();
}

void VolumeRendererBase::decreaseOperatorThreatsize()
{
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  m_subVolumeBoxWidget->GetPolyData(polyData.GetPointer());

  double oldBounds[6];

 polyData->GetBounds(oldBounds);

  QVector3D P0 = QVector3D(oldBounds[0],
                           oldBounds[2],
                           oldBounds[4]);

  QVector3D P1 = QVector3D(oldBounds[1],
                           oldBounds[3],
                           oldBounds[5]);


  QVector3D center =  {(P0.x() + P1.x())/2,
                       (P0.y() + P1.y())/2,
                       (P0.z() + P1.z())/2};

  double width  =  P1.x() - P0.x();
  double height =  P1.y() - P0.y();
  double length =  P1.z() - P0.z();

  double newWidth  = width*g_UserPickZoomOutScale;
  double newHeight = height*g_UserPickZoomOutScale;
  double newLength = length*g_UserPickZoomOutScale;

  double newBounds[6] =
    {  (center.x() - newWidth/2.0),
       (center.x() + newWidth/2.0),

       (center.y() - newHeight/2.0),
       (center.y() + newHeight/2.0),

       (center.z() - newLength/2.0),
       (center.z() + newLength/2.0)
    };

  m_subVolumeBoxWidget->PlaceWidget(newBounds);
  m_renderWindowRef->Render();
}

//------------------------------------------------------------------------------
void VolumeRendererBase::getSubWidgetBounds(QVector3D& p0,
                                            QVector3D& p1)
{
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  m_subVolumeBoxWidget->GetPolyData(polyData.GetPointer());

  double bounds[6];

  polyData->GetBounds(bounds);


  QVector3D flipP0 = QVector3D(bounds[0],
                               bounds[2],
                               bounds[4]);


  QVector3D flipP1 = QVector3D(bounds[1],
                               bounds[3],
                               bounds[5]);



  QVector3D imageP0;
  QVector3D imageP1;
  this->flipBoxInY(flipP0,
                   flipP1,
                   imageP0,
                   imageP1);

  double imageBounds[6];
  this->getBounds(imageBounds);


  double volBounds[6];
  m_rawVolume->GetBounds(volBounds);
  QVector3D volP0(volBounds[0],
                  volBounds[2],
                  volBounds[4]);

  QVector3D volP1(volBounds[1],
                  volBounds[3],
                  volBounds[5]);

  QVector3D volFlipP0;
  QVector3D volFlipP1;

  this->flipBoxInY(volP0,
                   volP1,
                   volFlipP0,
                   volFlipP1);

   if (imageP0.x() < volFlipP0.x()) imageP0.setX(volFlipP0.x());
   if (imageP0.y() < volFlipP0.y()) imageP0.setY(volFlipP0.y());
   if (imageP0.z() < volFlipP0.z()) imageP0.setZ(volFlipP0.z());

   if (imageP1.x() > volFlipP1.x()) imageP1.setX(volFlipP1.x());
   if (imageP1.y() > volFlipP1.y()) imageP1.setY(volFlipP1.y());
   if (imageP1.z() > volFlipP1.z()) imageP1.setZ(volFlipP1.z());

  p0 = imageP0;
  p1 = imageP1;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::clampHSV(double& h, double& s, double& v)
{
  if (h > 1.0)
  {
    h = 1.0;
    // std::cout << "xxxxxxxxx 1" << std::endl;
  }
  if (s > 1.0)
  {
    s = 1.0;
    // std::cout << "xxxxxxxxx 2" << std::endl;
  }
  if (v > 1.0)
  {
    // std::cout << "*** clampHSV(): v > 1.0 , v= "
    //          << v
    //          << std::endl;
    v = 1.0;
  }

  if (h < 0.0)
  {
    h = 0.0;
    // std::cout << "xxxxxxxxx 4" << std::endl;
  }
  if (s < 0.0)
  {
    s = 0.0;
    // std::cout << "xxxxxxxxx 5" << std::endl;
  }
  if (v < 0.0)
  {
    // std::cout << "*** clampHSV(): v < 0.0 , v= "
    //           << v
    //           << std::endl;
    v = 0.0;
  }
}



//------------------------------------------------------------------------------
// ARO-NOTE: from design.  -- MIGHT CHANGE
// For color stripping, we want to suppress the color, not the entire
// voxels. For example if we turn off organic, voxels between 400 and 1450
// would be changed from orange to grayscale.  This means loading
// a new color transfer table with 400-1450 changed to grayscale.
// For the grayscale color transfer you can start with this: (HSV) set
// Hue and Saturation to 0 for both 400 and 1450,
// Value starts at 0.9 for 400 and goes down to 0 for 1450.
// Opacity goes from 0.009 to 0.1 .
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void VolumeRendererBase::hsvOfOrganicGray(double signalValue,
                      double& h,
                      double& s,
                      double& v)
{
  h = 0;
  s = 0;
  //----------------------------
  // Linear relationship
  // v = A*signalValue + B;
  if (!m_inverseBackground)
  {
    double A = (m_grayscaleStruct.m_HSV_V_organicMax - m_grayscaleStruct.m_HSV_V_organicMin)/
        ( m_transtitonMap[SIG_ORGANIC_MAX] - m_transtitonMap[SIG_ORGANIC_MIN] );
    double B = m_grayscaleStruct.m_HSV_V_organicMin - A*m_transtitonMap[SIG_ORGANIC_MIN];
    v = A*signalValue + B;
    //-------------------------
    // ARO-DEBUG
    // if( v > 1.0)
    // {
    //    std:cout << "HSV (v) at clamp level: v= "
    //             << v
    //             << "  ' v = A*sigVal + B' A= "
    //             << A
    //             << " B= "
    //             << B
    //             << " Sig value= "
    //             << signalValue
    //             << std::endl;
    //  }
    //-------------------------
  }
  else
  {
    double A = (m_grayscaleStruct.m_HSV_V_organicMax_inv - m_grayscaleStruct.m_HSV_V_organicMin_inv)/
        ( m_transtitonMap[SIG_ORGANIC_MAX] - m_transtitonMap[SIG_ORGANIC_MIN] );
    double B = m_grayscaleStruct.m_HSV_V_organicMin_inv - A*m_transtitonMap[SIG_ORGANIC_MIN];
    v = A*signalValue + B;
  }
  this->clampHSV(h, s, v);
}


//------------------------------------------------------------------------------
void VolumeRendererBase::hsvOfInorganicGray(double signalValue,
                      double& h,
                      double& s,
                      double& v)
{
  h = 0;
  s = 0;
  //----------------------------
  // Linear relationship
  // v = A*signalValue + B;
  if (!m_inverseBackground)
  {
    double A =(m_grayscaleStruct.m_HSV_V_inorganicMax - m_grayscaleStruct.m_HSV_V_inorganicMin)/
        (m_transtitonMap[SIG_INORGANIC_MAX] - m_transtitonMap[SIG_INORGANIC_MIN]);
    double B = m_grayscaleStruct.m_HSV_V_inorganicMin - A*m_transtitonMap[SIG_INORGANIC_MIN];
    v = A*signalValue + B;
  }
  else
  {
    double A =(m_grayscaleStruct.m_HSV_V_inorganicMax_inv - m_grayscaleStruct.m_HSV_V_inorganicMin_inv)/
        (m_transtitonMap[SIG_INORGANIC_MAX] - m_transtitonMap[SIG_INORGANIC_MIN]);
    double B = m_grayscaleStruct.m_HSV_V_inorganicMin_inv - A*m_transtitonMap[SIG_INORGANIC_MIN];
    v = A*signalValue + B;
  }
  this->clampHSV(h, s, v);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::hsvOfMetalGray(double signalValue,
                      double& h,
                      double& s,
                      double& v)
{
  h = 0;
  s = 0;
  //----------------------------
  // Linear relationship
  // v = A*signalValue + B;
  if (!m_inverseBackground)
  {
    double A = (m_grayscaleStruct.m_HSV_V_metalMax - m_grayscaleStruct.m_HSV_V_metalMin)/
        ( m_transtitonMap[SIG_METAL_MAX] - m_transtitonMap[SIG_METAL_MIN] );
    double B = m_grayscaleStruct.m_HSV_V_metalMin - A*m_transtitonMap[SIG_METAL_MIN];
    v = A*signalValue + B;
  }
  else
  {
    double A = (m_grayscaleStruct.m_HSV_V_metalMax_inv - m_grayscaleStruct.m_HSV_V_metalMin_inv)/
        ( m_transtitonMap[SIG_METAL_MAX] - m_transtitonMap[SIG_METAL_MIN] );
    double B = m_grayscaleStruct.m_HSV_V_metalMin_inv - A*m_transtitonMap[SIG_METAL_MIN];
    v = A*signalValue + B;
  }
  this->clampHSV(h, s, v);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::opacityOfOrganicGray(double signalValue,
                          double& opacity)
{
  //---------------------------------
  // Linear relationship
  // o = A*signalValue + B
  double A = (m_grayscaleStruct.m_opacityOrganicMax - m_grayscaleStruct.m_opacityOrganicMin)/
      ( m_transtitonMap[SIG_ORGANIC_MAX]  - m_transtitonMap[SIG_ORGANIC_MIN]  );
  double B = m_grayscaleStruct.m_opacityOrganicMin - A*m_transtitonMap[SIG_ORGANIC_MIN];

  opacity = A*signalValue + B;
}


//------------------------------------------------------------------------------
void VolumeRendererBase::opacityOfInorganicGray(double signalValue,
                          double& opacity)
{
  //---------------------------------
  // Linear relationship
  // o = A*signalValue + B
  double A = (m_grayscaleStruct.m_opacityInorganicMax - m_grayscaleStruct.m_opacityInorganicMin)/
      (m_transtitonMap[SIG_INORGANIC_MAX]   - m_transtitonMap[SIG_INORGANIC_MIN] );
  double B = m_grayscaleStruct.m_opacityInorganicMin - A*m_transtitonMap[SIG_INORGANIC_MIN];

  opacity = A*signalValue + B;
}





//------------------------------------------------------------------------------
void VolumeRendererBase::opacityOfMetalGray(double signalValue,
                          double& opacity)
{
  //---------------------------------
  // Linear relationship
  // o = A*signalValue + B
  double A = (m_grayscaleStruct.m_opacityMetalMax - m_grayscaleStruct.m_opacityMetalMin)/
      (m_transtitonMap[SIG_METAL_MAX]   - m_transtitonMap[SIG_METAL_MIN] );
  double B = m_grayscaleStruct.m_opacityMetalMin - A*m_transtitonMap[SIG_METAL_MIN];

  opacity = A*signalValue + B;
}

//------------------------------------------------------------------------------
double VolumeRendererBase::largetDim(double dimX,
                 double dimY,
                 double dimZ)
{
  double  maxGuess;
  maxGuess = std::max(dimX, dimY);
  maxGuess = std::max(maxGuess, dimZ);
  return(maxGuess);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::clampBounds(double bounds[6])
{
  // Compare Bounds to Volume Bounds.
  double volBounds[6];
  if (!m_rawVolume) return;
  m_rawVolume->GetBounds(volBounds);
  QVector3D PV_Min(volBounds[0],
                   volBounds[2],
                   volBounds[4]);

  QVector3D PV_Max(volBounds[1],
                   volBounds[3],
                   volBounds[5]);


  QVector3D thMin(bounds[0],
                  bounds[2],
                  bounds[4]);


  QVector3D thMax(bounds[1],
                  bounds[3],
                  bounds[5]);
  //---------------------------------------------------
  // Clamp X
  if ( thMin.x() < PV_Min.x())  thMin.setX(PV_Min.x());
  if ( thMin.x() > PV_Max.x())  thMin.setX(PV_Max.x());

  if ( thMax.x() < PV_Min.x())  thMax.setX(PV_Min.x());
  if ( thMax.x() > PV_Max.x())  thMax.setX(PV_Max.x());

  //---------------------------------------------------
  // Clamp Y
  if ( thMin.y() < PV_Min.y())  thMin.setY(PV_Min.y());
  if ( thMin.y() > PV_Max.y())  thMin.setY(PV_Max.y());

  if ( thMax.y() < PV_Min.y())  thMax.setY(PV_Min.y());
  if ( thMax.y() > PV_Max.y())  thMax.setY(PV_Max.y());

  //---------------------------------------------------
  // Clamp Z
  if ( thMin.z() < PV_Min.z())  thMin.setZ(PV_Min.z());
  if ( thMin.z() > PV_Max.z())  thMin.setZ(PV_Max.z());

  if ( thMax.z() < PV_Min.z())  thMax.setZ(PV_Min.z());
  if ( thMax.z() > PV_Max.z())  thMax.setZ(PV_Max.z());

  bounds[0] = thMin.x();
  bounds[1] = thMax.x();

  bounds[2] = thMin.y();
  bounds[3] = thMax.y();

  bounds[4] = thMin.z();
  bounds[5] = thMax.z();
}



//------------------------------------------------------------------------------
void VolumeRendererBase::generatePolyDataFromPoints(QVector3D volP0,
                                                    QVector3D volP1,
                                                    vtkSmartPointer<vtkPolyData> polyData,
                                                    bool bIsShield)
{
  //-----------------------------------------------
  // Front Face of Box
  double origin[3] = {volP0.x(),
                      volP0.y(),
                      volP0.z()};

  double p0[3] = {volP0.x(),
                  volP1.y(),
                  volP0.z()};




  double p1[3] = {volP1.x(),
                  volP1.y(),
                  volP0.z()};

  double p2[3] = {volP1.x(),
                  volP0.y(),
                  volP0.z()};

  double p3[3] = {volP0.x(),
                  volP0.y(),
                  volP0.z()};

  //-----------------------------------------
  // Rear Face of Box
  double origin_rear[3] = {volP0.x(),
                           volP0.y(),
                           volP1.z()};

  double p4[3] = {volP0.x(),
                  volP1.y(),
                  volP1.z()};




  double p5[3] = {volP1.x(),
                  volP1.y(),
                  volP1.z()};

  double p6[3] = {volP1.x(),
                  volP0.y(),
                  volP1.z()};

  double p7[3] = {volP0.x(),
                  volP0.y(),
                  volP1.z()};


  int nPoints = 0;

  // Create a vtkPoints object and store the points in it
  vtkSmartPointer<vtkPoints> points =
      vtkSmartPointer<vtkPoints>::New();

  //-----------------------------------------------------------
  // Face A
  points->InsertNextPoint(origin);
  points->InsertNextPoint(p0);
  points->InsertNextPoint(p1);
  points->InsertNextPoint(p2);
  points->InsertNextPoint(p3);

  //-----------------------------------------------------------
  // Face B
  points->InsertNextPoint(origin_rear);
  points->InsertNextPoint(p4);
  points->InsertNextPoint(p5);
  points->InsertNextPoint(p6);
  points->InsertNextPoint(p7);

  // NOTE: we get a a free (i.e. unspecified  edge between {p3 & p7}.

  // Thus: to finish the cube edges we need only 3 more edges to get the total of
  // 12 edges.  The principal requirement is that the edge points be added so closed
  // loops on each square goes in a single direction. See below.

  points->InsertNextPoint(p4);
  points->InsertNextPoint(p0);

  points->InsertNextPoint(p1);
  points->InsertNextPoint(p5);

  points->InsertNextPoint(p6);
  points->InsertNextPoint(p2);

  ///---------------------------------------
  /// Note: ALSO  Works:

  /// points->InsertNextPoint(p6);
  /// points->InsertNextPoint(p2);

  /// points->InsertNextPoint(p1);
  /// points->InsertNextPoint(p5);

  /// points->InsertNextPoint(p4);
  /// points->InsertNextPoint(p0);
  ///---------------------------------------

  bool sheilldTest = bIsShield;
  //--------------------------------------------
  // ARO-NOTE: This feature is being reverted.
  // To keep the knowlege of palcing an X on
  // each side I will just remove the
  // shield test and forece shieldTest
  // to false.
  sheilldTest = false;
  if (!sheilldTest)
  {
    nPoints = 16;
  }
  else
  {
    nPoints = 40;
    // Diagonal 1 & 2
    points->InsertNextPoint(p0);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p6);
    points->InsertNextPoint(p4);

    // Diagonal 3 & 4
    points->InsertNextPoint(p7);
    points->InsertNextPoint(p5);
    points->InsertNextPoint(p1);
    points->InsertNextPoint(p3);

    // Diagonal 5 & 6
    points->InsertNextPoint(p4);
    points->InsertNextPoint(p3);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p5);

    // Diagonal 7 & 8
    points->InsertNextPoint(p6);
    points->InsertNextPoint(p1);
    points->InsertNextPoint(p0);
    points->InsertNextPoint(p7);

    // Diagonal 9 & 10
    points->InsertNextPoint(p0);
    points->InsertNextPoint(p5);
    points->InsertNextPoint(p6);
    points->InsertNextPoint(p3);

    // Diagonal 11 & 12
    points->InsertNextPoint(p4);
    points->InsertNextPoint(p1);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p7);
  }

  vtkSmartPointer<vtkPolyLine> polyLine =
      vtkSmartPointer<vtkPolyLine>::New();
  polyLine->GetPointIds()->SetNumberOfIds(nPoints);
  for(int i = 0; i < nPoints; i++)
  {
    polyLine->GetPointIds()->SetId(i, i);
  }

  // Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> cells =
      vtkSmartPointer<vtkCellArray>::New();
  cells->InsertNextCell(polyLine);


  // Add the points to the dataset
  polyData->SetPoints(points);

  // Add the lines to the dataset
  polyData->SetLines(cells);
}


//------------------------------------------------------------------------------
void VolumeRendererBase::flipPointInY(QVector3D& p0,
                                      QVector3D& flipP0)
{
  QVector3D inp0 = p0;
  QVector3D inp1 = p0;
  QVector3D outP0;
  this->flipBoxInY(inp0,
                   inp1,
                   flipP0,
                   outP0);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::flipBoxInY(QVector3D& p0,
                                     QVector3D& p1,
                                     QVector3D& flipP0,
                                     QVector3D& flipP1)
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->InsertNextPoint ( p0.x(), p0.y(), p0.z());
  points->InsertNextPoint ( p1.x(), p1.y(), p1.z());

  // Add the grid points to a polydata object
  vtkSmartPointer<vtkPolyData> cornerPolydata = vtkSmartPointer<vtkPolyData>::New();
      cornerPolydata->SetPoints(points);


  vtkSmartPointer<vtkTransform> flipTransForm = vtkSmartPointer<vtkTransform>::New();
  flipTransForm->SetMatrix(m_flipYMatrix);

  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =    vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  transformFilter->SetInputData(cornerPolydata);
  transformFilter->SetTransform(flipTransForm);
  transformFilter->Update();

  vtkSmartPointer<vtkPolyData> cornerPolydataFlipped = vtkSmartPointer<vtkPolyData>::New();
  cornerPolydataFlipped->SetPoints(transformFilter->GetOutput()->GetPoints());

  double df0[3];
  double df1[3];
  vtkPoints* pFlipped =  cornerPolydataFlipped->GetPoints();
  pFlipped->GetPoint(0, df0);
  pFlipped->GetPoint(1, df1);



  QVector3D pf0 = QVector3D(df0[0],
                            df0[1],
                            df0[2]);

  QVector3D pf1 = QVector3D(df1[0],
                            df1[1],
                            df1[2]);

  //-------------------------------------------
  // ARO Now that we flip Y we need to flip
  // the Y values of the pick box.

  double tmpY = pf0.y();
  pf0.setY(pf1.y());
  pf1.setY(tmpY);
  //--------------------------------------------

  flipP0 = pf0;
  flipP1 = pf1;
}

//------------------------------------------------------------------------------
void VolumeRendererBase::debugCompareSubWidgetToBounds(QVector3D& p0,
                                                       QVector3D& p1)
{
  // Compare Bounds to Volume Bounds.
  double volBounds[6];
  m_rawVolume->GetBounds(volBounds);


  QVector3D pv0 = QVector3D(volBounds[0],
      volBounds[2],
      volBounds[4]);


  QVector3D pv1 = QVector3D(volBounds[1],
      volBounds[3],
      volBounds[5]);


  double vW = p1.x() - p0.x();
  double vH = p1.y() - p0.y();
  double vL = p1.z() - p0.z();

  // Convert to liters from CUBIC MILLIMETERS;
  double threatVolumeLiters = vW*vH*vL*m_pixelSpacingX*m_pixelSpacingY*m_pixelSpacingZ*1.0E-6;

  QString strPolyVolume = "*SSVVSS* - Polydata volume (L) - " +  QString::number(threatVolumeLiters);


  QString strPolyBounds = "*SSVVSS* - PolyData bounds [x,y,x] P0[" +
      QString::number(p0.x(), 'g', 3) + ", " +
      QString::number(p0.y(), 'g', 3) + ", " +
      QString::number(p0.z(), 'g', 3) + "] - P1[" +
      QString::number(p1.x(), 'g', 3) + ", " +
      QString::number(p1.y(), 'g', 3) + ", " +
      QString::number(p1.z(), 'g', 3) + "]";

  QString strVolBounds  = "*SSVVSS* - Volume bounds   [x,y,x] P0[" +
      QString::number(pv0.x(), 'g', 3) + ", " +
      QString::number(pv0.y(), 'g', 3) + ", " +
      QString::number(pv0.z(), 'g', 3) + "] - P1[" +
      QString::number(pv1.x(), 'g', 3) + ", " +
      QString::number(pv1.y(), 'g', 3) + ", " +
      QString::number(pv1.z(), 'g', 3) + "]";

  qDebug() <<  strPolyBounds;
  qDebug() <<  strVolBounds;
  qDebug() <<  strPolyVolume;

  LOG(INFO) << strPolyBounds.toLocal8Bit().data();
  LOG(INFO) << strVolBounds.toLocal8Bit().data();
  LOG(INFO) << strPolyVolume.toLocal8Bit().data();
}


//------------------------------------------------------------------------------
void VolumeRendererBase::setPropertyColor(int index,
                      vtkColorTransferFunction* colorTF)
{
  //------------------------------------
  // ARO-DEBUG:
  // if (this->displayName().contains("Right"))
  // {
  //   std::cout << "Set Property Color - index: "
  //             << index << "  - Renderer name -: "
  //             << this->displayName().toStdString()
  //             << std::endl;
  //
  //
  //   this->showColorTransferFunction(colorTF);
  // }
  //------------------------------------
  this->m_rawProperty->SetColor(index, colorTF);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::setPropertyScalarOpacity(int index,
                              vtkPiecewiseFunction* opacityTF)
{
  m_rawProperty->SetScalarOpacity(index, opacityTF);
}

//------------------------------------------------------------------------------
void VolumeRendererBase::saveVTKColorMapToFile(
                     vtkSmartPointer<vtkColorTransferFunction> colorTF,
                     QString& filenameOut)
{
  std::ofstream file_vtkMap;
  file_vtkMap.open (filenameOut.toLocal8Bit().data());


  int sizeMap = colorTF->GetSize();
  double nodeValue[5];
  file_vtkMap << "Signal,R,G,B\n";
  for (int i = 0; i < sizeMap; i++)
  {
    colorTF->GetNodeValue(i, nodeValue);
    QString strOut = QString::number(nodeValue[0]) + " ," +
                     QString::number(nodeValue[1]) + " ," +
                     QString::number(nodeValue[2]) + " ," +
                     QString::number(nodeValue[3]) + "\n";

    file_vtkMap << strOut.toLocal8Bit().data();
  }

  file_vtkMap.close();
}


//------------------------------------------------------------------------------
void VolumeRendererBase::saveVTKOpacityMapToFile(vtkSmartPointer<vtkPiecewiseFunction>  opacityTF,
                                                 QString& filenameOut)
{
  std::ofstream file_vtkMap;
  file_vtkMap.open (filenameOut.toLocal8Bit().data());

  int sizeMap = opacityTF->GetSize();

  double nodeValue[5];
  file_vtkMap << "Signal,Opacity\n";
  for (int i=0; i < sizeMap; i++)
  {
    opacityTF->GetNodeValue(i, nodeValue);
    QString strOut = QString::number(nodeValue[0]) + " ," +
                     QString::number(nodeValue[1]) + " \n";

    file_vtkMap << strOut.toLocal8Bit().data();
  }

  file_vtkMap.close();
}



//------------------------------------------------------------------------------
double  VolumeRendererBase::scaleOpacityValue(int pcntChange, double setPoint)
{
  // Y = M*x + B;
  if (pcntChange == 50) return setPoint;
  if (pcntChange < 50)
  {
    double m = setPoint/50.0;
    double y = m*(1.0*pcntChange);
    return y;
  }
  double x = 1.0*(pcntChange - 50.0);
  double b = setPoint;
  double m = (m_contrastModifyScalar -1.0)*setPoint/50.0;
  double y = m*x + b;
  if (y > 1) y = 1;
  return y;
}
//------------------------------------------------------------------------------
double  VolumeRendererBase::scaleSaturationValue(int pcntChange, double setPoint)
{
  // Y = M*x + B;
  if (pcntChange == 50) return setPoint;
  if (pcntChange < 50)
  {
     double m = setPoint/50.0;
     double y = m*(1.0*pcntChange);
     return y;
  }

  double x = 1.0*(pcntChange - 50.0);
  double b = setPoint;
  double m = (m_brightnessModifyScalar -1.0)*setPoint/50.0;
  double y = m*x + b;
  if (y > 1) y = 1;
  return y;
}
//------------------------------------------------------------------------------
double  VolumeRendererBase::scaleSignalValue(int pcntChange, double setPoint)
{
 double deltaChange = (pcntChange - 50.0)/100.0;
 double y = setPoint - m_opacityModifyScalar*setPoint*deltaChange;
 return y;
}
