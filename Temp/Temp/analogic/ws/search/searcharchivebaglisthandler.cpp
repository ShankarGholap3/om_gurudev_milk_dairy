/*!
* @file     searcharchivebaglisthandler.cpp
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from remote system.
* @date     Feb, 24 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/search/searcharchivebaglisthandler.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       SearchArchiveBagListHandler
* @param    None
* @return   None
* @brief    Constructor for class SearchArchiveBagListHandler.
*/
SearchArchiveBagListHandler::SearchArchiveBagListHandler()
{

}

/*!
* @fn       ~SearchArchiveBagListHandler
* @param    None
* @return   None
* @brief    Destructor for class SearchArchiveBagListHandler.
*/
SearchArchiveBagListHandler::~SearchArchiveBagListHandler()
{
}

/*!
* @fn       fetchBagInfoList
* @param    QVariant
* @return   None
* @brief    Load bag info list.
*/
void SearchArchiveBagListHandler::fetchBagInfoList(QVariant data)
{
    TRACE_LOG("");
    // store in m_bagInfoList the list received from nss
    m_bagInfoList.clear();
    m_bagInfoList = data.value<QList<BagInfo>>();
}

/*!
* @fn       showBag
* @param    QVariant input
* @return   None
* @brief    Show the selected bag.
*/
void SearchArchiveBagListHandler::showBag(QVariant input)
{
    TRACE_LOG("");
    emit getBagDataFromServer(QMLEnums::ArchiveFilterSrchLoc::SEARCH_SERVER,
                              input.toString().toStdString(),QMLEnums::BagsListAndBagDataCommands::BLBDC_SHOW_BAG);    
}

/*!
* @fn       getBagData
* @param    boost::shared_ptr<BagData> bag
* @param    const int& rowNumber
* @return   None
* @brief    Fetch the bag data.
*/
bool SearchArchiveBagListHandler::getBagData(boost::shared_ptr<BagData> bag,
                                             const int& rowNumber)
{
    Q_UNUSED(bag);
    Q_UNUSED(rowNumber);
    return true;
}

}  // end of namespace ws
}  // end of namespace analogic
