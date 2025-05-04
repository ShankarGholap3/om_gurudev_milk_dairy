/*!
* @file     remotebaglisthandler.cpp
* @author   Agiliad
* @brief    This file contains class and functions for handling baglist from different sources for remote archive and search .
*           and network server.
* @date     Jan, 24 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDir>
#include <analogic/ws/common.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/bagdata/remotebaglisthandler.h>
namespace analogic
{
namespace ws
{

/*!
* @fn       RemoteBagListHandler();
* @param    None
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
RemoteBagListHandler::RemoteBagListHandler()
{
}

/*!
* @fn       ~RemoteBagListHandler();
* @param    None
* @return   None
* @brief    Destructor responsible for releasing the acquired resources.
*/
RemoteBagListHandler::~RemoteBagListHandler()
{
    m_bagInfoList.clear();
}

/*!
* @fn       getBagData
* @param    boost::shared_ptr<BagData> bag
* @param    const int& rowNumber
* @return   None
* @brief    Fetch the bag data.
*/
bool RemoteBagListHandler::getBagData(boost::shared_ptr<BagData> bag,
                                      const int& rowNumber)
{
    if(rowNumber < 0 || rowNumber >= m_bagInfoList.count())
    {
        ERROR_LOG("Index from UI not in list range: " << rowNumber << " " <<
                  m_bagInfoList.count());
        return false;
    }
    DEBUG_LOG("RowNumber:"<< rowNumber);
    QString fileNamePattern = m_bagInfoList[rowNumber].m_fileNamePattern;
    return bag->constructFromFiles(fileNamePattern, fileNamePattern.section("/", 0, -2));
}
}  // end of namespace ws
}  // end of namespace analogic
