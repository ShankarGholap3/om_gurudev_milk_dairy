/*!
* @file     AntiVirusAccessInterface.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           antivirus server and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/accesslayer/antivirusaccessinterface.h>
#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
AntiVirusAccessInterface* AntiVirusAccessInterface::m_raal = NULL;

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void AntiVirusAccessInterface::destroyInstance()
{
    SAFE_DELETE(m_raal);
}

/*!
* @fn       getInstance
* @param    None
* @return   boost::shared_ptr<AntiVirusAccessInterface> -  pointer to object
* @brief    This function creates instance of AntiVirusAccessInterface and returns pointer.
*/
AntiVirusAccessInterface* AntiVirusAccessInterface::getInstance()
{
    QMutex mutex;
    mutex.lock();
    if(!m_raal)
    {
        DEBUG_LOG("Creating instance of UserAdminAccessInterface");
        m_raal = new AntiVirusAccessInterface();
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
* @fn       AntiVirusAccessInterface
* @param    QObject
* @return   None
* @brief    This function is constructor for class AntiVirusAccessInterface.
*/
AntiVirusAccessInterface::AntiVirusAccessInterface(QObject *parent):
    Rial(parent)
{
}

/*!
* @fn       ~AntiVirusAccessInterface
* @param    None
* @return   None
* @brief    Private destructor.
*/
AntiVirusAccessInterface::~AntiVirusAccessInterface()
{
}
}  // end of namespace ws
}  // end of namespace analogic

