/*!
* @file     pushdatahandler.h
* @author   Agiliad
* @brief    This file contains functionality for listening for bag data and receives it when scanner pushes it as ready.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_BAGDATA_PUSHDATAHANDLER_H_
#define ANALOGIC_WS_COMMON_BAGDATA_PUSHDATAHANDLER_H_

#include <analogic/ws/common/bagdata/bagdataacqhandler.h>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{

/*!
 * \class   PushDataHandler
 * \brief   This class contains variable and function related to
 *          Push Data Handler.
 */
class PushDataHandler : public BagDataAcqHandler
{
public:
    /*!
    * @fn       PushDataHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class PushDataHandler.
    */
    PushDataHandler();

    /*!
    * @fn       ~PushDataHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class PushDataHandler.
    */
    ~PushDataHandler();
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_BAGDATA_PUSHDATAHANDLER_H_

