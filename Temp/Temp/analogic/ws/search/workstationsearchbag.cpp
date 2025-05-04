/*!
* @file     workstationsearchbag.cpp
* @author   Agiliad
* @brief    This file contains interface of SearchBag.
* @date     June, 28 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/search/workstationsearchbag.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       WorkstationSearchBag
* @param    None
* @return   None
* @brief    Constructor for class WorkstationSearchBag.
*/
WorkstationSearchBag::WorkstationSearchBag(): analogic::nss::SearchBag()
{

}

/*!
* @fn       ~WorkstationSearchBag
* @param    None
* @return   None
* @brief    Destructor for class WorkstationSearchBag.
*/
WorkstationSearchBag::~WorkstationSearchBag()
{

}

/*!
* @fn       ~RemoveFromList
* @param    None
* @return   None
* @brief    Instructs the network that his bag can be removed from the search list.
*/
//void WorkstationSearchBag::RemoveFromList()
//{
//    analogic::nss::SearchBag::RemoveFromList();
//}


}  // end of namespace ws
}  // end of namespace analogic
