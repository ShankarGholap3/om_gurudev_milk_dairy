//------------------------------------------------------------------------------
// File: RulerWidget.h
// Description:  Header file for vtk 2D Ruler widget based on the
//   class vtkAxisActor2D.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_RULERWIDGET_H_
#define ANALOGIC_WS_VOLUME_RENDERER_RULERWIDGET_H_
//------------------------------------------------------------------------------
#include <QString>
#include <QDebug>
#include <QVector3D>
#include <vtkSmartPointer.h>
#include <vtkAxisActor2D.h>
#include <vtkRenderer.h>
#include <vtkProperty2D.h>
#include <vtkCamera.h>
#include <vtkTextProperty.h>

#define LEFT_RULER_NAME  "LEFT_RULER"
#define RIGHT_RULER_NAME "RIGHT_RULER"

#define TEST_RULER_NAME "LEFT_RULER"

enum rulerUnit
{
  ru_Inch         = 0,
  ru_Centimeter   = 1,
};

class RulerWidget
{
public:
  explicit RulerWidget(vtkSmartPointer<vtkRenderer> renderer,
                       bool isThreat,
                       QString name);

  virtual ~RulerWidget();
  void setRulerUnitToInch();
  void setRulerUnitToCentimeter();
  void setRulerSize();
  double getRulerSize();
  rulerUnit getRlerUnit();
  double getRulerReference();
  void setName(QString&  name);
  QString getName();
  void initZoom();
  double getZoom();
  void hide();
  void show();

private:
  void setUnitTitle();

  vtkSmartPointer<vtkAxisActor2D>       m_vtkRulerObject;
  vtkRenderer*                          m_rendererRef;
  rulerUnit                             m_unit;
  double                                m_currentDistance;
  double                                m_initialDistance;
  bool                                  m_isThreatRuler;
  QString                               m_name;
};
//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_RULERWIDGET_H_
//------------------------------------------------------------------------------
