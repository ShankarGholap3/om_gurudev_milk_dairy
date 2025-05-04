//------------------------------------------------------------------------------
// Name: ThreatList.cpp
// Description: implementation of ThreatList class.
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "ThreatList.h"

//------------------------------------------------------------------------------
ThreatList::ThreatList(double pixelSpacingX,
                       double pixelSpacingY,
                       double pixelSpacingZ,
                       QVector3D imageOrigin,
                       bool visibleThreatwidget):
m_imageOffset(imageOrigin),
m_visibleThreatWidget(visibleThreatwidget),
m_laptopViewMode(false),
m_surfaceViewMode(false),
m_slabbingLaptop(false),
m_isATipBag(false),
m_bagIsSuspected(false),
m_machineThreatsEnabled(false),
m_pixelSpacingX(pixelSpacingX),
m_pixelSpacingY(pixelSpacingY),
m_pixelSpacingZ(pixelSpacingZ),
m_currentIndex(-1),
m_stashIndex(-1),
m_sizeList(0),
m_volumeRenderer(nullptr)
{
  m_objectVec.clear();
}



//------------------------------------------------------------------------------
ThreatList::~ThreatList()
{
}

//------------------------------------------------------------------------------
void ThreatList::getThreatBoundaries(int threatIndex,
                                     int& outW,
                                     int& outH,
                                     int& outL,
                                     QVector3D& outP0)
{
  ThreatObject* curThreat = this->getThreatAtIndex(threatIndex);
  if (!curThreat) return;

  ThreatVolume threatVol = curThreat->getVolume();
  QVector3D threatLocationP0 = threatVol.p0();
  QVector3D threatLocationP1 = threatVol.p1();

  int W  = ( threatLocationP1.x() - threatLocationP0.x() );
  int H  = ( threatLocationP1.y() - threatLocationP0.y() );
  int L  = ( threatLocationP1.z() - threatLocationP0.z() );

  outW  = W;
  outH  = H;
  outL  = L;
  outP0 = threatVol.p0();
  m_threatBuffer.resize(W*H*L);
  memset(&m_threatBuffer[0], 0, W*H*L*sizeof(uint16_t));
  m_stashIndex = threatIndex;
}



//------------------------------------------------------------------------------
void ThreatList::setVolumeRenderer(VolumeRendererBase* renderer)
{
  m_volumeRenderer = renderer;
}

//------------------------------------------------------------------------------
QStringList ThreatList::getThreatIDs()
{
  QStringList retList;
  for (unsigned int i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* pObj = m_objectVec[i];
    QString  ID   = pObj->getID();
    retList.push_back(ID);
  }
  return retList;
}

//------------------------------------------------------------------------------
std::list<ThreatObject*> ThreatList::getThreatObjectList()
{
  std::list<ThreatObject*> retList;
  std::map<int, ThreatObject*>::iterator iter;
  for (unsigned int i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* pObj = m_objectVec[i];
    retList.push_back(pObj);
  }
  return retList;
}

//------------------------------------------------------------------------------
int ThreatList::getNewOperatorThreatID()
{
  // 1. Get Count of MACHINE threats
  // 2. Add count of UNCLEARED OPERATOR threats
  // 3. Use the the sum of the above.
  int machine_threats             = 0;
  int uncleared_operator_threats  = 0;

  for (size_t i = 0;  i < m_objectVec.size();  i++)
  {
    ThreatObject* pObj = m_objectVec[i];
    if (pObj)
    {
      if (pObj->getGenType() == analogic::workstation::ATR_GEN)
      {
        machine_threats++;
      }

      if ( (pObj->getGenType() == analogic::workstation::OPERATOR_GEN) &&
           (pObj->getState() != ThreatObject::ThreatState::cleared   )
           )
      {
        uncleared_operator_threats++;
      }
    }
  }
  int retID = machine_threats + uncleared_operator_threats;

  // std::cout << "Generated OPERATOR threat ID= " << retID << std::endl;

  return retID;
}

//------------------------------------------------------------------------------
void ThreatList::clearThreatDirect(int threatIndex)
{
  ThreatObject* pThr = this->getThreatAtIndex(threatIndex);
  if (pThr)
  {
    if (pThr->isLapTop()) return;
    pThr->clearThreat();
  }
}

//------------------------------------------------------------------------------
void ThreatList::clearThreatMoveOperators(int threatIndex)
{
  // std::cout << "%%%%%% clearThreatAt(int index):  index= "
  //           << threatIndex << std::endl;

  int sizeVec = m_objectVec.size();

  ThreatObject* pThr = this->getThreatAtIndex(threatIndex);

  //----------------------------------------------------
  // Note non threat LAPTOPS aren't like other
  // threats they can't be cleared by the Operator so
  // we handle their visibility by other means.
  // So for this code we will return without doing
  // any thing for non treat LAPTOPS.
  if (pThr)
  {
    if (pThr->isLapTop()) return;
  }
  //----------------------------------------------------

  bool doMoveOperatorThreats = false;
  if (pThr)
  {
    pThr->clearThreat();
    analogic::workstation::ThreatGenerationType gentYpe = pThr->getGenType();
    if (gentYpe == analogic::workstation::OPERATOR_GEN )
    {
      doMoveOperatorThreats = true;
    }
  }
  if (doMoveOperatorThreats)
  {
    for (int i = 0; i < sizeVec; i++)
    {
      ThreatObject* pThTest = m_objectVec.at(i);
      if (pThTest)
      {
        if (pThTest->getGenType() == analogic::workstation::OPERATOR_GEN )
        {
          ThreatObject::ThreatState  curState = pThTest->getState();
          if (curState != ThreatObject::ThreatState::cleared )
          {
            int anIndex = pThTest->getIndex();
            if (anIndex > threatIndex)
            {
              pThTest->setIndex(anIndex - 1);
            }
            // std::cout << "Changeing OPERATOR Threat index from ["
            //           << anIndex << "] to ["
            //           << (anIndex - 1)
            //           << std::endl;
          }
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
void ThreatList::suspectThreatAt(int threatIndex)
{
  ThreatObject* pThr = this->getThreatAtIndex(threatIndex);
  if (pThr)
  {
    pThr->suspectThreat();
  }
}

//------------------------------------------------------------------------------
void ThreatList::clearAllThreats()
{
  if (m_objectVec.size() == 0 ) return;
  QString clearStr = "*CCYYCC* About to clear all threats - Number: " + QString::number(m_objectVec.size());
  LOG(INFO) << clearStr.toLocal8Bit().data();

  for (unsigned int i  = 0;  i < m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if (pThr)
    {
      vtkActor* thVis = pThr->getActor();
      if (thVis)
      {
        m_volumeRenderer->removeThreatVisual(thVis);
      }
    }
  }
  m_objectVec.clear();
  m_objectVec.resize(0);
}

//------------------------------------------------------------------------------
void ThreatList::addThreatObject(ThreatObject* newThreat)
{
  ThreatVolume v1 = newThreat->getVolume();
  m_objectVec.push_back(newThreat);

  QString message = "**HH** ThtreatList insert threat volume P0-P1[(";
  message += QString::number(v1.p0().x()) +  ",";
  message += QString::number(v1.p0().y()) +  ",";
  message += QString::number(v1.p0().z()) +  ")-(";

  message += QString::number(v1.p1().x()) +  ",";
  message += QString::number(v1.p1().y()) +  ",";
  message += QString::number(v1.p1().z()) +  ")]";

  qDebug() << message;
  LOG(INFO) << message.toLocal8Bit().data();
}

//------------------------------------------------------------------------------
void ThreatList::setActorAtIndex(unsigned int setIndex,
                                 vtkSmartPointer<vtkActor> anActor)
{
  if (setIndex > m_objectVec.size() -1) return;

  ThreatObject* setObj = m_objectVec[setIndex];
  if (setObj)
  {
    setObj->setActor(anActor);
  }
  else
  {
    QString message = "**TEET** Error getting ThreatObject* from list with index:";
    message += QString::number(setIndex);
  }
}

//------------------------------------------------------------------------------
int ThreatList::getCurrent()
{
  return m_currentIndex;
}

//------------------------------------------------------------------------------
void ThreatList::setCurrent(int index)
{
  m_currentIndex = index;

  for (unsigned int i = 0; i< m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    // std::cout << "Loop Threat [i]=" << i << " desired Index = " << index << std::endl;
    if (pThr)
    {
      if (pThr->getIndex() == index)
      {
        pThr->setThreatState(ThreatObject::active,
                             m_laptopViewMode,
                             m_machineThreatsEnabled,
                             m_bagIsSuspected,
                             m_isATipBag,
                             m_slabbingLaptop,
                             m_surfaceViewMode);
      }
      else
      {
        pThr->setThreatState(ThreatObject::inactive,
                             m_laptopViewMode,
                             m_machineThreatsEnabled,
                             m_bagIsSuspected,
                             m_isATipBag,
                             m_slabbingLaptop,
                             m_surfaceViewMode);
      }
    }
  }
}


//------------------------------------------------------------------------------
void ThreatList::hideLaptops()
{
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if (pThr)
    {
      if (pThr->isLapTop())
      {
        pThr->setThreatState(ThreatObject::inactive,
                             false,
                             m_machineThreatsEnabled,
                             m_bagIsSuspected,
                             m_isATipBag,
                             m_slabbingLaptop,
                             m_surfaceViewMode);

        vtkActor* threatActor = pThr->getActor();
        if (threatActor)
        {
          threatActor->GetProperty()->SetOpacity(HIDE_THREATBOX_OPACITY);
        }
      }
    }
  }
}



//------------------------------------------------------------------------------
void ThreatList::forceHideLaptops()
{
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if (pThr)
    {
      if (pThr->isLapTop())
      {
        vtkActor* threatActor = pThr->getActor();
        if (threatActor)
        {
          threatActor->GetProperty()->SetOpacity(HIDE_THREATBOX_OPACITY);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void ThreatList::hideMachineThreats()
{
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if (pThr)
    {
      if (!pThr->isLapTop())
      {
        pThr->setThreatState(ThreatObject::inactive,
                             false,
                             m_machineThreatsEnabled,
                             m_bagIsSuspected,
                             m_isATipBag,
                             m_slabbingLaptop,
                             m_surfaceViewMode);

        vtkActor* threatActor = pThr->getActor();
        if (threatActor)
        {
          if (!m_isATipBag)
          {
            threatActor->GetProperty()->SetOpacity(HIDE_THREATBOX_OPACITY);
          }
          else
          {
            threatActor->GetProperty()->SetOpacity(CLEAR_THREATBOX_OPACITY);
          }
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void ThreatList::unHideMachineThreats()
{
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if (pThr)
    {
      if (!pThr->isLapTop())
      {
        vtkActor* threatActor = pThr->getActor();
        if (threatActor)
        {
          if (!m_isATipBag)
          {
            if (pThr->getState() == ThreatObject::inactive)
            {
              threatActor->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY);
              // std::cout << "threatActor->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY); == 00" << std::endl;
            }
          }
          else
          {
            threatActor->GetProperty()->SetOpacity(CLEAR_THREATBOX_OPACITY);
          }
        }
      }
    }
  }
}



//------------------------------------------------------------------------------
void ThreatList::hideThreatAt(int index)
{
  this->clearThreatDirect(index);
}

//------------------------------------------------------------------------------
QString ThreatList::currentText()
{
  QString retS = "";
  std::map<int, ThreatObject*>::iterator iter;
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* tOb = m_objectVec[i];
    if (tOb->getIndex() == m_currentIndex)
    {
      retS = tOb->getText();
    }
  }
  return retS;
}

//------------------------------------------------------------------------------
int ThreatList::currentVolume()
{
  int retVol = 0;

  for (unsigned int i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* tOb = m_objectVec[i];
    if (tOb->getIndex() == m_currentIndex)
    {
      retVol = tOb->getVolumeSize();
    }
  }
  return retVol;
}

//------------------------------------------------------------------------------
QString ThreatList::getTextAt(int index)
{
  QString sRet ="";

  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* tOb = m_objectVec[i];
    if (tOb->getIndex() == index)
    {
      sRet = tOb->getText();
    }
  }
  return sRet;
}


//------------------------------------------------------------------------------
QString ThreatList::getIdAt(int index)
{
  QString sRet ="";

  for (unsigned int i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* tOb = m_objectVec[i];
    if (tOb->getIndex() == index)
    {
      sRet = tOb->getID();
    }
  }
  return sRet;
}

//------------------------------------------------------------------------------
bool ThreatList::isLaptopAtIndex(int index)
{
  bool bRet = false;

  ThreatObject* tOb = this->getThreatAtIndex(index);
  if (tOb)
  {
    if (tOb->getIndex() == index)
    {
      bRet = tOb->isLapTop();
    }
  }
  return bRet;
}

//------------------------------------------------------------------------------
void ThreatList::setAsTip()
{
  m_isATipBag = true;
}

//------------------------------------------------------------------------------
void ThreatList::setAsTipFalse()
{
  m_isATipBag = false;
}

//------------------------------------------------------------------------------
void ThreatList::suspectBag()
{
  m_bagIsSuspected = true;
}

//------------------------------------------------------------------------------
SDICOS::Bitmap* ThreatList::bmpAtIndex(unsigned int index)
{
  SDICOS::Bitmap* pRet = nullptr;

  if (index > m_objectVec.size() -1 ) return pRet;

  ThreatObject* tOb = this->getThreatAtIndex(index);
  if (tOb)
  {
    if ((unsigned int)tOb->getIndex() == index)
    {
     pRet = tOb->getBMP();
    }
  }
  return pRet;
}

//-------------------------------------------------------------------------------
ThreatObject* ThreatList::getThreatAtIndex(int itemIndex)
{
  if (itemIndex == -1) return nullptr;
  ThreatObject* retObj = nullptr;

  for (size_t i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* testObject = m_objectVec[i];
    if (testObject->getIndex() == itemIndex)
    {
     retObj = testObject;
     break;
    }
  }
  return retObj;
}

//-----------------------------------------------------------------------------
ThreatObject* ThreatList::findNextAvailbleThreatIndex()
{
  ThreatObject* retObj = nullptr;
  for (size_t i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* testObject = m_objectVec[i];
    if ( ( !testObject->isLapTop() ) &&
         ( testObject->getState() != ThreatObject::cleared)
       )
    {
     retObj = testObject;
     break;
    }
  }
  return retObj;
}

//-------------------------------------------------------------------------------
ThreatObject* ThreatList::findFirstNonThreatLaptop()
{
  ThreatObject* retObj = nullptr;

  for (size_t i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* testObject = m_objectVec[i];
    if (testObject->isLapTop())
    {
     retObj = testObject;
     break;
    }
  }
  return retObj;
}

//------------------------------------------------------------------------------
int ThreatList::nonThreatLaptopCount()
{
  int retVal = 0;
  for (size_t i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* testObject = m_objectVec[i];
    if (testObject->isLapTop())
    {
     retVal++;
    }
  }
  return retVal;
}


//------------------------------------------------------------------------------
std::vector<uint16_t>& ThreatList::getThreatImageData()
{
  return m_threatBuffer;
}

//------------------------------------------------------------------------------
std::vector<uint16_t>&  ThreatList::getThreatStashBuffer()
{
  return m_stashVec;
}

//------------------------------------------------------------------------------
bool ThreatList::isActive()
{
  if ( m_objectVec.size() == 0   ) return false;
  if ( this->getCurrent() < 0 ) return false;
  return true;
}


//------------------------------------------------------------------------------
bool ThreatList::isCurrent(int itemIndex)
{
  if (itemIndex != m_currentIndex) return false;
  return true;
}

//------------------------------------------------------------------------------
void ThreatList::setLaptopViewMode(bool lapMode)
{
  m_laptopViewMode = lapMode;
}

//------------------------------------------------------------------------------
void ThreatList::setSlabingLaptop(bool bSetVal)
{
  m_slabbingLaptop = bSetVal;
}

//------------------------------------------------------------------------------
void ThreatList::setSurfaceViewMode(bool surfaceMode)
{
  m_surfaceViewMode  = surfaceMode;
}

//------------------------------------------------------------------------------
void ThreatList::setMachineThreatsEndbled(bool setVal)
{
  m_machineThreatsEnabled = setVal;
}

//------------------------------------------------------------------------------
void ThreatList::clear()
{
  for (unsigned int i = 0; i < m_objectVec.size(); i++)
  {
    ThreatObject* pObj = m_objectVec[i];
    if (pObj)
    {
      pObj->clearThreat();
      delete pObj;
    }
  }
  m_objectVec.clear();
  m_currentIndex = -1;
  m_bagIsSuspected = false;
}


//------------------------------------------------------------------------------
void ThreatList::clearData()
{
  m_stashVec.clear();
  m_threatBuffer.clear();
  this->clear();
  m_currentIndex = -1;
}

//------------------------------------------------------------------------------
int ThreatList::size()
{
  return static_cast<int>(m_objectVec.size());
}


//------------------------------------------------------------------------------
int  ThreatList::unClearedThreats()
{
  int unclearCount = 0;
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if (pThr)
    {
      if (
          ( pThr->getState() != ThreatObject::cleared ) ||
          ( !pThr->isActorNull() )
          )
      {
        unclearCount++;
      }
    }
  }
  return unclearCount;
}

//------------------------------------------------------------------------------
int ThreatList::unClearedMachineThreats()
{
  int unclearCount = 0;
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if ((pThr) && (pThr->getGenType() == analogic::workstation::ATR_GEN))
    {
      if (
          ( pThr->getState() != ThreatObject::cleared ) ||
          ( pThr->getIndex() != -1)
          )
      {
         if(!pThr->isLapTop())
         {
           unclearCount++;
         }
      }
    }
  }
  return unclearCount;
}


//------------------------------------------------------------------------------
int ThreatList::clearedMachineThreats()
{
  int clearCount = 0;
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if (pThr)
    {
      if ( pThr->getIndex() == -1)
      {
        clearCount++;
      }
    }
  }
  return clearCount;
}



//------------------------------------------------------------------------------
int ThreatList::machineThreats()
{
  int retCount = 0;
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if ((pThr) && (pThr->getGenType() == analogic::workstation::ATR_GEN))
    {
       retCount++;
    }
  }
  return retCount;
}


//------------------------------------------------------------------------------
int ThreatList::laptopThreats()
{
  int retCount = 0;
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if ((pThr) && (pThr->getGenType() == analogic::workstation::ATR_GEN))
    {
      if(pThr->isLapTop())
      {
       retCount++;
      }
    }
  }
  return retCount;
}


//------------------------------------------------------------------------------
int ThreatList::operatorThreats()
{
  int retCount = 0;
  for (unsigned int i = 0; i <  m_objectVec.size(); i++)
  {
    ThreatObject* pThr = m_objectVec[i];
    if ((pThr) && (pThr->getGenType() == analogic::workstation::OPERATOR_GEN))
    {
       retCount++;
    }
  }
  return retCount;
}


//------------------------------------------------------------------------------
void ThreatList::saveVolumeData(QString& dataFile,
                                uint16_t* dataBuffer,
                                int sizeBuffer)
{
  std::ofstream outfile;
  outfile.open(dataFile.toLocal8Bit().data(), ios::out | ios::binary);
  outfile.write(reinterpret_cast<char*>(dataBuffer), sizeBuffer);
  outfile.close();

  int nData = sizeBuffer/sizeof(uint16_t);
  QString nameTail = dataFile.right(EXP_EXT_LENGTH);
  QString statFile = dataFile.left(dataFile.length() - nameTail.length());
  statFile += ".stats";
  int metalCount = 0;

  for (int i = 0; i <nData; i++)
  {
    uint16_t cv = dataBuffer[i];
    if ((cv > 2321) && (cv < 4095))
    {
      metalCount++;
    }
  }

  std::ofstream file_stat;
  file_stat.open (statFile.toLocal8Bit().data());
  QString statString = "Metal Count =" + QString::number(metalCount) + "\n";
  file_stat << statString.toLocal8Bit().data();
  file_stat.close();
}


//------------------------------------------------------------------------------
void ThreatList::saveVolumeHeader(QString& headerFile,
                                  int W,
                                  int H,
                                  int L)
{
  std::ofstream file_hdr;
  file_hdr.open (headerFile.toLocal8Bit().data());
  file_hdr << "Imag data header\n";
  file_hdr << "image_width="  << QString::number(W).toLocal8Bit().data() << "\n";
  file_hdr << "image_height=" << QString::number(H).toLocal8Bit().data() << "\n";
  file_hdr << "image_slices=" << QString::number(L).toLocal8Bit().data() << "\n";
  file_hdr.close();
}


