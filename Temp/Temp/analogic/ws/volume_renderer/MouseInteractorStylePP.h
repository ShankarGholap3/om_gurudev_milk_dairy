//------------------------------------------------------------------------------
// File: MouseInteractorStylePP.h
// Description: Interactor class for volume select point
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_MOUSEINTERACTORSTYLEPP_H_
#define ANALOGIC_WS_VOLUME_RENDERER_MOUSEINTERACTORSTYLEPP_H_
//------------------------------------------------------------------------------

#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>
#include <vtkVolumePicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <QVector3D>


typedef void (*PickCallback)(QVector3D&, void*);

class MouseInteractorStylePP : public vtkInteractorStyleTrackballCamera
{
public:
    static MouseInteractorStylePP* New();

    vtkTypeMacro(MouseInteractorStylePP, vtkInteractorStyleTrackballCamera);

    void setCallback(PickCallback aCallback, void* caller)
    {
     m_pickCallback = aCallback;
     m_caller       = caller;
    }

    virtual void OnRightButtonDown()
    {
      this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
                         this->Interactor->GetEventPosition()[1],
                         0,  // always zero.
                         this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
      double picked[3];
      this->Interactor->GetPicker()->GetPickPosition(picked);

      QVector3D pickPoint(picked[0],
                          picked[1],
                          picked[2]);


      m_pickCallback(pickPoint, m_caller);

      vtkInteractorStyleTrackballCamera::OnRightButtonDown();
    }
private:
    PickCallback   m_pickCallback;
    void*          m_caller;
};

//------------------------------------------------------------------------------
#endif  //  ANALOGIC_WS_VOLUME_RENDERER_MOUSEINTERACTORSTYLEPP_H_
//------------------------------------------------------------------------------
