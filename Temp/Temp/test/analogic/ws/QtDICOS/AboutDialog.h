//------------------------------------------------------------------------------
// File: AboutDialog.h
// Description: header for About Dialog
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_QTDICOS_ABOUTDIALOG_H_
#define TEST_ANALOGIC_WS_QTDICOS_ABOUTDIALOG_H_
//------------------------------------------------------------------------------
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
class AboutDialog : public QDialog
{
public:
  explicit AboutDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
  ~AboutDialog();
};

//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_QTDICOS_ABOUTDIALOG_H_
//------------------------------------------------------------------------------
