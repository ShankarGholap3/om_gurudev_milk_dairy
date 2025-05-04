/*!
* @file     SupervisorAccessInterface.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           supervisor and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/accesslayer/supervisoraccessinterface.h>
#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
SupervisorAccessInterface* SupervisorAccessInterface::m_pSupervisorAccessInterface = NULL;

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void SupervisorAccessInterface::destroyInstance()
{
    TRACE_LOG("");
    SAFE_DELETE(m_pSupervisorAccessInterface);
}

/*!
* @fn       getInstance
* @param    None
* @return   SupervisorAccessInterface* -  pointer to object
* @brief    This function creates instance of SupervisorAccessInterface and returns pointer to
*           it and also creates instance of Rial.
*/
SupervisorAccessInterface* SupervisorAccessInterface::getInstance()
{
    if(!m_pSupervisorAccessInterface)
    {
        INFO_LOG("Creating SupervisorAccessInterface instance");
        m_pSupervisorAccessInterface = new SupervisorAccessInterface();
        if(!m_pSupervisorAccessInterface)
        {
            ERROR_LOG("Error in allocating memory for SupervisorAccessInterface instance");
            throw QException();
        }
         DEBUG_LOG("Created SupervisorAccessInterface instance");
    }
    DEBUG_LOG("Return SupervisorAccessInterface instance");
    return m_pSupervisorAccessInterface;
}
/*!
* @fn       SupervisorAccessInterface
* @param    QObject
* @return   None
* @brief    This function is constructor for class SupervisorAccessInterface who is responsible for creating instance of Rial.
*/
SupervisorAccessInterface::SupervisorAccessInterface(QObject *parent):
    Rial(parent)
{
}

/*!
* @fn       ~SupervisorAccessInterface
* @param    None
* @return   None
* @brief    private destructor.
*/
SupervisorAccessInterface::~SupervisorAccessInterface()
{
}
}  // end of namespace ws
}  // end of namespace analogic
