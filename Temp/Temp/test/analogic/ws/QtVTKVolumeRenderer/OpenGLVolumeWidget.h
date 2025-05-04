//------------------------------------------------------------------------------
// File: OpenGLVolumeWidget.h
// Purpose: Subclass of QVTKOpenGLWidget to needed for Volume Rendering
//
// Copyright 2018 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_QTVTKVOLUMERENDERER_OPENGLVOLUMEWIDGET_H_
#define TEST_ANALOGIC_WS_QTVTKVOLUMERENDERER_OPENGLVOLUMEWIDGET_H_
//------------------------------------------------------------------------------
#include <vtkWindow.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageClip.h>
#include <vtkImageShiftScale.h>
#include <vtkImageMathematics.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKInteractorAdapter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageMapper.h>
#include <vtkImageViewer.h>
#include <vtkProperty.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkAxisActor.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkDistanceWidget.h>
#include <vtkCallbackCommand.h>
#include <vtkDistanceRepresentation3D.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkSphereHandleRepresentation.h>
#include <QVector3D>
#include <QVTKOpenGLWidget.h>
#include <QMouseEvent>
#include <QTouchEvent>
#include <QTimer>

#include "LocalTouchHandler.h"



#define VTK_MAIN_VIEWER_WIN_WIDTH     812
#define VTK_MAIN_VIEWER_WIN_HEIGHT    812
#define VTK_SPLASH_MARGIN_Y           100

#define DEFAULT_PINCH_THRESHOLD       15
#define DEFAULT_SPREAD_THRESHOLD      15
#define DEFAULT_MOVE_THRESHOLD        30

//---------------------------------------------------------------
// In Volume Distance Measurement:
#define MEASURE_RULER_HANDEL_SIZE_POINT     40
#define MEASURE_RULER_HANDEL_SIZE_SPHERE    10

#define MEASURE_RULER_TEXT_COLOR_RED        0.19
#define MEASURE_RULER_TEXT_COLOR_GREEN      0.78
#define MEASURE_RULER_TEXT_COLOR_BLUE       0.30

#define MEASURE_RULER_LINE_COLOR_RED        0.19
#define MEASURE_RULER_LINE_COLOR_GREEN      0.88
#define MEASURE_RULER_LINE_COLOR_BLUE       0.88


#define MEASURE_RULER_HANDLE_COLOR_RED      0.84
#define MEASURE_RULER_HANDLE_COLOR_GREEN    0.10
#define MEASURE_RULER_HANDLE_COLOR_BLUE     0.10


#define MEASURE_RULER_GLYPH_COLOR_RED      0.95
#define MEASURE_RULER_GLYPH_COLOR_GREEN    0.10
#define MEASURE_RULER_GLYPH_COLOR_BLUE     0.10
//---------------------------------------------------------------



//------------------------------------------------------------------------------
class OpenGLVolumeWidget : public QVTKOpenGLWidget
{
  Q_OBJECT
public:
  explicit OpenGLVolumeWidget(QWidget* parent,
                        Qt::WindowFlags flags = 0);
  virtual ~OpenGLVolumeWidget();
  void computeWorldToDisplay(vtkRenderer* ren,
                        double x,
                        double y,
                        double z,
                        double* displayPt);

  void computeDisplayToWorld(vtkRenderer* ren,
                             double x,
                             double y,
                             double z,
                             double* worldPt);

  void setRenderer(vtkRenderer* aRenderer);
  void showSplash();
  void removeSplash();
  void performWheelEvent(QWheelEvent* event);
  void performZoomIn(bool propagatge);
  void performZoomOut(bool propagate);
  void performPan(int panX,
                  int panY);

  void resetViewAlignment();

  void yawView(vtkSmartPointer<vtkRenderer> aRenderer,
               double angle);


  void rollView(vtkSmartPointer<vtkRenderer> aRenderer,
                double angle);

  void pitchView(vtkSmartPointer<vtkRenderer> aRenderer,
                 double angle);

  void simulateMousePress(QPoint curPos);
  void simulateMouseRelease(QPoint curPos);
  void simulateMouseMove(QPoint curPos);
  void updateRender();
  void setRendererBGColor(double bgRed,
                          double bgGreen,
                          double bgBlue);


  void vtkMousePressEvent(QPoint mousePos);
  void vtkMouseReleaseEvent(QPoint mousePos);
  void removeVolume();

  //----------------------------------
  // Test method
  void finalize();
  void armMeasuringRuler(QVector3D& p0,
                         QVector3D& p1);

  void disarmMeasuringRuler(QVector3D& curP0,
                            QVector3D& curP1);


public slots:
  void onIntractionEndTimeout();
  void onEndMultiTouchIntraction();
  void onMultiTouchEnd();

signals:
  void interactorActivityEnd();

protected:
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);
  bool event(QEvent *event);


private:
  LocalTouchHandler*            m_touchHandler;
  vtkActor2D*                   m_imageSplashRef;
  vtkRenderer*                  m_rendererRef;
  QVTKInteractorAdapter*        m_irenAdapter;

//------------------------------------------------
// Testing  distance widget:
 vtkSmartPointer<vtkDistanceRepresentation3D> m_distanceRep;
 vtkSmartPointer<vtkAxisActor>                m_measureAxisActor;
 vtkSmartPointer<vtkDistanceWidget>           m_distanceWidget;
};


//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_QTVTKVOLUMERENDERER_OPENGLVOLUMEWIDGET_H_
//------------------------------------------------------------------------------
