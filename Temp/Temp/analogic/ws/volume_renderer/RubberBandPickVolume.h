//------------------------------------------------------------------------------
// File: RubberBandPickVolume.h
// Description: Rubber Band Volume Selector.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef  ANALOGIC_WS_VOLUME_RENDERER_RUBBERBANDPICKVOLUME_H_
#define  ANALOGIC_WS_VOLUME_RENDERER_RUBBERBANDPICKVOLUME_H_
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkRenderer.h>
#include "vtkObjectFactory.h"
#include <QVector2D>

typedef void (*RubberBandCallback)(QVector2D&, QVector2D&, void*);

class RubberBandPickVolume : public vtkInteractorStyleRubberBandPick
{
public:
  static RubberBandPickVolume* New();
  vtkTypeMacro(RubberBandPickVolume, vtkInteractorStyleRubberBandPick);

  virtual void OnChar();
  virtual void OnLeftButtonUp();
  virtual void OnLeftButtonDown();
  virtual void arm();
  virtual void disArm();
  bool isArmed();


  void setCallback(RubberBandCallback aCallback, void* caller)
  {
   m_selectCallback = aCallback;
   m_caller         = caller;
  }

protected:
  RubberBandPickVolume();
  ~RubberBandPickVolume();
private:
    RubberBandCallback   m_selectCallback;
    void*                m_caller;
};

//------------------------------------------------------------------------------
#endif  //  ANALOGIC_WS_VOLUME_RENDERER_RUBBERBANDPICKVOLUME_H_
//------------------------------------------------------------------------------
