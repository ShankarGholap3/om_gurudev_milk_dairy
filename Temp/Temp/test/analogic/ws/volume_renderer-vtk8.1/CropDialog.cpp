//------------------------------------------------------------------------------
// File: CropDialog.cpp
// Description: Crop Dialiog Implementation.
// Copyright 2016 Analogic corp.
//------------------------------------------------------------------------------
#include "CropDialog.h"
#include "ui_CropDialog.h"

int g_cropDialogWidth  = 400;
int g_cropDialogHeight = 200;

//------------------------------------------------------------------------------
CropDialog::CropDialog(QWidget *parent):
QDialog(parent),
ui(new Ui::Dialog)
{
  ui->setupUi(this);
  this->setMaximumWidth(g_cropDialogWidth);
  this->setMaximumHeight(g_cropDialogHeight);
}


//------------------------------------------------------------------------------
CropDialog::~CropDialog()
{
  delete ui;
}


//------------------------------------------------------------------------------
void CropDialog::setCropCorners(QVector3D& P0,
                    QVector3D& P1)
{
  ui->lineEditXmin->setText(QString::number(P0.x()));
  ui->lineEditYmin->setText(QString::number(P0.y()));
  ui->lineEditZmin->setText(QString::number(P0.z()));

  ui->lineEditXmax->setText(QString::number(P1.x()));
  ui->lineEditYmax->setText(QString::number(P1.y()));
  ui->lineEditZmax->setText(QString::number(P1.z()));
}


//------------------------------------------------------------------------------
void CropDialog::getCropCorners(QVector3D& P0,
                    QVector3D& P1)
{
  P0 = QVector3D(ui->lineEditXmin->text().toDouble(),
                 ui->lineEditYmin->text().toDouble(),
                 ui->lineEditZmin->text().toDouble());

  P1 = QVector3D(ui->lineEditXmax->text().toDouble(),
                 ui->lineEditYmax->text().toDouble(),
                 ui->lineEditZmax->text().toDouble());
}
