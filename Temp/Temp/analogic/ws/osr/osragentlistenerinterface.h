/*!
* @file     osragentlistenerinterface.h
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     Feb, 28 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_OSRAGENTLISTENERINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_OSRAGENTLISTENERINTERFACE_H_


#include <boost/shared_ptr.hpp>

#include <analogic/nss/util/Version.h>
#include <analogic/nss/agent/osr/OsrLiveBag.h>
#include <analogic/nss/agent/osr/OsrAgentStatus.h>
#include <analogic/nss/agent/osr/OsrAgentListener.h>

#include <analogic/nss/agent/osr/OsrAgent.h>
#include <analogic/ws/common.h>
#include <analogic/ws/common/accesslayer/agentlistener.h>
#include <analogic/nss/agent/registration/RegistrationClient.h>
#include <analogic/nss/common/NssEvent.h>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>

using namespace analogic::nss;
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   OSRAgentListenerInterface
 * \brief   This class contains variable and function related to
 *           access Nss agent library for OSR Workstation.
 */
class OSRAgentListenerInterface :public AgentListener, public analogic::nss::OsrAgentListener
{
public:
    /*!
    * @fn       OSRAgentListenerInterface();
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    explicit OSRAgentListenerInterface();

    /*!
    * @fn       ~OSRAgentListenerInterface
    * @param    None
    * @return   None
    * @brief    Destructor responsible for deinitialization of members, memory and resources.
    */
    ~OSRAgentListenerInterface();


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
    virtual void RegistrationChanged(analogic::nss::OsrAgent* agent);

    /*!
    * @fn       RequestPushedBagDecision
    * @param    analogic::nss::OsrLiveBag* bag
    * @return   None
    * @brief    Requests to push bag for decision.
    */
    virtual void RequestPushedBagDecision(analogic::nss::OsrLiveBag* bag);

    /*!
    * @fn       RequestPullBagDecision
    * @param    analogic::nss::OsrLiveBag* bag
    * @return   None
    * @brief    Requests to pull bag for decision.
    */
    virtual void RequestPullBagDecision(analogic::nss::OsrLiveBag* bag);

    /*!
    * @fn       EventReceived
    * @param    analogic::nss::NssEvent event_code
    * @param    const std::vector<std::string>&
    * @return   None
    * @brief    Event received from nss
    */
    virtual void EventReceived(analogic::nss::NssEvent event_code, const std::vector<std::string>& data);

    /*!
    * @fn       RequestBagPickup
    * @param    std::string& id_assigning_authority
    * @param    const std::string primary bag  ID
    * @param    const std::string rfid
    * @param    analogic::nss::BhsDiverterDecision
    * @param    const std::string diverter decision string
    * @param    Callback<BasicCommandResult>* response
    * @return   None
    * @brief    Slot called when bag is ready for picked up.
    */
    virtual void RequestBagPickup(const std::string& id_assigning_authority,
                                  const std::string& bag_id, const std::string& rfid, const std::string& pickup_type,
                                  const std::string& source_bhs,
                                  BhsDiverterDecision diverter_decision_code, const std::string& diverter_decision_string,
                                  Callback<BasicCommandResult>* response_callback);

    /*!
    * @fn       onNotifyBagIsPickedUpsig
    * @param    analogic::nss::BasicCommandResult
    * @return   None
    * @brief    Slot To Notify Bag Is Picked Up.
    */
    virtual void onNotifyBagIsPickedUp(analogic::nss::BasicCommandResult commandResult);

private:
    Callback<BasicCommandResult>* m_responseCallback;   //!< stores callback function address
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_OSRAGENTLISTENERINTERFACE_H_

