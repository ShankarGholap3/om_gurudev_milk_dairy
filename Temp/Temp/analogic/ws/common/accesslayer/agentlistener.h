/*!
* @file     agentlistener.h
* @author   Agiliad
* @brief    This file contains interface to listen on NSS Agent
* @date     Mar, 03 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_AGENTLISTENER_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_AGENTLISTENER_H_

#include <QObject>
#include <analogic/nss/common/NssBag.h>
#include <analogic/nss/common/NssEvent.h>
#include <analogic/nss/agent/bhs/BhsDiverterDecision.h>
#include <analogic/nss/agent/registration/RegistrationState.h>

#include <analogic/ws/common.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   AgentListener
 * \brief   This class contains variable and function related to
 *          Agent listener
 */
class AgentListener : public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       AgentListener
    * @param    None
    * @return   None
    * @brief    Constructor for class AgentListener.
    */
    AgentListener();

    /*!
    * @fn       ~AgentListener
    * @param    None
    * @return   None
    * @brief    Destructor for class AgentListener.
    */
    virtual ~AgentListener();

signals:
    /*!
    * @fn       networkStatusChanged
    * @param    QMLEnums::NetworkState

    * @return   None
    * @brief    notifies about status of network from nss service.
    */
    void networkStatusChanged(QMLEnums::NetworkState state);

    /*!
    * @fn       requestBagDecision
    * @param    analogic::nss::NssBag bag
    * @param    QMLEnums::ArchiveFilterSrchLoc loc
    * @param    std::string data
    * @param    QMLEnums::BagsListAndBagDataCommands mode
    * @return   None
    * @brief    notifies workstation about the arrival of bag.
    */
    void requestBagDecision(analogic::nss::NssBag* bag, QMLEnums::ArchiveFilterSrchLoc loc,  std::string data , QMLEnums::BagsListAndBagDataCommands mode);

    /*!
    * @fn       eventReceivedsig
    * @param    analogic::nss::NssEvent event_code
    * @param    const std::vector<std::string>&
    * @return   None
    * @brief    Event received from nss
    */
    void eventReceivedsig(analogic::nss::NssEvent event_code, const std::vector<std::string>& data);       

    /*!
    * @fn       requestBagPickupsig
    * @param    const std::string id_assigning_authority
    * @param    const std::string primary bag  ID
    * @param    const std::string rfid
    * @param    const QString pickup_type
    * @param    analogic::nss::BhsDiverterDecision
    * @param    const std::string diverter decision string
    * @return   None
    * @brief    Signal to notify back is ready for pick up.
    */
    void requestBagPickupsig(const std::string& id_assigning_authority, const std::string &primary_bag_id, const std::string &rfid, const std::string& pickup_type, analogic::nss::
                             BhsDiverterDecision diverter_decision_code, const std::string &diverter_decision_string);

public slots:
    /*!
    * @fn       onNotifyBagIsPickedUpsig
    * @param    analogic::nss::BhsDiverterDecision
    * @return   None
    * @brief    Slot To Notify Bag Is Picked Up.
    */
    virtual void onNotifyBagIsPickedUp(analogic::nss::BasicCommandResult commandResult);
};



}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_AGENTLISTENER_H_

