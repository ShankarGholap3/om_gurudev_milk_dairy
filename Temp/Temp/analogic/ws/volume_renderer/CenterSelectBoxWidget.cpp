//------------------------------------------------------------------------------
// File: CenterSelectBoxWidget.cpp
// Description: CenterSelectBoxWidget implementation
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "CenterSelectBoxWidget.h"

vtkStandardNewMacro(CenterSelectBoxWidget);

//----------------------------------------------------------------------------
CenterSelectBoxWidget::CenterSelectBoxWidget()
{
}

//----------------------------------------------------------------------------
CenterSelectBoxWidget::~CenterSelectBoxWidget()
{
}

//----------------------------------------------------------------------------
void CenterSelectBoxWidget::SizeHandles()
{
  double radiusEdge    = this->vtk3DWidget::SizeHandles(HANDLE_RADIUS_EDGE);
  double radiusCenter  = this->vtk3DWidget::SizeHandles(HANDLE_RADIUS_CENTER);
  for(int i = 0; i < 6; i++)
  {
    this->HandleGeometry[i]->SetRadius(radiusEdge);
  }
  HandleGeometry[6]->SetRadius(radiusCenter);

  //----------------------------------------
  // Not sure to keep this
  HandleProperty->SetOpacity(0.5);
  SelectedHandleProperty->SetOpacity(0.5);
  //----------------------------------------
}


