/*!
* @file     archive.cpp
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDir>
#include <QDateTime>
#include <QQuickView>
#include <QQuickItem>

#include <Logger.h>
#include <analogic/ws/common/archive/archive.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/osr/osrmanager.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       Archive
* @param    WorkstationManager* wsm
* @return   None
* @brief    Constructor for class Archive.
*/
Archive::Archive(WorkstationManager* wsm) :
    m_bagInfoListSortOrder(SortOrder::NONE)
{
    m_workstationManagerRef = wsm;
    //m_bagListHandler.reset(new BagListHandler());
   // THROW_IF_FAILED((m_bagListHandler.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
}

/*!
* @fn       ~Archive
* @param    None
* @return   None
* @brief    Destructor for class Archive.
*/
Archive::~Archive()
{
    m_workstationManagerRef = NULL;
    DEBUG_LOG("Deleting m_bagListHandler baglist handler instance");
    //m_bagListHandler.reset();
}

/*!
* @fn       archiveCommand
* @param    QMLEnums::BagsListAndBagDataCommands command
* @param    QStringList input number 1
* @param    BagData* bagData
* @param    QVariantMap& results
* @return   None
* @brief    Command to process.
*/
void Archive::archiveCommand(QMLEnums::BagsListAndBagDataCommands command, QStringList input1, BagData* bagData,
                             QVariantMap& results)
{
    bool status = true;
    if(command < 0 || command >= QMLEnums::BagsListAndBagDataCommands::BLBDC_MAX)
    {
        ERROR_LOG("Incorrect command: " << command);
        status = false;
    }

    if(status)
    {
        LOG(INFO) << "Archive::archiveCommand: " << command;
        if(command == QMLEnums::BagsListAndBagDataCommands::BLBDC_ARCHIVE_BAG_TO_LOCAL_ARCHIVE)
            archiveToConfiguredLocation(bagData);

        else if(command == QMLEnums::BagsListAndBagDataCommands::BLBDC_SHOW_BAG)
            showBag(input1);

        else if(command == QMLEnums::BagsListAndBagDataCommands::BLBDC_DELETE_BAG)
            deleteBag(input1);

        else if(command == QMLEnums::BagsListAndBagDataCommands::BLBDC_SORT_BAGS_LIST)
            sortList(input1);

        else if(command == QMLEnums::BagsListAndBagDataCommands::BLBDC_ARCHIVE_BAG_TO_USB)
            archiveToUSB();

        bool refresh = (command == QMLEnums::BagsListAndBagDataCommands::BLBDC_SHOW_BAGS_LIST);
        showList(input1, refresh, results);
    }
}

/*!
* @fn       archiveToConfiguredLocation
* @param    BagData* bagData
* @return   None
* @brief    Archive bag data to configured path.
*/
void Archive::archiveToConfiguredLocation(BagData* bagData)
{
    QString folderPath = WorkstationConfig::getExportBagDataPath();
    int maxCount = WorkstationConfig::getExportBagDataMaxCount();

    if(!QDir(folderPath.toStdString().c_str()).exists())
    {
        ERROR_LOG("Local archive path does not exist: " << folderPath.toStdString());
        return;
    }

    QDateTime oldestCreationTime;
    int oldestSlot = 0;
    int slot = 0;
    for(; slot < maxCount; slot++)
    {
        std::string bagDataFolderPath = folderPath.toStdString();
        bagDataFolderPath += "/Bag_";

        char num[6];
        std::snprintf(num, sizeof(num), "%03d", slot);
        bagDataFolderPath += num;

        if(!QDir(bagDataFolderPath.c_str()).exists())
            break;

        QFileInfo fInfo(bagDataFolderPath.c_str());
        QDateTime creationTime = fInfo.created();

        if(slot == 0)
            oldestCreationTime = creationTime;
        else
        {
            if(creationTime < oldestCreationTime)
            {
                oldestCreationTime = creationTime;
                oldestSlot = slot;
            }
        }
    }

    if(slot == maxCount)
    {
        std::string bagDataFolderName = "Bag_";

        char num[6];
        std::snprintf(num, sizeof(num), "%03d", oldestSlot);
        bagDataFolderName += num;

        LOG(INFO) << "Deleting bag data folder " << bagDataFolderName << " from " << folderPath.toStdString();

        QDir parentFolder(folderPath.toStdString().c_str());
        parentFolder.rmpath(bagDataFolderName.c_str());

        slot = oldestSlot;
    }

    std::string bagDataFolderPathStr = folderPath.toStdString();
    bagDataFolderPathStr += "/Bag_";

    char num[6];
    std::snprintf(num, sizeof(num), "%03d", slot);
    bagDataFolderPathStr += num;

    QDir bagDataFolderPath(bagDataFolderPathStr.c_str());
    bool created = bagDataFolderPath.mkpath(".");
    if(!created)
    {
        ERROR_LOG("Failed local archive folder creation: "
                  << bagDataFolderPath.absoluteFilePath(".").toStdString());
        return;
    }

    QString userName;
    OsrManager* osrMgr = dynamic_cast<OsrManager*>(m_workstationManagerRef);
    if(osrMgr)
        userName = osrMgr->getUsername();
    bool status = bagData->saveUncroppedToFiles(bagDataFolderPathStr, "bagdata", userName.toStdString());
    if(!status)
        bagDataFolderPath.removeRecursively();
}

/*!
* @fn       applySearchCriteria
* @param    const QStringList& searchCriteria
* @return   None
* @brief    Apply the search criteria.
*/
void Archive::applySearchCriteria(const QStringList& searchCriteria)
{
    DEBUG_LOG("SearchCriteria list count:"<< searchCriteria.count());
    if(searchCriteria.count() != 16)
    {
        ERROR_LOG("Archive search string from QML is invalid: " << searchCriteria.count());
        return;
    }

    QList<BagInfo> newList;

    // If bagid is not empty, use only bagid for filtering.
    bool useBagId = false;
    QString bagId;

    if(searchCriteria[14] != "bagid")
    {
        ERROR_LOG("bagid search field missing.");
    }
    else
    {
        bagId = searchCriteria[15];
        if(bagId.length() > 0)
            useBagId = true;
    }


    // All search criterial other than the bagid.
    bool useDateInSearch = true;
    QDate startDate = QDate::currentDate();
    QDate endDate = startDate;

    bool useMachineId = false;
    QString machineId = tr("All");

    bool userMachineDecision = false;
    QString machineDecision = tr("All");

    bool useflagDecision = false;
    QString userDecision = tr("All");

    bool useUserId = false;
    QString userId = tr("All");



    if(searchCriteria[0] != "enddate" || searchCriteria[2] != "startdate")
    {
        ERROR_LOG("Date search criteria are missing.");
        useDateInSearch = false;
    }

    QDateTime startTime = QDateTime::fromString(searchCriteria[3], "dddd, MMMM d, yyyy HH:mm:ss");
    QDateTime endTime = QDateTime::fromString(searchCriteria[1], "dddd, MMMM d, yyyy HH:mm:ss");
    if(!startTime.isValid() || !endTime.isValid())
    {
        ERROR_LOG("Start/End dates in input are invalid: "
                  << searchCriteria[3].toStdString()
                << " : " << searchCriteria[1].toStdString());
        useDateInSearch = false;
    }
    startDate = startTime.date();
    endDate = endTime.date();


    if(searchCriteria[4] != "machineid")
    {
        ERROR_LOG("machineid search field missing.");
    }
    else
    {   machineId = searchCriteria[5];
        if(machineId != tr("All"))
            useMachineId = true;
    }


    if(searchCriteria[6] != "machinedecision")
    {
        ERROR_LOG("machinedecision search field missing.");
    }
    else
    {   machineDecision = searchCriteria[7];
        if(machineDecision != tr("All"))
            userMachineDecision = true;
    }


    if(searchCriteria[8] != "userid")
    {
        ERROR_LOG("userid search field missing.");
    }
    else
    {   userId = searchCriteria[9];
        if(userId != tr("All"))
            useUserId = true;
    }
    if(searchCriteria[10] != "userdecision")
    {
        ERROR_LOG("userdecision search field missing.");
    }
    else
    {   userDecision = searchCriteria[11];
        if(userDecision != tr("All"))
            useflagDecision = true;
    }

    if(useBagId)
    {
        QRegExp exp;
        exp.setPatternSyntax(QRegExp::WildcardUnix);
        exp.setPattern(bagId);

        for(int i = 0; i < m_bagInfoList.count(); i++)
        {
            int found = exp.indexIn(m_bagInfoList[i].m_bagId);

            if(found >= 0)
                newList.append(m_bagInfoList[i]);
        }
    }
    else
    {
        for(int i = 0; i < m_bagInfoList.count(); i++)
        {
            QDate dt = m_bagInfoList[i].m_timeStamp.date();

            if(useDateInSearch && (dt < startDate || dt > endDate))
                continue;

            if(useMachineId && machineId.compare(m_bagInfoList[i].m_machineId))
                continue;

            if(userMachineDecision)
            {
                if(machineDecision.compare(tr("Clear")) == 0 &&
                        (m_bagInfoList[i].m_decision.compare(tr("Clear")) != 0 ||
                         m_bagInfoList[i].m_decision.compare(tr("Timeout")) != 0))
                    continue;

                if(machineDecision.compare(tr("Alarm")) == 0 &&
                        m_bagInfoList[i].m_decision.compare(tr("Suspect")) != 0)
                    continue;

                if(machineDecision.compare(tr("Fault")) == 0 &&
                        m_bagInfoList[i].m_decision.compare(tr("Fault")) != 0)
                    continue;
            }

            if(useUserId)
                if(userId.compare(m_bagInfoList[i].m_userName))
                    continue;

            if(useflagDecision)
            {
                if(userDecision.compare(tr("Clear")) == 0 &&
                        (m_bagInfoList[i].m_userdecision.compare(tr("Clear")) != 0 ))
                    continue;

                if(userDecision.compare(tr("Suspect")) == 0 &&
                        m_bagInfoList[i].m_userdecision.compare(tr("Suspect")) != 0)
                    continue;

                if(userDecision.compare(tr("Timeout")) == 0 &&
                        m_bagInfoList[i].m_userdecision.compare(tr("Timeout")) != 0)
                    continue;
            }
            newList.append(m_bagInfoList[i]);
        }
    }

    m_bagInfoList = newList;
}

/*!
* @fn       showList
* @param    const QStringList& searchCriteria
* @param    bool refresh
* @param    QVariantMap& results
* @return   None
* @brief    Show the list of local archives on UI.
*/
void Archive::showList(const QStringList& searchCriteria, bool refresh, QVariantMap& results)
{
    if(refresh)
    {
        fetchBagInfoList();
        applySearchCriteria(searchCriteria);
        sort(SortOrder::DEC_TIME);
    }
    prepareBagListForDisplay(results);
}

/*!
* @fn       prepareBagListForDisplay
* @param    QVariantMap& results
* @return   None
* @brief    Put list data into results variable for sending to QML.
*/
void Archive::prepareBagListForDisplay(QVariantMap& results)
{
    results.clear();
    for(int i = 0; i < m_bagInfoList.count(); i++)
    {
        QVariantMap row;
        row.insert("bagId", m_bagInfoList[i].m_bagId);

        QString timeStamp = m_bagInfoList[i].m_timeStamp.toString("yyyy/MM/dd - hh:mm:ss");
        row.insert("dateTime", timeStamp);

        row.insert("machineid", m_bagInfoList[i].m_scannerId);
        row.insert("userDecision", m_bagInfoList[i].m_userdecision);
        row.insert("username", m_bagInfoList[i].m_userName);

        QString rowName = "row";
        rowName += QString::number(i);
        results.insert(rowName, row);
    }
}

/*!
* @fn       showBag
* @param    QStringList input
* @return   None
* @brief    Show bag.
*/
void Archive::showBag(QStringList input)
{
    if(input.count() != 1)
    {
        ERROR_LOG("Argument count to showBag incorrect: " << input.count());
        return;
    }

    int rowNumber = input[0].toInt();

    if(rowNumber < 0 || rowNumber >= m_bagInfoList.count())
    {
        ERROR_LOG("Index from UI not in list range: " << rowNumber << " " << m_bagInfoList.count());
        return;
    }

    boost::shared_ptr<BagData> bag;
    BagsDataPool* bags = BagsDataPool::getInstance();
    bag = bags->getBagbyState(BagData::DISPOSED | BagData::ERROR | BagData::EMPTY);
    if(bag)
    {
        bool success = getBagData(bag, rowNumber);
        if(!success)
            return;
    }
    else
    {
        ERROR_LOG("Failed to get an empty bag from pool to show archived bag.");
        return;
    }
}

/*!
* @fn       archiveToUSB
* @param    None
* @return   None
* @brief    Archive the bag to USB.
*/
void Archive::archiveToUSB()
{
}

/*!
* @fn       sortList
* @param    QStringList& type
* @return   None
* @brief    Archive the bag to USB.
*/
void Archive::sortList(QStringList& type)
{
    if(type.count() != 1)
    {
        ERROR_LOG("Argument count to sortList incorrect: " << type.count());
        return;
    }

    if(type[0].compare("bagid") == 0)
    {
        if(m_bagInfoListSortOrder == SortOrder::ASC_BAGID)
            m_bagInfoListSortOrder = SortOrder::DEC_BAGID;
        else
            m_bagInfoListSortOrder = SortOrder::ASC_BAGID;
    }
    else if(type[0].compare("time") == 0)
    {
        if(m_bagInfoListSortOrder == SortOrder::ASC_TIME)
            m_bagInfoListSortOrder = SortOrder::DEC_TIME;
        else
            m_bagInfoListSortOrder = SortOrder::ASC_TIME;
    }
    else
    {
        ERROR_LOG("Invalid argument to sortList: " << type[0].toStdString());
        return;
    }

    sort(m_bagInfoListSortOrder);
}

/*!
* @fn       sort
* @param    SortOrder sortOrder
* @return   None
* @brief    Sort the bag info list.
*/
void Archive::sort(SortOrder sortOrder)
{
    m_bagInfoListSortOrder = sortOrder;

    if(sortOrder == SortOrder::NONE)
        return;

    int n = m_bagInfoList.count();

    for(int i = 0; i < n-1; i++)
        for (int j = i+1; j < n; j++)
        {
            if(sortOrder == SortOrder::ASC_BAGID)
            {
                if(m_bagInfoList[i].m_bagId <= m_bagInfoList[j].m_bagId)
                    continue;
            }
            else if(sortOrder == SortOrder::DEC_BAGID)
            {
                if(m_bagInfoList[i].m_bagId >= m_bagInfoList[j].m_bagId)
                    continue;
            }
            else if(sortOrder == SortOrder::ASC_TIME)
            {
                if(m_bagInfoList[i].m_timeStamp <= m_bagInfoList[j].m_timeStamp)
                    continue;
            }
            else if(sortOrder == SortOrder::DEC_TIME)
            {
                if(m_bagInfoList[i].m_timeStamp >= m_bagInfoList[j].m_timeStamp)
                    continue;
            }

            BagInfo tmp = m_bagInfoList[i];
            m_bagInfoList[i] = m_bagInfoList[j];
            m_bagInfoList[j] = tmp;
        }
}
}  // end of namespace ws
}  // end of namespace analogic
