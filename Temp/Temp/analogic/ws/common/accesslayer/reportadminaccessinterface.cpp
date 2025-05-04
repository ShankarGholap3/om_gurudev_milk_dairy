/*!
* @file     reportadminaccessinterface.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           report admin server and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/accesslayer/reportadminaccessinterface.h>
#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
ReportAdminAccessInterface* ReportAdminAccessInterface::m_raal = NULL;

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void ReportAdminAccessInterface::destroyInstance()
{
    SAFE_DELETE(m_raal);
}

/*!
* @fn       getInstance
* @param    None
* @return   boost::shared_ptr<ReportAdminAccessInterface> -  pointer to object
* @brief    This function creates instance of ReportAdminAccessInterface and returns pointer.
*/
ReportAdminAccessInterface* ReportAdminAccessInterface::getInstance()
{
    QMutex mutex;
    mutex.lock();
    if(!m_raal)
    {
        DEBUG_LOG("Creating instance of UserAdminAccessInterface");
        m_raal = new ReportAdminAccessInterface();
        if(!m_raal)
        {
            ERROR_LOG("Failed to create instance of UserAdminAccessInterface");
            throw QException();
        }
    }
    mutex.unlock();
    return m_raal;    
}

/*!
* @fn       ReportAdminAccessInterface
* @param    QObject
* @return   None
* @brief    This function is constructor for class ReportAdminAccessInterface.
*/
ReportAdminAccessInterface::ReportAdminAccessInterface(QObject *parent):
    Rial(parent)
{
}

/*!
* @fn       ~ReportAdminAccessInterface
* @param    None
* @return   None
* @brief    Private destructor.
*/
ReportAdminAccessInterface::~ReportAdminAccessInterface()
{
}
}  // end of namespace ws
}  // end of namespace analogic

