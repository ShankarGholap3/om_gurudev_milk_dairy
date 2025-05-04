/*!
* @file     searchhandler.h
* @author   Agiliad
* @brief    This file contains class declaration for search workstation connections.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_SEARCH_SEARCHHANDLER_H_
#define ANALOGIC_WS_SEARCH_SEARCHHANDLER_H_

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
 * \class   SearchHandler
 * \brief   This class contains variable and function related to
 *          Search Handler.
 */
class SearchHandler: public NssHandler
{
    Q_OBJECT
public:
    /*!
    * @fn       SearchHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class SearchHandler.
    */
    SearchHandler();

    /*!
    * @fn       SearchHandler
    * @param    NSSAgentAccessInterface*
    * @return   None
    * @brief    Constructor for class SearchHandler,
    *           needed for mocking nss in unit testing.
    */
    explicit SearchHandler(NSSAgentAccessInterface* nssaccess);


    /*!
    * @fn       ~SearchHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class SearchHandler.
    */
    ~SearchHandler();

    /*!
    * @fn       getSearchBagList
    * @param    void
    * @return   void
    * @brief    Gets Search BagList.
    */
    void getSearchBagList();

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

#endif  // ANALOGIC_WS_SEARCH_SEARCHHANDLER_H_

