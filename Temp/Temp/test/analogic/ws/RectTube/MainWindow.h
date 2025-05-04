//------------------------------------------------------------------------------
// File: MainWindow.h
// Description: MainWindow class header.
// Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_RECTTUBE_MAINWINDOW_H_
#define TEST_ANALOGIC_WS_RECTTUBE_MAINWINDOW_H_
//------------------------------------------------------------------------------

#include <vtkSmartPointer.h>
#include <QMainWindow>
#include <QMessageBox>
#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>
#include <QVTKWidget.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkCamera.h>

// Forward Qt class declarations
class Ui_MainWindow;

// Forward VTK class declarations
class vtkQtTableView;


//------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
   MainWindow();
  ~MainWindow();


public slots:
  void onExit();
  void onAbout();
  void onCreateVTKWindow();

private:
  void resetViewAlignment();
  void setupFileMenu();
  void createVTKWindow();


  vtkSmartPointer<vtkTriangleFilter>  m_polyData;
  vtkSmartPointer<vtkRenderer>        m_renderer;
  QVTKWidget*                         m_qvtkWidget;
  vtkCallbackCommand*                 m_rotCallbackCommand;
  Ui_MainWindow*                      ui;
};

//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_RECTTUBE_MAINWINDOW_H_
//------------------------------------------------------------------------------
