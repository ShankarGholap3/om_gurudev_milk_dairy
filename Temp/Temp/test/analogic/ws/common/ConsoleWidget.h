//-------------------------------------------------------------------------------
// File: ConsoleWidget.h
// Description: Header file for class that uses a QPlainTextEdit as a console.
// Copyright 2016 Analogic corp.
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_COMMON_CONSOLEWIDGET_H_
#define TEST_ANALOGIC_WS_COMMON_CONSOLEWIDGET_H_
//-------------------------------------------------------------------------------

#include <QPlainTextEdit>
#include <QTextBlock>


class ConsoleWidget : public QObject
{
  Q_OBJECT
public:
  explicit ConsoleWidget(QPlainTextEdit* textEdit);
  virtual ~ConsoleWidget();
  void setFixedFont(int fontSize);
  void setScrollOn(bool setVal);
  void clear();
  void setCurserAtTop();
  void setCurserAtEnd();
  void logString(const QString& logStr);
  void search(QString searchStr);

 //------------------
 // ARO-Testing
 void setFocus();


private:
  QPlainTextEdit* m_textEdit;
  bool            m_scrollOn;
};


//-------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_COMMON_CONSOLEWIDGET_H_
//-------------------------------------------------------------------------------
