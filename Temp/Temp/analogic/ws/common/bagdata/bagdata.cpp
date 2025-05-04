/*!
* @file     bagdata.cpp
* @author   Agiliad
* @brief    This file encapsulate data structures and bag data processing details.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#include <analogic/ws/common/bagdata/bagdata.h>

#include <analogic/nss/common/Property.h>
#include <analogic/ws/common/utility/usbstorageservice.h>


namespace analogic
{

namespace ws
{

/*!
* @fn       BagData
* @param    None
* @return   None
* @brief    Constructor for class BagData.
*/
BagData::BagData(int m_Columns, int m_Rows, int n_max_slices)
{
  static int objectCount = 0;
  m_bagObjectCounter = objectCount;  //This is for debugging purpose only, We dont need to decrement when the object is deleted.
  objectCount = objectCount+1;
  m_volColumns = m_Columns;
  m_volRows = m_Rows;
  m_max_slices = n_max_slices;
  m_uncroppedVolColumns = m_Columns;
  m_uncroppedVolRows = m_Rows;
  m_uncropped_max_slices = n_max_slices;
  m_sliceQueue = new QQueue<SliceDataStruct*>();
  setState(BagState::EMPTY);
  DEBUG_LOG("**YYAYY** malloc  bag data here!");
  m_volume = (unsigned char*)malloc(m_volColumns * m_volRows * 2 * m_max_slices);
  if(m_volume == NULL)
    FATAL_LOG("Failed to allocate memory for bag data.");
  m_uncroppedvolume =(unsigned char*)malloc(m_uncroppedVolColumns * m_uncroppedVolRows * 2 * m_uncropped_max_slices);
  if(m_uncroppedvolume == NULL)
    FATAL_LOG("Failed to allocate memory for temp bag data.");
  m_sliceCountReceived = 0;
  m_slicequeueMutex = new QMutex(QMutex::Recursive);
#ifdef WORKSTATION
  connect(this, &BagData::bagFinished,
          NSSAgentAccessInterface::getInstance(),
          &NSSAgentAccessInterface::onBagFinishUpdates);
  connect(this, &BagData::sigSendBagRspToBHS,
          NSSAgentAccessInterface::getInstance(),
          &NSSAgentAccessInterface::onSendBagRspToBHS);
  connect(this, &BagData::sigSendObviousThreat,
          NSSAgentAccessInterface::getInstance(),
          &NSSAgentAccessInterface::onSetObviousThreatFlagtoBHS);
#endif
  m_isBagError = false;
  m_sliceProcessed = 0;
  m_volumectmodule = NULL;
  m_operatorTdr = NULL;
  m_currenttdrModule = NULL;
  m_currrentThreat = DEFAULT_VALUE_CURRENT_THREAT;
  m_visibleCurrentThreatIndex = DEFAULT_VALUE_CURRENT_THREAT;
  m_VRViewmode = DEFAULT_VALUE_VR_VIEW_MODE;
  m_slabViewType = VREnums::THREAT_VIEW;
  m_archiveSaveMode = false;
  m_CurrentThreatType = VREnums::THREATTYPE_UNKNOWN_THREAT;
  m_bhsBagId = "";
  m_isTIPBag = false;
  m_TIPPhase = TIP_NONE_PHASE;
  m_primaryOperatorTdr = NULL;
  m_data_size = 0;
  m_livebag = NULL;
  m_volumectmodule = NULL;
  m_availablealarmcount = 0;
  m_effectiveavailablealarmcount = 0;
  m_visiblealarmcount = 0;
  m_noAlarmcount = 0;
  m_obviousThreat = false;
  m_detectionException = false;
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  if(currentscreen == QMLEnums::RECALLBAG_SCREEN)
  {
    m_showAllThreat = DEFAULT_VALUE_RECALL_SHOW_ALL_THREAT;
  }
  else
  {
    m_showAllThreat = DEFAULT_VALUE_SHOW_ALL_THREAT;
  }
}

/*!
* @fn       setState
* @param    const BagState & - state
* @return   None
* @param    Sets Bag state
*/
void BagData::setState(const BagData::BagState &state)
{
  m_bagStateMutex.lock();
  m_state = state;

  INFO_LOG("Set Bag state to:" << BagStateText[m_state] << "  ObjID: "<<m_bagObjectCounter );

  if(m_state == INITIALISED ||
     m_state == CONSTRUCTION ||
     m_state == DISPOSED ||
     m_state == ERROR ||
     m_state == EMPTY)
  {
    if ((m_state == INITIALISED) ||
        (m_state == DISPOSED) ||
        (m_state == ERROR) ||
        (m_state == EMPTY) )
    {
      m_bhsBagId = "";
      m_primaryOperatorTdr = NULL;
    }

    m_volRows = WorkstationConfig::getInstance()->getBagDataResolution().height();
    m_volColumns = WorkstationConfig::getInstance()->getBagDataResolution().width();
    m_data_size = 0;
    m_ctmodule.Clear();
    m_sliceCountReceived = 0;
    m_sliceProcessed = 0;
    setUserDecision("");
    m_detectionException = false;

    m_operatorTdr = NULL;
    m_VRViewmode = DEFAULT_VALUE_VR_VIEW_MODE;
    m_currenttdrModule = NULL;
    m_currrentThreat = DEFAULT_VALUE_CURRENT_THREAT;
    m_visibleCurrentThreatIndex = DEFAULT_VALUE_CURRENT_THREAT;
    m_CurrentThreatType = VREnums::THREATTYPE_UNKNOWN_THREAT;
    for(unsigned int count = 0; count < m_tdrmodule.size(); count++)
    {
      SDICOS::TDRModule* tdr = m_tdrmodule[count];
      QString delMsg = "**YYTDRYY** Delete TDR:" + QString::number((uint64_t)tdr);
      INFO_LOG(delMsg.toLocal8Bit().data());
      delete tdr;
    }
    m_tdrmodule.clear();
    m_localArchivePath.clear();
    m_bagInfo.clear();

    if(m_state == DISPOSED ||
       m_state == EMPTY ||
       m_state == ERROR)
    {
      if (m_state != ERROR)
      {
        clearErrorMsg();
      }
      m_bagid.clear();
      m_livebag = NULL;
      m_archiveSaveMode = false;
      m_isTIPBag = false;
      m_TIPPhase = TIP_NONE_PHASE;
      m_volumectmodule = NULL;
      if(m_uncroppedvolume != NULL){
        INFO_LOG("setState : Memsetting m_uncroppedvolume");
        memset(m_uncroppedvolume, 0, m_uncroppedVolColumns * m_uncroppedVolRows * 2 * m_uncropped_max_slices);
      }
    }
    m_availablealarmcount = 0;
    m_effectiveavailablealarmcount = 0;
    m_visiblealarmcount = 0;
    m_noAlarmcount = 0;
    m_isBagError = false;
    QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
    if(currentscreen == QMLEnums::RECALLBAG_SCREEN)
    {
      m_showAllThreat = DEFAULT_VALUE_RECALL_SHOW_ALL_THREAT;
    }
    else
    {
      m_showAllThreat = DEFAULT_VALUE_SHOW_ALL_THREAT;
    }

    if (m_sliceQueue)
    {
      while (!m_sliceQueue->isEmpty())
      {
        FATAL_LOG("Queue should be empty.Size");
        SliceDataStruct* slice = dequeueSlice();
        SAFE_DELETE(slice);
      }
      m_sliceQueue->clear();
    }
    m_obviousThreat = false;
    m_bagRenderedTime = QDateTime::currentDateTime();
    m_bagDecisionTime = QDateTime::currentDateTime();
  }
  else if(m_state == RENDERING)
  {
    m_bagDecisionTime = QDateTime::currentDateTime();
  }
  else if(m_state == READYTORENDER)
  {
    m_bagReadytoRenderTime = QDateTime::currentDateTime();
  }
  m_bagStateMutex.unlock();
}

/*!
* @fn       addErrorMsg
* @param    QString - msg
* @return   None
* @brief    add Error Msg.
*/
void BagData::addErrorMsg(QString msg)
{
  setError(true);
  m_errorMsg.append(msg+"\n");
}

/*!
* @fn       clearErrorMsg
* @param    None
* @return   None
* @brief    clear Error Msg.
*/
void BagData::clearErrorMsg()
{
  m_errorMsg = "";
}

/*!
* @fn       populateErrorMsgFromTDR
* @param    None
* @return   None
* @brief    Populate TDT Error.
*/
void BagData::populateErrorMsgFromTDR()
{
  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
  if(tdrArray.size() > 0)
  {
    for(unsigned int i = 0; i < tdrArray.size(); i++)
    {
      SDICOS::TDRModule* tdrModule;
      tdrModule = tdrArray.at(i);
      if (tdrModule != NULL)
      {
        SDICOS::TDRTypes::ThreatDetectionReport tdrreport =
            tdrModule->GetThreatDetectionReport();
        SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdrType =
            tdrreport.GetTdrType();
        SDICOS::TDRTypes::ThreatDetectionReport::ALARM_DECISION alarmdecision =
            tdrreport.GetAlarmDecision();
        SDICOS::TDRTypes::ThreatDetectionReport::ABORT_FLAG abortFlag =
            tdrreport.GetAbortFlag();
        SDICOS::TDRTypes::ThreatDetectionReport::ABORT_REASON abortReason = tdrreport.GetAbortReason();

        INFO_LOG("Abort Flag: " << (int)abortFlag);
        INFO_LOG("Tdr Type: " << (int)tdrType);
        INFO_LOG("Alarm Decision: " << (int)alarmdecision);
        INFO_LOG("Abort Reason: " << tdrreport.GetAbortReasonAsString().Get());

        if ((tdrType == SDICOS::TDRTypes::ThreatDetectionReport::enumMachine) &&
            (alarmdecision == SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision) &&
            (abortFlag == SDICOS::TDRTypes::ThreatDetectionReport::enumAbort))
        {
          QString msg;
          if (abortReason == SDICOS::TDRTypes::ThreatDetectionReport::enumOversizeBag)
          {
            msg = tr("Oversize bag.");
            setDetectionException(true);
          }
          else if (abortReason == SDICOS::TDRTypes::ThreatDetectionReport::enumClippedBag)
          {
            msg = tr("Clipped bag.");
            setDetectionException(true);
          }
          else if (abortReason == SDICOS::TDRTypes::ThreatDetectionReport::enumIncompleteScan)
          {
            msg = tr("Incomplete scan.");
            setDetectionException(true);
          }
          else if (abortReason == SDICOS::TDRTypes::ThreatDetectionReport::enumTimeout)
          {
            msg = tr("Detection timeout.");
          }
          else if (abortReason == SDICOS::TDRTypes::ThreatDetectionReport::enumNotReviewed)
          {
            msg = QString("");
          }
          else
          {
            msg = tr("Error in TDR");
          }
          addErrorMsg(msg);
          setError(true);
        }
        else if (getSliceCountReceived() < WorkstationConfig::getInstance()->getMinimumRequiredSliceCount())
        {
          QString msg;
          msg = tr("Loose object or clipped bag.");
          setDetectionException(true);
          clearErrorMsg();
          addErrorMsg(msg);
          setError(true);
        }
        else if (getSliceCountReceived() > 1000)
        {
          // ANSS-1727
          // Due to a bug in BLM that sometimes the Abort Flag wont be set but still
          // we want to display "X"
          // Can be removed later.
          QString msg;
          msg = tr("Oversize Bag.");
          setDetectionException(true);
          clearErrorMsg();
          addErrorMsg(msg);
          setError(true);
        }
        else if ((tdrType == SDICOS::TDRTypes::ThreatDetectionReport::enumOperator) &&
                 (alarmdecision == SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision) &&
                 (abortFlag == SDICOS::TDRTypes::ThreatDetectionReport::enumAbort))
        {
          QString reason;
          if (abortReason == SDICOS::TDRTypes::ThreatDetectionReport::enumTimeout)
          {
            reason = "Unprocessed";
          }
          else if (abortReason == SDICOS::TDRTypes::ThreatDetectionReport::enumNotReviewed)
          {
            reason = "Unprocessed";
          }
          else
          {
            reason = tdrreport.GetAbortReasonAsString().Get();
          }
          setUserDecision(reason);
        }
        else
        {
        }
      }
      else
      {
        ERROR_LOG("tdrmodule in bag data is null.");
        QString msg = tr("Error in tdr received. Tdrmodule in bag data is null.");
        addErrorMsg(msg);
        setError(true);
      }
    }
  }
  else
  {
    ERROR_LOG("Zero TDRs are received for bagdata");
    QString msg = tr("Error in tdr received. Zero TDRs are received for bagdata.");
    addErrorMsg(msg);
    setError(true);
  }
}

/*!
* @fn       getErrorMsg
* @param    None
* @return   QString - msg
* @brief    get Error Msg.
*/
QString BagData::getErrorMsg()
{
  return m_errorMsg;
}

/*!
* @fn       refreshAttributes
* @param    None
* @return   None
* @brief    refresh all bag related attributes
*/
void BagData::refreshAttributes()
{
  TRACE_LOG("");
  INFO_LOG("Total no alarm Threats: " << m_noAlarmcount
           << ", available Alarm Threats: " << m_availablealarmcount
           << ", Visible alarm count: " << m_visiblealarmcount);

  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
  SDICOS::TDRModule *tdrModule;
  m_availablealarmcount = 0;
  m_visiblealarmcount = 0;
  m_effectiveavailablealarmcount = 0;
  m_noAlarmcount = 0;

  bool enableThreatHandling ;
  if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)
  {
    enableThreatHandling = WorkstationConfig::getInstance()->showMachineAlarmDecision();
  }
  else
  {
    enableThreatHandling = WorkstationConfig::getInstance()->getEnableThreathandling();
  }

  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();

  if (tdrArray.size() > 0)
  {
    for (unsigned int k = 0; k < tdrArray.size(); k++)
    {
      tdrModule = tdrArray.at(k);
      SDICOS::TDRTypes::ThreatSequence ts = tdrModule->GetThreatSequence();
      SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatItemArray =
          ts.GetPotentialThreatObject();

      SDICOS::TDRTypes::ThreatDetectionReport& threat_report = tdrModule->GetThreatDetectionReport();
      SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();

      for (unsigned int i = 0; i < threatItemArray.GetSize(); i++)
      {
        SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object;
        threatItemArray[i].GetReferencedPotentialThreatObject(ref_object);
        if (ref_object.GetReferencedPotentialThreatObject().GetSize() == 0)
        {
          SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem> assesmentitm =
              threatItemArray[i].GetAssessment().GetAssessment();
          if(assesmentitm.GetSize() > 0)
          {
            SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG machineDecision
                = assesmentitm[0].GetAssessmentFlag();
            SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG userDecision;
            if((currentscreen == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN) ||
               (currentscreen == QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN))
            {
              userDecision = getUserDecisionfromPrimaryTdr(i);
            }
            else
            {
              userDecision = getCurrentThreatUserDecision(i);
            }
            DEBUG_LOG("Current Threat flag: " << machineDecision);
            if (machineDecision != SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
            {
              m_availablealarmcount++;
              if (((userDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknown)
                   ||(userDecision == SDICOS::TDRTypes::AssessmentSequence::
                      AssessmentItem::enumUnknownAssessmentFlag)))
              {
                if (!enableThreatHandling)
                {
                  if(tdr_type == SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE::enumOperator)
                  {
                    m_effectiveavailablealarmcount++;
                  }
                }
                else
                {
                  if(currentscreen == QMLEnums::SEARCH_BAG_SCREEN)
                  {
                    if(getUserDecisionfromPrimaryTdr(i) == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
                    {
                      if(WorkstationConfig::getInstance()->isDisplayOperatorClearedAlarmEnabled())
                      {
                        m_effectiveavailablealarmcount++;
                      }
                    }
                    else
                    {
                      m_effectiveavailablealarmcount++;
                    }
                  }
                  else
                  {
                    m_effectiveavailablealarmcount++;
                  }
                }
              }
              else if (userDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
              {
                if((currentscreen == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN) ||
                   (currentscreen == QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN))
                {
                  if(m_showAllThreat)
                  {
                    m_effectiveavailablealarmcount++;
                  }
                }
              }
              else if (userDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat)
              {
                if (((currentscreen == QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN)
                     ||(currentscreen == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN)))

                {
                  m_effectiveavailablealarmcount++;
                }
                else if (((currentscreen == QMLEnums::BAGSCREENING_SCREEN)
                          ||(currentscreen == QMLEnums::TRAINING_BAG_SCREEN))
                         && isOperatorTdr(tdrModule))
                {
                  m_effectiveavailablealarmcount++;
                }
              }
              else
              {
                return;
              }

              VREnums::ThreatType threatType = VREnums::THREATTYPE_UNKNOWN_THREAT;
              if(isValidVisibleEntity(tdrModule, i, threatType))
              {
                m_visiblealarmcount++;
              }
            }
            else
            {
              // ARO - Make sure only Laptops are valid Non-Threat alarms.
              SDICOS::DcsString dcsDescription = assesmentitm[0].GetThreatCategoryDescription();
              QString threatDescrioption = dcsDescription.Get();
              if (threatDescrioption.compare("Laptop", Qt::CaseInsensitive) == 0 )
              {
                m_noAlarmcount++;
              }
            }
          }
        }
      }
    }
  }
  else
  {
    ERROR_LOG("Tdrmodule in bag data is null.");
    return;
  }

  // Get Volume CT Files
  SDICOS::CTTypes::CTImage::CT_PIXEL_DATA_CHARACTERISTICS pixel;
  SDICOS::CTTypes::CTImage::OOI_IMAGE_CHARACTERISTICS ooi;
  SDICOS::CTTypes::CTImage::IMAGE_FLAVOR flavor;
  SDICOS::CTTypes::CTImage::DERIVED_PIXEL_CONTRAST contrast;
  for(unsigned int i = 0; i < m_ctmodule.GetSize(); i++)
  {
    m_ctmodule[i].GetCTImage().GetImageTypeCT(pixel, ooi, flavor, contrast);
    if(flavor == SDICOS::CTTypes::CTImage::enumVolume)
    {
      m_volumectmodule = &m_ctmodule[i];
      break;
    }
  }
}
/*!
* @fn       getTotalAvailableAlarmThreats
* @param    None
* @return   int
* @brief    returns total available alarm threat.
*/
int BagData::getTotalAvailableAlarmThreats()
{
  return m_availablealarmcount;
}

/*!
* @fn       getTotalEffectiveAvailableAlarmThreats
* @param    None
* @return   int
* @brief    returns total effective available alarm threat.
*/
int BagData::getTotalEffectiveAvailableAlarmThreats()
{
  return m_effectiveavailablealarmcount;
}

/*!
* @fn       setBagDecisionTime
* @param    None
* @return   None
* @brief    sets Bag Decision Time.
*/
void BagData::setBagDecisionTime()
{
  m_bagDecisionTime = QDateTime::currentDateTime();
  m_bagDecisionTimeTraining = QDateTime::currentDateTime();
}

/*!
 * @fn       getBagRenderTimeOnTraining
 * @param    None
 * @return   float
 * @brief    get bag render time on  training viewer screen
 */
QDateTime BagData::getBagRenderTimeOnTraining()
{
  return m_bagRenderedTimeTraining;
}

/*!
* @fn       getOperatorDecisiontime
* @param    None
* @return   float
* @brief    get time taken to take decision by operator for training simulator
*/
float BagData::getOperatorDecisiontime()
{
  return (m_bagDecisionTimeTraining.toMSecsSinceEpoch() - m_bagRenderedTimeTraining.toMSecsSinceEpoch());
}

/*!
* @fn       isValidVisibleEntity
* @param    SDICOS::TDRModule *tdrModule
* @param    unsigned int localThreatIndex
* @param    VREnums::ThreatType& entityType
* @return   boolean
* @brief    This function will validate visible entity for given tdr
*/
bool BagData::isValidVisibleEntity(SDICOS::TDRModule *tdrModule,
                                   unsigned int localThreatIndex,
                                   VREnums::ThreatType& entityType)
{
  TRACE_LOG("");
  bool isVisibleEntity = false;
  entityType = VREnums::THREATTYPE_UNKNOWN_THREAT;
  if ((tdrModule == nullptr))
  {
    return false;
  }

  SDICOS::TDRTypes::ThreatSequence ts = tdrModule->GetThreatSequence();
  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatItemArray =
      ts.GetPotentialThreatObject();


  if (threatItemArray.GetSize() > 0 && localThreatIndex < threatItemArray.GetSize())
  {
    SDICOS::TDRTypes::ThreatDetectionReport& threat_report = tdrModule->GetThreatDetectionReport();
    SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();

    bool enableThreatHandling ;
    if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)
    {
      enableThreatHandling = WorkstationConfig::getInstance()->showMachineAlarmDecision();
    }
    else
    {
      enableThreatHandling = WorkstationConfig::getInstance()->getEnableThreathandling();
    }

    QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
    SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object;
    threatItemArray[localThreatIndex].GetReferencedPotentialThreatObject(ref_object);

    if (ref_object.GetReferencedPotentialThreatObject().GetSize() == 0)
    {
      SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem> assesmentitm =
          threatItemArray[localThreatIndex].GetAssessment().GetAssessment();

      if(assesmentitm.GetSize() > 0)
      {
        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG machineDecision
            = assesmentitm[0].GetAssessmentFlag();

        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG userDecision
            = getCurrentThreatUserDecision(static_cast<unsigned int>(localThreatIndex));

        DEBUG_LOG("Current Threat flag: " << machineDecision);
        if (machineDecision != SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
        {
          if ((getCurrentVRViewmode() == VREnums::VIEW_AS_THREAT) ||
              (getCurrentVRViewmode() == VREnums::VIEW_AS_SURFACE) ||
              ((getCurrentVRViewmode() == VREnums::VIEW_AS_SLAB) && getCurrentSlabViewType() != VREnums::LAPTOP_VIEW))
          {
            if(tdr_type == SDICOS::TDRTypes::ThreatDetectionReport::enumTruth)
            {
              if ((getTIPPhase() != TIP_INITIAL_DECISION_PHASE)
                  && (((machineDecision == SDICOS::TDRTypes::AssessmentSequence
                        ::AssessmentItem::enumUnknown)
                       || (machineDecision == SDICOS::TDRTypes::AssessmentSequence
                           ::AssessmentItem::enumThreat))))
              {
                isVisibleEntity = true;
                entityType = VREnums::THREATTYPE_TRUTH_THREAT;
              }
            }
            else
            {
              if (tdr_type == SDICOS::TDRTypes::ThreatDetectionReport::enumOperator)
              {
                if(currentscreen == QMLEnums::SEARCH_BAG_SCREEN)
                {
                  if(userDecision ==
                     SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknown)
                  {
                    isVisibleEntity = true;
                    entityType = VREnums::THREATTYPE_OPERTOR_THREAT;
                  }
                }
                else
                {
                  isVisibleEntity = true;
                  entityType = VREnums::THREATTYPE_OPERTOR_THREAT;
                }
              }
              else
              {
                if ((currentscreen == QMLEnums::BAGSCREENING_SCREEN)
                    || (currentscreen == QMLEnums::TRAINING_BAG_SCREEN))
                {
                  if(userDecision ==
                     SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknown)
                  {
                    if(enableThreatHandling)
                    {
                      isVisibleEntity = true;
                      entityType = VREnums::THREATTYPE_ATR_THREAT;
                    }
                  }
                }
                else if (currentscreen ==  QMLEnums::SEARCH_BAG_SCREEN)
                {
                  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem
                      ::ASSESSMENT_FLAG primaryUserDecision = getUserDecisionfromPrimaryTdr
                      (static_cast<unsigned int>(localThreatIndex));

                  if(userDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknown)
                  {
                    if(primaryUserDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
                    {
                      if(WorkstationConfig::getInstance()->isDisplayOperatorClearedAlarmEnabled())
                      {
                        isVisibleEntity = true;
                        entityType = VREnums::THREATTYPE_ATR_THREAT;
                      }
                    }
                    else
                    {
                      isVisibleEntity = true;
                      entityType = VREnums::THREATTYPE_ATR_THREAT;
                    }
                  }
                }
                else if((currentscreen == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN) ||
                        (currentscreen == QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN))
                {
                  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem
                      ::ASSESSMENT_FLAG primaryUserDecision = getUserDecisionfromPrimaryTdr
                      (static_cast<unsigned int>(localThreatIndex));
                  if(primaryUserDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
                  {
                    if(m_showAllThreat)
                    {
                      isVisibleEntity = true;
                      entityType = VREnums::THREATTYPE_ATR_THREAT;
                    }
                  }
                  else
                  {
                    isVisibleEntity = true;
                    entityType = VREnums::THREATTYPE_ATR_THREAT;
                  }
                }
                else
                {
                  isVisibleEntity = true;
                  entityType = VREnums::THREATTYPE_ATR_THREAT;
                }
              }
            }
          }
        }
        else
        {
          if (getCurrentVRViewmode() == VREnums::VIEW_AS_LAPTOP
              ||((getCurrentVRViewmode() == VREnums::VIEW_AS_SLAB)
                 && (getCurrentSlabViewType() == VREnums::LAPTOP_VIEW)))
          {
            isVisibleEntity = true;
            DEBUG_LOG("Current Threat is laptop flag Alarm threat");
            entityType = VREnums::THREATTYPE_LAPTOP;
          }
        }
      }
    }
  }
  return isVisibleEntity;
}


/*!
* @fn       getTotalVisibleAlarmThreats
* @param    None
* @return   int
* @brief    returns total visible alarm threat.
*/
int BagData::getTotalVisibleAlarmThreats()
{
  return m_visiblealarmcount;
}

/*!
* @fn       getTotalNoAlarmThreats
* @param    None
* @return   int
* @brief    returns total alarm threat.
*/
int BagData::getTotalNoAlarmThreats()
{
  return m_noAlarmcount;
}


/*!
* @fn       getTotalAvailableThreats
* @param    None
* @return   int
* @brief    returns total threat alarm and non alarm.
*/
int BagData::getTotalAvailableThreats()
{
  return m_noAlarmcount + m_availablealarmcount;
}

/*!
* @fn       setRowsColumns
* @param    rows - number of rows in volume data.
* @return   columns - number of columns in volume data.
* @brief    Set number of rows and columns.
*/
void BagData::setRowsColumns(int rows, int columns)
{
  m_volRows = rows;
  m_volColumns = columns;
  DEBUG_LOG("Rows and Columns of volume data: "
            << m_volRows << ", " << m_volColumns);
}

/*!
* @fn       getCtModules
* @param    None
* @return   SDICOS::Array1D<SDICOS::CTModule>
* @brief    get CT data
*/
SDICOS::Array1D<SDICOS::CTModule> BagData::getCtModules()
{
  TRACE_LOG("");
  SDICOS::Array1D<SDICOS::CTModule> ctModules;

  int countCtModules = m_ctmodule.GetSize();
  for(int i = 0; i < countCtModules; i++)
    ctModules.Add(m_ctmodule[i]);

  // Note: Below operations are required by current implementation of
  // volume_renderer. But on performing them, the CTModule.isValid()
  // starts returning false and saving to file fails. Therefore kept
  // these in get() instead of set().
  DEBUG_LOG("Starting to get CTModule data where module count is: " << countCtModules);
  for(int i = 0; i < countCtModules; i++)
  {
    ctModules[i].GetCTImage().DeletePixelData();
    ctModules[i].GetCTImage().SetImageInfo(m_volRows,
                                           m_volColumns,
                                           16,
                                           14,
                                           14,
                                           SDICOS::ImagePixelMacro::enumUnsignedInteger);
  }
  return ctModules;
}

/*!
* @fn       getVolumeCtModule
* @param    None
* @return   SDICOS::CTModule *
* @brief    get CT data
*/
SDICOS::CTModule *BagData::getVolumeCtModule()
{
  return m_volumectmodule;
}

/*!
* @fn       setVolumeCtModule
* @param    SDICOS::CTModule *
* @return   None
* @brief    set CT data
*/
void BagData::setVolumeCtModule(SDICOS::CTModule *ct)
{
  m_volumectmodule = ct;
}

/*!
* @fn       addCtModule
* @param    SDICOS::CTModule *
* @return   None
* @brief    Add a CTModule
*/
void BagData::addCtModule(SDICOS::CTModule *ctmodule)
{
  if (NULL != ctmodule)
  {
    m_ctmodule.Add(*ctmodule);
    INFO_LOG("Add CTModule completed.");
  }
}

/*!
* @fn       getTdrModule
* @param    None
* @return   SDICOS::Array1D<SDICOS::TDRModule>
* @brief    get TDR data
*/
std::vector<SDICOS::TDRModule*> BagData::getTdrModules()
{
  return m_tdrmodule;
}

/*!
* @fn       addTdrModule
* @param    SDICOS::TDRModule *value
* @return   SDICOS::TDRModule *
* @brief    Add Tdr Module
*/
SDICOS::TDRModule *BagData::addTdrModule(SDICOS::TDRModule *value)
{
  SDICOS::TDRModule  *tdr = NULL;
  if (value)
  {
    DEBUG_LOG("Set TdrModule list using: " << value);
    tdr = new SDICOS::TDRModule(*value);
    QString delMsg = "Create TDR:" + QString::number((uint64_t)tdr);
    INFO_LOG(delMsg.toLocal8Bit().data());
    m_tdrmodule.push_back(tdr);
  }
  return tdr;
}

/*!
* @fn       getBagid
* @param    None
* @return   string - bagid
* @brief    get bag id
*/
std::string BagData::getBagid() const
{
  DEBUG_LOG("Bagid is: "<< m_bagid.c_str());
  return m_bagid;
}

/*!
* @fn       setBagid
* @param    const std::string &bagid
* @return   None
* @brief    set bag id
*/
void BagData::setBagid(const std::string &bagid)
{
  m_bagid = bagid;
  DEBUG_LOG("Setting Bagid to: "<< m_bagid.c_str());
}

/*!
* @fn       getvolumedata
* @param    None
* @return   const unsigned char* - volume buffer
* @brief    gets volume data buffer
*/
const unsigned char* BagData::getvolumedata() const
{
  return m_volume;
}

/*!
* @fn       setVolumeSliceData
* @param    void *buffer - contains slice data
* @param    size_t length
* @return   None
* @brief    set volume slice data
*/
void BagData::setVolumeSliceData(void *buffer, size_t length)
{
  DEBUG_LOG("1BagData::setVolumeSliceData() Set volume slice data of size: " << length);
  memcpy(&m_uncroppedvolume[m_data_size], buffer, length);
  m_data_size += length;
}

/*!
* @fn       setVolumeSliceData
* @param    void *buffer - contains slice data
* @param    size_t length
* @param    int - slice count
* @return   None
* @brief    set volume slice data
*/
void BagData::setVolumeSliceData(void *buffer, size_t length, int sliceCount)
{
  DEBUG_LOG("2BagData::setVolumeSliceData() Set volume slice data of size: " << length);
  memcpy(&m_uncroppedvolume[sliceCount*m_volRows*m_volColumns*2], buffer, length);
  m_data_size += length;
}

/*!
* @fn       getLivebag
* @param    None
* @return   analogic::nss::NssBag *
* @brief    gets live bag data
*/
analogic::nss::NssBag *BagData::getLivebag()
{
  if (m_livebag != NULL)
  {
    DEBUG_LOG("Current livebag id: " << m_livebag->get_transfer_bag_identifier());
  }
  else
  {
    ERROR_LOG("Current livebag id: NULL");
  }
  return m_livebag;
}

/*!
* @fn       setLivebag
* @param    None
* @return   analogic::nss::NssBag *
* @param    set live bag
*/
void BagData::setLivebag(analogic::nss::NssBag* livebag)
{
  m_data_size = 0;
  m_livebag = livebag;
  if (m_livebag != NULL)
  {
    DEBUG_LOG("Setting current livebag to: "<< m_livebag->get_transfer_bag_identifier());
  }
  else
  {
    ERROR_LOG("Setting current livebag to: NULL");
  }
}

/**
* @fn       addKeystroke()
* @param    keystroke String definition of the keystroke: 2 Character code
* @param    alarm_type If pressed while viewing a threat, the type of threat. Otherwise, blank.
* @brief    recorded keystroke for a bag.
*/
void BagData::addKeystroke(QString keystroke, QString alarm_type)
{
  if(m_livebag)
  {
    quint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    DEBUG_LOG("Adding Keystroke user action- keystroke: " << keystroke.toStdString()
              << " timestamp: " << timestamp << " alarm_type: " << alarm_type.toStdString());
    analogic::nss::BasicCommandResult result = m_livebag->AddKeystroke(keystroke.toStdString(), timestamp, alarm_type.toStdString());
    if(result != analogic::nss::BASIC_COMMAND_RESULT_SUCCESS)
    {
      ERROR_LOG("Error adding keystroke user action: " << analogic::nss::basic_command_result_to_string(result));
    }
  }
}

/*!
* @fn       getState
* @param    None
* @return   BagState
* @param    gets Bag state
*/
BagData::BagState BagData::getState()
{
  BagState localState;
  m_bagStateMutex.lock();
  localState = m_state;
  DEBUG_LOG("Bag state is: " << (int)m_state);
  m_bagStateMutex.unlock();
  return localState;
}

/*!
* @fn       isBagEmpty
* @param    None
* @return   bool - Is the bag empty?
* @brief    Tells if the bag is empty.
*/
bool BagData::isBagEmpty()
{
  bool result = false;
  m_bagStateMutex.lock();
  if (m_state == INITIALISED ||
      m_state == CONSTRUCTION ||
      m_state == READYTORENDER ||
      m_state == RENDERING)
  {
    result = false;
  }
  else{
    result =  true;
  }
  m_bagStateMutex.unlock();
  return result;
}

/*!
* @fn       constructFromFiles
* @param    QString bagDataFilePath
* @param    QString tdrFilePath
* @return   bool success or failure
* @brief    Construct BagData object from archived files.
*/
bool BagData::constructFromFiles(QString bagDataFilePath, QString tdrFilePath)
{
  setBagReqLoc(QMLEnums::LOCAL_SERVER); //to check imact on rerun and training.
  TRACE_LOG("");
  INFO_LOG("Construct Bag from file: " << bagDataFilePath.toStdString().c_str());
  INFO_LOG("TDR file: " << tdrFilePath.toStdString().c_str());
  QString msg;
  QString fname = bagDataFilePath;
  bool status = true;
  bool volumeFilePresent = true;
  QString volFileFormat;

  setState(BagState::CONSTRUCTION);
  // checking of the Bag List File to make sure that
  // ".properties", ".dicos_ct_vol", ".dicos_tdr_1"(atleast 1), ".vol"
  // files exist

  fname += BAGFILE_PROPERTY_FORMAT;
  QFile infoFile(fname.toStdString().c_str());
  if(!infoFile.exists())
  {
    msg = tr("File doesn't exist: ") + fname;
    ERROR_LOG(msg.toStdString());
    addErrorMsg(msg);
    status = false;
  }

  if (WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::RERUN_EMULATOR)
  {
    QFileInfo rernFile(fname);
    m_rerunBagFilename = rernFile.baseName();
  }
  DEBUG_LOG("Status: " << status);
  if (status) {
    fname = bagDataFilePath;
    fname += BAGFILE_DICOSCT_FORMAT;
#ifdef RERUN
    infoFile.setFileName(fname.toStdString().c_str());
    if (!infoFile.exists())
    {
      fname += ".dcs";
      infoFile.setFileName(fname.toStdString().c_str());
      if (!infoFile.exists())
      {
        msg = tr("File doesn't exist: ") + fname;
        ERROR_LOG(msg.toStdString());
        addErrorMsg(msg);
        status = false;
      }
    }
#else
    fname += ".dcs";
    infoFile.setFileName(fname.toStdString().c_str());
    if (!infoFile.exists())
    {
      msg = tr("File doesn't exist: ") + fname;
      ERROR_LOG(msg.toStdString());
      addErrorMsg(msg);
      status = false;
    }
#endif
  }

  if (status) {
    fname = tdrFilePath;
    fname += "/";
    fname += bagDataFilePath.section("/", -1, -1);
    fname += BAGFILE_DICOSTDR_FORMAT;
    fname += "1";
#ifdef RERUN
    infoFile.setFileName(fname.toStdString().c_str());
    if (!infoFile.exists())
    {
      fname += ".dcs";
      infoFile.setFileName(fname.toStdString().c_str());
      if (!infoFile.exists())
      {
        msg = tr("Not a single TDR file exists: ") + fname;
        ERROR_LOG(msg.toStdString());
        addErrorMsg(msg);
        status = false;
      }
    }
#else
    fname += ".dcs";
    infoFile.setFileName(fname.toStdString().c_str());
    if (!infoFile.exists())
    {
      msg = tr("Not a single TDR file exists: ") + fname;
      ERROR_LOG(msg.toStdString());
      addErrorMsg(msg);
      status = false;
    }
#endif
  }

  // Read metadata from .properties file.
  if(status)
  {
    fname = bagDataFilePath;
    fname += BAGFILE_PROPERTY_FORMAT;

    if(m_bagInfo.constructFromFile(fname))
    {
      m_volumedimension.setZ(m_bagInfo.m_sliceCount.toInt());
      m_volumedimension.setX(m_bagInfo.m_volWidth.toInt());
      m_volumedimension.setY(m_bagInfo.m_volHeight.toInt());
      m_roiStart = m_bagInfo.m_roiStart;
      m_roiEnd = m_bagInfo.m_roiEnd;
      m_localArchivePath = fname;
      m_uncroppedVolColumns = m_volColumns;
      m_uncroppedVolRows = m_volRows;
      m_volColumns = m_volumedimension.x();
      m_volRows = m_volumedimension.y();
    }
    else
    {
      msg =  tr("Failed to read: ") + fname;
      ERROR_LOG(msg.toStdString());
      addErrorMsg(msg);
      status = false;
    }
  }

  if (status) {
    fname = bagDataFilePath;
    if(m_bagInfo.m_sliceFormat == VOLUME_FORMAT_JPEG)
    {
      volFileFormat = BAGFILE_JPEG_VOLUME_FORMAT;
    }
    else if(m_bagInfo.m_sliceFormat == VOLUME_FORMAT_UNCOMPRESSED)
    {
      volFileFormat = BAGFILE_VOLUME_FORMAT;
    }
    else
    {
      status = false;
    }
    if(status)
    {
      fname += volFileFormat;
      infoFile.setFileName(fname.toStdString().c_str());
      if (!infoFile.exists())
      {
        ERROR_LOG(".Vol file dosent exist. Checking if JPEG Vol file is present" << fname.toStdString());
        fname = bagDataFilePath;
        volFileFormat = BAGFILE_JPEG_VOLUME_FORMAT;
        fname += volFileFormat;
        infoFile.setFileName(fname.toStdString().c_str());
        if (!infoFile.exists())
        {
          volumeFilePresent = false;
          ERROR_LOG("JPEG Vol file dosent exist" << fname.toStdString());
        }
        else
        {
          volumeFilePresent = true;
          m_bagInfo.m_sliceFormat = VOLUME_FORMAT_JPEG;
          INFO_LOG("JPEG Vol file exist" << fname.toStdString());
        }
      }
    }
  }


  // Read TDR Modules.
  if(status)
  {
    int i = 1;
    bool tdrFilePresent = true;
    fname = tdrFilePath;
    fname += "/";
    fname += bagDataFilePath.section("/", -1, -1);
    fname += BAGFILE_DICOSTDR_FORMAT;
    fname += QString::number(i);
#ifdef RERUN
    infoFile.setFileName(fname);
    if(!infoFile.exists())
    {
      fname += ".dcs";
    }
#else
    fname += ".dcs";
#endif
    while (tdrFilePresent)
    {
      SDICOS::ErrorLog errorLog;
      SDICOS::TDRModule* tdrModule = new SDICOS::TDRModule();

      QString delMsg = "Create TDR:" + QString::number((uint64_t)tdrModule);
      ERROR_LOG(delMsg.toLocal8Bit().data());

      SDICOS::Filename filename;
      filename.Set(fname.toStdString());

      bool result = tdrModule->Read(filename, errorLog);

      if(!result)
      {
        msg = tr("DICOS TDR file read failed: ") + fname
            + tr(". Error: ") + errorLog.GetErrorLog().Get();
        ERROR_LOG(msg.toStdString());
        addErrorMsg(msg);
        status = false;
        delete tdrModule;
        break;
      }
      // Set TDR Type from tdr module
      SDICOS::TDRTypes::ThreatDetectionReport& threat_report = tdrModule->GetThreatDetectionReport();
      SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();
      if (tdr_type == SDICOS::TDRTypes::ThreatDetectionReport::enumOperator)
      {
        m_operatorTdr = tdrModule;
      }

      m_tdrmodule.push_back(tdrModule);

      ++i;
      fname = tdrFilePath;
      fname += "/";
      fname += bagDataFilePath.section("/", -1, -1);
      fname += BAGFILE_DICOSTDR_FORMAT;
      fname += QString::number(i);
#ifdef RERUN
      infoFile.setFileName(fname.toStdString().c_str());
      if(!infoFile.exists())
      {
        fname += ".dcs";
        infoFile.setFileName(fname.toStdString().c_str());
        if(!infoFile.exists())
        {
          tdrFilePresent = false;
        }
      }
#else
      fname += ".dcs";
      infoFile.setFileName(fname.toStdString().c_str());
      if(!infoFile.exists())
      {
        tdrFilePresent = false;
      }
#endif
    }
  }

  // Read CT Modules.
  if(status)
  {
    fname = bagDataFilePath;
    fname += BAGFILE_DICOSCT_FORMAT;
#ifdef RERUN
    infoFile.setFileName(fname);
    if(!infoFile.exists())
    {
      fname += ".dcs";
    }
#else
    fname += ".dcs";
#endif

    SDICOS::ErrorLog errorLog;
    SDICOS::CTModule ctModule;
    SDICOS::Filename filename;
    filename.Set(fname.toStdString());

    bool result = ctModule.Read(filename, errorLog);

    if(!result)
    {
      msg = tr("DICOS CT file read failed: ") + fname
          + tr(". Error: ") + errorLog.GetErrorLog().Get();
      ERROR_LOG(msg.toStdString());
      addErrorMsg(msg);
      status = false;
    }
    else
    {
      m_ctmodule.Add(ctModule);
      if(!volumeFilePresent)
      {
        SDICOS::CTTypes::CTImage &image = ctModule.GetCTImage();
        SDICOS::Array3DLarge<SDICOS::S_UINT16> dicos_volume;
        image.GetPixelData(dicos_volume);
        for(int i = 0; i <m_bagInfo.m_sliceCount.toInt(); i++)
        {
          memcpy(&m_volume[i * m_bagInfo.m_volWidth.toInt()*m_bagInfo.m_volHeight.toInt()*sizeof(SDICOS::S_UINT16)],
              dicos_volume[i].GetBuffer(),
              (m_bagInfo.m_volWidth.toInt()*m_bagInfo.m_volHeight.toInt()*sizeof(SDICOS::S_UINT16)));
        }
        setSliceCountReceived(m_bagInfo.m_sliceCount.toInt());
        m_uncropped_max_slices = getSliceCountReceived();
      }
      status = true;
    }

    //////////////////////////////////////////////////////////////////
    fname = bagDataFilePath;
    fname += BAGFILE_DICOSCT_PROJ00_FORMAT;
    filename.Set(fname.toStdString());
    result = ctModule.Read(filename, errorLog);
    if(result)
    {
      m_ctmodule.Add(ctModule);
      status = true;
    }
    //////////////////////////////////////////////////////////////////
    fname = bagDataFilePath;
    fname += BAGFILE_DICOSCT_PROJ90_FORMAT;
    filename.Set(fname.toStdString());
    result = ctModule.Read(filename, errorLog);
    if(result)
    {
      m_ctmodule.Add(ctModule);
      status = true;
    }
    //////////////////////////////////////////////////////////////////
  }

  // Read the volume data.
  if(status && volumeFilePresent)
  {
    fname = bagDataFilePath;
    fname += volFileFormat;

    FILE *fp = std::fopen(fname.toStdString().c_str(), "rb");
    if(fp)
    {
      // obtain file size:
      std::fseek (fp , 0 , SEEK_END);
      long lSize = std::ftell (fp);
      std::rewind (fp);

      size_t bytes = std::fread(m_volume, 1, lSize, fp);
      if(bytes <= 0)
      {
        msg = tr("Incorrect number of bytes read from vol file: ") + QString("%1").arg(bytes);
        ERROR_LOG(msg.toStdString());
        addErrorMsg(msg);
        status = false;
      }
      else
      {
        status = true;
        int readSliceCount;
        if(m_bagInfo.m_sliceFormat == VOLUME_FORMAT_JPEG)
        {
          std::rewind (fp);
          readSliceCount = m_bagInfo.m_sliceCount.toInt();
          long size = (m_volumedimension.x()*m_volumedimension.y()*2);
          if(size <= 0)
          {
            msg =  tr("Failed to read: ") + fname;
            ERROR_LOG(msg.toStdString());
            addErrorMsg(msg);
            status = false;
          }
          else
          {
            char buffer[size];
            for(int i = 0; i < readSliceCount; i++)
            {
              uint32_t compressed_slice_size;

              if (fread(&compressed_slice_size, sizeof(char), sizeof(uint32_t), fp) != sizeof(uint32_t))
              {
                status = false;
                break;
              }
              if (fread(buffer, sizeof(char), compressed_slice_size, fp) != compressed_slice_size ||
                  ferror(fp) != 0)
              {
                status = false;
                break;
              }
              addSliceDataToQueueForDecompression(buffer, compressed_slice_size);
            }

            // Wait Till all slices get decrompressed
            while (!isSliceQueueEmpty() &&
                   getSliceProcessed() != getVolumeDimension().z())
            {
              if(status == false)
              {
                break;
              }
              DEBUG_LOG("Waiting for Data processing threads to complete."
                        << " Slice processed: " << getSliceProcessed()
                        << ", Slice Received: " << getSliceCountReceived()
                        << ", Total slices: " << getVolumeDimension().z());
              usleep(10 * 1000);  // sleep for 10ms
            }

            // changing bag properties volume format to uncompressed
            setvolumeFormat(VOLUME_FORMAT_UNCOMPRESSED);
          }
        }
        else if(m_bagInfo.m_sliceFormat == VOLUME_FORMAT_UNCOMPRESSED)
        {
          m_data_size = bytes;
          readSliceCount = m_data_size / (m_volumedimension.x()*m_volumedimension.y()*2);
          setSliceCountReceived(readSliceCount);
        }
        else
        {
          status = false;
        }
        fclose(fp);
      }
    }
    else
    {
      msg = tr("Failed to open: ") + fname;
      ERROR_LOG(msg.toStdString());
      addErrorMsg(msg);
      status = false;
    }
  }
  if(status)
  {
    int roiwidth = m_roiEnd.x()-m_roiStart.x()+2;
    int roiheight = m_roiEnd.y()-m_roiStart.y()+2;
    int roidepth = m_roiEnd.z()-m_roiStart.z()+2;
    if((roiwidth < m_volumedimension.x()) ||
       (roiheight < m_volumedimension.y()) ||
       (roidepth < m_volumedimension.z()))
    {
      cropVolumeAccordingtoROI();
    }
    refreshAttributes();
  }

  // Mark the bag as rendered, for display manager to be able to pick it up.
  if(status)
  {
    DEBUG_LOG("BagState::READYTORENDER");
    setState(BagState::READYTORENDER);
  }
  else
  {
    DEBUG_LOG("BagState::ERROR");
    setState(BagState::ERROR);
  }
  return status;
}


/*!
* @fn       isCtModuleProj90
* @param    const SDICOS::CTModule *ct_module
* @return   true or false
* @brief    Check if CT Module is Proj 90 or not
*/
bool BagData::isCtModuleProj90(SDICOS::CTModule const *ct_module)
{
  float fRowCosineX;
  float fRowCosineY;
  float fRowCosineZ;
  float fColumnCosineX;
  float fColumnCosineY;
  float fColumnCosineZ;
  SDICOS::CTTypes::CTImage::CT_PIXEL_DATA_CHARACTERISTICS pixel;
  SDICOS::CTTypes::CTImage::OOI_IMAGE_CHARACTERISTICS ooi;
  SDICOS::CTTypes::CTImage::IMAGE_FLAVOR flavor;
  SDICOS::CTTypes::CTImage::DERIVED_PIXEL_CONTRAST contrast;
  ct_module->GetCTImage().GetImageTypeCT(pixel, ooi, flavor, contrast);

  if(SDICOS::CTTypes::CTImage::enumProjection == flavor)
  {
    ct_module->GetMultiframeFunctionalGroups().GetSharedFunctionalGroups()->GetPlaneOrientation()->GetImageOrientation(fRowCosineX,
                                                                                                                       fRowCosineY,
                                                                                                                       fRowCosineZ,
                                                                                                                       fColumnCosineX,
                                                                                                                       fColumnCosineY,
                                                                                                                       fColumnCosineZ);
    INFO_LOG("isCtModuleProj90: " << fRowCosineX << " " << fRowCosineY << " "
             << fRowCosineZ << " " << fColumnCosineX << " " << fColumnCosineY
             << " " << fColumnCosineZ);

    if(fRowCosineX == 0 &&   \
       fRowCosineY == -1 &&   \
       fRowCosineZ == 0 &&   \
       fColumnCosineX == 1 &&   \
       fColumnCosineY == 0 &&   \
       fColumnCosineZ ==  0)
    {
      INFO_LOG("isCtModuleProj90: Found to be a Proj 90 CT Module");
      return true;
    }
  }

  return false;
}


/*!
* @fn       isCtModuleProj00
* @param    const SDICOS::CTModule *ct_module
* @return   true or false
* @brief    Check if CT Module is Proj 00 or not
*/
bool BagData::isCtModuleProj00(const SDICOS::CTModule *ct_module)
{
  float fRowCosineX;
  float fRowCosineY;
  float fRowCosineZ;
  float fColumnCosineX;
  float fColumnCosineY;
  float fColumnCosineZ;
  SDICOS::CTTypes::CTImage::CT_PIXEL_DATA_CHARACTERISTICS pixel;
  SDICOS::CTTypes::CTImage::OOI_IMAGE_CHARACTERISTICS ooi;
  SDICOS::CTTypes::CTImage::IMAGE_FLAVOR flavor;
  SDICOS::CTTypes::CTImage::DERIVED_PIXEL_CONTRAST contrast;
  ct_module->GetCTImage().GetImageTypeCT(pixel, ooi, flavor, contrast);

  if(SDICOS::CTTypes::CTImage::enumProjection == flavor)
  {
    ct_module->GetMultiframeFunctionalGroups().GetSharedFunctionalGroups()->GetPlaneOrientation()->GetImageOrientation(fRowCosineX,
                                                                                                                       fRowCosineY,
                                                                                                                       fRowCosineZ,
                                                                                                                       fColumnCosineX,
                                                                                                                       fColumnCosineY,
                                                                                                                       fColumnCosineZ);

    INFO_LOG("isCtModuleProj00: " << fRowCosineX << " " << fRowCosineY << " "
             << fRowCosineZ << " " << fColumnCosineX << " " << fColumnCosineY
             << " " << fColumnCosineZ);

    if(fRowCosineX == 1 &&   \
       fRowCosineY == 0 &&   \
       fRowCosineZ == 0 &&   \
       fColumnCosineX == 0 &&   \
       fColumnCosineY == 1 &&   \
       fColumnCosineZ ==  0)
    {
      INFO_LOG("isCtModuleProj00: Found to be a Proj 00 CT Module");
      return true;
    }
  }

  return false;
}


/*!
* @fn       saveToFiles
* @param    const std::string& folderPath
* @param    const std::string& filenamePrefix
* @return   Success or failure
* @param    Save bag data to files.
*/
QString BagData::saveToFiles(const std::string& folderPath, const std::string&
                             filenamePrefix)
{
  TRACE_LOG("");
  std::string fileName = filenamePrefix;
  INFO_LOG("Saving bag locally. Folder: " << folderPath.c_str() << ", FileNamePrefix: " << fileName.c_str());
  bool status = true;
  QString result = "";
  QString volFileFormat;
  if(getvolumeFormat() == VOLUME_FORMAT_JPEG)
  {
    volFileFormat = BAGFILE_JPEG_VOLUME_FORMAT;
  }
  else if(getvolumeFormat() == VOLUME_FORMAT_UNCOMPRESSED)
  {
    volFileFormat = BAGFILE_VOLUME_FORMAT;
  }
  else
  {
    status = false;
    result = tr("The file formate is not proper");
  }
  if(status)
  {
    if(isTIPBag())
    {
      if(UILayoutManager::getUILayoutManagerInstance()->getScreenState() ==
         QMLEnums::ScreenEnum::BAGSCREENING_SCREEN)
      {
        INFO_LOG("This is the tip bag, not going to save locally");
        return result;
      }
      else
      {
        fileName = TIP_BAG_PREFIX + fileName;
      }
    }
  }

  if(status)
  {
    DEBUG_LOG("Folderpath: " << folderPath.c_str() << " , fileName: " << fileName);
    if(!QDir(folderPath.c_str()).exists())
    {
      ERROR_LOG("Folder does not exists : " << folderPath);
      status = false;
      result = tr("Folder does not exists");
    }
  }

  // Write CTModule files
  int countCtModules = m_ctmodule.GetSize();
  if(status)
  {
    if(countCtModules < 1 || countCtModules > 9999)
    {
      ERROR_LOG("Invalid number of CTModules: " << countCtModules);
      status = false;
      result = tr("Invalid number of CTModules: ") + countCtModules;
    }
  }

  if(status && DEFAULT_VALUE_SAVE_VOLUME_FILE)
  {
    std::string fname = folderPath;
    fname += "/";
    fname += fileName;
    fname += volFileFormat.toStdString();

    FILE *fp = std::fopen(fname.c_str(), "wb");
    if(fp)
    {
      DEBUG_LOG("Writing Volume file: "<< fname.c_str());
      size_t ret = std::fwrite(m_volume, 1, m_data_size, fp);
      if((ret == 0) || (ret < m_data_size))
      {
        ERROR_LOG("Failed to create volume file: "<< fname.c_str());
        status = false;
        result = tr("Failed to create volume file: ");
        result.append(fname.c_str());
        result.append(tr("<br />Please check disk space"));
        status = false;
      }
      fclose(fp);
    }
    else
    {
      ERROR_LOG("Failed to create volume file: "<< fname.c_str());
      status = false;
      result = tr("Failed to create volume file: ");
      result.append(fname.c_str());
    }
  }

  if(status)
  {
    INFO_LOG("Start writing CTModule files...");
    // For Now, only one ctModule is present
    for(int i = 0; i < countCtModules; i++)
    {
      std::string fname = folderPath;
      fname += "/";
      fname += fileName;


      SDICOS::CTTypes::CTImage::CT_PIXEL_DATA_CHARACTERISTICS pixel;
      SDICOS::CTTypes::CTImage::OOI_IMAGE_CHARACTERISTICS ooi;
      SDICOS::CTTypes::CTImage::IMAGE_FLAVOR flavor;
      SDICOS::CTTypes::CTImage::DERIVED_PIXEL_CONTRAST contrast;
      m_ctmodule[i].GetCTImage().GetImageTypeCT(pixel, ooi, flavor, contrast);
      if(flavor == SDICOS::CTTypes::CTImage::enumVolume)
      {
        fname += BAGFILE_DICOSCT_FORMAT;

        /// Add Slice Data Inside CtModule
        SDICOS::Array3DLarge<SDICOS::S_UINT16> dicos_volume;
        SDICOS::S_UINT16 sliceCount = static_cast<SDICOS::S_UINT16>(getSliceCountReceived());
        SDICOS::S_UINT16 kunCompressedSliceWidth = static_cast<SDICOS::S_UINT16>(m_volColumns);
        SDICOS::S_UINT16 kunCompressedSliceHeight = static_cast<SDICOS::S_UINT16>(m_volRows);
        /// Set the size of volume image
        dicos_volume.SetSize(kunCompressedSliceWidth,
                             kunCompressedSliceHeight,
                             sliceCount);
        for(int i = 0; i < sliceCount; i++)
        {
          memcpy(dicos_volume[i].GetBuffer()
                 , &m_volume[i*kunCompressedSliceWidth*kunCompressedSliceHeight*sizeof(SDICOS::S_UINT16)]
              , (kunCompressedSliceWidth*kunCompressedSliceHeight*sizeof(SDICOS::S_UINT16)));
        }
        SDICOS::CTTypes::CTImage &image = m_volumectmodule->GetCTImage();
        image.SetPixelData(dicos_volume);
        m_volumectmodule->GetMultiframeFunctionalGroups().SetNumberOfFrames(sliceCount);
      }
      else
      {
        if (isCtModuleProj00(&m_ctmodule[i]))
        {
          fname += BAGFILE_DICOSCT_PROJ00_FORMAT;
        }
        else if (isCtModuleProj90(&m_ctmodule[i]))
        {
          fname += BAGFILE_DICOSCT_PROJ90_FORMAT;
        }
        else
        {
          ERROR_LOG("saveToFiles : Invalid CT module received ");
        }
      }



      SDICOS::ErrorLog errorLog;
      if(m_ctmodule[i].IsValid(errorLog))
      {
        fname += ".dcs";
        DEBUG_LOG("Writing file: "<< fname.c_str());
        status = m_ctmodule[i].Write(SDICOS::Filename(fname),
                                     SDICOS::DicosFile::enumLittleEndianExplicit,
                                     errorLog);
        if(!status)
        {
          ERROR_LOG("CTModule Write failed: " << errorLog.GetErrorLog().Get());
          status = false;
          result = tr("CTModule Write failed: ");
          result.append(errorLog.GetErrorLog().Get());
          break;
        }
      }
      else
      {
        ERROR_LOG("CTModule invalid: " << errorLog.GetErrorLog().Get());
        status = false;
        result = tr("CTModule invalid: ");
        result.append(errorLog.GetErrorLog().Get());
        break;
      }
    }
    INFO_LOG("Finished writing CTModule files...");
  }

  // Writing TDR Module
  int countTdrModules = m_tdrmodule.size();
  if(status)
  {
    if(countTdrModules < 1 || countTdrModules > 9999)
    {
      ERROR_LOG("Invalid number of TDRModules: " << countTdrModules);
      status = false;
      result = tr("Invalid number of TDRModules: ") + countTdrModules;
    }
  }
  if(status)
  {
    INFO_LOG("Start writing TDRModule files...");
    for(int i = 0; i < countTdrModules; i++)
    {
      std::string fname = folderPath;
      fname += "/";
      fname += fileName;
      fname += BAGFILE_DICOSTDR_FORMAT;
      fname += QString::number((i+1)).toStdString();
      fname += ".dcs";

      SDICOS::ErrorLog errorLog;
      if (m_tdrmodule[i] && m_tdrmodule[i]->IsValid(errorLog))
      {
        DEBUG_LOG("Writing file: "<< fname.c_str());
        bool status = m_tdrmodule[i]->Write(SDICOS::Filename(fname),
                                            SDICOS::DicosFile::enumLittleEndianExplicit,
                                            errorLog);
        if(!status)
        {
          ERROR_LOG("TDRModule Write failed: " << errorLog.GetErrorLog().Get());
          status = false;
          result = tr("TDRModule Write failed: ");
          result.append( errorLog.GetErrorLog().Get());
          break;
        }
      }
      else
      {
        ERROR_LOG("TDRModule invalid: " << errorLog.GetErrorLog().Get());
        status = false;
        result = tr("TDRModule invalid: ");
        result.append( errorLog.GetErrorLog().Get());
        break;
      }
    }
    INFO_LOG("Finished writing TDRModule files...");
  }

  // Write all bag-related info in a text file and put it in same folder.
  if(status)
  {
    std::string fname = folderPath;
    fname += "/";
    fname += fileName;
    fname += BAGFILE_PROPERTY_FORMAT;

    m_bagInfo.nss_bag_id = m_bagid.c_str();

    if (m_volumectmodule != NULL)
    {
      m_bagInfo.displayable_bag_id = m_volumectmodule->GetObjectOfInspection().
          GetID().GetID().Get();
    }
    m_bagInfo.m_volWidth = QString::number(m_volumedimension.x());
    m_bagInfo.m_volHeight = QString::number(m_volumedimension.y());
    m_bagInfo.m_sliceCount = QString::number(m_volumedimension.z());
    m_bagInfo.m_sliceFormat = m_volumeformat;

    m_bagInfo.m_roiStart = m_roiStart;
    m_bagInfo.m_roiEnd = m_roiEnd;

    status = m_bagInfo.writeToFile(fname.c_str());
    if(status)
      m_localArchivePath = QString(fname.c_str());
    else
    {
      ERROR_LOG("Failed to create " << BAGFILE_PROPERTY_FORMAT << " file.");
    }
  }

  // Cleanup incomplete data on failure.
  if(!status)
  {
    QDir dir(folderPath.c_str());
    QString filenamePrefixStr = QString::fromStdString(fileName);
    dir.remove(filenamePrefixStr+volFileFormat);
    dir.remove(filenamePrefixStr+BAGFILE_PROPERTY_FORMAT);
    dir.remove(filenamePrefixStr+BAGFILE_DICOSCT_FORMAT);
    int i = 1;
    bool tdrFilePresent = true;

    do {
      QString tdrFileName;
      tdrFileName = filenamePrefixStr;
      tdrFileName += BAGFILE_DICOSTDR_FORMAT;
      tdrFileName += QString::number(i);
      tdrFilePresent = dir.remove(tdrFileName);
      i++;
    }while (tdrFilePresent);
  }
  return result;
}

/*!
* @fn       saveUncroppedToFiles
* @param    const std::string& folderPath
* @param    const std::string& filenamePrefix
* @return   Success or failure
* @param    Save uncropped bag data to files.
*/
QString BagData::saveUncroppedToFiles(const std::string& folderPath, const std::string&
                                      filenamePrefix)
{
  INFO_LOG("Save to file function is called.");
  TRACE_LOG("");
  std::string fileName = filenamePrefix;
  INFO_LOG("Saving bag locally. Folder: " << folderPath.c_str() << ", FileNamePrefix: " << fileName.c_str());
  bool status = true;
  QString result = "";
  QString volFileFormat;
  UsbStorageSrv m_usb;
  QString path = "";
  std::string mountpath = m_usb.getUsbfilePath();

  QString mouthpathStr = QString::fromStdString(mountpath);
  QString folderpathStr = QString::fromStdString(folderPath);
  if(mountpath.empty())
  {
    path = WorkstationConfig::getInstance()->getDefaultDataPath();
  }
  else
  {
    path = WorkstationConfig::getInstance()->getUsbMountPath();
  }

  qint64 usbSize = m_usb.sizeofdrive(path);
  if(usbSize == 0)
  {
    if(folderpathStr.contains(mouthpathStr))
      result = tr("File copy failed as Drive is full");
    return result;
  }

  if(getvolumeFormat() == VOLUME_FORMAT_JPEG)
  {
    volFileFormat = BAGFILE_JPEG_VOLUME_FORMAT;
  }
  else if(getvolumeFormat() == VOLUME_FORMAT_UNCOMPRESSED)
  {
    volFileFormat = BAGFILE_VOLUME_FORMAT;
  }
  else
  {
    status = false;
    result = tr("The file formate is not proper");
  }
  if(status)
  {
    if(isTIPBag())
    {
      if(UILayoutManager::getUILayoutManagerInstance()->getScreenState() ==
         QMLEnums::ScreenEnum::BAGSCREENING_SCREEN)
      {
        INFO_LOG("This is the tip bag, not going to save locally");
        return result;
      }
      else
      {
        fileName = TIP_BAG_PREFIX + fileName;
      }
    }
  }

  if(status)
  {
    DEBUG_LOG("Folderpath: " << folderPath.c_str() << " , fileName: " << fileName);
    if(!QDir(folderPath.c_str()).exists())
    {
      ERROR_LOG("Folder does not exists : " << folderPath);
      status = false;
      result = tr("Folder does not exists");
    }
  }

  // Write CTModule files
  int countCtModules = m_ctmodule.GetSize();
  if(status)
  {
    if(countCtModules < 1 || countCtModules > 9999)
    {
      ERROR_LOG("Invalid number of CTModules: " << countCtModules);
      status = false;
      result = tr("Invalid number of CTModules: ") + countCtModules;
    }
  }

  if(status && DEFAULT_VALUE_SAVE_VOLUME_FILE)
  {
    std::string fname = folderPath;
    fname += "/";
    fname += fileName;
    fname += volFileFormat.toStdString();

    FILE *fp = std::fopen(fname.c_str(), "wb");
    if(fp)
    {
      INFO_LOG("Writing Volume file: "<< fname.c_str());
      size_t ret = std::fwrite(m_uncroppedvolume, 1, m_uncropped_data_size, fp);
      if((ret == 0) || (ret < m_uncropped_data_size))
      {
        ERROR_LOG("Failed to create volume file: "<< fname.c_str());
        status = false;
        result = tr("Failed to create volume file: ");
        result.append(fname.c_str());
        result.append(tr("<br />Please check disk space"));
        status = false;
      }
      fclose(fp);
    }
    else
    {
      ERROR_LOG("Failed to create volume file: "<< fname.c_str());
      status = false;
      result = tr("Failed to create volume file: ");
      result.append(fname.c_str());
    }
  }

  if(status)
  {
    INFO_LOG("Start writing CTModule files...");
    // For Now, only one ctModule is present
    for(int i = 0; i < countCtModules; i++)
    {
      std::string fname = folderPath;
      fname += "/";
      fname += fileName;


      SDICOS::CTTypes::CTImage::CT_PIXEL_DATA_CHARACTERISTICS pixel;
      SDICOS::CTTypes::CTImage::OOI_IMAGE_CHARACTERISTICS ooi;
      SDICOS::CTTypes::CTImage::IMAGE_FLAVOR flavor;
      SDICOS::CTTypes::CTImage::DERIVED_PIXEL_CONTRAST contrast;
      m_ctmodule[i].GetCTImage().GetImageTypeCT(pixel, ooi, flavor, contrast);
      if(flavor == SDICOS::CTTypes::CTImage::enumVolume)
      {
        fname += BAGFILE_DICOSCT_FORMAT;

        /// Add Slice Data Inside CtModule
        SDICOS::Array3DLarge<SDICOS::S_UINT16> dicos_volume;
        SDICOS::S_UINT16 sliceCount = static_cast<SDICOS::S_UINT16>(m_uncropped_max_slices); //to be checked
        SDICOS::S_UINT16 kunCompressedSliceWidth = static_cast<SDICOS::S_UINT16>(m_uncroppedVolColumns);
        SDICOS::S_UINT16 kunCompressedSliceHeight = static_cast<SDICOS::S_UINT16>(m_uncroppedVolRows);
        /// Set the size of volume image
        dicos_volume.SetSize(kunCompressedSliceWidth,
                             kunCompressedSliceHeight,
                             sliceCount);
        for(int i = 0; i < sliceCount; i++)
        {
          memcpy(dicos_volume[i].GetBuffer()
                 , &m_uncroppedvolume[i*kunCompressedSliceWidth*kunCompressedSliceHeight*sizeof(SDICOS::S_UINT16)]
              , (kunCompressedSliceWidth*kunCompressedSliceHeight*sizeof(SDICOS::S_UINT16)));
        }
        SDICOS::CTTypes::CTImage &image = m_volumectmodule->GetCTImage();
        image.SetPixelData(dicos_volume);
        m_volumectmodule->GetMultiframeFunctionalGroups().SetNumberOfFrames(sliceCount);
      }
      else
      {
        if (isCtModuleProj00(&m_ctmodule[i]))
        {
          fname += BAGFILE_DICOSCT_PROJ00_FORMAT;
        }
        else if (isCtModuleProj90(&m_ctmodule[i]))
        {
          fname += BAGFILE_DICOSCT_PROJ90_FORMAT;
        }
        else
        {
          ERROR_LOG("saveToFiles : Invalid CT module received ");
        }
      }



      SDICOS::ErrorLog errorLog;
      if(m_ctmodule[i].IsValid(errorLog))
      {
        fname += ".dcs";
        DEBUG_LOG("Writing file: "<< fname.c_str());
        status = m_ctmodule[i].Write(SDICOS::Filename(fname),
                                     SDICOS::DicosFile::enumLittleEndianExplicit,
                                     errorLog);
        if(!status)
        {
          ERROR_LOG("CTModule Write failed: " << errorLog.GetErrorLog().Get());
          status = false;
          result = tr("CTModule Write failed: ");
          result.append(errorLog.GetErrorLog().Get());
          break;
        }
      }
      else
      {
        ERROR_LOG("CTModule invalid: " << errorLog.GetErrorLog().Get());
        status = false;
        result = tr("CTModule invalid: ");
        result.append(errorLog.GetErrorLog().Get());
        break;
      }
    }
    INFO_LOG("Finished writing CTModule files...");
  }

  // Writing TDR Module
  int countTdrModules = m_tdrmodule.size();
  if(status)
  {
    if(countTdrModules < 1 || countTdrModules > 9999)
    {
      ERROR_LOG("Invalid number of TDRModules: " << countTdrModules);
      status = false;
      result = tr("Invalid number of TDRModules: ") + countTdrModules;
    }
  }
  if(status)
  {
    INFO_LOG("Start writing TDRModule files...");
    for(int i = 0; i < countTdrModules; i++)
    {
      std::string fname = folderPath;
      fname += "/";
      fname += fileName;
      fname += BAGFILE_DICOSTDR_FORMAT;
      fname += QString::number((i+1)).toStdString();
      fname += ".dcs";

      SDICOS::ErrorLog errorLog;
      if (m_tdrmodule[i] && m_tdrmodule[i]->IsValid(errorLog))
      {
        INFO_LOG("TDR_UNCROPPED : Writing file: "<< fname.c_str());
        bool status = m_tdrmodule[i]->Write(SDICOS::Filename(fname),
                                            SDICOS::DicosFile::enumLittleEndianExplicit,
                                            errorLog);
        if(!status)
        {
          ERROR_LOG("TDRModule Write failed: " << errorLog.GetErrorLog().Get());
          status = false;
          result = tr("TDRModule Write failed: ");
          result.append( errorLog.GetErrorLog().Get());
          break;
        }
      }
      else
      {
        ERROR_LOG("TDRModule invalid: " << errorLog.GetErrorLog().Get());
        status = false;
        result = tr("TDRModule invalid: ");
        result.append( errorLog.GetErrorLog().Get());
        break;
      }
    }
    INFO_LOG("Finished writing TDRModule files...");
  }

  // Write all bag-related info in a text file and put it in same folder.
  if(status)
  {
    std::string fname = folderPath;
    fname += "/";
    fname += fileName;
    fname += BAGFILE_PROPERTY_FORMAT;

    m_bagInfo.nss_bag_id = m_bagid.c_str();

    if (m_volumectmodule != NULL)
    {
      m_bagInfo.displayable_bag_id = m_volumectmodule->GetObjectOfInspection().
          GetID().GetID().Get();
    }
    m_bagInfo.m_volWidth = QString::number(m_uncroppedVolDimension.x());
    m_bagInfo.m_volHeight = QString::number(m_uncroppedVolDimension.y());
    m_bagInfo.m_sliceCount = QString::number(m_uncroppedVolDimension.z());
    m_bagInfo.m_sliceFormat = m_volumeformat;

    m_bagInfo.m_roiStart = m_roiStart;
    m_bagInfo.m_roiEnd = m_roiEnd;

    status = m_bagInfo.writeToFile(fname.c_str());
    if(status)
      m_localArchivePath = QString(fname.c_str());
    else
    {
      ERROR_LOG("Failed to create " << BAGFILE_PROPERTY_FORMAT << " file.");
    }
  }

  // Cleanup incomplete data on failure.
  if(!status)
  {
    QDir dir(folderPath.c_str());
    QString filenamePrefixStr = QString::fromStdString(fileName);
    dir.remove(filenamePrefixStr+volFileFormat);
    dir.remove(filenamePrefixStr+BAGFILE_PROPERTY_FORMAT);
    dir.remove(filenamePrefixStr+BAGFILE_DICOSCT_FORMAT);
    int i = 1;
    bool tdrFilePresent = true;

    do {
      QString tdrFileName;
      tdrFileName = filenamePrefixStr;
      tdrFileName += BAGFILE_DICOSTDR_FORMAT;
      tdrFileName += QString::number(i);
      tdrFilePresent = dir.remove(tdrFileName);
      i++;
    }while (tdrFilePresent);
  }
  return result;
}

/*!
* @fn       applyDecision
* @param    decision - suspect, clear, timeout
* @return   bool - success
* @brief    Send bag decision to NSS
*/
bool BagData::applyDecision(QMLEnums::ThreatEvalPanelCommand decision)
{
  TRACE_LOG("");
  INFO_LOG("Apply decision. Decision: " << decision);
  bool status = true;
#ifdef WORKSTATION

  if (m_livebag != NULL)
  {
    QMLEnums::ScreenEnum screenState = UILayoutManager::getUILayoutManagerInstance()->getScreenState();

    // SDICOS::TDRModule tdr;
    analogic::nss::BasicCommandResult nss_result;
    bool res = false;
    res = populateCommonTdrParts(m_operatorTdr, decision, getVolumeCtModule());
    if (!res)
    {
      ERROR_LOG("Population of common TDR Parts failed.");
      return res;
    }

    SDICOS::ErrorLog errorLog;
    if(m_operatorTdr->IsValid(errorLog))
    {
      if (((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::OSR_WORKSTATION) &&
           ((m_bagReqLoc == QMLEnums::PRIMARY_SERVER)))
          || ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::SEARCH_WORKSTATION) &&
              ((m_bagReqLoc == QMLEnums::SEARCH_SERVER) || (m_bagReqLoc == QMLEnums::BHS_SERVER))))
      {
        if (screenState == QMLEnums::SEARCH_BAG_SCREEN)
        {
          if((decision == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG)
             || (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG))
          {
            populateDecisionOnAllthreat(m_operatorTdr, decision);
            nss_result = m_livebag->AddTDRModule(*m_operatorTdr);
          }
        }
        else
        {
          nss_result = m_livebag->AddTDRModule(*m_operatorTdr);
          if (nss_result != analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_SUCCESS) {
            DEBUG_LOG("Processor::Start: Error notifying the NSS of the TDR: " <<
                      analogic::nss::basic_command_result_to_string(nss_result));
          }
          unsigned int diff = m_bagDecisionTime.toMSecsSinceEpoch() - m_bagRenderedTime.toMSecsSinceEpoch();
          ((analogic::nss::OsrLiveBag*)m_livebag)->SetDecisionTime(diff);
          DEBUG_LOG("Time taken to take decision: " << diff << "ms");
        }
      }
    }
    else
    {
      ERROR_LOG("TDR Generation failed." << errorLog.GetErrorLog().Get());
    }
    // If this is a search bag and the user did not retain, then we should remove the bag
    // from the search list
    if ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::SEARCH_WORKSTATION) &&
        ((m_bagReqLoc == QMLEnums::SEARCH_SERVER)||
         (m_bagReqLoc == QMLEnums::BHS_SERVER)))
    {
      WorkstationSearchBag* derived = reinterpret_cast<WorkstationSearchBag*>(m_livebag);
      if (derived) {
        if ((decision == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG)
            || (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG))
        {
          DEBUG_LOG("search decision is Clear, removing from search list");
          derived->RemoveFromList();
        } else {
          DEBUG_LOG("search decision is Retain; leaving in list");
        }
      }
    }

    if(WorkstationConfig::getInstance()->isBagPickupEnabled()
       && (WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::OSR_WORKSTATION)
       && ( screenState == QMLEnums::ScreenEnum::BAGSCREENING_SCREEN)
       && ((decision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG) ||
           (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_TIMEOUT_BAG)))
    {
      ((analogic::nss::OsrLiveBag*) m_livebag)->NotifyOfPickup();
    }

    emit bagFinished(m_livebag);

    // TODO(Agiliad): If clearing fails due to network connectivity, don't
    // dispose the bag, and don't clear the widgets. Keep user on same
    // screen. Show a network error dialog.
    status = true;
  }
  else
  {
    ERROR_LOG("No live bag. Can't send decision.");
    status = false;
  }

  // m_bagInfo.m_userdecision = decision;
  if (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG)
  {
    m_bagInfo.user_alarm_decision = SDICOS::TDRTypes::ThreatDetectionReport::enumClear;
    if (m_bagReqLoc == QMLEnums::BHS_SERVER)
    {
      ERROR_LOG("Sending BHS res");
      emit sigSendBagRspToBHS(getBHSBagId(), analogic::ancp00::BagStatusUpdate::
                              BAG_DISPOSITION::BagStatusUpdate_BAG_DISPOSITION_CLEAR);
    }
  }
  else if (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG)
  {
    m_bagInfo.user_alarm_decision = SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm;
  }
  else if (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_TIMEOUT_BAG)
  {
    m_bagInfo.user_alarm_decision = SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision;
  }
#endif
  setState(BagData::DISPOSED);
  return status;
}

/*!
* @fn       identifyDisplayableThreat
* @return   std::vector< std::pair<int, std::string> >
* @brief    gets displayable threat information
*/
std::vector<std::pair<int, string> > BagData::identifyDisplayableThreat()
{
  TRACE_LOG("");
  std::vector<std::pair<int, string> > res;
  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  for (unsigned int i = 0; i < tdrArray.size(); i++)
  {
    SDICOS::TDRModule* tdrModule = tdrArray.at(i);
    if (tdrModule != NULL)
    {
      string sopSeqId = tdrModule->GetSopCommon().GetSopInstanceUID().Get();
      SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>&
          threat_item_array_src = tdrModule->GetThreatSequence().GetPotentialThreatObject();
      for(int j = 0; j < threat_item_array_src.GetSize(); j++)
      {
        SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object_src;
        threat_item_array_src[j].GetReferencedPotentialThreatObject(ref_object_src);
        if (ref_object_src.GetReferencedPotentialThreatObject().GetSize() > 0)
        {
          continue;
        }

        SDICOS::TDRTypes::AssessmentSequence& seq = threat_item_array_src[j].GetAssessment();
        SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem>& items = seq.GetAssessment();
        if (items.GetSize() > 0)
        {
          if (items[0].GetAssessmentFlag() ==
              SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat)
          {
            if (getUserDecisionfromPrimaryTdr(j) ==
                SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
            {
              if((currentscreen == QMLEnums::REMOTE_ARCHIVE_BAG_SCREEN) ||
                 (currentscreen == QMLEnums::LOCAL_ARCHIVE_BAG_SCREEN))
              {
                if(m_showAllThreat)
                {
                  res.push_back(std::pair<int, string>{j, sopSeqId});
                }
              }
              else if ((currentscreen != QMLEnums::SEARCH_BAG_SCREEN)
                       || ((currentscreen == QMLEnums::SEARCH_BAG_SCREEN)
                           && WorkstationConfig::getInstance()->isDisplayOperatorClearedAlarmEnabled()) )
              {
                res.push_back(std::pair<int, string>{j, sopSeqId});
              }
            }
            else
            {
              res.push_back(std::pair<int, string>{j, sopSeqId});
            }
          }
        }
      }
    }
  }
  return res;
}

/*!
* @fn       validateOperatorDecisionForTIP
* @param    decision - suspect, clear, timeout
* @return   bool - success
* @brief    validate operator decision for TIP
*/
QString BagData::validateOperatorDecisionForTIP(QMLEnums::ThreatEvalPanelCommand userDecision,
                                                QMLEnums::TIPResult& tipResult)
{
  TRACE_LOG("");
  QString message;
  if ( (userDecision != QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG)
       && (userDecision != QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG)
       && (userDecision != QMLEnums::ThreatEvalPanelCommand::TEPC_TIMEOUT_BAG)
       )
  {
    return "";
  }

  SDICOS::TDRModule* tdrModule = NULL;
  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
  for(unsigned int i = 0; i < tdrArray.size(); i++)
  {
    tdrModule = tdrArray.at(i);
    if (tdrModule != NULL)
    {
      SDICOS::TDRTypes::ThreatDetectionReport tdrreport =
          tdrModule->GetThreatDetectionReport();
      SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdrType =
          tdrreport.GetTdrType();
      if (tdrType == SDICOS::TDRTypes::ThreatDetectionReport::enumTruth)
      {
        SDICOS::TDRTypes::ThreatDetectionReport::ALARM_DECISION tdrDecision = tdrreport.GetAlarmDecision();
        if(tdrDecision == SDICOS::TDRTypes::ThreatDetectionReport::enumClear)
        {
          if (userDecision != QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG)
          {
            message = WorkstationConfig::getInstance()->getTipNONTIPMsgDialog();
            tipResult = QMLEnums::TIP_NONE;
          }
        }
        else  if ((tdrDecision == SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm) ||
                  (tdrDecision == SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision))
        {
          if ( (userDecision == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG)
               || (userDecision == QMLEnums::ThreatEvalPanelCommand::TEPC_TIMEOUT_BAG))
          {
            message = WorkstationConfig::getInstance()->getTipMISSMsgDialog();
            tipResult = QMLEnums::TIP_MISS;
          }
          else  if ((userDecision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG))
          {
            message = WorkstationConfig::getInstance()->getTipHITMsgDialog();
            tipResult = QMLEnums::TIP_HIT;
          }
          else
          {
          }
        }
        else
        {
        }

        // Identify TIP Threat type
        SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>&
            threat_item_array = tdrModule->GetThreatSequence().GetPotentialThreatObject();
        int numPTOs = threat_item_array.GetSize();
        QString categoryDesc;
        if (numPTOs > 0)
        {
          SDICOS::TDRTypes::AssessmentSequence& assessment_sequence = threat_item_array[0].GetAssessment();
          categoryDesc = assessment_sequence.GetAssessment()[0].GetThreatCategoryDescription().Get();
        }
        message.replace(QString("XXX"), categoryDesc);
        message.replace(QString("BTN"), QString("OK"));
      }
    }
  }
  return message;
}

/*!
* @fn       notifyReadyforPushTransfer
* @param    None
* @return   void
* @brief    Notifying NSS VM for ready for push transfer
*/
void BagData::notifyReadyforPushTransfer()
{
#ifdef WORKSTATION
  if ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::OSR_WORKSTATION) &&
      ((m_bagReqLoc == QMLEnums::PRIMARY_SERVER)))
  {
    if (m_livebag != NULL)
    {
      INFO_LOG("Notifying NSS VM for ready for push transfer");
      ((analogic::nss::OsrLiveBag*) m_livebag)->NotifyReadyForPushTransfer();
    }
  }
#endif
}


/*!
* @fn       getData_size
* @param    None
* @return   size_t
* @param    gets bag data size
*/
size_t BagData::getData_size() const
{
  DEBUG_LOG("Data size is: " << m_data_size);
  return m_data_size;
}

/*!
* @fn       setData_size
* @param    const size_t & - data_size
* @return   None
* @param    Sets bag data size.
*/
void BagData::setData_size(const size_t &data_size)
{
  DEBUG_LOG("Set data size to: " << data_size);
  m_data_size = data_size;
}



/*!
* @fn       setVolumeDimension
* @param    QVector3D dimension
* @return   void - set volume dimension
* @brief    set volume dimension
*/
void BagData::setVolumeDimension(QVector3D dimension)
{
  m_volumedimension = dimension;
}

/*!
* @fn       cropVolumeAccordingtoROI
* @param    None
* @return   None
* @param    crop volume according to ROI
*/
bool BagData::cropVolumeAccordingtoROI()
{
  TRACE_LOG("");
  INFO_LOG("Croping volume according to ROI()");
  size_t data_size = 0;
  if(m_uncroppedvolume != NULL)
  {
    INFO_LOG("Creating copy for bagdata before cropping");
    m_uncroppedVolColumns = m_volColumns;
    m_uncroppedVolRows = m_volRows;
    m_uncropped_max_slices = m_max_slices;
    m_uncroppedVolDimension = getVolumeDimension();
  }
  QVector3D roistart  =  getvolumeStartROI();
  QVector3D roiend    =  getvolumeEndROI();
  QVector3D dimension = getVolumeDimension();
  if((roistart.x() > roiend.x()) ||
     (roistart.y() > roiend.y()) ||
     (roistart.z() > roiend.z()) ||
     (roistart.x() < 0) ||
     (roistart.y() < 0) ||
     (roistart.z() < 0) ||
     (roiend.x() > dimension.x()) ||
     (roiend.y() > dimension.y()) ||
     (roiend.z() > dimension.z())
     )
  {
    // DEBUG_LOG("Starting to get CTModule data where module count is: " << countCtModules);
    ERROR_LOG("ROI received is not correct.");
    return false;
  }
  DEBUG_LOG("roi start x:" << roistart.x()
            << " y:" << roistart.y() << " z:" << roistart.z());
  DEBUG_LOG("roi end x:" << roiend.x()
            << " y:" << roiend.y() << " z:" << roiend.z());

  QVector3D tempdimension(roiend.x()-roistart.x()+1,
                          roiend.y()-roistart.y()+1,
                          roiend.z()-roistart.z()+1);
  for(int z = roistart.z(); z <= roiend.z(); z++)
  {
    unsigned char* slice_buffer = &m_uncroppedvolume[(size_t)(static_cast<int>(dimension.x())*
                                                     static_cast<int>(dimension.y())*z*2)];
    for(int y = roistart.y(); y <= roiend.y(); y++)
    {
      unsigned char* currentrow = &slice_buffer[(size_t)((static_cast<int>(dimension.x())*(y)+
                                                          static_cast<int>(roistart.x()))*2)];
      memcpy(&m_volume[data_size], currentrow, (size_t)(static_cast<int>(tempdimension.x())*2));
      data_size+= (size_t)tempdimension.x()*2;
    }
  }
  INFO_LOG("Copied crop data size: " << data_size
           << ", Original volume size: " << m_data_size);
  m_uncropped_data_size = m_data_size;
  int location = getBagReqLoc();
  if((data_size <= m_data_size) || (location == QMLEnums::LOCAL_SERVER)) //to be checked
    //incase of archived files m_data_size will be larger than data size as they are uncropped.
  {
    m_data_size = data_size;

    setVolumeDimension(tempdimension);
    m_volRows = tempdimension.y();
    m_volColumns = tempdimension.x();

    INFO_LOG("Creating copy of tdr_module before Cropping");
    for(unsigned int i = 0; i < m_tdrmodule.size(); i++)
    {
      SDICOS::TDRModule* tdrModule = m_tdrmodule.at(i);

      if(tdrModule != NULL)
      {
        m_uncropped_tdrmodule.push_back(tdrModule);
      }
    }

    for (unsigned int i = 0; i < m_tdrmodule.size(); i++)
    {
      SDICOS::TDRModule* tdrModule = m_tdrmodule.at(i);

      if(tdrModule != NULL)
      {
        SDICOS::TDRTypes::ThreatSequence ts = tdrModule->GetThreatSequence();
        SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatItemArray =
            ts.GetPotentialThreatObject();
        for(unsigned int i = 0; i < threatItemArray.GetSize(); i++)
        {
          SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object;
          threatItemArray[i].GetReferencedPotentialThreatObject(ref_object);
          if (ref_object.GetReferencedPotentialThreatObject().GetSize() == 0)
          {
            // bdavidson: modified for DICOS v2.0a
            // Careful: I did not check to make sure the PTO array actually contains
            // an item.
            SDICOS::TDRTypes::CommonSpecificTDR::ThreatROI roi =
                threatItemArray[i].GetBaggageTDR().GetPTORepresentationSequenceItems()[0].GetThreatROI();
            float roiBasex, roiBasey, roiBasez;
            roi.GetThreatRoiBase(roiBasex, roiBasey, roiBasez);
            roi.SetThreatRoiBase(roiBasex-roistart.x(),
                                 roiBasey-roistart.y(),
                                 roiBasez-roistart.z());
            roi.GetThreatRoiBase(roiBasex, roiBasey, roiBasez);
            DEBUG_LOG("TDR ROI After Crop. roiBaseX: " << roiBasex << " roiBaseY: "
                      << roiBasey << " roiBaseZ: " << roiBasez);
          }
        }
      }
      else
      {
        ERROR_LOG("TDR Module is null.");
      }
    }
    DEBUG_LOG("roi after crop start x:" << m_roiStart.x()
              << " y:" << m_roiStart.y() << " z:" << m_roiStart.z());
    DEBUG_LOG("roi end x:"<< m_roiEnd.x()
              << " y:"<< m_roiEnd.y() << " z:" << m_roiEnd.z());
  }
  else
  {
    ERROR_LOG("Calculated size after cropping is more than actual volume.");
    return false;
  }
  return true;
}

/*!
* @fn       setvolumeROIStartX
* @param    float ROI startx
* @return   None
* @brief    sets volume ROI
*/
void BagData::setvolumeROIStartX(float startx)
{
  m_roiStart.setX(startx);
}

/*!
* @fn       setvolumeROIStartY
* @param    float ROI starty
* @return   None
* @brief    sets volume ROI
*/
void BagData::setvolumeROIStartY(float starty)
{
  m_roiStart.setY(starty);
}
/*!
* @fn       setvolumeROIStartZ
* @param    float ROI startz
* @return   None
* @brief    sets volume ROI
*/
void BagData::setvolumeROIStartZ(float startz)
{
  m_roiStart.setZ(startz);
}

/*!
* @fn       setvolumeROIEndX
* @param    float ROI endx
* @return   None
* @brief    sets volume ROI
*/
void BagData::setvolumeROIEndX(float endx)
{
  m_roiEnd.setX(endx);
}

/*!
* @fn       setvolumeROIEndY
* @param    float ROI endy
* @return   None
* @brief    sets volume ROI
*/
void BagData::setvolumeROIEndY(float endy)
{
  m_roiEnd.setY(endy);
}

/*!
* @fn       setvolumeROIEndZ
* @param    float ROI endz
* @return   None
* @brief    sets volume ROI EndZ
*/
void BagData::setvolumeROIEndZ(float endz)
{
  m_roiEnd.setZ(endz);
}

/*!
* @fn       getvolumeStartROI
* @param    None
* @return   QVector3D - start ROI
* @brief    gets volume ROI
*/
QVector3D BagData::getvolumeStartROI()
{
  return m_roiStart;
}

/*!
* @fn       getvolumeEndROI
* @param    None
* @return   QVector3D - end ROI
* @brief    gets volume End ROI
*/
QVector3D BagData::getvolumeEndROI()
{
  return m_roiEnd;
}

/*!
* @fn       getLocalArchivedPath
* @param    None
* @return   QString
* @brief    gets Local Archived Path
*/
QString BagData::getLocalArchivedPath()
{
  return m_localArchivePath;
}

/*!
* @fn       getBagFileName
* @param    None
* @return   string - bag file name pattern
*           MachineID_YYYYMMDD_hhmmss_BagID
* @brief    get bag file name pattern
*/
std::string BagData::getBagFileName() const
{
  if (m_volumectmodule == NULL)
  {
    return "";
  }
  std::string bagFilePathName = m_volumectmodule->GetGeneralEquipment().
      GetMachineID().Get();
  bagFilePathName += "_";

  std::string dicosDate = m_volumectmodule->GetSeries().GetDate().Get();
  std::string formatDate = dicosDate.substr(0, 4) + dicosDate.
      substr(4, 2) + dicosDate.substr(6, 2);
  bagFilePathName += formatDate;
  bagFilePathName += "_";

  std::string dicosTime = m_volumectmodule->GetSeries().GetTime().Get();
  std::string formatedTime = dicosTime.substr(0, 2) + dicosTime.substr(2, 2)
      + dicosTime.substr(4, 2);
  bagFilePathName += formatedTime;
  bagFilePathName += "_";

  bagFilePathName += m_volumectmodule->GetObjectOfInspection().
      GetID().GetID().Get();
  return bagFilePathName;
}

/*!
* @fn       setvolumeWidth
* @param    float width
* @return   None
* @brief    sets volume width
*/
void BagData::setvolumeWidth(float val)
{
  m_volumedimension.setX(val);
}

/*!
* @fn       setvolumeHeight
* @param    float height
* @return   None
* @brief    sets volume height
*/
void BagData::setvolumeHeight(float val)
{
  m_volumedimension.setY(val);
}

/*!
* @fn       getVolumeDimension
* @param    None
* @return   QVector3D - dimension
* @brief    gets volume dimensions
*/
QVector3D BagData::getVolumeDimension()
{
  return m_volumedimension;
}

/*!
* @fn       setvolumeSliceCount
* @param    float slice
* @return   None
* @brief    sets volume slice count
*/
void BagData::setvolumeSliceCount(float val)
{
  m_volumedimension.setZ(val);
}

/*!
* @fn       getvolumeFormat
* @param    None
* @return   QString - format
* @brief    gets volume slice format
*/
QString BagData::getvolumeFormat()
{
  return m_volumeformat;
}


/*!
* @fn       setvolumeFormat
* @param    QString - format
* @return   None
* @brief    sets volume slice format
*/
void BagData::setvolumeFormat(QString val)
{
  m_volumeformat = val;
}

/*!
* @fn       isBagError
* @param    None
* @return   bool
* @brief    Tells if bag has error.
*/
bool BagData::isBagError()
{
  return m_isBagError;
}
/*!
* @fn       setError
* @param    bool value
* @return   void
* @brief    sets bag as error prone
*/
void BagData::setError(bool value)
{
  m_isBagError = value;
}

/*!
* @fn       ~BagData
* @param    None
* @return   None
* @brief    Destructor for class BagData.
*/
BagData::~BagData()
{
  clearErrorMsg();
  TRACE_LOG("");
  if(m_volume != NULL)
  {
    free(m_volume);
    DEBUG_LOG("BagData Dtor **YYF1YY** free bag data here!");
    m_volume = NULL;
  }
  if(m_uncroppedvolume != NULL)
  {
    free(m_uncroppedvolume);
    DEBUG_LOG("BagData Dtor **YYF2YY** free temp volume here!");
    m_uncroppedvolume = NULL;
  }
  for (unsigned int i = 0; i < m_tdrmodule.size(); i++)
  {
    SDICOS::TDRModule* tdr = m_tdrmodule.at(i);
    QString delMsg = "**YYTDRYY** Delete TDR:" + QString::number((uint64_t)tdr);
    DEBUG_LOG("BagData Dtor " << delMsg.toLocal8Bit().data());
    delete  tdr;
  }
  for (unsigned int i = 0; i < m_uncropped_tdrmodule.size(); i++)
  {
    SDICOS::TDRModule* tdr = m_uncropped_tdrmodule.at(i);
    QString delMsg = "**YYTDRYY** Delete TDR:" + QString::number((uint64_t)tdr);
    DEBUG_LOG("BagData Dtor " << delMsg.toLocal8Bit().data());
    delete  tdr;
  }
  SAFE_DELETE(m_sliceQueue);
  m_tdrmodule.clear();
  m_uncropped_tdrmodule.clear();
  SAFE_DELETE(m_slicequeueMutex);
}

/*!
* @fn       getSliceCountReceived
* @param    None
* @return   int
* @brief    get slice count received.
*/
int BagData::getSliceCountReceived()
{
  return m_sliceCountReceived;
}

// TODO(Agiliad) Revisit when separating rerun and training #ifdef WORKSTATION
/*!
* @fn       PopulateCommonTdrParts
* @param    SDICOS::TDRModule* tdr
* @param    QMLEnums::ThreatEvalPanelCommand decision
* @param    SDICOS::CTModule* ctmodule
* @return   bool
* @brief    This function applies TDR to bag
*/
bool BagData::populateCommonTdrParts(SDICOS::TDRModule* tdr,
                                     QMLEnums::ThreatEvalPanelCommand decision,
                                     SDICOS::CTModule* ctmodule)
{
  TRACE_LOG("");
  // Validate CT Module
  if(ctmodule == NULL)
  {
    return false;
  }
  if(tdr == nullptr)
  {
    tdr = addOperatorTDRModule();
  }

  SDICOS::DcsDate date;
  SDICOS::DcsTime time;

  date.SetToday();
  time.SetNow();

  // Set TDR Series
  SDICOS::TDRTypes::TDRSeries& series = tdr->GetSeries();
  series.SetAcquisitionStatus(SDICOS::GeneralSeriesModule::enumSuccessful);
  series.SetDateAndTime(date, time);
  series.SetInstanceUID(SDICOS::DcsUniqueIdentifier::CreateGUID());

  // Set the FrameOfReference
  SDICOS::FrameOfReference& frame = tdr->GetFrameOfReference();
  frame.SetFrameOfReferenceUID(ctmodule->GetFrameOfReference().GetFrameOfReferenceUID());

  // Set the ObjectOfInspection
  SDICOS::ObjectOfInspectionModule& ooi = tdr->GetObjectOfInspection();
  ooi = ctmodule->GetObjectOfInspection();

  // Set the GeneralScanModule
  SDICOS::GeneralScanModule& general_scan = tdr->GetGeneralScan();
  general_scan = ctmodule->GetGeneralScan();

  // Set the GeneralEquipmentModule
  SDICOS::GeneralEquipmentModule& gem = tdr->GetGeneralEquipment();
  SDICOS::DcsDate hack_cal_date("20180101");
  SDICOS::DcsTime hack_cal_time("000000");
  gem.SetCalibrationDateAndTime(hack_cal_date, hack_cal_time);
  // gem.SetCalibrationDateAndTime(date, time);
  gem.SetDeviceSerialNumber(WorkstationConfig::getInstance()->getWorkstationSerialNumber().toStdString().c_str());
  gem.SetMachineAddress(WorkstationConfig::getInstance()->getMachineAddress().toStdString().c_str());
  gem.SetMachineID(WorkstationConfig::getInstance()->getWorkstationMachineId().toStdString().c_str());
  gem.SetMachineLocation(WorkstationConfig::getInstance()->getMachineLocation().toStdString().c_str());
  gem.SetMachineSubLocation(WorkstationConfig::getInstance()->getMachineAddress().toStdString().c_str());
  gem.SetManufacturer(WorkstationConfig::getInstance()->getWorkstationManufactId().toStdString().c_str());
  gem.SetManufacturerModelName(WorkstationConfig::getInstance()->getManufacturerModelNumber().toStdString().c_str());
  gem.SetSoftwareVersion(WorkstationConfig::getInstance()->getWsVersion().toStdString().c_str());

  // Set the SopCommonModule
  SDICOS::SopCommonModule& sop = tdr->GetSopCommon();
  sop.SetSopInstanceCreationDateAndTime(date, time);
  sop.SetSopInstanceUID(SDICOS::DcsUniqueIdentifier::CreateGUID());

  // Set the GeneralReport
  SDICOS::TDRTypes::GeneralReport& general_report = tdr->GetGeneralReport();
  general_report.SetContentDateAndTime(date, time);
  general_report.SetInstanceNumber(1);

  // Initialize the ThreatDetectionReport
  SDICOS::TDRTypes::ThreatDetectionReport& threat_report = tdr->GetThreatDetectionReport();
  threat_report.SetAlarmDecisionDateAndTime(date, time);
  threat_report.SetImageScaleRepresentation(1.0);
  threat_report.SetTdrType(SDICOS::TDRTypes::ThreatDetectionReport::enumOperator);
  threat_report.SetOperatorInformation(getOperatorName().toStdString().c_str(),
                                       WorkstationConfig::getInstance()->getOperatorCompany().toStdString().c_str(),
                                       getOperatorName().toStdString().c_str(),
                                       SDICOS::IdentificationEncodingType::enumTextEncoding);

  unsigned int diff = m_bagDecisionTime.toMSecsSinceEpoch() - m_bagRenderedTime.toMSecsSinceEpoch();
  threat_report.SetTotalProcessingTime(diff);

  refreshAttributes();

  // Calculate total threats and potential alarm
  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array =
      tdr->GetThreatSequence().GetPotentialThreatObject();

  unsigned int totalavailableThreat = threat_item_array.GetSize();
  unsigned int totalalarmthreat = 0;
  for(unsigned int j = 0; j < totalavailableThreat; j++)
  {
    SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem> assesmentitm
        = threat_item_array[j].GetAssessment().GetAssessment();

    if(assesmentitm.GetSize() == 0)
    {
      continue;
    }

    SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG threatdecision
        = assesmentitm[0].GetAssessmentFlag();

    if (threatdecision != SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumNoThreat)
    {
      totalalarmthreat++;
    }
  }

  threat_report.SetNumberOfTotalObjects(totalavailableThreat);

  if ((decision == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG) )
  {
    threat_report.SetNumberOfAlarmObjects(0);
    DEBUG_LOG("Applying Clear threat report");
    threat_report.SetAbortFlag(SDICOS::TDRTypes::ThreatDetectionReport::enumSuccess);
    threat_report.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumClear);
  }
  else if (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG)
  {
    // ARO- SPLIT "TEPC_SUSPECT_BAG" and "TEPC_RETAIN_BAG"  and handle them differently.
    // For susopect bag add "dummy" operator NON-THREAT that is NOT a LAPTOP.
    threat_report.SetNumberOfAlarmObjects(totalalarmthreat);
    DEBUG_LOG("Applying Suspect threat report");
    threat_report.SetAbortFlag(SDICOS::TDRTypes::ThreatDetectionReport::enumSuccess);
    threat_report.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm);
    if (totalalarmthreat == 0)
    {
      threat_report.SetNumberOfAlarmObjects(0);
      threat_report.SetNumberOfTotalObjects(0);
      threat_report.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm);
      //---------------------------------
      SDICOS::TDRTypes::ThreatSequence::ThreatItem pickbox_threat_item;
      pickbox_threat_item.EnableBaggageTDR(true);
      pickbox_threat_item.SetID(0);
      SDICOS::TDRTypes::AssessmentSequence& assessment_sequence = pickbox_threat_item.GetAssessment();
      assessment_sequence.GetAssessment().SetSize(1);
      assessment_sequence.GetAssessment()[0].SetThreatCategory(
            SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumExplosive);
      //------------------------------------------------------
      // ARO - This can be anything but "Laptop" or "LAPTOP"
      assessment_sequence.GetAssessment()[0].SetThreatCategoryDescription("BULK");
      //------------------------------------------------------
      assessment_sequence.GetAssessment()[0].SetAbilityAssessment(
            SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoInterference);

      assessment_sequence.GetAssessment()[0].SetAssessmentFlag(
            SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat);
      SDICOS::TDRTypes::BaggageSpecificTDR& baggage_tdr = pickbox_threat_item.GetBaggageTDR();

      SDICOS::Array1D<SDICOS::TDRTypes::BaggageSpecificTDR::PTORepresentationSequenceItem>& pto_array =
          baggage_tdr.GetPTORepresentationSequenceItems();
      pto_array.SetSize(1);
      SDICOS::TDRTypes::BaggageSpecificTDR::PTORepresentationSequenceItem& pto = pto_array[0];

      SDICOS::Array1D<SDICOS::SopInstanceReference> & sop_sequence = pto.GetReferencedInstance();
      sop_sequence.SetSize(1);
      sop_sequence[0].SetClassUID(SDICOS::SOPClassUID::GetCT());
      sop_sequence[0].SetInstanceUID(ctmodule->GetSopCommon().GetSopInstanceUID());

      // ARO - The size is irrelevant perhaps 1x1x1 might work.
      int xmin = 10;
      int ymin = 10;
      int zmin = 10;
      int size_x = 10;
      int size_y = 10;
      int size_z = 10;

      SDICOS::TDRTypes::CommonSpecificTDR::ThreatROI& threat_roi = pto.GetThreatROI();
      threat_roi.SetThreatRoiBase(xmin, ymin, zmin);
      threat_roi.SetThreatRoiExtents(size_x, size_y, size_z);
      threat_item_array.Add(pickbox_threat_item);
    }
  }
  else if ((decision == QMLEnums::ThreatEvalPanelCommand::TEPC_RETAIN_BAG))
  {
    threat_report.SetNumberOfAlarmObjects(totalalarmthreat);
    DEBUG_LOG("Applying Suspect threat report");
    threat_report.SetAbortFlag(SDICOS::TDRTypes::ThreatDetectionReport::enumSuccess);
    threat_report.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm);
  }
  else if ( (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_TIMEOUT_BAG) )
  {
    threat_report.SetNumberOfAlarmObjects(totalalarmthreat);
    DEBUG_LOG("Applying Timeout threat report");
    threat_report.SetAbortFlag(SDICOS::TDRTypes::ThreatDetectionReport::enumAbort);
    threat_report.SetAbortReason(SDICOS::TDRTypes::ThreatDetectionReport::enumTimeout);
    threat_report.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision);
  }
  else
  {
    threat_report.SetNumberOfAlarmObjects(totalalarmthreat);
    DEBUG_LOG("Unknown alarm setting Abort flag");
    threat_report.SetAbortFlag(SDICOS::TDRTypes::ThreatDetectionReport::enumAbort);
    threat_report.SetAbortReason(SDICOS::TDRTypes::ThreatDetectionReport::ABORT_REASON::enumIncompleteScan);
    threat_report.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision);
  }
  return true;
}
/*!
* @fn       populateTDRWithError
* @param    Nome
* @return   SDICOS::TDRModule
* @brief    This function populate TDR in case error while transfering bag.
*/
SDICOS::TDRModule BagData::populateTDRWithError(void) {
  SDICOS::TDRModule tdr;

  // Add return result
  populateCommonTdrParts(&tdr, QMLEnums::ThreatEvalPanelCommand::TEPC_ERROR, getVolumeCtModule());

  SDICOS::ErrorLog errorlog;
  if (tdr.IsValid(errorlog))
  {
    INFO_LOG("Error TDR is valid");
  }
  else
  {
    ERROR_LOG("Error TDR is invalid");
  }
  return tdr;
}
/*!
 * @fn       getObviousThreatFlag
 * @param    None
 * @return   bool
 * @brief    gets obvious threat flag
 */
bool BagData::getObviousThreatFlag() const
{
  return m_obviousThreat;
}
#ifdef WORKSTATION
/*!
 * @fn       setObviousThreatFlag
 * @param    const bool flag
 * @param    bool fromBagServer
 * @return   None
 * @brief    sets obvious threat flag
 */
void BagData::setObviousThreatFlag(bool obviousThreat, bool fromBagServer)
{
  TRACE_LOG("");
  if(m_obviousThreat == obviousThreat)
  {
    INFO_LOG("Bag is already set to obvious threat: " << obviousThreat);
  }
  else
  {
    m_obviousThreat = obviousThreat;
    if (!fromBagServer)
    {
      QMLEnums::WSType wsType = WorkstationConfig::getInstance()->
          getWorkstationNameEnum();
      if ((wsType == QMLEnums::SEARCH_WORKSTATION))
      {
        INFO_LOG("BHS LIB Sending Stop-the-Check to NSS. Decision: " << obviousThreat);
        if (m_livebag)
        {
          //=========================================================================
          //---------------------------------------------------
          // We used to do this!
          // m_livebag->SetObviousThreatFlag(m_obviousThreat);
          //---------------------------------------------------
          NSSAgentAccessInterface* nssAgent = NSSAgentAccessInterface::getInstance();
          if (m_volumectmodule != NULL)
          {
            analogic::ancp00::ExternalBhsBagInterface*  bhsLib = nssAgent->getBHSBagInterface();
            SDICOS::Array1D<SDICOS::ObjectOfInspectionModule::IdInfo>& alternateIDs =
                m_volumectmodule->GetObjectOfInspection().GetAlternateIDs();
            bool foundBHS = false;
            int count_alt = alternateIDs.GetSize();
            for (int i = 0; i < count_alt; i++)
            {
              std::string signID   = alternateIDs[i].GetID().Get();
              if (alternateIDs[i].GetIdType() == SDICOS::ObjectOfInspectionModule::IdInfo::enumRFID )
              {
                foundBHS = true;
                INFO_LOG("Found 'bhs'Alternate Signing Authorithy in DICOS CTModule ID: "
                         << signID);

                bool setResp =  bhsLib->SetObviousThreatFlag(signID, obviousThreat);
                INFO_LOG("response = " << setResp);
              }
            }
            if (!foundBHS)
            {
              INFO_LOG("No 'bhs' Alternate Signing Authority found in DICOS CTModule");
            }
          }
          else
          {
            INFO_LOG("No CTModule to get OBVIOUS THREAT flag!");
          }
          //=========================================================================
        }
        else
        {
          ERROR_LOG("live bag is null.");
        }
      }
      else if(wsType == QMLEnums::OSR_WORKSTATION)
      {
        INFO_LOG("Non BHS Sending Stop-the-Check to NSS. Decision: " << obviousThreat);
        if (m_livebag)
        {
          m_livebag->SetObviousThreatFlag(m_obviousThreat);
        }
      }
      else
      {
        INFO_LOG("Workstation type is not PVS or SVS");
      }
    }
  }
}

#endif

/*!
* @fn       getBagTypeInfo
* @param    None
* @return   QString
* @brief    gets bag type info
*/
QString BagData::getBagTypeInfo() const
{
  return m_BagTypeInfo;
}

/*!
 * @fn       setBagTypeInfo
 * @param    const QString &BagTypeInfo
 * @return   None
 * @brief    sets bag type info
 */
void BagData::setBagTypeInfo(const QString &BagTypeInfo)
{
  m_BagTypeInfo = BagTypeInfo;
}
/*!
* @fn       setSearchReason
* @param    const std::string &searchReaso
* @return   None
* @brief    set search reason
*/
void BagData::setSearchReason(const std::string &searchReason)
{
  m_searchReason = searchReason;
}
/*!
* @fn       getSearchReason
* @param    None
* @return   std::string - search reason
* @brief    get search reason
*/
std::string BagData::getSearchReason() const
{
  return m_searchReason;
}
// TODO(Agiliad) Revisit when separating rerun and training#endif
/*!
* @fn       setSliceCountReceived
* @param    int sliceCountReceived
* @return   None
* @brief    sets number slice received
*/
void BagData::setSliceCountReceived(int sliceCountReceived)
{
  m_sliceCountReceived = sliceCountReceived;
}

/*!
* @fn       getSliceProcessed
* @param    None
* @return   int
* @brief    gets number slice processed
*/
int BagData::getSliceProcessed() const
{
  return m_sliceProcessed;
}

/*!
* @fn       setSliceProcessed
* @param    int sliceProcessed
* @return   None
* @brief    sets number slice processed
*/
void BagData::setSliceProcessed(int sliceProcessed)
{
  m_sliceProcessed = sliceProcessed;
}
/*!
* @fn       increamentSliceProcessed
* @param    None
* @return   None
* @brief    increament number slice processed by 1
*/
void BagData::increamentSliceProcessed()
{
  ++m_sliceProcessed;
}

/*!
* @fn       setBagReqLoc
* @param    QMLEnums::ArchiveFilterSrchLoc
* @return   void
* @brief    sets bag req location.
*/
void BagData::setBagReqLoc(QMLEnums::ArchiveFilterSrchLoc loc)
{
  m_bagReqLoc = loc;
}

/*!
* @fn       getBagReqLoc
* @param    void
* @return   QMLEnums::ArchiveFilterSrchLoc
* @brief    gets bag req location.
*/
QMLEnums::ArchiveFilterSrchLoc  BagData::getBagReqLoc()
{
  return m_bagReqLoc;
}

/*!
* @fn       setBHSBagId
* @param    string - bhs bagid
* @return   void
* @brief    sets BHS bag id
*/
void BagData::setBHSBagId(std::string id)
{
  m_bhsBagId = id;
}

/*!
* @fn       getBHSBagId
* @param    void
* @return   string - bhs id
* @brief    gets bhs bag id.
*/
std::string  BagData::getBHSBagId()
{
  return m_bhsBagId;
}

/*!
* @fn       enqueueSlice
* @param    SliceDataStruct*
* @return   void
* @brief    enqueue slice to queue
*/
void  BagData::enqueueSlice(SliceDataStruct* slice)
{
  if (m_sliceQueue)
  {
    m_slicequeueMutex->lock();
    m_sliceQueue->enqueue(slice);
    m_slicequeueMutex->unlock();
  }
}

/*!
* @fn       dequeueSlice
* @param    void
* @return   SliceDataStruct*
* @brief     dequeue slice from queue
*/
SliceDataStruct* BagData::dequeueSlice()
{
  SliceDataStruct* slice = NULL;
  if (m_sliceQueue)
  {
    m_slicequeueMutex->lock();
    slice = m_sliceQueue->dequeue();
    m_slicequeueMutex->unlock();
  }
  return slice;
}

/*!
* @fn       isSliceQueueEmpty
* @param    void
* @return   bool
* @brief    return slice queue empty status.
*/
bool BagData::isSliceQueueEmpty()
{
  return m_sliceQueue->isEmpty();
}

/*!
* @fn       buffer
* @param    None
* @return   None
* @brief    get buffer pointer
*/
unsigned char *SliceDataStruct::buffer() const
{
  return m_buffer;
}

/*!
* @fn       setBuffer
* @param    void *buffer
* @return   None
* @brief    set buffer pointer
*/
void SliceDataStruct::setBuffer(unsigned char *buffer)
{
  m_buffer = buffer;
}

/*!
* @fn       length
* @param    None
* @return   size_t
* @brief    get length
*/
size_t SliceDataStruct::length() const
{
  return m_length;
}
/*!
* @fn       setLength
* @param    const size_t &length
* @return   None
* @brief    set length
*/
void SliceDataStruct::setLength(const size_t &length)
{
  m_length = length;
}

/*!
* @fn       SliceNo
* @param    None
* @return   slice number
* @brief    get slice number
*/
unsigned int SliceDataStruct::SliceNo() const
{
  return m_SliceNo;
}

/*!
* @fn       setSliceNo
* @param    unsigned int slice number
* @return   None
* @brief    set slice number
*/
void SliceDataStruct::setSliceNo(unsigned int SliceNo)
{
  m_SliceNo = SliceNo;
}

// --------------------------------------------------------------------------------------
// Read TDR information present in bag. Get supplementary TDR information from
// live bag and add it in bag data
// --------------------------------------------------------------------------------------
void BagData::readSupplementaryTDR()
{
  TRACE_LOG("");
  // Read supplementary TDR module information from live bag
  int num_supplement_tdr = 0;
  if (m_livebag)
  {
    num_supplement_tdr = m_livebag->get_supplemental_tdr_count();
    m_livebag->get_extended_property_count();
    for (int i = 0; i < num_supplement_tdr; ++i)
    {
      // Read supplementary TDR module from live bag and add it in BAG data
      SDICOS::TDRModule* tdr_module_s = m_livebag->get_supplemental_tdr(i);
      SDICOS::TDRTypes::ThreatDetectionReport& threat_report = tdr_module_s->GetThreatDetectionReport();
      SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();
      if (tdr_type == SDICOS::TDRTypes::ThreatDetectionReport::enumOperator)
      {
        m_primaryOperatorTdr = tdr_module_s;
        SDICOS::TDRTypes::ThreatDetectionReport&
            threat_report = m_primaryOperatorTdr->GetThreatDetectionReport();
        if (threat_report.GetAlarmDecision() == SDICOS::TDRTypes::ThreatDetectionReport::enumClear)
        {
          setUserDecision("CLEAR");
        }
        else if (threat_report.GetAlarmDecision() == SDICOS::TDRTypes::ThreatDetectionReport::enumAlarm)
        {
          setUserDecision("ALARM");
        }
        else if (threat_report.GetAlarmDecision() ==
                 SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision)
        {
          setUserDecision("NO DECISION");
        }
      }
      addTdrModule(tdr_module_s);
    }
  }
}

/*!
* @fn       canTakeSuspectDecisionOnBag
* @return   bool
* @brief    return true if decsion is taken on all threat
*/
bool BagData::canTakeSuspectDecisionOnBag()
{
  TRACE_LOG("");
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG result =
      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown;
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  bool enablethreathandling;
  if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)
  {
    enablethreathandling = WorkstationConfig::getInstance()->showMachineAlarmDecision();
  }
  else
  {
    enablethreathandling = WorkstationConfig::getInstance()->getEnableThreathandling();
  }
  //= WorkstationConfig::getInstance()->showMachineAlarmDecision();//getEnableThreathandling();
  if(!enablethreathandling)
  {
    return true;
  }
  else if(isTIPBag())
  {
    return true;
  }
  else if ((currentscreen != QMLEnums::BAGSCREENING_SCREEN)
           && (currentscreen != QMLEnums::TRAINING_BAG_SCREEN)
           && (currentscreen != QMLEnums::SEARCH_BAG_SCREEN))
  {
    return true;
  }


  bool enable = false;
  SDICOS::TDRModule *tdr_module = nullptr;
  if(currentscreen == QMLEnums::SEARCH_BAG_SCREEN)
  {
    tdr_module = getOperatorTdrModule();
  }
  else
  {
    if((m_currenttdrModule != nullptr) && (m_tdrmodule.size() > 0))
    {
      SDICOS::TDRTypes::ThreatDetectionReport& threat_report = m_currenttdrModule->GetThreatDetectionReport();
      SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();

      if(tdr_type == SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE::enumOperator)
      {
        tdr_module = m_tdrmodule[0];
      }
      else
      {
        tdr_module = m_currenttdrModule;
      }
    }
    else if(m_operatorTdr != nullptr)
    {
      tdr_module = m_operatorTdr;
    }
    else if(getTotalEffectiveAvailableAlarmThreats() == 0)
    {
      enable = true;
    }
  }

  if(tdr_module)
  {
    SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array =
        tdr_module->GetThreatSequence().GetPotentialThreatObject();

    unsigned int numPTOs = threat_item_array.GetSize();
    unsigned int j = 0;
    for(j = 0; j < numPTOs; j++)
    {
      SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem> assesmentitm
          = threat_item_array[j].GetAssessment().GetAssessment();

      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG machinedecision
          = assesmentitm[0].GetAssessmentFlag();
      result = getCurrentThreatUserDecision(j);
      if(currentscreen == QMLEnums::SEARCH_BAG_SCREEN)
      {
        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem
            ::ASSESSMENT_FLAG primaryUserDecision = getUserDecisionfromPrimaryTdr
            (static_cast<unsigned int>(j));
        if(result == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown)
        {
          if(primaryUserDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown)
          {
            break;
          }
          if(primaryUserDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumNoThreat)
          {
            if(WorkstationConfig::getInstance()->isDisplayOperatorClearedAlarmEnabled())
            {
              break;
            }
          }
          else if(primaryUserDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumThreat)
          {
            if(result == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown)
            {
              break;
            }
          }
        }
      }
      else
      {
        if(machinedecision != SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumNoThreat)
        {
          if(result == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown)
          {
            break;
          }
        }
      }
    }
    if(j == numPTOs)
    {
      enable = true;
    }
  }

  DEBUG_LOG("Enable flag: " << enable);
  return enable;
}

/*!
* @fn       canTakeClearDecisionOnBag
* @return   bool
* @brief    return true if any threat marked as suspect
*/
bool BagData::canTakeClearDecisionOnBag()
{
  TRACE_LOG("");
  bool enable = true;
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();

  if ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::OSR_WORKSTATION)
      && (isTIPBag() == false))
  {
    int sliceCount = getSliceCountReceived();
    if (sliceCount < WorkstationConfig::getInstance()->getMinimumRequiredSliceCount())
    {
      QString msg = QString("Bag slice count is less than minimum required count.");
      ERROR_LOG("slice count = " << sliceCount << " Required count= " <<
                WorkstationConfig::getInstance()->getMinimumRequiredSliceCount())
          addErrorMsg(msg);
      enable = false;
    }
  }
  SDICOS::TDRModule *tdr_module = nullptr;
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG result =
      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown;


  if(currentscreen == QMLEnums::SEARCH_BAG_SCREEN)
  {
    tdr_module = getOperatorTdrModule();
  }
  else
  {
    if((m_currenttdrModule != nullptr) && (m_tdrmodule.size() > 0))
    {
      SDICOS::TDRTypes::ThreatDetectionReport& threat_report = m_currenttdrModule->GetThreatDetectionReport();
      SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();
      if(tdr_type == SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE::enumOperator)
      {
        tdr_module = m_tdrmodule[0];
      }
      else
      {
        tdr_module = m_currenttdrModule;
      }
    }
    else if(m_operatorTdr != nullptr)
    {
      tdr_module = m_operatorTdr;
    }
  }

  if(tdr_module)
  {
    SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array =
        tdr_module->GetThreatSequence().GetPotentialThreatObject();

    unsigned int numPTOs = threat_item_array.GetSize();
    unsigned int j = 0;
    for(j = 0; j < numPTOs; j++)
    {
      SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem> assesmentitm
          = threat_item_array[j].GetAssessment().GetAssessment();

      SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG machinedecision
          = assesmentitm[0].GetAssessmentFlag();

      if((currentscreen == QMLEnums::SEARCH_BAG_SCREEN)
         && (WorkstationConfig::getInstance()->getEnableThreathandling()))
      {
        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem
            ::ASSESSMENT_FLAG primaryUserDecision = getUserDecisionfromPrimaryTdr
            (static_cast<unsigned int>(j));
        result = getCurrentThreatUserDecision(j);
        if(result == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown)
        {
          if(primaryUserDecision == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumNoThreat)
          {
            if(WorkstationConfig::getInstance()->isDisplayOperatorClearedAlarmEnabled())
            {
              enable = false;
              break;
            }
          }
          else
          {
            enable = false;
            break;
          }
        }
        if(result == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumThreat)
        {
          enable = false;
          break;
        }
      }
      else if((!isTIPBag())
              && ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)?
                  WorkstationConfig::getInstance()->showMachineAlarmDecision():
                  WorkstationConfig::getInstance()->getEnableThreathandling())
              && (machinedecision !=
                  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumNoThreat))
      {
        result = getCurrentThreatUserDecision(j);
        if((result == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumThreat)
           || (result ==
               SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown))
        {
          enable = false;
          break;
        }
      }
    }
  }

  bool isbagError = false;
  if(currentscreen != QMLEnums::SEARCH_BAG_SCREEN)
  {
    isbagError = isBagError();
  }

  return ((!isbagError)
          && ((currentscreen == QMLEnums::SEARCH_BAG_SCREEN) || !(getOperatorAlarmThreat() > 0))
          && enable);
}


/*!
* @fn       populateDecisionOnAllthreat
* @param    SDICOS::TDRModule* tdr
* @param    QMLEnums::ThreatEvalPanelCommand - bag decision
* @brief    This function will populate bag decision on all threats.
*/
void BagData::populateDecisionOnAllthreat(SDICOS::TDRModule* tdr,
                                          QMLEnums::ThreatEvalPanelCommand decision)
{
  TRACE_LOG("");
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG flag;
  if ( (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_BAG)
       || (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_RETAIN_BAG)
       || (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_TIMEOUT_BAG)
       )
  {
    flag = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat;
  }
  else if (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_BAG)
  {
    flag = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat;
  }
  else
  {
    flag = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknown;
  }

  // Enquire threat item array and report for input TDR module
  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array =
      tdr->GetThreatSequence().GetPotentialThreatObject();
  unsigned int numPTOs = threat_item_array.GetSize();
  for(unsigned int j = 0; j < numPTOs; j++)
  {
    populateDecisionOnParticularThreat(j, flag);
  }
}

/*!
* @fn       populateDecisionOnParticularThreat
* @param    QMLEnums::ThreatEvalPanelCommand - bag decision
* @brief    This function will populate bag decision on particular threat.
*/
void BagData::populateDecisionOnParticularThreat (
    QMLEnums::ThreatEvalPanelCommand decision)
{
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG flag;

  if (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_SUSPECT_THREAT)
  {
    flag = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat;
  }
  else if (decision == QMLEnums::ThreatEvalPanelCommand::TEPC_CLEAR_THREAT)
  {
    flag = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat;
  }
  else
  {
    flag = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknown;
  }
  DEBUG_LOG("Populate threat as " << flag << "on current threat no " << m_currrentThreat);
  populateDecisionOnParticularThreat(m_currrentThreat, flag);
}

/*!
* @fn       populateDecisionOnParticularThreat
* @param    int - threat id
* @param    ASSESSMENT_FLAG - enumNoThreat, enumThreat, enumUnknown
* @return   None
* @brief    This function will polulate particular threat on tdr with decision
*/
void BagData::populateDecisionOnParticularThreat(unsigned int threatcount,
                                                 SDICOS::TDRTypes::AssessmentSequence
                                                 ::AssessmentItem::ASSESSMENT_FLAG flag)
{
  if(m_operatorTdr != nullptr)
  {
    SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array =
        m_operatorTdr->GetThreatSequence().GetPotentialThreatObject();

    SDICOS::TDRTypes::AssessmentSequence& assessnentSeq = threat_item_array[threatcount].GetAssessment();
    assessnentSeq.GetAssessment()[0].SetAssessmentFlag(flag);
  }
}

/*!
* @fn       getUserDecisionFromTdr
* @param    SDICOS::TDRModule* tdr, unsigned int threatIndex
* @return   ASSESSMENT_FLAG: enumNoThreat, enumThreat, enumUnknown
* @brief    This function will return user decision for particular threat on given TDR.
*/
SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG
BagData::getUserDecisionFromTdr(SDICOS::TDRModule* tdr, unsigned int threatIndex)
{
  TRACE_LOG("");
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG flag
      = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown;
  if (tdr == nullptr)
  {
    return flag;
  }
  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array =
      tdr->GetThreatSequence().GetPotentialThreatObject();

  if ((threat_item_array.GetSize() <= threatIndex) || (threat_item_array.GetSize() == 0))
  {
    return flag;
  }
  SDICOS::TDRTypes::AssessmentSequence& assessnentSeq = threat_item_array[threatIndex].GetAssessment();
  if(assessnentSeq.GetAssessment().GetSize() == 0)
  {
    return flag;
  }
  flag = assessnentSeq.GetAssessment()[0].GetAssessmentFlag();
  DEBUG_LOG("User decision from tdr flag: " << flag);
  return flag;
}

/*!
* @fn       getCurrentThreatUserDecision
* @param    unsigned int threatIndex
* @return   ASSESSMENT_FLAG: enumNoThreat, enumThreat, enumUnknown
* @brief    This function will return user decision for particular threat on operator TDR.
*/
SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG
BagData::getCurrentThreatUserDecision(unsigned int threatIndex)
{
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG flag
      = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown;

  flag = getUserDecisionFromTdr(m_operatorTdr, threatIndex);
  DEBUG_LOG("User decision from operator tdr flag: " << flag);
  return flag;
}

/*!
* @fn       getUserDecisionfromPrimaryTdr
* @param    unsigned int threatIndex
* @return   ASSESSMENT_FLAG: enumNoThreat, enumThreat, enumUnknown
* @brief    This function will return user decision for particular threat on operator TDR.
*/
SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG
BagData::getUserDecisionfromPrimaryTdr(unsigned int threatIndex)
{
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG flag
      = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown;
  if (m_primaryOperatorTdr != nullptr)
  {
    flag = getUserDecisionFromTdr(m_primaryOperatorTdr, threatIndex);
  }
  DEBUG_LOG("User decision from primary operator tdr flag: " << flag);
  return flag;
}

/*!
* @fn       getUserDecisionfromOperatorTdr
* @param    unsigned int threatIndex
* @return   ASSESSMENT_FLAG: enumNoThreat, enumThreat, enumUnknown
* @brief    This function will return user decision for particular threat on operator TDR.
*/
SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG
BagData::getUserDecisionfromOperatorTdr(unsigned int threatIndex)
{
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG flag
      = SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown;
  if (m_operatorTdr != nullptr)
  {
    flag = getUserDecisionFromTdr(m_operatorTdr, threatIndex);
  }
  return flag;
}

/*!
* @fn       initializeOperatorTDR
* @param    none
* @return   none
* @brief    This function will initialize operator TDR with initial unknown decsion.
*/
void BagData::initializeOperatorTDR()
{
  SDICOS::TDRModule* tdr = addOperatorTDRModule();
}

/*!
* @fn       populateThreatsOnTDR
* @param    SDICOS::TDRModule& tdr
* @param    SDICOS::CTModule *
* @return   bool - result
* @brief    This function will add all threat data to given operator TDR module.
*/
bool BagData::populateThreatsOnTDR(SDICOS::TDRModule& my_tdr,
                                   SDICOS::CTModule * ctmodule)
{
  TRACE_LOG("");
  if (ctmodule == NULL)
  {
    return false;
  }
  // Enquire threat item array and report for input TDR module
  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& my_threat_item_array =
      my_tdr.GetThreatSequence().GetPotentialThreatObject();
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();

  int size = m_tdrmodule.size();
  if (size > 0)
  {
    SDICOS::TDRModule* tdrModule = NULL;
    std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
    int baseThreatCount = 0;
    for(unsigned int i = 0; i < tdrArray.size(); i++)
    {
      tdrModule = tdrArray.at(i);
      if (tdrModule != NULL)
      {
        SDICOS::TDRTypes::ThreatDetectionReport tdrreport =
            tdrModule->GetThreatDetectionReport();
        SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdrType =
            tdrreport.GetTdrType();

        SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>&
            threat_item_array_src = tdrModule->GetThreatSequence().GetPotentialThreatObject();
        unsigned int numPTOs = threat_item_array_src.GetSize();

        for(unsigned int j = 0; j < numPTOs; j++)
        {
          SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object_src;
          threat_item_array_src[j].GetReferencedPotentialThreatObject(ref_object_src);
          if (ref_object_src.GetReferencedPotentialThreatObject().GetSize() > 0)
          {
            continue;
          }
          // Create Base ThreatItem
          SDICOS::TDRTypes::ThreatSequence::ThreatItem threat_item;
          threat_item.EnableBaggageTDR(true);
          threat_item.SetID(baseThreatCount + j);
          threat_item.SetAssessment(threat_item_array_src[j].GetAssessment());

          SDICOS::TDRTypes::AssessmentSequence& seq = threat_item.GetAssessment();
          SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem>& items = seq.GetAssessment();

          if (items.GetSize() > 0)
          {
            if (items[0].GetAssessmentFlag() !=
                SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
            {
              items[0].SetAssessmentFlag(SDICOS::TDRTypes::AssessmentSequence::AssessmentItem
                                         ::enumUnknown);
            }
          }

          // bdavidson: added for DICOS v2.0a
          SDICOS::Array1D<SDICOS::TDRTypes::BaggageSpecificTDR::PTORepresentationSequenceItem>& dest_pto_array =
              threat_item.GetBaggageTDR().GetPTORepresentationSequenceItems();
          dest_pto_array.SetSize(1);
          SDICOS::TDRTypes::BaggageSpecificTDR::PTORepresentationSequenceItem& dest_pto = dest_pto_array[0];

          // bdavidson: modified for DICOS v2.0a
          SDICOS::Array1D<SDICOS::SopInstanceReference>& sop_sequence =
              dest_pto.GetReferencedInstance();
          sop_sequence.SetSize(1);
          sop_sequence[0].SetClassUID(SDICOS::SOPClassUID::GetCT());
          sop_sequence[0].SetInstanceUID(
                ctmodule->GetSopCommon().GetSopInstanceUID());
          // Set bounds of threat ROI
          // bdavidson: modified for DICOS v2.0a
          SDICOS::TDRTypes::CommonSpecificTDR::ThreatROI& threat_roi_dest =
              dest_pto.GetThreatROI();
          // bdavidson: modified for DICOS v2.0a
          // bdavidson: Careful, I did not check to see if the array actually has an index 0 entry!
          SDICOS::TDRTypes::CommonSpecificTDR::ThreatROI& threat_roi_src =
              threat_item_array_src[j].GetBaggageTDR().GetPTORepresentationSequenceItems()[0].GetThreatROI();
          float xmin, ymin, zmin;
          float size_x, size_y, size_z;

          threat_roi_src.GetThreatRoiBase(xmin, ymin, zmin);
          threat_roi_src.GetThreatRoiExtents(size_x, size_y, size_z);
          threat_roi_dest.SetThreatRoiBase(xmin, ymin, zmin);
          threat_roi_dest.SetThreatRoiExtents(size_x, size_y, size_z);

          // Reference ThreatItems of other TDRs than operator
          SDICOS::TDRTypes::ReferencedPotentialThreatObject::PTOItem pto_item;
          pto_item.SetObjectID(threat_item_array_src[j].GetID());

          SDICOS::SopInstanceReference& sop_ref = pto_item.GetSopInstanceReference();
          sop_ref.SetClassUID(SDICOS::SOPClassUID::GetTDR());
          sop_ref.SetInstanceUID(tdrModule->GetSopCommon().GetSopInstanceUID());

          SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object_dest;
          ref_object_dest.GetReferencedPotentialThreatObject().Add(pto_item);

          if ((currentscreen == QMLEnums::SEARCH_BAG_SCREEN)
              && m_primaryOperatorTdr)
          {
            SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>&
                threat_item_array_operator = m_primaryOperatorTdr->GetThreatSequence()
                .GetPotentialThreatObject();
            if(threat_item_array_operator.GetSize() > j)
            {
              SDICOS::TDRTypes::ReferencedPotentialThreatObject::PTOItem pto_item;
              pto_item.SetObjectID(threat_item_array_operator[j].GetID());

              SDICOS::SopInstanceReference& sop_ref = pto_item.GetSopInstanceReference();
              sop_ref.SetClassUID(SDICOS::SOPClassUID::GetTDR());
              sop_ref.SetInstanceUID(m_primaryOperatorTdr->GetSopCommon().GetSopInstanceUID());
              ref_object_dest.GetReferencedPotentialThreatObject().Add(pto_item);
            }
          }
          threat_item.SetReferencedPotentialThreatObject(ref_object_dest);
          my_threat_item_array.Add(threat_item);
        }
        baseThreatCount += numPTOs;
        INFO_LOG("Total Potential threat items in TDR module: "  <<
                 numPTOs << " Threat item size in TDR: " << my_threat_item_array.GetSize());
      }
    }
  }
  return true;
}

/*!
   * @fn       getShowAllThreat
   * @param    None
   * @return   bool
   * @brief    returns show all threat.
   */
bool BagData::getShowAllThreat() const
{
  return m_showAllThreat;
}

/*!
   * @fn       isSlabViewThreatModeEnable
   * @param    None
   * @return   bool
   * @brief    return Slab View Threat mode enable/disable status.
   */
bool BagData::isSlabViewThreatModeEnable()
{
  bool enableThreatMode = false;
  if(getTotalVisibleAlarmThreats() > 0)
  {
    enableThreatMode = true;
  }
  return enableThreatMode;
}

/*!
   * @fn       setShowAllThreat
   * @param    bool
   * @return   None
   * @brief    Sets show all threat.
   */
void BagData::setShowAllThreat(bool showAllThreat)
{
  m_showAllThreat = showAllThreat;
}

// TODO(Agiliad) Revisit when separating rerun and training#ifdef WORKSTATION
/*!
* @fn       addOperatorTDRModule
* @param    None
* @return   SDICOS::TDRModule *
* @param    Add Operator TDR module
*/
SDICOS::TDRModule *BagData::addOperatorTDRModule()
{
  TRACE_LOG("");
  bool res;
  SDICOS::TDRModule tdr;
  m_operatorTdr = nullptr;

  res = populateCommonTdrParts(&tdr, QMLEnums::ThreatEvalPanelCommand::TEPC_UNKNOWN, getVolumeCtModule());
  if(res)
  {
    // Add Operator threat data in given tdr module
    bool res = populateThreatsOnTDR(tdr, getVolumeCtModule());
    if (!res)
    {
      ERROR_LOG("Population of Threats related data on TDR failed.");
      return m_operatorTdr;
    }
    SDICOS::ErrorLog errorLog;
    if(tdr.IsValid(errorLog))
    {
      m_operatorTdr = addTdrModule(&tdr);
    }
  }
  return m_operatorTdr;
}

/*!
* @fn       getOperatorAlarmThreat
* @param    None
* @return   int
* @brief    returns total operator alarm threat
*/
int BagData::getOperatorAlarmThreat()
{
  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
  SDICOS::TDRModule* tdrModule = NULL;
  int totalAlarmThreat = 0;
  int num_modules = tdrArray.size();
  if(num_modules > 0)
  {
    tdrModule = getOperatorTdrModule();
    if(tdrModule != NULL)
    {
      SDICOS::TDRTypes::ThreatSequence ts = tdrModule->GetThreatSequence();
      SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatItemArray =
          ts.GetPotentialThreatObject();


      for (unsigned int i = 0; i < threatItemArray.GetSize(); i++)
      {
        SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object;
        threatItemArray[i].GetReferencedPotentialThreatObject(ref_object);
        if (ref_object.GetReferencedPotentialThreatObject().GetSize() == 0)
        {
          SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem> assesmentitm =
              threatItemArray[i].GetAssessment().GetAssessment();
          if(assesmentitm.GetSize() > 0)
          {
            SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG threatflag
                = assesmentitm[0].GetAssessmentFlag();
            if(threatflag != SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
            {
              totalAlarmThreat++;
            }
          }
        }
      }
    }
  }
  DEBUG_LOG("Total operator alarm threat: " <<totalAlarmThreat);
  return totalAlarmThreat;
}
/*!
* @fn       getOperatorTdrModule
* @param    None
* @return   SDICOS::TDRModule*
* @brief    returns operator tdr module
*/
SDICOS::TDRModule* BagData::getOperatorTdrModule()
{
  return m_operatorTdr;
}

/*!
* @fn       setBagRenderedTime
* @param    QDateTime
* @return   None
* @brief    Sets bag rendered time.
*/
void BagData::setBagRenderedTime(const QDateTime &bagRenderedTime)
{
  m_bagRenderedTime = bagRenderedTime;
  m_bagRenderedTimeTraining = QDateTime::currentDateTime();
}

/*!
* @fn       getIsTIPPhase
* @param    None
* @return   TIPPhase
* @brief    gets TIP Phase
*/
TIPPhase BagData::getTIPPhase() const
{
  return m_TIPPhase;
}

/*!
* @fn       setIsTIPPhase
* @param    const TIPPhase
* @return   None
* @brief    sets TIP Phase
*/
void BagData::setTIPPhase(const TIPPhase &TIPPhase)
{
  m_TIPPhase = TIPPhase;
}


/*!
* @fn       getBagReadytoRenderTime
* @param    None
* @return   QDateTime
* @brief    gets bag ready to render time
*/
QDateTime BagData::getBagReadytoRenderTime() const
{
  return m_bagReadytoRenderTime;
}


/*!
* @fn       getCurrenttdrModule
* @param    None
* @return   SDICOS::TDRModule *
* @brief    gets current tdr module
*/
SDICOS::TDRModule *BagData::getCurrenttdrModule() const
{
  return m_currenttdrModule;
}

/*!
* @fn       setCurrenttdrModule
* @param    SDICOS::TDRModule *
* @return   None
* @brief    gets current tdr module
*/
void BagData::setCurrenttdrModule(SDICOS::TDRModule *tdrmodule)
{
  m_currenttdrModule = tdrmodule;
}

/*!
* @fn       setCurrentVRViewmode()
* @param    const VREnums::VRViewModeEnum
* @return   None
* @brief    sets current VR view mode(threat, surface or laptop)
*/
void BagData::setCurrentVRViewmode(const VREnums::VRViewModeEnum &VRViewmode)
{
  m_VRViewmode = VRViewmode;
  refreshAttributes();
}

/*!
 * @fn       setSlabViewType()
 * @param    const VREnums::SlabViewType
 * @return   None
 * @brief    sets current SlabViewType
 */
void BagData::setCurrentSlabViewType(const VREnums::SlabViewType &SlabViewType)
{
  m_slabViewType = SlabViewType;
  refreshAttributes();
}

/*!
 * @fn       getCurrentSlabViewType()
 * @param    None
 * @return   VREnums::SlabViewType
 * @brief    gets current SlabViewType
 */
VREnums::SlabViewType BagData::getCurrentSlabViewType() const
{
  return m_slabViewType;
}


QString BagData::getOperatorName() const
{
  return m_operatorName;
}

void BagData::setOperatorName(const QString &operatorName)
{
  m_operatorName = operatorName;
}
/*!
* @fn       getUserDecision
* @param    None
* @return   QString
* @brief    Gets user decision.
*/
QString BagData::getUserDecision() const
{
  return m_userDecision;
}

/*!
* @fn       setUserDecision
* @param    const QString &
* @return   None
* @brief    Sets user decision.
*/
void BagData::setUserDecision(const QString &userDecision)
{
  m_userDecision = userDecision;
}

/*!
* @fn       removePickBoxThreat
* @param    unsigned int - local 0 index basedd threat id inside operator tdr module
* @return
* @brief    remove pick box threat from threat id
*/
int BagData::removePickBoxThreat(unsigned int threatid)
{
  TRACE_LOG("");
  if(!isCurrentThreatOperator())
  {
    ERROR_LOG("Asked for removing threat which is not operator. Threatid: " << threatid);
    return -1;
  }
  SDICOS::TDRModule *tdrModule = getOperatorTdrModule();
  if (tdrModule == nullptr)
  {
    return -1;
  }
  unsigned int nonOperatorThreatCount = 0;
  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();

  for (unsigned int k = 0; k < tdrArray.size(); k++)
  {
    SDICOS::TDRModule* tdrModule = tdrArray.at(k);
    if(tdrModule == nullptr)
    {
      continue;
    }
    SDICOS::TDRTypes::ThreatDetectionReport tdrreport =
        tdrModule->GetThreatDetectionReport();
    SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdrType =
        tdrreport.GetTdrType();
    if (tdrType != SDICOS::TDRTypes::ThreatDetectionReport::enumOperator)
    {
      SDICOS::TDRTypes::ThreatSequence ts = tdrModule->GetThreatSequence();
      SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatItemArray =
          ts.GetPotentialThreatObject();
      nonOperatorThreatCount += threatItemArray.GetSize();
    }
  }
  if (threatid < nonOperatorThreatCount)
  {
    ERROR_LOG("Operator id can not be less than total alarm threat. Can not remove operator threat");
    return -1;
  }

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array_src =
      tdrModule->GetThreatSequence().GetPotentialThreatObject();
  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> new_threat_item_array;
  for (unsigned int i = 0; i < threat_item_array_src.GetSize(); i++)
  {
    if(i == threatid)
    {
      continue;
    }
    new_threat_item_array.Add(threat_item_array_src[i]);
  }
  tdrModule->GetThreatSequence().SetPotentialThreatObject(new_threat_item_array);


  refreshAttributes();
  for (unsigned int k = 0; k < tdrArray.size(); k++)
  {
    SDICOS::TDRModule* tdrModule = tdrArray.at(k);
    if(tdrModule == nullptr)
    {
      continue;
    }
    SDICOS::TDRTypes::ThreatDetectionReport& threat_report = tdrModule->GetThreatDetectionReport();
    threat_report.SetNumberOfAlarmObjects(getTotalAvailableAlarmThreats());
    threat_report.SetNumberOfTotalObjects(getTotalAvailableThreats());
    threat_report.SetAbortFlag(SDICOS::TDRTypes::ThreatDetectionReport::enumSuccess);
    threat_report.SetAlarmDecision(SDICOS::TDRTypes::ThreatDetectionReport::enumUnknownAlarmDecision);
  }

  // Correct global index for next threat
  if ((m_currrentThreat == 0) && ((m_availablealarmcount == 1) || (m_availablealarmcount == 0)))
  {
    m_currrentThreat = DEFAULT_VALUE_CURRENT_THREAT;
    m_currenttdrModule = nullptr;
  }
  else if ((m_currrentThreat >0) && (m_currrentThreat <(m_availablealarmcount-1)))
  {
    --m_currrentThreat;
  }
  else
  {
    m_currrentThreat = DEFAULT_VALUE_CURRENT_THREAT;
    m_currenttdrModule = nullptr;
  }
  // Correct visible index for next threat
  decrementCurrentVisibleEntityIndex();
  refreshAttributes();
  return m_currrentThreat;
}

/*!
* @fn       addPickBoxThreat()
* @param    SDICOS::CTModule* ctModules
* @param    QVector3D& start
* @param    QVector3D& end
* @return   int -1 if error 0 - success
* @brief    Create new operator tdr module if doesnt exists already and add new threat for given threat bounds
*/
int BagData::addPickBoxThreat(QVector3D& start,
                              QVector3D& end,
                              QMLEnums::ThreatEvalPanelCommand eThreatType)
{
  TRACE_LOG("");
  SDICOS::CTModule * ctModule = getVolumeCtModule();
  if (ctModule == NULL)
  {
    return -1;
  }

  SDICOS::TDRModule *tdrModule = getOperatorTdrModule();
  if (tdrModule == NULL)
  {
    // Operator TDR will be added only if we are in OSR mode. that is we have live bags
    tdrModule = addOperatorTDRModule();
    if (tdrModule == NULL)
    {
      return -1;
    }
  }

  // Create new threat item
  SDICOS::TDRTypes::ThreatSequence::ThreatItem pickbox_threat_item;
  pickbox_threat_item.EnableBaggageTDR(true);
  pickbox_threat_item.SetID(0);

  // Set threat assessment information
  SDICOS::TDRTypes::AssessmentSequence& assessment_sequence = pickbox_threat_item.GetAssessment();
  assessment_sequence.GetAssessment().SetSize(1);

  if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_EXPLOSIVES)
  {
    assessment_sequence.GetAssessment()[0].SetThreatCategory(
          SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumExplosive);
    assessment_sequence.GetAssessment()[0].SetThreatCategoryDescription("BULK");
  }
  else if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_ANOMALY)
  {
    assessment_sequence.GetAssessment()[0].SetThreatCategory(
          SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumAnomaly);
    assessment_sequence.GetAssessment()[0].SetThreatCategoryDescription("BULK");
  }
  else if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_WEAPON)
  {
    assessment_sequence.GetAssessment()[0].SetThreatCategory(
          SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumProhibitedItem);
    assessment_sequence.GetAssessment()[0].SetThreatCategoryDescription("WEAPON");
  }
  else if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_LIQUID)
  {
    assessment_sequence.GetAssessment()[0].SetThreatCategory(
          SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumProhibitedItem);
    assessment_sequence.GetAssessment()[0].SetThreatCategoryDescription("LIQUID");
  }
  else if(eThreatType == QMLEnums::ThreatEvalPanelCommand::TEPC_PICKBOX_ACCEPT_CONTRABAND)
  {
    assessment_sequence.GetAssessment()[0].SetThreatCategory(
          SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumContraband);
    assessment_sequence.GetAssessment()[0].SetThreatCategoryDescription("BULK");
  }
  else
  {
    assessment_sequence.GetAssessment()[0].SetThreatCategory(
          SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumUnknownThreatCategory);
    assessment_sequence.GetAssessment()[0].SetThreatCategoryDescription("BULK");
  }

  assessment_sequence.GetAssessment()[0].SetAbilityAssessment(
        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoInterference);
  assessment_sequence.GetAssessment()[0].SetAssessmentFlag(
        SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumThreat);

  SDICOS::TDRTypes::BaggageSpecificTDR &baggage_tdr = pickbox_threat_item.GetBaggageTDR();

  // bdavidson: added for DICOS v2.0a
  SDICOS::Array1D<SDICOS::TDRTypes::BaggageSpecificTDR::PTORepresentationSequenceItem>& pto_array =
      baggage_tdr.GetPTORepresentationSequenceItems();
  pto_array.SetSize(1);
  SDICOS::TDRTypes::BaggageSpecificTDR::PTORepresentationSequenceItem& pto = pto_array[0];

  // bdavidson: modified for DICOS v2.0a
  SDICOS::Array1D<SDICOS::SopInstanceReference> &sop_sequence = pto.GetReferencedInstance();
  sop_sequence.SetSize(1);
  sop_sequence[0].SetClassUID(SDICOS::SOPClassUID::GetCT());
  sop_sequence[0].SetInstanceUID(ctModule->GetSopCommon().GetSopInstanceUID());
  int xmin = start.x();
  int ymin = start.y();
  int zmin = start.z();
  int size_x = end.x();
  int size_y = end.y();
  int size_z = end.z();

  // Set bounds of threat ROI
  // bdavidson: modified for DICOS v2.0a
  SDICOS::TDRTypes::CommonSpecificTDR::ThreatROI& threat_roi = pto.GetThreatROI();
  threat_roi.SetThreatRoiBase(xmin, ymin, zmin);
  threat_roi.SetThreatRoiExtents(size_x, size_y, size_z);

  SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threat_item_array =
      m_operatorTdr->GetThreatSequence().GetPotentialThreatObject();
  threat_item_array.Add(pickbox_threat_item);
  refreshAttributes();

  increamentCurrentVisibleEntityIndex();
  return 0;
}
// #endif
/*!
* @fn       isValid
* @param    None
* @return   bool
* @brief    validated received bagdata
*/
bool BagData::isValid()
{
  bool res = true;
  if(getCtModules().GetSize() < 1)
  {
    addErrorMsg(tr("CT Module is empty. "));
    res = false;
  }
  if(getvolumedata() == NULL)
  {
    addErrorMsg(tr("Volume Data is null. "));
    res = false;
  }
  if (getVolumeDimension().isNull())
  {
    addErrorMsg(tr("Volume dimension are not set. "));
    res = false;
  }
  if (getTdrModules().size() < 1)
  {
    addErrorMsg(tr("There should be atleast one TDR Module present with Bag. "));
  }
  return res;
}

/*!
* @fn       getCurrrentThreat
* @param    None
* @return   int
* @brief    get Current Threat count
*/
int BagData::getCurrrentThreat() const
{
  return m_currrentThreat;
}
/*!
* @fn       setCurrentThreat
* @param    int
* @return   None
* @brief    set Current Threat count
*/
void BagData::setCurrentThreat(int currrentThreat)
{
  m_currrentThreat = currrentThreat;
}

/*!
* @fn       getCurrentVisibleEntityIndex
* @param    None
* @return   int
* @brief    Gets Currently visible entity index
*/
int BagData::getCurrentVisibleEntityIndex()
{
  return m_visibleCurrentThreatIndex;
}

/*!
* @fn       setCurrentVisibleEntityIndex
* @param    int
* @return   Node
* @brief    sets Currently visible entity index
*/
void BagData::setCurrentVisibleEntityIndex(int index)
{
  m_visibleCurrentThreatIndex = index;
}

/*!
* @fn       decrementCurrentVisibleEntityIndex
* @param    None
* @return   None
* @brief    decreament current visible entity index
*/
void BagData::decrementCurrentVisibleEntityIndex()
{
  if ((m_visibleCurrentThreatIndex == 0) && ((m_visiblealarmcount == 1) || (m_visiblealarmcount == 0)))
  {
    m_visibleCurrentThreatIndex = DEFAULT_VALUE_CURRENT_THREAT;
  }
  else if ((m_visibleCurrentThreatIndex >0) && (m_visibleCurrentThreatIndex <(m_visiblealarmcount-1)))
  {
    --m_visibleCurrentThreatIndex;
  }
  else
  {
    m_visibleCurrentThreatIndex = DEFAULT_VALUE_CURRENT_THREAT;
  }
}
/*!
* @fn       increamentCurrentVisibleEntityIndex
* @param    int
* @return   Node
* @brief    increament current visible entity index
*/
void BagData::increamentCurrentVisibleEntityIndex()
{
  ++m_visibleCurrentThreatIndex;
  if(m_visibleCurrentThreatIndex >= getCurrentVisibleEntityCount())
  {
    m_visibleCurrentThreatIndex = 0;
  }
  if (getCurrentVisibleEntityCount() <=0)
  {
    m_visibleCurrentThreatIndex = DEFAULT_VALUE_CURRENT_THREAT;
  }
}

/*!
* @fn       applyDecisionOnThreat
* @param    None
* @return   None
* @brief    Apply Decision on threat.
*/
void BagData::applyDecisionOnThreat()
{
  int visibleEntityIndex = getCurrentVisibleEntityIndex();
  visibleEntityIndex--;

  if (getCurrentVisibleEntityCount() > 0)
  {
    if (visibleEntityIndex == -1)
    {
      visibleEntityIndex = getCurrentVisibleEntityCount();
    }
  }

  setCurrentVisibleEntityIndex(visibleEntityIndex);
}

/*!
* @fn       setNextThreat()
* @param    None
* @return   bool
* @brief    Set Next threat to highlight.
*/
bool BagData::setNextThreat()
{
  TRACE_LOG("");
  refreshAttributes();
  // 1. Validate current bag states
  int totalVisibleEntity = getCurrentVisibleEntityCount();
  if (totalVisibleEntity <=0)
  {
    INFO_LOG("Can not set next threat or laptop as either there is no threat"
             " in threat mode or no laptop in laptop mode.");
    return false;
  }


  // Get number of threat modules in current bag data
  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
  unsigned int num_tdr_modules = tdrArray.size();
  if(num_tdr_modules > 0)
  {
    setCurrenttdrModule(tdrArray.at(0));
  }
  else
  {
    DEBUG_LOG("Tdrmodule in bag data is null.");
    return false;
  }
  bool bEnableThreatHandeling = WorkstationConfig::getInstance()->getEnableThreathandling();
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  // 2. Increament current threat global index and validate
  m_currrentThreat++;

  // 3. Identify total threats and total threats per tdr
  int threats_per_tdr[num_tdr_modules];
  int total_available_entity = 0;
  for(unsigned int k = 0; k < num_tdr_modules; ++k)
  {
    SDICOS::TDRModule* temp_module = tdrArray.at(k);
    SDICOS::TDRTypes::ThreatSequence ts = temp_module->GetThreatSequence();
    SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatItemArray =
        ts.GetPotentialThreatObject();
    unsigned int numthreats = 0;

    for (unsigned int j=0; j < threatItemArray.GetSize(); j++)
    {
      SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object;
      threatItemArray[j].GetReferencedPotentialThreatObject(ref_object);

      if (ref_object.GetReferencedPotentialThreatObject().GetSize() == 0)
      {
        //For ignoring Dummy variab;e
        SDICOS::TDRTypes::ThreatDetectionReport& threat_report = temp_module->GetThreatDetectionReport();
        SDICOS::TDRTypes::CommonSpecificTDR::ThreatROI roi =
            threatItemArray[j].GetBaggageTDR().GetPTORepresentationSequenceItems()[0].GetThreatROI();
        float roiBasex, roiBasey, roiBasez;
        float size_x, size_y, size_z;
        roi.GetThreatRoiBase(roiBasex, roiBasey, roiBasez);
        roi.GetThreatRoiExtents(size_x, size_y, size_z);
        if(roiBasex == 10 && roiBasey == 10 && roiBasez == 10
           && size_x == 10 && size_y == 10 && size_z ==10 )
        {
          INFO_LOG("Skipping the dummy threat that has : ROI("<<roiBasex<<","<<roiBasey<<","<<roiBasez
                   <<") Size(" <<size_x <<","<< size_y <<","<<size_z<<")");
        }
        else
        {
          numthreats++;
        }
      }
    }
    threats_per_tdr[k] = numthreats;
    total_available_entity+=numthreats;
  }

  // 4. Validate Current global threat index
  if(m_currrentThreat >= total_available_entity)
  {
    m_currrentThreat = 0;
  }

  // 5. Identify local index and TDR out of global
  int tempcurrentthreat = m_currrentThreat;
  DEBUG_LOG("Current Threat temp: " << tempcurrentthreat);

  do
  {
    DEBUG_LOG("Current Threat: " << m_currrentThreat);

    DEBUG_LOG("Primary tdr Threat type: " << getUserDecisionfromPrimaryTdr(m_currrentThreat));

    unsigned int local_index = 0;
    int global_index = 0;
    for(unsigned int k = 0; k < num_tdr_modules; ++k)
    {
      global_index += threats_per_tdr[k];
      if (m_currrentThreat < global_index)
      {
        setCurrenttdrModule(tdrArray.at(k));
        if ((currentscreen == QMLEnums::BAGSCREENING_SCREEN)
            || (currentscreen == QMLEnums::TRAINING_BAG_SCREEN))
        {
          if(tdrArray.at(k)->GetThreatDetectionReport().GetTdrType() ==
             SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE::enumMachine)
          {
            // this is the module
            local_index = m_currrentThreat - (global_index -  threats_per_tdr[k]);
          }
          else
          {
            local_index = m_currrentThreat;
          }
        }
        else
        {
          local_index = m_currrentThreat;
        }
        break;
      }
    }

    m_CurrentThreatType = VREnums::THREATTYPE_UNKNOWN_THREAT;


    // 6. Check that currently dentified entity is threat or laptop and visible on UI.
    //   If its not visible go to next available entity
    SDICOS::TDRModule *currTdrmodule = getCurrenttdrModule();
    if(currTdrmodule == nullptr)
    {
      ERROR_LOG("Error in current tdr module.");
      return false;
    }
    SDICOS::TDRTypes::ThreatSequence ts = currTdrmodule->GetThreatSequence();
    SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatitem =
        ts.GetPotentialThreatObject();
    if ((threatitem.GetSize() <= 0)
        || (threatitem.GetSize() <=  local_index))
    {
      DEBUG_LOG("There is no threat in TDR.");
      break;
    }
    SDICOS::TDRTypes::ThreatDetectionReport& threat_report = currTdrmodule->GetThreatDetectionReport();
    SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdr_type = threat_report.GetTdrType();
    SDICOS::Array1D<SDICOS::TDRTypes::AssessmentSequence::AssessmentItem> assesmentitm;
    assesmentitm = threatitem[local_index].GetAssessment().GetAssessment();
    if(assesmentitm.GetSize() > 0)
    {
      if(isValidVisibleEntity(currTdrmodule, local_index, m_CurrentThreatType))
      {
        break;
      }
    }

    // 7. Controll reached here means, currently identified entity is not visible over UI.
    //   Cycle to next visible entity (threat/ laptop)
    m_currrentThreat++;
    if(m_currrentThreat >= total_available_entity)
    {
      m_currrentThreat = 0;
    }

    DEBUG_LOG("Current Threat is not alarm. Incrementing threat" << m_currrentThreat);
  } while((m_CurrentThreatType == VREnums::THREATTYPE_OPERTOR_THREAT)
          || (m_CurrentThreatType == VREnums::THREATTYPE_ATR_THREAT)
          || (m_CurrentThreatType == VREnums::THREATTYPE_LAPTOP)
          || (m_CurrentThreatType == VREnums::THREATTYPE_TRUTH_THREAT)
          || (m_currrentThreat != tempcurrentthreat ));

  if ((m_CurrentThreatType == VREnums::THREATTYPE_OPERTOR_THREAT)
      || (m_CurrentThreatType == VREnums::THREATTYPE_ATR_THREAT)
      || (m_CurrentThreatType == VREnums::THREATTYPE_LAPTOP)
      || (m_CurrentThreatType == VREnums::THREATTYPE_TRUTH_THREAT))
  {
    // 5. Increament current threat visible index and validate
    if ((getCurrentSlabViewType() == VREnums::LAPTOP_VIEW)
        && (getCurrentVRViewmode() == VREnums::VIEW_AS_SLAB))
    {
      if (m_CurrentThreatType == VREnums::THREATTYPE_LAPTOP)
      {
        increamentCurrentVisibleEntityIndex();
      }
    }
    else
    {
      increamentCurrentVisibleEntityIndex();
    }
    return true;
  }

  DEBUG_LOG("End of BagData::setNextThreat()");
  return false;
}

/*!
* @fn       updateTIPThreat()
* @param    None
* @return   bool
* @brief    update TIP Threat
*/
bool BagData::updateTIPThreat()
{
  setTIPPhase(TIP_ANALYSIS_PHASE);
  refreshAttributes();

  // Get number of threat modules in current bag data
  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
  unsigned int num_tdr_modules = tdrArray.size();
  if(num_tdr_modules > 0)
  {
    setCurrenttdrModule(tdrArray.at(0));
  }
  else
  {
    DEBUG_LOG("Tdrmodule in bag data is null.");
    return false;
  }

  // 2. Set Current Threat as 0
  m_currrentThreat = 0;

  m_CurrentThreatType = VREnums::THREATTYPE_TRUTH_THREAT;
  m_visibleCurrentThreatIndex = 0;

  return true;
}

/*!
* @fn       isCurrentThreatOperator
* @param    None
* @return   bool
* @brief    retrurn true if Current Threat is operator
*/
bool BagData::isCurrentThreatOperator()
{
  return isOperatorTdr(m_currenttdrModule);
}

/*!
* @fn       isCurrentThreatOperatorAddedInTrainig
* @param    None
* @return   bool
* @brief    retrurn true if Current Threat is operator added after the bag rendered.
*           the operator threat added in pvs screen is neglected
*           it acts as machine threat for training simulator.
*/
bool BagData::isCurrentThreatOperatorAddedInTrainig()
{
  std::vector<SDICOS::TDRModule*> tdrArray = getTdrModules();
  SDICOS::TDRModule *tdrModule;
  if (tdrArray.size() > 0)
  {
    m_numberOfThreatInTDR1=0;
    for (unsigned int k = 0; k < tdrArray.size(); k++)
    {
      tdrModule = tdrArray.at(k);
      SDICOS::TDRTypes::ThreatSequence ts = tdrModule->GetThreatSequence();
      SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatItemArray =
          ts.GetPotentialThreatObject();

      for (unsigned int i = 0; i < threatItemArray.GetSize(); i++)
      {
        SDICOS::TDRTypes::ReferencedPotentialThreatObject ref_object;
        threatItemArray[i].GetReferencedPotentialThreatObject(ref_object);
        //get the total number of threats in tdr module 1 (Machine threat + Laptop threats +Operator threat added in PVS)
        if(k==1)
        {
          m_numberOfThreatInTDR1 =threatItemArray.GetSize();
        }

        if (ref_object.GetReferencedPotentialThreatObject().GetSize() == 0)
        {
          //check if the threat is operator, only for tdr 2 and skip the operator threats that is in tdr1
          //tdr module 2 has (Machine Threat+ Laptop Threat+ Opearator Threat added in PVS+ Operator added in Training Screen)
          if(k==2 && i>(m_numberOfThreatInTDR1-1))
          {
            return isOperatorTdrInTraining(m_currenttdrModule);
          }
        }
      }
    }

  }
  return false;
}

/*!
* @fn       isOperatorTdr
* @param    SDICOS::TDRModule* tdrModule
* @return   bool
* @brief    retrurn true if tdr is operator
*/
bool BagData::isOperatorTdr(SDICOS::TDRModule* tdrModule)
{
  if (tdrModule != NULL)
  {
    SDICOS::TDRTypes::ThreatDetectionReport tdrreport =
        tdrModule->GetThreatDetectionReport();
    SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdrType =
        tdrreport.GetTdrType();
    if (tdrType == SDICOS::TDRTypes::ThreatDetectionReport::enumOperator)
    {
      return true;
    }
  }
  return false;
}


/*!
* @fn       isOperatorTdr
* @param    SDICOS::TDRModule* tdrModule
* @return   bool
* @brief    retrurn true if tdr is operator added in tdr2
*           the operator threat added in pvs screen is neglected
*           it acts as machine threat for training simulator.
*/
bool BagData::isOperatorTdrInTraining(SDICOS::TDRModule* tdrModule)
{
  if (tdrModule != NULL)
  {
    SDICOS::TDRTypes::ThreatDetectionReport tdrreport =
        tdrModule->GetThreatDetectionReport();
    SDICOS::TDRTypes::ThreatDetectionReport::TDR_TYPE tdrType =
        tdrreport.GetTdrType();
    SDICOS::TDRTypes::ThreatSequence ts = tdrModule->GetThreatSequence();
    SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem> threatItemArray =
        ts.GetPotentialThreatObject();
    if(threatItemArray.GetSize()>m_numberOfThreatInTDR1)
    {
      if (tdrType == SDICOS::TDRTypes::ThreatDetectionReport::enumOperator)
      {
        return true;
      }
    }
  }
  return false;
}

/*!
* @fn       getCurrentVRViewmode()
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets current VR view mode(threat, surface or laptop)
*/
VREnums::VRViewModeEnum BagData::getCurrentVRViewmode() const
{
  return m_VRViewmode;
}

/*!
 * @fn       getArchiveSaveMode
 * @param    none
 * @return   bool mode
 * @brief    get Archive Save Mode
 */
bool BagData::getArchiveSaveMode()
{
  return m_archiveSaveMode;
}


/*!
 * @fn       setArchiveSaveMode
 * @param    bool mode
 * @return   None
 * @brief    sets Archive Save Mode
 */
void BagData::setArchiveSaveMode(bool mode)
{
  m_archiveSaveMode = mode;
}

/*!
* @fn       copyBagFiles
* @param    QString dest
* @param    QString src
* @param    QString bagFileNamePattern
* @return   int
* @brief    copy bagdata file to file
*/
int BagData::copyBagFiles(QString dest, QString src , QString bagFileNamePattern)
{
  QString volFileFormat;
  BagInfo bagInfo;
  if(bagInfo.constructFromFile(src+ "/" +bagFileNamePattern + BAGFILE_PROPERTY_FORMAT))
  {
    if(bagInfo.m_sliceFormat == VOLUME_FORMAT_JPEG)
    {
      volFileFormat = BAGFILE_JPEG_VOLUME_FORMAT;
    }
    else if(bagInfo.m_sliceFormat == VOLUME_FORMAT_UNCOMPRESSED)
    {
      volFileFormat = BAGFILE_VOLUME_FORMAT;
    }
    else
    {
      ERROR_LOG("Improper volume format: " << (src+bagFileNamePattern+volFileFormat).toStdString());
      return Errors::FILE_OPERATION_FAILED;
    }
  }
  else
  {
    ERROR_LOG("Improper file format: " << (src+bagFileNamePattern).toStdString());
    return Errors::FILE_OPERATION_FAILED;
  }

  //  Agiliad - Commented vol file copy for ANSS-1272
  //  int copySuccess = Utility::saveFile(dest+volFileFormat, src+ "/"
  //                                      +bagFileNamePattern+volFileFormat);
  //  if(copySuccess != Errors::FILE_OPERATION_SUCCESS)
  //  {
  //    ERROR_LOG("Failed to copy: " << (bagFileNamePattern+volFileFormat).toStdString());
  //    return copySuccess;
  //  }
  int copySuccess = Utility::saveFile(dest+BAGFILE_PROPERTY_FORMAT, src+
                                      "/" +bagFileNamePattern+BAGFILE_PROPERTY_FORMAT);
  if(copySuccess != Errors::FILE_OPERATION_SUCCESS)
  {
    ERROR_LOG("Failed to copy: " << (bagFileNamePattern+BAGFILE_PROPERTY_FORMAT).toStdString());
    return copySuccess;
  }
  copySuccess = Utility::saveFile(dest+BAGFILE_DICOSCT_FORMAT+".dcs", src+
                                  "/" +bagFileNamePattern+BAGFILE_DICOSCT_FORMAT+".dcs");
  if(copySuccess != Errors::FILE_OPERATION_SUCCESS)
  {
    ERROR_LOG("Failed to copy: " << (bagFileNamePattern+BAGFILE_DICOSCT_FORMAT+".dcs").toStdString());
    return copySuccess;
  }

  Utility::saveFile(dest+BAGFILE_DICOSCT_PROJ00_FORMAT+".dcs", src+
                    "/" +bagFileNamePattern+BAGFILE_DICOSCT_PROJ00_FORMAT+".dcs");

  Utility::saveFile(dest+BAGFILE_DICOSCT_PROJ90_FORMAT+".dcs", src+
                    "/" +bagFileNamePattern+BAGFILE_DICOSCT_PROJ90_FORMAT+".dcs");

  int i = 1;
  int tdrFilePresent = Errors::UNDEFINED_STATUS;
  do {
    QString tdrFileName;
    tdrFileName = BAGFILE_DICOSTDR_FORMAT;
    tdrFileName += QString::number(i);
    tdrFileName += ".dcs";

    tdrFilePresent = Utility::saveFile(dest+tdrFileName, src+
                                       "/" + bagFileNamePattern + tdrFileName);

    i++;
  }while (tdrFilePresent == Errors::FILE_OPERATION_SUCCESS);
  return copySuccess;
}
/*!
* @fn       removeBagFiles
* @param    QString src
* @param    QString bagFileNamePattern
* @return   bool
* @brief    remove bagdata file to file
*/
bool BagData::removeBagFiles(QString src, QString bagFileNamePattern)
{
  QString volFileFormat;
  BagInfo bagInfo;
  if(bagInfo.constructFromFile(src+ "/" +bagFileNamePattern + BAGFILE_PROPERTY_FORMAT))
  {
    if(bagInfo.m_sliceFormat == VOLUME_FORMAT_JPEG)
    {
      volFileFormat = BAGFILE_JPEG_VOLUME_FORMAT;
    }
    else if(bagInfo.m_sliceFormat == VOLUME_FORMAT_UNCOMPRESSED)
    {
      volFileFormat = BAGFILE_VOLUME_FORMAT;
    }
    else
    {
      ERROR_LOG("Improper voulme format: " << (src+bagFileNamePattern+volFileFormat).toStdString());
      return 0;
    }
  }
  else
  {
    ERROR_LOG("Improper file format: " << (src+bagFileNamePattern).toStdString());
    return 0;
  }
  int removeSuccess = Utility::removeFile(src+ "/" +bagFileNamePattern+volFileFormat);
  if(removeSuccess != Errors::FILE_OPERATION_SUCCESS)
  {
    ERROR_LOG("Failed to remove: " << (bagFileNamePattern+volFileFormat).toStdString());
  }
  removeSuccess = Utility::removeFile(src+ "/" +bagFileNamePattern+BAGFILE_PROPERTY_FORMAT);
  if(removeSuccess != Errors::FILE_OPERATION_SUCCESS)
  {
    ERROR_LOG("Failed to remove: " << (bagFileNamePattern+BAGFILE_PROPERTY_FORMAT).toStdString());
  }
  removeSuccess = Utility::removeFile(src+ "/" +bagFileNamePattern+BAGFILE_DICOSCT_FORMAT);
  if(removeSuccess != Errors::FILE_OPERATION_SUCCESS)
  {
    ERROR_LOG("Failed to remove: " << (bagFileNamePattern+BAGFILE_DICOSCT_FORMAT).toStdString());
  }

  Utility::removeFile(src+ "/" +bagFileNamePattern+BAGFILE_DICOSCT_PROJ00_FORMAT);

  Utility::removeFile(src+ "/" +bagFileNamePattern+BAGFILE_DICOSCT_PROJ90_FORMAT);

  int i = 1;
  bool tdrFilePresent = true;
  do {
    QString tdrFileName;
    tdrFileName = bagFileNamePattern;
    tdrFileName += BAGFILE_DICOSTDR_FORMAT;
    tdrFileName += QString::number(i);

    tdrFilePresent = Utility::removeFile(src+ "/" +tdrFileName);

    i++;
  }while (tdrFilePresent);
  return removeSuccess;
}

/*!
* @fn       isViewModeEnabled
* @return   bool
* @brief    return if view mode is enabled
*/
bool BagData::isViewModeEnabled()
{
  if ((m_visiblealarmcount > 0) || (m_noAlarmcount > 0))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*!
* @fn       isVRModeVisible
* @return   bool
* @brief    return if vrmode argument needed visible on UI
*/
bool BagData::isVRModeVisible(VREnums::VRViewModeEnum vrmode)
{
  bool alarmThreatPresent =  getTotalEffectiveAvailableAlarmThreats() > 0;
  bool laptopPresent = (m_noAlarmcount > 0);
  if(vrmode == VREnums::VIEW_AS_THREAT)
  {
    if(isTIPBag())
    {
      if(getTotalVisibleAlarmThreats() > 0)
        return true;
    }
    else if(alarmThreatPresent)
    {
      return true;
    }
  }
  else if (vrmode == VREnums::VIEW_AS_LAPTOP)
  {
    if (laptopPresent)
    {
      return true;
    }
  }
  else if (vrmode == VREnums::VIEW_AS_SURFACE)
  {
    return true;
  }
  else if (vrmode == VREnums::VIEW_AS_SLAB)
  {
    if(!isDetectionException())
    {
      return true;
    }
  }
  return false;
}

/*!
* @fn       isCurrentSelectedEntityMachineThreat
* @return   bool
* @brief    return if selected entity is machine threat
*/
bool BagData::isCurrentSelectedEntityMachineThreat()
{
  if( (m_CurrentThreatType == VREnums::THREATTYPE_ATR_THREAT) &&
      (getCurrentVRViewmode() == VREnums::VIEW_AS_THREAT) &&
      !isCurrentThreatOperator())
  {
    return true;
  }
  return false;
}

/*!
* @fn       isNextThreatAvailable
* @return   bool
* @brief    return if next threat is available
*/
bool BagData::isNextThreatAvailable()
{
  return (getCurrentVisibleEntityCount() > 1)?true:false;
}

/*!
 * @fn       canApplyClearDecisionOnCurrentThreat
 * @return   bool
 * @brief    return if clear decision applied on current threat or not
 */
bool BagData::canApplyClearDecisionOnCurrentThreat()
{
  bool res = true;
  if ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::OSR_WORKSTATION)
      && (isTIPBag() == false))
  {
    if (isCurrentSelectedEntityMachineThreat())
    {
      if(WorkstationConfig::getInstance()->getCanUserClearAlarmBag() == false)
      {
        res = false;
      }
    }
    int sliceCount = getSliceCountReceived();
    if (sliceCount < WorkstationConfig::getInstance()->getMinimumRequiredSliceCount())
    {
      ERROR_LOG("Bag slice count is less than minimum required count." <<
                "Current slice count = " << sliceCount << " Required count= " <<
                WorkstationConfig::getInstance()->getMinimumRequiredSliceCount())
          res = false;
    }
  }
  return res;
}

/*!
* @fn       canApplyDecisionOnCurrentThreat
* @return   bool
* @brief    return if decision applied on current threat or not
*/
bool BagData::canApplyDecisionOnCurrentThreat()
{
  bool res = false;
  QMLEnums::ScreenEnum currentscreen = UILayoutManager::getUILayoutManagerInstance()->getScreenState();
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG
      flag = getCurrentThreatUserDecision(static_cast<unsigned int>(m_currrentThreat));

  if (isCurrentSelectedEntityMachineThreat())
  {
    if(flag == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown)
    {
      res = true;
    }
  }
  else if((m_currrentThreat != -1) &&
          (currentscreen == QMLEnums::SEARCH_BAG_SCREEN) &&
          isCurrentThreatOperator())
  {
    if(flag == SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG::enumUnknown)
    {
      res = true;
    }
  }
  return res;
}


/*!
* @fn       getCurrentVisibleEntityCount
* @return   int
* @brief    return total number of visible entity(Machine Threat/ Operator Threat/ Laptop)
*/
int BagData::getCurrentVisibleEntityCount()
{
  int count = 0;
  // Set Total Currently visile threat/laptop
  if(getCurrentVRViewmode() == VREnums::VIEW_AS_LAPTOP)
  {
    count = getTotalNoAlarmThreats();
  }
  else if ((getCurrentVRViewmode() == VREnums::VIEW_AS_THREAT) ||
           (getCurrentVRViewmode() == VREnums::VIEW_AS_SURFACE) ||
           (getCurrentVRViewmode() == VREnums::VIEW_AS_SLAB))
  {
    if (getCurrentSlabViewType() == VREnums::LAPTOP_VIEW)
    {
      count = getTotalNoAlarmThreats();
    }
    else
      count = getTotalVisibleAlarmThreats();
  }
  else
  {
    count = getTotalAvailableThreats();
  }

  INFO_LOG("Currently Visible Total Threat/non Threat count : " << count);
  return count;
}

/*!
* @fn       isTIPBag
* @param    void
* @return   bool
* @brief    identify if its tip bag.
*/
bool BagData::isTIPBag() const
{
  return m_isTIPBag;
}

/*!
* @fn       setTIPBag
* @param    bool
* @return   void
* @brief    set TIP Bag
*/
void BagData::setTIPBag(const bool tipBag)
{
  m_isTIPBag = tipBag;
}

/*!
* @fn       addSliceDataToQueueForDecompression
* @param    void* buffer
* @param    size_t length
* @return   None
* @brief    Add slice data buffer to queue for de-compression
*
*/
void BagData::addSliceDataToQueueForDecompression(void *buffer, size_t length)
{
  DEBUG_LOG("Adding slice to queue[" << length << "]");
  SliceDataStruct* slice = new SliceDataStruct();
  memcpy(slice->buffer() , buffer, length);
  slice->setLength(length);
  int sliceCount = getSliceCountReceived();
  slice->setSliceNo(sliceCount);
  enqueueSlice(slice);
  setSliceCountReceived(++sliceCount);
  emit compressedSliceReceived();
}

/*!
 * @fn       isDetectionException
 * @return   bool
 * @brief    gets detection exception
 */
bool BagData::isDetectionException()
{
  return m_detectionException;
}

/*!
 * @fn       setDetectionException
 * @param    bool
 * @brief    sets detection exception
 */
void BagData::setDetectionException(bool val)
{
  m_detectionException = val;
}

/*!
 * @fn       toggleShowAllThreat
 * @return   None
 * @brief    toggles show all threat flag.
 */
void BagData::toggleShowAllThreat()
{
  m_showAllThreat = !m_showAllThreat;
  refreshAttributes();

  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG userDecision;
  int totalAvailableAlarmThreats = getTotalAvailableAlarmThreats();
  if(m_showAllThreat)
  {
    if(totalAvailableAlarmThreats > 0)
    {
      setCurrentVisibleEntityIndex(0);
      setCurrentThreat(0);
    }
  }
  else
  {
    for(int i = 0; i < totalAvailableAlarmThreats; i++)
    {
      userDecision = getUserDecisionfromPrimaryTdr(i);
      if(userDecision != SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::enumNoThreat)
      {
        setCurrentVisibleEntityIndex(0);
        setCurrentThreat(i);
        break;
      }
    }
  }
}

/*!
   * @fn       getCurrentThreatAlgorithm
   * @param    None
   * @return   QString
   * @brief    return current bag threat detection algorithm.
   */
QString BagData::getCurrentThreatAlgorithm()
{
  m_currentThreatAlgorithm ="Unknown";
  SDICOS::DcsString strVersionNumber;
  SDICOS::DcsString strManufacturer;
  SDICOS::Array1D<SDICOS::DcsString> arrayControlParameters;
  if(m_tdrmodule.size() > 0)
  {
    for(unsigned int count = 0; count < m_tdrmodule.size(); count++)
    {
      SDICOS::TDRTypes::ThreatDetectionReport& threat_report = m_tdrmodule[count]->GetThreatDetectionReport();

      if(threat_report.GetTdrType() == SDICOS::TDRTypes::ThreatDetectionReport
         ::enumMachine)
      {
        threat_report.GetThreatDetectionAlgorithmAndVersion(strManufacturer,
                                                            strVersionNumber,
                                                            arrayControlParameters);
        break;
      }
    }
    INFO_LOG("Getting current Algorithm from the TDR : "<<QString::fromLatin1(strVersionNumber.Get()).toStdString());
    if(QString::fromLatin1(strVersionNumber.Get()) == "1.1"){ //Check ANSS-1610
      m_currentThreatAlgorithm = "Unknown";
    }
    else{
      m_currentThreatAlgorithm = QString::fromLatin1(strVersionNumber.Get());
    }
  }
  INFO_LOG("Algorithm Version set as  : "<<m_currentThreatAlgorithm.toStdString());
  return m_currentThreatAlgorithm;
}

/*!
   * @fn       getRerunBagFilename
   * @param    None
   * @return   QString
   * @brief    return current bag file name for rerun.
   */
QString BagData::getRerunBagFilename()
{
  return m_rerunBagFilename;
}

}  // end of namespace ws
}  // end of namespace analogic
