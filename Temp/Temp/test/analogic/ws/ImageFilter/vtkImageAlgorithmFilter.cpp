//------------------------------------------------------------------------------
// File: vtkImageAlgorithmFilter.cpp
// Description: VTK Custom Image filter
// Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------
#include "vtkImageAlgorithmFilter.h"

#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkDataObject.h>
#include <vtkSmartPointer.h>

vtkStandardNewMacro(vtkImageAlgorithmFilter);

int vtkImageAlgorithmFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                             vtkInformationVector **inputVector,
                                             vtkInformationVector *outputVector)
{
  // Get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // Get the input and ouptut
  vtkImageData *input = vtkImageData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkImageData *output = vtkImageData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkSmartPointer<vtkImageData> image =
    vtkSmartPointer<vtkImageData>::New();
  image->ShallowCopy(input);


  //----------------------------------
  //******************************
  // THIS IS THE DUMMY FILTER CODE
  for (int i=0; i< 100; i++)
  {
    for  (int j=0; j < 100; j++)
    {
      //  From header file ...
      //  virtual void SetScalarComponentFromDouble(int x, int y,
      //                     int z, int component, double v);
      image->SetScalarComponentFromDouble(i, j, 0, 0, 255);
    }
  }
  //******************************
  // ----------------------------------


  output->ShallowCopy(image);

  // Without these lines, the output will appear real but will not work as the input to any other filters
  int extent[6];
  input->GetExtent(extent);
  output->SetExtent(extent);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
               extent,
               6);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               extent,
               6);
  return 1;
}
