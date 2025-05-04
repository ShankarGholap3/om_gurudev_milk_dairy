//------------------------------------------------------------------------------
// File: MainWindow.h
// Descrioption:  Main Window Header
// Copyright 2018 Analogic corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_SHARPENIMAGE_MAINWINDOW_H_
#define TEST_ANALOGIC_WS_SHARPENIMAGE_MAINWINDOW_H_
//------------------------------------------------------------------------------
#include <QTime>
#include <QMainWindow>
#include <QLineEdit>

#include <vtkDataSetReader.h>
#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>
#include <vtkImageClip.h>
#include <vtkImageShiftScale.h>
#include <vtkImageRFFT.h>
#include <vtkLookupTable.h>



#include "SceneWidget.h"

#define MAIN_WINDOW_WIDTH       1280
#define MAIN_WINDOW_HEIGHT       980
#define MAIN_WINDOW_POS_X         50
#define MAIN_WINDOW_POS_Y         50

#define TOOLBAR_ICON_SIZE_X       50
#define TOOLBAR_ICON_SIZE_Y       50

#define CURRENT_FILE_EDIT_WIDTH  300
#define PROCESS_TIIME_EDIT_WIDTH 100


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  // File menu
  void onOpenVtkFileDialog();
  void onOpenImagePngFileDialog();
  void onSaveImage();
  void onCloseApp();



  // Help menu
  void onShowAboutDialog();

  // View menu/buttons
  void onResetView();
  void onReloadFile();
  void onZoomToExtent();
  void onUpdateProcessTimer();

  // Image properties  menu
  void onShwImageInfo();
  void onShowImageHistogramBarPlot();
  void onShowImageHistogramLinePlot();
  void onShwImageStats();
  void onSetViewBackground();

  // Image math
  void onImageBrighten();
  void onBlendAddTwoImages();
  void onSubrtractTwoImages();
  void onBlendMultiplyTwoImages();
  void onInvertImage();

  // Filters
  void onSharpenImageUnSharpMask();
  void onImageLaplacianFilter();
  void onSobelFilter();
  void onSeparableConvolution();
  void onButterworthLowPass();
  void onButterworthHighPass();
  void onComputeContours();


  // Image Manipulation menu
  void onColorizeImage();
  void onRgbToGray();
  void onExtractComponent();
  void onRemoveImageAlpha();
  void onMergeOneComponentImages();

  // image Stats Menu
  void onClearRunTime();

protected:
  // Read a VTK File
  void openFile(QString& fileName);

  // Read a PNG image file
  void readImageFile(QString& fileName);


private:
  void setupToolbar();
  void setupMenu();
  void generateLookupTables();
  void removeImage();
  void replaceCurrentImage(vtkSmartPointer<vtkImageData> newImage);
  void invertGrayscaleImage();
  void invertColorImage();


  // Used to conver imaf data from real to unsigned char.
  vtkSmartPointer<vtkImageShiftScale>
        reScaleRFFTImage(vtkSmartPointer<vtkImageRFFT> inverseFilter);
  void zoomToFitImage();
  void showImageProperties();
  void showHistogramBarPlot();
  void showHistogramLinePlot();
  void showImageStats(vtkSmartPointer<vtkImageClip> anImage);

  void setCurrentFileAndPath(QString& fullPath,
                             QString& shortName);

  // Toolbar Actions
  QAction*                 m_actionOpenImageFile;
  QAction*                 m_actionOpenVtkFile;
  QAction*                 m_actionZoomArea;
  QAction*                 m_actionReseView;
  QAction*                 m_actionReloadFile;
  QAction*                 m_actionProcessTimerUpdate;
  QLineEdit*               m_currentLoadedFile;
  QString                  m_currentFileFullPath;
  QLineEdit*               m_processTimeEdit;

  vtkImageData*            m_imageRef;

  vtkSmartPointer<vtkLookupTable> m_redLookup;
  vtkSmartPointer<vtkLookupTable> m_greenLookup;
  vtkSmartPointer<vtkLookupTable> m_blueLookup;
  vtkSmartPointer<vtkLookupTable> m_defaultLookup;

  SceneWidget*             m_sceneWidget;
  double                   m_winBackgroundRed;
  double                   m_winBackgroundGreen;
  double                   m_winBackgroundBlue;
  Ui::MainWindow*          ui;
};


//------------------------------------------------------------------------------
#endif  //  TEST_ANALOGIC_WS_SHARPENIMAGE_MAINWINDOW_H_
//------------------------------------------------------------------------------
