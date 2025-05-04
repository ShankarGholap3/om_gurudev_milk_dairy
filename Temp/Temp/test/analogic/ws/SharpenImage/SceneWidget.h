//------------------------------------------------------------------------------
// File: SceneWidget.h
// Description: Scene Widget header file
// Copyright 2018 Analogic corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_SHARPENIMAGE_SCENEWIDGET_H_
#define TEST_ANALOGIC_WS_SHARPENIMAGE_SCENEWIDGET_H_
//------------------------------------------------------------------------------

#include <QVTKOpenGLWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkDataSet.h>
#include <vtkImageData.h>

#define SCENE_WIDGET_BG_RED    0.3
#define SCENE_WIDGET_BG_GREEN  0.3
#define SCENE_WIDGET_BG_BLUE   0.3

class SceneWidget : public QVTKOpenGLWidget
{
  Q_OBJECT
public:
  explicit SceneWidget(QWidget *parent = 0);

  ~SceneWidget();

  void doRender();
  void setWindowBackGround(double winRed,
                           double winGreen,
                           double winBlue);


  // Add a data set to the scene
  // param[in] dataSet The data set to add
  void addDataSet(vtkSmartPointer<vtkDataSet> dataSet);

  // Add an image to the scene
  void addImage(vtkSmartPointer<vtkImageData> imageData);

  vtkRenderer* getRenderer();

  // Remove the data set from the scene
  void removeDataSet();
  void removeImages();
  void resetView();
  void performZoom(int delta);



public slots:
  // Zoom to the extent of the data set in the scene
  void zoomToExtent();

private:
  vtkSmartPointer<vtkRenderer> m_renderer;
  QVTKInteractorAdapter*       m_irenAdapter;
};

//------------------------------------------------------------------------------
#endif  //  TEST_ANALOGIC_WS_SHARPENIMAGE_SCENEWIDGET_H_
//------------------------------------------------------------------------------
