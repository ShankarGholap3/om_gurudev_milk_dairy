//------------------------------------------------------------------------------
// File: OpenGLVolumeWidget.cpp
// Purpose: Implementation of QVTKOpenGLWidget subclass OpenGLVolumeWidget
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "OpenGLVolumeWidget.h"


double g_pinchZoomScaleFactor = 1.0;


const double g_scaleInThreshold  = 0.95;
const double g_scaleOutThreshold = 1.15;


const double g_zoomDisplayLowerLimit = 0.0;
const double g_zoomDisplayUpperLimit = 1000.0;


double g_mouseWheelMotionsScalingFactror = 0.88;


int g_wheelTimeOut = 60;

vtkStandardNewMacro(MouseInteractorStylePP);



int  OpenGLVolumeWidget::s_zoomCount       = 0;
int  OpenGLVolumeWidget::s_zoomCountThreat = 0;


//------------------------------------------------------------------------------
OpenGLVolumeWidget::OpenGLVolumeWidget(QWidget* parent,
                                       Qt::WindowFlags flags):
  QVTKOpenGLWidget(parent, flags),
  m_enableInteraction(true),
  m_volumeInitialized(false),
  m_distanceMeasureToolActive(false),
  m_distMeasureStatus(DISTANCE_MEASURE_INACTIVE),
  m_distanceCurrentRulerLength(0),
  m_distanceLastRulerLength(0),
  m_allowSplash(false),
  m_irenAdapter(nullptr),
  m_viewLabel(nullptr),
  m_ruler(nullptr),
  m_touchRotate(false),
  m_enableSplash(false),
  m_paintCount(0),
  m_isThreatViewer(false),
  m_touchPinchCount(0),
  m_touchSpreadCount(0),
  m_touchMoveCount(0),
  m_touchMoveX(0),
  m_touchMoveY(0),
  m_armForVolumeMeasurement(false),
  m_beginUserRegionSelection(false),
  m_parent(parent),
  m_touchHandler(nullptr),
  m_pickStyleRef(nullptr),
  m_wheelCompleteTimer(nullptr),
  m_widgetRenderer(nullptr)
{
  m_irenAdapter   =  new QVTKInteractorAdapter(this);

  m_touchHandler = new OpenGLTouchHandler(this);
  //----------------------------------
  // A Better touch pinch-spared-rotate
  this->setAttribute(Qt::WA_AcceptTouchEvents);
  //-----------------------------------

  // Set the Mouse wheel Motions scaling
  this->setMouseWheelMotionFactor(g_mouseWheelMotionsScalingFactror);


  m_wheelCompleteTimer = new QTimer(this);
  m_wheelCompleteTimer->setInterval(g_wheelTimeOut);
  this->connect(m_wheelCompleteTimer, &QTimer::timeout,
                this, &OpenGLVolumeWidget::onWheelTimeout);
}

//------------------------------------------------------------------------------
OpenGLVolumeWidget::~OpenGLVolumeWidget()
{
  delete m_touchHandler;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::paintGL()
{
  QVTKOpenGLWidget::paintGL();
  this->allowSplashScreenOn();
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::allowSplashScreenOn()
{
  m_allowSplash = true;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::allowSplashScreenOff()
{
  m_allowSplash = false;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::mousePickCallback(QVector3D& aPoint, void* caller)
{
  OpenGLVolumeWidget* pVolWidget =  reinterpret_cast<OpenGLVolumeWidget*>(caller);
  Q_UNUSED(pVolWidget);
  Q_UNUSED(aPoint);
  // std::cout << "====================================="  << std::endl;
  // std::cout << "Picked Point [x,y,z]: " <<  aPoint.x()  << ", "
  //                                       <<  aPoint.y()  << ", "
  //                                       <<  aPoint.z()  << std::endl;
  // std::cout << "====================================="  << std::endl;
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::rubberbandSelectCallback(QVector2D& p0,
                                                  QVector2D& p1,
                                                  void* caller)
{
  OpenGLVolumeWidget* pVolWidget =  reinterpret_cast<OpenGLVolumeWidget*>(caller);
  bool isArmed = pVolWidget->isArmedForRubberBandThreatSelect();
  if ( isArmed)
  {
    emit pVolWidget->rubberBandSelected(p0,
                                        p1);
    // std::cout << "====================================="  << std::endl;
    // std::cout << "Rubber band spelected p0 [x,y]: "  <<  p0.x()  << ", "
    //                                                  <<  p0.y()  <<  std::endl;
    // std::cout << "Rubber band spelected p1 [x,y]: "  <<  p1.x()  << ", "
    //                                                  <<  p1.y()  <<  std::endl;
    // std::cout << "====================================="  << std::endl;
  }
}

//------------------------------------------------------------------------------
bool OpenGLVolumeWidget::event(QEvent *event)
{
  if (event->type() ==  QEvent::TouchBegin )
  {
    emit this->beginInteraction();
  }
  // A Better touch pinch-spred-rotate.
  QList<QTouchEvent::TouchPoint> touchPoints;
  QTouchEvent *touchEvent = nullptr;
  if ( (event->type() ==  QEvent::TouchBegin ) ||
       (event->type() ==  QEvent::TouchUpdate) ||
       (event->type() ==  QEvent::TouchEnd   )
       )
  {
    // Two finger touch actions.
    touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
    touchEvent = static_cast<QTouchEvent *>(event);
  }

  QWheelEvent* pWheelEvent;
  switch (event->type())
  {
  case QEvent::Wheel:

    pWheelEvent = reinterpret_cast<QWheelEvent *>(event);
    if (pWheelEvent)
    {
      this->wheelEvent(pWheelEvent);
    }
    break;
  case QEvent::TouchBegin:
    if (touchPoints.count() == 1)
    {
      m_touchHandler->resetCurrentKeyEvent();
      m_touchHandler->setTwoFingerTouchActive(false);
      const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
      QPoint PS0 = QPoint(touchPoint0.startPos().rx(), touchPoint0.startPos().ry());
      m_touchHandler->setStartPos(PS0, PS0);

      if ( m_distanceMeasureToolActive &&
           (m_distMeasureStatus == DISTANCE_MEASURE_START))
      {
        // this->showDebugDistanceMeasure("QEvent::TouchBegin:");
        this->distanceMeasurementSetStart();
        this->simulateMousePress(PS0);
        this->distanceMeasureInputUpdate(false);
      }
      else if(m_armForVolumeMeasurement)
      {
        // For Volume Computatin two fingers are needed.
        // But we stil want to be able to rotate the window.
        this->simulateMousePress(PS0);
      }
      else
      {
        this->simulateMousePress(PS0);
      }
    }
    break;
  case QEvent::TouchUpdate:
    if (touchPoints.count() == 1)
    {
      m_touchHandler->setTwoFingerTouchActive(false);

      const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
      QPoint PC0 = QPoint(touchPoint0.pos().rx(), touchPoint0.pos().ry());
      m_touchHandler->setCurrentPos(PC0, PC0);

      if (m_distanceMeasureToolActive)
      {
         m_distanceCurrentRulerLength = this->distanceMeasurementCurrentLength();
         double deltaLength = m_distanceCurrentRulerLength - m_distanceLastRulerLength;
         if ( fabs(deltaLength) >  TOUCH_LENGTH_DELTA_THRESHOLD )
         {
           m_distMeasureStatus = DISTANCE_MEASURE_MOVE_HEAD;
         }
         else
         {
           m_distMeasureStatus = DISTANCE_MEASURE_TOUCH_ROTATE;
         }
         m_distanceLastRulerLength  = m_distanceCurrentRulerLength;
      }

      if  ( (m_distMeasureStatus ==  DISTANCE_MEASURE_INACTIVE ) ||
            (m_distMeasureStatus ==  DISTANCE_MEASURE_TOUCH_ROTATE ) )
      {
        // std::cout <<  "Distance Rotate or Inactive" << std::endl;
        // this->showDebugDistanceMeasure("TOUCH-ROTATE:");
        QPoint pMove = PC0;
        TouchProcResulst_E tpr = m_touchHandler->processOneFingerTouch(pMove);
        if (tpr == TPR_Rotate)
        {
          // std::cout << "Touch simulate mouse move [x,y]: " << PC0.rx() << ", " << PC0.ry() << std::endl;
          if (this->isInteractioEnabled())
          {
            m_touchRotate = true;
            this->simulateMouseMove(pMove);
          }
        }
      }
      else
      {
        // Move Head of distae widget.
        // std::cout << "ZZZ m_distanceMeasureStaus:" << std::endl;
        // this->showDistancMeasureStatus(m_distMeasureStatus);
        // this->showDebugDistanceMeasure("TOUCH-GROW-RULER:");
        m_touchRotate = false;
        this->simulateMouseMove(PC0);
      }
    }
    if (touchPoints.count() == 2)
    {
      const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
      const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();

      QPoint PS0 = QPoint(touchPoint0.startPos().rx(), touchPoint0.startPos().ry());
      QPoint PS1 = QPoint(touchPoint1.startPos().rx(), touchPoint1.startPos().ry());

      QPoint midPoint((PS0.rx() + PS1.rx())/2,
                      (PS0.ry() + PS1.ry())/2);

      if (m_armForVolumeMeasurement)
      {
        m_touchHandler->resetCurrentKeyEvent();
        //---------------------------------------
        // Two-finger volume computation
        //  now enabled.
        this->simulateMousePress(midPoint);
        this->volumeMeasureTouchUpdate(midPoint);
        //---------------------------------------
      }
      //-------------------------------------------------------------------
      // ARO-NOTE: for two fingered touch reset the distance
      //  measuring tool. To original state.
      if (m_distanceMeasureToolActive)
      {
        // this->showDebugDistanceMeasure("TWO-FINGER-RESET");
        //----------------------------------------------
        // ARO-TODO: try thhis afte below.
        // this->distanceMeasurementEndInteraction();
        //----------------------------------------------
        // std::cout << ">> Two point Touch Update mid-point distanceMeasurementSetStart() "
        //           << std::endl;
        this->setDistanceMeasureStatus(DISTANCE_MEASURE_INACTIVE);
        this->distanceMeasurementSetStart();
        this->simulateMousePress(midPoint);
        this->simulateMouseRelease(midPoint);
      }
      //-------------------------------------------------------------------
    }
    //------------------------------------------------------------
    //  ARO-NOTE: 9-17-2021
    //  Normally we would break here but now we _WANT_ to
    //  fall throught to the next case which is QEvent::TouchEdn
    //  So the break is commented.
    //  ARO-TODO: clean this up.
    // break;
    //------------------------------------------------------------
  case QEvent::TouchEnd:
  {
    if (touchPoints.count() == 2)
    {
      //---------------------------------------------
      // Determine if the two points are moving:
      // 1. Closer together
      // 2. Further apart
      // 3. In roughly the same direction
      // From the choices above decide to
      // implement a:
      // A. ZOOM Out of the scene
      // B. ZOOM In to the scene
      // C. Pan the Scene in the direction specified.
      //---------------------------------------------
      const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
      const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();


      QPoint PS0 = QPoint(touchPoint0.startPos().rx(), touchPoint0.startPos().ry());
      QPoint PS1 = QPoint(touchPoint1.startPos().rx(), touchPoint1.startPos().ry());

      QPoint PC0 = QPoint(touchPoint0.pos().rx(), touchPoint0.pos().ry());
      QPoint PC1 = QPoint(touchPoint1.pos().rx(), touchPoint1.pos().ry());

      if (!m_touchHandler->isTwoFingerTouchActive())
      {
        m_touchHandler->setStartPos(PS0, PS1);
        m_touchHandler->setTwoFingerTouchActive(true);
      }
      m_touchHandler->setCurrentPos(PC0, PC1);

      double deltaX = 0;
      double deltaY = 0;

      TouchProcResulst_E tpr = m_touchHandler->processTwoFingerTouch(deltaX,
                                                                     deltaY);
      //-------------------------------------------
      // ARO-NOTE: To see debug output uncomment.
      // m_touchHandler->debugShowStatus();
      //-------------------------------------------

      if (tpr == TPR_Pinch)
      {
        if (this->isInteractioEnabled())
        {
          m_touchHandler->performTouchPinch();
          m_touchHandler->setStartPos(PC0, PC1);
        }
      }
      if (tpr == TPR_Spread)
      {
        if (this->isInteractioEnabled())
        {
          m_touchHandler->performTouchSpread();
          m_touchHandler->setStartPos(PC0, PC1);
        }
      }
      if (tpr == TPR_Move)
      {
        if (this->isInteractioEnabled())
        {
          m_touchHandler->performTouchMove(deltaX, deltaY);
          m_touchHandler->setStartPos(PC0, PC1);
        }
      }
      if (touchEvent->touchPointStates() & Qt::TouchPointReleased)
      {
        //----------------------------------------------------------
        // ARO-NOTE: This might confuse Scene Pan so we remove it.
        // Added to allow quick reset of distance measure tool
        // if needed.
        // if (m_distanceMeasureToolActive)
        // {
        //   std::cout << "Two finger reset measure tool!"
        //             << std::endl;
        //  this->distanceMeasurementReset();
        // }
        //----------------------------------------------------------

        if (m_touchHandler->getCurrentKeyEvent() == TPR_Spread)
        {
          this->logKeyStrokeEvent("ZI", "");
        }
        if (m_touchHandler->getCurrentKeyEvent() == TPR_Pinch)
        {
          this->logKeyStrokeEvent("ZO", "");
        }
        if (m_touchHandler->getCurrentKeyEvent() == TPR_Move)
        {
          this->logKeyStrokeEvent("EP", "");
        }

        // If one of the fingers is released, set active two finger touch to false,
        bool wasTwoFingers = m_touchHandler->setTwoFingerTouchActive(false);
        if (wasTwoFingers)
        {
          const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.last();
          QPoint PC0 = QPoint(touchPoint0.pos().rx(), touchPoint0.pos().ry());
          // std::cout << "Touch relase from TWO fingers to ONE - Relase Pos [x,y]: "
          //           << PC0.rx() << ", " << PC0.ry() << std::endl;
          this->simulateMousePress(PC0);
        }

        event->accept();
        return true;
      }
      event->accept();
      return true;
    }
    if (touchPoints.count() == 1)
    {
      if (touchEvent->touchPointStates() & Qt::TouchPointReleased)
      {
        if( m_touchRotate )
        {
          if (!m_distanceMeasureToolActive)
          {
            emit this->interactorActivityEnd();
          }
        }

        const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
        QPoint PC0 = QPoint(touchPoint0.pos().rx(), touchPoint0.pos().ry()  + 1);

        //--------------------------------------
        if ( m_distanceMeasureToolActive &&
             (m_distMeasureStatus == DISTANCE_MEASURE_TOUCH_ROTATE)
           )
        {
          if (this->distanceMeasurementCurrentLength() > 1.0)
          {
             // this->showDebugDistanceMeasure("TOUCH-END:");
             this->setDistanceMeasureStatus(DISTANCE_MEASURE_INACTIVE);
             if (!this->isDistanceMeasurementComplete())
             {
              this->distanceMeasurementEndInteraction();
             }
          }
        }
        //--------------------------------------

        if ( ( m_distanceMeasureToolActive) &&
             ( m_distMeasureStatus ==  DISTANCE_MEASURE_MOVE_HEAD)
           )
        {
           m_distMeasureStatus = DISTANCE_MEASURE_TOUCH_ROTATE;
        }
        if (this->isArmedForRubberBandThreatSelect())
        {
          this->simulateMouseRelease(PC0);
          emit this->interactorActivityEnd();

          this->simulateMousePress(PC0);
          this->simulateMouseRelease(PC0);
          m_beginUserRegionSelection = false;
        }
        if (m_touchHandler->getCurrentKeyEvent()== TPR_Ignore)
        {
          this->logKeyStrokeEvent("ER", "");
        }
      }
    }
  }
    break;
  default:
    return QVTKOpenGLWidget::event(event);
  }
  return true;
}

//-----------   -------------------------------------------------------------------
void OpenGLVolumeWidget::onWheelTimeout()
{
  emit this->interactorActivityEnd();
  m_wheelCompleteTimer->stop();
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::doZoom(int wheelDelta,
                                bool propagateToSiblings)
{
  if (!m_isThreatViewer)
  {
    // Not a threat viewer
    if (wheelDelta > 0 )
    {
      // ZOOM IN wheelDelta > 0
      if (s_zoomCount < VTK_ZOOM_IN_LIMIT )
      {
        this->performZoom(wheelDelta);
        m_wheelCompleteTimer->start();
        s_zoomCount++;
      }
    }
    else
    {
      // Zoom OUT wheelDelta < 0
      if (s_zoomCount > VTK_ZOOM_OUT_LIMIT )
      {
        this->performZoom(wheelDelta);
        m_wheelCompleteTimer->start();
        s_zoomCount--;
      }
    }
    this->removeSplash();
    if (propagateToSiblings)
    {
      this->syncSibling();
    }
  }
  else
  {
    // Is a Threat viewer
    if (wheelDelta > 0 )
    {
      // ZOOM IN wheelDelta > 0
      if (s_zoomCountThreat < VTK_THREAT_ZOOM_IN_LIMIT )
      {
        this->performZoom(wheelDelta);
        m_wheelCompleteTimer->start();
        s_zoomCountThreat++;
      }
    }
    else
    {
      // Zoom OUT
      if (s_zoomCountThreat > VTK_THREAT_ZOOM_OUT_LIMIT )
      {
        this->performZoom(wheelDelta);
        m_wheelCompleteTimer->start();
        s_zoomCountThreat--;
      }
    }
    this->removeSplash();
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setDistanceMeasureStatus(distanceMeasureStatus newStatus)
{
  m_distMeasureStatus = newStatus;
}

//------------------------------------------------------------------------------
std::string OpenGLVolumeWidget::distancMeasureStatusAsString(distanceMeasureStatus aStatus)
{
  std::string statStr  = "";
  switch (aStatus)
  {
  case DISTANCE_MEASURE_INACTIVE:
    statStr = "DISTANCE_MEASURE_INACTIVE";
    break;
  case DISTANCE_MEASURE_START:
    statStr = "DISTANCE_MEASURE_START";
    break;
  case  DISTANCE_MEASURE_MOVE_HEAD:
    statStr = "DISTANCE_MEASURE_MOVE_HEAD";
   break;
  case DISTANCE_MEASURE_TOUCH_ROTATE:
    statStr = "DISTANCE_MEASURE_TOUCH_ROTATE";
    break;
  }
  return statStr;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::showDistancMeasureStatus(distanceMeasureStatus aStatus)
{
  std::string statStr  = distancMeasureStatusAsString(aStatus);
  std::cout << " *** OpenGLVolumeWidget::m_distMeasureStatus = "
            << statStr
            << std::endl;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::showDebugDistanceMeasure(const char* dbgString)
{
  std::string strOut = ">><<:";
  strOut += dbgString;
  strOut += " - m_distMeasureStatus:[";
  strOut += this->distancMeasureStatusAsString(m_distMeasureStatus);
  strOut += "]";
  std::string strDistMeasureActive ="";
  if (m_distanceMeasureToolActive)
  {
    strDistMeasureActive = " - m_distanceMeasureToolActive[true]";
  }
  else
  {
    strDistMeasureActive = " - m_distanceMeasureToolActive[false]";
  }
  strOut +=  strDistMeasureActive;
  std::cout << strOut << std::endl;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::distanceMeasurementSetStart()
{
  // std::cout << "%%%%%%%%% distanceMeasurementSetStart() %%%%%%%%"
  //          << std::endl;
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      this->GetInteractor();
  if (renderWindowInteractor)
  {
    double screenPick_X = renderWindowInteractor->GetEventPosition()[0];
    double screenPick_Y = renderWindowInteractor->GetEventPosition()[1];
    this->distanceMeasurementReset();
    QVector2D resetPoint(screenPick_X,
                         screenPick_Y);
    this->distancMeasureZeroLengthAt(resetPoint);
    this->simulateMouseRelease(QPoint(resetPoint.x(),
                                      resetPoint.y()));

    m_distanceLastRulerLength    = this->distanceMeasurementCurrentLength();
    m_distanceCurrentRulerLength = this->distanceMeasurementCurrentLength();
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::wheelEvent(QWheelEvent* event)
{
  if (!this->isInteractioEnabled()) return;

  QPoint wheelMove = event->angleDelta();
  int wheelDelta = 0;
  if (wheelMove.x() == 0)
  {
    wheelDelta = wheelMove.y();
  }
  else
  {
    wheelDelta = wheelMove.x();
  }

  if (wheelDelta > DEF_ZOOM_WHEEL_DELTA)
  {
    wheelDelta = DEF_ZOOM_WHEEL_DELTA;
  }
  if (wheelDelta < -1.0*DEF_ZOOM_WHEEL_DELTA)
  {
    wheelDelta = -1.0*DEF_ZOOM_WHEEL_DELTA;
  }

  // Note: wheel events propagate to siblings.
  this->doZoom(wheelDelta,
               true);
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if ( event->button() == Qt::LeftButton )
  {
    this->update();
    emit this->cameraPositionModified();
    if (m_distMeasureStatus != DISTANCE_MEASURE_INACTIVE )
    {
      // std::cout << "Mouse Release -> m_distanceMeasureStatus != DISTANCE_MEASURE_INACTIVE"
      //          <<  std::endl;
      this->setDistanceMeasureStatus(DISTANCE_MEASURE_INACTIVE);
      this->distanceMeasurementEndInteraction();
      QPoint curPos(event->pos().rx(),
                    event->pos().ry());
      this->simulateMousePress(curPos);
    }
  }
  emit this->interactorActivityEnd();
  QVTKOpenGLWidget::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------------`
void OpenGLVolumeWidget::resizeEvent(QResizeEvent*  event)
{
  QVTKOpenGLWidget::resizeEvent(event);
}

//----------------------------------------------------------------------------
void OpenGLVolumeWidget::paintEvent(QPaintEvent* event)
{
  QVTKOpenGLWidget::paintEvent(event);
  if (m_enableSplash)
  {
    if(m_paintCount == 0)
    {
      emit this->splashReady();
    }
    m_paintCount++;
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::mousePressEvent(QMouseEvent* event)
{
  if (m_armForVolumeMeasurement)
  {
    if (event->buttons() & Qt::RightButton)
    {
      this->volumeMeasureMouseUpdate();
    }
  }

  if ((m_distanceMeasureToolActive) && (event->buttons() & Qt::LeftButton))
  {
    this->distanceMeasureInputUpdate(true);
  }
  if ((m_distanceMeasureToolActive) && (event->buttons() & Qt::RightButton))
  {
    this->distanceMeasurementSetStart();
  }
  QVTKOpenGLWidget::mousePressEvent(event);
  emit this->beginInteraction();
}


//------------------------------------------------------------------------------`
void OpenGLVolumeWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!m_enableInteraction) return;
  // std::cout << "mouseMoveEvent()" << std::endl;
  if (event->buttons() & Qt::RightButton)
  {
    return;
  }
  QVTKOpenGLWidget::mouseMoveEvent(event);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setVTKRenderer(vtkRenderer* widgetRenderer)
{
  m_widgetRenderer = widgetRenderer;
}

//------------------------------------------------------------------------------
vtkRenderer* OpenGLVolumeWidget::getVTKRenderer()
{
  return m_widgetRenderer;
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setRuler(RulerWidget* pRuler)
{
  m_ruler  = pRuler;
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setViewLabel(ViewLabelWidget* pViewLabel)
{
  m_viewLabel  = pViewLabel;
}

//------------------------------------------------------------------------------
ViewLabelWidget* OpenGLVolumeWidget::getViewLabel()
{
  return m_viewLabel;
}

//------------------------------------------------------------------------------
OpenGLTouchHandler* OpenGLVolumeWidget::getTouchHandler()
{
  return m_touchHandler;
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::performZoom(int delta)
{
  QWheelEvent  wheelDirEvent(QPoint(0, 0),
                             delta,
                             Qt::NoButton,
                             Qt::NoModifier);

  vtkRenderWindow* t_renWin = this->GetRenderWindow();
  if (t_renWin)
  {
    m_irenAdapter->ProcessEvent(&wheelDirEvent, t_renWin->GetInteractor());
  }
  m_ruler->setRulerSize();
  double rZoom = m_ruler->getZoom();
  m_viewLabel->setZoomValue(rZoom);
}




//------------------------------------------------------------------------------
void OpenGLVolumeWidget::performWheelEvent(QWheelEvent* event)
{
  this->wheelEvent(event);
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::performPan(int panX, int panY)
{
  int lastEventPosX = 500;
  int lastEventPosY = 500;
  int currentEventPiosX = 500 + panX;
  int currentEventPiosY = 500 + panY;
  vtkRenderWindowInteractor* rwi = this->GetInteractor();

  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];

  vtkRenderer*    renderer =  this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
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
  emit this->cameraPositionModified();
}


//-----------------------------------------------------------------------------
void OpenGLVolumeWidget::enableSplash(bool doSplash)
{
  m_enableSplash = doSplash;
}

//-----------------------------------------------------------------------------
bool OpenGLVolumeWidget::isSplashEenabled()
{
  return m_enableSplash;
}

//-----------------------------------------------------------------------------
void OpenGLVolumeWidget::clearPaintCount()
{
  m_paintCount = 0;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::update()
{
  QVTKOpenGLWidget::update();
  if(!m_volumeInitialized) return;
  if ((m_ruler) && (m_viewLabel))
  {
    // std::cout << "eeeeee  m_ruler->setRulerSize() - OpenGLVolumeWidget::update()" << std::endl;
    m_ruler->setRulerSize();
    double rZoom = m_ruler->getZoom();
    m_viewLabel->setZoomValue(rZoom);
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setZoomCount(int zoomCount,
                                      int threatZoomCount)
{
  s_zoomCount       = zoomCount;
  s_zoomCountThreat = threatZoomCount;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::getZoomCount(int& zoomCount,
                                      int& threatZoomCount)
{
  zoomCount         = s_zoomCount;
  threatZoomCount   = s_zoomCountThreat;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::simulateMousePress(QPoint curPos)
{
  QMouseEvent  pressEvent(QEvent::MouseButtonPress,
                          curPos,
                          Qt::LeftButton,
                          Qt::NoButton,
                          Qt::NoModifier);
  QVTKOpenGLWidget::mousePressEvent(&pressEvent);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::simulateMousePressWithInteraction(QPoint curPos)
{
  QMouseEvent  pressEvent(QEvent::MouseButtonPress,
                          curPos,
                          Qt::LeftButton,
                          Qt::NoButton,
                          Qt::NoModifier);
  QVTKOpenGLWidget::mousePressEvent(&pressEvent);
  emit this->beginInteraction();
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::simulateMouseRelease(QPoint curPos)
{
  QMouseEvent  releaseEvent(QEvent::MouseButtonRelease,
                            curPos,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier);
  QVTKOpenGLWidget::mouseReleaseEvent(&releaseEvent);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::simulateMouseReleaseWithInteraction(QPoint curPos)
{
  QMouseEvent  releaseEvent(QEvent::MouseButtonRelease,
                            curPos,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier);
  QVTKOpenGLWidget::mouseReleaseEvent(&releaseEvent);
  emit this->beginInteraction();
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::simulateTouchRelease()
{
  QPoint curPos(0, 0);

  QMouseEvent  pressEvent(QEvent::MouseButtonPress,
                          curPos,
                          Qt::LeftButton,
                          Qt::NoButton,
                          Qt::NoModifier);
  QVTKOpenGLWidget::mousePressEvent(&pressEvent);

  QMouseEvent  releaseEvent(QEvent::MouseButtonRelease,
                            curPos,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier);
  QVTKOpenGLWidget::mouseReleaseEvent(&releaseEvent);




  vtkRenderWindow* t_renWin = this->GetRenderWindow();
  if (t_renWin)
  {
    m_irenAdapter->ProcessEvent(&pressEvent, t_renWin->GetInteractor());
    m_irenAdapter->ProcessEvent(&releaseEvent, t_renWin->GetInteractor());
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::simulateMouseMove(QPoint curPos)
{
  // Reomove splash if present.
  this->removeSplash();
  QMouseEvent  mvEvent(QEvent::MouseMove,
                       curPos,
                       Qt::LeftButton,
                       Qt::NoButton,
                       Qt::NoModifier);
  QVTKOpenGLWidget::mouseMoveEvent(&mvEvent);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setupPointPicker()
{
  //-----------------------------------
  // Mandatory:
  // We have a volume renderer so make sure the picker is
  // a volume picker.
  vtkSmartPointer<vtkVolumePicker> volPicker = vtkSmartPointer<vtkVolumePicker>::New();
  vtkRenderWindowInteractor* rwi = this->GetInteractor();
  if (rwi)
  {
    rwi->SetPicker(volPicker);
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setupRubberBandPicker()
{
  vtkSmartPointer<RubberBandPickVolume> style =
      vtkSmartPointer<RubberBandPickVolume>::New();
  vtkRenderWindowInteractor* rwi = this->GetInteractor();
  if (rwi)
  {
    rwi->SetInteractorStyle( style );
  }
  style->setCallback(&OpenGLVolumeWidget::rubberbandSelectCallback, this);
  m_pickStyleRef = style.GetPointer();
}


void setupDistanceMeasurer()
{}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::pickMousePressAndRelease(QVector2D pickPoint)
{
  QPoint pick(pickPoint.x(),
              pickPoint.y());
  this->simulateMousePress(pick);
  this->simulateMouseRelease(pick);
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setAsThreatViewer(bool setVal)
{
  m_isThreatViewer = setVal;
}


//-------------------------------------------------------------------------------
bool OpenGLVolumeWidget::isSplashAllowed()
{
  return m_allowSplash;
}

//-------------------------------------------------------------------------------
void OpenGLVolumeWidget::setEnableInteraction(bool setVal)
{
  m_enableInteraction = setVal;
}

//--------------------------------------------------------------------------------
bool OpenGLVolumeWidget::isInteractioEnabled()
{
  return m_enableInteraction;
}

//------------------------------------------------------------------------------
QVector3D OpenGLVolumeWidget::getPickPointWithPicker(QPoint screenPickPos)
{
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      this->GetInteractor();

  double screenPick_X = screenPickPos.rx();
  double screenPick_Y = screenPickPos.ry();
  QVector3D worldPoint(0, 0, 0);
  if (renderWindowInteractor)
  {
    vtkVolumePicker* volPicker =
        reinterpret_cast<vtkVolumePicker*>(renderWindowInteractor->GetPicker());
    double picked[3];

    int picRet =  volPicker->Pick(screenPick_X,
                                  screenPick_Y,
                                  0,  // always zero.
                                  m_widgetRenderer);

    QString volPickMessage  = "Volume Measurement - Screen pick point     (x,y): ";
    volPickMessage +=  QString::number(screenPick_X) +  ", ";
    volPickMessage +=  QString::number(screenPick_Y);

    LOG(INFO) << volPickMessage.toStdString();
    // std::cout << volPickMessage.toStdString() << std::endl;


    if (picRet != 0)
    {
      renderWindowInteractor->GetPicker()->GetPickPosition(picked);
      worldPoint = QVector3D(picked[0],
          picked[1],
          picked[2]);
      volPickMessage  =  "Volume Measurement - Good pick point     [x,y,z]: ";
      volPickMessage +=  QString::number(worldPoint.x()) + ", ";
      volPickMessage +=  QString::number(worldPoint.y()) + ", ";
      volPickMessage +=  QString::number(worldPoint.z());
      LOG(INFO) << volPickMessage.toStdString();
      // std::cout << volPickMessage.toStdString() << std::endl;
    }
    else
    {
      double viewFocus[4], focalDepth, estimatedPoint[3];
      vtkRenderer*    renderer =  this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
      vtkCamera*      camera = renderer->GetActiveCamera();
      camera->GetFocalPoint(viewFocus);
      //---------------------------------------------------------
      // Need to call this once first with the vewFocus as input
      // and return.
      this->computeWorldToDisplay(renderer,
                                  viewFocus[0],
          viewFocus[1],
          viewFocus[2],
          viewFocus);
      //---------------------------------------------------------

      focalDepth = viewFocus[2];

      // Second call get estimated point.
      this->computeDisplayToWorld(renderer,
                                  screenPick_X,
                                  screenPick_Y,
                                  focalDepth,
                                  estimatedPoint);

      worldPoint = QVector3D(estimatedPoint[0],
          estimatedPoint[1],
          estimatedPoint[2]);
      volPickMessage  =  "Volume Measurement - Estimated pick point[x,y,z]: ";
      volPickMessage +=  QString::number(worldPoint.x()) + ", ";
      volPickMessage +=  QString::number(worldPoint.y()) + ", ";
      volPickMessage +=  QString::number(worldPoint.z());

      LOG(INFO) << volPickMessage.toStdString();
      // std::cout << volPickMessage.toStdString() << std::endl;
    }
  }
  return worldPoint;
}

//------------------------------------------------------------------------------
bool OpenGLVolumeWidget::simulateVolumePick(QVector2D screenPoint,
                                            QVector3D& retPick)
{
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      this->GetInteractor();
  if (renderWindowInteractor)
  {
    vtkRenderer* renWin =
        renderWindowInteractor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    renderWindowInteractor->GetPicker()->Pick(screenPoint.x(),
                                              screenPoint.y(),
                                              0,  // always zero.
                                              renWin);

    double picked[3];
    renderWindowInteractor->GetPicker()->GetPickPosition(picked);
    retPick = QVector3D(picked[0],
        picked[1],
        picked[2]);
    return true;
  }
  else
  {
    retPick = QVector3D(0, 0, 0);
  }
  return false;
}




//------------------------------------------------------------------------------
void OpenGLVolumeWidget::computeWorldToDisplay(vtkRenderer* ren,
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
void OpenGLVolumeWidget::computeDisplayToWorld(vtkRenderer* ren,
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
double OpenGLVolumeWidget::getMouseWheelMotionFactor()
{
  vtkRenderWindow* renderWindow = this->GetRenderWindow();
  if (renderWindow)
  {
    vtkRenderWindowInteractor* interactor = renderWindow->GetInteractor();
    vtkInteractorStyle *s = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());
    double dMs = s->GetMouseWheelMotionFactor();
    return dMs;
  }
  // Error condition.
  return -100.0;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setMouseWheelMotionFactor(double scaleFactor)
{
  vtkRenderWindow* renderWindow = this->GetRenderWindow();
  if (renderWindow)
  {
    vtkRenderWindowInteractor* interactor = renderWindow->GetInteractor();
    vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());
    style->SetMouseWheelMotionFactor(scaleFactor);
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::distanceMeasureInputUpdate(bool isRealMouse)
{
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      this->GetInteractor();
  if (renderWindowInteractor)
  {
    double screenPick_X = renderWindowInteractor->GetEventPosition()[0];
    double screenPick_Y = renderWindowInteractor->GetEventPosition()[1];
    // std::cout << "PICK [x, y] :  " << testX << ", " << testY << std::endl;
    QVector2D screenCoords(screenPick_X,
                           screenPick_Y);
    double picked[3];
    renderWindowInteractor->GetPicker()->Pick(renderWindowInteractor->GetEventPosition()[0],
        renderWindowInteractor->GetEventPosition()[1],
        0,  // always zero.
        renderWindowInteractor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
    renderWindowInteractor->GetPicker()->GetPickPosition(picked);
    QVector3D worldPoint(picked[0],
        picked[1],
        picked[2]);
    switch (m_distMeasureStatus)
    {
    case DISTANCE_MEASURE_START:
      // std::cout << "Touch DISTANCE_MEASURE_START   ->  transition: to DISTANCE_MEASURE_MOVE_HEAD"
      //           << std::endl;
      this->distanceMeasurePickPointTouch(worldPoint);
      this->setDistanceMeasureStatus(DISTANCE_MEASURE_MOVE_HEAD);

    break;
    case DISTANCE_MEASURE_MOVE_HEAD:
       // std::cout << "Distance Measure update on P1" << std::endl;
       // std::cout << "Touch DISTANCE_MEASURE_MOVE_HEAD"
       //           << std::endl;
       this->distanceMeasurePickPointTouch(worldPoint);
      break;
    case DISTANCE_MEASURE_INACTIVE:
      // std::cout << "DISTANCE_MEASURE_INACTIVE:" << std::endl;
      if(!isRealMouse)
      {
        QPoint touchPoint(screenPick_X,
                          screenPick_Y);

        QPoint retPoint;
        bool touchRedoMeasure = this->isToucPointNearHandle(touchPoint,
                                                            retPoint);
        if (touchRedoMeasure)
        {
          // std::cout << "Tuch near handle "
          //           << " DISTANCE_MEASURE_INACTIVE: transition to DISTANCE_MEASURE_START "
          //           << std::endl;
          // std::cout << "Do we need to restart? " << std::endl;
          this->setDistanceMeasureStatus(DISTANCE_MEASURE_START);
          this->distancMeasureZeroLengthAt(screenCoords);
          m_distanceLastRulerLength    = this->distanceMeasurementCurrentLength();
          m_distanceCurrentRulerLength = this->distanceMeasurementCurrentLength();
        }
      }
      break;
    }
  }
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::volumeMeasureMouseUpdate()
{
  setCursor(Qt::BusyCursor);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      this->GetInteractor();
  double screenPick_X = renderWindowInteractor->GetEventPosition()[0];
  double screenPick_Y = renderWindowInteractor->GetEventPosition()[1];
  QPoint pickPoint(screenPick_X,
                   screenPick_Y);

  QVector3D worldPoint =   this->getPickPointWithPicker(pickPoint);
  emit this->volumeMeasurePickPoint(worldPoint);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::volumeMeasureTouchUpdate(QPoint& touchPoint)
{
  m_queuedTouchPoints.push_back(touchPoint);
  if (m_queuedTouchPoints.size() == 4)
  {
    setCursor(Qt::BusyCursor);
    double avgX = 0;
    double avgY = 0;
    for (int i = 0; i < 4; i++)
    {
      QPoint testPoint = m_queuedTouchPoints.at(i);
      avgX += testPoint.rx();
      avgY += testPoint.ry();
    }
    m_queuedTouchPoints.clear();
    avgX /= 4.0;
    avgY /= 4.0;
    QPoint avjPos(avgX, avgY);
    QVector3D worldPoint = this->getPickPointWithPicker(avjPos);
    emit this->volumeMeasurePickPoint(worldPoint);
  }
}



//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setVolumeInitialized(bool volInit)
{
  m_volumeInitialized = volInit;
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::initZoomIndex()
{
  s_zoomCount       = 0;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::initThreatZoomIndex()
{
  s_zoomCountThreat = 0;
}

//------------------------------------------------------------------------------
bool OpenGLVolumeWidget::isVolumeInitialized()
{
  return m_volumeInitialized;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::armForOperatorRubberBandSelect(bool armval)
{
  if (!m_pickStyleRef) return;
  if (armval)
  {
    m_pickStyleRef->arm();
  }
  else
  {
    m_pickStyleRef->disArm();
  }
}

//------------------------------------------------------------------------------
bool OpenGLVolumeWidget::isArmedForRubberBandThreatSelect()
{
  return m_pickStyleRef->isArmed();
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::armForVolumeMeasurement(bool armVal)
{
  m_armForVolumeMeasurement = armVal;
}

//------------------------------------------------------------------------------
bool OpenGLVolumeWidget::isArmedForVolumeMeasurement()
{
  return m_armForVolumeMeasurement;
}
