//-----------------------------------------------------------------------------
// File: RulerWidget.cpp
// Description: VTK based 2d RulerWidget implementation.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "RulerWidget.h"


QString g_unitStrNormalCentimeter = QObject::tr("cm");
QString g_unitStrNormalInch       = QObject::tr("in");


QString g_unitStrThreatCentimeter = QObject::tr("cm");
QString g_unitStrThreatInch       = QObject::tr("in");

char g_threatFormat[] = "%'.1f";


double g_threatLabelFactor = 1.2;
double g_threatFontFactor  = 1.5;



double g_rulerPosX0 = 0.125;
double g_rulerPosX1 = 0.875;


double g_rulerNormalPosY0 = 0.06;
double g_rulerNormalPosY1 = 0.06;


double g_rulerThreatPosY0 = 0.13;
double g_rulerThreatPosY1 = 0.13;


int    g_numMinorTicksNormal = 2;
int    g_numMinorTicksThreat = 0;

double g_rulerColorRed   = 0.3;
double g_rulerColorGreen = 0.3;
double g_rulerColorBlue  = 0.3;

double g_rulerDefRefValue = .8;

double g_convInchPerMilliMeter       = 0.0393701;
double g_convCentimeterPerMillimeter = 0.10;

int    g_threatFontSizeRelatioveToAxis = 1;


//------------------------------------------------------------------------------
RulerWidget::RulerWidget(vtkSmartPointer<vtkRenderer> renderer,
                         bool isThreat,
                         QString name):
  m_vtkRulerObject(nullptr),
  m_rendererRef(nullptr),
  m_unit(ru_Inch),
  m_currentDistance(1.0),
  m_initialDistance(0.5),
  m_isThreatRuler(isThreat),
  m_name(name)
{
  m_vtkRulerObject = vtkSmartPointer<vtkAxisActor2D>::New();

  m_vtkRulerObject->GetProperty()->SetColor(g_rulerColorRed,
                                   g_rulerColorGreen,
                                   g_rulerColorBlue);





  m_vtkRulerObject->AxisVisibilityOn();
  if (m_isThreatRuler)
  {
    int *size;
    vtkViewport* viewport = renderer;
    size = viewport->GetSize();
    qDebug() << "Threatwidget Viewport Get Size:" <<  size[0] << size[1];


    m_vtkRulerObject->SetPoint1(g_rulerPosX0,
                     g_rulerThreatPosY0);

    m_vtkRulerObject->SetPoint2(g_rulerPosX1,
                     g_rulerThreatPosY1);

    m_vtkRulerObject->SetNumberOfLabels(2);
    m_vtkRulerObject->SetNumberOfMinorTicks(g_numMinorTicksThreat);


    m_vtkRulerObject->SetLabelFactor(g_threatLabelFactor);
    m_vtkRulerObject->SetFontFactor(g_threatFontFactor);
    m_vtkRulerObject->SetLabelFormat(g_threatFormat);
  }
  else
  {
    m_vtkRulerObject->SetPoint1(g_rulerPosX0,
                     g_rulerNormalPosY0);

    m_vtkRulerObject->SetPoint2(g_rulerPosX1,
                     g_rulerNormalPosY1);

     m_vtkRulerObject->SetNumberOfMinorTicks(g_numMinorTicksNormal);
  }

  this->setUnitTitle();

  m_rendererRef = renderer.GetPointer();
  if (m_rendererRef)
  {
   m_rendererRef->AddViewProp(m_vtkRulerObject);
  }
}

//------------------------------------------------------------------------------
RulerWidget::~RulerWidget()
{
}



//------------------------------------------------------------------------------
void RulerWidget::setRulerUnitToInch()
{
  m_unit = ru_Inch;
  this->setUnitTitle();
}

//------------------------------------------------------------------------------
void RulerWidget::setRulerUnitToCentimeter()
{
  m_unit = ru_Centimeter;
  this->setUnitTitle();
}

//------------------------------------------------------------------------------
void RulerWidget::setName(QString& name)
{
  m_name  = name;
}

//------------------------------------------------------------------------------
QString RulerWidget::getName()
{
  return m_name;
}

//------------------------------------------------------------------------------
void RulerWidget::setRulerSize()
{
  double rulerPosY0;
  double rulerPosY1;
  if (!m_isThreatRuler)
  {
    rulerPosY0 = g_rulerNormalPosY0;
    rulerPosY1 = g_rulerNormalPosY1;
  }
  else
  {
    rulerPosY0 = g_rulerThreatPosY0;
    rulerPosY1 = g_rulerThreatPosY1;
  }

  vtkSmartPointer<vtkCoordinate> coordinate =
      vtkSmartPointer<vtkCoordinate>::New();
  coordinate->SetCoordinateSystemToNormalizedDisplay();
  coordinate->SetValue(g_rulerPosX0,
                       rulerPosY0,
                       0);
  double* val;
  val = coordinate->GetComputedWorldValue(m_rendererRef);
  double x0, y0, z0;

  x0 = val[0];
  y0 = val[1];
  z0 = val[2];



  coordinate->SetValue(g_rulerPosX1,
                       rulerPosY1,
                       0);

  val = coordinate->GetComputedWorldValue(m_rendererRef);

  double x1, y1, z1;

  x1 = val[0];
  y1 = val[1];
  z1 = val[2];


  //-------------------------------------------------------

  //----------------------------------------------------
  // ARO-NOTE: workaround for a pernicious vtk bug that
  //    I havent tracked down yet!
  if ( (z0  < -500) &&
       (z1  > 1000) )
  {
    z0 =   0;
    z1 = 796;
    // std::cout  << "***** VTK BUG  ISSUE z0 < -500 && z1 > 1000" << std::endl;
  }
  //----------------------------------------------------
  QVector3D r0(x0, y0, z0);
  QVector3D r1(x1, y1, z1);
  QVector3D distVec = r1 -= r0;

  double rulerDistance = distVec.length();

  m_currentDistance    = rulerDistance;


  double conversionValue = 1.0;

  if (m_unit == ru_Inch)
  {
    conversionValue = g_convInchPerMilliMeter;
  }
  else
  {
    conversionValue = g_convCentimeterPerMillimeter;
  }
  rulerDistance *= conversionValue;
  m_vtkRulerObject->SetRange(0, rulerDistance);
}

//------------------------------------------------------------------------------
double RulerWidget::getRulerSize()
{
  return m_currentDistance;
}

//------------------------------------------------------------------------------
rulerUnit RulerWidget::getRlerUnit()
{
  return m_unit;
}

//------------------------------------------------------------------------------
double RulerWidget::getRulerReference()
{
  return m_initialDistance;
}


//------------------------------------------------------------------------------
void RulerWidget::initZoom()
{
  m_initialDistance = m_currentDistance;
}

//------------------------------------------------------------------------------
double RulerWidget::getZoom()
{
  return m_initialDistance/m_currentDistance;
}


//------------------------------------------------------------------------------
void RulerWidget::hide()
{
  m_vtkRulerObject->TickVisibilityOff();
  m_vtkRulerObject->AxisVisibilityOff();
  m_vtkRulerObject->LabelVisibilityOff();
  m_vtkRulerObject->TitleVisibilityOff();
}

//------------------------------------------------------------------------------
void RulerWidget::show()
{
  m_vtkRulerObject->TickVisibilityOn();
  m_vtkRulerObject->AxisVisibilityOn();
  m_vtkRulerObject->LabelVisibilityOn();
  m_vtkRulerObject->TitleVisibilityOn();
}

//------------------------------------------------------------------------------
void RulerWidget::setUnitTitle()
{
  if (m_unit == ru_Inch)
  {
    if (m_isThreatRuler)
    {
      m_vtkRulerObject->SetTitle(g_unitStrThreatInch.toLocal8Bit().data());
    }
    else
    {
      m_vtkRulerObject->SetTitle(g_unitStrNormalInch.toLocal8Bit().data());
    }
  }
  else
  {
    if (m_isThreatRuler)
    {
      m_vtkRulerObject->SetTitle(g_unitStrThreatCentimeter.toLocal8Bit().data());
    }
    else
    {
      m_vtkRulerObject->SetTitle(g_unitStrNormalCentimeter.toLocal8Bit().data());
    }
  }
}


