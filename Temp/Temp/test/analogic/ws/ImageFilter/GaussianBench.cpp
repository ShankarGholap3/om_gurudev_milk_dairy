//--------------------------------------------------------------------------
// Test Application used to benchmark vtk algorythims.
// Written by Andre R. Oughton - 8-21-2017
// Copyright 2017 Analogic Corp.
//--------------------------------------------------------------------------
#include <QTime>
#include <QDebug>
#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkImageData.h>
#include <vtkImageCast.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include "vtkImageAlgorithmFilter.h"

std::string filename_in  = "/home/analogic/junk/FilterInputGray.png";
std::string filename_out = "/home/analogic/junk/FilterOutputGray.png";

int main(int argc, char* argv[])
{
  Q_UNUSED(argc)
  Q_UNUSED(argv)

  // Read the image
  vtkSmartPointer<vtkPNGReader> reader =
      vtkSmartPointer<vtkPNGReader>::New();
  reader->SetFileName(filename_in.c_str());
  reader->Update();

  vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
  imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  imageData = reader->GetOutput();

  vtkSmartPointer<vtkImageCast> castFilter = vtkSmartPointer<vtkImageCast>::New();
  castFilter->SetInputConnection(reader->GetOutputPort());
  castFilter->SetOutputScalarTypeToUnsignedChar();
  castFilter->Update();

  QTime timerGausian = QTime::currentTime();


  //----------------------------------------------------
  vtkSmartPointer<vtkImageAlgorithmFilter> testFilter =
      vtkSmartPointer<vtkImageAlgorithmFilter>::New();
  testFilter->SetInputConnection(castFilter->GetOutputPort());
  testFilter->Update();
  //----------------------------------------------------


  // Visualize
  vtkSmartPointer<vtkImageViewer2> imageViewer =
      vtkSmartPointer<vtkImageViewer2>::New();
  imageViewer->SetInputConnection(testFilter->GetOutputPort());

  int filterElapsed = timerGausian.elapsed();




  qDebug() << "Custom Filter elapsed time (ms)" << filterElapsed;

  vtkSmartPointer<vtkPNGWriter> writer =
      vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName(filename_out.c_str());
  writer->SetInputConnection(testFilter->GetOutputPort());
  writer->Write();


  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
