//------------------------------------------------------------------------------
// File: SceneWidget.cpp
// Description: Scene Widget Implementation
// Copyright 2018 Analogic corp.
//------------------------------------------------------------------------------
#include "SceneWidget.h"
#include <vtkCamera.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkImageActor.h>
#include <QVTKInteractorAdapter.h>
#include <QWheelEvent>

//------------------------------------------------------------------------------
SceneWidget::SceneWidget(QWidget *parent)
  : QVTKOpenGLWidget(parent),
    m_irenAdapter(nullptr)
{
  vtkNew<vtkGenericOpenGLRenderWindow> window;
  SetRenderWindow(window.Get());


  m_irenAdapter   =  new QVTKInteractorAdapter(this);

  // Camera
  vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
  camera->SetViewUp(0, 1, 0);
  camera->SetPosition(0, 0, 10);
  camera->SetFocalPoint(0, 0, 0);

  // Renderer
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  m_renderer->SetActiveCamera(camera);
  m_renderer->SetBackground(SCENE_WIDGET_BG_RED,
                            SCENE_WIDGET_BG_GREEN,
                            SCENE_WIDGET_BG_BLUE);
  this->GetRenderWindow()->AddRenderer(m_renderer);
}

//------------------------------------------------------------------------------
SceneWidget::~SceneWidget()
{
}

//------------------------------------------------------------------------------
void SceneWidget::doRender()
{
  this->renderVTK();
}

//------------------------------------------------------------------------------
void SceneWidget::setWindowBackGround(double winRed,
                                      double winGreen,
                                      double winBlue)
{
  m_renderer->SetBackground(winRed, winGreen, winBlue);
  this->renderVTK();
}



//------------------------------------------------------------------------------
void SceneWidget::addDataSet(vtkSmartPointer<vtkDataSet> dataSet)
{
  // Actor
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

  // Mapper
  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
  mapper->SetInputData(dataSet);
  actor->SetMapper(mapper);

  m_renderer->AddActor(actor);
  m_renderer->ResetCamera(dataSet->GetBounds());


  this->renderVTK();
}

//------------------------------------------------------------------------------
vtkRenderer* SceneWidget::getRenderer()
{
  return m_renderer.GetPointer();
}

//------------------------------------------------------------------------------
void SceneWidget::addImage(vtkSmartPointer<vtkImageData> imageData)
{
  // Create actors
  vtkSmartPointer<vtkImageActor> imageActor =
       vtkSmartPointer<vtkImageActor>::New();
  imageActor->SetInputData(imageData);

  m_renderer->AddActor(imageActor);
  m_renderer->ResetCamera(imageData->GetBounds());

  this->renderVTK();
}

//------------------------------------------------------------------------------
void SceneWidget::removeImages()
{
  m_renderer->RemoveAllViewProps();
}

//------------------------------------------------------------------------------
void SceneWidget::removeDataSet()
{
  vtkActor *actor = m_renderer->GetActors()->GetLastActor();
  if (actor != nullptr)
  {
    m_renderer->RemoveActor(actor);
  }

  this->renderVTK();
}


//------------------------------------------------------------------------------
void SceneWidget::performZoom(int delta)
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
  this->renderVTK();
}

//------------------------------------------------------------------------------
void SceneWidget::zoomToExtent()
{
  // Zoom to extent of last added actor
  vtkSmartPointer<vtkActor> actor = m_renderer->GetActors()->GetLastActor();
  if (actor != nullptr)
  {
    m_renderer->ResetCamera(actor->GetBounds());
  }

  this->renderVTK();
}

//------------------------------------------------------------------------------
void SceneWidget::resetView()
{
  m_renderer->ResetCamera();
  double fp[3];
  double pos[3];
  fp[0] = m_renderer->GetActiveCamera()->GetFocalPoint()[0];
  fp[1] = m_renderer->GetActiveCamera()->GetFocalPoint()[1];
  fp[2] = m_renderer->GetActiveCamera()->GetFocalPoint()[2];
  pos[0] = m_renderer->GetActiveCamera()->GetPosition()[0];
  pos[1] = m_renderer->GetActiveCamera()->GetPosition()[1];
  pos[2] = m_renderer->GetActiveCamera()->GetPosition()[2];
  double dist = std::sqrt( (pos[0]- fp[0])*(pos[0] - fp[0]) +
      (pos[1]- fp[1])*(pos[1] - fp[1]) +
      (pos[2]- fp[2])*(pos[2] - fp[2]) );

  m_renderer->GetActiveCamera()->SetPosition(fp[0], fp[1], fp[2]+ dist);
  m_renderer->GetActiveCamera()->SetViewUp(0.0, 1.0, 0.0);
  this->renderVTK();
}

