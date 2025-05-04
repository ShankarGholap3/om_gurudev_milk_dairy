/*!
* @file     networkfaulthandler.cpp
* @author   Agiliad
* @brief    This file contains interface, responsible for getting workstation heartbeat signal and sending it to remote monitor.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/faults/networkfaulthandler.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       NetworkFaultHandler
* @param    None
* @return   None
* @brief    Constructor for class NetworkFaultHandler.
*/
NetworkFaultHandler::NetworkFaultHandler()
{
    TRACE_LOG("");
    INFO_LOG("Create timer for network fault");
    m_pNetworkStatusTimer.reset(new QTimer(this));
    THROW_IF_FAILED((m_pNetworkStatusTimer.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    // setting up for status timer
    m_timerinterval = WorkstationConfig::getInstance()->getNSSServerCheckInterval();
    m_pNetworkStatusTimer->setInterval(m_timerinterval);
    m_pNetworkStatusTimer->setTimerType(Qt::PreciseTimer);
    // connecting timer events to proper slot
    connect(m_pNetworkStatusTimer.get(), &QTimer::timeout, this, &NetworkFaultHandler::onTimeout, Qt::QueuedConnection);

    m_nwstate = QMLEnums::NetworkState::UNREGISTERED;
}

/*!
* @fn       ~NetworkFaultHandler
* @param    None
* @return   None
* @brief    Destructor for class NetworkFaultHandler.
*/
NetworkFaultHandler::~NetworkFaultHandler()
{
    TRACE_LOG("");
    m_pNetworkStatusTimer.reset();
}

/*!
* @fn       getNetworkState
* @param    None
* @return   QMLEnums::NetworkState
* @brief    Returns current network state.
*/
QMLEnums::NetworkState NetworkFaultHandler::getNetworkState()
{
    return m_nwstate;
}

/*!
* @fn       networkStatus
* @param    QMLEnums::NetworkState status
* @return   None
* @brief    notifies about status of network from nss service.
*/
void NetworkFaultHandler::networkStatus(QMLEnums::NetworkState status)
{
    TRACE_LOG("");
    DEBUG_LOG("Setting network status to "<<status);
    m_nwstate = status;
    if (m_nwstate == QMLEnums::NetworkState::UNREGISTERED) {
        if (!m_pNetworkStatusTimer->isActive()) {
            INFO_LOG("Start timer for network fault");
            onTimeout();
            m_pNetworkStatusTimer->start();
        }
    }
    else if (m_nwstate == QMLEnums::NetworkState::REGISTERED) {
        m_pNetworkStatusTimer->stop();
    }
}

/*!
* @fn       onTimeout
* @param    None
* @return   None
* @brief    slot will call on timeout
*/
void NetworkFaultHandler::onTimeout()
{
    if (m_nwstate != QMLEnums::NetworkState::REGISTERED)
    {
        INFO_LOG("Update Network State : " << m_nwstate);
        emit updateNetwork();
    }
}

/*!
* @fn      onExit
* @param   None
* @return  None
* @brief   slot call on exit
*/
void NetworkFaultHandler::onExit()
{
    if(m_pNetworkStatusTimer != NULL)
    {
        m_pNetworkStatusTimer->stop();
        m_pNetworkStatusTimer.reset();
    }
}
}  // end of namespace ws
}  // end of namespace analogic

