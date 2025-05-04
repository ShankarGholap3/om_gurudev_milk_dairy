/*!
* @file     usbfilesystemmodel.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to usb fileSystemModel
*           which displays usb file system.
* @date     Apr 18, 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QUrl>
#include <QTextStream>

#include <analogic/ws/rerun/usbfilesystemmodel.h>
#include <analogic/ws/common/utility/usbstorageservice.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       UsbFileSystemModel
* @param    QObject *parent
* @return   None
* @brief    Constructor for class UsbFileSystemModel.
*/
UsbFileSystemModel::UsbFileSystemModel(QObject *parent)
  : QFileSystemModel(parent)
{
  m_bagCount = 0;
  m_alarmCount = 0;
  m_alarmRate = 0;
  m_description = "";
}

/*!
* @fn       UsbFileSystemModel
* @param    None
* @return   None
* @brief    Destructor for class UsbFileSystemModel.
*/
UsbFileSystemModel::~UsbFileSystemModel()
{

}

/*!
* @fn       initialize
* @param    None
* @return   None
* @brief    intialization of usb fs.
*/
void UsbFileSystemModel::initialize()
{
  if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::RERUN_EMULATOR)
    setNameFilters(QStringList("rerun.txt"));
  else if(WorkstationConfig::getInstance()->getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)
    setNameFilters(QStringList("*"));
  setNameFilterDisables(false);
  m_rerunConfigObj= new RerunConfig();
  THROW_IF_FAILED((m_rerunConfigObj== nullptr)? Errors::E_OUTOFMEMORY : Errors::S_OK);

}

/*!
* @fn       data
* @param    const QModelIndex &index
* @param    int role = Qt::DisplayRole
* @return   QVariant
* @brief    Gives data for current role and index.
*/
QVariant UsbFileSystemModel::data(const QModelIndex &index, int role) const
{
  bool success = customFileInfo(fileInfo(index));
  if (index.isValid() && role >= BagCountRole && success) {
    switch (role) {
    case BagCountRole:
      return QVariant(QString::number(m_bagCount));
    case AlarmCountRole:
      return QVariant(QString::number(m_alarmCount));
    case AlarmRateRole:
      return QVariant(QString::number(m_alarmRate));
    case DescriptionRole:
      return QVariant(m_description);
    default:
      break;
    }
  }
  return QFileSystemModel::data(index, role);
}

/*!
* @fn       customFileInfo
* @param    const QFileInfo &fi
* @return   bool
* @brief    Gives customFileInfo of current file.
*/
bool UsbFileSystemModel::customFileInfo(const QFileInfo &fi) const
{
  if (!fi.isFile()) {
    return false;
  }
  const_cast<UsbFileSystemModel*>(this)->m_bagCount = 0;
  const_cast<UsbFileSystemModel*>(this)->m_alarmCount = 0;
  const_cast<UsbFileSystemModel*>(this)->m_alarmRate = 0;
  const_cast<UsbFileSystemModel*>(this)->m_description = "";
  QFile file(fi.absoluteFilePath());
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    // First row is discarded as it contains column header names
    QString line = in.readLine();
    if(line.startsWith("#"))
    {
      const_cast<UsbFileSystemModel*>(this)->m_description = line.remove(0,1);
    }
    while (!in.atEnd())
    {
      line = in.readLine();
      if(line.isEmpty())
        continue;
      ++const_cast<UsbFileSystemModel*>(this)->m_bagCount;
      QString macDecInRerun = line.section(",",1,1);
      if ((macDecInRerun == "A") ||
          (macDecInRerun == "T") ||
          (macDecInRerun == "U"))
      {
        ++const_cast<UsbFileSystemModel*>(this)->m_alarmCount;
      }
    }
    float temp = ((float)(m_alarmCount) / (float)m_bagCount) * 100;
    const_cast<UsbFileSystemModel*>(this)->m_alarmRate = (uint)temp;

    file.close();
  }
  return true;
}

/*!
* @fn       roleNames
* @param    None
* @return   QHash<int,QByteArray>
* @brief    gives roleNames.
*/
QHash<int,QByteArray> UsbFileSystemModel::roleNames() const
{
  QHash<int, QByteArray> result = QFileSystemModel::roleNames();
  result.insert(BagCountRole, QByteArrayLiteral("bagCount"));
  result.insert(AlarmCountRole, QByteArrayLiteral("alarmCount"));
  result.insert(AlarmRateRole, QByteArrayLiteral("alarmRate"));
  result.insert(DescriptionRole, QByteArrayLiteral("description"));
  return result;
}

/*!
* @fn       bagIDList
* @param    const QModelIndex &index
* @return   QVariant
* @brief    Gives bagList for current index.
*/
QVariant UsbFileSystemModel::bagIDList(const QString playlistname)
{
  QString playlistDirPath = RerunConfig::getPlaylistPath();
  QVariantList results;
  QDir dir(playlistDirPath);
  if(!dir.exists())
  {
    playlistDirPath = WorkstationConfig::getInstance()->getDefaultDataPath() + "/Playlist";
  }
  playlistDirPath = playlistDirPath +"/" +playlistname;
  QFile file(playlistDirPath);
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    results << QVariant::fromValue(1);
    // First row is discarded as it contains column header names
    QString line = in.readLine();
    while (!in.atEnd())
    {
      line = in.readLine();
      if(line.isEmpty())
        continue;
      BagFileModel bagStruct;
      bagStruct.m_bagDataFilePath = line.section(",", 0, 0);
      bagStruct.m_bagId = bagStruct.m_bagDataFilePath.section("/", -1, -1).section("_", -1, -1);
      results << QVariant::fromValue<BagFileModel>(bagStruct);
    }
    file.close();
  }
  return results;
}

QVariant UsbFileSystemModel::readFromDecisionCSV(const QString filePathName)
{
  INFO_LOG("readFromDecisionCSV");
  QVariantList results;
  QFile file(filePathName);
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
  //  results << QVariant::fromValue(1);
    // First row is discarded as it contains column header names
    // line = in.readLine();
    while (!in.atEnd())
    {
     QString line = in.readLine();
      INFO_LOG("LINE "<<line.toStdString());
      if(line.isEmpty())
        continue;
      PlaylistDetails bagStruct;
      bagStruct.m_bagId = line.section(",", 0, 0);
      bagStruct.m_threatCategory = line.section(",", 1, 1);
      bagStruct.m_machineCorrectDecison =line.section(",", 2, 2);
      results << QVariant::fromValue<PlaylistDetails>(bagStruct);
    }
    file.close();
  }
  return results;

}
/*!
* @fn       bagSelectionList
* @param    const QModelIndex &index
* @return   QVariant
* @brief    Gives bagList for current index.
*/
QVariant UsbFileSystemModel::bagSelectionList(const QModelIndex &index)
{
  QFileInfo fi = fileInfo(index);
  QVariantList results;
  if (!fi.isFile()) {
    results << QVariant::fromValue(0);
    return results;
  }
  QFile file(fi.absoluteFilePath());
  m_playlistname = fi.baseName();
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    results << QVariant::fromValue(1);
    // First row is discarded as it contains column header names
    QString line = in.readLine();
    while (!in.atEnd())
    {
      line = in.readLine();
      if(line.isEmpty())
        continue;
      BagFileModel bagStruct;
      bagStruct.m_bagDataFilePath = line.section(",", 0, 0);
      bagStruct.m_bagId = bagStruct.m_bagDataFilePath.section("/", -1, -1).section("_", -1, -1);
      QString macDecInRerun = line.section(",",1,1);
      if (macDecInRerun == "A") {
        bagStruct.m_machineDecison = tr("Alarm");
      }
      else if (macDecInRerun == "C") {
        bagStruct.m_machineDecison = tr("Clear");
      }
      else if (macDecInRerun == "T") {
        bagStruct.m_machineDecison = tr("Timeout");
      }
      if (macDecInRerun == "U") {
        bagStruct.m_machineDecison = tr("Unknown");
      }
      results << QVariant::fromValue<BagFileModel>(bagStruct);
    }
    file.close();
  }
  return results;
}

/*!
* @fn       setRootDirPath
* @param    None
* @return   QModelIndex
* @brief    sets rootPath and Gives modelIndex of current rootPath.
*/
QModelIndex UsbFileSystemModel::setRootDirPath()
{
  std::string root_file_path = UsbStorageSrv::getUsbfilePath();
  if (root_file_path.empty() || !(RerunConfig::isRemovableStorage()))
    root_file_path = WorkstationConfig::getInstance()->getDefaultDataPath().toStdString();
  return setRootPath(QString::fromStdString(root_file_path));
}

/*!
* @fn       setPlaylistFilePath
* @param    None
* @return   QModelIndex
* @brief    sets playlist file path from configuration file.
*/
QModelIndex UsbFileSystemModel::setPlaylistFilePath()
{
  QString playlistDirPath = RerunConfig::getPlaylistPath();
  QDir dir(playlistDirPath);
  if(!dir.exists())
  {
    playlistDirPath = WorkstationConfig::getInstance()->getDefaultDataPath() + "/Playlist";
  }
  return setRootPath(playlistDirPath);
}

/*!
* @fn       readFromFile
* @param    none
* @return   QMap<QString,QString>
* @brief    retrun the playlist categorise file details to comapre an create OQT Reports
*/
QMap<QString,QString> UsbFileSystemModel::getPlaylistDetails()
{
  return m_playlistCategorizeFileDetails;
}

/*!
* @fn       readFromFile
* @param    QString filePathName
* @return   bool
* @brief    return the contents of a given file line-by-line
*/
bool UsbFileSystemModel::readFromCsvFile(QString filePathName)
{

  QFile file(filePathName);
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      QString bagID=line.split(",").first();
      m_playlistCategorizeFileDetails.insert(bagID,line);
      //m_decisionlist.append(line);
    }
    file.close();
  }
  else
  {
    ERROR_LOG(filePathName.toStdString() + " failed to open.");
    return false;
  }
  return true;
}

/*!
* @fn       trainingDetailedReportdataList
* @param    None
* @return   QVariantList
* @brief    populates the OQT Detailed Report table.
*/
QVariant UsbFileSystemModel :: trainingDetailedReportdataList(QString selectedUserID, QString selectedUserReport)
{
  INFO_LOG("UsbFileSystemModel :: trainingDetailedReportdataList");
  QString dir= m_rerunConfigObj->getReportResultPath(selectedUserID)+"/"+selectedUserReport;

  QFile file(dir);

  QVariantList results;

  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    results << QVariant::fromValue(1);
    // First row is discarded as it contains column header names
    QString line = in.readLine();
    while (!in.atEnd())
    {
      line = in.readLine();
      if(line.isEmpty())
        continue;
      QStringList data = line.split(",");
      ReportFileModel reportStruct;
      reportStruct.m_useName = data[0];
      reportStruct.m_userId= data[1];
      reportStruct.m_siteCode = data[2];
      reportStruct.m_date = data[3];
      reportStruct.m_time = data[4];
      reportStruct.m_testName = data[5];
      reportStruct.m_BagID = data[6];
      reportStruct.m_Category = data[7];
      reportStruct.m_elapsedTime = data[8];
      reportStruct.m_userDecision = data[9];
      reportStruct.m_expected = data[10];
      reportStruct.m_Result = data[11];
      results << QVariant::fromValue<ReportFileModel>(reportStruct);
    }
    file.close();
  }
  return results;
}

/*!
* @fn       trainingSummaryReportdataList
* @param    None
* @return   QVariantList
* @brief    populates the OQT Summary Report table.
*/
QVariant UsbFileSystemModel :: trainingSummaryReportdataList(QString selectedUserID, QString selectedUserReport)
{
  INFO_LOG("UsbFileSystemModel :: trainingSummaryReportdataList");
  QString dir= m_rerunConfigObj->getReportResultPath(selectedUserID)+"/"+selectedUserReport;
  QFile file(dir);
  QVariantList results;

  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    results << QVariant::fromValue(1);
    // First row is discarded as it contains column header names
    QString line = in.readLine();
    while (!in.atEnd())
    {
      line = in.readLine();
      if(line.isEmpty())
        continue;
      QStringList data = line.split(",");
      ReportFileModel reportStruct;
      reportStruct.m_useName = data[0];
      reportStruct.m_userId= data[1];
      reportStruct.m_siteCode = data[2];
      reportStruct.m_date = data[3];
      reportStruct.m_time = data[4];
      reportStruct.m_testName = data[5];
      reportStruct.m_elapsedTime = data[6];
      reportStruct.m_pd = data[7];
      reportStruct.m_pfa = data[8];
      reportStruct.m_score = data[9];
      results << QVariant::fromValue<ReportFileModel>(reportStruct);
    }
    file.close();
  }
  return results;
}


/*!
* @fn       getAllChildrenIndexes
* @param    None
* @return   QVariant
* @brief    Gives modelIndexes of all child folders & files.
*/
QVariant UsbFileSystemModel::getAllChildrenIndexes()
{
  QVariantList indexList;

  QString rootPathStr = rootPath();
  if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::RERUN_EMULATOR)
  {
    getChildrenIndexes(rootPathStr, indexList);
  } else if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION) {
    getPlaylistChildrenIndexes(rootPathStr, indexList);
  }
  return indexList;
}

/*!
* @fn       getRootIndex
* @param    None
* @return   QModelIndex
* @brief    return playlist file path from configuration file.
*/
QModelIndex UsbFileSystemModel::getRootIndex()
{
  QString playlistDirPath;
  if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::RERUN_EMULATOR)
  {
    playlistDirPath = QString::fromStdString(UsbStorageSrv::getUsbfilePath());
    if (playlistDirPath.isEmpty() || !(RerunConfig::isRemovableStorage()))
      playlistDirPath = WorkstationConfig::getInstance()->getDefaultDataPath();
  }
  else if(WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION)
  {
    playlistDirPath = RerunConfig::getPlaylistPath();
    QDir dir(playlistDirPath);
    if(!dir.exists())
    {
      playlistDirPath = WorkstationConfig::getInstance()->getDefaultDataPath() + "/Playlist";
    }
  }
  return index(playlistDirPath);
}

/*!
* @fn       getPlaylistChildrenIndexes
* @param    QString
* @param    QVariantList& - indexlist
* @return   None
* @brief    Gives modelIndexes of child Playlist files.
*/
void UsbFileSystemModel::getPlaylistChildrenIndexes(QString parentPath, QVariantList& indexList)
{
  QDir dir(parentPath);

  QStringList list = dir.entryList(QDir::Files);
  foreach(QString childPath, list)
  {
    childPath.prepend(parentPath+"/");
    QModelIndex childIndex = index(childPath);
    indexList << QVariant::fromValue(childIndex);
  }
}

/*!
* @fn       getChildrenIndexes
* @param    QString
* @param    QVariantList& - indexlist
* @return   None
* @brief    Gives modelIndexes of child folders & files.
*/
void UsbFileSystemModel::getChildrenIndexes(QString parentPath, QVariantList& indexList)
{
  QDir dir(parentPath);

  QStringList list = dir.entryList(QStringList("rerun.txt"), QDir::NoDot | QDir::NoDotDot | QDir::AllDirs | QDir::Files);
  foreach(QString childPath, list)
  {
    childPath.prepend(parentPath+"/");
    QModelIndex childIndex = index(childPath);
    indexList << QVariant::fromValue(childIndex);
    if (isDir(childIndex))
    {
      getChildrenIndexes(childPath,indexList);
    }
  }
}

/*!
* @fn       getPlayListName
* @param    None
* @return   QString
* @brief    sets playlist file name from configuration file.
*/
QString UsbFileSystemModel::getPlayListName()
{
  return m_playlistname;
}


}  // namespace ws
}  // namespace analogic
