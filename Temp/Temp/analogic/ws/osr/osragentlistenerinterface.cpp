/*!
* @file     osragentlistenerinterface.cpp
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     Feb, 28 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/osr/osragentlistenerinterface.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       OSRAgentListenerInterface();
* @param    QObject* - parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
OSRAgentListenerInterface::OSRAgentListenerInterface()
{
  TRACE_LOG("");
  m_responseCallback = nullptr;
}

/*!
* @fn       ~OSRAgentListenerInterface
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
OSRAgentListenerInterface::~OSRAgentListenerInterface()
{
  TRACE_LOG("");
}

/*!
* @fn       KeepAlive
* @param    None
* @return   None
* @brief    Gives alive status of the workstation to the nss service.
*/
void OSRAgentListenerInterface::KeepAlive(void) {
  TRACE_LOG("");
}

/*!
* @fn       RegistrationChanged
* @param    analogic::nss::OsrAgent* agent
* @return   None
* @brief    Gives Registration Changed status of the workstation to the nss service.
*/
void OSRAgentListenerInterface::RegistrationChanged(analogic::nss::OsrAgent* agent) {
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
  INFO_LOG("RegistrationChanged state:" << state);
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
* @fn       RequestPushedBagDecision
* @param    analogic::nss::OsrLiveBag* bag
* @return   None
* @brief    Requests to push bag for decision.
*/
void OSRAgentListenerInterface::RequestPushedBagDecision(analogic::nss::OsrLiveBag* bag) {
  TRACE_LOG("");
  if ( NULL != bag)
  {
    DEBUG_LOG("Request Pushed BagDecision for bag:" << bag->get_transfer_bag_identifier());
  }
}

/*!
* @fn       RequestPullBagDecision
* @param    analogic::nss::OsrLiveBag* bag
* @return   None
* @brief    Requests to pull bag for decision.
*/
void OSRAgentListenerInterface::RequestPullBagDecision(analogic::nss::OsrLiveBag* bag) {
  TRACE_LOG("");
  if ( NULL != bag)
  {
    INFO_LOG("Bag Flow Event :: Bag Notification received for Bag Identifier" <<bag->get_transfer_bag_identifier());
    // notify workstation about the bag
    emit requestBagDecision(bag, QMLEnums::ArchiveFilterSrchLoc::PRIMARY_SERVER,"",QMLEnums::BagsListAndBagDataCommands::BLBDC_SHOW_BAG);
  }
}

/*!
* @fn       EventReceived
* @param    analogic::nss::NssEvent event_code
* @param    const std::vector<std::string>&
* @return   None
* @brief    Event received from nss
*/
void OSRAgentListenerInterface::EventReceived(analogic::nss::NssEvent event_code, const std::vector<std::string>& data)
{
  emit eventReceivedsig(event_code,data);
}

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
void OSRAgentListenerInterface::RequestBagPickup(const std::string& id_assigning_authority,
                                                 const std::string& bag_id, const std::string& rfid, const std::string& pickup_type,
                                                 const std::string& source_bhs,
                                                 BhsDiverterDecision diverter_decision_code, const std::string& diverter_decision_string,
                                                 Callback<BasicCommandResult>* response_callback)
{
  TRACE_LOG("");
  if(nullptr != m_responseCallback){
    WARN_LOG("Pickup callback (RequestBagPickup): previous m_responseCallback Is not null, \
             earlier callback response was not sent and a new one was received.");
  }
  QString scannername = WorkstationConfig::getInstance()->getScannername();
  INFO_LOG("Bag Flow Event ::Request Bag Pickup received for "<<source_bhs<<"  "<<bag_id<<" " <<scannername.toStdString());
  if(source_bhs.compare(scannername.toStdString()) == 0)
  {
    m_responseCallback = response_callback;
    emit requestBagPickupsig(id_assigning_authority, bag_id, rfid, pickup_type, diverter_decision_code, diverter_decision_string);
  }
}

/*!
* @fn       onNotifyBagIsPickedUp
* @param    analogic::nss::BasicCommandResult
* @return   None
* @brief    Slot To Notify Bag Is Picked Up.
*/
void OSRAgentListenerInterface::onNotifyBagIsPickedUp(BasicCommandResult commandResult)
{
  TRACE_LOG("");
  if(nullptr == m_responseCallback){
    WARN_LOG("Pickup callback Response : m_responseCallback Is null, A response will not be sent to NSS. ");
  }

  if(m_responseCallback != nullptr)
  {
    m_responseCallback->Run(commandResult);
    m_responseCallback = nullptr;
  }
}
}  // end of namespace ws
}  // end of namespace analogic
