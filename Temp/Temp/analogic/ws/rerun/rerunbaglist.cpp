/*!
* @file     rerunbaglist.cpp
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from local system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common.h>
#include <analogic/ws/rerun/rerunbaglist.h>
#include <analogic/ws/common/bagdata/bagdata.h>


namespace analogic
{
namespace ws
{
/*!
* @fn       RerunBagList
* @param    None
* @return   None
* @brief    Constructor for class RerunBagList.
*/
RerunBagList::RerunBagList()
{
}

/*!
* @fn       ~RerunBagList
* @param    None
* @return   None
* @brief    Destructor for class RerunBagList.
*/
RerunBagList::~RerunBagList()
{
}

/*!
* @fn       getBagData
* @param    boost::shared_ptr<BagData> bag
* @param    const QString bagDataFilePath
* @param    const QString outputFolderPath
* @return   None
* @brief    Fetch the bag data.
*/
bool RerunBagList::getBagData(boost::shared_ptr<BagData> bag, const QString bagDataFilePath, const QString outputFolderPath)
{
    if(bagDataFilePath.isEmpty())
    {
        ERROR_LOG("BagDataFilePath from UI is empty: " << bagDataFilePath.toStdString());
        return false;
    }
    DEBUG_LOG("BagDataFilePath: "<< bagDataFilePath.toStdString());
    return bag->constructFromFiles(bagDataFilePath, outputFolderPath);
}
}  // end of namespace ws
}  // end of namespace analogic
