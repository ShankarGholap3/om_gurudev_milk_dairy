/*!
* @file     networkfaulthandler.h
* @author   Agiliad
* @brief    This file contains interface, responsible for getting workstation heartbeat signal and sending it to remote monitor.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_FAULTS_NETWORKFAULTHANDLER_H_
#define ANALOGIC_WS_COMMON_FAULTS_NETWORKFAULTHANDLER_H_

#include <QTimer>
#include <QDateTime>
#include <boost/shared_ptr.hpp>

#include <analogic/ws/common.h>
#include <analogic/ws/common/faults/ifaulthandler.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   NetworkFaultHandler
 * \brief   This class contains variable and function related to
 *          Network Fault Handler.
 */
class NetworkFaultHandler : public IFaultHandler
{
    Q_OBJECT
public:
    /*!
    * @fn       NetworkFaultHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class NetworkFaultHandler.
    */
    NetworkFaultHandler();

    /*!
    * @fn       ~NetworkFaultHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class NetworkFaultHandler.
    */
    ~NetworkFaultHandler();

    /*!
    * @fn       getNetworkState
    * @param    None
    * @return   QMLEnums::NetworkState
    * @brief    Returns current network state.
    */
    QMLEnums::NetworkState getNetworkState();

public slots:
    /*!
    * @fn       networkStatus
    * @param    QMLEnums::NetworkState status
    * @return   None
    * @brief    notifies about status of network from nss service.
    */
    void networkStatus(QMLEnums::NetworkState status);

    /*!
    * @fn       onTimeout
    * @param    None
    * @return   None
    * @brief    slot will call on timeout
    */
    void onTimeout();

    /*!
    * @fn      onExit
    * @param   None
    * @return  None
    * @brief   slot call on exit
    */
    void onExit();

signals:
    /*!
    * @fn       updateNetwork
    * @param    None
    * @return   None
    * @brief    update Network
    */
    void updateNetwork();
private:
    int                         m_timerinterval;           //!< Network check interval
    QMLEnums::NetworkState      m_nwstate;                 //!< handle to network state
    boost::shared_ptr<QTimer>   m_pNetworkStatusTimer;     //!< handle to timer
};
}  // end of namespace ws
}  // end of namespace analogic


#endif  // ANALOGIC_WS_COMMON_FAULTS_NETWORKFAULTHANDLER_H_

