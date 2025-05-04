#include <analogic/ws/rerun/rerundataacqhandler.h>
#include <analogic/ws/rerun/rerunviewerhandler.h>
#include <analogic/ws/common/bagdata/bagdata.h>


namespace analogic
{

namespace ws
{

int RerunDataAcqHandler::count;

/*!
* @fn       BagDataAcqHandler
* @param    QThread* thread
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class BagDataAcqHandler.
*/
RerunDataAcqHandler::RerunDataAcqHandler()
{
  TRACE_LOG("");
}

/*!
* @fn       initRerunDataAcqHandler
* @param    None
* @return   None
* @brief    init for class BagDataAcqHandler.
*/
void RerunDataAcqHandler::initRerunDataAcqHandler()
{
  m_Thread = new QThread();
  THROW_IF_FAILED((m_Thread == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  // move current object to thread
  this->moveToThread(m_Thread);

  m_Thread->start();
  m_pUpdateTimer  = NULL;
  m_selectedBagList.clear();
  connect (m_Thread, SIGNAL(started()), this, SLOT(onThreadStarted()),
           Qt::QueuedConnection);
  connect (this, &RerunDataAcqHandler::exit, this, &RerunDataAcqHandler::onExit,
           Qt::QueuedConnection);
}

/*!
* @fn       onThreadStarted
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void RerunDataAcqHandler::onThreadStarted()
{
  TRACE_LOG("");
  m_bagqueue = BagsDataPool::getInstance();
  DEBUG_LOG("RerunDataAcqHandler::BagQueue handle used: " << m_bagqueue);

  // creating new qtimer instance
  m_pUpdateTimer = new QTimer(this);
  THROW_IF_FAILED((m_pUpdateTimer == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);
  // setting timer interval
  m_pUpdateTimer->setInterval(1000);
  // connecting timer events to proper slot
  DEBUG_LOG("RerunDataAcqHandler::Connecting getBagData() call, in construction threads");
  connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(getBagData()));
  connect(this, SIGNAL(viewbags()), this, SLOT(getBagData()));
  m_pUpdateTimer->start();

  Semaphore::getInstance()->release(1);
}

/*!
* @fn       getBagData
* @param    None
* @return   None
* @brief    get bag data.
*/
void RerunDataAcqHandler::getBagData()
{
  TRACE_LOG("");
  boost::shared_ptr<BagData> bag;

  if (m_bagqueue != NULL && RerunViewerHandler::listOfBags.count() > 0)
  {
    for(int i = 0; i < RerunViewerHandler::listOfBags.count(); i++)
    {
      bag = m_bagqueue->getBagbyState(BagData::DISPOSED | BagData::EMPTY);
      if(bag != NULL )
      {
        if(count == RerunViewerHandler::listOfBags.count())
        {
          return;
        }
        if(RerunViewerHandler::listOfBags.count() && i < (RerunViewerHandler::listOfBags.count()) )
        {
          bool success = false;
          if (WorkstationConfig::getInstance()->getWorkstationNameEnum()
              == QMLEnums::WSType::RERUN_EMULATOR)
          {
            success = bag->constructFromFiles( RerunViewerHandler::listOfBags.at(count)->bagFilename, m_outputFilePath);
          }
          else if (WorkstationConfig::getInstance()->getWorkstationNameEnum()
                   == QMLEnums::WSType::TRAINING_WORKSTATION)
          {
            int start = ( RerunViewerHandler::listOfBags.at(count)->bagFilename.count("/")+1) * -1;
            QString tdrFilePath = RerunViewerHandler::listOfBags.at(count)->bagFilename.section("/", start, -2);
            INFO_LOG("Bag is constructing: " << RerunViewerHandler::listOfBags.at(count)->bagFilename.toStdString());
            success = bag->constructFromFiles(RerunViewerHandler::listOfBags.at(count)->bagFilename, tdrFilePath);
            INFO_LOG("Bag " << RerunViewerHandler::listOfBags.at(count)->bagFilename.toStdString()
                     << " has been constructed with result: " << success);
          }
          if(!success)
          {
            DEBUG_LOG("Error in constructing bag: "<< RerunViewerHandler::listOfBags.at(count)->bagFilename.toStdString()
                      << ". Waiting for renderer to finish..." );
            while(m_bagqueue->getBagbyIndex(0).get()->getState() == BagData::RENDERING ||
                  m_bagqueue->getBagbyIndex(1).get()->getState() == BagData::RENDERING )
              QThread::usleep(500);
            DEBUG_LOG("Waiting for lock to initialize bag construction state");
            m_bagCheckListMutex.lock();
            DEBUG_LOG("Acquired lock, Failed to construct bag, changing construction state.");
            RerunViewerHandler::listOfBags.at(count)->isConstructed = BagConstructionState::CS_FAILED;   //failed
            bag.get()->setState(BagData::ERROR);
            m_bagCheckListMutex.unlock();
            DEBUG_LOG("Released lock, Failed to construct bag and BagState changed to ERROR.");
          }
          else
          {
            m_bagCheckListMutex.lock();
            RerunViewerHandler::listOfBags.at(count)->isConstructed = BagConstructionState::CS_SUCCESS;  // success
            m_bagCheckListMutex.unlock();
          }
          DEBUG_LOG("Bag: " << RerunViewerHandler::listOfBags.at(count)->bagFilename.toStdString()
                    << ", State: " << RerunViewerHandler::listOfBags.at(count)->isConstructed);
          count++;
        }
      }
    }
  }
  else
  {
    DEBUG_LOG("Bagqueue is NULL");
  }
}

/*!
* @fn       onViewSelectedBags
* @param    QVariantList
* @param    QModelIndex outputFilePathIndex
* @param    QString opID
* @return   None
* @brief    notifies workstationManager when ViewBags is pressed.
*/
void RerunDataAcqHandler::onViewSelectedBags(QVariantList selectedBagsList,
                                             QModelIndex outputFilePathIndex,
                                             QString opID)
{
  m_bagCheckListMutex.lock();
  RerunViewerHandler::listOfBags.clear();
  m_selectedBagList.clear();
  foreach (QVariant var, selectedBagsList) {
    m_selectedBagList << var.toString();
    bagCheckList *bag = new bagCheckList;
    bag->bagFilename = var.toString();
    bag->isConstructed = BagConstructionState::CS_PENDING;
    bag->isRendering = false;
    RerunViewerHandler::listOfBags.append(bag);
  }
  INFO_LOG("Selected View list count: " << RerunViewerHandler::listOfBags.count());
  m_bagCheckListMutex.unlock();

  BagsDataPool* bagqueue = BagsDataPool::getInstance();
  for (int i=0; i<bagqueue->getBagQueueCount(); i++) {
    boost::shared_ptr<BagData> bagData = bagqueue->getBagbyState(
          BagData::READYTORENDER | BagData::RENDERING |
          BagData::CONSTRUCTION | BagData::INITIALISED | BagData::ERROR);
    if(bagData != NULL) {
      bagData->setState(BagData::DISPOSED);
    }
  }

  m_currentBagIndex = 0;
  count = 0;
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

  emit notifyToChangeOrNotScreen(true, "", Errors::S_OK);

  INFO_LOG("started view bags on selection baglist.");
  emit viewbags();
}

/*!
* @fn       ~BagDataAcqHandler
* @param    None
* @return   None
* @brief    Destructor for class BagDataAcqHandler.
*/
RerunDataAcqHandler::~RerunDataAcqHandler()
{
  TRACE_LOG("");
}

/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    calls on exit
*/
void RerunDataAcqHandler::onExit()
{
  TRACE_LOG("");

  if (m_pUpdateTimer != NULL)
  {
    m_pUpdateTimer->stop();

    SAFE_DELETE(m_pUpdateTimer);
  }
  if (NULL != m_bagqueue)
  {
    BagsDataPool::destroyInstance();
    m_bagqueue = NULL;
  }
  Semaphore::getInstance()->release(1);
}
}
}
