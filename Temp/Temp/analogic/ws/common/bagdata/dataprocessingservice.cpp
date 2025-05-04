/*!
* @file     dataprocessingservice.cpp
* @author   Agiliad
* @date     Sep, 29 2016
* @brief    This class contains functionality related to bag data processing. This
*           class run on background thread.
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/bagdata/dataprocessingservice.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{

/*!
* @fn       DataProcessingService
* @param    QThread* - thread
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class BagDataAcqHandler.
*/
DataProcessingService::DataProcessingService(QObject *parent):
    QObject(parent)
{
    TRACE_LOG("");
    m_pUpdateTimer  = NULL;
    m_bagprocessingHandler=NULL;
    m_threatCount=-1;

}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    init the memeber data
*/
void DataProcessingService::init()
{
    TRACE_LOG("");
    m_bagqueue = BagsDataPool::getInstance();
    m_uncompressed_slice_size = WorkstationConfig::getInstance()->getBagDataResolution().height() *
            WorkstationConfig::getInstance()->getBagDataResolution().width() * 2;  // Max size an uncompressed slice could be

    m_workerSemaphore = new QSemaphore(WorkstationConfig::getInstance()->getNoofBagPreprocessingThreads());

    m_bagprocessingHandler = new BagPreProcessingHandler*[WorkstationConfig::getInstance()->getNoofBagPreprocessingThreads()];
    THROW_IF_FAILED((m_bagprocessingHandler == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

    m_bag_processing_threads = new QThread*[WorkstationConfig::getInstance()->getNoofBagPreprocessingThreads()];
    THROW_IF_FAILED((m_bag_processing_threads == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);

    for (int i=0; i<WorkstationConfig::getInstance()->getNoofBagPreprocessingThreads(); i++)
    {
        m_bag_processing_threads[i] = new QThread();
        THROW_IF_FAILED((m_bag_processing_threads[i] == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
        m_bag_processing_threads[i]->start();

        m_bagprocessingHandler[i] = new BagPreProcessingHandler(m_workerSemaphore);
        THROW_IF_FAILED((m_bagprocessingHandler[i] == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
        m_bagprocessingHandler[i]->moveToThread(m_bag_processing_threads[i]);

    }    
}

/*!
* @fn       ~DataProcessingService
* @param    None
* @return   None
* @brief    Destructor for class DataDecompressionService.
*/
DataProcessingService::~DataProcessingService()
{
    TRACE_LOG("");
}

/*!
* @fn       consumeQueue
* @param    None
* @return   None
* @brief    consume the slice data queue for decompression
*/
void DataProcessingService::consumeQueue()
{
    if(m_bagqueue == NULL)
    {
        ERROR_LOG("Bag data queue instance can not be null.");
        return;
    }
    for (unsigned int i=0; i<m_bagqueue->getSize(); i++)
    {
        data = m_bagqueue->getBagbyIndex(i).get();
        if (data != NULL)
        {
            BagData::BagState state=data->getState();
            if (state == BagData::CONSTRUCTION)
            {
                if (!data->isSliceQueueEmpty()
                        && m_workerSemaphore)
                {

                    while(true)
                    {
                        m_threatCount++;
                        if (m_threatCount == WorkstationConfig::getInstance()->getNoofBagPreprocessingThreads())
                        {
                            m_threatCount=0;
                        }
                        if (m_bagprocessingHandler[m_threatCount])
                        {
                            m_workerSemaphore->acquire();
                            m_bagprocessingHandler[m_threatCount]->setBagData(data);
                            m_bagprocessingHandler[m_threatCount]->setTask(BagProcessingTaskName::DECOMPRESSION);
                            emit m_bagprocessingHandler[m_threatCount]->startBagProcessing();
                            break;
                        }
                    }
                }
                else
                {
                    ERROR_LOG("slice data queue is empty");
                }
            }
            else
            {
                DEBUG_LOG("Bag state is: "<<state);
            }
        }
    }
}


/*!
* @fn       exit
* @param    None
* @return   None
* @brief    signals onexit
*/
void DataProcessingService::onExit()
{
    for(int i=0; i<WorkstationConfig::getInstance()->getNoofBagPreprocessingThreads(); i++)
    {
        SAFE_DELETE(m_bagprocessingHandler[i])
    }
    SAFE_DELETE(m_bagprocessingHandler);

    for(int i=0; i<WorkstationConfig::getInstance()->getNoofBagPreprocessingThreads(); i++)
    {
        SAFE_DELETE(m_bag_processing_threads[i])
    }
    SAFE_DELETE(m_bag_processing_threads);
}
}
}
