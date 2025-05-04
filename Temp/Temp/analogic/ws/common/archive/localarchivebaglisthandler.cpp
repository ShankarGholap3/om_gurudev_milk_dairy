/*!
* @file     localarchivebaglisthandler.cpp
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from local system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDir>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/archive/localarchivebaglisthandler.h>


namespace analogic
{
namespace ws
{
/*!
* @fn       LocalArchiveBagListHandler
* @param    None
* @return   None
* @brief    Constructor for class LocalArchiveBagListHandler.
*/
LocalArchiveBagListHandler::LocalArchiveBagListHandler()
{
}

/*!
* @fn       ~LocalArchiveBagListHandler
* @param    None
* @return   None
* @brief    Destructor for class LocalArchiveBagListHandler.
*/
LocalArchiveBagListHandler::~LocalArchiveBagListHandler()
{
}

/*!
* @fn       deleteBag
* @param    QVariant input
* @return   bool success
* @brief    Delete the locally archived bag.
*/
bool LocalArchiveBagListHandler::deleteBag(QVariant input)
{
    QString data;
    if (input.canConvert<QString>())
    {
        data = input.value<QString>();
    }
    else
    {
        ERROR_LOG("Recieved data for LocalArchive command is incorrect.");
        return false;
    }
    int index = 0;
    bool delSuccess;
    for (; index < m_bagInfoList.count(); index++) {
        if(m_bagInfoList[index].m_fileNamePattern.compare(data) == 0)
        {
            QString bagFolderPath = m_bagInfoList[index].m_fileNamePattern.section("/", 0, -2);
            QDir dir(bagFolderPath);
            if(!dir.exists())
            {
                ERROR_LOG("Folder doesn't exist: " << bagFolderPath.toStdString());
                return false;
            }

            QString bagFileNamePattern = m_bagInfoList[index].m_fileNamePattern.section("/", -1, -1);

            delSuccess = dir.remove(bagFileNamePattern+BAGFILE_VOLUME_FORMAT);
            if(!delSuccess) {
               ERROR_LOG("Failed to delete: " << (bagFileNamePattern+BAGFILE_VOLUME_FORMAT).toStdString());
               break;
            }
            delSuccess = dir.remove(bagFileNamePattern+BAGFILE_PROPERTY_FORMAT);
            if(!delSuccess) {
               ERROR_LOG("Failed to delete: " << (bagFileNamePattern+BAGFILE_PROPERTY_FORMAT).toStdString());
               break;
            }
            delSuccess = dir.remove(bagFileNamePattern+BAGFILE_DICOSCT_FORMAT+".dcs");
            if(!delSuccess) {
               ERROR_LOG("Failed to delete: " << (bagFileNamePattern+BAGFILE_DICOSCT_FORMAT+".dcs").toStdString());
               break;
            }
            dir.remove(bagFileNamePattern+BAGFILE_DICOSCT_PROJ00_FORMAT+".dcs");
            dir.remove(bagFileNamePattern+BAGFILE_DICOSCT_PROJ90_FORMAT+".dcs");

            int i = 1;
            bool tdrFilePresent = true;
            QString tdrFileName;
            do {
                tdrFileName = bagFileNamePattern;
                tdrFileName += BAGFILE_DICOSTDR_FORMAT;
                tdrFileName += QString::number(i);
                tdrFileName += ".dcs";
                tdrFilePresent = dir.remove(tdrFileName);
                i++;
            }while (tdrFilePresent);
            break;
        }
    }
    if(index < 0 || index >= m_bagInfoList.count())
    {
        ERROR_LOG("displayedBagId from UI not in list range: " <<
                  data.toStdString() << " " << m_bagInfoList.count());
        return false;
    }
    else
    {
        m_bagInfoList.removeAt(index);
    }

    return true;
}

/*!
* @fn       fetchBagInfoList
* @param    QVariant
* @return   None
* @brief    Load bag info list.
*/
void LocalArchiveBagListHandler::fetchBagInfoList(QVariant data)
{
    m_bagInfoList.clear();

    QString localArchiveFolderPath = WorkstationConfig::getInstance()->getExportBagDataPath();

    QString nameFilter = "*";
    nameFilter += BAGFILE_PROPERTY_FORMAT;
    QDir archiveBagDir(localArchiveFolderPath.toStdString().c_str(), nameFilter);
    if(!archiveBagDir.exists())
    {
        ERROR_LOG("Local archive path does not exist: " << localArchiveFolderPath.toStdString());
        return;
    }

    QStringList fileNameList = archiveBagDir.entryList();
    foreach (QString fInfo, fileNameList)
    {
        std::string bagDataFile = localArchiveFolderPath.toStdString();
        bagDataFile += "/";
        bagDataFile += fInfo.toStdString();

        QFile fname(bagDataFile.c_str());
        BagInfo bagInfo;
        bagInfo.constructFromFile(fname.fileName());
        bagInfo.m_fileNamePattern = QString::fromStdString(bagDataFile).section(".", 0, -2);

        std::string bagTdrFile = QString::fromStdString(bagDataFile).section(".", 0, -2).toStdString();
        bagTdrFile += BAGFILE_DICOSTDR_FORMAT;
        bagTdrFile += "1";
        bagTdrFile += ".dcs";

        SDICOS::ErrorLog errorLog;
        SDICOS::TDRModule* tdrModule = new SDICOS::TDRModule();

        SDICOS::Filename filename;
        filename.Set(bagTdrFile);

        bool result = tdrModule->Read(filename, errorLog);

        if(!result)
        {
            QString msg = QString::fromStdString("DICOS TDR file read failed: %1. Error:%2.").
                    arg(QString::fromStdString(bagTdrFile), errorLog.GetErrorLog().Get());
            LOG(SEVERITY_LEVEL::ERROR) << msg.toStdString();
        }
        else
        {
            SDICOS::TDRTypes::ThreatDetectionReport tdrreport = tdrModule->GetThreatDetectionReport();
            bagInfo.machine_alarm_decision = tdrreport.GetAlarmDecision();
        }
        SAFE_DELETE(tdrModule)
        m_bagInfoList.append(bagInfo);
    }
    DEBUG_LOG("Locally archived bags count: " << m_bagInfoList.count());

    applySearchCriteria(data);
}

/*!
* @fn       getBagData
* @param    boost::shared_ptr<BagData> bag
* @param    const int& rowNumber
* @return   None
* @brief    Fetch the bag data.
*/
bool LocalArchiveBagListHandler::getBagData(boost::shared_ptr<BagData> bag, const int& rowNumber)
{
    if(rowNumber < 0 || rowNumber >= m_bagInfoList.count())
    {
        ERROR_LOG("Index from UI not in list range: " << rowNumber << " " << m_bagInfoList.count());
        return false;
    }
    DEBUG_LOG("RowNumber:"<< rowNumber);
    QString fileNamePattern = m_bagInfoList[rowNumber].m_fileNamePattern;
    return bag->constructFromFiles(fileNamePattern, fileNamePattern.section("/", 0, -2));
}

/*!
* @fn       displaySaveOperationResult
* @param    None
* @return   None
* @brief    display save operation result.
*/
void LocalArchiveBagListHandler::displaySaveOperationResult()
{
    UsbStorageSrv srv;
    QString fileList = "";
    QMap<QString, int>::Iterator it;
    int totalBagCount = m_bagListMap.count();

    for(it = m_bagListMap.begin(); it != m_bagListMap.end(); ++it)
    {
        if(it.value() == Errors::FILE_EXISTS)
        {
            fileList = fileList + it.key() + ",";
        }
    }
    if(!fileList.isEmpty())
    {
        fileList.chop(1);
        UILayoutManager::getUILayoutManagerInstance()->displayMsg(Errors::FILE_EXISTS,
                                                                  fileList,
                                                                  QMLEnums::QML_MESSAGE_MODEL);
    }
    else
    {
        QString msg;
        msg = QString::number(totalBagCount) + tr(" out of ") + QString::number(totalBagCount)
                + tr(" Local Archive bag saved successfully");
        UILayoutManager::getUILayoutManagerInstance()->displayMsg(Errors::FILE_OPERATION_SUCCESS,
                                                                  msg,
                                                                  QMLEnums::QML_MESSAGE_MODEL);
        srv.unMountUsb();
    }
}

/*!
* @fn       saveBag
* @param    QVariant input
* @return   int: Error code
* @brief    Save the selected bag.
*/
int LocalArchiveBagListHandler::saveBag(QVariant input)
{
    TRACE_LOG("");
    QString resultStr;
    QString dest = qvariant_cast<QString>(input);
    QStringList bagfileList = dest.split(":");
    UsbStorageSrv srv;
    bool result = false;

    for(int i = 0; i < bagfileList.size(); i++)
    {
        QString destFilePath = bagfileList.at(i);
        if(destFilePath.length() > 0)
        {
            int hr = Utility::checkFileFolderPermission(destFilePath+BAGFILE_VOLUME_FORMAT, resultStr);
            if(hr != Errors::FILE_OPERATION_SUCCESS)
            {
                if((hr == Errors::FILE_PERMISSION_DENIED)
                        || (hr == Errors::FILE_NOT_PRESENT))
                {
                    srv.unMountUsb();
                    UILayoutManager::getUILayoutManagerInstance()->displayMsg(hr,
                                                                              resultStr,
                                                                              QMLEnums::QML_MESSAGE_MODEL);
                    return hr;
                }
                else if(hr == Errors::FILE_EXISTS)
                {
                    m_bagListMap.insert(destFilePath, hr);
                }
            }
            else if(hr == Errors::FILE_OPERATION_SUCCESS)
            {
                QString data = destFilePath.section("/", -1, -1);
                int index = 0;
                for (; index < m_bagInfoList.count(); index++)
                {
                    QString temp = m_bagInfoList[index].m_fileNamePattern.section("/", -1, -1);
                    if(temp.compare(data) == 0)
                    {
                        QString bagFolderPath = m_bagInfoList[index].m_fileNamePattern.section("/", 0, -2);
                        QDir dir(bagFolderPath);
                        if(!dir.exists())
                        {
                            ERROR_LOG("Folder doesn't exist: " << bagFolderPath.toStdString());
                        }
                        else
                        {
                            QString bagFileNamePattern = m_bagInfoList[index].m_fileNamePattern.section("/", -1, -1);
                            int ret = BagData::copyBagFiles(destFilePath, bagFolderPath , bagFileNamePattern);
                            m_bagListMap.insert(destFilePath, ret);
                        }
                    }
                }
            }
        }
    }
    displaySaveOperationResult();
    return result;
}

/*!
* @fn       overWriteFile
* @param    QVariant input
* @return   bool
* @brief    over wrirte the selected bag.
*/
bool LocalArchiveBagListHandler::overWriteFile(QVariant input)
{
    QString dest = qvariant_cast<QString>(input);
    if (dest.length() > 0)
    {
        QString data = dest.section("/", -1, -1);
        QString destDir = dest.section("/", 0, -2);
        int index = 0;

        for (; index < m_bagInfoList.count(); index++)
        {
            QString temp = m_bagInfoList[index].m_fileNamePattern.section("/", -1, -1);
            if(temp.compare(data) == 0)
            {
                QString bagFolderPath = m_bagInfoList[index].m_fileNamePattern.section("/", 0, -2);
                QString bagFileNamePattern = m_bagInfoList[index].m_fileNamePattern.section("/", -1, -1);
                BagData::copyBagFiles(dest, bagFolderPath , bagFileNamePattern);
                m_bagListMap.insert(dest, Errors::FILE_OPERATION_SUCCESS);
                break;
            }
        }
        bool fileOperationPending = false;
        QMap<QString, int>::Iterator it;
        for(it = m_bagListMap.begin(); it != m_bagListMap.end(); ++it)
        {
            if(it.value() != Errors::FILE_OPERATION_SUCCESS)
            {
                fileOperationPending = true;
                break;
            }
        }
        if(!fileOperationPending)
        {
            UsbStorageSrv srv;
            srv.unMountUsb();
        }
        return true;
    }
    else
    {
        return false;
    }
}
}  // end of namespace ws
}  // end of namespace analogic
