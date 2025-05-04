/*!
* @file     bagdataqueue.cpp
* @author   Agiliad
* @brief    This file contains functions related to bag data queue.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/bagdata/bagdataqueue.h>


namespace analogic
{

namespace ws
{
BagsDataPool* BagsDataPool::m_instance = NULL;
QMutex  BagsDataPool::m_singletoneLock;
int BagsDataPool::m_maxqueuecount;

/*!
* @fn       BagsDataPool
* @param    BagsDataPool&
* @return   None
* @brief    declaration for private copy constructor.
*/
BagsDataPool::BagsDataPool()
{
  if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::SEARCH_WORKSTATION ||
     WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::SUPERVISOR_WORKSTATION)
  {
    m_maxqueuecount = SEARCH_BAG_ALLOWED;
  }

  else if (WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::OSR_WORKSTATION)
  {
    m_maxqueuecount = OSR_BAG_ALLOWED;
  }
  else
  {
    m_maxqueuecount = MAX_BAG_ALLOWED;
  }
  DEBUG_LOG("Creating pool of bagdata objects");

  for(int i = 0; i < m_maxqueuecount; i++)
  {
    m_bagdataqueue[i].reset(new BagData());
  }
}

/*!
* @fn       ~BagsDataPool
* @param    None
* @return   None
* @brief    Destructor for class BagsDataPool.
*/
BagsDataPool::~BagsDataPool()
{
  for(int i = 0; i < m_maxqueuecount; i++)
  {
    m_bagdataqueue[i].reset();
  }
}

/*!
* @fn       getBagbyState
* @param    int
* @return   boost::shared_ptr<BagData>
* @brief    gets Bag by state.
*/
boost::shared_ptr<BagData> BagsDataPool::getBagbyState(int state)
{
  boost::shared_ptr<BagData> bag = NULL;
  QDateTime oldestbagtime = QDateTime::currentDateTime();
  for (int i=0; i< m_maxqueuecount; i++)
  {
    if(m_bagdataqueue[i]->getState() & state)
    {
      if(state != BagData::READYTORENDER)
      {
        DEBUG_LOG("Returning bag with state: " << state << ", BagId: "
                  << m_bagdataqueue[i]->getBagid() << ", Bag number in queue: " << i);
        return m_bagdataqueue[i];
      }
      else
      {
        QDateTime currentbagtime = m_bagdataqueue[i]->getBagReadytoRenderTime();
        if (oldestbagtime >= currentbagtime)
        {
          oldestbagtime = currentbagtime;
          bag = m_bagdataqueue[i];
        }
      }
    }
  }
  return bag;
}

/*!
* @fn       getBagbyIndex
* @param    int
* @return   boost::shared_ptr<BagData>
* @brief    gets Bag by index.
*/
boost::shared_ptr<BagData> BagsDataPool::getBagbyIndex(int index)
{
  boost::shared_ptr<BagData> data;
  if((index < 0) || (index > m_maxqueuecount))
  {
    return data;
  }
  else
  {
    return m_bagdataqueue[index];
  }
}

/*!
* @fn       isBagdataqueueEmpty
* @param    None
* @return   bool
* @brief    returns empty state of bag data queue.
*/
bool BagsDataPool::isBagdataqueueEmpty()
{
  bool result = false;
  for(int i = 0; i < m_maxqueuecount; i++)
  {
    if (m_bagdataqueue[i]->isBagEmpty())
    {
      result = true;
      continue;
    }

    else
    {
      result = false;
      break;
    }
  }
  return result;
}

/*!
* @fn       getSize
* @param    None
* @return   unsigned int
* @brief    gets size.
*/
unsigned int BagsDataPool::getSize()
{
  return 2;
}

/*!
* @fn       getInstance
* @param    None
* @return   static boost::shared_ptr<BagsDataPool>
* @brief    This function creates instance of BagsDataPool and returns pointer to
*           it.
*/
BagsDataPool* BagsDataPool::getInstance()
{
  m_singletoneLock.lock();
  if(!m_instance)
  {
    DEBUG_LOG("Creating static instance of BagDataPool");
    m_instance = new BagsDataPool();
    THROW_IF_FAILED((m_instance == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
  }
  m_singletoneLock.unlock();
  return m_instance;
}

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    Destroys BagsDataPool.
*/
void BagsDataPool::destroyInstance()
{
  TRACE_LOG("");
  if(m_instance)
  {
    INFO_LOG("Destroying BagDataPool");
    delete m_instance;
    m_instance = NULL;
  }
}

/*!
* @fn       getBagQueueCount
* @param    None
* @return   None
* @brief    return no of bag queue
*/
int BagsDataPool::getBagQueueCount()
{
  return m_maxqueuecount;
}
}  // end of namespace ws
}  // end of namespace analogic

