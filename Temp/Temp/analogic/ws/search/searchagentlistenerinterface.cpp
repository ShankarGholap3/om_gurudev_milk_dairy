/*!
* @file     searchagentlistenerinterface.cpp
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     Feb, 28 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/search/searchagentlistenerinterface.h>
#include <analogic/ws/ulm/uilayoutmanager.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       SearchAgentListenerInterface();
* @param    QObject* - parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
SearchAgentListenerInterface::SearchAgentListenerInterface()
{
    TRACE_LOG("");
    // creating new qtimer instance
    m_pSearchUpdateTimer.reset(new QTimer(this));
    THROW_IF_FAILED((m_pSearchUpdateTimer.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    // setting timer interval
    m_pSearchUpdateTimer->setInterval(SearchConfig::getSearchlistupdateInterval());
    // connecting timer events to proper slot
    connect(m_pSearchUpdateTimer.get(), &QTimer::timeout, this,
            &SearchAgentListenerInterface::searchListChanged ,
            Qt::QueuedConnection);
    m_pSearchUpdateTimer->start();
}

/*!
* @fn       ~SearchAgentListenerInterface
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
SearchAgentListenerInterface::~SearchAgentListenerInterface()
{
    TRACE_LOG("");
}

/*!
* @fn       KeepAlive
* @param    None
* @return   None
* @brief    Gives alive status of the workstation to the nss service.
*/
void SearchAgentListenerInterface::KeepAlive(void) {
    TRACE_LOG("");
}

/*!
* @fn       RegistrationChanged
* @param    analogic::nss::OsrAgent* agent
* @return   None
* @brief    Gives Registration Changed status of the workstation to the nss service.
*/
void SearchAgentListenerInterface::RegistrationChanged(analogic::nss::SearchAgent* agent) {
    TRACE_LOG("");
    if(agent == NULL)
    {
        ERROR_LOG("RegistrationChanged: input agent is NULL");
        return;
    }
    analogic::nss::RegistrationClient* client = agent->get_registration();
    if(client == NULL)
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

/*!
* @fn       SearchListChanged
* @param    analogic::nss::SearchAgent *agent
* @return   None.
* @brief    Notifies to workstation if searchList changes
*/
void SearchAgentListenerInterface::SearchListChanged(analogic::nss::SearchAgent *agent)
{
    Q_UNUSED(agent);
    //TRACE;
    emit searchListChanged();
}
/*!
* @fn       EventReceived
* @param    analogic::nss::NssEvent event_code
* @param    const std::vector<std::string>&
* @return   None
* @brief    Event received from nss
*/
void SearchAgentListenerInterface::EventReceived(analogic::nss::NssEvent event_code, const std::vector<std::string>& data)
{
    emit eventReceivedsig(event_code, data);
}
}  // end of namespace ws
}  // end of namespace analogic
