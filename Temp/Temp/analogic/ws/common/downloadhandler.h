/*!
* @file     downloadhandler.h
* @author   Agiliad
* @brief    This file contains interface, responsible for downloading the file from web service and notifying ModelUpdater about the file download status.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_DOWNLOADHANDLER_H_
#define ANALOGIC_WS_COMMON_DOWNLOADHANDLER_H_

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   DownloadHandler
 * \brief   This class contains variable and function related to
 *          Download Handler.
 */
class DownloadHandler
{
public:
    /*!
    * @fn       DownloadHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class DownloadHandler.
    */
    DownloadHandler();

    /*!
    * @fn       ~DownloadHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class DownloadHandler.
    */
    ~DownloadHandler();
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_COMMON_DOWNLOADHANDLER_H_

