/*!
* @file     dataprocessingservice.h
* @author   Agiliad
* @date     Sep, 29 2016
* @brief    This class contains functionality related to bag data processing. This
*           class run on background thread.
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef DATADECOMPRESSIONSERVICE_H
#define DATADECOMPRESSIONSERVICE_H

#include <QTimer>
#include <QObject>
#include <QThread>
#include <QQueue>
#include <Logger.h>
#include <semaphore.h>
#include <boost/shared_ptr.hpp>
#include "CharLS/interface.h"
#include "CharLS/util.h"
#include <analogic/ws/common.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>
#include <analogic/ws/common/bagdata/bagpreprocessinghandler.h>
#include <QSemaphore>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{
/*!
 * \class   DataProcessingService
 * \brief   This Class takes care of decompression of slice data
 */
class DataProcessingService:public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       DataProcessingService
    * @param    QThread* - thread
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class BagDataAcqHandler.
    */
    explicit DataProcessingService(QObject *parent = NULL);

    /*!
    * @fn       ~DataProcessingService
    * @param    None
    * @return   None
    * @brief    Destructor for class DataDecompressionService.
    */
    virtual ~DataProcessingService();

    /*!
    * @fn       init
    * @param    None
    * @return   None
    * @brief    init the memeber data
    */
    void init();

signals:
    /*!
    * @fn       exit
    * @param    None
    * @return   None
    * @brief    signals exit
    */
    void exit();



public slots:
    /*!
    * @fn       consumeQueue
    * @param    None
    * @return   None
    * @brief    consume the slice data queue for decompression
    */
    void consumeQueue();

    /*!
    * @fn       onExit
    * @param    None
    * @return   None
    * @brief    calls on exit
    */
    void onExit();
protected:

    uint32_t                           m_uncompressed_slice_size;
    BagsDataPool                       * m_bagqueue;
    BagData                            * data;
    QTimer                             * m_pUpdateTimer;  //!< handle for timer
    QSemaphore                         * m_workerSemaphore;
    BagPreProcessingHandler            ** m_bagprocessingHandler;
    QThread                            ** m_bag_processing_threads;
    int                                m_threatCount;
};
}
}


#endif // DATADECOMPRESSIONSERVICE_H
