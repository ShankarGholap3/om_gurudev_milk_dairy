//----------------------------------------------------------------------------
// File: SelectFromListDialog.h
// Description: Header file for dialog to select from a list
// (c) Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_COMMON_SELECTFROMLISTDIALOG_H_
#define TEST_ANALOGIC_WS_COMMON_SELECTFROMLISTDIALOG_H_
//------------------------------------------------------------------------------
#include <QDialog>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QLabel>
#include <QPushButton>
#include <iostream>



class SelectFromListDialog : public QDialog
{
  Q_OBJECT
public:
  explicit SelectFromListDialog(QStringList& selectList,
                                QWidget *parent = 0,
                                Qt::WindowFlags flags = 0);
  virtual ~SelectFromListDialog();
  virtual QSize sizeHint();
  virtual int getSelected();
  virtual void setTitleAndHeader(QString title, QString header);

public slots:
 void handleButtonPress(int  aSelection);



private:
 QLabel*                      m_labelHeader;
 int                          m_selectedItem;
 QSignalMapper*               m_signalMapper;
 std::map<int, QPushButton* > m_buttonMap;
};
//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_COMMON_SELECTFROMLISTDIALOG_H_
//------------------------------------------------------------------------------
