/*!
* @file     agentlistener.cpp
* @author   Agiliad
* @brief    This file contains interface to listen on NSS Agent.
* @date     Mar, 03 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common.h>
#include <analogic/ws/common/accesslayer/agentlistener.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       AgentListener
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class AgentListener.
*/
AgentListener::AgentListener()
{
    TRACE_LOG("");
}

/*!
* @fn       ~AgentListener
* @param    None
* @return   None
* @brief    Destructor for class AgentListener.
*/
AgentListener::~AgentListener()
{
    TRACE_LOG("");
}

/*!
* @fn       onNotifyBagIsPickedUpsig
* @param    analogic::nss::BhsDiverterDecision
* @return   None
* @brief    Slot To Notify Bag Is Picked Up.
*/
void AgentListener::onNotifyBagIsPickedUp(nss::BasicCommandResult commandResult)
{

}
}  // end of namespace ws
}  // end of namespace analogic
