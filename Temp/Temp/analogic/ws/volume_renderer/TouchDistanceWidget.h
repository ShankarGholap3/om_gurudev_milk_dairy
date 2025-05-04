//------------------------------------------------------------------------------
// File: TouchDistanceWidget.h
// Description: Touch Enablled Distance widget base on vtkDistanceWidget
// Copyright 2020 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef  ANALOGIC_WS_VOLUME_RENDERER_TOUCHDISTANCEWIDGET_H_
#define  ANALOGIC_WS_VOLUME_RENDERER_TOUCHDISTANCEWIDGET_H_
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include <vtkCommand.h>
#include <vtkDistanceWidget.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkObjectFactory.h>

class TouchDistanceWidget : public vtkDistanceWidget
{
public:
  static TouchDistanceWidget* New();
  virtual void RestartMeasurement();
  virtual void StopInteraction();
protected:
  TouchDistanceWidget();
  ~TouchDistanceWidget();
};

//------------------------------------------------------------------------------
#endif  //  ANALOGIC_WS_VOLUME_RENDERER_TOUCHDISTANCEWIDGET_H_
//------------------------------------------------------------------------------
