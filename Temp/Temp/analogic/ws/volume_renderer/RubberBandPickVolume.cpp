//------------------------------------------------------------------------------
// File: RubberBandPickVolume.cpp
// Description: Rubber Band Pick Volume implementation
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "RubberBandPickVolume.h"
#include "vtkRenderWindowInteractor.h"

vtkStandardNewMacro(RubberBandPickVolume);

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

//----------------------------------------------------------------------------
RubberBandPickVolume::RubberBandPickVolume()
{
}

//----------------------------------------------------------------------------
RubberBandPickVolume::~RubberBandPickVolume()
{
}


//----------------------------------------------------------------------------
void RubberBandPickVolume::OnLeftButtonUp()
{
  vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
  QVector2D pStart(StartPosition[0],
                   StartPosition[1]);

  QVector2D pEnd(EndPosition[0],
                   EndPosition[1]);

  m_selectCallback(pStart,
                   pEnd,
                   m_caller);
}

//----------------------------------------------------------------------------
void RubberBandPickVolume::OnLeftButtonDown()
{
  vtkInteractorStyleRubberBandPick::OnLeftButtonDown();
}


void RubberBandPickVolume::OnChar()
{
  //--------------------
  // ARO-NOTE: For reference only
  // Do not promote in production code.
  /*
  switch (this->Interactor->GetKeyCode())
  {
  case 's':
  case 'S':
    // s toggles the rubber band selection mode for mouse button 1
    if (this->CurrentMode == VTKISRBP_ORIENT)
    {
      this->CurrentMode = VTKISRBP_SELECT;
    }
    else
    {
      this->CurrentMode = VTKISRBP_ORIENT;
    }
    break;
  default:
    this->Superclass::OnChar();
  }
  */
}

//----------------------------------------------------------------------------
void RubberBandPickVolume::arm()
{
  this->CurrentMode = VTKISRBP_SELECT;
}


//----------------------------------------------------------------------------
void RubberBandPickVolume::disArm()
{
  this->CurrentMode = VTKISRBP_ORIENT;
}


//----------------------------------------------------------------------------
bool RubberBandPickVolume::isArmed()
{
  if ( this->CurrentMode == VTKISRBP_SELECT ) return true;
  return false;
}


