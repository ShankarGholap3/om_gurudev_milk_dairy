/*!
* @file     rerunmanager.h
* @author   Agiliad
* @brief    This file contains interface, responsible rerun emulator.
* @date     March, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_RERUN_RERUNMANAGER_H_
#define ANALOGIC_WS_RERUN_RERUNMANAGER_H_

#include <boost/scoped_ptr.hpp>
#include <analogic/ws/workstationmanager.h>
#include <analogic/ws/rerun/rerunhandler.h>
#include <analogic/ws/rerun/rerunviewerhandler.h>
#include <analogic/ws/rerun/rerundataacqhandler.h>
#include <analogic/ws/common/utility/usbstorageservice.h>
#include <analogic/ws/common/utility/customfiledialog.h>
#include <analogic/ws/rerun/rerunconfig.h>
#include <analogic/ws/common/utility/connectemulator.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   RerunManager
 * \brief   This class contains functionality related to rerun emulator with detection algorithms
*/

class RerunManager : public WorkstationManager
{
    Q_OBJECT
public:
    /*!
       * @fn       RerunManager
       * @param    None
       * @return   None
       * @brief    Constructor for class RerunManager.
       */
    RerunManager();

    /*!
       * @fn       RerunManager
       * @param    None
       * @return   None
       * @brief    Destructor for class RerunManager.
       */
    virtual ~RerunManager();

    /*!
    * @fn       getRerunHandler
    * @param    None
    * @return   boost::shared_ptr<RerunHandler> - RerunHandler
    * @brief    Gets rerun handler
    */
    boost::shared_ptr<RerunHandler> getRerunHandler()
    {
        return m_rerunHandler;
    }
    /*!
    * @fn       updateAlgorithmList
    * @param    None
    * @return   None
    * @brief    get available algorithmsv from  rerun libs api and update the list.
    */
    void updateAlgorithmList();


    /*!
    * @fn       getRerunViewerHandler
    * @param    None
    * @return   boost::shared_ptr<RerunViewerHandler> - archive handler
    * @brief    Gets local RerunViewer handler
    */
    virtual boost::shared_ptr<RerunViewerHandler> getRerunViewerHandler();

    /*!
    * @fn       getRerunDataAcqHandler
    * @param    None
    * @return   boost::shared_ptr<RerunDataAcqHandler> - data aqc handler
    * @brief    Gets local RerunDatAcq handler
    */
    virtual boost::shared_ptr<RerunDataAcqHandler> getRerunDataAcqHandler();

    /*!
    * @fn       getdetectionAlgList
    * @param    None
    * @return   QStringList*
    * @brief    Gets algorithm list
    */
    QStringList* getdetectionAlgList()
    {
        return m_detectionAlgList;
    }

signals:
    /*!
    * @fn       stateChanged();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    void stateChanged(QMLEnums::AppState state);

    /*!
    * @fn       deAuthenticate
    * @param    None
    * @return   None
    * @brief    signal to propagate call for Deauthenticate user
    */
    void deAuthenticate();

public slots:
    /*!
    * @fn       onCommandButtonEvent();
    * @param    QMLEnums::ModelEnum model
    * @param    QString data
    * @return   None
    * @brief    This function is responsible to process model data.
    */
    void onCommandButtonEvent(QMLEnums::ModelEnum model, QString data);

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
    * @fn       mock_init
    * @param    RerunHandler pointer
    * @return   None
    * @brief    Mocking init method to run test suites.
    */
    void mock_init(RerunHandler *pRerunHandler);

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
    * @fn       onRerunCommand
    * @param    QMLEnums::RERUN_COMMANDS cmd
    * @param    RerunTaskConfig taskConfig
    * @return   None
    * @brief    slot to execute rerun detection
    */
    void onRerunCommand(QMLEnums::RERUN_COMMANDS cmd,RerunTaskConfig taskConfig);

    /*!
    * @fn       onDeAuthenticate
    * @param    None
    * @return   None
    * @brief    Slot calls on deauthentication
    */
    void onDeAuthenticate();

protected:
private:
    /*!
    * @fn       authenticate
    * @param    QString username
    * @param    QString password
    * @return   QMLEnums::WSAuthenticationResult
    * @brief    Authenticate user
    */
    QMLEnums::WSAuthenticationResult  authenticate(QString username, QString password);

    QString                                 m_username;            //!< user name
    QString                                 m_password;            //!< Password
    QStringList                            *m_detectionAlgList;    //!< available detection algorithm list
    boost::shared_ptr<RerunHandler>         m_rerunHandler;        //!< handle to RerunHandler
    boost::shared_ptr<RerunViewerHandler>   m_rerunviewerHandler;  //!< handle to ReviewHandler
    boost::shared_ptr<RerunDataAcqHandler>  m_rerundataacqHandler; //!< handle to Review Data Acq Handler

};

}  // end of namespace ws
}  // end of namespace analogic
#endif // ANALOGIC_WS_RERUNMANAGER_H_
