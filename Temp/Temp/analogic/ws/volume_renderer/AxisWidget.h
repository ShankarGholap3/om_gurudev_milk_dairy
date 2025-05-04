//------------------------------------------------------------------------------
// File: AxisWidget.h
// Descrioption: Moveable 4D Axis Widget for vtk windows.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_AXISWIDGET_H_
#define ANALOGIC_WS_VOLUME_RENDERER_AXISWIDGET_H_
//------------------------------------------------------------------------------
#include <vector>
#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkTransform.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>



class AxisWidget
{
 public:
  AxisWidget(vtkGenericOpenGLRenderWindow* renWin,
             double viewportMinX, double viewportMinY,
             double viewportMaxX, double viewportMaxY);
  virtual ~AxisWidget();

 private:
  vtkSmartPointer<vtkOrientationMarkerWidget>  m_orientationWidget;
};


//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_AXISWIDGET_H_
//------------------------------------------------------------------------------
