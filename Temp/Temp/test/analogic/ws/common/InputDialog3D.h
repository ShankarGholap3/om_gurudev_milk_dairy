//----------------------------------------------------------------------------
// File: InputDialog3D.h
// Description: Header file for dialog to to input 3D data.
// (c) Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_COMMON_INPUTDIALOG3D_H_
#define TEST_ANALOGIC_WS_COMMON_INPUTDIALOG3D_H_
//------------------------------------------------------------------------------

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>


class InputDialog3D : public QDialog
{
  Q_OBJECT
public:
  explicit InputDialog3D(QWidget *parent = 0,
                         Qt::WindowFlags flags = 0);
  virtual ~InputDialog3D();
  virtual QSize sizeHint();

  virtual void setValues(double vx, double vy, double vz);
  virtual void getValues(double& vx, double& vy, double& vz);
  virtual void setTitleAndHeader(QString title, QString header);
  virtual void setLabels(QString strX, QString strY, QString strZ);
  virtual void disableZ();

private:
 QLabel*        m_labelHeader;
 QLabel*        m_labelX;
 QLabel*        m_labelY;
 QLabel*        m_labelZ;
 QLineEdit*     m_lineEditX;
 QLineEdit*     m_lineEditY;
 QLineEdit*     m_lineEditZ;
 QPushButton*   m_okButton;
 QPushButton*   m_cancelButton;
};
//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_COMMON_INPUTDIALOG3D_H_
//------------------------------------------------------------------------------
