/*!
* @file     dicoshandler.h
* @author   Agiliad
* @brief    This file contains interface to do DICOS related operations.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_DICOSHANDLER_H_
#define ANALOGIC_WS_COMMON_DICOSHANDLER_H_

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   DICOSHandler
 * \brief   This class contains variable and function related to
 *          DICOS Handler.
 */
class DICOSHandler
{
public:
    /*!
    * @fn       DICOSHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class DICOSHandler.
    */
    DICOSHandler();

    /*!
    * @fn       ~DICOSHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class DICOSHandler.
    */
    ~DICOSHandler();
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_COMMON_DICOSHANDLER_H_

