//------------------------------------------------------------------------------
// File: InputDialog3D.cpp
// Description:  Impleimentatrion of dialog for inputing 3D data.
// (c) Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------
#include "InputDialog3D.h"

#define INPUT_DIALOG_3D_TITLE  "Input 3D Value Dialog"
#define INPUT_DIALOG_3D_HEADER "Input 3D Values"
#define INPUT_DIALOG_3D_X_VAL  "X Value"
#define INPUT_DIALOG_3D_Y_VAL  "Y Value"
#define INPUT_DIALOG_3D_Z_VAL  "Z Value"

#define INPUT_DAILOG_3D_WIDTH  250
#define INPUT_DAILOG_3D_HEIGHT 200

//------------------------------------------------------------------------------
InputDialog3D::InputDialog3D(QWidget *parent, Qt::WindowFlags flags):
  QDialog(parent, flags)
{
  QVBoxLayout*  mainLayout   = new QVBoxLayout();
  QHBoxLayout*  layoutX       = new QHBoxLayout();
  QHBoxLayout*  layoutY       = new QHBoxLayout();
  QHBoxLayout*  layoutZ       = new QHBoxLayout();
  QHBoxLayout*  layoutButtons = new QHBoxLayout();

  m_labelHeader =  new QLabel(INPUT_DIALOG_3D_HEADER, this);
  m_labelX       = new QLabel(INPUT_DIALOG_3D_X_VAL,  this);
  m_labelY       = new QLabel(INPUT_DIALOG_3D_Y_VAL,  this);
  m_labelZ       = new QLabel(INPUT_DIALOG_3D_Z_VAL,  this);
  m_lineEditX    = new QLineEdit(this);
  m_lineEditY    = new QLineEdit(this);
  m_lineEditZ    = new QLineEdit(this);
  m_okButton     = new QPushButton("OK",     this);
  m_cancelButton = new QPushButton("Cancel", this);

  layoutX->addWidget(m_labelX);
  layoutX->addWidget(m_lineEditX);

  layoutY->addWidget(m_labelY);
  layoutY->addWidget(m_lineEditY);

  layoutZ->addWidget(m_labelZ);
  layoutZ->addWidget(m_lineEditZ);

  layoutButtons->addWidget(m_okButton);
  layoutButtons->addWidget(m_cancelButton);

  this->setLayout(mainLayout);
  mainLayout->addWidget(m_labelHeader);
  mainLayout->addLayout(layoutX);
  mainLayout->addLayout(layoutY);
  mainLayout->addLayout(layoutZ);
  mainLayout->addLayout(layoutButtons);
  this->setMinimumWidth(INPUT_DAILOG_3D_WIDTH);
  this->setTitleAndHeader(INPUT_DIALOG_3D_TITLE,
                          INPUT_DIALOG_3D_HEADER);
  this->connect(m_okButton, SIGNAL(clicked()), this,
                SLOT(accept()));

  this->connect(m_cancelButton, SIGNAL(clicked()), this,
                SLOT(reject()));
}

//------------------------------------------------------------------------------
InputDialog3D::~InputDialog3D()
{
}

//------------------------------------------------------------------------------
QSize InputDialog3D::sizeHint()
{
   return QSize(INPUT_DAILOG_3D_WIDTH,
                INPUT_DAILOG_3D_HEIGHT);
}

//------------------------------------------------------------------------------
void InputDialog3D::disableZ()
{
 m_lineEditZ->hide();
 m_labelZ->hide();
}

//------------------------------------------------------------------------------
void InputDialog3D::setValues(double vx, double vy, double vz)
{
  m_lineEditX->setText(QString::number(vx));
  m_lineEditY->setText(QString::number(vy));
  m_lineEditZ->setText(QString::number(vz));
}

//------------------------------------------------------------------------------
void InputDialog3D::getValues(double& vx, double& vy, double& vz)
{
  vx = m_lineEditX->text().toDouble();
  vy = m_lineEditY->text().toDouble();
  vz = m_lineEditZ->text().toDouble();
}

//------------------------------------------------------------------------------
void InputDialog3D::setTitleAndHeader(QString title, QString header)
{
  m_labelHeader->setText(header);
  this->setWindowTitle(title);
}

//------------------------------------------------------------------------------
void InputDialog3D::setLabels(QString strX, QString strY, QString strZ)
{
  m_labelX->setText(strX);
  m_labelY->setText(strY);
  m_labelZ->setText(strZ);
}
