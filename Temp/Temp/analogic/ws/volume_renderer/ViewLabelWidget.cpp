//------------------------------------------------------------------------------
// File: ViewLabelWidget.cpp
// Purpose: Text Label widget implimentation
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "ViewLabelWidget.h"



const int g_labelFontSize  = 15;

const double g_labelColorRed   = 0.49;
const double g_labelColorGreen = 0.49;
const double g_labelColorBlue  = 0.49;

//------------------------------------------------------------------------------
ViewLabelWidget::ViewLabelWidget(vtkSmartPointer<vtkGenericOpenGLRenderWindow> pRenWin,
                                 vtkSmartPointer<vtkRenderer> pRenderer,
                                 QPoint displayPos1,
                                 QPoint displayPos2,
                                 QString viewLabel,
                                 bool isThreat):
 m_renderWindowRef(pRenWin.GetPointer()),
 m_rendererRef(pRenderer.GetPointer()),
 m_labelText(viewLabel),
 m_textActor(nullptr),
 m_fontSize(g_labelFontSize),
 m_displayPos1(displayPos1),
 m_displayPos2(displayPos2),
 m_isAThreatLabel(isThreat),
 m_canShow(true)
{
  // Setup the text and add it to the renderer
  m_textActor =     vtkSmartPointer<vtkTextActor>::New();
  m_textActor->SetInput( viewLabel.toLocal8Bit().data());
  m_textActor->SetDisplayPosition(m_displayPos1.x(),
                                  m_displayPos1.y());

  m_textActor->SetPosition2( m_displayPos2.x(),
                             m_displayPos2.y());

  m_textActor->GetTextProperty()->SetFontSize( m_fontSize );
  m_textActor->GetTextProperty()->SetColor( g_labelColorRed,
                                            g_labelColorGreen,
                                            g_labelColorBlue );
  m_textActor->GetTextProperty()->SetFontFamily(VTK_ARIAL);


  m_rendererRef->AddActor2D( m_textActor);
  m_renderWindowRef->Render();
}

//------------------------------------------------------------------------------
ViewLabelWidget::~ViewLabelWidget()
{
}

//------------------------------------------------------------------------------
void ViewLabelWidget::setZoomValue(double zoomVal)
{
    Q_UNUSED(zoomVal);
  if (m_isAThreatLabel) return;
//  QString stemp =   m_labelText;
//  if (m_canShow)
//  {
//    stemp += "Zoom: " + QString::number(zoomVal, 'g', 2);
//    stemp += " X";
//    m_textActor->SetInput(stemp.toLocal8Bit().data());
//  }
//  this->showLabel();
}



//------------------------------------------------------------------------------
void ViewLabelWidget::setLabel(QString strLabel)
{
  if (m_canShow)
  {
    m_labelText = strLabel;
    m_textActor->SetInput(m_labelText.toLocal8Bit().data());
  }
  this->showLabel();
}

//------------------------------------------------------------------------------
void ViewLabelWidget::moveDisplayPositionInY(int deltaY)
{
  int old_x1 = m_displayPos1.x();
  int old_y1 = m_displayPos1.y();

  int old_x2 = m_displayPos2.x();
  int old_y2 = m_displayPos2.y();

  QPoint newPos1    = QPoint(old_x1, (old_y1 + deltaY));
  QPoint newPos2    = QPoint(old_x2, (old_y2 + deltaY));


  m_textActor->SetDisplayPosition(newPos1.x(),
                                  newPos1.y());

  m_textActor->SetPosition2( newPos2.x(),
                             newPos2.y());
}

//------------------------------------------------------------------------------
void ViewLabelWidget::insertNewText(QStringList& list)
{
  if (m_canShow)
  {
    QString appendStr = "";

    for (int i = 0; i < list.size(); i++)
    {
      appendStr +=list[i] + "\n";
    }
    m_labelText += appendStr;

    m_textActor->SetInput(m_labelText.toLocal8Bit().data());
    this->moveDisplayPositionInY(-1.0*VIEW_TEXT_FONT_HEIGHT*list.size());
  }
  this->showLabel();
}

//------------------------------------------------------------------------------
void ViewLabelWidget::setTextPosition(QPoint& pos)
{
  m_textActor->SetDisplayPosition(pos.x(),
                                  pos.y());
}


//------------------------------------------------------------------------------
void ViewLabelWidget::enableShowText(bool doShow)
{
  m_canShow = doShow;
}

//------------------------------------------------------------------------------
void ViewLabelWidget::setTextSize(int fontSize)
{
  m_fontSize = fontSize;
  m_textActor->GetTextProperty()->SetFontSize( m_fontSize );
}

//------------------------------------------------------------------------------
void ViewLabelWidget::showLabel()
{
 // m_renderWindowRef->Render();
}
