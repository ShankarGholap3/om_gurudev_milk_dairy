//------------------------------------------------------------------------------
// File: MainWindow.cpp
// Description: VTK Generic Qt Application use for testing
// Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------
#include <QDebug>
#include <QVector3D>
#include <QFileDialog>
#include <QInputDialog>
#include "ui_MainWindow.h"
#include "MainWindow.h"
#include "RectTube.h"

const QString g_mainWindowTitle = "VTK Rect Tube PolyData Generator";
const QString g_aboutText       = "Application that generates vtkPolyDataAlgorithm \n"
                                  "Qt & VTK\n"
                                  "This Application is a CMake Project\n"
                                  "Author: Andre R. Oughton \n"
                                  "(c) Analogic Corp -  2017";

QColor g_vtkBackgroundColor("#1E90FF");

const int g_mainWindowWidth  = 820;
const int g_mainWindowHeight = 820;

const int g_mainWindowPosX   = 50;
const int g_mainWindowPosY   = 50;

const int g_vtkWinWidth      = 800;
const int g_vtkWinHeight     = 800;


//------------------------------------------------------------------------------
MainWindow::MainWindow():
  ui(new Ui_MainWindow)
{
  ui->setupUi(this);
  this->setupFileMenu();
  this->setWindowTitle(g_mainWindowTitle);
  this->createVTKWindow();

  this->setWindowIcon(QIcon(":images/RectTubeIcon.png"));
  this->move(g_mainWindowPosX,
             g_mainWindowPosY);
  this->setMinimumWidth(g_mainWindowWidth);
  this->setMinimumHeight(g_mainWindowHeight);
  this->setMaximumWidth(g_mainWindowWidth);
  this->setMaximumHeight(g_mainWindowHeight);
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
}

//------------------------------------------------------------------------------
void MainWindow::onAbout()
{
  QMessageBox::information(this, g_mainWindowTitle, g_aboutText);
}


//------------------------------------------------------------------------------
void MainWindow::onCreateVTKWindow()
{
  // this->createVTKWindow();
}

//------------------------------------------------------------------------------
void MainWindow::createVTKWindow()
{
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  if (!m_renderer)
  {
    QMessageBox::information(this,
                             "Error!",
                             "Can't Create Vtk Renderer!!!!");
    this->close();
  }

  m_renderer->SetBackground(g_vtkBackgroundColor.red()/255.0,
                            g_vtkBackgroundColor.green()/255.0,
                            g_vtkBackgroundColor.blue()/255.0);

  m_qvtkWidget = new QVTKWidget(ui->centralwidget);
  m_qvtkWidget->setMinimumWidth(g_vtkWinWidth);
  m_qvtkWidget->setMinimumHeight(g_vtkWinHeight);
  m_qvtkWidget->show();

  m_qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);



  double cubeWidth     = 5;
  double cubeHeight    = 5;
  double cubeLength    = 20;
  double topMargin     = 1;
  double leftMargin    = 2.45;
  double bottomMargin  = 1;
  double rightMargin   = 1;

  m_polyData = createRectTube(cubeWidth, cubeHeight, cubeLength,
                              topMargin, leftMargin, bottomMargin,
                              rightMargin);

  // Mapper and Actor  below
  vtkPolyDataMapper* cubeMapper = vtkPolyDataMapper::New();
  cubeMapper->SetInputConnection(m_polyData->GetOutputPort());


  vtkSmartPointer<vtkActor> cubeActor = vtkSmartPointer<vtkActor>::New();
  cubeActor->SetMapper(cubeMapper);
  m_renderer->AddActor(cubeActor);
  m_qvtkWidget->update();
  this->resetViewAlignment();
}

//------------------------------------------------------------------------------
void MainWindow::onExit()
{
  this->close();
}

//------------------------------------------------------------------------------
void MainWindow::setupFileMenu()
{
  this->connect(ui->actionExit, &QAction::triggered,
                this, &MainWindow::onExit);


  this->connect(ui->actionAbout, &QAction::triggered,
                this, &MainWindow::onAbout);
}


//------------------------------------------------------------------------------
void MainWindow::resetViewAlignment()
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
}



