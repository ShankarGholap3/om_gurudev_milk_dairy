//------------------------------------------------------------------------------
// File: AxisWidget.cpp
// Description: Implemenatatrion of coordinate axis widget.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

#include "AxisWidget.h"

std::vector<double> g_axisColorRGB = {0.53, 0.53, 0.53};


//------------------------------------------------------------------------------
AxisWidget::AxisWidget(vtkGenericOpenGLRenderWindow* renWin,
                       double viewportMinX, double viewportMinY,
                       double viewportMaxX, double viewportMaxY)
{
  vtkSmartPointer< vtkAxesActor > axes =
      vtkSmartPointer< vtkAxesActor >::New();


  axes->SetXAxisLabelText("x");
  axes->SetYAxisLabelText("y");
  axes->SetZAxisLabelText("z");


  m_orientationWidget = vtkSmartPointer< vtkOrientationMarkerWidget >::New();

  vtkRenderWindowInteractor* rwi = renWin->GetInteractor();
  m_orientationWidget->SetInteractor(rwi);

  m_orientationWidget->SetOutlineColor(g_axisColorRGB.at(0),
                                       g_axisColorRGB.at(1),
                                       g_axisColorRGB.at(2));

  m_orientationWidget->SetOrientationMarker(axes);

  m_orientationWidget->SetViewport(viewportMinX,
                                   viewportMinY,
                                   viewportMaxX,
                                   viewportMaxY);

  m_orientationWidget->EnabledOn();
  m_orientationWidget->InteractiveOff();

  //-------------------------------------------
  // To Enable interaction
  // m_orientationWidget->InteractiveOn();
  //-------------------------------------------
}

//------------------------------------------------------------------------------
AxisWidget::~AxisWidget()
{
}
