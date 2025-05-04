/*!
* @file     localarchive.cpp
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from local system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDir>
#include <analogic/ws/common.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/common/archive/localarchive.h>


namespace analogic
{
namespace ws
{
/*!
* @fn       LocalArchive
* @param    WorkstationManager* wsm
* @return   None
* @brief    Constructor for class LocalArchive.
*/
LocalArchive::LocalArchive(WorkstationManager* wsm) :
    Archive(wsm)
{
}

/*!
* @fn       ~LocalArchive
* @param    None
* @return   None
* @brief    Destructor for class LocalArchive.
*/
LocalArchive::~LocalArchive()
{
}

/*!
* @fn       deleteBag
* @param    QStringList input
* @return   bool success
* @brief    Delete the locally archived bag.
*/
bool LocalArchive::deleteBag(QStringList input)
{
    if(input.count() != 1)
    {
        ERROR_LOG("Argument count to deleteBag incorrect: " << input.count());
        return false;
    }

    int rowNumber = input[0].toInt();;

    if(rowNumber < 0 || rowNumber >= m_bagInfoList.count())
    {
        ERROR_LOG("Index from UI not in list range: " << rowNumber << " " << m_bagInfoList.count());
        return false;
    }

    QString bagFolderPath = m_bagInfoList[rowNumber].m_path;
    QDir dir(bagFolderPath);
    if(!dir.exists())
    {
        ERROR_LOG("Folder doesn't exist: " << bagFolderPath.toStdString());
        return false;
    }

    bool success = dir.removeRecursively();
    if(!success)
    {
       ERROR_LOG("Failed to delete: " << bagFolderPath.toStdString());
    }
    else
        m_bagInfoList.removeAt(rowNumber);

    return success;
}

/*!
* @fn       fetchBagInfoList
* @param    None
* @return   None
* @brief    Load bag info list.
*/
void LocalArchive::fetchBagInfoList()
{
    m_bagInfoList.clear();

    QString localArchiveFolderPath = WorkstationConfig::getExportBagDataPath();

    if(!QDir(localArchiveFolderPath.toStdString().c_str()).exists())
    {
        ERROR_LOG("Local archive path does not exist: " << localArchiveFolderPath.toStdString());
        return;
    }

    int maxCount = WorkstationConfig::getExportBagDataMaxCount();

    for(int slot = 0; slot < maxCount; slot++)
    {
        std::string bagDataFolderPath = localArchiveFolderPath.toStdString();
        bagDataFolderPath += "/Bag_";

        char num[6];
        std::snprintf(num, sizeof(num), "%03d", slot);
        bagDataFolderPath += num;

        if(!QDir(bagDataFolderPath.c_str()).exists())
            continue;

        std::string infoFileName = bagDataFolderPath + "/info.txt";
        QFile fname(infoFileName.c_str());
        if(fname.exists())
        {
            BagInfo bagInfo;
            bagInfo.constructFromFile(fname.fileName());
            bagInfo.m_path = bagDataFolderPath.c_str();

            m_bagInfoList.append(bagInfo);
        }
    }
    DEBUG_LOG("Locally archived bags count: " << m_bagInfoList.count());
}

/*!
* @fn       getBagData
* @param    boost::shared_ptr<BagData> bag
* @param    const int& rowNumber
* @return   None
* @brief    Fetch the bag data.
*/
bool LocalArchive::getBagData(boost::shared_ptr<BagData> bag, const int& rowNumber)
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
