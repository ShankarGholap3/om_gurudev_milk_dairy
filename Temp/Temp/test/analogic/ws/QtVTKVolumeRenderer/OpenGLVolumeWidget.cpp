//------------------------------------------------------------------------------
// File: OpenGLVolumeWidget.cpp
// Purpose: Implementation of subclass of QVTKOpenGLWidget
//
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include <QMessageBox>
#include "OpenGLVolumeWidget.h"
#include "LocalTouchHandler.h"

int g_interactionEndSplahTimeout = 10;


const int g_defaultWheelDelta = 40;
double g_pinchZoomScaleFactor = 1.0;


const double g_scaleInThreshold  = 0.95;
const double g_scaleOutThreshold = 1.15;


const double g_zoomDisplayLowerLimit = 0.0;
const double g_zoomDisplayUpperLimit = 1000.0;


double g_mouseWheelMotionsScalingFactror = 0.88;


int g_wheelTimeOut = 60;


//------------------------------------------------------------------------------
void myCallbackFunction(vtkObject* pObject,
                        uint64_t eid,
                        void* clientData,
                        void* callData)
{
  std::cout << "xxxxxxxxxxxxxxxxxx Left Mouse press Event xxxxxxxxxxxxxxxxxxxxxxx" << std::endl;
}


//------------------------------------------------------------------------------
OpenGLVolumeWidget::OpenGLVolumeWidget(QWidget* parent,
                                       Qt::WindowFlags flags):
  QVTKOpenGLWidget(parent, flags),
  m_imageSplashRef(nullptr),
  m_rendererRef(nullptr),
  m_touchHandler(nullptr),
  m_irenAdapter(nullptr),
  m_distanceRep(nullptr),
  m_measureAxisActor(nullptr)
{
  //----------------------------------
  // A Better touch pinch-spared-rotate
  this->setAttribute(Qt::WA_AcceptTouchEvents);
  //-----------------------------------

  m_touchHandler  =  new LocalTouchHandler(this);

  m_irenAdapter   =  new QVTKInteractorAdapter(this);

  this->connect(this, &OpenGLVolumeWidget::interactorActivityEnd,
                this,  &OpenGLVolumeWidget::onMultiTouchEnd);
}

//------------------------------------------------------------------------------
OpenGLVolumeWidget::~OpenGLVolumeWidget()
{
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
void OpenGLVolumeWidget::mousePressEvent(QMouseEvent* event)
{
  QVTKOpenGLWidget::mousePressEvent(event);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QVTKOpenGLWidget::mouseReleaseEvent(event);
  if ( event->button() == Qt::LeftButton )
  {
    // std::cout << "Left Button Mouse Release" << std::endl;
    QTimer::singleShot(g_interactionEndSplahTimeout,
                       this,
                        &OpenGLVolumeWidget::onIntractionEndTimeout);
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  QVTKOpenGLWidget::mouseMoveEvent(event);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::wheelEvent(QWheelEvent* event)
{
  vtkRenderWindow* t_renWin = this->GetRenderWindow();
  if (t_renWin)
  {
    m_irenAdapter->ProcessEvent(event, t_renWin->GetInteractor());
  }
}




//------------------------------------------------------------------------------
void OpenGLVolumeWidget::vtkMousePressEvent(QPoint mousePos)
{
  vtkRenderWindow* t_renWin = this->GetRenderWindow();
  QMouseEvent event = QMouseEvent(QEvent::MouseButtonPress,
                                       mousePos,
                                       Qt::LeftButton,
                                       Qt::NoButton,
                                       Qt::NoModifier);

  if (t_renWin && t_renWin->GetInteractor())
  {
    this->InteractorAdaptor->ProcessEvent(&event,
                                          t_renWin->GetInteractor());
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::vtkMouseReleaseEvent(QPoint mousePos)
{
  vtkRenderWindow* t_renWin = this->GetRenderWindow();
  QMouseEvent event = QMouseEvent(QEvent::MouseButtonRelease,
                                       mousePos,
                                       Qt::LeftButton,
                                       Qt::NoButton,
                                       Qt::NoModifier);

  if (t_renWin && t_renWin->GetInteractor())
  {
    this->InteractorAdaptor->ProcessEvent(&event,
                                          t_renWin->GetInteractor());
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::removeVolume()
{
  vtkSmartPointer<vtkRenderWindow>  renderWin = this->GetRenderWindow();
  vtkRendererCollection* renderCollection  = renderWin->GetRenderers();
  vtkRenderer* firstren = renderCollection->GetFirstRenderer();
  firstren->RemoveAllViewProps();
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::removeSplash()
{
   vtkSmartPointer<vtkRenderWindow>  renderWin = this->GetRenderWindow();
   vtkRendererCollection* renderCollection  = renderWin->GetRenderers();
   vtkRenderer* firstren = renderCollection->GetFirstRenderer();

  if(m_imageSplashRef)
  {
      firstren->RemoveActor2D(m_imageSplashRef);
      // std::cout << "Remove Splash" << std::endl;
      m_imageSplashRef = nullptr;
  }
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::performWheelEvent(QWheelEvent* event)
{
  this->wheelEvent(event);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::performZoomIn(bool propagatge)
{
  QWheelEvent  wheelInEvent(QPoint(0, 0),
                           g_defaultWheelDelta,
                           Qt::NoButton,
                           Qt::NoModifier);

  // QVTKOpenGLWidget::wheelEvent(&wheelInEvent);
  vtkRenderWindow* t_renWin = this->GetRenderWindow();
  if (t_renWin)
  {
    m_irenAdapter->ProcessEvent(&wheelInEvent, t_renWin->GetInteractor());
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::performZoomOut(bool propagate)
{
  QWheelEvent  wheelOutEvent(QPoint(0, 0),
                           -1*g_defaultWheelDelta,
                           Qt::NoButton,
                           Qt::NoModifier);
  vtkRenderWindow* t_renWin = this->GetRenderWindow();
  if (t_renWin)
  {
    m_irenAdapter->ProcessEvent(&wheelOutEvent,
                                t_renWin->GetInteractor());
  }
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::performPan(int panX,
                int panY)
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
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::resetViewAlignment()
{
  if (m_imageSplashRef)
  {
    this->removeSplash();
  }
  vtkRenderer*    renderer =  this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  renderer->ResetCamera();

  double fp[3];
  double pos[3];
  fp[0] = renderer->GetActiveCamera()->GetFocalPoint()[0];
  fp[1] = renderer->GetActiveCamera()->GetFocalPoint()[1];
  fp[2] = renderer->GetActiveCamera()->GetFocalPoint()[2];
  pos[0] = renderer->GetActiveCamera()->GetPosition()[0];
  pos[1] = renderer->GetActiveCamera()->GetPosition()[1];
  pos[2] = renderer->GetActiveCamera()->GetPosition()[2];

  double dist = std::sqrt( (pos[0]- fp[0])*(pos[0] - fp[0]) +
      (pos[1]- fp[1])*(pos[1] - fp[1]) +
      (pos[2]- fp[2])*(pos[2] - fp[2])  );



  renderer->GetActiveCamera()->SetPosition(fp[0], fp[1], fp[2] + dist);
  renderer->GetActiveCamera()->SetViewUp(0.0, 1.0, 0.0);


  // Angles here are defined in Degrees.
   double rollAngle = 90;
   double yawAngle  = 90;

   this->yawView(renderer,  yawAngle);
   this->rollView(renderer, rollAngle);

   renderer->ResetCamera();
   this->updateRender();

   emit this->interactorActivityEnd();
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
void OpenGLVolumeWidget::simulateMouseRelease(QPoint curPos)
{
  std::cout << "simulated Mouse Release %%%%%%" << std::endl;

  QMouseEvent  releaseEvent(QEvent::MouseButtonRelease,
                            curPos,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier);
   QVTKOpenGLWidget::mouseReleaseEvent(&releaseEvent);
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::simulateMouseMove(QPoint curPos)
{
  QMouseEvent  mvEvent(QEvent::MouseMove,
                       curPos,
                       Qt::LeftButton,
                       Qt::NoButton,
                       Qt::NoModifier);
  QVTKOpenGLWidget::mouseMoveEvent(&mvEvent);
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::updateRender()
{
   vtkSmartPointer<vtkRenderWindow>  renderWin = this->GetRenderWindow();
   if (renderWin)
   {
    renderWin->Render();
   }
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setRendererBGColor(double bgRed,
                                            double bgGreen,
                                            double bgBlue)
{
  vtkRenderer*    renderer =  this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  if (renderer)
  {
    renderer->SetBackground(bgRed,
                            bgGreen,
                            bgBlue);
  }
}



//------------------------------------------------------------------------------
void OpenGLVolumeWidget::setRenderer(vtkRenderer* aRenderer)
{
  m_rendererRef = aRenderer;
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::showSplash()
{
  this->removeSplash();
  vtkSmartPointer<vtkRenderWindow>  renderWin = this->GetRenderWindow();

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

  //------------------------------------------------------------
  // ARO_NOTE:  old code
  // int clip_y_max = VTK_MAIN_VIEWER_WIN_HEIGHT -  marginY;
  //------------------------------------------------------------
  int clip_y_max = VTK_MAIN_VIEWER_WIN_HEIGHT;

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
  imageScale->SetScale(0.15);
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
  m_rendererRef->AddActor(imageActor);
  m_rendererRef->Render();
  //=====================================
}



//------------------------------------------------------------------------------
bool OpenGLVolumeWidget::event(QEvent *event)
{
  /*
  if (event->type() ==  QEvent::TouchBegin )
  {
    emit this->beginInteraction();
  }
  */
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

  switch (event->type())
  {
  //  vtkRenderWindow* t_renWin = this->GetRenderWindow();
  //  if (t_renWin)
  //  {
  //    m_irenAdapter->ProcessEvent(&releaseEvent,
  //                                t_renWin->GetInteractor());
  //  }

  // QVTKOpenGLWidget::mouseReleaseEvent(&releaseEvent);
  case QEvent::TouchBegin:
    this->removeSplash();
    if (touchPoints.count() == 1)
    {
      m_touchHandler->setTwoFingerTouchActive(false);
      const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
      QPoint PS0 = QPoint(touchPoint0.startPos().rx(), touchPoint0.startPos().ry());
      m_touchHandler->setStartPos(PS0, PS0);
      this->simulateMousePress(PS0);
      /*
      if (m_armForUserThreat)
      {
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            this->GetInteractor();
        if (renderWindowInteractor)
        {
          renderWindowInteractor->GetPicker()->Pick(renderWindowInteractor->GetEventPosition()[0],
              renderWindowInteractor->GetEventPosition()[1],
              0,  // always zero.
              renderWindowInteractor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

          double picked[3];
          renderWindowInteractor->GetPicker()->GetPickPosition(picked);
          QVector3D pickPoint(picked[0],
              picked[1],
              picked[2]);

          emit this->volumepointPickEvent(pickPoint);
          m_beginUserRegionSelection = true;
        }
        m_armForUserThreat = false;
      }
      */
    }
    break;
  case QEvent::TouchUpdate:
    if (touchPoints.count() == 1)
    {
      m_touchHandler->setTwoFingerTouchActive(false);

      const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
      QPoint PC0 = QPoint(touchPoint0.pos().rx(), touchPoint0.pos().ry());
      m_touchHandler->setCurrentPos(PC0, PC0);

      QPoint pMove = PC0;
      TouchProcResulst_E tpr = m_touchHandler->processOneFingerTouch(pMove);
      if (tpr == TPR_Rotate)
      {
       // qDebug() << "***** Doing a Rotate with simulateMouseMove(PMove) PMove(x,y)="
       // << pMove.rx() << ", " << pMove.ry();
        this->simulateMouseMove(pMove);
      }
    }
  case QEvent::TouchEnd:
  {
    if (touchPoints.count() == 2)
    {
      //---------------------------------------------
      // Determine if the two points are moving:
      // 1. Closer together
      // 2. Further apart//  vtkRenderWindow* t_renWin = this->GetRenderWindow();
      //  if (t_renWin)
      //  {
      //    m_irenAdapter->ProcessEvent(&releaseEvent,
      //                                t_renWin->GetInteractor());
      //  }

      // QVTKOpenGLWidget::mouseReleaseEvent(&releaseEvent);
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
      //  vtkRenderWindow* t_renWin = this->GetRenderWindow();
      //  if (t_renWin)
      //  {
      //    m_irenAdapter->ProcessEvent(&releaseEvent,
      //                                t_renWin->GetInteractor());
      //  }

      // QVTKOpenGLWidget::mouseReleaseEvent(&releaseEvent);
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
        m_touchHandler->performTouchPinch();
        m_touchHandler->setStartPos(PC0, PC1);
      }
      if (tpr == TPR_Spread)
      {
        m_touchHandler->performTouchSpread();
        m_touchHandler->setStartPos(PC0, PC1);
      }
      if (tpr == TPR_Move)
      {
        m_touchHandler->performTouchMove(deltaX, deltaY);
        m_touchHandler->setStartPos(PC0, PC1);
      }
      if (touchEvent->touchPointStates() & Qt::TouchPointReleased)
      {
        // If one of the fingers is released, set active two finger touch to false,


        bool wasTwoFingers = m_touchHandler->setTwoFingerTouchActive(false);
        if (wasTwoFingers)
        {
          const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.last();

          QPoint PC0 = QPoint(touchPoint0.pos().rx(), touchPoint0.pos().ry());
          // qDebug() << "Release Pos (x,y):" << PC0.rx() << ", " << PC0.ry();
          this->simulateMousePress(PC0);
          // qDebug() << "Mouse Release wasTwoFingers = true!!!";
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
        const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
        QPoint PC0 = QPoint(touchPoint0.pos().rx(), touchPoint0.pos().ry()  + 1);
        this->simulateMouseRelease(PC0);

        emit this->interactorActivityEnd();
        /*
        if (m_beginUserRegionSelection)
        {
          this->simulateMousePress(PC0);
          this->simulateMouseRelease(PC0);
          m_beginUserRegionSelection = false;
        }
        */
      }
    }
  }
    break;
  default:
    return QVTKOpenGLWidget::event(event);
  }
  return true;
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::onIntractionEndTimeout()
{
 this->showSplash();
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::onEndMultiTouchIntraction()
{
 this->showSplash();
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::onMultiTouchEnd()
{
  QTimer::singleShot(g_interactionEndSplahTimeout,
                     this,
                      &OpenGLVolumeWidget::onIntractionEndTimeout);
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::yawView(vtkSmartPointer<vtkRenderer> aRenderer,
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
void OpenGLVolumeWidget::rollView(vtkSmartPointer<vtkRenderer> aRenderer,
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
void OpenGLVolumeWidget::pitchView(vtkSmartPointer<vtkRenderer> aRenderer,
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
void OpenGLVolumeWidget::finalize()
{
  //-----------------------------------------------------------
  // ARO TESTING
   m_distanceRep =  vtkSmartPointer<vtkDistanceRepresentation3D>::New();
   m_distanceRep->SetLabelFormat("%-#6.3g cm");
   m_distanceRep->SetScale(0.1);


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
    m_distanceRep->SetHandleRepresentation(handleRep);
  }

 // m_distanceRep->InstantiateHandleRepresentation();

  // The Glyphs are the tick marks.
  m_distanceRep->GetGlyphActor()->GetProperty()->SetColor(MEASURE_RULER_GLYPH_COLOR_RED,
                                                          MEASURE_RULER_GLYPH_COLOR_GREEN,
                                                          MEASURE_RULER_GLYPH_COLOR_BLUE);



   //-------------------------------------------------------------
   // Ruler Text Properites
   // double textScale[3] = {20, 20, 20};
   // m_distanceRep->SetLabelScale(textScale);
  vtkAxisActor* testAct = reinterpret_cast<vtkAxisActor*>(m_distanceRep->GetLabelActor());
  if (testAct)
  {
    testAct->GetProperty()->SetColor(MEASURE_RULER_TEXT_COLOR_RED,
                                     MEASURE_RULER_TEXT_COLOR_GREEN,
                                     MEASURE_RULER_TEXT_COLOR_BLUE);
  }

  // double lablePos = m_distanceRep->GetLabelPosition();
  // std::cout << "LABEL Position:" << lablePos << std::endl;
  m_distanceRep->SetLabelPosition(1.0);
  //-------------------------------------------------------------


  vtkSmartPointer<vtkCallbackCommand> myCommandCallback =
      vtkSmartPointer<vtkCallbackCommand>::New();
  myCommandCallback->SetCallback(myCallbackFunction);

  m_distanceWidget = vtkSmartPointer<vtkDistanceWidget>::New();

   vtkRenderWindow* t_renWin = this->GetRenderWindow();

  m_distanceWidget->SetInteractor(t_renWin->GetInteractor());

  m_distanceWidget->SetRepresentation(m_distanceRep);
  m_distanceWidget->SetWidgetStateToManipulate();

  m_distanceWidget->AddObserver(vtkCommand::LeftButtonPressEvent, myCommandCallback);

  //-----------------------------------------------------------
}

//------------------------------------------------------------------------------
void OpenGLVolumeWidget::armMeasuringRuler(QVector3D& p0,
                                     QVector3D& p1)
{
  double arrayP0[3] = {p0.x(), p0.y(), p0.z()};
  double arrayP1[3] = {p1.x(), p1.y(), p1.z()};
  m_distanceWidget->On();
  m_distanceWidget->SetWidgetStateToStart();
  m_distanceRep->SetPoint1WorldPosition(arrayP0);
  m_distanceRep->SetPoint2WorldPosition(arrayP1);
  m_distanceWidget->On();
}


//------------------------------------------------------------------------------
void OpenGLVolumeWidget::disarmMeasuringRuler(QVector3D& curP0,
                                              QVector3D& curP1)
{
  double arrayP0[3];
  double arrayP1[3];

  m_distanceWidget->Off();
  m_distanceRep->GetPoint1WorldPosition(arrayP0);
  m_distanceRep->GetPoint2WorldPosition(arrayP1);


  curP0.setX(arrayP0[0]);
  curP0.setY(arrayP0[1]);
  curP0.setZ(arrayP0[2]);

  curP1.setX(arrayP1[0]);
  curP1.setY(arrayP1[1]);
  curP1.setZ(arrayP1[2]);

  double testScale = m_distanceRep->GetScale();
  std::cout << "CURRENT SCALE B At DISABLE:" << testScale << std::endl;
}
