//------------------------------------------------------------------------------
// File: MarginWidget.cpp
// Description: Implementation of the VTK 2D MarGineWidget class
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "MarginWidget.h"

//------------------------------------------------------------------------------
MarginWidget* MarginWidget::New()
{
  return new MarginWidget();
}


//------------------------------------------------------------------------------
MarginWidget::MarginWidget():
  m_x0(1),
  m_x1(100),
  m_y0(1),
  m_y1(100),
  m_penRed(255),
  m_penGreen(0),
  m_penBlue(0)
{
}

//------------------------------------------------------------------------------
MarginWidget::~MarginWidget()
{
}

//------------------------------------------------------------------------------
void MarginWidget::setColor(int r, int g, int b)
{
  m_penRed   = r;
  m_penGreen = g;
  m_penBlue  = b;
}


//------------------------------------------------------------------------------
void MarginWidget::setRect(int x0, int y0, int x1, int y1)
{
  m_x0 = x0;
  m_y0 = y0;
  m_x1 = x1;
  m_y1 = y1;
}



//------------------------------------------------------------------------------
bool MarginWidget::Paint(vtkContext2D *painter)
{
  painter->GetPen()->SetColor(m_penRed,
                              m_penGreen,
                              m_penBlue);

  //--------------------------
  // Bottom Hotizontal
  painter->DrawLine(m_x0, m_y0, m_x1, m_y0);
  // Left Vertical
  painter->DrawLine(m_x0, m_y0, m_x0, m_y1);
  // Top Horizontal
  painter->DrawLine(m_x0, m_y1, m_x1, m_y1);
  // Right Vertical
  painter->DrawLine(m_x1, m_y0, m_x1, m_y1);
  //--------------------------

  return true;
}


