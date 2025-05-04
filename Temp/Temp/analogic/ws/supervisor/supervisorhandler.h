/*!
* @file     supervisorhandler.h
* @author   Agiliad
* @brief    This file contains class declaration for supervisor workstation connections.
* @date     Jul, 11 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORHANDLER_H
#define SUPERVISORHANDLER_H

#include <boost/shared_ptr.hpp>
#include <analogic/ws/common.h>
#include <analogic/ws/nsshandler.h>
#include <analogic/ws/ulm/uilayoutmanager.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>
#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorHandler
 * \brief   This class contains variable and function related to
 *          Supervisor Handler.
 */
class SupervisorHandler: public NssHandler
{
  Q_OBJECT
public:
  /*!
    * @fn       SupervisorHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class SupervisorHandler.
    */
  SupervisorHandler();

  /*!
    * @fn       SupervisorHandler
    * @param    NSSAgentAccessInterface*
    * @return   None
    * @brief    Constructor for class SupervisorHandler,
    *           needed for mocking nss in unit testing.
    */
  explicit SupervisorHandler(NSSAgentAccessInterface* nssaccess);


  /*!
    * @fn       ~SupervisorHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class SupervisorHandler.
    */
  ~SupervisorHandler();

  /*!
    * @fn       getSearchBagList
    * @param    void
    * @return   void
    * @brief    Gets Search BagList.
    */
  // Search List not available in Supervisor Agent
 // void getSearchBagList();

signals:
  /*!
    * @fn       stateChanged
    * @param    AppState
    * @return   None
    * @brief    signal state changed
    */
  void stateChanged(QMLEnums::AppState);

  /*!
    * @fn       bagRetain
    * @param    None
    * @return   None
    * @brief    signal to retain bag
    */
  void bagRetain();

public slots:
  /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
  void onUpdateState(QMLEnums::AppState state);

  /*!
    * @fn       onReviewBag
    * @param    std::string &bhsid
    * @return   None
    * @brief    slot gets called on review bag from BHS
    */
  void onReviewBag(const std::string &bhsid);

};
}  // end of namespace ws
}  // end of namespace analogic

#endif // SUPERVISORHANDLER_H
