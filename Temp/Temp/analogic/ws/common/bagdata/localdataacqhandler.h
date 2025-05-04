/*!
* @file     localdataacqhandler.h
* @author   Agiliad
* @brief    This file contains interface, responsible for managing local bag data.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_BAGDATA_LOCALDATAACQHANDLER_H_
#define ANALOGIC_WS_COMMON_BAGDATA_LOCALDATAACQHANDLER_H_

#include <analogic/ws/common/bagdata/bagdataacqhandler.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{

/*!
 * \class   LocalDataAcqHandler
 * \brief   This class contains variable and function related to
 *          Local Data Acquisition Handler.
 */
class LocalDataAcqHandler : public BagDataAcqHandler
{
public:
    /*!
    * @fn       LocalDataAcqHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class LocalDataAcqHandler.
    */
    LocalDataAcqHandler();

    /*!
    * @fn       ~LocalDataAcqHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class LocalDataAcqHandler.
    */
    ~LocalDataAcqHandler();
};
}  // end of namespace ws
}  // end of namespace analogic


#endif  // ANALOGIC_WS_COMMON_BAGDATA_LOCALDATAACQHANDLER_H_

