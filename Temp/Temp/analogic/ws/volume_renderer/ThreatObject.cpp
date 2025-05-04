//------------------------------------------------------------------------------
// File: ThreatObject.cpp
// Description: Implementation of Threat Object class
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "ThreatObject.h"


//------------------------------------------------------------------------------
ThreatObject::ThreatObject(int index,
                           ThreatVolume& vol,
                           QString id,
                           QString text,
                           SDICOS::Bitmap* bmpRef):
 m_state(inactive),
 m_threatActorRef(nullptr),
 m_volume(vol),
 m_text(text),
 m_ID(id),
 m_index(index),
 m_pBmp(bmpRef),
 m_threatType(analogic::workstation::ANOMALY),
 m_threatGenType(analogic::workstation::ATR_GEN),
 m_offset(5000),
 m_mass(0),
 m_isLaptop(false)
{
}


//------------------------------------------------------------------------------
ThreatObject::~ThreatObject()
{
  m_text.clear();
  m_ID.clear();
}


//------------------------------------------------------------------------------
void ThreatObject::clearThreat()
{
  // std::cout << "**** ThreatObject::clearThreat() ****"
  //          << std::endl;
   m_state = ThreatObject::cleared;
   if (m_threatActorRef)
   {
      m_threatActorRef->GetProperty()->SetOpacity(CLEAR_THREATBOX_OPACITY);
   }
   m_index = -1;
}

//------------------------------------------------------------------------------
void ThreatObject::setAsLaptop(bool val)
{
  m_isLaptop = val;
}


//------------------------------------------------------------------------------
bool ThreatObject::isLapTop()
{
  return m_isLaptop;
}

//------------------------------------------------------------------------------
ThreatObject::ThreatState ThreatObject::getState()
{
  return m_state;
}



//------------------------------------------------------------------------------
void ThreatObject::setThreatState(ThreatObject::ThreatState aState,
                                  bool laptopViewMode,
                                  bool machineThretsEnabled,
                                  bool bagIsSuspected,
                                  bool isATipBag,
                                  bool slabbingLaptop,
                                  bool surfaceView)
{
  // Do I have an acto or not??
  if (m_threatActorRef == nullptr)
  {
    m_state = cleared;
    return;
  }
  if (m_state == cleared) return;
  m_state = aState;
  if (laptopViewMode)
  {
    // LAPTOP VIEW MODE
    if (this->isLapTop())
    {
      if (aState == ThreatObject::inactive)
      {
        if (m_threatActorRef)
        {
          m_threatActorRef->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY);
          // std::cout << "threatActor->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY); == 01" << std::endl;
        }
      }
      if (aState == ThreatObject::active)
      {
        if (m_threatActorRef)
        {
          m_threatActorRef->GetProperty()->SetOpacity(ACTIVE_THREATBOX_OPACITY);
        }
      }
    }
    else
    {
      if ( (aState != ThreatObject::cleared) &&
           (m_threatActorRef) )
      {
        m_threatActorRef->GetProperty()->SetOpacity(HIDE_THREATBOX_OPACITY);
      }
    }
  }
  else
  {
    analogic::workstation::ThreatGenerationType genType = this->getGenType();
    if( machineThretsEnabled || (genType == analogic::workstation::OPERATOR_GEN) )
    {
      // MACHINE THREAT VIEW MODE
      if (!this->isLapTop())
      {
        if (aState == ThreatObject::inactive)
        {
          if (m_threatActorRef)
          {
            if (isATipBag)
            {
              if (bagIsSuspected ||
                 (this->getGenType() == analogic::workstation::OPERATOR_GEN) )
              {
                m_threatActorRef->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY);
                // std::cout << "SetOpacity(INACTIVE_THREATBOX_OPACITY); == 02" << std::endl;
              }
            }
            else
            {
              if (!slabbingLaptop)
              {
                m_threatActorRef->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY);
                // std::cout << "SetOpacity(INACTIVE_THREATBOX_OPACITY); == 03" << std::endl;
              }
            }
          }
        }
        if (aState == ThreatObject::active)
        {
          if (m_threatActorRef)
          {
            if (isATipBag)
            {
               if ( bagIsSuspected ||
                    (this->getGenType() == analogic::workstation::OPERATOR_GEN ))
               {
                 m_threatActorRef->GetProperty()->SetOpacity(ACTIVE_THREATBOX_OPACITY);
               }
            }
            else
            {
              m_threatActorRef->GetProperty()->SetOpacity(ACTIVE_THREATBOX_OPACITY);
            }
          }
        }
      }
      else
      {
        if ( (aState != ThreatObject::cleared) &&
             (m_threatActorRef)  &&
             (!slabbingLaptop)
           )
        {
          m_threatActorRef->GetProperty()->SetOpacity(HIDE_THREATBOX_OPACITY);
        }
        else
        {
          if (aState == ThreatObject::inactive)
          {
            if (m_threatActorRef)
            {
              if (!surfaceView)
              {
                m_threatActorRef->GetProperty()->SetOpacity(INACTIVE_THREATBOX_OPACITY);
                // std::cout << "SetOpacity(INACTIVE_THREATBOX_OPACITY); == 04" << std::endl;
              }
            }
          }
          if (aState == ThreatObject::active)
          {
            if (m_threatActorRef)
            {
              m_threatActorRef->GetProperty()->SetOpacity(ACTIVE_THREATBOX_OPACITY);
            }
          }
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
void ThreatObject::suspectThreat()
{
  m_state = ThreatState::suspected;
}

//------------------------------------------------------------------------------
void ThreatObject::nullActor()
{
  m_threatActorRef = nullptr;
}


//------------------------------------------------------------------------------
void ThreatObject::setType(analogic::workstation::ThreatAlarmType aType)
{
  m_threatType = aType;
}

//------------------------------------------------------------------------------
void ThreatObject::setGenType(analogic::workstation::ThreatGenerationType aGenType)
{
 m_threatGenType =  aGenType;
}

//------------------------------------------------------------------------------
void ThreatObject::setMass(double mass)
{
  m_mass = mass;
}

//------------------------------------------------------------------------------
double ThreatObject::getMass()
{
  return m_mass;
}

//------------------------------------------------------------------------------
void ThreatObject::setOffset(uint16_t offset)
{
  m_offset = offset;
}

//------------------------------------------------------------------------------
uint16_t ThreatObject::getOffset()
{
  return m_offset;
}

//------------------------------------------------------------------------------
analogic::workstation::ThreatGenerationType
  ThreatObject::getGenType()
{
  return m_threatGenType;
}

//------------------------------------------------------------------------------
analogic::workstation::ThreatAlarmType
  ThreatObject::getType()
{
 return  m_threatType;
}

//------------------------------------------------------------------------------
void ThreatObject::setActor(vtkSmartPointer<vtkActor> anActor)
{
  m_threatActorRef = anActor.GetPointer();
}

//------------------------------------------------------------------------------
bool ThreatObject::compare(ThreatObject& other)
{
  ThreatVolume tvCur     = this->getVolume();
  ThreatVolume tvOther   = other.getVolume();
  QString      idCur     = this->getID();
  QString      idOther   = other.getID();


  if ( ( tvCur.p0() == tvOther.p0()  ) &&
       ( tvCur.p1() == tvOther.p1()  ) &&
       ( idCur.compare(idOther) == 0 )
      )
  {
    return true;
  }
  return false;
}


//------------------------------------------------------------------------------
bool ThreatObject::isActorNull()
{
  if (m_threatActorRef == nullptr)  return true;
  return false;
}

//------------------------------------------------------------------------------
vtkActor* ThreatObject::getActor()
{
  return m_threatActorRef;
}

//------------------------------------------------------------------------------
ThreatVolume ThreatObject::getVolume()
{
  return m_volume;
}

//-----------------------------------------------------------------------------
int ThreatObject::getVolumeSize()
{
  return m_volume.getSize();
}

//------------------------------------------------------------------------------
QString ThreatObject::getText()
{
  return m_text;
}

//------------------------------------------------------------------------------
QString ThreatObject::getID()
{
  return m_ID;
}

//------------------------------------------------------------------------------
int ThreatObject::getIndex()
{
  return m_index;
}

//------------------------------------------------------------------------------
void ThreatObject::setIndex(int newIndex)
{
  m_index = newIndex;
}

//------------------------------------------------------------------------------
SDICOS::Bitmap* ThreatObject::getBMP()
{
  return m_pBmp;
}

//------------------------------------------------------------------------------
void ThreatObject::setBMP(SDICOS::Bitmap* aBMP)
{
  m_pBmp = aBMP;
}
