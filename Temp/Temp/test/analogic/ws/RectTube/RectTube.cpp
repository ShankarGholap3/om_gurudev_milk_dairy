//------------------------------------------------------------------------------
// File: RectTube.cpp
// Description: Create a Rectanuglar TUBE  poly data.
// Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------

#include "RectTube.h"


vtkSmartPointer<vtkTriangleFilter> createRectTube(double width,
                                                  double height,
                                                  double length,
                                                  double marginTop,
                                                  double marginLeft,
                                                  double marginBottom,
                                                  double marginRight)
{
  double px, py, pz;

  //---------------------------------
  // So that we have a closed surfae
  // We need to keep P4 from being
  // the same as P0, hence epsilon.
  double epsilon = height/1.0E6;
  //---------------------------------

  QList<QVector3D> tubeWarpPoints;

  //-------------------------------------------
  // Build Warp
  pz = 0;

  // P0
  px = -1.0*(width/2  - marginLeft);
  py = -1.0*(height/2 - marginBottom);
  tubeWarpPoints.push_back(QVector3D(px, py, pz));


  // P1
  px = 1.0*(width/2  - marginRight);
  tubeWarpPoints.push_back(QVector3D(px, py, pz));


  // P2
  py = 1.0*(height/2  - marginTop);
  tubeWarpPoints.push_back(QVector3D(px, py, pz));


  // P3
  px = -1.0*(width/2  - marginLeft);
  tubeWarpPoints.push_back(QVector3D(px, py, pz));


  // P4
  py = -1.0*(height/2 - marginBottom) + epsilon;
  tubeWarpPoints.push_back(QVector3D(px, py, pz));


  // P5
  px = -1.0*width/2;
  py = -1.0*height/2;
  tubeWarpPoints.push_back(QVector3D(px, py, pz));

  // P6
  py = 1.0*height/2;
  tubeWarpPoints.push_back(QVector3D(px, py, pz));

  // P7
  px = 1.0*width/2;
  tubeWarpPoints.push_back(QVector3D(px, py, pz));


  // P8
  py = -1.0*height/2;
  tubeWarpPoints.push_back(QVector3D(px, py, pz));


  // P9
  px = -1.0*width/2;
  tubeWarpPoints.push_back(QVector3D(px, py, pz));

  int numPoints = tubeWarpPoints.size();
  //-------------------------------------------

  vtkSmartPointer<vtkPoints> pointsA = vtkSmartPointer<vtkPoints>::New();
  pointsA->SetNumberOfPoints(numPoints);

  for (int i = 0; i < numPoints; i++)
  {
    pointsA->InsertPoint(i, tubeWarpPoints.at(i).x(),
                         tubeWarpPoints.at(i).y(),
                         tubeWarpPoints.at(i).z());
  }


  vtkSmartPointer<vtkPolygon> poly =    vtkSmartPointer<vtkPolygon>::New();
  poly->GetPointIds()->SetNumberOfIds(numPoints);
  for (int i = 0; i < numPoints; i++)
  {
    poly->GetPointIds()->SetId(i, i);
  }

  // Create a cell array to store the quad in
  vtkSmartPointer<vtkCellArray> quads =
      vtkSmartPointer<vtkCellArray>::New();
  quads->InsertNextCell(poly);



  // Create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> polydataA =
      vtkSmartPointer<vtkPolyData>::New();

  // Add the points and quads to the dataset
  polydataA->SetPoints(pointsA);
  polydataA->SetPolys(quads);

  vtkSmartPointer<vtkWarpVector> warpVector =
      vtkSmartPointer<vtkWarpVector>::New();
  warpVector->SetInputData(polydataA);




  // Apply linear extrusion
  vtkSmartPointer<vtkLinearExtrusionFilter> extrude =
      vtkSmartPointer<vtkLinearExtrusionFilter>::New();
  extrude->SetInputConnection( warpVector->GetOutputPort());
  extrude->SetExtrusionTypeToNormalExtrusion();
  extrude->SetVector(0, 0, 1 );
  extrude->SetScaleFactor(length);
  extrude->Update();


  vtkSmartPointer<vtkTriangleFilter> triangleFilter =
        vtkSmartPointer<vtkTriangleFilter>::New();
    triangleFilter->SetInputConnection(extrude->GetOutputPort());
    triangleFilter->Update();

  return triangleFilter;
}
