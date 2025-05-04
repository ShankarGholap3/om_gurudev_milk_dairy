/*!
* @file     searchagentlistenerinterface.h
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     Feb, 28 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_SEARCHAGENTLISTENERINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_SEARCHAGENTLISTENERINTERFACE_H_


#include <boost/shared_ptr.hpp>

#include <analogic/nss/util/Version.h>
#include <analogic/nss/agent/search/SearchBag.h>
#include <analogic/nss/agent/search/SearchAgent.h>
#include <analogic/nss/agent/search/SearchListEntry.h>
#include <analogic/nss/agent/search/SearchAgentStatus.h>
#include <analogic/nss/agent/search/SearchAgentListener.h>
#include <analogic/ws/search/searchconfig.h>
#include <analogic/ws/common.h>
#include <analogic/ws/common/accesslayer/agentlistener.h>
//#include <analogic/ws/common/accesslayer/scanneragentstatushandler.h>
#include <analogic/nss/agent/registration/RegistrationClient.h>
#include <analogic/nss/common/NssEvent.h>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>
#include <QTimer>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   SearchAgentListenerInterface
 * \brief   This class contains variable and function related to
 *           access Nss agent library.
 */
class SearchAgentListenerInterface :public AgentListener,
        public analogic::nss::SearchAgentListener
{
    Q_OBJECT
public:
    /*!
    * @fn       SearchAgentListenerInterface();
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    explicit SearchAgentListenerInterface();

    /*!
    * @fn       ~SearchAgentListenerInterface
    * @param    None
    * @return   None
    * @brief    Destructor responsible for deinitialization of members, memory and resources.
    */
    ~SearchAgentListenerInterface();

    /*!
    * @fn       KeepAlive
    * @param    None
    * @return   None
    * @brief    Gives alive status of the workstation to the nss service.
    */
    virtual void KeepAlive(void);

    /*!
    * @fn       RegistrationChanged
    * @param    analogic::nss::OsrAgent* agent
    * @return   None
    * @brief    Gives Registration Changed status of the workstation to the nss service.
    */
    virtual void RegistrationChanged(analogic::nss::SearchAgent* agent);

    /*!
    * @fn       SearchListChanged
    * @param    analogic::nss::SearchAgent *agent
    * @return   None.
    * @brief    Notifies to workstation if searchList changes
    */
    virtual void SearchListChanged (analogic::nss::SearchAgent *agent);

    /*!
    * @fn       EventReceived
    * @param    analogic::nss::NssEvent event_code
    * @param    const std::vector<std::string>&
    * @return   None
    * @brief    Event received from nss
    */
    virtual void EventReceived(analogic::nss::NssEvent event_code, const std::vector<std::string>& data);

signals:
    void searchListChanged();

protected:

    std::vector<analogic::nss::SearchListEntry>     m_searchList;           //!< Contains updated search bag list
    boost::shared_ptr<QTimer>                       m_pSearchUpdateTimer;   //!< handle for search list update timer
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_SEARCHAGENTLISTENERINTERFACE_H_

