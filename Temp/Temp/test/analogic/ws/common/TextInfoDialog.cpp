//------------------------------------------------------------------------------
// File: TestInfoDialog.cpp
// Description: Implementation of Modal dialog for displaying a
//     sizable quantity of text.
// (c) Copyright 2018 Analogic Corp.
//------------------------------------------------------------------------------


#include <QFileInfo>
#include <QFileDialog>
#include <iostream>
#include <fstream>

#include "TextInfoDialog.h"



const char fixedFontStyle[]  = "Courier New";

QString g_defaultTextOutputFile = "AppTextOut.txt";
QString g_defaultFilter         = "Text File (*.txt);;";
QString m_defaultSavePath       = "/home/analogic/junk/";


//------------------------------------------------------------------------------
TextInfoDialog::TextInfoDialog(QWidget* parent,
                               Qt::WindowFlags  flags)
  :QDialog(parent, flags),
  m_textEdit(nullptr),
  m_okButton(nullptr),
  m_saveButton(nullptr),
  m_saveFile(g_defaultTextOutputFile)
{
  this->move(TEXT_INFO_DLG_POS_X,
            TEXT_INFO_DLG_POS_Y);

  QVBoxLayout*  mainLayout   = new QVBoxLayout();

  QHBoxLayout*  buttonLayout   = new QHBoxLayout();

  m_textEdit     = new QPlainTextEdit(nullptr);
  m_okButton     = new QPushButton("OK", nullptr);
  m_saveButton   = new QPushButton("Save", nullptr);
  m_okButton->setDefault(true);


  mainLayout->addWidget(m_textEdit);
  mainLayout->addLayout(buttonLayout);
  buttonLayout->addWidget(m_saveButton);
  buttonLayout->addWidget(m_okButton);

  this->setMinimumWidth(TEXT_INFO_DLG_WIDTH);
  this->setMinimumHeight(TEXT_INFO_DLG_HEIGHT);
  this->setLayout(mainLayout);

  this->setFixedFont(TEXT_INFO_DLG_FONT_SIZE);
  this->connect(m_okButton, &QPushButton::clicked,
                this, &TextInfoDialog::onOKPressed);
  this->connect(m_saveButton, &QPushButton::clicked,
                this, &TextInfoDialog::onSavePressed);
}

//------------------------------------------------------------------------------
TextInfoDialog::~TextInfoDialog()
{
}


//------------------------------------------------------------------------------
QSize TextInfoDialog::sizeHint()
{
   return QSize(TEXT_INFO_DLG_WIDTH,
                TEXT_INFO_DLG_HEIGHT);
}

//------------------------------------------------------------------------------
QSize TextInfoDialog::minimumSizeHint()
{
   return QSize(TEXT_INFO_DLG_WIDTH,
                TEXT_INFO_DLG_HEIGHT);
}

//------------------------------------------------------------------------------
void TextInfoDialog::setFixedFont(int fontSize)
{
  QFont fixedFont = QFont();
  fixedFont.setFamily(fixedFontStyle);
  fixedFont.setFixedPitch(true);
  fixedFont.setPointSize(fontSize);
  m_textEdit->setFont(fixedFont);
}

//------------------------------------------------------------------------------
void TextInfoDialog::setSaveFile(QString& saveFile)
{
  m_saveFile = saveFile;
}

//------------------------------------------------------------------------------
void TextInfoDialog::onOKPressed()
{
  this->close();
}

//-----------------------------------------------------------------------------
void TextInfoDialog::onSavePressed()
{
  QFileInfo  saveFileInfo(m_defaultSavePath);
  QString    filePath = saveFileInfo.absolutePath();
  QFileDialog newFileDlg(this,
                         "Save Text as ...",
                         filePath,
                         g_defaultFilter);
  newFileDlg.setAcceptMode(QFileDialog::AcceptSave);
  newFileDlg.selectFile(m_saveFile);

  bool dlgRet =  newFileDlg.exec();
  if (dlgRet)
  {
    QString saveFile = newFileDlg.selectedFiles().first();
    std::cout << "saving to" << saveFile.toLocal8Bit().data() <<  std::endl;
    this->saveTextToFile(saveFile);
  }
}

//------------------------------------------------------------------------------
void TextInfoDialog::setText(QString& dialogText)
{
  m_textEdit->insertPlainText(dialogText);
  m_okButton->setFocus();
}

//------------------------------------------------------------------------------
void TextInfoDialog::saveTextToFile(QString& saveFile)
{
  std::ofstream fileOut(saveFile.toLocal8Bit().data());
  QString sData = m_textEdit->toPlainText();
  std::string outStr = sData.toLocal8Bit().data();
  fileOut << outStr;
}


//------------------------------------------------------------------------------
void TextInfoDialog::setTitle(QString titleText)
{
   this->setWindowTitle(titleText);
}
