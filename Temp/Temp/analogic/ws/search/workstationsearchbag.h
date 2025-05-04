/*!
* @file     workstationsearchbag.h
* @author   Agiliad
* @brief    This file contains interface of SearchBag.
* @date     June, 28 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef WORKSTATIONSEARCHBAG_H
#define WORKSTATIONSEARCHBAG_H

#include <analogic/nss/agent/search/SearchBag.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   WorkstationSearchBag
 * \brief   This class contains structure for search config
 */
class WorkstationSearchBag: public analogic::nss::SearchBag
{
public:
    /*!
    * @fn       WorkstationSearchBag
    * @param    None
    * @return   None
    * @brief    Constructor for class WorkstationSearchBag.
    */
    WorkstationSearchBag();

    /*!
    * @fn       ~WorkstationSearchBag
    * @param    None
    * @return   None
    * @brief    Destructor for class WorkstationSearchBag.
    */
    ~WorkstationSearchBag();

    /*!
    * @fn       ~RemoveFromList
    * @param    None
    * @return   None
    * @brief    Instructs the network that his bag can be removed from the search list.
    */
    //virtual void RemoveFromList();
};
}  // end of namespace ws
}  // end of namespace analogic

#endif // WORKSTATIONSEARCHBAG_H
