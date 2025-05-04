/*!
* @file     wsloggingsink.h
* @author   Agiliad
* @brief    This file contains changes related to customization
*           nss logging sink class
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef WSLOGGINGSINK_H
#define WSLOGGINGSINK_H

#include <analogic/ws/common.h>

namespace analogic {
namespace ws {

#include <analogic/nss/log/StdoutNssLoggingSink.h>

/*!
 * \class   wsLoggingSink
 * \brief   This file contains changes related to customization of
 *          nss logging sink class
 */
class wsLoggingSink:public analogic::nss::NssLoggingSink
{
public:
    /*!
    * @fn       wsLoggingSink
    * @param    None
    * @return   None
    * @brief    Constructor for wsLoggingSink
    */
    wsLoggingSink();

    /*!
    * @fn       Log
    * @param    const std::string
    * @return   None
    * @brief    This function logs messages
    */
    void Log(const std::string& line);

    /*!
    * @fn       ~wsLoggingSink
    * @param    None
    * @return   None
    * @brief    Destructor for wsLoggingSink
    */
    ~wsLoggingSink();
};

}
}
#endif // WSLOGGINGSINK_H
