//------------------------------------------------------------------------------
// File: ConsoleWidget.cpp
// Description: Implementation of Console Widget class.
// Copyright 2016 Analogic corp.
//------------------------------------------------------------------------------
#include "ConsoleWidget.h"


const char fixedFontStyle[]  = "Courier New";

const QColor g_searchColor(Qt::yellow);

//------------------------------------------------------------------------------
ConsoleWidget::ConsoleWidget(QPlainTextEdit* textEdit):
  QObject(NULL),
  m_textEdit(textEdit),
  m_scrollOn(false)
{
}

//------------------------------------------------------------------------------
ConsoleWidget::~ConsoleWidget()
{
}

//------------------------------------------------------------------------------
void ConsoleWidget::setFixedFont(int fontSize)
{
  QFont fixedFont = QFont();
  fixedFont.setFamily(fixedFontStyle);
  fixedFont.setFixedPitch(true);
  fixedFont.setPointSize(fontSize);
  m_textEdit->setFont(fixedFont);
}

//------------------------------------------------------------------------------
void ConsoleWidget::setScrollOn(bool setVal)
{
  m_scrollOn = setVal;
}

//------------------------------------------------------------------------------
void ConsoleWidget::clear()
{
  m_textEdit->clear();
}


//------------------------------------------------------------------------------
void ConsoleWidget::setCurserAtTop()
{
  int ln = 1;
  QTextCursor cursor(m_textEdit->document()->findBlockByLineNumber(ln-1));
  m_textEdit->setTextCursor(cursor);
}

//------------------------------------------------------------------------------
void ConsoleWidget::setCurserAtEnd()
{
  int line = m_textEdit->document()->lineCount() - 1;
  QTextCursor cursor(m_textEdit->document()->findBlockByLineNumber(line));
  m_textEdit->setTextCursor(cursor);
}

//------------------------------------------------------------------------------
void ConsoleWidget::logString(const QString& logStr)
{
  m_textEdit->insertPlainText(logStr);
  m_textEdit->insertPlainText("\n");
  if (m_scrollOn) this->setCurserAtEnd();
}

//------------------------------------------------------------------------------
void ConsoleWidget::search(QString searchStr)
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if(!m_textEdit->isReadOnly())
  {
    m_textEdit->moveCursor(QTextCursor::Start);
    QColor color = g_searchColor;

    while(m_textEdit->find(searchStr))
    {
      QTextEdit::ExtraSelection extra;
      extra.format.setBackground(color);

      extra.cursor = m_textEdit->textCursor();
      extraSelections.append(extra);
    }
  }

  m_textEdit->setExtraSelections(extraSelections);
}


//------------------------------------------------------------------------------
void ConsoleWidget::setFocus()
{
  m_textEdit->setFocus();
}
