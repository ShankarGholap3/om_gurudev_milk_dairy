/*!
* @file     osrhandler.h
* @author   Agiliad
* @brief    This file contains class declaration for osr connections.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_OSR_OSRHANDLER_H_
#define ANALOGIC_WS_OSR_OSRHANDLER_H_

#include <boost/shared_ptr.hpp>
#include <analogic/ws/common.h>
#include <analogic/ws/nsshandler.h>
#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   OsrHandler
 * \brief   This class contains functionality related to OSR handler - scanner agent ,
 *          osr agent , OSR state and bag notification
 */
class OsrHandler: public NssHandler
{
    Q_OBJECT
public:
    /*!
    * @fn       OsrHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class OsrHandler.
    */
    OsrHandler();

    /*!
    * @fn       OsrHandler
    * @param    NSSAgentAccessInterface*
    * @return   None
    * @brief    Constructor for class OsrHandler , needed for mocking nss in unit testing.
    */
    explicit OsrHandler(NSSAgentAccessInterface* nssaccess);

    /*!
    * @fn       ~OsrHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class NetworkFaultHandler.
    */
    ~OsrHandler();

    /*!
    * @fn       setOsrReadyState
    * @param    bool
    * @return   int
    * @brief    Sets OSR ready state.
    */
    int setOsrReadyState(bool state);



signals:
    /*!
    * @fn       stateChanged
    * @param    AppState
    * @return   None
    * @brief    signal state changed
    */
    void stateChanged(QMLEnums::AppState);

public slots:
    /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    void onUpdateState(QMLEnums::AppState state);

protected:
private:
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_OSR_OSRHANDLER_H_

