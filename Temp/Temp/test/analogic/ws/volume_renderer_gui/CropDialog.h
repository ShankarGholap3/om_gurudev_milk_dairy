//------------------------------------------------------------------------------
// File: CropDialog.h
// Description: Crop Dialog header
// Copyright 2016 Analogic corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_CROPDIALOG_H_
#define TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_CROPDIALOG_H_
//------------------------------------------------------------------------------
#include <QDialog>
#include <QVector3D>

namespace Ui {
class Dialog;
}

class CropDialog : public QDialog
{
  Q_OBJECT
public:
  explicit CropDialog(QWidget *parent = 0);

  virtual ~CropDialog();

  void setCropCorners(QVector3D& P0,
                      QVector3D& P1);

  void getCropCorners(QVector3D& P0,
                      QVector3D& P1);


private:
  Ui::Dialog* ui;
};


//------------------------------------------------------------------------------
#endif  //  TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_CROPDIALOG_H_
//------------------------------------------------------------------------------
