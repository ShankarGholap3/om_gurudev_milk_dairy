/*!
* @file     searchmanager.h
* @author   Agiliad
* @brief    This file contains interface, responsible for receiving the search bag list and data from NAL, and providing it to the UI.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_SEARCH_SEARCHMANAGER_H_
#define ANALOGIC_WS_SEARCH_SEARCHMANAGER_H_

#include <boost/shared_ptr.hpp>
#include <analogic/ws/workstationmanager.h>
#include <analogic/ws/search/searchconfig.h>
#include <analogic/ws/search/searchhandler.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{
class RemoteArchiveBagListHandler;
class LocalArchiveBagListHandler;
class SearchArchiveBagListHandler;

/*!
 * \class   SearchManager
 * \brief   This class contains functionality related to receiving search bag list and data from NAL and providing it to the UI
 */
class SearchManager : public WorkstationManager
{
    Q_OBJECT
public:
    /*!
       * @fn       SearchManager
       * @param    None
       * @return   None
       * @brief    Constructor for class SearchManager.
       */
    SearchManager();

    /*!
       * @fn       SearchManager
       * @param    None
       * @return   None
       * @brief    Destructor for class SearchManager.
       */
    virtual ~SearchManager();
    /*!
    * @fn       RegistrationChanged
    * @param    analogic::nss::OsrAgent* - agent
    * @return   None
    * @brief    Gives Registration Changed status of the workstation to the
    *           nss service.
    */
    void RegistrationChanged(analogic::nss::SearchAgent *agent);

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
    * @return   int (Errors::S_OK -Successful /Errors::E_FAIL -failure)
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



private:
    /*!
    * @fn       createSearchAgent
    * @param    None
    * @return   int - Agent creation status (0/1 - success/failure).
    * @brief    Creates Search agent.
    */
    int createSearchAgent();

private:
    boost::shared_ptr<SearchArchiveBagListHandler>    m_searchArchiveBagListHandler;   //!< handle to search archive bag list
    boost::shared_ptr<RemoteArchiveBagListHandler>    m_remoteArchiveBagLHndlr;        //!< Handle to remote archive.
    boost::shared_ptr<LocalArchiveBagListHandler>     m_localArchiveBagLHndlr;         //!< Handle to local archive.
//    boost::shared_ptr<analogic::ancp00::InternalBhsBagInterface> m_BHSInterface;       //!< BHS interface

};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_SEARCH_SEARCHMANAGER_H_
