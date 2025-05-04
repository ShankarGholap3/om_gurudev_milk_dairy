/*!
* @file     rerunviewerhandler.cpp
* @author   Agiliad
* @brief    This file contains interface, responsible review after detection run
* @date     March, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/ulm/dataobject.h>
#include <analogic/ws/rerun/rerunviewerhandler.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{

namespace ws
{
QList<bagCheckList*> RerunViewerHandler::listOfBags;
QString RerunViewerHandler::m_op_decision_file;
QString RerunViewerHandler::m_oqt_detailreport_file;
QString RerunViewerHandler::m_oqt_summaryreport_file;
/*!
   * @fn       RerunViewerHandler
   * @param    None
   * @return   None
   * @brief    Constructor for class RerunViewerHandler.
   */
RerunViewerHandler::RerunViewerHandler()
{
  TRACE_LOG("");
  m_currentBagIndex = 0;
  // intialize UsbFileSystemModel
  m_usbfsm = new UsbFileSystemModel();
  THROW_IF_FAILED((m_usbfsm == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  m_rerunBagList.reset(new RerunBagList());
  THROW_IF_FAILED((m_rerunBagList.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_uniqueBagFolderPathList.clear();

  m_rerunConfigObj= new RerunConfig();
  THROW_IF_FAILED((m_rerunConfigObj== nullptr)? Errors::E_OUTOFMEMORY : Errors::S_OK);

}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    intialization of baglist and selectionmodel.
*/
void RerunViewerHandler::init()
{
  TRACE_LOG("");
  m_usbfsm->initialize();
}

/*!
* @fn       setOperatorName
* @param    QString
* @return   None
* @brief    set Operator Name in TDR.
*/
void RerunViewerHandler::setOperatorName(QString operatorName)
{
  BagsDataPool *bagpool = BagsDataPool::getInstance();
  if(bagpool)
  {
    for (unsigned int i=0; i < bagpool->getSize(); i++)
    {
      boost::shared_ptr<BagData> bag = bagpool->getBagbyIndex(i);
      if (bag != NULL)
      {
        bag->setOperatorName(operatorName);
      }
    }
  }
}

void RerunViewerHandler::setUserName(QString userName)
{
  INFO_LOG("userName ::"<<userName.toStdString());
  m_userName = userName;
}

/*!
   * @fn       RerunViewerHandler
   * @param    None
   * @return   None
   * @brief    Destructor for class RerunViewerHandler.
   */
RerunViewerHandler::~RerunViewerHandler()
{
  TRACE_LOG("");
  INFO_LOG("deleting instance of Rerun handler");
  SAFE_DELETE(m_usbfsm);
  m_uniqueBagFolderPathList.clear();
}

/*!
* @fn       onViewSelectedBags
* @param    QVariantList
* @param    QModelIndex outputFilePathIndex
* @param    QString opID
* @return   None
* @brief    notifies workstationManager when ViewBags is pressed.
*/
void RerunViewerHandler::onViewSelectedBags(QVariantList selectedBagsList,
                                            QModelIndex outputFilePathIndex,
                                            QString opID)
{
  m_uniqueBagFolderPathList.clear();
  m_selectedBagList.clear();
  m_currentBagIndex = 0;
  m_outputFilePath.clear();
  m_outputFilePath = outputFilePathIndex.data().toString();
  m_outputFilePath.prepend("/");
  QModelIndex parentIndex = outputFilePathIndex.parent();
  while ((parentIndex.isValid()) && (parentIndex.data().toString() != "/")) {
    m_outputFilePath.prepend(parentIndex.data().toString());
    m_outputFilePath.prepend("/");
    outputFilePathIndex = parentIndex;
    parentIndex = outputFilePathIndex.parent();
  }

  foreach (QVariant var, selectedBagsList) {
    m_selectedBagList << var.toString();
  }
  m_currentlyViewedBag = m_selectedBagList[0];

  m_op_decision_file.clear();
  m_oqt_summaryreport_file.clear();
  m_oqt_detailreport_file.clear();
  QString decisionFoleder = m_currentlyViewedBag.section("/", 0, -2);
  if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::RERUN_EMULATOR)
  {
    m_op_decision_file = decisionFoleder + "/Operator_Results_";
    m_op_decision_file = m_op_decision_file + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".txt";
  }
  else if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION) {
    m_op_decision_file = m_rerunConfigObj->getOperatorResultPath();
    if(!dirExists(m_op_decision_file))
    {
      QDir dir(m_op_decision_file);
      dir.mkdir(m_rerunConfigObj->getOperatorResultPath());
    }

    m_oqt_detailreport_file = m_oqt_summaryreport_file = m_rerunConfigObj->getReportResultPath(m_userName);
    if(!dirExists(m_oqt_detailreport_file))
    {
      QDir dir(m_oqt_detailreport_file);
      dir.mkdir(m_oqt_detailreport_file);
      QDir dir1(m_oqt_summaryreport_file);
      dir1.mkdir(m_oqt_summaryreport_file);

    }

    m_oqt_detailreport_file = m_oqt_detailreport_file + "/" + m_usbfsm->getPlayListName()+"_"+m_userName+"_OQT_Detail_Report_";
    m_oqt_detailreport_file = m_oqt_detailreport_file + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".csv";
    m_oqt_summaryreport_file = m_oqt_summaryreport_file + "/" +m_usbfsm->getPlayListName() +"_"+m_userName+"_OQT_Summary_Report_";
    m_oqt_summaryreport_file = m_oqt_summaryreport_file + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".csv";

    INFO_LOG(" OQT_detailreport_file -"<<m_oqt_detailreport_file.toStdString());
    INFO_LOG(" OQT_summaryreport_file -"<<m_oqt_summaryreport_file.toStdString());

    if(!dirExists(m_op_decision_file))
    {
      QDir dir(m_op_decision_file);
      dir.mkdir(m_op_decision_file);
    }
    setOperatorName(opID);
    m_opID = opID;
    m_op_decision_file = m_op_decision_file + "/" + opID + "_Operator_Results_";
    m_op_decision_file = m_op_decision_file + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".txt";
  }

  emit notifyToChangeOrNotScreen(true, "", Errors::S_OK);

  if (!getBagFromPool(m_selectedBagList[0])) {
    showNextBag();
  }
}

/*!
* @fn       showNextBag
* @param    None
* @return   None
* @brief    sest next bag in Bagdata.
*/
void RerunViewerHandler::showNextBag()
{
  // checks if next bag is present  or not
  m_currentBagIndex++;
  if (checkForLastBag(m_currentBagIndex)) {
    // if bag files could not found, then check for next bag
    m_currentlyViewedBag = m_selectedBagList[m_currentBagIndex];
    if (!getBagFromPool(m_selectedBagList[m_currentBagIndex])) {
      showNextBag();
    }
  }
}

/*!
* @fn       getBagFromPool
* @param    QString bagDataFilePath
* @return   bool
* @brief    gets bag from BagDataPool.
*/
bool RerunViewerHandler::getBagFromPool(QString bagDataFilePath)
{
  //! Checking for enough disk space required to play bag.
  QString tempBagDatapath = bagDataFilePath.section("/", 0 , bagDataFilePath.count("/")-1);
  if(!checkAvailableDiskSpace(tempBagDatapath))
  {
    if(!m_uniqueBagFolderPathList.contains(tempBagDatapath))
    {
      m_uniqueBagFolderPathList.append(tempBagDatapath);
      QString err = tr("Disk space is not sufficient at : ")+ tempBagDatapath;
      ERROR_LOG(err.toStdString() << false);
      emit notifyToChangeOrNotScreen(false, err, Errors::E_FAIL);
    }
    return false;
  }


  int countToCheck = 0;
  int i = 0;
  for (int j = 0; j < RerunViewerHandler::listOfBags.count(); j++)
  {
    if(RerunViewerHandler::listOfBags.at(j)->bagFilename == bagDataFilePath
       && RerunViewerHandler::listOfBags.at(j)->isRendering == false)
    {
      countToCheck = j;
      break;
    }
  }

  DEBUG_LOG("Waiting for lock to mark rendered bag as success.");
  m_bagCheckListMutex.lock();
  DEBUG_LOG("Released lock, Baglist count: " << RerunViewerHandler::listOfBags.count());
  DEBUG_LOG("Acquired lock to mark rendered bag as success.");
  bagCheckList *objBagCheckList = RerunViewerHandler::listOfBags.at(countToCheck);
  if(objBagCheckList != nullptr)
  {
    objBagCheckList->isRendering= true;
  }
  m_bagCheckListMutex.unlock();

  while(i < RerunViewerHandler::listOfBags.count())
  {
    BagConstructionState state = BagConstructionState::CS_PENDING;

    while((i <= RerunViewerHandler::listOfBags.count()) &&
          (countToCheck <= (RerunViewerHandler::listOfBags.count() - 1)))
    {
      QThread::usleep(500);
      BagConstructionState state1 = BagConstructionState::CS_PENDING;
      m_bagCheckListMutex.lock();
      state1 = RerunViewerHandler::listOfBags.at(countToCheck)->isConstructed;
      m_bagCheckListMutex.unlock();
      if (state1 != BagConstructionState::CS_PENDING)
        break;
    }

    state = RerunViewerHandler::listOfBags.at(i)->isConstructed;

    if(state != BagConstructionState::CS_PENDING)
    {

      if(RerunViewerHandler::listOfBags.at(i)->bagFilename == bagDataFilePath && RerunViewerHandler::listOfBags.at(i)->isConstructed == BagConstructionState::CS_FAILED)
      {
        DEBUG_LOG("Error in constructing bag, notify to change screen and populate error dialog");
        boost::shared_ptr<BagData> bag = BagsDataPool::getInstance()->getBagbyState(BagData::ERROR);

        if (i >= (RerunViewerHandler::listOfBags.count() - 1))
        {
          if(bag != nullptr)
            bag->setError(true);
          emit notifyToChangeOrNotScreen(false, tr("Cannot play bag: ") + bagDataFilePath + tr("<br />Trying to show next bag if present or Stopping if it was last."), Errors::E_FAIL);
        }
        else
        {
          if(bag != nullptr)
            bag->setError(true);
          emit notifyToChangeOrNotScreen(true, tr("Cannot play bag: ") + bagDataFilePath + tr("<br />Trying to show next bag if present or Stopping if it was last."), Errors::E_FAIL);
        }

        if (bag != NULL)
        {
          DEBUG_LOG("Setting BagState to EMPTY for: " << bagDataFilePath.toStdString());
          bag.get()->setState(BagData::EMPTY);
        }
        sleep(2);
        return false;
      }
    }
    i++;
  }
  return true;
}

/*!
* @fn       checkForLastBag
* @param    int index
* @return   bool
* @brief    checks if currently viwed bag is last
*/
bool RerunViewerHandler::checkForLastBag(int index)
{
  if (index == m_selectedBagList.count())
  {
    DEBUG_LOG("There is no bag to show, notify to stop showing bags");
    emit notifyToStopShowingBag();
    createOQTSummaryReport();
    return false;
  }
  else if (index > m_selectedBagList.count())
  {
    DEBUG_LOG("There is no bag to show, notify to stop showing bag");
    emit notifyToStopShowingBag();
    createOQTSummaryReport();
    return false;
  }

  return true;
}

/*!
* @fn       checkAvailableDiskSpace
* @param    QString
* @return   bool
* @brief    check enough space available on drive
*/
bool RerunViewerHandler::checkAvailableDiskSpace(QString bagDataFilePath)
{
  int64_t defaultRequiredSpace = RerunConfig::getDefaultRequiredSpace();
  int64_t availableSpace = Utility::getAvailableDiskSpace(bagDataFilePath);
  if((availableSpace != -1)
     && (availableSpace < (defaultRequiredSpace*1024*1024)))
  {
    return false;
  }
  else
  {
    return true;
  }
}

/*!
* @fn       getUsbFileSystemModel
* @param    None
* @return   UsbFileSystemModel*
* @brief    gives UsbFileSystemModel object
*/
UsbFileSystemModel* RerunViewerHandler::getUsbFileSystemModel() {
  return m_usbfsm;
}


/*!
* @fn       saveOperatorDecision
* @param    QString bagId
* @param    QMLEnums::ThreatEvalPanelCommand decision
* @return   None
* @brief    save operator decision
*/
void RerunViewerHandler::saveOperatorDecision(QString bagId,
                                              QMLEnums::ThreatEvalPanelCommand decisionEnum,
                                              float timeToTakeDecision, QDateTime renderStartTime)
{
  if(decisionEnum!=QMLEnums::ThreatEvalPanelCommand::TEPC_STOP_BAG)
  {
    QString operatorDecision;
    QDateTime tempdatetime = renderStartTime;
    QString date = tempdatetime.toString("yyyy-MM-dd");
    QString time = tempdatetime.toString("hh:mm:ss");
    if(m_currentBagIndex==0)
    {//storing 1st bag's renderd time from the selected playlist
      m_initialBagRenderTime=time;
      m_bagRederedDate=date;

    }
    if (decisionEnum == QMLEnums::TEPC_CLEAR_BAG)
    {
      operatorDecision = tr("Clear");
    }
    else if (decisionEnum == QMLEnums::TEPC_SUSPECT_BAG)
    {
      operatorDecision = tr("Suspect");
    }
    else if (decisionEnum == QMLEnums::TEPC_TIMEOUT_BAG)
    {
      operatorDecision = tr("Timeout");
    }
    m_totalTimeOfTest+=timeToTakeDecision;
    QString operatorDecisionTime = QString::number(timeToTakeDecision / 1000.0, 'f', 2);
    QString bagid = m_currentlyViewedBag;
    bagid = bagid.section("/", -1, -1);
    QString decision = bagid + ", " + operatorDecision + ", " + operatorDecisionTime + "\n";

    operatorDecisionTime=QDateTime::fromTime_t(timeToTakeDecision/1000).toUTC().toString("hh:mm:ss");

    if (Utility::appendToFile(m_op_decision_file, decision))
    {
      DEBUG_LOG("save decision success");
    }
    else
    {
      DEBUG_LOG("save decision failed");
    }
    INFO_LOG("m_op_decision_file"<<m_op_decision_file.toStdString());
    INFO_LOG("m_oqt_detailreport_file"<<m_oqt_detailreport_file.toStdString());
    INFO_LOG("m_oqt_detailreport_file"<<m_oqt_summaryreport_file.toStdString());
    m_playlistname=m_usbfsm->getPlayListName();
    QString Bag_ID = bagid.section("_", -1, -1);

    QMap<QString,QString> playlistCategorizeFileDetails=m_usbfsm->getPlaylistDetails();
    QString categoryAndCorrectDecion=playlistCategorizeFileDetails.value(Bag_ID);
    QStringList data=categoryAndCorrectDecion.split(",");
    QString Category=data[1];
    QString ExpectedDecision=data[2];
    QString Result;
    if(QString::compare(operatorDecision, data[2], Qt::CaseInsensitive)==0){
      Result=tr("Correct");
    }else
    {
      Result=tr("InCorrect");
    }

    //Checking for Total Correct decison
    if(Result=="Correct")
    {
      m_score += 1.0;
    }
    //Checking for Correct decion on Threat  and False Alarm Image
    if((Result=="Correct")&&(Category!="False Alarm"))
    {
      m_pd +=1.0;
    }
    else if ((Result=="Correct")&&(Category=="False Alarm"))
    {
      m_pfa +=1.0;
    }

    QString SiteCode= WorkstationConfig::getInstance()->getMachineLocation();
    QString oqt_detailreport_file_header="";
    if(m_currentBagIndex==0)
    {
      oqt_detailreport_file_header="User ID, Users Name,Site Code, Date, Time,Test Name, Bag ID, Category, Elapsed Time, User Decision, Expected, Result\n";

      totalFalseAlarmAndThreatImages();
    }
    QString oqt_detailreport_file= m_userName+","+ m_opID+","+ SiteCode +","+ date +","+time +","+m_playlistname+" ,"+Bag_ID +" , "+Category  +", "+operatorDecisionTime +", "  + operatorDecision + ","+ ExpectedDecision+" ,"+ Result +"\n";
    if(Utility::appendToFile(m_oqt_detailreport_file,oqt_detailreport_file_header+oqt_detailreport_file))
    {
      INFO_LOG(" m_oqt_detailreport_file File Saved success");
    }
    else
    {
      INFO_LOG("m_op_decision_file File saving failed");
    }
  }
  else{
    createOQTSummaryReport();

  }

}

/*!
  * @fn       totalFalseAlarmAndThreatImages
  * @param    null
  * @return   None
  * @brief    Calculate Total FalseAlarm and ThreatImages
  *           in Selected Playlist.
  */
void RerunViewerHandler::totalFalseAlarmAndThreatImages()
{
  QMap<QString,QString> playlistCategorizeFileDetails=m_usbfsm->getPlaylistDetails();
  QMap<QString,QString>::iterator i;
  for (i = playlistCategorizeFileDetails.begin(); i != playlistCategorizeFileDetails.end(); i++)
  {
    QString  playlistDecisionLine = i.value();
    QStringList data = playlistDecisionLine.split(",");
    QString Category=data[1];
    //Total number of Threat and false alarm images
    if (Category!="False Alarm"){
      m_pdTotal +=1.0;
    }
    else if(Category=="False Alarm")
    {
      m_pfaTotal +=1.0;
    }


  }
}

/*!
  * @fn       createOQTSummaryReport
  * @param    null
  * @return   None
  * @brief    Update OQT Summary Report and
  *           add Summary line in OQT Detailed Report.
  */
void RerunViewerHandler::createOQTSummaryReport()
{
  if(m_currentBagIndex!=0)
  {
    if(m_pfaTotal==0.0)
    {
      m_pfaTotal=1.0;
    }
    if(m_pdTotal==0.0)
    {
      m_pdTotal=1.0;
    }
    float f_pd=(m_pd/m_pdTotal)*100.0;
    QString pdInPercent = QString::number(f_pd, 'f', 2)+"%";
    float f_pfa=(m_pfa/m_pfaTotal)*100.0;
    QString pfaInPercent = QString::number(f_pfa, 'f', 2)+"%";
    float f_score=(m_score/m_selectedBagList.count())*100.0;
    QString ScoreInPercent =QString::number(f_score, 'f', 2)+"%";

    QString totalTimeOfTest =QDateTime::fromTime_t(m_totalTimeOfTest/1000).toUTC().toString("hh:mm:ss");
    QString SiteCode= WorkstationConfig::getInstance()->getMachineLocation();
    QString oqt_summaryreport_file_header="User ID, Users Name,Site Code, Date, Time, Elapsed Time, Test Name, pd, pfa, Score \n";
    QString oqt_summaryreport_file= m_userName+","+ m_opID+","+ SiteCode +","+ m_bagRederedDate +","+m_initialBagRenderTime +","+m_playlistname+" ,"+totalTimeOfTest +", "  + pdInPercent + ","+ pfaInPercent+" ,"+ ScoreInPercent +"\n";

    if(Utility::appendToFile(m_oqt_summaryreport_file,oqt_summaryreport_file_header+oqt_summaryreport_file))
    {
      INFO_LOG(" m_oqt_summaryreport_file File Saved success");
    }
    else
    {
      INFO_LOG("m_oqt_summaryreport_file File saving failed");
    }
    //append Summary line at end of Detailed Report
    QString oqt_detailreport_file= "Totals: , , , , , ,"+QString::number(m_selectedBagList.count()) +",  , "+totalTimeOfTest +", , ,"+ ScoreInPercent +"\n";
    if(Utility::appendToFile(m_oqt_detailreport_file,oqt_detailreport_file))
    {
      INFO_LOG(" m_oqt_detailreport_file File Saved success");
    }
    else
    {
      INFO_LOG("m_op_decision_file File saving failed");
    }
  }
  //Reset variables
  m_totalTimeOfTest=0;
  m_score=0;
  m_pd=0;
  m_pfa=0;
  m_pdTotal=0;
  m_pfaTotal=0;
  m_initialBagRenderTime="";

}


}  // end of namespace ws
}  // end of namespace analogic
