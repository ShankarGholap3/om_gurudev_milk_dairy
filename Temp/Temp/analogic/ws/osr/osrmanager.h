/*!
* @file     osrmanager.h
* @author   Agiliad
* @brief    This file contains functions related to OsrManager which communicates with nss service to authenticate user and get live bag data.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_OSR_OSRMANAGER_H_
#define ANALOGIC_WS_OSR_OSRMANAGER_H_

#include <semaphore.h>
#include <boost/scoped_ptr.hpp>
#include <analogic/ws/common.h>
#include <analogic/ws/nsshandler.h>
#include <analogic/ws/osr/osrconfig.h>
#include <analogic/ws/osr/osrhandler.h>
#include <analogic/ws/workstationmanager.h>
#include <analogic/ws/common/utility/xmlservice.h>
#include <analogic/ws/osr/osragentlistenerinterface.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{
class RemoteArchiveBagListHandler;
class LocalArchiveBagListHandler;

/*!
 * \class   OsrManager
 * \brief   This class contains functionality related to user authentication and receiving new bag list and data from NAL and providing it to the UI.
 */
class OsrManager : public WorkstationManager
{
    Q_OBJECT
public:
    /*!
       * @fn       OsrManager
       * @param    None
       * @return   None
       * @brief    Constructor for class OsrManager.
       */
    OsrManager();

    /*!
       * @fn       OsrManager
       * @param    None
       * @return   None
       * @brief    Destructor for class OsrManager.
       */
    virtual ~OsrManager();

    /*!
    * @fn       RegistrationChanged
    * @param    analogic::nss::OsrAgent* - agent
    * @return   None
    * @brief    Gives Registration Changed status of the workstation to the nss service.
    */
    void RegistrationChanged(analogic::nss::OsrAgent* agent);

signals:
    /*!
    * @fn       stateChanged();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    void stateChanged(QMLEnums::AppState state);

public slots:
    /*!
    * @fn       onCommandButtonEvent();
    * @param    QMLEnums::ModelEnum model
    * @param    QString data
    * @return   None
    * @brief    This function is responsible to process model data.
    */
    void onCommandButtonEvent(QMLEnums::ModelEnum, QString data);

    /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    void onUpdateState(QMLEnums::AppState state);

    /*!
    * @fn       init
    * @param    None
    * @return   None
    * @brief    Calls NssHandler init for intialization of nss and connection agent.
    */
    void init();

    /*!
    * @fn       onNetworkUpdate
    * @param    None
    * @return   None
    * @brief    This slot gets call if networks gets disconnected.
    */
    void onNetworkUpdate();
    /*!
    * @fn       onExit
    * @param    None
    * @return   None
    * @brief    Slot calls on exit if application.
    */
    void onExit();

    /*!
    * @fn      onNetworkStatus
    * @param    QMLEnums::NetworkState
    * @return   None
    * @brief    notifies about status of network.
    */
    void onNetworkStatus(QMLEnums::NetworkState status);

    /*!
    * @fn       onScannerNetworkUpdate
    * @param    None
    * @return   None
    * @brief    This slot gets call if scanner network gets updated
    */
    void onScannerNetworkUpdate();

    /*!
    * @fn       bagListCommand
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @param    QVariant data
    * @return   None
    * @brief    Command to process.
    */
    void bagListCommand(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                        QMLEnums::BagsListAndBagDataCommands command, QVariant data);
protected:
private:
    /*!
    * @fn       createOsrAgent
    * @param    None
    * @return   int - Agent creation status (0/1 - success/failure).
    * @brief    Creates Osr agent.
    */
    int createOsrAgent();

    boost::shared_ptr<RemoteArchiveBagListHandler>      m_remoteArchiveBagLHndlr;      //!< Handle to remote archive.
    boost::shared_ptr<LocalArchiveBagListHandler>       m_localArchiveBagLHndlr;       //!< Handle to local archive.
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_OSR_OSRMANAGER_H_

