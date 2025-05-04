//------------------------------------------------------------------------------
// File: OpenGLVolumeWidget.h
// Purpose: Subclass of QVTKWidget to help fix initialization issues.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_OPENGLVOLUMEWIDGET_H_
#define ANALOGIC_WS_VOLUME_RENDERER_OPENGLVOLUMEWIDGET_H_
//------------------------------------------------------------------------------
#include <QApplication>
#include <QVTKWidget.h>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QPainter>
#include <QVTKOpenGLWidget.h>
#include <QVTKInteractorAdapter.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkInteractorStyle.h>

#include "Logger.h"
#include "RulerWidget.h"
#include "AxisWidget.h"
#include "ViewLabelWidget.h"
#include "OpenGLTouchHandler.h"

#include "MouseInteractorStylePP.h"
#include "RubberBandPickVolume.h"

#define VTK_ZOOM_OUT_LIMIT            -2
#define VTK_ZOOM_IN_LIMIT              8

#define VTK_THREAT_ZOOM_OUT_LIMIT     -2
#define VTK_THREAT_ZOOM_IN_LIMIT       4


#define DEFAULT_PINCH_THRESHOLD       15
#define DEFAULT_SPREAD_THRESHOLD      15
#define DEFAULT_MOVE_THRESHOLD        30

#define DEFAULT_SCALE_IN_THRESHOLD    0.8
#define DEFAULT_SCALE_OUT_THRESHOLD   1.2

#define DEF_ZOOM_DISP_LOWER_LIMIT     0.0
#define DEF_ZOOM_DISP_UPPER_LIMIT     1000.0


#define DEF_OPACITY_MODIFY_SCALAR     1.23
#define DEF_CONTRAST_MODIFY_SCALAR    3.0
#define DEF_BRIGHTNESS_MODIFY_SCALAR  4.0

#define VTK_MAIN_VIEWER_WIN_WIDTH     812
#define VTK_MAIN_VIEWER_WIN_HEIGHT    812
#define VTK_SPLASH_MARGIN_Y            85

#define TOUCH_LENGTH_DELTA_THRESHOLD   1.5

enum distanceMeasureStatus
{
  DISTANCE_MEASURE_INACTIVE     = 0,
  DISTANCE_MEASURE_START        = 1,
  DISTANCE_MEASURE_MOVE_HEAD    = 2,
  DISTANCE_MEASURE_TOUCH_ROTATE = 3,
};

//------------------------------------------------------------------------------
class OpenGLVolumeWidget : public QVTKOpenGLWidget
{
  Q_OBJECT
public:
  explicit OpenGLVolumeWidget(QWidget* parent,
                        Qt::WindowFlags flags = 0);
  virtual ~OpenGLVolumeWidget();

  virtual void paintGL();

  void setVTKRenderer(vtkRenderer* widgetRenderer);
  vtkRenderer* getVTKRenderer();


  void setRuler(RulerWidget* pRuler);
  void setViewLabel(ViewLabelWidget* pViewLabel);
  OpenGLTouchHandler* getTouchHandler();
  ViewLabelWidget* getViewLabel();

  void performZoom(int delta);
  void performWheelEvent(QWheelEvent* event);
  void performPan(int panX,
                  int panY);
  static void setZoomCount(int zoomCount,
                    int threatZoomCount);

  static void getZoomCount(int& zoomCount,
                    int& threatZoomCount);


  void enableSplash(bool doSplash);
  bool isSplashEenabled();
  void clearPaintCount();
  virtual void update();
  void setVolumeInitialized(bool volInit);
  void initThreatZoomIndex();
  void initZoomIndex();
  bool isVolumeInitialized();


  void armForVolumeMeasurement(bool armval);
  bool isArmedForVolumeMeasurement();

  void armForOperatorRubberBandSelect(bool armval);
  bool isArmedForRubberBandThreatSelect();

  static void mousePickCallback(QVector3D& aPoint, void* caller);

  static void rubberbandSelectCallback(QVector2D& p0,
                                       QVector2D& p1,
                                       void* caller);

  bool simulateVolumePick(QVector2D screenPoint,
                          QVector3D& retPick);

  QVector3D getPickPointWithPicker(QPoint screenPickPos);

  void pickMousePressAndRelease(QVector2D pickPoint);

  void setAsThreatViewer(bool setVal);
  bool isSplashAllowed();
  void setEnableInteraction(bool setVal);
  bool isInteractioEnabled();

  virtual void removeSplash() = 0;

  // Return true if we need to reset and false if not.
  virtual bool distanceMeasurePickPoint(QVector3D& worldPoint, bool bFirst) = 0;
  virtual void distanceMeasurePickPointTouch(QVector3D& worldPoint) = 0;
  virtual void distancMeasureZeroLengthAt(QVector2D& screenPoint) = 0;

  virtual bool isDistancToolStateManipulate() = 0;

  virtual bool isToucPointNearHandle(QPoint touchPoint, QPoint& retPoint) = 0;

  virtual void getViewCenter(QVector3D& viewCenter) = 0;

  virtual void logKeyStrokeEvent(QString keyStroke, QString alarm_type) = 0;
signals:
  void cameraPositionModified();
  void volumepointPickEvent(QVector3D& aPoint);
  void volumeMeasurePickPoint(QVector3D& worldPoint);


  void beginInteraction();
  void splashReady();
  void rubberBandSelected(QVector2D& p0,
                          QVector2D& p1);

  void interactorActivityEnd();

public slots:
  void onWheelTimeout();

protected:
  void allowSplashScreenOn();
  void allowSplashScreenOff();

  virtual void syncSibling() = 0;
  bool event(QEvent* event);
  void wheelEvent(QWheelEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void resizeEvent(QResizeEvent*  event);
  void paintEvent(QPaintEvent* event);

  //------------------------------------------
  // Debugging test
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);

  void setupPointPicker();
  void setupRubberBandPicker();
  void simulateTouchRelease();

  void doZoom(int wheelDelta,
              bool propagateToSiblings);


  void setDistanceMeasureStatus(distanceMeasureStatus newStatus);
  std::string distancMeasureStatusAsString(distanceMeasureStatus aStatus);
  void showDistancMeasureStatus(distanceMeasureStatus aStatus);
  void showDebugDistanceMeasure(const char* dbgString);
  void distanceMeasurementSetStart();

  virtual bool isDistanceMeasurementComplete() = 0;
  virtual void distanceMeasurementReset() = 0;
  virtual void distanceMeasurementEndInteraction()  = 0;
  virtual double distanceMeasurementCurrentLength() = 0;


  bool                     m_enableInteraction;
  static int               s_zoomCount;
  static int               s_zoomCountThreat;
  bool                     m_volumeInitialized;
  bool                     m_distanceMeasureToolActive;
  distanceMeasureStatus    m_distMeasureStatus;
  double                   m_distanceCurrentRulerLength;
  double                   m_distanceLastRulerLength;

private:
  void simulateMouseRelease(QPoint curPos);
  void simulateMouseReleaseWithInteraction(QPoint curPos);
  void simulateMousePress(QPoint curPos);
  void simulateMousePressWithInteraction(QPoint curPos);
  void simulateMouseMove(QPoint curPos);



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

  double getMouseWheelMotionFactor();
  void setMouseWheelMotionFactor(double scaleFactor);


  void distanceMeasureInputUpdate(bool isRealMouse);
  void volumeMeasureTouchUpdate(QPoint& touchPoint);
  void volumeMeasureMouseUpdate();

  bool                       m_allowSplash;
  QVTKInteractorAdapter*     m_irenAdapter;

  ViewLabelWidget*           m_viewLabel;
  RulerWidget*               m_ruler;

  bool                       m_touchRotate;

  //-----------------------
  // Splash Screen capabilities.
  bool m_enableSplash;
  int m_paintCount;
  //------------------------
  bool m_isThreatViewer;


  int  m_touchPinchCount;
  int  m_touchSpreadCount;

  int  m_touchMoveCount;
  int  m_touchMoveX;
  int  m_touchMoveY;

  QList<QPoint>         m_queuedTouchPoints;

  bool                  m_armForVolumeMeasurement;
  bool                  m_beginUserRegionSelection;


  QWidget*              m_parent;
  OpenGLTouchHandler*   m_touchHandler;
  RubberBandPickVolume* m_pickStyleRef;
  QTimer*               m_wheelCompleteTimer;
  vtkRenderer*          m_widgetRenderer;
};


//------------------------------------------------------------------------------
#endif  //  ANALOGIC_WS_VOLUME_RENDERER_OPENGLVOLUMEWIDGET_H_
//------------------------------------------------------------------------------
