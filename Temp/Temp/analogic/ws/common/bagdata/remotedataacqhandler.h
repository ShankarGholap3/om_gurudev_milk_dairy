/*!
* @file     remotedataacqhandler.h
* @author   Agiliad
* @brief    This file contains interface, responsible for managing remote bag data.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_BAGDATA_REMOTEDATAACQHANDLER_H_
#define ANALOGIC_WS_COMMON_BAGDATA_REMOTEDATAACQHANDLER_H_

#include <analogic/ws/common/bagdata/bagdataacqhandler.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   RemoteDataAcqHandler
 * \brief   This class contains variable and function related to
 *          Remote Data Acquisition Handler.
 */
class RemoteDataAcqHandler : public BagDataAcqHandler
{
public:
    /*!
    * @fn       RemoteDataAcqHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class RemoteDataAcqHandler.
    */
    RemoteDataAcqHandler();

    /*!
    * @fn       ~RemoteDataAcqHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class RemoteDataAcqHandler.
    */
    ~RemoteDataAcqHandler();
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_COMMON_BAGDATA_REMOTEDATAACQHANDLER_H_

