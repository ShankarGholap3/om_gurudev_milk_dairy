/*!
* @file     bagpreprocessinghandler.cpp
* @author   Agiliad
* @brief    This file contains functions related to
*           bag processing tasks in back ground thread
* @date     Sep, 26 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/bagdata/bagpreprocessinghandler.h>
namespace analogic
{

namespace ws
{
/*!
* @fn       BagPreProcessingHandler
* @param    QSemaphore * semaphore
* @return   None
* @brief    Constructor for class BagPreProcessingHandler.
*/
BagPreProcessingHandler::BagPreProcessingHandler(QSemaphore * semaphore)
{
  m_uncompressed_slice_size = WorkstationConfig::getInstance()->getBagDataResolution().height() *
      WorkstationConfig::getInstance()->getBagDataResolution().width() * 2;  // Max size an uncompressed slice could be
  m_workerSemaphore = semaphore;
  connect(this, &BagPreProcessingHandler::startBagProcessing,
          this, &BagPreProcessingHandler::onStartBagProcessing, Qt::QueuedConnection);
  m_mutex = new QMutex(QMutex::Recursive);
  THROW_IF_FAILED((m_mutex == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
}
/*!
* @fn       ~BagPreProcessingHandler
* @param    None
* @return   None
* @brief    Destructor for class BagPreProcessingHandler.
*/
BagPreProcessingHandler::~BagPreProcessingHandler()
{
  SAFE_DELETE(m_mutex);
}

/*!
* @fn       run
* @param    None
* @return   None
* @brief    overriden function for QThread::run
*/
void BagPreProcessingHandler::onStartBagProcessing()
{
  switch(m_taskName)
  {
  case DECOMPRESSION:
    if (m_bagData &&
        !m_bagData->isSliceQueueEmpty() &&
        m_mutex
        )
    {

      SliceDataStruct *sliceData = NULL;
      m_mutex->lock();
      if(!m_bagData->isSliceQueueEmpty())
      {
        sliceData = m_bagData->dequeueSlice();
      }
      m_mutex->unlock();
      if (sliceData)
      {
        unsigned char uncompressed_buffer[m_uncompressed_slice_size];
        DEBUG_LOG("Decompression started, compressed size = " << sliceData->length());
        JLS_ERROR ret = JpegLsDecode(uncompressed_buffer, m_uncompressed_slice_size,
                                     sliceData->buffer(), sliceData->length(), NULL);
        DEBUG_LOG("Decompression ended");
        if (ret != 0 )
        {
          ERROR_LOG("Error while decompressing slice , ErrorEnum: " << ret);
        }
        else
        {
          DEBUG_LOG("Compressed slice data received: size after decompression [" <<
                   m_uncompressed_slice_size  << "]");
          m_bagData->setVolumeSliceData(uncompressed_buffer,
                                        m_uncompressed_slice_size,
                                        sliceData->SliceNo());
        }
        int totalSlices = m_bagData->getVolumeDimension().z();
        int sliceCount = sliceData->SliceNo()+1;
        SAFE_DELETE(sliceData);

        m_bagData->increamentSliceProcessed();
        if ((sliceCount == 1) || (sliceCount == (totalSlices)))
        {
          INFO_LOG("Slice data decompressed; SliceNo: " << sliceCount << " of "<<totalSlices);
        }
        else
        {
          DEBUG_LOG("Slice data decompressed; SliceNo: " << sliceCount);
        }
      }
    }
    break;
  case COMPRESSION:
    break;
  default:
    break;
  }
  if( m_workerSemaphore)
  {
    m_workerSemaphore->release();
  }
}
/*!
* @fn       setTask
* @param    None
* @return   BagProcessingTaskName
* @brief    get bag processing task
*/
BagProcessingTaskName BagPreProcessingHandler::getTask() const
{
  return m_taskName;
}

/*!
* @fn       setTask
* @param    BagProcessingTaskName
* @return   None
* @brief    set bag processing task
*/
void BagPreProcessingHandler::setTask(BagProcessingTaskName value)
{
  m_taskName = value;
}

/*!
* @fn       getBagData
* @param    None
* @return   BagData * -
* @brief    get bag data
*/
BagData* BagPreProcessingHandler::getBagData() const
{
  return m_bagData;
}

/*!
* @fn       setBagData
* @param    BagData* bagData
* @return   None
* @brief    set bag data
*/
void BagPreProcessingHandler::setBagData(BagData *bagData)
{
  m_bagData = bagData;
}

}  // namespace ws
}  // namespace analogic
