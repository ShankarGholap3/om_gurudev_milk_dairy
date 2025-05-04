//------------------------------------------------------------------------------
// File: SceneWidget.h
// Description: Header file for VTK 8.1 Volunme Renderer Scene Handlear
//
// Copyright 2018 Analogic Corp.
//------------------------------------------------------------------------------



#ifndef TEST_ANALOGIC_WS_VOLUME_RENDERER_VTK8_1_SCENEWIDGET_H_
#define TEST_ANALOGIC_WS_VOLUME_RENDERER_VTK8_1_SCENEWIDGET_H_

#include <QVTKOpenGLWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkDataSet.h>

class SceneWidget : public QVTKOpenGLWidget
{
  Q_OBJECT
public:
  explicit SceneWidget(QWidget *parent = 0);

  //! Add a data set to the scene
  /*!
    \param[in] dataSet The data set to add
  */
  void addDataSet(vtkSmartPointer<vtkDataSet> dataSet);

  //! Remove the data set from the scene
  void removeDataSet();

public slots:
  //! Zoom to the extent of the data set in the scene
  void zoomToExtent();

private:
  vtkSmartPointer<vtkRenderer> m_renderer;
};

#endif  // TEST_ANALOGIC_WS_VOLUME_RENDERER_VTK8_1_SCENEWIDGET_H_

