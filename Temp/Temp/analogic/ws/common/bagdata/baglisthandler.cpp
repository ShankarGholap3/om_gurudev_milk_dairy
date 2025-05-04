/*!
* @file     baglisthandler.cpp
* @author   Agiliad
* @brief    This file contains class and functions for handling baglist from different sources for local/remote archive
*           and network server.
* @date     Jan, 19 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#include <analogic/ws/common/bagdata/baglisthandler.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       BagListHandler();
* @param    None
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
BagListHandler::BagListHandler():
  m_bagInfoListSortOrder(SortOrder::NONE)
{
}

/*!
* @fn       ~BagListHandler();
* @param    None
* @return   None
* @brief    Destructor responsible for releasing the acquired resources.
*/
BagListHandler::~BagListHandler()
{
  TRACE_LOG("");
  m_bagInfoList.clear();
}

/*!
* @fn       bagListCommand
* @param    QMLEnums::BagsListAndBagDataCommands command
* @param    QVariant input1
* @param    QVariantList& results
* @return   None
* @brief    Command to process.
*/
void BagListHandler::bagListCommand(QMLEnums::BagsListAndBagDataCommands
                                    command, QVariant input1, QVariantList& results, QMLEnums::ArchiveFilterSrchLoc loc)
{
  TRACE_LOG("");
  bool status = true;
  if(command <= QMLEnums::BagsListAndBagDataCommands::BLBDC_START ||
     command >= QMLEnums::BagsListAndBagDataCommands::BLBDC_END)
  {
    ERROR_LOG("Incorrect command: " << command);
    status = false;
  }

  if(status)
  {
    INFO_LOG("Bag list command: " << QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(command));
    if(command == QMLEnums::BagsListAndBagDataCommands::
       BLBDC_ARCHIVE_BAG_TO_LOCAL_ARCHIVE) {
      BagData* bagData;
      if (input1.canConvert<BagData*>())
      {
        bagData = input1.value<BagData*>();
      }
      else
      {
        ERROR_LOG("Recieved BagData for BLBDC_ARCHIVE_BAG_TO_LOCAL_ARCHIVE command is incorrect.");
        return;
      }
      archiveToConfiguredLocation(bagData);
    }

    else if(command == QMLEnums::BagsListAndBagDataCommands::
            BLBDC_SHOW_BAG)
    {
      showBag(input1);
    }

    else if(command == QMLEnums::BagsListAndBagDataCommands::
            BLBDC_DELETE_BAG)
    {
      deleteBag(input1);
    }

    else if(command == QMLEnums::BagsListAndBagDataCommands::
            BLBDC_SORT_BAGS_LIST)
    {
      sortList(input1);
    }

    else if(command == QMLEnums::BagsListAndBagDataCommands::
            BLBDC_ARCHIVE_BAG_TO_USB)
    {
      m_bagListMap.clear();
      QString destPath = qvariant_cast<QString>(input1);
      saveBag(destPath);
    }

    else if(command == QMLEnums::BagsListAndBagDataCommands::
            BLBDC_ARCHIVE_BAG_OVERWRITE)
    {
       m_bagListMap.clear();
      QString destPath = qvariant_cast<QString>(input1);
      overWriteFile(destPath);
    }

    bool refresh = (command == QMLEnums::BagsListAndBagDataCommands::
                    BLBDC_SHOW_BAGS_LIST);
    showList(input1, refresh, results, loc);
  }
}

bool BagListHandler::deleteBag(QVariant input)
{
  ERROR_LOG("Permission denied: Cannot delete the bag: " << input.toString().toStdString());
  return false;
}

/*!
* @fn       archiveToConfiguredLocation
* @param    BagData* bagData
* @return   None
* @brief    Archive bag data to configured path.
*/
void BagListHandler::archiveToConfiguredLocation(BagData* bagData)
{
  // if bagData is already archived
  TRACE_LOG("");

  QString folderPath = WorkstationConfig::getInstance()->getExportBagDataPath();
  int maxCount = WorkstationConfig::getInstance()->getExportBagDataMaxCount();

  QString nameFilter = "*";
  nameFilter += BAGFILE_DICOSCT_FORMAT;
  nameFilter += ".dcs";
  QDir archiveBagDir = QDir(folderPath, nameFilter);

  if(!archiveBagDir.exists())
  {
    ERROR_LOG("Local archive path does not exist: " <<
              folderPath.toStdString());
    return;
  }

  QFileInfoList fileList = archiveBagDir.entryInfoList(QDir::Files, QDir::Time|QDir::Reversed);
  DEBUG_LOG("FileList Count: " << fileList.count());
  DEBUG_LOG("Maximum Archived Bag Count: " << maxCount);

  if (fileList.count() >= maxCount)
  {
    QString getBagForCheck = QString::fromStdString(bagData->getBagFileName());
    QString bagNameCheck = folderPath + "/" + getBagForCheck.append(".dicos_ct_vol.dcs");

    if(fileList.contains(bagNameCheck))
    {
      bool delSuccess = archiveBagDir.remove(getBagForCheck+BAGFILE_PROPERTY_FORMAT);
      if(!delSuccess) {
        ERROR_LOG("Failed to delete: " << (getBagForCheck+BAGFILE_PROPERTY_FORMAT).toStdString());
      }
      delSuccess = archiveBagDir.remove(getBagForCheck+BAGFILE_DICOSCT_FORMAT+".dcs");
      if(!delSuccess) {
        ERROR_LOG("Failed to delete: " << (getBagForCheck+BAGFILE_DICOSCT_FORMAT+".dcs").toStdString());
      }
      delSuccess =archiveBagDir.remove(getBagForCheck+BAGFILE_DICOSCT_PROJ00_FORMAT+".dcs");
      if(!delSuccess) {
        ERROR_LOG("Failed to delete: " << (getBagForCheck+BAGFILE_DICOSCT_PROJ00_FORMAT+".dcs").toStdString());
      }
      delSuccess =archiveBagDir.remove(getBagForCheck+BAGFILE_DICOSCT_PROJ90_FORMAT+".dcs");
      if(!delSuccess) {
        ERROR_LOG("Failed to delete: " << (getBagForCheck+BAGFILE_DICOSCT_PROJ90_FORMAT).toStdString());
      }
      int i = 1;
      bool tdrFilePresent = true;
      do {
        QString tdrFileName;
        tdrFileName = getBagForCheck;
        tdrFileName += BAGFILE_DICOSTDR_FORMAT;
        tdrFileName += QString::number(i);
        tdrFileName += ".dcs";
        tdrFilePresent = archiveBagDir.remove(tdrFileName);
        i++;
      }while (tdrFilePresent);
      fileList.removeOne(bagNameCheck);
    }
    else
    {
      // if archived bags quantity is already maximum,
      // then remove oldest archived bag
      int filesToBeDeletedCount = fileList.count() - maxCount +1;
      for(int j =0; j < filesToBeDeletedCount; j++)
      {
        QString bagFileNamePattern = fileList[j].absoluteFilePath().section("/", -1, -1).section(".", 0, 0);
        bool delSuccess = archiveBagDir.remove(bagFileNamePattern+BAGFILE_PROPERTY_FORMAT);
        if(!delSuccess) {
          ERROR_LOG("Failed to delete: " << (bagFileNamePattern+BAGFILE_PROPERTY_FORMAT).toStdString());
        }
        delSuccess = archiveBagDir.remove(bagFileNamePattern+BAGFILE_DICOSCT_FORMAT + ".dcs");
        if(!delSuccess) {
          ERROR_LOG("Failed to delete: " << (bagFileNamePattern+BAGFILE_DICOSCT_FORMAT + ".dcs").toStdString());
        }
        archiveBagDir.remove(bagFileNamePattern+BAGFILE_DICOSCT_PROJ00_FORMAT + ".dcs");
        if(!delSuccess) {
          ERROR_LOG("Failed to delete: " << (bagFileNamePattern+BAGFILE_DICOSCT_PROJ00_FORMAT + ".dcs").toStdString());
        }
        archiveBagDir.remove(bagFileNamePattern+BAGFILE_DICOSCT_PROJ90_FORMAT + ".dcs");
        if(!delSuccess) {
          ERROR_LOG("Failed to delete: " << (bagFileNamePattern+BAGFILE_DICOSCT_PROJ90_FORMAT + ".dcs").toStdString());
        }
        int i = 1;
        bool tdrFilePresent = true;
        do {
          QString tdrFileName;
          tdrFileName = bagFileNamePattern;
          tdrFileName += BAGFILE_DICOSTDR_FORMAT;
          tdrFileName += QString::number(i);
          tdrFileName += ".dcs";
          tdrFilePresent = archiveBagDir.remove(tdrFileName);
          i++;
        }while (tdrFilePresent);
      }
    }
  }
  QString result = bagData->saveUncroppedToFiles(folderPath.toStdString(), bagData->getBagFileName());

  // Send status to qml
  if (result.isEmpty())
  {
    emit bagArchivedStatus(Errors::S_OK, tr("Bag Archived successfully."), QMLEnums::
                           ModelEnum::QML_MESSAGE_MODEL);
  }
  else
  {
    emit bagArchivedStatus(Errors::E_FAIL, tr("Bag Archived failed. ") + result, QMLEnums::
                           ModelEnum::QML_MESSAGE_MODEL);
  }
}

/*!
* @fn       applySearchCriteria
* @param    const QVariant searchCriteria
* @return   None
* @brief    Apply the search criteria.
*/
void BagListHandler::applySearchCriteria(const QVariant searchCriteria)
{
  TRACE_LOG("");
  QString filterparam = searchCriteria.toString();
  QJsonParseError jsonError;
  QJsonDocument jsondoc = QJsonDocument::fromJson(filterparam.toUtf8(), &jsonError);
  if (jsonError.error == QJsonParseError::NoError)
  {
    QDateTime startTime;
    QDateTime endTime;

    QJsonObject jobj =  jsondoc.object();
    QVariantMap mapobj =  jobj.toVariantMap();

    startTime =  mapobj["startDate"].toDateTime();
    endTime = mapobj["endDate"].toDateTime();


    QList<BagInfo> newList;
    // All search criterial other than the bagid.
    bool useDateInSearch = true;
    QDate startDate = QDate::currentDate();
    QDate endDate = startDate;

    if(startTime.isNull() || endTime.isNull())
    {
      ERROR_LOG("Date search criteria are missing.");
      useDateInSearch = false;
    }

    if(!startTime.isValid() || !endTime.isValid())
    {
      ERROR_LOG("Start/End dates in input are invalid: "
                << startTime.toString().toStdString()
                << " : " << endTime.toString().toStdString());
      useDateInSearch = false;
    }

    startDate = startTime.date();
    endDate = endTime.date();

    for(int i = 0; i < m_bagInfoList.count(); i++)
    {
      QDateTime datetime = QDateTime::fromString(m_bagInfoList[i].m_timeStampStr, "yyyy/MM/dd '-' HH:mm:ss");
      if(useDateInSearch && ((datetime < startTime) || (datetime > endTime)))
      {
        continue;
      }
      newList.append(m_bagInfoList[i]);
    }
    m_bagInfoList = newList;
  }
  else
  {
    ERROR_LOG("Recieved searchCriteria for LocalArchive command is incorrect: "
              <<jsonError.errorString().toStdString().c_str());
    return;
  }
}

/*!
* @fn       showList
* @param    const QVariant data
* @param    bool refresh
* @param    QVariantList& results
* @return   None
* @brief    Show the list of local archives on UI.
*/
void BagListHandler::showList(const QVariant data, bool refresh,
                              QVariantList& results, QMLEnums::ArchiveFilterSrchLoc loc)
{
  if(refresh)
  {
    fetchBagInfoList(data);
    if(loc == QMLEnums::ArchiveFilterSrchLoc::SEARCH_SERVER)
    {
      sort(SortOrder::ASC_TIME);
    }
    else if((loc == QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER) ||
            (loc == QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER))
    {
      sort(SortOrder::DEC_TIME);
    }
    else
    {
      sort(SortOrder::DEC_TIME);
    }
  }
  prepareBagListForDisplay(results);
}

/*!
* @fn       prepareBagListForDisplay
* @param    QVariantList& results
* @return   None
* @brief    Put list data into results variable for sending to QML.
*/
void BagListHandler::prepareBagListForDisplay(QVariantList& results)
{
  results.clear();
  foreach (BagInfo var, m_bagInfoList) {
    results << QVariant::fromValue(var);
  }
}

/*!
* @fn       showBag
* @param    QVariant input
* @return   None
* @brief    Show bag.
*/
void BagListHandler::showBag(QVariant input)
{
  QString data;
  int hr = Errors::E_FAIL;
  if (input.canConvert<QString>())
  {
    data = input.value<QString>();
  }
  else
  {
    ERROR_LOG("Recieved data for LocalArchive command is incorrect.");
    return;
  }

  QString bagFileNamePattern = data;
  int index = 0;
  for (; index < m_bagInfoList.count(); index++) {
    if(m_bagInfoList[index].m_fileNamePattern.compare(
         bagFileNamePattern) == 0)
    {
      break;
    }
  }
  if(index < 0 || index >= m_bagInfoList.count())
  {
    ERROR_LOG("displayedBagId from UI not in list range: " <<
              bagFileNamePattern.toStdString() << " " << m_bagInfoList.count());
    return;
  }

  boost::shared_ptr<BagData> bag;
  BagsDataPool* bags = BagsDataPool::getInstance();
  bag = bags->getBagbyState(BagData::DISPOSED | BagData::ERROR |
                            BagData::EMPTY);
  bool toBeChanged = true;
  QString erroMsg;
  if(bag)
  {
    bool success = getBagData(bag, index);
    hr = Errors::S_OK;
    if(bag && (!success || bag->isBagError()))
    {
      hr = Errors::E_FAIL;
      toBeChanged = false;
      erroMsg = bag->getErrorMsg();
    }
  }
  else
  {
    ERROR_LOG("Failed to get an empty bag from pool to show archived bag.");
    toBeChanged = false;
    erroMsg = tr("Failed to get an empty bag from pool to show archived bag.");
  }
  // Send Message in qml of the screen toBeChanged
  emit notifyToChangeOrNotScreen(toBeChanged, erroMsg, hr);
}


/*!
* @fn       saveBag
* @param    QVariant input
* @return   int: Error code
* @brief    Save the selected bag.
*/
int BagListHandler::saveBag(QVariant input)
{
  Q_UNUSED(input);
  return false;
}

/*!
* @fn       overWriteFile
* @param    QVariant input
* @return   bool
* @brief    over wrirte the selected bag.
*/
bool BagListHandler::overWriteFile(QVariant input)
{
  Q_UNUSED(input)
  return false;
}
/*!
* @fn       sortList
* @param    QVariant type
* @return   None
* @brief    Archive the bag to USB.
*/
void BagListHandler::sortList(QVariant type)
{
  TRACE_LOG("");

  QString data;
  if (type.canConvert<QString>())
  {
    data = type.value<QString>();
  }
  else
  {
    ERROR_LOG("Recieved data for LocalArchive command is incorrect.");
    return;
  }

  if(data.compare("bagid") == 0)
  {
    if(m_bagInfoListSortOrder == SortOrder::ASC_BAGID)
    {
      m_bagInfoListSortOrder = SortOrder::DEC_BAGID;
    }
    else
    {
      m_bagInfoListSortOrder = SortOrder::ASC_BAGID;
    }
  }
  else if(data.compare("time") == 0)
  {
    if(m_bagInfoListSortOrder == SortOrder::ASC_TIME)
    {
      m_bagInfoListSortOrder = SortOrder::DEC_TIME;
    }
    else
    {
      m_bagInfoListSortOrder = SortOrder::ASC_TIME;
    }
  }
  else
  {
    ERROR_LOG("Invalid argument to sortList: " << data.toStdString());
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
void BagListHandler::sort(SortOrder sortOrder)
{
  TRACE_LOG("");

  m_bagInfoListSortOrder = sortOrder;

  if(sortOrder == SortOrder::NONE){
    return;
  }

  int n = m_bagInfoList.count();

  for(int i = 0; i < n-1; i++)
    for (int j = i+1; j < n; j++)
    {
      if(sortOrder == SortOrder::ASC_BAGID)
      {
        if(m_bagInfoList[i].displayable_bag_id <= m_bagInfoList[j].
           displayable_bag_id) {
          continue;
        }
      }
      else if(sortOrder == SortOrder::DEC_BAGID)
      {
        if(m_bagInfoList[i].displayable_bag_id >= m_bagInfoList[j].
           displayable_bag_id) {
          continue;
        }
      }
      else if(sortOrder == SortOrder::ASC_TIME)
      {
        if(m_bagInfoList[i].m_timeStampStr <= m_bagInfoList[j].
           m_timeStampStr) {
          continue;
        }
      }
      else if(sortOrder == SortOrder::DEC_TIME)
      {
        if(m_bagInfoList[i].m_timeStampStr >= m_bagInfoList[j].
           m_timeStampStr) {
          continue;
        }
      }

      BagInfo tmp = m_bagInfoList[i];
      m_bagInfoList[i] = m_bagInfoList[j];
      m_bagInfoList[j] = tmp;
    }
}
}  // end of namespace ws
}  // end of namespace analogic
