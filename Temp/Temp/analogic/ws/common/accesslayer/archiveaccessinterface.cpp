/*!
* @file     archiveaccessinterface.cpp
* @author   Agiliad
* @brief    This file contains interface to provide archive data from archive service hosted on Admin Server.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common.h>
#include <analogic/ws/common/accesslayer/archiveaccessinterface.h>

namespace analogic
{
namespace ws
{
ArchiveAccessInterface* ArchiveAccessInterface::m_aal = NULL;

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void ArchiveAccessInterface::destroyInstance()
{
    TRACE_LOG("");
    SAFE_DELETE(m_aal);
}

/*!
* @fn       getInstance
* @param    None
* @return   ArchiveAccessInterface* -  pointer to object
* @brief    This function creates instance of ArchiveAccessInterface and returns pointer to
*           it and also creates instance of Rial.
*/
ArchiveAccessInterface* ArchiveAccessInterface::getInstance()
{
    if(!m_aal)
    {
        INFO_LOG("Creating ArchiveAccessInterface instance");
        m_aal = new ArchiveAccessInterface();
        if(!m_aal)
        {
            ERROR_LOG("Error in allocating memory for ArchiveAccessInterface instance");
            throw QException();
        }
         DEBUG_LOG("Created ArchiveAccessInterface instance");
    }
    DEBUG_LOG("Return ArchiveAccessInterface instance");
    return m_aal;
}
/*!
* @fn       ArchiveAccessInterface
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class ArchiveAccessInterface.
*/
ArchiveAccessInterface::ArchiveAccessInterface(QObject *parent): Rial(parent)
{
    TRACE_LOG("");
}

/*!
* @fn       ~ArchiveAccessInterface
* @param    None
* @return   None
* @brief    Destructor for class ArchiveAccessInterface.
*/
ArchiveAccessInterface::~ArchiveAccessInterface()
{
    TRACE_LOG("");
}
}  // end of namespace ws
}  // end of namespace analogic
