/*!
* @file     bagpreprocessinghandler.h
* @author   Agiliad
* @brief    This file contains functions related to
*           bag processing task in back ground thread
* @date     Sep, 26 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_COMMON_BAGDATA_BAGPREPROCESSINGHANDLER_H_
#define ANALOGIC_WS_COMMON_BAGDATA_BAGPREPROCESSINGHANDLER_H_
#include <QThread>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <CharLS/interface.h>
#include <CharLS/util.h>
#include <QSemaphore>
#include <QMutex>
namespace analogic
{

namespace ws
{

/*!
 * \enum   BagProcessingTaskName
 * \brief   This enum contains various bag data preprocessing task.
 */
enum BagProcessingTaskName
{
  COMPRESSION,
  DECOMPRESSION
};

/*!
 * \class   BagPreProcessingHandler
 * \brief   This class contains variable and function related to
 *           bag data processing
 */
class BagPreProcessingHandler:public QObject
{
  Q_OBJECT

public:
  /*!
    * @fn       BagPreProcessingHandler
    * @param    QSemaphore * semaphore
    * @return   None
    * @brief    Constructor for class BagPreProcessingHandler.
    */
  explicit BagPreProcessingHandler(QSemaphore * semaphore);

  /*!
    * @fn       ~BagPreProcessingHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class BagPreProcessingHandler.
    */
  virtual ~BagPreProcessingHandler();


  /*!
    * @fn       getCompressDecompress
    * @param    None
    * @return   BagProcessingTaskName
    * @brief    get bag processing task name
    */
  BagProcessingTaskName getTask() const;

  /*!
    * @fn       setCompressDecompress
    * @param    BagProcessingTaskName
    * @return   None
    * @brief    set bag processing task name
    */
  void setTask(BagProcessingTaskName value);

  /*!
    * @fn       getBagData
    * @param    None
    * @return   BagData * -
    * @brief    get bag data
    */
  BagData* getBagData() const;

  /*!
    * @fn       setBagData
    * @param    BagData* bagData
    * @return   None
    * @brief    set bag data
    */
  void setBagData(BagData* bagData);

signals:
  /*!
    * @fn       startBagProcessing
    * @param    None
    * @return   None
    * @brief    start bag processing
    */
  void startBagProcessing();

public slots:
  /*!
    * @fn       onStartBagProcessing
    * @param    None
    * @return   None
    * @brief    start bag processing
    */
  void onStartBagProcessing();
private:
  BagProcessingTaskName   m_taskName;                        //! bag processing task name
  BagData                 * m_bagData;                       //! Bag data pointer
  QSemaphore              * m_workerSemaphore;               //! semaphore
  uint32_t                m_uncompressed_slice_size;         //! uncompress slice size
  QMutex                  * m_mutex;
};
}  // namespace ws
}  // namespace analogic
#endif  // ANALOGIC_WS_COMMON_BAGDATA_BAGPREPROCESSINGHANDLER_H_
