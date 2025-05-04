//------------------------------------------------------------------------------
// File: vtkImageAlgorithmFilter.h
// Description: VTK Custom Image filter header
// Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_IMAGEFILTER_VTKIMAGEALGORITHMFILTER_H_
#define TEST_ANALOGIC_WS_IMAGEFILTER_VTKIMAGEALGORITHMFILTER_H_
//------------------------------------------------------------------------------

#include <vtkImageAlgorithm.h>

class vtkImageAlgorithmFilter : public vtkImageAlgorithm
{
public:
  static vtkImageAlgorithmFilter *New();
  vtkTypeMacro(vtkImageAlgorithmFilter, vtkImageAlgorithm);

  vtkImageAlgorithmFilter(){}

protected:
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);

private:
  vtkImageAlgorithmFilter(const vtkImageAlgorithmFilter&);  // Not implemented.
  void operator=(const vtkImageAlgorithmFilter&);  // Not implemented.
};


//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_IMAGEFILTER_VTKIMAGEALGORITHMFILTER_H_
//------------------------------------------------------------------------------
