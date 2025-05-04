//------------------------------------------------------------------------------
// File:  MarginWidget.h
// Porpose: Testbed for making a VTK 2D Margin Widget.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_MARGINWIDGET_H_
#define ANALOGIC_WS_VOLUME_RENDERER_MARGINWIDGET_H_
//------------------------------------------------------------------------------

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkContextItem.h"
#include "vtkContextActor.h"
#include "vtkContextScene.h"
#include "vtkPen.h"
#include "vtkBrush.h"
#include "vtkTextProperty.h"
#include "vtkOpenGLContextDevice2D.h"
#include "vtkStdString.h"

//----------------------------------------------------------------------------
class MarginWidget: public vtkContextItem
{
public:
  static MarginWidget* New();
  vtkTypeMacro(MarginWidget, vtkContextItem);


  // Paint event for the object, called whenever the chart needs to be drawn
  // Note: "Capital P" in method name this is a VTK naming convention.
  virtual bool Paint(vtkContext2D* painter);

  void setColor(int r, int g, int b);
  void setRect(int x0, int y0, int x1, int y1);

protected:
    MarginWidget();
    ~MarginWidget();

private:
  int m_x0;
  int m_x1;
  int m_y0;
  int m_y1;
  int m_penRed;
  int m_penGreen;
  int m_penBlue;
};

//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_MARGINWIDGET_H_
//------------------------------------------------------------------------------
