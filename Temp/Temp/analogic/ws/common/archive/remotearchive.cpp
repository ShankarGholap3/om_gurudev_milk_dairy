/*!
* @file     remotearchive.cpp
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from remote system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/common/archive/remotearchive.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       RemoteArchive
* @param    WorkstationManager* wsm
* @return   None
* @brief    Constructor for class RemoteArchive.
*/
RemoteArchive::RemoteArchive(WorkstationManager* wsm) :
    Archive(wsm)
{
}

/*!
* @fn       ~RemoteArchive
* @param    None
* @return   None
* @brief    Destructor for class RemoteArchive.
*/
RemoteArchive::~RemoteArchive()
{
}

/*!
* @fn       deleteBag
* @param    QStringList input
* @return   bool success
* @brief    Delete the locally archived bag.
*/
bool RemoteArchive::deleteBag(QStringList input)
{
    ERROR_LOG("Trying to delete a remote archive bag failed.");
    return false;
}
/*!
* @fn       fetchBagInfoList
* @param    None
* @return   None
* @brief    Load bag info list.
*/
void RemoteArchive::fetchBagInfoList()
{
    m_bagInfoList = fetchBaglist();
    DEBUG_LOG("Remote archived bags count: " << m_bagInfoList.count());
}

/*!
* @fn       getBagData
* @param    boost::shared_ptr<BagData> bag
* @param    const int& rowNumber
* @return   None
* @brief    Fetch the bag data.
*/
bool RemoteArchive::getBagData(boost::shared_ptr<BagData> bag, const int& rowNumber)
{
    if(rowNumber < 0 || rowNumber >= m_bagInfoList.count())
    {
        ERROR_LOG("Index from UI not in list range: " << rowNumber << " " << m_bagInfoList.count());
        return false;
    }
    DEBUG_LOG("RowNumber:"<< rowNumber);
    return bag->constructFromFiles(m_bagInfoList[rowNumber].m_path.toStdString());
}

}  // end of namespace ws
}  // end of namespace analogic
