//------------------------------------------------------------------------------
// File: ViewLabelWidget.h
// Purpose: Text Label widget for 3D render window
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_VIEWLABELWIDGET_H_
#define ANALOGIC_WS_VOLUME_RENDERER_VIEWLABELWIDGET_H_
//------------------------------------------------------------------------------

#include <QString>
#include <QStringList>
#include <QPoint>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#define DEFAULT_VIEW_LABEL_POSITION_X     50
#define DEFAULT_VIEW_LABEL_POSITION_Y    720
#define VIEW_TEXT_FONT_HEIGHT             21


class ViewLabelWidget
{
 public:
 explicit ViewLabelWidget(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renWin,
                          vtkSmartPointer<vtkRenderer> pRenderer,
                          QPoint displayPos1,
                          QPoint displayPos2,
                          QString viewLabel,
                          bool isThreat);
 virtual ~ViewLabelWidget();
 void setTextSize(int fontSize);
 void setLabel(QString strLabel);
 void insertNewText(QStringList& list);
 void moveDisplayPositionInY(int deltaY);
 void setZoomValue(double zoomVal);
 void setTextPosition(QPoint& pos);
 void enableShowText(bool doShow);


 vtkRenderWindow*                  m_renderWindowRef;
 vtkRenderer*                      m_rendererRef;

 private:
    void showLabel();
    QString                           m_labelText;
    vtkSmartPointer<vtkTextActor>     m_textActor;
    int                               m_fontSize;
    QPoint                            m_displayPos1;
    QPoint                            m_displayPos2;
    bool                              m_isAThreatLabel;
    bool                              m_canShow;
};

//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_VIEWLABELWIDGET_H_
//------------------------------------------------------------------------------
