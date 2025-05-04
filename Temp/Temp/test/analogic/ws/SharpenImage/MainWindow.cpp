//------------------------------------------------------------------------------
// File: MainWindow.cpp
// Description: MainWindow Implementation
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageLaplacian.h>
#include <vtkImageGradientMagnitude.h>
#include <vtkImageCast.h>
#include <vtkImageMathematics.h>
#include <vtkImageMapper.h>
#include <vtkImageLuminance.h>
#include <vtkPolyDataMapper.h>
#include <vtkSimpleImageToImageFilter.h>
#include <vtkIntArray.h>
#include <vtkFloatArray.h>
#include <vtkFieldData.h>
#include <vtkImageAccumulate.h>
#include <vtkImageSeparableConvolution.h>
#include <vtkImageButterworthLowPass.h>
#include <vtkImageButterworthHighPass.h>
#include <vtkImageHistogramStatistics.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageFFT.h>
#include <vtkLookupTable.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkStripper.h>
#include <vtkOutlineFilter.h>
#include <vtkImageMapToColors.h>
#include <vtkProperty2D.h>
#include <vtkBarChartActor.h>
#include <vtkXYPlotActor.h>
#include <vtkLegendBoxActor.h>

#include <QFile>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <sstream>

#include "MainWindow.h"
#include "TextInfoDialog.h"
#include "SelectFromListDialog.h"
#include "InputDialog3D.h"
#include "ScopedTimer.h"
#include "ui_MainWindow.h"


QString   g_aboutTitle = "Vtk Qt Object/Image Viewer";
QString   g_aboutText  = "By Andre R. Oughton.\n"
                         "This application shows how to cobine Qt and VTK\n"
                         "(c) Analogic Corp - 2018";

// Image data path
QString g_defaultImageDirectorty = "/home/analogic/devData/image-data/";

// VTK data path
QString g_defaultVtkDirectory    = "/home/analogic/devData/vtk-data/";

// Image Output path
QString g_defaultImageOutputDir  = "/home/analogic/junk/";

// FILE DEFALTS
QString g_VTK_FileFilter        =  "VTK Files (*.vtk)";
QString g_PNG_FileFilter        =  "PNG Image Files (*.png)";

QString g_VTK_FileDialogTitle   =  "Open VTK file";
QString g_PNG_FileDialogTitle   =  "Open Image (PNG) file";
QString g_SafeImageDialogTitle  =  "Save Image File";

QString g_PNG_EXT               = ".png";
QString g_VTK_EXT               = ".vtk";

QString g_PNG_FILE_Extension    =  "*.png";
QString g_PNG_ImageFileFilter   =  "PNG Image File (*.png)";

QString g_defaultOutputPrefix   =  "FilterOut-xxxx-";

float   g_sharpenSmoothScale    =  0.3;
float   g_sharpenScaleOut       =  1.3;
float   g_sharpenShift          = -0.15;

float  g_brightenScale          =  2.5;
float  g_brightenOffset         =  0.0;

float  g_blendScaleA            =  0.5;
float  g_blendScaleB            =  0.5;

float  g_lowPassCutoffX         =  0.05;
float  g_lowPassCutoffY         =  0.05;

float  g_highPassCutoffX        =  0.05;
float  g_highPassCutoffY        =  0.05;


double g_numContoursDef         = 1;
double g_contourRangMin         = 20;
double g_contourRangMax         = 256;




//----------------------------------------------
// Histogram chart constants

double g_histogramChart_x0 = 0.10;
double g_histogramChart_y0 = 0.10;
double g_histogramChart_z0 = 0.00;


double g_histogramChart_x1 = 0.90;
double g_histogramChart_y1 = 0.90;
double g_histogramChart_z1 = 0.00;


double g_HistogramBarColors[3][3] =
{
  { 1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, 1 }
};

double g_histo_chart_color_red    = 0.44;
double g_histo_chart_color_green  = 0.44;
double g_histo_chart_color_blue   = 0.44;

//----------------------------------------------


//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_actionOpenImageFile(nullptr),
  m_actionOpenVtkFile(nullptr),
  m_actionZoomArea(nullptr),
  m_actionReseView(nullptr),
  m_actionReloadFile(nullptr),
  m_actionProcessTimerUpdate(nullptr),
  m_currentLoadedFile(nullptr),
  m_currentFileFullPath(""),
  m_imageRef(nullptr),
  m_processTimeEdit(nullptr),
  m_redLookup(nullptr),
  m_greenLookup(nullptr),
  m_blueLookup(nullptr),
  m_defaultLookup(nullptr),
  m_sceneWidget(nullptr),
  m_winBackgroundRed(SCENE_WIDGET_BG_RED),
  m_winBackgroundGreen(SCENE_WIDGET_BG_GREEN),
  m_winBackgroundBlue(SCENE_WIDGET_BG_BLUE)
{
  ui->setupUi(this);

  //-----------------------------------------------------
  // Create the VTK Open GL viewere here
  m_sceneWidget = new SceneWidget(this);

  // Put it in the central widget layout.
  ui->centralwidget->layout()->addWidget(m_sceneWidget);
  //------------------------------------------------------

  this->setWindowIcon(QIcon(":iconImages/VTKViewerMainIcon.png"));
  this->setWindowTitle("VTK Object/Image Viewer");

  this->setMinimumWidth(MAIN_WINDOW_WIDTH);
  this->setMinimumHeight(MAIN_WINDOW_HEIGHT);
  this->move(MAIN_WINDOW_POS_X,
             MAIN_WINDOW_POS_Y);

  this->setupMenu();
  this->setupToolbar();
  this->generateLookupTables();
}


//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete ui;
}


//------------------------------------------------------------------------------
void MainWindow::onShowAboutDialog()
{
  QMessageBox aboutBox(this);
  aboutBox.setWindowTitle(g_aboutTitle);
  aboutBox.setText(g_aboutText);
  aboutBox.setIconPixmap(QPixmap(":iconImages/AboutIcon.png"));
  aboutBox.exec();
  std::cout << "Window Width-Height : "
            << this->width()
            << ", "
            << this->height()
            << std::endl;
}


//------------------------------------------------------------------------------
void MainWindow::onOpenVtkFileDialog()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  g_VTK_FileDialogTitle,
                                                  g_defaultVtkDirectory,
                                                  g_VTK_FileFilter);
  QFile file(fileName);
  file.open(QIODevice::ReadOnly);

  if (!file.exists())  return;
  QFileInfo fileInfo(fileName);
  QString shortName = fileInfo.fileName();
  this->setCurrentFileAndPath(fileName,
                              shortName);
  this->openFile(fileName);
}

//------------------------------------------------------------------------------
void MainWindow::onOpenImagePngFileDialog()
{
  QString fileName = "";

  fileName = QFileDialog::getOpenFileName(this,
                                          g_PNG_FileDialogTitle,
                                          g_defaultImageDirectorty,
                                          g_PNG_FileFilter);

  QFile file(fileName);
  if (!file.exists()) return;

  this->removeImage();
  this->readImageFile(fileName);
  this->zoomToFitImage();
}

//------------------------------------------------------------------------------
void MainWindow::onSaveImage()
{
  QFileDialog saveImageDlg(this);
  saveImageDlg.setWindowTitle(g_SafeImageDialogTitle);
  saveImageDlg.setViewMode(QFileDialog::Detail);
  saveImageDlg.setNameFilter(g_PNG_ImageFileFilter);
  saveImageDlg.setDefaultSuffix(g_PNG_FILE_Extension);
  saveImageDlg.setDirectory(g_defaultImageOutputDir);

  QString saveFilePrompt = g_defaultOutputPrefix;
  saveFilePrompt += m_currentLoadedFile->text();

  saveImageDlg.selectFile(saveFilePrompt);

  saveImageDlg.setAcceptMode(QFileDialog::AcceptSave);
  int saveDlgRes = saveImageDlg.exec();
  if (saveDlgRes != QDialog::Accepted)
  {
    return;
  }
  QString saveFile = saveImageDlg.selectedFiles().first();


  //---------------------------------
  // Now we save the results:
  vtkSmartPointer<vtkPNGWriter> writer =
      vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName(saveFile.toStdString().c_str());
  writer->SetInputData(m_imageRef);
  writer->Write();
}


//------------------------------------------------------------------------------
void MainWindow::onCloseApp()
{
  this->close();
}



//------------------------------------------------------------------------------
void MainWindow::onResetView()
{
  m_sceneWidget->resetView();
  this->zoomToFitImage();
}

//------------------------------------------------------------------------------
void MainWindow::onReloadFile()
{
  if (m_currentFileFullPath.isEmpty()) return;
  QMessageBox msgBox(this);
  msgBox.setWindowTitle("Reload Current File");
  msgBox.setText("Do you want to reload the currently loaded File?");
  msgBox.setStandardButtons(QMessageBox::Yes);
  msgBox.addButton(QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);

  int mbResp = msgBox.exec();

  if (mbResp != QMessageBox::Yes) return;

  QString fileExt = m_currentFileFullPath.right(g_VTK_EXT.length());
  if (fileExt.contains(g_PNG_EXT))
  {
    this->removeImage();
    this->readImageFile(m_currentFileFullPath);
    this->zoomToFitImage();
  }
  if (fileExt.contains(g_VTK_EXT))
  {
    this->openFile(m_currentFileFullPath);
    m_sceneWidget->resetView();
    m_sceneWidget->zoomToExtent();
  }
}


//------------------------------------------------------------------------------
void MainWindow::onZoomToExtent()
{
  m_sceneWidget->zoomToExtent();
}


//------------------------------------------------------------------------------
void MainWindow::onUpdateProcessTimer()
{
  double timerupdate = ScopedTimer::getProcessTimerValue();
  QString timeCompleteStr = QString::number(timerupdate);
  m_processTimeEdit->setText(timeCompleteStr);
}


//------------------------------------------------------------------------------
void MainWindow::onShwImageInfo()
{
  this->showImageProperties();
}


//------------------------------------------------------------------------------
void MainWindow::onShowImageHistogramBarPlot()
{
  this->showHistogramBarPlot();
}

//------------------------------------------------------------------------------
void MainWindow::onShowImageHistogramLinePlot()
{
  this->showHistogramLinePlot();
}

//------------------------------------------------------------------------------
void MainWindow::onShwImageStats()
{
  if (m_imageRef == nullptr) return;
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();
  this->showImageStats(source);
}

//------------------------------------------------------------------------------
void MainWindow::onSetViewBackground()
{
  QColor pickInit(m_winBackgroundRed*255.0,
                  m_winBackgroundGreen*255.0,
                  m_winBackgroundBlue*255.0);

  QColor pickedcolor  = QColorDialog::getColor(pickInit,
                                               this,
                                               "Pick a color",
                                               QColorDialog::DontUseNativeDialog);


  m_winBackgroundRed    = 1.0*pickedcolor.red()/255.0;
  m_winBackgroundGreen  = 1.0*pickedcolor.green()/255.0;
  m_winBackgroundBlue   = 1.0*pickedcolor.blue()/255.0;

  m_sceneWidget->setWindowBackGround(m_winBackgroundRed,
                                     m_winBackgroundGreen,
                                     m_winBackgroundBlue);
}

//==============================================================================
// IMAGE MATH
//==============================================================================

//------------------------------------------------------------------------------
void MainWindow::onImageBrighten()
{
  if (!m_imageRef) return;

  QInputDialog inputDlg(this);
  // Setup For scaling
  float fValScale = g_brightenScale;
  QString fTextScale = QString::number(fValScale);
  inputDlg.setLabelText("Input Output Scale Factor");
  inputDlg.setTextValue(fTextScale);
  int dlgResp =  inputDlg.exec();
  if (dlgResp == QDialog::Rejected) return;

  fValScale = inputDlg.textValue().toDouble();
  g_brightenScale = fValScale;

  // Setup For Offset
  float fValOffset = g_brightenOffset;
  QString fTextOffset = QString::number(fValOffset);
  inputDlg.setLabelText("Input Output Offset Value");
  inputDlg.setTextValue(fTextOffset);
  dlgResp = inputDlg.exec();
  if (dlgResp == QDialog::Rejected) return;

  fValOffset = inputDlg.textValue().toDouble();
  g_brightenOffset = fValOffset;

  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();


  ScopedTimer processTimer;
  //==============================================================
  // Scale the image to Unsigned short
  vtkSmartPointer<vtkImageShiftScale> imageScale =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScale->SetInputConnection(source->GetOutputPort());
  imageScale->SetScale(g_brightenScale);
  imageScale->SetShift(g_brightenOffset);
  imageScale->Update();

  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(imageScale->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onBlendAddTwoImages()
{
  if (!m_imageRef) return;

  QString fileName = QFileDialog::getOpenFileName(this,
                                                  g_PNG_FileDialogTitle,
                                                  g_defaultImageDirectorty,
                                                  g_PNG_FileFilter);

  QFile file(fileName);
  if (!file.exists()) return;

  //-------------------------------
  // Image Source A
  vtkSmartPointer<vtkImageClip> imageSourceA = vtkSmartPointer<vtkImageClip>::New();
  imageSourceA->SetInputData(m_imageRef);
  imageSourceA->Update();

  int numComponentsA = m_imageRef->GetNumberOfScalarComponents();

  //-------------------------------
  // Determin Blend Scaling

  QInputDialog inputDlg(this);
  float fScaleA = g_blendScaleA;
  QString fTextA = QString::number(fScaleA);
  inputDlg.setLabelText("Input Scale Factor for image A");
  inputDlg.setTextValue(fTextA);
  inputDlg.exec();
  fScaleA = inputDlg.textValue().toDouble();
  g_blendScaleA = fScaleA;

  float fScaleB = g_blendScaleB;
  QString fTextB = QString::number(fScaleB);
  inputDlg.setLabelText("Input Scale Factor for image B");
  inputDlg.setTextValue(fTextB);
  inputDlg.exec();
  fScaleB = inputDlg.textValue().toDouble();
  g_blendScaleB = fScaleB;


  //-------------------------------
  // Image Source B
  vtkSmartPointer<vtkPNGReader> pngReader =
      vtkSmartPointer<vtkPNGReader>::New();
  pngReader->SetFileName (fileName.toLocal8Bit() );
  pngReader->Update();

  vtkSmartPointer<vtkImageData> imageDataSource = vtkSmartPointer<vtkImageData>::New();
  imageDataSource->ShallowCopy(pngReader->GetOutput());

  int numComponentsB = imageDataSource->GetNumberOfScalarComponents();

  if (numComponentsA != numComponentsB)
  {
    QMessageBox::information(this,
                             "Image Add Error condition!",
                             "Note the Image Number of Components Differ");
    return;
  }

  ScopedTimer processTimer;

  vtkSmartPointer<vtkImageClip> imageSourceB = vtkSmartPointer<vtkImageClip>::New();
  imageSourceB->SetInputData(imageDataSource);
  imageSourceB->Update();



  vtkSmartPointer<vtkImageShiftScale> imageScaleA =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScaleA->SetInputConnection(imageSourceA->GetOutputPort());
  imageScaleA->SetScale(g_blendScaleA);
  imageScaleA->Update();



  vtkSmartPointer<vtkImageShiftScale> imageScaleB =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScaleB->SetInputConnection(imageSourceB->GetOutputPort());
  imageScaleB->SetScale(g_blendScaleB);
  imageScaleB->Update();



  vtkSmartPointer<vtkImageMathematics> imageCombineFilter =
      vtkSmartPointer<vtkImageMathematics>::New();
  imageCombineFilter->SetOperationToAdd();
  imageCombineFilter->SetInput1Data(imageScaleA->GetOutput());
  imageCombineFilter->SetInput2Data(imageScaleB->GetOutput());

  imageCombineFilter->Update();

  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(imageCombineFilter->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}



//------------------------------------------------------------------------------
void MainWindow::onSubrtractTwoImages()
{
  if (!m_imageRef) return;

  QString fileName = QFileDialog::getOpenFileName(this,
                                                  g_PNG_FileDialogTitle,
                                                  g_defaultImageDirectorty,
                                                  g_PNG_FileFilter);

  QFile file(fileName);
  if (!file.exists()) return;



  //-------------------------------
  // Image Source A
  vtkSmartPointer<vtkImageClip> imageSourceA = vtkSmartPointer<vtkImageClip>::New();
  imageSourceA->SetInputData(m_imageRef);
  imageSourceA->Update();

  int numComponentsA = m_imageRef->GetNumberOfScalarComponents();

  //-------------------------------
  // Determin Blend Scaling

  QInputDialog inputDlg(this);
  float fScaleA = g_blendScaleA;
  QString fTextA = QString::number(fScaleA);
  inputDlg.setLabelText("Input Scale Factor for image A");
  inputDlg.setTextValue(fTextA);
  inputDlg.exec();
  fScaleA = inputDlg.textValue().toDouble();
  g_blendScaleA = fScaleA;

  float fScaleB = g_blendScaleB;
  QString fTextB = QString::number(fScaleB);
  inputDlg.setLabelText("Input Scale Factor for image B");
  inputDlg.setTextValue(fTextB);
  inputDlg.exec();
  fScaleB = inputDlg.textValue().toDouble();
  g_blendScaleB = fScaleB;


  //-------------------------------
  // Image Source B
  vtkSmartPointer<vtkPNGReader> pngReader =
      vtkSmartPointer<vtkPNGReader>::New();
  pngReader->SetFileName (fileName.toLocal8Bit() );
  pngReader->Update();

  vtkSmartPointer<vtkImageData> imageDataSource = vtkSmartPointer<vtkImageData>::New();
  imageDataSource->ShallowCopy(pngReader->GetOutput());

  int numComponentsB = imageDataSource->GetNumberOfScalarComponents();

  if (numComponentsA != numComponentsB)
  {
    QMessageBox::information(this,
                             "Image Add Error condition!",
                             "Note the Image Number of Components Differ");
    return;
  }

  ScopedTimer processTimer;

  vtkSmartPointer<vtkImageClip> imageSourceB = vtkSmartPointer<vtkImageClip>::New();
  imageSourceB->SetInputData(imageDataSource);
  imageSourceB->Update();



  vtkSmartPointer<vtkImageShiftScale> imageScaleA =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScaleA->SetInputConnection(imageSourceA->GetOutputPort());
  imageScaleA->SetScale(g_blendScaleA);
  imageScaleA->Update();



  vtkSmartPointer<vtkImageShiftScale> imageScaleB =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScaleB->SetInputConnection(imageSourceB->GetOutputPort());
  imageScaleB->SetScale(g_blendScaleB);
  imageScaleB->Update();



  vtkSmartPointer<vtkImageMathematics> imageCombineFilter =
      vtkSmartPointer<vtkImageMathematics>::New();
  imageCombineFilter->SetOperationToSubtract();
  imageCombineFilter->SetInput1Data(imageScaleA->GetOutput());
  imageCombineFilter->SetInput2Data(imageScaleB->GetOutput());

  imageCombineFilter->Update();

  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(imageCombineFilter->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}


//------------------------------------------------------------------------------
void MainWindow::onBlendMultiplyTwoImages()
{
  if (!m_imageRef) return;

  QString fileName = QFileDialog::getOpenFileName(this,
                                                  g_PNG_FileDialogTitle,
                                                  g_defaultImageDirectorty,
                                                  g_PNG_FileFilter);

  QFile file(fileName);
  if (!file.exists()) return;


  ScopedTimer processTimer;
  //-------------------------------
  // Image Source A
  vtkSmartPointer<vtkImageClip> imageSourceA = vtkSmartPointer<vtkImageClip>::New();
  imageSourceA->SetInputData(m_imageRef);
  imageSourceA->Update();


  //-------------------------------
  // Determin Blend Scaling

  QInputDialog inputDlg(this);
  float fScaleA = g_blendScaleA;
  QString fTextA = QString::number(fScaleA);
  inputDlg.setLabelText("Input Scale Factor for image A");
  inputDlg.setTextValue(fTextA);
  inputDlg.exec();
  fScaleA = inputDlg.textValue().toDouble();
  g_blendScaleA = fScaleA;

  float fScaleB = g_blendScaleB;
  QString fTextB = QString::number(fScaleB);
  inputDlg.setLabelText("Input Scale Factor for image B");
  inputDlg.setTextValue(fTextB);
  inputDlg.exec();
  fScaleB = inputDlg.textValue().toDouble();
  g_blendScaleB = fScaleB;


  //-------------------------------
  // Image Source B
  vtkSmartPointer<vtkPNGReader> pngReader =
      vtkSmartPointer<vtkPNGReader>::New();
  pngReader->SetFileName (fileName.toLocal8Bit() );
  pngReader->Update();

  vtkSmartPointer<vtkImageData> imageDataSource = vtkSmartPointer<vtkImageData>::New();
  imageDataSource->ShallowCopy(pngReader->GetOutput());


  vtkSmartPointer<vtkImageClip> imageSourceB = vtkSmartPointer<vtkImageClip>::New();
  imageSourceB->SetInputData(imageDataSource);
  imageSourceB->Update();



  vtkSmartPointer<vtkImageShiftScale> imageScaleA =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScaleA->SetInputConnection(imageSourceA->GetOutputPort());
  imageScaleA->SetScale(g_blendScaleA);
  imageScaleA->Update();



  vtkSmartPointer<vtkImageShiftScale> imageScaleB =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScaleB->SetInputConnection(imageSourceB->GetOutputPort());
  imageScaleB->SetScale(g_blendScaleB);
  imageScaleB->Update();



  vtkSmartPointer<vtkImageMathematics> imageCombineFilter =
      vtkSmartPointer<vtkImageMathematics>::New();
  imageCombineFilter->SetOperationToMultiply();
  imageCombineFilter->SetInput1Data(imageScaleA->GetOutput());
  imageCombineFilter->SetInput2Data(imageScaleB->GetOutput());

  imageCombineFilter->Update();

  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(imageCombineFilter->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onInvertImage()
{
  if (!m_imageRef) return;

  ScopedTimer processTimer;

  int numComponents = m_imageRef->GetNumberOfScalarComponents();
  if (numComponents == 1)
  {
    this->invertGrayscaleImage();
  }
  else
  {
    this->invertColorImage();
  }
}

//==============================================================================
// END IMAGE MATH
//==============================================================================


//==============================================================================
// FILTERS
//==============================================================================


//------------------------------------------------------------------------------
void MainWindow::onSharpenImageUnSharpMask()
{
  if (!m_imageRef) return;

  // Get Source Image
  vtkSmartPointer<vtkImageClip> imageSource = vtkSmartPointer<vtkImageClip>::New();
  imageSource->SetInputData(m_imageRef);
  imageSource->Update();

  QInputDialog inputDlg(this);
  float fVal = g_sharpenSmoothScale;
  QString fText = QString::number(fVal);
  inputDlg.setLabelText("Input Scale Factor");
  inputDlg.setTextValue(fText);
  inputDlg.exec();
  fVal = inputDlg.textValue().toDouble();
  g_sharpenSmoothScale = fVal;

  ScopedTimer processTimer;

  //============================================================================
  // Unsharp Mask Filter
  // Uses a Gausssian Smooth and a difference filter .
  vtkSmartPointer<vtkImageGaussianSmooth> imageSmoothFilter =
      vtkSmartPointer<vtkImageGaussianSmooth>::New();
  imageSmoothFilter->SetInputData(m_imageRef);
  imageSmoothFilter->Update();

  vtkSmartPointer<vtkImageShiftScale> imageScale =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScale->SetInputConnection(imageSmoothFilter->GetOutputPort());
  imageScale->SetScale(g_sharpenSmoothScale);
  imageScale->Update();

  vtkSmartPointer<vtkImageMathematics> imageDiffFilter =
      vtkSmartPointer<vtkImageMathematics>::New();
  imageDiffFilter->SetOperationToSubtract();
  imageDiffFilter->SetInput1Data(imageSource->GetOutput());
  imageDiffFilter->SetInput2Data(imageScale->GetOutput());

  imageDiffFilter->Update();
  //============================================================================

  // Seup For scaling
  float fValScale = g_sharpenScaleOut;
  QString fTextScale = QString::number(fValScale);
  inputDlg.setLabelText("Input Output Scale Factor");
  inputDlg.setTextValue(fTextScale);
  inputDlg.exec();
  fValScale = inputDlg.textValue().toDouble();
  g_sharpenScaleOut = fValScale;


  float fValShift = g_sharpenShift;

  QString fTextShift = QString::number(fValShift);
  inputDlg.setLabelText("Input Output Shift");
  inputDlg.setTextValue(fTextShift);
  inputDlg.exec();
  fValShift = inputDlg.textValue().toDouble();
  g_sharpenShift = fValShift;

  //============================================================================
  // One more Scale and shift so Output isn't so dark

  vtkSmartPointer<vtkImageShiftScale> imageScaleOut =
      vtkSmartPointer<vtkImageShiftScale>::New();
  imageScaleOut->SetInputConnection(imageDiffFilter->GetOutputPort());
  imageScaleOut->SetScale(g_sharpenScaleOut);
  imageScaleOut->SetShift(g_sharpenShift);
  imageScaleOut->Update();

  vtkSmartPointer<vtkImageClip> imageInterim = vtkSmartPointer<vtkImageClip>::New();
  imageInterim->SetInputConnection(imageScaleOut->GetOutputPort());
  imageInterim->Update();

  vtkSmartPointer<vtkImageClip> imageOutput = vtkSmartPointer<vtkImageClip>::New();
  imageOutput->SetInputConnection(imageInterim->GetOutputPort());
  imageOutput->Update();

  //============================================================================

  // On final Image data to backup the m_imageRef pointer

  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(imageOutput->GetOutput());

  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onImageLaplacianFilter()
{
  if (!m_imageRef) return;

  ScopedTimer processTimer;

  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  vtkSmartPointer<vtkImageCast> castOriginal =
      vtkSmartPointer<vtkImageCast>::New();
  castOriginal->SetInputConnection(source->GetOutputPort());
  castOriginal->SetOutputScalarTypeToFloat();
  castOriginal->Update();

  vtkSmartPointer<vtkImageLaplacian> laplacianFilter =
      vtkSmartPointer<vtkImageLaplacian>::New();
  laplacianFilter->SetInputConnection(source->GetOutputPort());
  laplacianFilter->Update();

  vtkSmartPointer<vtkImageCast> castLaplacian =
      vtkSmartPointer<vtkImageCast>::New();
  castLaplacian->SetInputConnection(laplacianFilter->GetOutputPort());
  castLaplacian->SetOutputScalarTypeToFloat();
  castLaplacian->Update();


  // On final Image data to backup the m_imageRef pointer

  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(castLaplacian->GetOutput());

  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onSobelFilter()
{
  if (!m_imageRef) return;
  ScopedTimer processTimer;
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  vtkSmartPointer<vtkImageGradientMagnitude> gradientMagnitudeFilter =
      vtkSmartPointer<vtkImageGradientMagnitude>::New();
  gradientMagnitudeFilter->SetInputConnection(source->GetOutputPort());
  gradientMagnitudeFilter->Update();

  vtkSmartPointer<vtkImageCast> gradientMagnitudeCastFilter =
      vtkSmartPointer<vtkImageCast>::New();
  gradientMagnitudeCastFilter->SetInputConnection(gradientMagnitudeFilter->GetOutputPort());
  gradientMagnitudeCastFilter->SetOutputScalarTypeToUnsignedChar();
  gradientMagnitudeCastFilter->Update();


  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(gradientMagnitudeCastFilter->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onSeparableConvolution()
{
  if (!m_imageRef) return;

  int numComponents = m_imageRef->GetNumberOfScalarComponents();

  if (numComponents != 1)
  {
    QMessageBox::information(this,
                             "Error in usage",
                             "VTK Separable Convolution - vald for 1 Component image only!");
    return;
  }
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  vtkSmartPointer<vtkFloatArray> xKernel =
      vtkSmartPointer<vtkFloatArray>::New();
  xKernel->SetNumberOfTuples(3);
  xKernel->SetNumberOfComponents(1);
  xKernel->SetValue(0,   -1);
  xKernel->SetValue(1,    0);
  xKernel->SetValue(2,    1);

  vtkSmartPointer<vtkFloatArray> yKernel =
      vtkSmartPointer<vtkFloatArray>::New();
  yKernel->SetNumberOfTuples(3);
  yKernel->SetNumberOfComponents(1);
  yKernel->SetValue(0,    1);
  yKernel->SetValue(1,    2);
  yKernel->SetValue(2,    1);

  vtkSmartPointer<vtkImageSeparableConvolution> convolutionFilter =
      vtkSmartPointer<vtkImageSeparableConvolution>::New();
  convolutionFilter->SetInputConnection(source->GetOutputPort());
  convolutionFilter->SetXKernel(xKernel);
  convolutionFilter->SetYKernel(yKernel);
  convolutionFilter->Update();


  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(convolutionFilter->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onButterworthLowPass()
{
  if (!m_imageRef) return;

  //-------------------------------
  // Get filter inputs:

  QInputDialog inputDlg(this);
  float fCuttoffA = g_lowPassCutoffX;
  QString fTextA = QString::number(fCuttoffA);
  inputDlg.setLabelText("Filter cutoff X");
  inputDlg.setTextValue(fTextA);
  inputDlg.exec();
  fCuttoffA = inputDlg.textValue().toDouble();
  g_lowPassCutoffX = fCuttoffA;

  float fCuttoffB = g_lowPassCutoffY;
  QString fTextB = QString::number(fCuttoffB);
  inputDlg.setLabelText("Factor cutoff Y");
  inputDlg.setTextValue(fTextB);
  inputDlg.exec();
  fCuttoffB = inputDlg.textValue().toDouble();
  g_lowPassCutoffY = fCuttoffB;


  ScopedTimer processTimer;
  //-------------------------------
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  // Apply FFT to image
  vtkSmartPointer<vtkImageFFT> fftFilter =
      vtkSmartPointer<vtkImageFFT>::New();
  fftFilter->SetDimensionality(2);
  fftFilter->SetInputConnection(source->GetOutputPort());
  fftFilter->Update();

  // Apply bandpass to FFT
  vtkSmartPointer<vtkImageButterworthLowPass> lowPassFilter = vtkSmartPointer<vtkImageButterworthLowPass>::New();
  lowPassFilter->SetOrder(4);
  lowPassFilter->SetXCutOff(g_lowPassCutoffX);
  lowPassFilter->SetYCutOff(g_lowPassCutoffY);
  lowPassFilter->SetInputConnection(fftFilter->GetOutputPort());
  lowPassFilter->Update();

  vtkSmartPointer<vtkImageClip> fliterOut = vtkSmartPointer<vtkImageClip>::New();
  fliterOut->SetInputConnection(lowPassFilter->GetOutputPort());
  fliterOut->Update();

  vtkSmartPointer<vtkImageRFFT> inverseFilter  = vtkSmartPointer<vtkImageRFFT>::New();
  inverseFilter->SetDimensionality(2);
  inverseFilter->SetInputConnection(fliterOut->GetOutputPort());
  inverseFilter->Update();

  vtkSmartPointer<vtkImageShiftScale>  scale = this->reScaleRFFTImage(inverseFilter);

  vtkSmartPointer<vtkImageData> imageDataOut =
      vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(scale->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onButterworthHighPass()
{
  if (!m_imageRef) return;

  //-------------------------------
  // Get filter inputs:

  QInputDialog inputDlg(this);
  float fCuttoffA = g_highPassCutoffX;
  QString fTextA = QString::number(fCuttoffA);
  inputDlg.setLabelText("Filter cutoff X");
  inputDlg.setTextValue(fTextA);
  inputDlg.exec();
  fCuttoffA = inputDlg.textValue().toDouble();
  g_highPassCutoffX = fCuttoffA;

  float fCuttoffB = g_highPassCutoffY;
  QString fTextB = QString::number(fCuttoffB);
  inputDlg.setLabelText("Factor cutoff Y");
  inputDlg.setTextValue(fTextB);
  inputDlg.exec();
  fCuttoffB = inputDlg.textValue().toDouble();
  g_highPassCutoffY = fCuttoffB;



  ScopedTimer processTimer;
  //-------------------------------
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  // Apply FFT to image
  vtkSmartPointer<vtkImageFFT> fftFilter =
      vtkSmartPointer<vtkImageFFT>::New();
  fftFilter->SetDimensionality(2);
  fftFilter->SetInputConnection(source->GetOutputPort());
  fftFilter->Update();

  // Apply bandpass to FFT
  vtkSmartPointer<vtkImageButterworthHighPass> highPassFilter = vtkSmartPointer<vtkImageButterworthHighPass>::New();
  highPassFilter->SetOrder(4);
  highPassFilter->SetXCutOff(g_highPassCutoffX);
  highPassFilter->SetYCutOff(g_highPassCutoffY);
  highPassFilter->SetInputConnection(fftFilter->GetOutputPort());
  highPassFilter->Update();


  vtkSmartPointer<vtkImageClip> fliterOut = vtkSmartPointer<vtkImageClip>::New();
  fliterOut->SetInputConnection(highPassFilter->GetOutputPort());
  fliterOut->Update();

  // Invert the FFT
  vtkSmartPointer<vtkImageRFFT> inverseFilter  = vtkSmartPointer<vtkImageRFFT>::New();
  inverseFilter->SetDimensionality(2);
  inverseFilter->SetInputConnection(fliterOut->GetOutputPort());
  inverseFilter->Update();

  vtkSmartPointer<vtkImageShiftScale>  scale = this->reScaleRFFTImage(inverseFilter);


  vtkSmartPointer<vtkImageData> imageDataOut = vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(scale->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onComputeContours()
{
  if (!m_imageRef) return;

  double numContours = g_numContoursDef;
  double minValue    = g_contourRangMin;
  double maxValue    = g_contourRangMax;



  InputDialog3D contourInputDlg(this);
  contourInputDlg.setLabels("# countours:",
                            "Min Val:",
                            "Max Val:");
  contourInputDlg.setValues(numContours,
                            minValue,
                            maxValue);
  contourInputDlg.exec();

  contourInputDlg.getValues(numContours,
                            minValue,
                            maxValue);

  g_numContoursDef =  numContours;
  g_contourRangMin =  minValue;
  g_contourRangMax =  maxValue;


  ScopedTimer processTimer;

  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);

  source->Update();


  int numComponents = m_imageRef->GetNumberOfScalarComponents();

  // Create an isosurface
  vtkSmartPointer<vtkContourFilter> contourFilter =  vtkSmartPointer<vtkContourFilter>::New();

  if (numComponents == 1)
  {
    vtkSmartPointer<vtkImageCast> imageCastFilter =
        vtkSmartPointer<vtkImageCast>::New();
    imageCastFilter->SetInputConnection(source->GetOutputPort());
    imageCastFilter->SetOutputScalarTypeToUnsignedInt();
    imageCastFilter->Update();
    contourFilter->SetInputConnection(imageCastFilter->GetOutputPort());
    contourFilter->GenerateValues(numContours,
                                  minValue,
                                  maxValue);
  }
  else
  {
    contourFilter->SetInputConnection(source->GetOutputPort());
    contourFilter->GenerateValues(numContours,
                                  minValue,
                                  maxValue);
  }

  // Map the contours to graphical primitives
  vtkSmartPointer<vtkPolyDataMapper> contourMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  contourMapper->SetInputConnection(contourFilter->GetOutputPort());

  // Create an actor for the contours
  vtkSmartPointer<vtkActor> contourActor =
      vtkSmartPointer<vtkActor>::New();
  contourActor->SetMapper(contourMapper);

  // Create the outline
  vtkSmartPointer<vtkOutlineFilter> outlineFilter =
      vtkSmartPointer<vtkOutlineFilter>::New();
  outlineFilter->SetInputData(source->GetOutput());

  vtkSmartPointer<vtkPolyDataMapper> outlineMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());
  vtkSmartPointer<vtkActor> outlineActor =
      vtkSmartPointer<vtkActor>::New();
  outlineActor->SetMapper(outlineMapper);


  vtkSmartPointer<vtkPolyDataNormals> pickNormals =
      vtkSmartPointer<vtkPolyDataNormals>::New();

  pickNormals->SetInputConnection(contourFilter->GetOutputPort());

  pickNormals->SetFeatureAngle(60.0);



  vtkSmartPointer<vtkStripper> selectStripper =
      vtkSmartPointer<vtkStripper>::New();

  selectStripper->SetInputConnection(pickNormals->GetOutputPort());


  vtkSmartPointer<vtkPolyDataMapper> selectMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();

  selectMapper->SetInputConnection(selectStripper->GetOutputPort());

  selectMapper->ScalarVisibilityOff();
  selectMapper->SetColorModeToMapScalars();
  selectMapper->Update();

  vtkSmartPointer<vtkActor> selectOutline =
      vtkSmartPointer<vtkActor>::New();
  selectOutline->SetMapper(selectMapper);

  // ARO-TODO: Move to Scene Widget! With Remove Chart.
  vtkRenderer* sceneRenderer = m_sceneWidget->getRenderer();
  if (sceneRenderer)
  {
    // sceneRenderer->AddActor(contourActor);
    // sceneRenderer->AddActor(outlineActor);
    sceneRenderer->AddActor(selectOutline);
    m_sceneWidget->doRender();
  }
}

//==============================================================================
// END FILTERS
//==============================================================================

//==============================================================================
// IMAGE MANIPULATION
//==============================================================================

//------------------------------------------------------------------------------
void MainWindow::onColorizeImage()
{
  if (!m_imageRef) return;
  vtkLookupTable* lookupTable = nullptr;
  QStringList selectList = {"RED",
                            "GREEN",
                            "BLUE",
                            "DEFAULT"};

  SelectFromListDialog slectFromList(selectList, this);
  slectFromList.exec();
  int retval = slectFromList.getSelected();
  switch (retval)
  {
  case 0:
    lookupTable = m_redLookup.GetPointer();
    break;
  case 1:
    lookupTable = m_greenLookup.GetPointer();
    break;
  case 2:
    lookupTable = m_blueLookup.GetPointer();
    break;
  case 3:
    lookupTable = m_defaultLookup.GetPointer();
    break;
  default:
    lookupTable = m_defaultLookup.GetPointer();
  }

  ScopedTimer processTimer;
  // Pass the original image and the lookup table to a filter to create
  // a color image:
  vtkSmartPointer<vtkImageMapToColors> scalarValuesToColors =
      vtkSmartPointer<vtkImageMapToColors>::New();
  scalarValuesToColors->SetLookupTable(lookupTable);
  scalarValuesToColors->PassAlphaToOutputOff();
  scalarValuesToColors->SetInputData(m_imageRef);
  scalarValuesToColors->Update();



  vtkSmartPointer<vtkImageExtractComponents> extractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  extractor->SetInputConnection(scalarValuesToColors->GetOutputPort());
  extractor->SetComponents(0, 1, 2);


  vtkSmartPointer<vtkImageClip> outputImage = vtkSmartPointer<vtkImageClip>::New();
  outputImage->SetInputConnection(extractor->GetOutputPort());
  outputImage->Update();

  vtkSmartPointer<vtkImageData> imageDataOut = vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(outputImage->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onRgbToGray()
{
  if (!m_imageRef) return;
  int numComponents = m_imageRef->GetNumberOfScalarComponents();
  if (numComponents < 3 )
  {
    QMessageBox::information(this,
                             "Impage Component extraction",
                             "Image has less that 3 scalsr components.");
    return;
  }

  ScopedTimer processTimer;
  //-------------------------------
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();


  vtkSmartPointer<vtkImageExtractComponents> component_extractor = vtkSmartPointer<vtkImageExtractComponents>::New();
  component_extractor->SetInputConnection(source->GetOutputPort());
  component_extractor->SetComponents(0, 1, 2);
  component_extractor->Update();


  vtkSmartPointer<vtkImageLuminance> luminance_filter = vtkSmartPointer<vtkImageLuminance>::New();
  luminance_filter->SetInputConnection(component_extractor->GetOutputPort());
  luminance_filter->Update();


  vtkSmartPointer<vtkImageClip> outputImage = vtkSmartPointer<vtkImageClip>::New();
  outputImage->SetInputConnection(luminance_filter->GetOutputPort());
  outputImage->Update();

  vtkSmartPointer<vtkImageData> imageDataOut = vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(outputImage->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//------------------------------------------------------------------------------
void MainWindow::onExtractComponent()
{
  if (!m_imageRef) return;

  int numComponentsA = m_imageRef->GetNumberOfScalarComponents();
  if (numComponentsA == 1)
  {
    QMessageBox::information(this,
                             "Impage Component extraction",
                             "The Image has only ONE component.");
    return;
  }

  ScopedTimer processTimer;

  QStringList selList;
  for (int i = 0; i< numComponentsA; i++ )
  {
    QString listEntry = "Comp: " + QString::number(i);
    selList.push_back(listEntry);
  }

  SelectFromListDialog slectFromList(selList, this);
  slectFromList.exec();
  int selValue = slectFromList.getSelected();



  //-------------------------------
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  //==============================================================
  // Scale the image to Unsigned short
  vtkSmartPointer<vtkImageHistogramStatistics> stats =
      vtkSmartPointer<vtkImageHistogramStatistics>::New();
  stats->SetInputConnection(source->GetOutputPort());
  stats->Update();

  double range[2];
  stats->GetAutoRange(range);
  //----------------------------------
  // Filter outpout Range
  // std::cout << "Scale Filter output range R0 - R1 :"
  //          << range[0] << " - "
  //          << range[1] << std::endl;
  //==============================================================

  vtkSmartPointer<vtkImageExtractComponents> extractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

   extractor->SetInputConnection(source->GetOutputPort());
   extractor->SetComponents(selValue);


   vtkSmartPointer<vtkImageClip> outputImage = vtkSmartPointer<vtkImageClip>::New();
   outputImage->SetInputConnection(extractor->GetOutputPort());
   outputImage->Update();

   vtkSmartPointer<vtkImageData> imageDataOut = vtkSmartPointer<vtkImageData>::New();
   imageDataOut->DeepCopy(outputImage->GetOutput());
   this->replaceCurrentImage(imageDataOut);
}


//------------------------------------------------------------------------------
void MainWindow::onRemoveImageAlpha()
{
  if (!m_imageRef) return;


  int numComponents = m_imageRef->GetNumberOfScalarComponents();
  if (numComponents < 4)
  {
    QMessageBox::information(this,
                             "Image Alpha Removal Error!",
                             "The image has less than 4 components.\nAlpha is generally component # 4");
    return;
  }

  ScopedTimer processTimer;

  //-------------------------------
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();


  //-----------------
  // Red Extractor
  vtkSmartPointer<vtkImageExtractComponents> redExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  redExtractor->SetInputConnection(source->GetOutputPort());
  redExtractor->SetComponents(0);
  redExtractor->Update();
  //------------------

  //-----------------
  // Green Extractor
  vtkSmartPointer<vtkImageExtractComponents> greenExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  greenExtractor->SetInputConnection(source->GetOutputPort());
  greenExtractor->SetComponents(1);
  greenExtractor->Update();
  //------------------

  //-----------------
  // Blue Extractor
  vtkSmartPointer<vtkImageExtractComponents> blueExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  blueExtractor->SetInputConnection(source->GetOutputPort());
  blueExtractor->SetComponents(2);
  blueExtractor->Update();
  //------------------


  vtkSmartPointer<vtkImageAppendComponents> appendFilter =
           vtkSmartPointer<vtkImageAppendComponents>::New();
  appendFilter->SetInputConnection(0, redExtractor->GetOutputPort());
  appendFilter->AddInputConnection(0, greenExtractor->GetOutputPort());
  appendFilter->AddInputConnection(0, blueExtractor->GetOutputPort());
  appendFilter->Update();


  vtkSmartPointer<vtkImageClip> outputImage = vtkSmartPointer<vtkImageClip>::New();
  outputImage->SetInputConnection(appendFilter->GetOutputPort());
  outputImage->Update();

  vtkSmartPointer<vtkImageData> imageDataOut = vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(outputImage->GetOutput());
  this->replaceCurrentImage(imageDataOut);
  QMessageBox::information(this,
                           "Impage Component extraction",
                           "Alpha Removed From Image.");
}

//------------------------------------------------------------------------------
void MainWindow::onMergeOneComponentImages()
{
  QFileDialog loadFileDialog(this,
                             "Merge selected files ...",
                             g_defaultImageOutputDir,
                             g_PNG_ImageFileFilter);
  loadFileDialog.setAcceptMode(QFileDialog::AcceptOpen);
  loadFileDialog.setFileMode(QFileDialog::ExistingFiles);

  int dlgRet =  loadFileDialog.exec();
  if (dlgRet == QDialog::Rejected) return;

  QStringList selectList = loadFileDialog.selectedFiles();
  if (selectList.size() !=3 )
  {
    QMessageBox::information(this,
                             "Error in usage",
                             "Select THREE files to merge.");
    return;
  }

  ScopedTimer processTimer;

  //--------------------------------------
  // Red image
  QString filenameRed = selectList.at(0);
  vtkSmartPointer<vtkPNGReader> pngReaderRed =
      vtkSmartPointer<vtkPNGReader>::New();
  pngReaderRed->SetFileName (filenameRed.toLocal8Bit() );
  pngReaderRed->Update();

  // Green image
  QString filenameGreen = selectList.at(1);
  vtkSmartPointer<vtkPNGReader> pngReaderGreen =
      vtkSmartPointer<vtkPNGReader>::New();
  pngReaderGreen->SetFileName (filenameGreen.toLocal8Bit() );
  pngReaderGreen->Update();

  // Blue image
  QString filenameBlue = selectList.at(2);
  vtkSmartPointer<vtkPNGReader> pngReaderBlue =
      vtkSmartPointer<vtkPNGReader>::New();
  pngReaderBlue->SetFileName (filenameBlue.toLocal8Bit() );
  pngReaderBlue->Update();

  //-----------------
  // Red Extractor
  vtkSmartPointer<vtkImageExtractComponents> redExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  redExtractor->SetInputConnection(pngReaderRed->GetOutputPort());
  redExtractor->SetComponents(0);
  redExtractor->Update();
  //------------------

  //-----------------
  // Green Extractor
  vtkSmartPointer<vtkImageExtractComponents> greenExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  greenExtractor->SetInputConnection(pngReaderGreen->GetOutputPort());
  greenExtractor->SetComponents(1);
  greenExtractor->Update();
  //------------------

  //-----------------
  // Blue Extractor
  vtkSmartPointer<vtkImageExtractComponents> blueExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  blueExtractor->SetInputConnection(pngReaderBlue->GetOutputPort());
  blueExtractor->SetComponents(2);
  blueExtractor->Update();
  //------------------


  vtkSmartPointer<vtkImageAppendComponents> appendFilter =
           vtkSmartPointer<vtkImageAppendComponents>::New();
  appendFilter->SetInputConnection(0, redExtractor->GetOutputPort());
  appendFilter->AddInputConnection(0, greenExtractor->GetOutputPort());
  appendFilter->AddInputConnection(0, blueExtractor->GetOutputPort());
  appendFilter->Update();


  vtkSmartPointer<vtkImageClip> outputImage = vtkSmartPointer<vtkImageClip>::New();
  outputImage->SetInputConnection(appendFilter->GetOutputPort());
  outputImage->Update();

  vtkSmartPointer<vtkImageData> imageDataOut = vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(outputImage->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}

//==============================================================================
// END IMAGE MANIPULATION
//==============================================================================



//==============================================================================
// RUN STATS MENU
//==============================================================================

//------------------------------------------------------------------------------
void MainWindow::onClearRunTime()
{
   ScopedTimer::resetProcessTimer();
   m_processTimeEdit->setText("");
}

//==============================================================================
// END RUN STATS MENU
//==============================================================================



//------------------------------------------------------------------------------
vtkSmartPointer<vtkImageShiftScale>
MainWindow::reScaleRFFTImage(vtkSmartPointer<vtkImageRFFT> inverseFilter)
{
  //-----------------------------------
  // Scale the image to Unsigned short
  vtkSmartPointer<vtkImageHistogramStatistics> stats =
      vtkSmartPointer<vtkImageHistogramStatistics>::New();
  stats->SetInputConnection(inverseFilter->GetOutputPort());
  stats->Update();

  double range[2];
  stats->GetAutoRange(range);
  //----------------------------------
  // Filter outpout Range
  // std::cout << "Filter output range R0 - R1 :"
  //          << range[0] << " - "
  //          << range[1] << std::endl;
  //---------------------------------
  if (range[0] > 0.0)
  {
    std::cout << "Warning AutoRange - range[0] < 0.0" << std::endl;
    range[0] = 0.0;
  }

  vtkSmartPointer<vtkImageShiftScale> shiftScale =
      vtkSmartPointer<vtkImageShiftScale>::New();
  shiftScale->SetInputData(inverseFilter->GetOutput());
  shiftScale->SetShift(-range[0]);
  shiftScale->SetScale(255.0/(range[1] - range[0]));
  shiftScale->ClampOverflowOn();
  shiftScale->SetOutputScalarTypeToUnsignedChar();
  shiftScale->Update();
  //----------------------------
  return shiftScale;
}



//------------------------------------------------------------------------------
void MainWindow::openFile(QString& fileName)
{
  m_sceneWidget->removeDataSet();

  // Create reader
  vtkSmartPointer<vtkDataSetReader> reader = vtkSmartPointer<vtkDataSetReader>::New();
  reader->SetFileName(fileName.toStdString().c_str());

  // Read the file
  reader->Update();

  // Add 3d data set to the view
  vtkSmartPointer<vtkDataSet> dataSet = reader->GetOutput();
  if (dataSet != nullptr)
  {
    m_sceneWidget->addDataSet(reader->GetOutput());
  }
}

//------------------------------------------------------------------------------
void MainWindow::readImageFile(QString& fileName)
{
  QFileInfo fileInfo(fileName);
  QString shortName = fileInfo.fileName();
  this->setCurrentFileAndPath(fileName,
                              shortName);

  //=================================
  // Read the image
  vtkSmartPointer<vtkPNGReader> pngReader =
      vtkSmartPointer<vtkPNGReader>::New();
  pngReader->SetFileName (fileName.toLocal8Bit() );
  pngReader->Update();

  vtkSmartPointer<vtkImageData> image1 = vtkSmartPointer<vtkImageData>::New();
  image1->ShallowCopy(pngReader->GetOutput());
  m_sceneWidget->addImage(image1);

  m_imageRef = image1.GetPointer();
}


void MainWindow::setupMenu()
{
  //-------------------------------------------------------
  // FILE MENU
  this->connect(ui->actionOpen_VTK_File, &QAction::triggered,
                this, &MainWindow::onOpenVtkFileDialog);

  this->connect(ui->actionOpen_Image_File, &QAction::triggered,
                this, &MainWindow::onOpenImagePngFileDialog);

  this->connect(ui->actionSave_Image_File, &QAction::triggered,
                this, &MainWindow::onSaveImage);

  this->connect(ui->actionExit, &QAction::triggered,
                this, &MainWindow::onCloseApp);
  //-------------------------------------------------------


  //-------------------------------------------------------
  // IMAGE FILTERS MENU
  this->connect(ui->actionSharpen_Image, &QAction::triggered,
                this, &MainWindow::onSharpenImageUnSharpMask);

  this->connect(ui->actionLapacian_Filter, &QAction::triggered,
                this, &MainWindow::onImageLaplacianFilter);

  this->connect(ui->actionSobel_Filter, &QAction::triggered,
                this, &MainWindow::onSobelFilter);


  this->connect(ui->actionSeparable_Convolution, &QAction::triggered,
                this,  &MainWindow::onSeparableConvolution);

  this->connect(ui->actionButterworth_Low_pass, &QAction::triggered,
                this, &MainWindow::onButterworthLowPass);

  this->connect(ui->actionButtwerworth_High_pass, &QAction::triggered,
                this, &MainWindow::onButterworthHighPass);

  this->connect(ui->actionGenerate_Countour_Lines, &QAction::triggered,
                this, &MainWindow::onComputeContours);

  //-------------------------------------------------------

  //-------------------------------------------------------
  // IMAGE MATH MENU
  this->connect(ui->actionBrighten_Image, &QAction::triggered,
                this, &MainWindow::onImageBrighten);

  this->connect(ui->actionBlend_Two_Images, &QAction::triggered,
                this,  &MainWindow::onBlendAddTwoImages);

  this->connect(ui->actionSubtract_A_B_image, &QAction::triggered,
                this, &MainWindow::onSubrtractTwoImages);

  this->connect(ui->actionBlend_MULTIPLY_Two_Images, &QAction::triggered,
                this, &MainWindow::onBlendMultiplyTwoImages);

  this->connect(ui->actionInvert_Image,  &QAction::triggered,
                this, &MainWindow::onInvertImage);
  //-------------------------------------------------------


  //-------------------------------------------------------
  // IMAGE COMPOPNENT CHANGE MENU
  this->connect(ui->actionColorize_Image,  &QAction::triggered,
                this, &MainWindow::onColorizeImage);

  this->connect(ui->actionRGB_To_Gray,  &QAction::triggered,
                this, &MainWindow::onRgbToGray);


  this->connect(ui->actionExtract_Component, &QAction::triggered,
                this, &MainWindow::onExtractComponent);

  this->connect(ui->actionRemove_Alpha, &QAction::triggered,
                this, &MainWindow::onRemoveImageAlpha);

  this->connect(ui->actionMegre_Component_Images, &QAction::triggered,
                this, &MainWindow::onMergeOneComponentImages);
  //-------------------------------------------------------


  //-------------------------------------------------------
  // SCENE/IMAGE SETTINGS MENU
  this->connect(ui->actionShow_Image_Info, &QAction::triggered,
                this, &MainWindow::onShwImageInfo);


  this->connect(ui->actionShow_Image_Hisogram_02, &QAction::triggered,
                this, &MainWindow::onShowImageHistogramLinePlot);

   this->connect(ui->actionShow_Image_Staitistics, &QAction::triggered,
                 this, &MainWindow::onShwImageStats);

  this->connect(ui->actionSet_Window_Background, &QAction::triggered,
                this, &MainWindow::onSetViewBackground);


  this->connect(ui->actionShow_Image_Histogram, &QAction::triggered,
                this, &MainWindow::onShowImageHistogramBarPlot);




  //-------------------------------------------------------



  //-------------------------------------------------------
  // RUN STATS MENU
  this->connect(ui->actionClear_Runtime_Sum, &QAction::triggered,
                this, &MainWindow::onClearRunTime);
  //--------------------------------------------------------

  //--------------------------------------------------------
  // HELP MENU
  this->connect(ui->actionAbout, &QAction::triggered,
                this, &MainWindow::onShowAboutDialog);
  //--------------------------------------------------------
}

//------------------------------------------------------------------------------
void MainWindow::generateLookupTables()
{
  int tableSize     = 256;
  double deltaColor = 1.0/(tableSize*1.0 - 1.0);

  m_redLookup = vtkSmartPointer<vtkLookupTable>::New();
  m_redLookup->SetNumberOfTableValues(tableSize);
  m_redLookup->SetRange(0, 255);
  m_redLookup->Build();

  m_greenLookup = vtkSmartPointer<vtkLookupTable>::New();
  m_greenLookup->SetNumberOfTableValues(tableSize);
  m_greenLookup->SetRange(0, 255);
  m_greenLookup->Build();

  m_blueLookup = vtkSmartPointer<vtkLookupTable>::New();
  m_blueLookup->SetNumberOfTableValues(tableSize);
  m_blueLookup->SetRange(0, 255);
  m_blueLookup->Build();

  m_defaultLookup = vtkSmartPointer<vtkLookupTable>::New();
  m_defaultLookup->SetNumberOfTableValues(tableSize);
  m_defaultLookup->SetRange(0, 255);
  m_defaultLookup->Build();

  for (int i =0; i < tableSize; i++)
  {
    double di  = 1.0*i;
    double cVal = di*deltaColor;
    // std::cout << "Lookp  i - cVal " <<  i  << ", " << cVal << std::endl;

    m_redLookup->SetTableValue(i     ,   // index
                                cVal ,   // R
                                0    ,   // G
                                0    ,   // B
                                1);      // Alpha

    m_greenLookup->SetTableValue(i   ,   // index
                                0    ,   // R
                                cVal ,   // G
                                0    ,   // B
                                1);      // Alpha

    m_blueLookup->SetTableValue(i    ,   // index
                                0    ,   // R
                                0    ,   // G
                                cVal ,   // B
                                1);      // Alpha
  }
}

//------------------------------------------------------------------------------
void MainWindow::setupToolbar()
{
  ui->toolBar->setIconSize(QSize(TOOLBAR_ICON_SIZE_X,
                                 TOOLBAR_ICON_SIZE_Y));

  m_actionOpenVtkFile   = new QAction(nullptr);
  m_actionOpenVtkFile->setIcon(QIcon(":iconImages/VtkFolderIcon.png"));
  m_actionOpenVtkFile->setToolTip("Load VTK FILE");
  ui->toolBar->addAction(m_actionOpenVtkFile);

  m_actionOpenImageFile = new QAction(nullptr);
  m_actionOpenImageFile->setIcon(QIcon(":iconImages/ImageFolderIcon.png"));
  m_actionOpenImageFile->setToolTip("Load PNG Image File");
  ui->toolBar->addAction(m_actionOpenImageFile);

  m_actionZoomArea      = new QAction(nullptr);
  m_actionZoomArea->setIcon(QIcon(":iconImages/ZoomToIcon.png"));
  m_actionZoomArea->setToolTip("Reset ZOOM");
  ui->toolBar->addAction(m_actionZoomArea);

  m_actionReseView      = new QAction(nullptr);
  m_actionReseView->setIcon(QIcon(":iconImages/ResetIcon.png"));
  m_actionReseView->setToolTip("Reset Orientation");
  ui->toolBar->addAction(m_actionReseView);


  m_actionReloadFile    = new QAction(nullptr);
  m_actionReloadFile->setIcon(QIcon(":iconImages/ReloadFileIcon.png"));
  m_actionReloadFile->setToolTip("Reload Current File");
  ui->toolBar->addAction(m_actionReloadFile);




  QLabel* curFileLabel = new QLabel("Current File: ", nullptr);
  m_currentLoadedFile = new QLineEdit(nullptr);
  m_currentLoadedFile->setReadOnly(true);
  m_currentLoadedFile->setMaximumWidth(CURRENT_FILE_EDIT_WIDTH);

  ui->toolBar->addWidget(curFileLabel);
  ui->toolBar->addWidget(m_currentLoadedFile);

  QLabel* processTimeLab = new QLabel("Process Time (ms):", nullptr);

  m_actionProcessTimerUpdate = new QAction(nullptr);
  m_actionProcessTimerUpdate->setIcon(QIcon(":iconImages/TimerUpdateIcon.png"));
  m_actionProcessTimerUpdate->setToolTip("Update Process Timer");


  m_processTimeEdit = new QLineEdit(nullptr);
  m_processTimeEdit->setReadOnly(true);
  m_processTimeEdit->setMaximumWidth(PROCESS_TIIME_EDIT_WIDTH);
  ui->toolBar->addWidget(processTimeLab);
  ui->toolBar->addAction(m_actionProcessTimerUpdate);
  ui->toolBar->addWidget(m_processTimeEdit);

  this->connect(m_actionOpenVtkFile, &QAction::triggered,
                this, &MainWindow::onOpenVtkFileDialog);

  this->connect(m_actionOpenImageFile, &QAction::triggered,
                this, &MainWindow::onOpenImagePngFileDialog);

  this->connect(m_actionZoomArea, &QAction::triggered,
                this, &MainWindow::onZoomToExtent);

  this->connect(m_actionReseView, &QAction::triggered,
                this, &MainWindow::onResetView);

  this->connect(m_actionReloadFile, &QAction::triggered,
                this, &MainWindow::onReloadFile);

  this->connect(m_actionProcessTimerUpdate, &QAction::triggered,
                this, &MainWindow::onUpdateProcessTimer);
}


//------------------------------------------------------------------------------
void MainWindow::removeImage()
{
  m_imageRef = nullptr;
  m_sceneWidget->removeImages();
}

//------------------------------------------------------------------------------
void MainWindow::replaceCurrentImage(vtkSmartPointer<vtkImageData> newImage)
{
  m_sceneWidget->removeImages();
  m_sceneWidget->addImage(newImage);
  m_imageRef    = newImage.GetPointer();

  this->zoomToFitImage();
}

//------------------------------------------------------------------------------
void MainWindow::invertGrayscaleImage()
{
  //-------------------------------
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  // Scale the image to Unsigned short
  vtkSmartPointer<vtkImageHistogramStatistics> stats =
        vtkSmartPointer<vtkImageHistogramStatistics>::New();
   stats->SetInputConnection(source->GetOutputPort());
   stats->Update();

   double range[2];
   stats->GetAutoRange(range);


   //-----------------
   // gray Extractor
   vtkSmartPointer<vtkImageExtractComponents> grayExtractor =
       vtkSmartPointer<vtkImageExtractComponents>::New();

   grayExtractor->SetInputConnection(source->GetOutputPort());
   grayExtractor->SetComponents(0);
   grayExtractor->Update();

   double grayShift = -255.01;
   // std::cout << "Gray Shift:" << grayShift << std::endl;

   vtkSmartPointer<vtkImageShiftScale>  grayInverter = vtkSmartPointer<vtkImageShiftScale>::New();
   grayInverter->SetInputConnection(grayExtractor->GetOutputPort());
   grayInverter->SetOutputScalarTypeToUnsignedChar();
   grayInverter->SetShift(grayShift);
   grayInverter->SetScale(-1.0);
   grayInverter->Update();


   vtkSmartPointer<vtkImageClip> outputImage = vtkSmartPointer<vtkImageClip>::New();
   outputImage->SetInputConnection(grayInverter->GetOutputPort());
   outputImage->Update();

   vtkSmartPointer<vtkImageData> imageDataOut = vtkSmartPointer<vtkImageData>::New();
   imageDataOut->DeepCopy(outputImage->GetOutput());
   this->replaceCurrentImage(imageDataOut);
}


//------------------------------------------------------------------------------
void MainWindow::invertColorImage()
{
  //-------------------------------
  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();


  //-----------------
  // Red Extractor
  vtkSmartPointer<vtkImageExtractComponents> redExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  redExtractor->SetInputConnection(source->GetOutputPort());
  redExtractor->SetComponents(0);
  redExtractor->Update();
  //------------------

  //-----------------
  // Green Extractor
  vtkSmartPointer<vtkImageExtractComponents> greenExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  greenExtractor->SetInputConnection(source->GetOutputPort());
  greenExtractor->SetComponents(1);
  greenExtractor->Update();
  //------------------

  //-----------------
  // Blue Extractor
  vtkSmartPointer<vtkImageExtractComponents> blueExtractor =
      vtkSmartPointer<vtkImageExtractComponents>::New();

  blueExtractor->SetInputConnection(source->GetOutputPort());
  blueExtractor->SetComponents(2);
  blueExtractor->Update();
  //----------------------------

  // Do the Inversaion
  // Use vtkImageShiftScale object to apply the transformation
  // 255 - px = (px - 255.0) * -1.0

   vtkSmartPointer<vtkImageShiftScale>  redInverter = vtkSmartPointer<vtkImageShiftScale>::New();
   redInverter->SetInputConnection(redExtractor->GetOutputPort());
   redInverter->SetOutputScalarTypeToUnsignedChar();
   redInverter->SetShift(-255.01);
   redInverter->SetScale(-1.0);
   redInverter->Update();

   vtkSmartPointer<vtkImageShiftScale>  greenInverter = vtkSmartPointer<vtkImageShiftScale>::New();
   greenInverter->SetInputConnection(greenExtractor->GetOutputPort());
   greenInverter->SetOutputScalarTypeToUnsignedChar();
   greenInverter->SetShift(-255.01);
   greenInverter->SetScale(-1.0);
   greenInverter->Update();

   vtkSmartPointer<vtkImageShiftScale>  blueInverter = vtkSmartPointer<vtkImageShiftScale>::New();
   blueInverter->SetInputConnection(blueExtractor->GetOutputPort());
   blueInverter->SetOutputScalarTypeToUnsignedChar();
   blueInverter->SetShift(-255.01);
   blueInverter->SetScale(-1.0);
   blueInverter->Update();



  //---------------------------


  vtkSmartPointer<vtkImageAppendComponents> appendFilter =
           vtkSmartPointer<vtkImageAppendComponents>::New();
  appendFilter->SetInputConnection(0, redInverter->GetOutputPort());
  appendFilter->AddInputConnection(0, greenInverter->GetOutputPort());
  appendFilter->AddInputConnection(0, blueInverter->GetOutputPort());
  appendFilter->Update();


  vtkSmartPointer<vtkImageClip> outputImage = vtkSmartPointer<vtkImageClip>::New();
  outputImage->SetInputConnection(appendFilter->GetOutputPort());
  outputImage->Update();

  vtkSmartPointer<vtkImageData> imageDataOut = vtkSmartPointer<vtkImageData>::New();
  imageDataOut->DeepCopy(outputImage->GetOutput());
  this->replaceCurrentImage(imageDataOut);
}


//------------------------------------------------------------------------------
void MainWindow::zoomToFitImage()
{
  m_sceneWidget->performZoom(120);
  m_sceneWidget->performZoom(120);
  m_sceneWidget->performZoom(120);
}


//------------------------------------------------------------------------------
void MainWindow::showImageProperties()
{
  if (!m_imageRef)  return;

  std::stringstream ss;
  m_imageRef->PrintSelf(ss, vtkIndent(2));
  std::string imageProps = ss.str();

  QString messageStr = imageProps.c_str();

  TextInfoDialog textInfo(this);
  textInfo.setTitle("Image Attributes:");
  textInfo.setText(messageStr);
  QString saveFile = m_currentLoadedFile->text();
  saveFile += ".info.txt";
  textInfo.setSaveFile(saveFile);
  textInfo.exec();
}


//------------------------------------------------------------------------------
void MainWindow::showHistogramBarPlot()
{
  if (!m_imageRef)  return;

  int ignoreZero = 0;

  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  int numComponents = m_imageRef->GetNumberOfScalarComponents();
  if( numComponents > 3 )
  {
    QString message = "Error: cannot process an image with ";
    message += QString::number(numComponents) + " components!";
    QMessageBox::information(this,
                             "Erro in Histogram usage:",
                             message);
    return;
  }

  vtkSmartPointer<vtkIntArray> redFrequencies =
      vtkSmartPointer<vtkIntArray>::New();

  vtkSmartPointer<vtkIntArray> greenFrequencies =
      vtkSmartPointer<vtkIntArray>::New();

  vtkSmartPointer<vtkIntArray> blueFrequencies =
      vtkSmartPointer<vtkIntArray>::New();

  // Process the image, extracting and barChartting a histogram for each component
  for( int i = 0; i < numComponents; ++i )
  {
    vtkSmartPointer<vtkImageExtractComponents> extract =
        vtkSmartPointer<vtkImageExtractComponents>::New();
    extract->SetInputConnection(source->GetOutputPort() );
    extract->SetComponents( i );
    extract->Update();

    vtkSmartPointer<vtkImageAccumulate> histogram =
        vtkSmartPointer<vtkImageAccumulate>::New();
    histogram->SetInputConnection( extract->GetOutputPort() );
    histogram->SetComponentExtent(  0, 255, 0, 0, 0, 0 );
    histogram->SetComponentOrigin(  0,   0, 0 );
    histogram->SetComponentSpacing( 1,   0, 0 );
    histogram->SetIgnoreZero(ignoreZero);
    histogram->Update();

    vtkIntArray* currentArray = 0;
    if( i == 0 )
    {
      currentArray = redFrequencies;
    }
    else if( i == 1 )
    {
      currentArray = greenFrequencies;
    }
    else
    {
      currentArray = blueFrequencies;
    }

    currentArray->SetNumberOfComponents(1);
    currentArray->SetNumberOfTuples( 256 );
    int* output = static_cast<int*>(histogram->GetOutput()->GetScalarPointer());

    // int testA = *output;
    // std::cout << "TEST A: "<< testA <<  std::endl;

    for( int j = 0; j < 256; ++j )
    {
      currentArray->SetTuple1( j, *output++ );
    }
  }

  vtkSmartPointer<vtkDataObject> dataObject =
      vtkSmartPointer<vtkDataObject>::New();

  if( numComponents == 1 )
  {
    dataObject->GetFieldData()->AddArray( redFrequencies );
  }
  else
  {
    vtkIntArray* rgb[3] = {0, 0, 0};
    vtkSmartPointer<vtkIntArray> allFrequencies = vtkSmartPointer<vtkIntArray>::New();
    allFrequencies->SetNumberOfComponents(1);
    if( numComponents == 2 )
    {
      rgb[0] = redFrequencies;
      rgb[1] = greenFrequencies;
    }
    else
    {
      rgb[0] = redFrequencies;
      rgb[1] = greenFrequencies;
      rgb[2] = blueFrequencies;
    }
    for( int i = 0; i < 256; ++i )
    {
      for( int j = 0; j < numComponents; ++j )
      {
        allFrequencies->InsertNextTuple1( rgb[j]->GetTuple1( i ) );
      }
      // std::cout << "RED at I:"<< i << "," <<rgb[0]->GetTuple1(i) << std::endl;
    }
    dataObject->GetFieldData()->AddArray( allFrequencies );
  }



  // Create a vtkBarChartActor
  vtkSmartPointer<vtkBarChartActor> barChart =
      vtkSmartPointer<vtkBarChartActor>::New();

  // BUG 1: if input is not set first, the x-axis of the bar chart will be too long
  //
  barChart->SetInput( dataObject );
  barChart->SetTitle( "Histogram" );
  barChart->GetPositionCoordinate()->SetValue(g_histogramChart_x0,
                                              g_histogramChart_y0,
                                              g_histogramChart_z0);

  barChart->GetPosition2Coordinate()->SetValue(g_histogramChart_x1,
                                               g_histogramChart_y1,
                                               g_histogramChart_z1);

  barChart->GetProperty()->SetColor(g_histo_chart_color_red,
                                    g_histo_chart_color_green,
                                    g_histo_chart_color_blue);
  //---------------------


  //--------------------

  // BUG 2: if the number of entries is not set to the number of data array tuples, the bar chart actor will crash.
  // The crash occurs whether the legend and or labels are visible or not.

  barChart->GetLegendActor()->SetNumberOfEntries( dataObject->GetFieldData()->GetArray(0)->GetNumberOfTuples() );
  barChart->LegendVisibilityOff();
  barChart->LabelVisibilityOff();

  // barChart->G

  // BUG 3: the y-axis labels do not accurately reflect the range of data

  int count = 0;
  for( int i = 0; i < 256; ++i )
  {
    for( int j = 0; j < numComponents; ++j )
    {
      barChart->SetBarColor( count++, g_HistogramBarColors[j] );
    }
  }

  // ARO-TODO: Move to Scene Widget! With Remove Chart.
  vtkRenderer* sceneRenderer = m_sceneWidget->getRenderer();
  if (sceneRenderer)
  {
    sceneRenderer->AddActor(barChart);
    m_sceneWidget->doRender();
  }
}



//------------------------------------------------------------------------------
void MainWindow::showHistogramLinePlot()
{
  if (!m_imageRef)  return;

  int ignoreZero = 0;

  // Get Source Image
  vtkSmartPointer<vtkImageClip> source = vtkSmartPointer<vtkImageClip>::New();
  source->SetInputData(m_imageRef);
  source->Update();

  int numComponents = m_imageRef->GetNumberOfScalarComponents();

  if( numComponents > 3 )
  {
    QString message = "Error: cannot process an image with ";
    message += QString::number(numComponents) + " components!";
    QMessageBox::information(this,
                             "Erro in Histogram usage:",
                             message);
    return;
  }

  // Create a vtkXYPlotActor
  vtkSmartPointer<vtkXYPlotActor> plot =
      vtkSmartPointer<vtkXYPlotActor>::New();
  plot->ExchangeAxesOff();
  plot->SetLabelFormat( "%g" );
  plot->SetXTitle( "Level" );
  plot->SetYTitle( "Freq" );
  plot->SetXValuesToValue();
  plot->SetTitle( "Histogram" );
  //====================
  plot->GetPositionCoordinate()->SetValue(g_histogramChart_x0,
                                          g_histogramChart_y0,
                                          g_histogramChart_z0);

  plot->GetPosition2Coordinate()->SetValue(g_histogramChart_x1,
                                           g_histogramChart_y1,
                                           g_histogramChart_z1);



  plot->GetProperty()->SetColor(g_histo_chart_color_red,
                                g_histo_chart_color_green,
                                g_histo_chart_color_blue);
  //======================
  double xmax = 0.;
  double ymax = 0.;

  double colors[3][3] =
  {
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 }
  };

  const char* labels[3] =
  {
    "Red", "Green", "Blue"
  };

  // Process the image, extracting and plotting a histogram for each
  // component
  for( int i = 0; i < numComponents; ++i )
  {
    vtkSmartPointer<vtkImageExtractComponents> extract =
        vtkSmartPointer<vtkImageExtractComponents>::New();
    extract->SetInputConnection( source->GetOutputPort() );
    extract->SetComponents( i );
    extract->Update();

    double range[2];
    extract->GetOutput()->GetScalarRange( range );

    vtkSmartPointer<vtkImageAccumulate> histogram =
        vtkSmartPointer<vtkImageAccumulate>::New();
    histogram->SetInputConnection( extract->GetOutputPort() );
    histogram->SetComponentExtent( 0,
                                   static_cast<int>(range[1])-static_cast<int>(range[0]) -1, 0, 0, 0, 0 );
    histogram->SetComponentOrigin( range[0], 0, 0 );
    histogram->SetComponentSpacing( 1, 0, 0 );
    histogram->SetIgnoreZero( ignoreZero );
    histogram->Update();

    if( range[1] > xmax )
    {
      xmax = range[1];
    }
    if( histogram->GetOutput()->GetScalarRange()[1] > ymax )
    {
      ymax = histogram->GetOutput()->GetScalarRange()[1];
    }

    plot->AddDataSetInputConnection( histogram->GetOutputPort() );
    if( numComponents > 1 )
    {
      plot->SetPlotColor(i, colors[i]);
      plot->SetPlotLabel(i, labels[i]);
      plot->LegendOn();
    }
  }
  plot->SetXRange( 0, xmax );
  plot->SetYRange( 0, ymax );


  // ARO-TODO: Move to Scene Widget! With Remove Chart.
  vtkRenderer* sceneRenderer = m_sceneWidget->getRenderer();
  if (sceneRenderer)
  {
    sceneRenderer->AddActor(plot);
    m_sceneWidget->doRender();
  }
}

//------------------------------------------------------------------------------
void MainWindow::showImageStats(vtkSmartPointer<vtkImageClip> anImage)
{
  //=====================================================
  // Scale the image to Unsigned short
  vtkSmartPointer<vtkImageHistogramStatistics> stats =
      vtkSmartPointer<vtkImageHistogramStatistics>::New();
  stats->SetInputConnection(anImage->GetOutputPort());
  stats->Update();

  double range[2];
  stats->GetAutoRange(range);

  // Filter outpout Range
  QString statRange =  "Scale Filter output range R0 - R1 :";
  statRange +=     QString::number(range[0])  +
                   " - "  +
                   QString::number(range[1]);


  std::stringstream streamStat;
  stats->PrintSelf(streamStat, vtkIndent(2));
  std::string statProps = streamStat.str();

  QString messageStr = statProps.c_str();
  messageStr += "****************** END SECTION ******************\n";

  messageStr +=   statRange + "\n";

  messageStr += "****************** END SECTION ******************\n";
  //=====================================================

  std::stringstream ss;
  anImage->PrintSelf(ss, vtkIndent(2));
  std::string imageProps = ss.str();

  messageStr += imageProps.c_str();


  TextInfoDialog textInfo(this);
  textInfo.setTitle("Image Stats Info:");
  textInfo.setText(messageStr);
  QString saveFile = m_currentLoadedFile->text();
  saveFile += ".stats-info.txt";
  textInfo.setSaveFile(saveFile);
  textInfo.exec();
}

//------------------------------------------------------------------------------
void MainWindow::setCurrentFileAndPath(QString& fullPath,
                                       QString& shortName)
{
  m_currentLoadedFile->setText(shortName);
  m_currentFileFullPath = fullPath;
}
