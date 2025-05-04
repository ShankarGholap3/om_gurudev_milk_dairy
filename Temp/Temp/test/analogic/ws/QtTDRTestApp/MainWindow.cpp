
//------------------------------------------------------------------------------
// File: MainWindow.cpp
// Description: Main Window Implemenetation - SDICOS TDR and Qt
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------
#include "MainWindow.h"
#include "ui_MainWindow.h"

QString g_tdrFileDir       = "/home/analogic/devData/VolumeData/TDR";
QString g_errorLogfile     = "/home/analogic/junk/SDICOSError.log";
QString g_attribListFile   = "/home/analogic/junk/AttribList.txt";
QString m_defaultSavePath  = "/home/analogic/junk/";


QString g_mainWindowTitile = "SDICOS TDR Reader/Writer Application";

int g_threatComboWidth     = 180;
int g_currentTdrNameWidth  = 150;

double g_densityAluminum    = 2.7;     // g/cm^3  grams per cubic centimeters
double g_pixelVolume        = 0.96;    // mm^3  1.0*1.0*0.96   (cubic millimeters)
double g_cubicCMPerCubuicMM = 0.001;   // cm^3/mm^3   (cubic millimeters/cubic centimeters)


typedef analogic::workstation::ThreatAlarmType  threatType;


QStringList g_threatNames =
{
  "Explosive-Threat",
  "Liquid-Threat",
  "Weapon-Threat",
  "Contraband-Threat",
  "Anomoly-Threat",
  "Bulk-Threat",
  "Shield-Threat",
  "LiquidAerosolGas-Threat",
};
// comboBoxThreatSelect
QStringList g_threatImageList =
{
  ":images/Explosive.png",
  ":images/Liquid.png",
  ":images/Weapon.png",
  ":images/Contraband.png",
  ":images/Anomoly.png",
  ":images/Bulk.png",
  ":images/Shield.png",
  ":images/LiquidAerosolGas.png",
};

std::vector<int> g_threatThickness =
{
  26,
  10,
  26,
  10,
  8,
  5,
  10,
  30,
};


//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_console(nullptr),
  m_tdrModule(nullptr),
  m_tdrListCombo(nullptr),
  m_image(nullptr),
  m_clearButton(nullptr)
{
  ui->setupUi(this);

  this->setWindowIcon(QIcon(":images/ThreatIcon.png"));
  m_console = new ConsoleWidget(ui->plainTextEdit);
  this->setupMenu();
  this->setWindowTitle(g_mainWindowTitile);
  this->setupToolBar();
  this->setupSlider();
  this->setupThreatCombo();
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete ui;
}

//------------------------------------------------------------------------------
void MainWindow::onAppExit()
{
  this->close();
}

//------------------------------------------------------------------------------
void MainWindow::onConsoleClearButton()
{
  m_console->clear();
}



//------------------------------------------------------------------------------
void MainWindow::onShowThreatInfo()
{
  if (m_tdrListCombo->count() == 0)
  {
    m_console->logString("No threats loaded!");
    return;
  }
  int curIndex = m_tdrListCombo->currentIndex();
  if (curIndex > (m_threatVolumeList.size() -1 ) )
  {
    QString message = "Error on Selected index. Index greater than Size of Threat List.";
    m_console->logString(message);
    return;
  }
  ThreatVolume tv = m_threatVolumeList.at(curIndex);
  this->showThreatInfo(curIndex, tv);
}


//------------------------------------------------------------------------------
void MainWindow::onReadTDRFromFile()
{
  QFileDialog fdlg(this);
  QString nameFilter = TDR_FILE_FILTER;
  QString selectedFilename;

  fdlg.setAcceptMode(QFileDialog::AcceptOpen);
  fdlg.setNameFilter(nameFilter);
  fdlg.setDirectory(g_tdrFileDir);
  bool bRet = fdlg.exec();
  if(bRet)
  {
    QStringList fileList = fdlg.selectedFiles();
    if (fileList.size() > 0)
    {
      selectedFilename = fileList.at(0);
      m_console->logString(selectedFilename);
      this->readTDRFile(selectedFilename);
    }
  }
}


//------------------------------------------------------------------------------
void MainWindow::onSliceSliderValuechanged()
{
 if (m_threatVolumeList.size() == 0) return;
 int slicePosition = this->bmpSliceFromSliderPosition();
 int threatPos     = m_tdrListCombo->currentIndex();
 SDICOS::Bitmap& bmp = m_threatBMPList.at(threatPos);
 this->showBMP(slicePosition, bmp);
}



//------------------------------------------------------------------------------
void MainWindow::onSaveConsole()
{
  QFileInfo  saveFileInfo(m_defaultSavePath);
  QString    filePath = saveFileInfo.absolutePath();
  QFileDialog newFileDlg(this,
                         "Select Output File",
                         filePath,
                         CONSOLE_TEXT_FILE_FILTER);
  newFileDlg.setAcceptMode(QFileDialog::AcceptSave);

  bool dlgRet =  newFileDlg.exec();
  if (dlgRet)
  {
    QString saveFile = newFileDlg.selectedFiles().first();
    this->saveConsoleToFile(saveFile);
  }
}

//-----------------------------------------------------------------------------
void MainWindow::onSaveTDRAs()
{
  if (!m_tdrModule)
  {
    QMessageBox::information(this,
                             "Error in usage!",
                             "No TDR currently loaded!");
    return;
  }

  QFileInfo  saveFileInfo(m_defaultSavePath);
  QString    filePath = saveFileInfo.absolutePath();
  QFileDialog newFileDlg(this,
                         "Save TDR as ...",
                         filePath,
                         TDR_FILE_FILTER);
  newFileDlg.setAcceptMode(QFileDialog::AcceptSave);

  bool dlgRet =  newFileDlg.exec();
  if (dlgRet)
  {
    QString saveFile = newFileDlg.selectedFiles().first();
    this->saveTDRToFile(saveFile);
  }
}


//-----------------------------------------------------------------------------
void MainWindow::onMoveVolume()
{
  if (m_tdrListCombo->count() == 0)
  {
    m_console->logString("No threats loaded!");
    return;
  }
  int curIndex = m_tdrListCombo->currentIndex();
  if (curIndex > (m_threatVolumeList.size() -1 ) )
  {
    QString message = "Error on Selected index. Index greater than Size of Threat List.";
    m_console->logString(message);
    return;
  }
  ThreatVolume tv = m_threatVolumeList.at(curIndex);
  double dx = tv.p0().x();
  double dy = tv.p0().y();
  double dz = tv.p0().z();

  QString sMsg = "P0(x, y, z) = ";
  sMsg += QString::number(dx);
  sMsg += ", ";
  sMsg += QString::number(dy);
  sMsg += ", ";
  sMsg += QString::number(dz);
  m_console->logString(sMsg);

  InputDialog3D dlg3D(this);

  dlg3D.setValues(dx, dy, dz);

  dlg3D.setTitleAndHeader("Current P0(x,y,z)",
                          "Input New P0(x,y,z)");

  int dlgResp = dlg3D.exec();
  double newX, newY, newZ;
  if (dlgResp)
  {
    dlg3D.getValues(newX, newY, newZ);
    double deltaX = newX - dx;
    double deltaY = newY - dy;
    double deltaZ = newZ - dz;

    QString msgMove = "Moving: (DX,DY,DZ)";
    msgMove += QString::number(deltaX) + ", ";
    msgMove += QString::number(deltaY) + ", ";
    msgMove += QString::number(deltaZ);
    m_console->logString(msgMove);
    QVector3D vecMove(deltaX,
                      deltaY,
                      deltaZ);
    this->moveTDR(curIndex, vecMove);
  }
}

//------------------------------------------------------------------------------
void MainWindow::onResizeVolume()
{
  if (m_tdrListCombo->count() == 0)
  {
    m_console->logString("No threats loaded!");
    return;
  }
  int curIndex = m_tdrListCombo->currentIndex();
  if (curIndex > (m_threatVolumeList.size() -1 ) )
  {
    QString message = "Error on Selected index. Index greater than Size of Threat List.";
    m_console->logString(message);
    return;
  }
  ThreatVolume tv = m_threatVolumeList.at(curIndex);

  int w = tv.p1().x() - tv.p0().x();
  int h = tv.p1().y() - tv.p0().y();
  int l = tv.p1().z() - tv.p0().z();



  QString sMsg = "Dimensions(w, h, l) = ";
  sMsg += QString::number(w);
  sMsg += ", ";
  sMsg += QString::number(h);
  sMsg += ", ";
  sMsg += QString::number(l);
  m_console->logString(sMsg);

  InputDialog3D dlg3D(this);

  dlg3D.setValues(w, h, l);

  dlg3D.setTitleAndHeader("Current Dimensi(W,H,L)",
                          "Input New Dim(W,H,L)");

  int dlgResp = dlg3D.exec();
  if (dlgResp)
  {
    double newW;
    double newH;
    double newL;
    dlg3D.getValues(newW, newH, newL);
    QVector3D newSize(newW, newH, newL);
    this->resizeTDR(curIndex,
                    newSize);
  }
}

//------------------------------------------------------------------------------
void MainWindow::onAddTestThreat()
{
  if (m_tdrListCombo->count() == 0)
  {
    m_console->logString("No threats loaded!");
    return;
  }
  int curIndex = m_tdrListCombo->currentIndex();
  if (curIndex > (m_threatVolumeList.size() -1 ) )
  {
    QString message = "Error on Selected index. Index greater than Size of Threat List.";
    m_console->logString(message);
    return;
  }
  QString whichAxis =  this->pickExtrudeDirection();

  if (whichAxis.compare("X") == 0)
  {
    this->addThreatExtrudeAlognX(curIndex);
  }
  if (whichAxis.compare("Y") == 0)
  {
    this->addThreatExtrudeAlognY(curIndex);
  }
  if (whichAxis.compare("Z") == 0)
  {
    this->addThreatExtrudeAlognZ(curIndex);
  }
}

//------------------------------------------------------------------------------
void MainWindow::setupMenu()
{
  this->connect(ui->actionExit, &QAction::triggered,
                this, &MainWindow::onAppExit);


  this->connect(ui->actionRead_TDR_from_file, &QAction::triggered,
                this, &MainWindow::onReadTDRFromFile);

  this->connect(ui->actionShowThreatInfo, &QAction::triggered,
                this, &MainWindow::onShowThreatInfo);

  this->connect(ui->actionSave_Console_to_File, &QAction::triggered,
                this, &MainWindow::onSaveConsole);

  this->connect(ui->actionSave_TDR_as, &QAction::triggered,
                this,  &MainWindow::onSaveTDRAs);

  this->connect(ui->actionMoveThreatVolume, &QAction::triggered,
                this, &MainWindow::onMoveVolume);

  this->connect(ui->actionAddSelectedThreat, &QAction::triggered,
                this, &MainWindow::onAddTestThreat);

  this->connect(ui->actionResize_Threat_Volume, &QAction::triggered,
                this, &MainWindow::onResizeVolume);

  this->connect(ui->actionSet_Threat_Type, &QAction::triggered,
                this, &MainWindow::onSetThreatType);

  this->connect(ui->actionSetMass, &QAction::triggered,
                this, &MainWindow::onVolumeSetMass);

  this->connect(ui->actionClear_All_Threats, &QAction::triggered,
                this, &MainWindow::onClearAllThreats);
}


//------------------------------------------------------------------------------
void MainWindow::setupToolBar()
{
  // Threat List Combo:
  QLabel* comboLabel = new QLabel("Threat List:", nullptr);
  m_tdrListCombo  = new QComboBox(nullptr);
  m_tdrListCombo->setMinimumWidth(g_threatComboWidth);
  ui->mainToolBar->addWidget(comboLabel);
  ui->mainToolBar->addWidget(m_tdrListCombo);

  // Clear Console
  m_clearButton = new QPushButton("Clear", nullptr);
  m_clearButton->setIcon(QPixmap(":images/EraserIcon.png"));
  ui->mainToolBar->addWidget(m_clearButton);
  this->connect(m_clearButton, &QPushButton::clicked,
                this, &MainWindow::onConsoleClearButton);

  // Current TDR File
  QLabel* nameLabel = new QLabel("Current TDR File:", nullptr);
  ui->mainToolBar->addWidget(nameLabel);
  m_tdrShortName = new QLineEdit(this);
  m_tdrShortName->setReadOnly(true);
  m_tdrShortName->setMaximumWidth(g_currentTdrNameWidth);
  ui->mainToolBar->addWidget(m_tdrShortName);
}


//------------------------------------------------------------------------------
void MainWindow::setupSlider()
{
  this->connect(ui->imageLayerSlider, &QSlider::valueChanged,
                this, &MainWindow::onSliceSliderValuechanged);
}

//------------------------------------------------------------------------------
void MainWindow::setupThreatCombo()
{
  int nCombosize =  g_threatNames.size();
  for (int i = 0;  i < nCombosize; i++)
  {
    ui->comboBoxThreatSelect->addItem(g_threatNames.at(i));
  }

  QString sImage = g_threatImageList.at(0);

  ui->testThreatImage->setPixmap(QPixmap(sImage));

  this->connect(ui->comboBoxThreatSelect, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &MainWindow::onThreatComboSlectionChanged);
}



//------------------------------------------------------------------------------
void MainWindow::writeAttributesFile()
{
  if (!m_tdrListCombo) return;
  SDICOS::ErrorLog errorlog;
  SDICOS::Filename attribListFilename(g_attribListFile.toLocal8Bit().data());
  bool bResp = m_tdrModule->WriteAttributeList(attribListFilename, errorlog);
  if (!bResp)
  {
    QString message = "Error writing Attibutes to file:";
    message += g_attribListFile;
    m_console->logString(message);
    return;
  }
  if(errorlog.HasErrors() || errorlog.HasWarnings())
  {
    errorlog.WriteLog(SDICOS::Filename(g_errorLogfile.toLocal8Bit().data()));
    std::cout << errorlog << std::endl;
    QString erroMessage = "Write Attributes Error on TDR File - ";
    erroMessage +=  errorlog.GetErrorLog().Get();
    m_console->logString(erroMessage);
    return;
  }
  else
  {
    m_console->logString("Write Attributes to file completed without errors or warnings!");
  }
}


//------------------------------------------------------------------------------
void MainWindow::showBMP(int layer, SDICOS::Bitmap& bmp)
{
  if (m_image)
  {
    delete m_image;
  }
  SDICOS::S_UINT32 bm_w;
  SDICOS::S_UINT32 bm_h;
  SDICOS::S_UINT32 bm_l;
  bmp.GetDims(bm_w, bm_h, bm_l);
  m_image = new QImage(bm_w, bm_h, QImage::Format_ARGB32);

  // qDebug() << "IMAGE WxH:" << bm_w << ", " << bm_h;

  for (int x = 0; x < bm_w; x++)
  {
    for (int y = 0; y < bm_h; y++)
    {
     bool pixOn = bmp.GetBit(x, y, layer);
     QPoint pixPos(x, y);
     if (pixOn)
     {
        m_image->setPixelColor(pixPos, Qt::white);
     }
     else
     {
        m_image->setPixelColor(pixPos, Qt::black);
     }
    }
  }
  QPixmap pixMap;
  pixMap = pixMap.fromImage(*m_image);
  ui->imageLabel->setPixmap(pixMap);
  ui->imageLabel->setMinimumSize(bm_w, bm_h);
  ui->imageLabel->setMaximumSize(bm_w, bm_h);
}



//------------------------------------------------------------------------------
void MainWindow::showThreatInfo(int id, ThreatVolume& tv)
{
   // THREAT TYPE
   analogic::workstation::ThreatAlarmType ttype =
       m_threatTypeList.at(id);
   QString threatTypeStr = "Threat Type: ";
   threatTypeStr += analogic::workstation::threatTypeName(ttype);
   m_console->logString(threatTypeStr);



  QString sRoi =  "Threat Volume [" + QString::number(id + 1);
  sRoi += "] P0 = ";

  sRoi +=  QString::number(tv.p0().x()) + ", " +
           QString::number(tv.p0().y()) + ", " +
           QString::number(tv.p0().z());

  sRoi += "  P1 = ";

  sRoi +=  QString::number(tv.p1().x()) + ", " +
           QString::number(tv.p1().y()) + ", " +
           QString::number(tv.p1().z());

  m_console->logString(sRoi);



  QString massInfo = "Threat Mass (g):";
  double dM = m_massList.at(id);
  massInfo += QString::number(dM);
  m_console->logString(massInfo);


  SDICOS::Bitmap& bmp = m_threatBMPList.at(id);
  SDICOS::S_UINT32 wx, wy, wz;
  bmp.GetDims(wx, wy, wz);
  QString bmpMessage = "BMP: W,H,L ";

  bmpMessage += QString::number(wx) + ", " +
                QString::number(wy) + ", " +
                QString::number(wz);

  m_console->logString(bmpMessage);
  int slicePosition = this->bmpSliceFromSliderPosition();
  QString sldrMessage = "Slice Position: " + QString::number(slicePosition);
  m_console->logString(sldrMessage);
  this->showBMP(slicePosition, bmp);


  QString threatStatInfo = "Threat Status:";
  threatStatInfo += m_threatStatus.at(id);
  m_console->logString(threatStatInfo);
}

//------------------------------------------------------------------------------
int  MainWindow::bmpSliceFromSliderPosition()
{
  int threatLoc = m_tdrListCombo->currentIndex();
  SDICOS::Bitmap& bmpCur = m_threatBMPList.at(threatLoc);
  SDICOS::S_UINT32 W, H, L;
  bmpCur.GetDims(W, H, L);
  int cursLiderPos = ui->imageLayerSlider->value();
  // Note 0 - 99 maps to 0 - L-1
  // Y = m*X + Bm_threatStatus
  // 0 = 0*X + B  => B = 0;
  // m = Delta(Y)/Delta(X).
  // m = (L -1)/99
  double m = ( L - 1.0) /99.0;
  double slicePosd = m*(1.0*cursLiderPos);
  int retVal = slicePosd;
  return retVal;
}

//------------------------------------------------------------------------------
void MainWindow::readTDRFile(QString filenameTDR)
{
  if (m_tdrListCombo->maxCount() > 0)
  {
    m_tdrListCombo->clear();
    m_threatVolumeList.clear();
    m_threatBMPList.clear();
    m_threatStatus.clear();
  }
  if (m_tdrModule)
  {
    delete m_tdrModule;
    m_tdrModule = nullptr;
  }
  std::string strTDR = filenameTDR.toLocal8Bit().data();
  SDICOS::ErrorLog errorlog;
  SDICOS::Filename tdrFilename(strTDR);


  m_tdrModule = new SDICOS::TDRModule();

  bool bResp = m_tdrModule->Read(tdrFilename, errorlog);
  if (!bResp)
  {
    QString message = "Error reading TDR file:";
    message += filenameTDR;
    m_console->logString(message);
  }


  if(errorlog.HasErrors() || errorlog.HasWarnings())
  {
    errorlog.WriteLog(SDICOS::Filename(g_errorLogfile.toLocal8Bit().data()));
    std::cout << errorlog << std::endl;
    QString erroMessage = "Read Error on TDR File - ";
    erroMessage +=  errorlog.GetErrorLog().Get();
    m_console->logString(erroMessage);
    return;
  }
  else
  {
    m_console->logString("TDR File read without errors or warnings!");
    QFileInfo fInfo(filenameTDR);
    QString shortName = fInfo.fileName();
    m_tdrShortName->setText(shortName);
  }



  SDICOS::TDRTypes::ThreatDetectionReport& pReport = m_tdrModule->GetThreatDetectionReport();
  int nAlarms = pReport.GetNumberOfAlarmObjects();

  QString countMessage = "Number Alarms: " + QString::number(nAlarms);
  m_console->logString(countMessage);

  int nTotalObjects =  pReport.GetNumberOfTotalObjects();
  countMessage = "Number of Total Objects: " + QString::number(nTotalObjects);
  m_console->logString(countMessage);


  SDICOS::TDRTypes::ThreatSequence &ts = m_tdrModule->GetThreatSequence();

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects =
            ts.GetPotentialThreatObject();


  int nTI = threatObjects.GetSize();
  countMessage = "ThreatObject size: " + QString::number(nTI);

  m_console->logString(countMessage);

  for (int i = 0; i < nTI; i++)
  {
    SDICOS::S_UINT16 tid = threatObjects[i].GetID();
    QString treatId = "Threat ID: " + QString::number(tid);
    m_console->logString(treatId);
    m_tdrListCombo->addItem(treatId);
    float xb, yb, zb;
    float xt, yt, zt;
    threatObjects[i].GetBaggageTDR().GetThreatROI().GetThreatRoiBase(xb, yb, zb);
    threatObjects[i].GetBaggageTDR().GetThreatROI().GetThreatRoiExtents(xt, yt, zt);
    QVector3D p0 = QVector3D(xb, yb, zb);
    QVector3D p1 = QVector3D(xb + xt,
                             yb + yt,
                             zb + zt);

    //--------------------------------
    // Threat Type

    SDICOS::TDRTypes::ThreatSequence::ThreatItem& thretItem = threatObjects[i];

    analogic::workstation::ThreatAlarmType thType =
        analogic::workstation::getThreatAlarmTypeFromTdrThreatObject(thretItem);
    m_threatTypeList.push_back(thType);
    //---------------------------------

    //--------------------------------
    // Threat Mass
     double massG = threatObjects[i].GetBaggageTDR().GetMass();
    //--------------------------------


    ThreatVolume tv(p0, p1);
    m_threatVolumeList.push_back(tv);
    m_massList.push_back(massG);

    SDICOS::Bitmap& bmp = threatObjects[i].GetBaggageTDR().GetThreatROI().GetThreatRoiBitmap();
    m_threatBMPList.push_back(bmp);




    SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem>& assmentItems =
        threatObjects[i].GetAssessment().GetAssessment();
    if (assmentItems.GetSize() > 0)
    {
      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG aFlag =
          assmentItems[0].GetAssessmentFlag();

      QString assementFlag;
      if (aFlag == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat)
      {
       assementFlag = "THREAT";
      }


      if (aFlag == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknownAssessmentFlag)
      {
        assementFlag = "UNKNOWN-ASSESMENT";
      }

      if (aFlag == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
      {
        assementFlag = "NO-THREAT";
      }

      if (aFlag == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknown)
      {
        assementFlag = "UNKNOWN";
      }
      QString message = "Aassesment Flag:";
      message +=  assementFlag;
      m_console->logString(message);
      m_threatStatus.push_back(assementFlag);
    }
  }
}


//------------------------------------------------------------------------------
void MainWindow::moveTDR(int index, QVector3D& vecMove)
{
  SDICOS::TDRTypes::ThreatSequence &ts = m_tdrModule->GetThreatSequence();

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects =
            ts.GetPotentialThreatObject();


  float xb, yb, zb;
  float xt, yt, zt;
  threatObjects[index].GetBaggageTDR().GetThreatROI().GetThreatRoiBase(xb, yb, zb);
  threatObjects[index].GetBaggageTDR().GetThreatROI().GetThreatRoiExtents(xt, yt, zt);

  float xm, ym, zm;
  xm = xb + vecMove.x();
  ym = yb + vecMove.y();
  zm = zb + vecMove.z();

  threatObjects[index].GetBaggageTDR().GetThreatROI().SetThreatRoiBase(xm, ym, zm);

  QVector3D p0 = QVector3D(xm, ym, zm);
  QVector3D p1 = QVector3D(xm + xt,
                           ym + yt,
                           zm + zt);
  ThreatVolume tv(p0, p1);
  SDICOS::ErrorLog errorLog;
  m_threatVolumeList[index] = tv;

  // ARO-TODO:  WORKING HERE
}

//------------------------------------------------------------------------------
void MainWindow::clearaAllThreats()
{
//  QMessageBox::information(this,
//                           "Unimplimented functionality",
//                           "Clear All threats not currently Implemented");

   SDICOS::TDRTypes::ThreatSequence &ts = m_tdrModule->GetThreatSequence();
   ts.FreeMemory();




   // SDICOS::TDRTypes::ThreatSequence::ThreatItem ti;

   // ts.AddPotentialThreatObject(ti);

   SDICOS::TDRTypes::ThreatDetectionReport& tdrType = m_tdrModule->GetThreatDetectionReport();
   tdrType.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumClear);

   m_threatVolumeList.clear();
   m_threatBMPList.clear();
   m_threatTypeList.clear();
   m_massList.clear();
   m_tdrListCombo->clear();
}

//------------------------------------------------------------------------------
void MainWindow::setTDRThreatType(int index,
                                  analogic::workstation::ThreatAlarmType aType)
{
  SDICOS::TDRTypes::ThreatSequence &ts = m_tdrModule->GetThreatSequence();

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects =
            ts.GetPotentialThreatObject();

  SDICOS::TDRTypes::ThreatSequence::ThreatItem& threatItem = threatObjects[index];

  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::THREAT_CATEGORY sDicosCat =
      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknownThreatCategory;


  switch(aType)
  {
  case analogic::workstation::EXPLOSIVE:
     sDicosCat = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumExplosive;
    break;
  case analogic::workstation::LIQUID:
    sDicosCat = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumLiquid;
    break;
  case analogic::workstation::WEAPON:
     sDicosCat = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumWeapon;
    break;
  case analogic::workstation::CONTRABAND:
    sDicosCat = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumContraband;
    break;
  case analogic::workstation::ANOMALY:
    sDicosCat = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumAnomaly;
    break;
  case analogic::workstation::SHIELD:
    break;
  }
  // Analogic defined threats.
  // We can add confitions here when the other Analogic definitions are made
  if (
       (aType == analogic::workstation::SHIELD) ||
       (aType == analogic::workstation::UNDEFINED)
      )
  {
    SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem>&  assesmentArray
        = threatItem.GetAssessment().GetAssessment();
    SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ABILITY_ASSESSMENT abilityAssement =
         SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumShield;
        assesmentArray[0].SetAbilityAssessment(abilityAssement);
  }
  else
  {
    SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem>&  assesmentArray
        = threatItem.GetAssessment().GetAssessment();
    SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ABILITY_ASSESSMENT abilityAssement =
         SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoInterference;
        assesmentArray[0].SetAbilityAssessment(abilityAssement);
    // SDICOS Defined Threats
    threatItem.GetAssessment().GetAssessment()[0].SetThreatCategory(sDicosCat);


    SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem>&
         asssmentList = threatItem.GetAssessment().GetAssessment();
    for (int i = 0;  i < asssmentList.GetSize(); i++)
    {
      asssmentList[i].SetAssessmentFlag(SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat);
    }
  }
}

//------------------------------------------------------------------------------
void MainWindow::setTDRMass(int index)
{
  SDICOS::TDRTypes::ThreatSequence &ts = m_tdrModule->GetThreatSequence();

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects =
            ts.GetPotentialThreatObject();

  SDICOS::Bitmap& bmp = threatObjects[index].GetBaggageTDR().GetThreatROI().GetThreatRoiBitmap();
  SDICOS::S_UINT32  w;
  SDICOS::S_UINT32  h;
  SDICOS::S_UINT32  l;
  double threatVolume = 0;
  bmp.GetDims(w, h, l);

  for ( int i = 0;  i < w; i++)
  {
    for ( int j = 0; j < h; j++)
    {
      for ( int k = 0; k < l; k++)
      {
         bool pixOn = bmp.GetBit(i, j, k);
         if (pixOn)
         {
           threatVolume += g_pixelVolume;
         }
      }
    }
  }
  double threatMass = threatVolume*g_densityAluminum*g_cubicCMPerCubuicMM;
  QString computedMass = "Computed mass (g): ";
  computedMass+= QString::number(threatMass);
  m_console->logString(computedMass);
  threatObjects[index].GetBaggageTDR().SetMass(threatMass);
  m_massList[index] = threatMass;
}

//------------------------------------------------------------------------------
QString MainWindow::pickExtrudeDirection()
{
  QStringList listDirections = {
    "Extrued in X",
    "Extrued in Y",
    "Extrued in Z",
  };
  QString sRet = "";
  QInputDialog selectExtreudeDirDlg(this);
  selectExtreudeDirDlg.setOption(QInputDialog::UseListViewForComboBoxItems);
  selectExtreudeDirDlg.setWindowTitle("Select Extrusion Direction");
  selectExtreudeDirDlg.setLabelText(tr("Type Selection:"));
  selectExtreudeDirDlg.setComboBoxItems(listDirections);
  int ret = selectExtreudeDirDlg.exec();
  if  (ret == QDialog::Accepted)
  {
     QString selText = selectExtreudeDirDlg.textValue();
     if (selText.compare(listDirections.at(0)) ==0)
     {
       sRet ="X";
     }
     if (selText.compare(listDirections.at(1)) ==0)
     {
       sRet ="Y";
     }
     if (selText.compare(listDirections.at(2)) ==0)
     {
       sRet ="Z";
     }
 }
 return sRet;
}


//------------------------------------------------------------------------------
void MainWindow::addThreatExtrudeAlognX(int curentIndex)
{
  ThreatVolume tv = m_threatVolumeList.at(curentIndex);
  int threatW = tv.p1().z() - tv.p0().z();
  int threatH = tv.p1().y() - tv.p0().y();
  int threatL = tv.p1().x() - tv.p0().x();
  QString sVol = "Volume Dimensions (WxHxL):";
  sVol += QString::number(threatW) + ", " +
      QString::number(threatH) + ", " +
      QString::number(threatL);
  m_console->logString(sVol);

  int imageW = ui->testThreatImage->pixmap()->width();
  int imageH = ui->testThreatImage->pixmap()->height();

  sVol = "Image Dimenstions: ";
  sVol += QString::number(imageW) + ", " +
      QString::number(imageH);
  m_console->logString(sVol);

  if ( (imageW > threatW) ||
       (imageH > threatH))
  {
    QMessageBox::information(this,
                             "Error in usage",
                             "Threat too large for TDR Volume");
    return;
  }
  int threatSelect = ui->comboBoxThreatSelect->currentIndex();

  int p0Y = (threatH - imageH)/2;
  int p1Y = p0Y      + imageH;

  int p0X = 1;
  int p1X = p0X + g_threatThickness.at(threatSelect);

  int p0Z = (threatW - imageW)/2;
  int p1Z = p0Z + imageW;

  SDICOS::Bitmap& bmp = m_threatBMPList.at(curentIndex);
  SDICOS::S_UINT32 sx, sy, sz;
  bmp.GetDims(sx, sy, sz);
  if ( (sx < threatL) ||
       (sy < threatH) ||
       (sz < threatW)
       )
  {
    QMessageBox::information(this,
                             "Error in usage",
                             "TDR BMP too small!");
    return;
  }

  QImage pImage = ui->testThreatImage->pixmap()->toImage();

  int ix;
  int iy;

  // Loop over entire volume setting bits as needeed.
  for (int z = 0; z < threatW; z++)
  {
    for (int y = 0; y < threatH; y++)
    {
      for (int x = 0; x < threatL; x++)
      {
         if ( (z >= p0Z) &&
              (z <  p1Z) &&

              (x >= p0X) &&
              (x <   p1X) &&

              (y >= p0Y) &&
              (y < p1Y)
             )
         {
           ix = z - p0Z;
           iy = y - p0Y;
           QColor pc = pImage.pixelColor(QPoint(ix, iy));
           if ((pc.red()   == 255) &&
               (pc.green() == 255) &&
               (pc.blue()  == 255)
              )
           {
             bmp.SetBit(x, y, z, false);
           }
           else
           {
             bmp.SetBit(x, y, z, true);
           }
         }
         else
         {
           bmp.SetBit(x, y, z, false);
         }
      }
    }
  }
}

//------------------------------------------------------------------------------
void MainWindow::addThreatExtrudeAlognY(int curentIndex)
{
  ThreatVolume tv = m_threatVolumeList.at(curentIndex);
  int threatW = tv.p1().z() - tv.p0().z();
  int threatH = tv.p1().x() - tv.p0().x();
  int threatL = tv.p1().y() - tv.p0().y();
  QString sVol = "Volume Dimensions (WxHxL):";
  sVol += QString::number(threatW) + ", " +
      QString::number(threatH) + ", " +
      QString::number(threatL);
  m_console->logString(sVol);

  int imageW = ui->testThreatImage->pixmap()->width();
  int imageH = ui->testThreatImage->pixmap()->height();

  sVol = "Image Dimenstions: ";
  sVol += QString::number(imageW) + ", " +
      QString::number(imageH);
  m_console->logString(sVol);

  if ( (imageW > threatW) ||
       (imageH > threatH))
  {
    QMessageBox::information(this,
                             "Error in usage",
                             "Threat too large for TDR Volume");
    return;
  }
  int threatSelect = ui->comboBoxThreatSelect->currentIndex();

  int p0X = (threatH - imageH)/2;
  int p1X = p0X      + imageH;

  int p0Y = 1;
  int p1Y = p0Y + g_threatThickness.at(threatSelect);

  int p0Z = (threatW - imageW)/2;
  int p1Z = p0Z + imageW;

  SDICOS::Bitmap& bmp = m_threatBMPList.at(curentIndex);
  SDICOS::S_UINT32 sx, sy, sz;
  bmp.GetDims(sx, sy, sz);
  if ( (sx < threatH) ||
       (sy < threatL) ||
       (sz < threatW)
       )
  {
    QMessageBox::information(this,
                             "Error in usage",
                             "TDR BMP too small!");
    return;
  }

  QImage pImage = ui->testThreatImage->pixmap()->toImage();

  int ix;
  int iy;

  // Loop over entire volume setting bits as needeed.
  for (int z = 0; z < threatW; z++)
  {
    for (int x = 0; x < threatH; x++)
    {
      for (int y = 0; y < threatL; y++)
      {
         if ( (z >= p0Z) &&
              (z <  p1Z) &&

              (x >= p0X) &&
              (x <   p1X) &&

              (y >= p0Y) &&
              (y <  p1Y)
             )
         {
           ix = z - p0Z;
           iy = x - p0X;
           QColor pc = pImage.pixelColor(QPoint(ix, iy));
           if ((pc.red()   == 255) &&
               (pc.green() == 255) &&
               (pc.blue()  == 255)
              )
           {
             bmp.SetBit(x, y, z, false);
           }
           else
           {
             bmp.SetBit(x, y, z, true);
           }
         }
         else
         {
           bmp.SetBit(x, y, z, false);
         }
      }
    }
  }
}


//------------------------------------------------------------------------------
void MainWindow::addThreatExtrudeAlognZ(int curentIndex)
{
  ThreatVolume tv = m_threatVolumeList.at(curentIndex);
  int threatW = tv.p1().x() - tv.p0().x();
  int threatH = tv.p1().y() - tv.p0().y();
  int threatL = tv.p1().z() - tv.p0().z();
  QString sVol = "Volume Dimensions (WxHxL):";
  sVol += QString::number(threatW) + ", " +
      QString::number(threatH) + ", " +
      QString::number(threatL);
  m_console->logString(sVol);

  int imageW = ui->testThreatImage->pixmap()->width();
  int imageH = ui->testThreatImage->pixmap()->height();

  sVol = "Image Dimenstions: ";
  sVol += QString::number(imageW) + ", " +
      QString::number(imageH);
  m_console->logString(sVol);

  if ( (imageW > threatW) ||
       (imageH > threatH))
  {
    QMessageBox::information(this,
                             "Error in usage",
                             "Threat too large for TDR Volume");
    return;
  }
  int threatSelect = ui->comboBoxThreatSelect->currentIndex();

  int p0X = (threatW - imageW)/2;
  int p0Y = (threatH - imageH)/2;
  int p0Z = threatL/2;
  int p1X = p0X + imageW;
  int p1Y = p0Y + imageH;
  int p1Z = p0Z + g_threatThickness.at(threatSelect);

  SDICOS::Bitmap& bmp = m_threatBMPList.at(curentIndex);
  SDICOS::S_UINT32 wx, wy, wz;
  bmp.GetDims(wx, wy, wz);
  if ( (wx < threatW) ||
       (wy < threatH) ||
       (wz < threatL)
       )
  {
    QMessageBox::information(this,
                             "Error in usage",
                             "TDR BMP too small!");
    return;
  }

  QImage pImage = ui->testThreatImage->pixmap()->toImage();

  int ix;
  int iy;

  // Loop over entire volume setting bits as needeed.
  for (int z = 0; z < threatL; z++)
  {
    for (int y = 0; y < threatH; y++)
    {
      for (int x = 0; x < threatW; x++)
      {
         if ( (x >= p0X) &&
              (x < p1X) &&

              (y >= p0Y) &&
              (y < p1Y) &&

              (z >= p0Z) &&
              (z < p1Z)
             )
         {
           ix = x - p0X;
           iy = y - p0Y;
           QColor pc = pImage.pixelColor(QPoint(ix, iy));
           if ((pc.red()   == 255) &&
               (pc.green() == 255) &&
               (pc.blue()  == 255)
              )
           {
             bmp.SetBit(x, y, z, false);
           }
           else
           {
             bmp.SetBit(x, y, z, true);
           }
         }
         else
         {
           bmp.SetBit(x, y, z, false);
         }
      }
    }
  }
}



//------------------------------------------------------------------------------
void MainWindow::resizeTDR(int index, QVector3D& vecResize)
{
  SDICOS::TDRTypes::ThreatSequence &ts = m_tdrModule->GetThreatSequence();

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects =
            ts.GetPotentialThreatObject();


  float xb, yb, zb;

  float xt = vecResize.x();
  float yt = vecResize.y();
  float zt = vecResize.z();

  threatObjects[index].GetBaggageTDR().GetThreatROI().GetThreatRoiBase(xb, yb, zb);


  threatObjects[index].GetBaggageTDR().GetThreatROI().SetThreatRoiExtents(xt,
                                                                          yt,
                                                                          zt);

  QVector3D p0 = QVector3D(xb, yb, zb);
  QVector3D p1 = QVector3D(xb + xt,
                           yb + yt,
                           zb + zt);
  ThreatVolume tv(p0, p1);
  m_threatVolumeList[index] = tv;


  SDICOS::Bitmap& bmp = threatObjects[index].GetBaggageTDR().GetThreatROI().GetThreatRoiBitmap();
  bmp.FreeMemory();
  bmp.SetDims(xt, yt, zt);


  QString sResult = "New [W,H,L]: ";
  sResult += QString::number(xt) + ",";
  sResult += QString::number(yt) + ",";
  sResult += QString::number(zt);
  m_console->logString(sResult);
}


//------------------------------------------------------------------------------
void MainWindow::saveConsoleToFile(QString& saveFile)
{
  std::ofstream fileOut(saveFile.toLocal8Bit().data());
  QString sData = ui->plainTextEdit->toPlainText();
  std::string outStr = sData.toLocal8Bit().data();
  fileOut << outStr;
}

//------------------------------------------------------------------------------
bool MainWindow::saveTDRToFile(QString& saveFile)
{
  SDICOS::ErrorLog errorLog;
  QFileInfo fInfo(saveFile);
  std::string path = fInfo.absolutePath().toLocal8Bit().data();
  path += "/";
  std::string name = fInfo.fileName().toLocal8Bit().data();

  SDICOS::Folder tdrFolder(path);
  SDICOS::Filename tdrFilename(tdrFolder, name);
  SDICOS::DicosFile::TRANSFER_SYNTAX nSyntax = SDICOS::DicosFile::enumLittleEndianExplicit;
  bool writeStat = m_tdrModule->Write(tdrFilename, nSyntax, errorLog);
  if (writeStat)
  {
    QString message = "Successful DTRModule write to file:";
    message +=  saveFile;
    m_console->logString(message);
  }
  else
  {
    std::string  stdLogName = g_errorLogfile.toLocal8Bit().data();
    // qDebug() << "Fail DTRModule write to file:" << saveFile;

    QString message = "Fail DTRModule write to file:";
    message += saveFile;
    m_console->logString(message);
    errorLog.WriteLog(SDICOS::Filename(stdLogName));
  }
  return writeStat;
}

//------------------------------------------------------------------------------
void MainWindow::onThreatComboSlectionChanged(int selection)
{
  int threatSel = ui->comboBoxThreatSelect->currentIndex();
  QString sImage = g_threatImageList.at(threatSel);
  ui->testThreatImage->setPixmap(QPixmap(sImage));
  qDebug() << "Selection Changesd to" << threatSel;
}

//------------------------------------------------------------------------------
void MainWindow::onSetThreatType()
{
  if (m_tdrListCombo->count() == 0)
  {
    m_console->logString("No threats loaded!");
    return;
  }
  int curIndex = m_tdrListCombo->currentIndex();
  analogic::workstation::ThreatAlarmType aType = m_threatTypeList.at(curIndex);
  QString currentTypeName = "Threat Type:";
  currentTypeName += analogic::workstation::threatTypeName(aType);
  m_console->logString(currentTypeName);

  QStringList listNames = analogic::workstation::allThreatTypeNames();
  QInputDialog selectThreatTypeDlg(this);

  selectThreatTypeDlg.setOption(QInputDialog::UseListViewForComboBoxItems);
  selectThreatTypeDlg.setWindowTitle("Select Threat Type");
  selectThreatTypeDlg.setLabelText(tr("Type Selection:"));
  selectThreatTypeDlg.setComboBoxItems(listNames);
  int ret = selectThreatTypeDlg.exec();
  if  (ret == QDialog::Accepted)
  {
     QString selText = selectThreatTypeDlg.textValue();
     QString sRet = "Retval=" + selText;
     m_console->logString(sRet);

     analogic::workstation::ThreatAlarmType thType =
         analogic::workstation::threatAlarmTypeFromName(selText);
     this->setTDRThreatType(curIndex, thType);
  }
}


//------------------------------------------------------------------------------
void MainWindow::onVolumeSetMass()
{
  if (m_tdrListCombo->count() == 0)
  {
    m_console->logString("No threats loaded!");
    return;
  }
  int curIndex = m_tdrListCombo->currentIndex();
  this->setTDRMass(curIndex);
}

//------------------------------------------------------------------------------
void MainWindow::onClearAllThreats()
{
  if (m_tdrListCombo->count() == 0)
  {
    m_console->logString("No threats loaded!");
    return;
  }
  this->clearaAllThreats();
}

