/*!
* @file     traininghandler.cpp
* @author   Agiliad
* @brief    This file contains functionality related to osr connections.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/training/traininghandler.h>
#include <analogic/ws/nsshandler.h>

namespace analogic
{

namespace ws
{
/*!
* @fn       TrainingHandler
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class TrainingHandler.
*/
TrainingHandler::TrainingHandler()
{
    TRACE_LOG("");
    if (m_nssAccesslayer)
    {      
        m_nssAccesslayer->createOsrAgentListener();
    }
}

/*!
* @fn       TrainingHandler
* @param    NSSAgentAccessInterface*
* @return   None
* @brief    Constructor for class TrainingHandler, needed for mocking nss in unit testing.
*/
TrainingHandler::TrainingHandler(NSSAgentAccessInterface* nssaccess):NssHandler(nssaccess)
{
    TRACE_LOG("");
}

/*!
* @fn       ~NetworkFaultHandler
* @param    None
* @return   None
* @brief    Destructor for class NetworkFaultHandler.
*/
TrainingHandler::~TrainingHandler()
{
    TRACE_LOG("");
}

/*!
* @fn       setOsrReadyState
* @param    bool
* @return   int
* @brief    Sets OSR ready state.
*/
int TrainingHandler::setOsrReadyState(bool state)
{
    TRACE_LOG("");
    if (m_nssAccesslayer)
    {
        m_nssAccesslayer->setOsrReadyState(state);
    }
    else
    {
        m_nssAccesslayer = NSSAgentAccessInterface::getInstance();
        m_nssAccesslayer->setOsrReadyState(state);
    }
    return Errors::S_OK;
}


/*!
* @fn       onUpdateState();
* @param    QMLEnums::AppState state
* @return   void
* @brief    call on update state.
*/
void TrainingHandler::onUpdateState(QMLEnums::AppState state)
{
    TRACE_LOG("");
    if(state == QMLEnums::ONLINE)
    {
        setOsrReadyState(true);
    }
    else if(state == QMLEnums::SET_TO_OFFLINE)
    {
        setOsrReadyState(false);
    }
}
}  // end of namespace ws
}  // end of namespace analogic
