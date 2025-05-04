//------------------------------------------------------------------------------
// File: CenterSelectBoxWidget.h
// Description: Center Select Box Widget Header
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef  ANALOGIC_WS_VOLUME_RENDERER_CENTERSELECTBOXWIDGET_H_
#define  ANALOGIC_WS_VOLUME_RENDERER_CENTERSELECTBOXWIDGET_H_
//------------------------------------------------------------------------------

#define HANDLE_RADIUS_EDGE        0.0125
#define HANDLE_RADIUS_CENTER      1.5
//------------------------------------------------------------------------------
#include <vtkBoxWidget.h>
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkObjectFactory.h"
class CenterSelectBoxWidget : public vtkBoxWidget
{
public:
  static CenterSelectBoxWidget* New();
  virtual void SizeHandles();

protected:
  CenterSelectBoxWidget();
  ~CenterSelectBoxWidget();
};

//------------------------------------------------------------------------------
#endif  //  ANALOGIC_WS_VOLUME_RENDERER_CENTERSELECTBOXWIDGET_H_
//------------------------------------------------------------------------------
