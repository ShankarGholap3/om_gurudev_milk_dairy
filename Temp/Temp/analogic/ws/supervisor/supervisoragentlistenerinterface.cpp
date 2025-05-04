/*!
* @file     SupervisorAgentListenerInterface.cpp
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     Jul, 11 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/ulm/uilayoutmanager.h>
#include "supervisoragentlistenerinterface.h"
#include "supervisorconfig.h"

namespace analogic
{
namespace ws
{
/*!
* @fn       SupervisorAgentListenerInterface();
* @param    QObject* - parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
SupervisorAgentListenerInterface::SupervisorAgentListenerInterface()
{
  TRACE_LOG("");
  // creating new qtimer instance
  m_pSupervisorUpdateTimer.reset(new QTimer(this));
  THROW_IF_FAILED((m_pSupervisorUpdateTimer.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

  // setting timer interval
  m_pSupervisorUpdateTimer->setInterval(SupervisorConfig::getSupervisorListUpdateInterval());

 //  searchListChanged not available in SupervisorAgentListener
//  connect(m_pSupervisorUpdateTimer.get(), &QTimer::timeout, this,
//          &SearchAgentListenerInterface::searchListChanged ,
//          Qt::QueuedConnection);
//  m_pSupervisorUpdateTimer->start();
}

/*!
* @fn       ~SupervisorAgentListenerInterface
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
SupervisorAgentListenerInterface::~SupervisorAgentListenerInterface()
{
  TRACE_LOG("");
}

/*!
* @fn       KeepAlive
* @param    None
* @return   None
* @brief    Gives alive status of the workstation to the nss service.
*/
void SupervisorAgentListenerInterface::KeepAlive(void)
{

}

/*!
* @fn       RegistrationChanged
* @param    analogic::nss::SupervisorAgent* agent
* @return   None
* @brief    Gives Registration Changed status of the workstation to the nss service.
*/
void SupervisorAgentListenerInterface::RegistrationChanged(analogic::nss::SupervisorAgent* agent)
{
  if(agent == nullptr)
  {
    ERROR_LOG("RegistrationChanged: input agent is NULL");
    return;
  }
  analogic::nss::RegistrationClient* client = agent->get_registration();
  if(client == nullptr)
  {
    ERROR_LOG("RegistrationChanged: registered client is NULL");
    return;
  }
  analogic::nss::RegistrationState state = client->get_registration_state();
  DEBUG_LOG("RegistrationChanged state:" << state);
  switch (state) {
  case analogic::nss::RegistrationState::UNREGISTERED:
    emit networkStatusChanged(QMLEnums::NetworkState::UNREGISTERED);
    break;
  case analogic::nss::RegistrationState::RECONNECTING:
    emit networkStatusChanged(QMLEnums::NetworkState::RECONNECTING);
    break;
  case analogic::nss::RegistrationState::REGISTERED:
    emit networkStatusChanged(QMLEnums::NetworkState::REGISTERED);
    break;
  default:
    break;
  }
}

}  // end of namespace ws
}  // end of namespace analogic
