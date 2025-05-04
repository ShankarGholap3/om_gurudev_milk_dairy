/*!
* @file     bagdataqueue.h
* @author   Agiliad
* @brief    This file contains functions related to bag data queue.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_BAGDATA_BAGDATAQUEUE_H_
#define ANALOGIC_WS_COMMON_BAGDATA_BAGDATAQUEUE_H_

#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>

#include <boost/shared_ptr.hpp>
#include <boost/lockfree/queue.hpp>

#include <QMutex>
#include <iostream>
#include <Logger.h>
#include <queue>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{
#define MAX_BAG_ALLOWED 2
#define OSR_BAG_ALLOWED MAX_BAG_ALLOWED

#define SEARCH_BAG_ALLOWED 1

/*!
 * \class   BagsDataPool
 * \brief   This class contains variable and function related to
 *          bag data Pool.
 */
class BagsDataPool
{
public:
  /*!
    * @fn       getInstance
    * @param    None
    * @return   BagsDataPool*
    * @brief    This function creates instance of BagsDataPool and returns pointer to
    *           it.
    */
  static BagsDataPool* getInstance();

  /*!
    * @fn       destroyInstance
    * @param    None
    * @return   None
    * @brief    Destroys BagsDataPool.
    */
  static void destroyInstance();

  /*!
    * @fn       getBagbyState
    * @param    int - state
    * @return   boost::shared_ptr<BagData>
    * @brief    gets Bag by state.
    */
  boost::shared_ptr<BagData> getBagbyState(int state);

  /*!
    * @fn       getBagbyIndex
    * @param    int
    * @return   boost::shared_ptr<BagData>
    * @brief    gets Bag by index.
    */
  boost::shared_ptr<BagData> getBagbyIndex(int index);

  /*!
    * @fn       getSize
    * @param    None
    * @return   unsigned int
    * @brief    gets size.
    */
  unsigned int getSize();

  /*!
    * @fn       isBagdataqueueEmpty
    * @param    None
    * @return   bool
    * @brief    returns empty state of bag data queue.
    */
  bool isBagdataqueueEmpty();

  /*!
    * @fn       getBagQueueCount
    * @param    None
    * @return   None
    * @brief    return no of bag queue
    */
  int getBagQueueCount();

private:
  /*!
    * @fn       BagsDataPool
    * @param    BagsDataPool&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
  BagsDataPool();

  /*!
    * @fn       ~BagsDataPool
    * @param    None
    * @return   None
    * @brief    Destructor for class BagsDataPool.
    */
  ~BagsDataPool();
  static QMutex               m_singletoneLock;                   //!< mutex lock
  static int                  m_maxqueuecount;                    //!< maximum bag allowed
  boost::shared_ptr<BagData>  m_bagdataqueue[OSR_BAG_ALLOWED];    //!< Pool of bag data objects
  static BagsDataPool*        m_instance;                         //!< pointer to static instance of bag data pool
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_BAGDATA_BAGDATAQUEUE_H_

