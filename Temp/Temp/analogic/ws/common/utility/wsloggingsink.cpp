/*!
* @file     wsloggingsink.cpp
* @author   Agiliad
* @brief    This file contains changes related to customization
*           nss logging sink class
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include "wsloggingsink.h"
namespace analogic
{
namespace ws
{

/*!
* @fn       wsLoggingSink
* @param    None
* @return   None
* @brief    Constructor for wsLoggingSink
*/
wsLoggingSink::wsLoggingSink()
{

}

/*!
* @fn       Log
* @param    const std::string
* @return   None
* @brief    This function logs messages
*/
void wsLoggingSink::Log(const std::string& line)
{
   LOG(Logger::severity_level)<<line.c_str();
}

/*!
* @fn       ~wsLoggingSink
* @param    None
* @return   None
* @brief    Destructor for wsLoggingSink
*/
wsLoggingSink::~wsLoggingSink()
{

}
}
}
