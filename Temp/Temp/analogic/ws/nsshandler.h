/*!
* @file     nsshandler.h
* @author   Agiliad
* @brief    This file contains functions related to NssHandler
*           which manages interface between workstation and Nss Access interface.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_NSSHANDLER_H_
#define ANALOGIC_WS_NSSHANDLER_H_

#include <boost/shared_ptr.hpp>

#include <QObject>
#include <QVariant>

#include <analogic/ws/common.h>
#include <analogic/ws/osr/osrconfig.h>
#include <analogic/ws/ulm/uilayoutmanager.h>
#include <analogic/ws/common/faults/networkfaulthandler.h>
#include <analogic/nss/agent/authentication/AuthMethodType.h>
#include <analogic/ws/common/accesslayer/scanneraccessinterface.h>
#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class NSSAgentAccessInterface;
/*!
 * \class   NssHandler
 * \brief   This class contains variable and function related to
 *          managing  interface between workstation and Nss Access interface.
 */

class NssHandler: public QObject
{
  Q_OBJECT
public:
  /*!
    * @fn       NssHandler();
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
  NssHandler();
  /*!
    * @fn       NssHandler();
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
  explicit NssHandler(NSSAgentAccessInterface* nssAccess);

  /*!
    * @fn       ~NssHandler
    * @param    None
    * @return   None
    * @brief    Destructor responsible for deinitialization of members, memory and resources.
    */
  virtual ~NssHandler();

  /*!
    * @fn       init
    * @param    None
    * @return   int (Errors::S_OK -Successful /Errors::E_FAIL -failure)
    * @brief    Initializes nss, connection agent and authentication agent
    */
  int init();

  /*!
    * @fn       Authenticate
    * @param    QString username
    * @param    Qstring password
    * @return   QMLEnums::WSAuthenticationResult
    * @brief    Authenticate user on the connection.
    */
  virtual  QMLEnums::WSAuthenticationResult authenticate(QString username,
                                                         const std::map<analogic::nss::AuthMethodType, std::string>& auth_methods);

  /*!
    * @fn       changePassword
    * @param    const std::string& - username
    * @param    const std::string& - oldAuthMethodDetail
    * @param    const std::string& - newAuthMethodDetail
    * @param    QMLEnums::AuthMethodType - authMethodType
    * @return   QMLEnums::WSBasicCommandResult - result
    * @brief    Changes password of the user to new password.
    */
  virtual QMLEnums::WSBasicCommandResult changePassword(const std::string& username,
                                                        const std::string& oldAuthMethodDetail, const std::string& newAuthMethodDetail, QMLEnums::AuthMethodType authMethodType);

  /*!
    * @fn       createWSAgent
    * @param    analogic::nss::OsrAgentStaticProperties properties
    * @return   int
    * @brief    Creates OSR Agent.
    */
  int createWSAgent( analogic::nss::OsrAgentStaticProperties properties);

  /*!
    * @fn       createWSAgent
    * @param    analogic::nss::SearchAgentStaticProperties properties
    * @return   int
    * @brief    Creates Search Agent.
    */
  int createWSAgent( analogic::nss::SearchAgentStaticProperties properties);

  /*!
    * @fn       createWSAgent
    * @param    analogic::nss::SupervisorAgentStaticProperties properties
    * @return   int
    * @brief    Creates SupervisorAgent.
    */
  int createWSAgent(analogic::nss::SupervisorAgentStaticProperties properties);

  /*!
    * @fn       setCredentialsOfWSAgent
    * @param    None
    * @return   None
    * @brief    set Credentials of OSR/ SEARCH agent.
    */
  void setCredentialsOfWSAgent();

  /*!
  * @fn       CreateScannerOperationAgent
  * @param    const std::string&  scannername
  * @return   int- Agent creation status (S_OK/E_POINTER - success/failure).
  * @brief    Creates Osr/ Search Scanner Operation agent.
  */
  int CreateScannerOperationAgent(const std::string& name);

  /*!
  * @fn       fetchRemoteArchiveList
  * @param    QVariant searchFields
  * @return   None
  * @brief    fetches RemoteArchive list from nss
  */
  void fetchRemoteArchiveList(QVariant searchFields);

public slots:
  /*!
    * @fn       onDeAuthenticate
    * @param    None
    * @return   None
    * @brief    Call for Deauthenticate user
    */
  void onDeAuthenticate();

  /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
  virtual void onUpdateState(QMLEnums::AppState state) = 0;


signals:

  /*!
    * @fn       bagNotification();
    * @param    analogic::nss::NssBag* bag
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @param    string data
    * @param    QMLEnums::BagsListAndBagDataCommands mode
    * @return   None
    * @brief    This function is responsible to notify bagacqdatahandler about
    *           the bag arrival and get the bag data.
    */
  void bagNotification(analogic::nss::NssBag* bag, QMLEnums::ArchiveFilterSrchLoc, std::string data,
                       QMLEnums::BagsListAndBagDataCommands mode);

  /*!
    * @fn       stateChanged
    * @param    AppState
    * @return   None
    * @brief    signal state changed
    */
  void stateChanged(QMLEnums::AppState);

  /*!
    * @fn       bagListUpdated();
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @param    QVariant
    * @return   None
    * @brief    This function is responsible to notify bagListHandler about
    *           the bag list arrival.
    */
  void bagListUpdated(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                      QMLEnums::BagsListAndBagDataCommands command, QVariant results);

  /*!
    * @fn       getBagDataFromServer
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @param    std::string nss_bag_id
    * @param    QMLEnums::BagsListAndBagDataCommands mode
    * @return   None
    * @brief    This function is responsible to notify nssAgentAccess about bagData requests
    */
  void getBagDataFromServer(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                            std::string nss_bag_id,QMLEnums::BagsListAndBagDataCommands mode);

  /*!
     * @fn      displayMsg
     * @param   int - errorcode
     * @param   QString - status
     * @param   int - model
     * @return  void
     * @brief   signal for display msg
     */
  void displayMsg(int hr, QString status, int model);

  /*!
    * @fn       notifyToChangeOrNotScreen
    * @param    bool toBeChanged
    * @param    QString errorMsg
    * @param    int hr
    * @return   None
    * @brief    notifies to change screen or not.
    */
  void notifyToChangeOrNotScreen(bool toBeChanged, QString errorMsg, int hr);


protected:
  NSSAgentAccessInterface*    m_nssAccesslayer;    //!< handle to NSS agent access interface

private:
  /*!
    * @fn       createConnectionAgent
    * @param    None
    * @return   int (Errors::S_OK -Successful /Errors::E_FAIL -failure)
    * @brief    Creates connection agent and connects to nss.
    */
  int createConnectionAgent();

  /*!
    * @fn       createAuthenticationAgent
    * @param    None
    * @return   int (Errors::S_OK -Successful /Errors::E_FAIL -failure)
    * @brief    Creates Authentication agent to authenticate user on the connection.
    */
  int createAuthenticationAgent();

  QString                                 m_nssaddress;                   //!< Nss service address
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_NSSHANDLER_H_

