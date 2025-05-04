//------------------------------------------------------------------------------
// File: RectTube.h
// Description: Header file for reading a Wavefront .obj files into a
//     vtkPolyDataAlgorithm
// (c) Analogic Corp -  2017
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef _RECT_TUBE_H_ 
#define _RECT_TUBE_H_ 
//------------------------------------------------------------------------------
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkCellArray.h>
#include <vtkRenderWindow.h>
#include <vtkTriangleFilter.h>
#include <vtkSmartPointer.h>
#include <vtkWarpVector.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkPolygon.h>
#include <string>
#include <QString>
#include <QList>
#include <QVector3D>



vtkSmartPointer<vtkTriangleFilter> createRectTube(double width,
                                                  double height,
                          double length,
                          double marginTop,
                          double marginLeft,
                          double marginBottom,
                          double marginRight);


//------------------------------------------------------------------------------
#endif // _RECT_TUBE_H_
//------------------------------------------------------------------------------
