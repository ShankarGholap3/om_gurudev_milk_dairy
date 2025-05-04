//------------------------------------------------------------------------------
// File: AboutDialog.cpp
// Description: Implementation of About Dialog
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

#include "AboutDialog.h"

QString g_dlgAboutText = "Test application to help understand<br>reading and writing <b>DICOS</b> Files.";

int g_dlgWidth  = 300;
int g_dlgHeight = 200;

//------------------------------------------------------------------------------
AboutDialog::AboutDialog(QWidget *parent, Qt::WindowFlags f):
  QDialog(parent, f)
{
  QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok);

  QPushButton* okBtn = bb->button(QDialogButtonBox::Ok);
  okBtn->setAutoDefault(true);
  okBtn->setDefault(true);


  QVBoxLayout* dlgLayout = new QVBoxLayout();

  QLabel* dlgImage = new QLabel("", nullptr);
  dlgImage->setPixmap(QPixmap(":images/DICOSAbout.png"));

  QLabel* dlgText = new QLabel(g_dlgAboutText, nullptr);


  dlgLayout->addWidget(dlgImage);
  dlgLayout->addWidget(dlgText);
  dlgLayout->addWidget(bb);





  this->setLayout(dlgLayout);

  this->connect(okBtn, &QPushButton::clicked,
                this, &QDialog::accept);
  this->setWindowTitle("DICOS Qt Test App.");

  this->setMinimumWidth(g_dlgWidth);
  this->setMinimumHeight(g_dlgHeight);
}

//------------------------------------------------------------------------------
AboutDialog::~AboutDialog()
{
}
