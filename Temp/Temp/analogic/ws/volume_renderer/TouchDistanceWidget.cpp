//------------------------------------------------------------------------------
// File: TouchDistanceWidget.cpp
// Description: Touch Based Distance Widget  implementation
// Copyright 2020 Analogic Corp.
//------------------------------------------------------------------------------
#include "TouchDistanceWidget.h"

vtkStandardNewMacro(TouchDistanceWidget);

//----------------------------------------------------------------------------
TouchDistanceWidget::TouchDistanceWidget()
{
}

//----------------------------------------------------------------------------
TouchDistanceWidget::~TouchDistanceWidget()
{
}

//----------------------------------------------------------------------------
void TouchDistanceWidget::RestartMeasurement()
{
  this->SetWidgetStateToStart();
}


//----------------------------------------------------------------------------
void TouchDistanceWidget::StopInteraction()
{
  this->EndDistanceInteraction(0);
  this->EndDistanceInteraction(1);
  this->EndSelectAction(this);
  this->ReleaseFocus();
}
