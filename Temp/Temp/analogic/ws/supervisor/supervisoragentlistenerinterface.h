/*!
* @file     SupervisorAgentListenerInterface.h
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     Jul, 11 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORAGENTLISTENERINTERFACE_H_
#define SUPERVISORAGENTLISTENERINTERFACE_H_


#include <boost/shared_ptr.hpp>
#include <analogic/nss/util/Version.h>
#include <analogic/nss/agent/supervisor/SupervisorAgent.h>
#include <analogic/nss/agent/supervisor/SupervisorAgentListener.h>
#include <analogic/ws/common.h>
#include <analogic/ws/common/accesslayer/agentlistener.h>
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
 * \class   SupervisorAgentListenerInterface
 * \brief   This class contains variable and function related to
 *           access Nss agent library.
 */
class SupervisorAgentListenerInterface :public AgentListener,
    public analogic::nss::SupervisorAgentListener
{
  Q_OBJECT
public:
  /*!
    * @fn       SupervisorAgentListenerInterface();
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
  explicit SupervisorAgentListenerInterface();

  /*!
    * @fn       ~SupervisorAgentListenerInterface
    * @param    None
    * @return   None
    * @brief    Destructor responsible for deinitialization of members, memory and resources.
    */
  ~SupervisorAgentListenerInterface();

  /*!
    * @fn       KeepAlive
    * @param    None
    * @return   None
    * @brief    Gives alive status of the workstation to the nss service.
    */
  virtual void KeepAlive(void);

  /*!
    * @fn       RegistrationChanged
    * @param    analogic::nss::SupervisorAgent* agent
    * @return   None
    * @brief    Gives Registration Changed status of the workstation to the nss service.
    */
  virtual void RegistrationChanged(analogic::nss::SupervisorAgent* agent);

protected:
  boost::shared_ptr<QTimer>                       m_pSupervisorUpdateTimer;   //!< handle for supervisor list update timer
};


}  // end of namespace ws
}  // end of namespace analogic
#endif  // SUPERVISORAGENTLISTENERINTERFACE_H_

