//------------------------------------------------------------------------------
// File: TestInfoDialog.h
// Description: Modal dialog for displaying a sizable quantity of text.
// (c) Copyright 2018 Analogic Corp.
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_COMMON_TEXTINFODIALOG_H_
#define TEST_ANALOGIC_WS_COMMON_TEXTINFODIALOG_H_
//------------------------------------------------------------------------------

#include <QDialog>
#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPlainTextEdit>


#define TEXT_INFO_DLG_WIDTH     700
#define TEXT_INFO_DLG_HEIGHT    800

#define TEXT_INFO_DLG_POS_X     20
#define TEXT_INFO_DLG_POS_Y     20

#define TEXT_INFO_DLG_FONT_SIZE 10

class TextInfoDialog : public QDialog
{
   Q_OBJECT
public:
  explicit TextInfoDialog(QWidget* parent,
                 Qt::WindowFlags  flags = 0);

  ~TextInfoDialog();
  virtual QSize sizeHint();
  virtual QSize minimumSizeHint();
  void setTitle(QString titleText);
  void setText(QString& dialogText);
  void setFixedFont(int fontSize);
  void setSaveFile(QString& saveFile);

public slots:
  void onOKPressed();
  void onSavePressed();

private:
  void saveTextToFile(QString& saveFile);


  QPlainTextEdit*  m_textEdit;
  QPushButton*     m_okButton;
  QPushButton*     m_saveButton;
  QString          m_saveFile;
};


//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_COMMON_TEXTINFODIALOG_H_
//------------------------------------------------------------------------------
