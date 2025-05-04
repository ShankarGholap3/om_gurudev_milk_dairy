/*!
* @file     remotearchivebaglisthandler.cpp
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from remote system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/common/archive/remotearchivebaglisthandler.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       RemoteArchiveBagListHandler
* @param    None
* @return   None
* @brief    Constructor for class RemoteArchiveBagListHandler.
*/
RemoteArchiveBagListHandler::RemoteArchiveBagListHandler()
{
}

/*!
* @fn       ~RemoteArchiveBagListHandler
* @param    None
* @return   None
* @brief    Destructor for class RemoteArchiveBagListHandler.
*/
RemoteArchiveBagListHandler::~RemoteArchiveBagListHandler()
{
}

/*!
* @fn       fetchBagInfoList
* @param    QVariant
* @return   None
* @brief    Load bag info list.
*/
void RemoteArchiveBagListHandler::fetchBagInfoList(QVariant data)
{
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
void RemoteArchiveBagListHandler::showBag(QVariant input)
{
  TRACE_LOG("");
  emit getBagDataFromServer(QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER,
                            input.toString().toStdString(),
                            QMLEnums::BagsListAndBagDataCommands::BLBDC_SHOW_BAG);
}


/*!
* @fn       saveBag
* @param    QVariant input
* @return   int: Error code
* @brief    save the selected bag.
*/
int RemoteArchiveBagListHandler::saveBag(QVariant input)
{
  TRACE_LOG("");
  QString destPath = qvariant_cast<QString>(input);
  QStringList bagfileList = destPath.split(":");
  for(int i = 0; i < bagfileList.size(); i++)
  {
    m_bagListMap.insert(bagfileList.at(i), -1);
    QVariant bagId = bagfileList.at(i).section("/", -1, -1);
    bagfileList[i] = bagfileList.at(i).section("/", 0, -2);
    m_usbArchiveTargetPath = bagfileList.at(i);
    emit getBagDataFromServer(QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER,
                              bagId.toString().toStdString(),
                              QMLEnums::BagsListAndBagDataCommands::BLBDC_ARCHIVE_BAG_TO_USB);
  }
  return true;
}

/*!
* @fn       deleteBag
* @param    QVariant input
* @return   None
* @brief    delete the selected bag.
*/
bool RemoteArchiveBagListHandler::deleteBag(QVariant input)
{
  TRACE_LOG("");
  QString dest = qvariant_cast<QString>(input);
  QString bagfileNamePattern = dest.section("/", -1, -1);
  BagData::removeBagFiles(TEMP_BAG_STORAGE, m_bagfilenamepattern);
  return true;
}

/*!
* @fn       overWriteFile
* @param    QVariant input
* @return   bool
* @brief    over wrirte the selected bag.
*/
bool RemoteArchiveBagListHandler::overWriteFile(QVariant input)
{
  TRACE_LOG("");
  int copySuccess;
  QString dest = qvariant_cast<QString>(input);
  m_bagfilenamepattern = dest.section("/", -1, -1);

  copySuccess = BagData::copyBagFiles(dest, TEMP_BAG_STORAGE, m_bagfilenamepattern);
  BagData::removeBagFiles(TEMP_BAG_STORAGE, m_bagfilenamepattern);
  m_bagListMap.insert(dest, copySuccess);


  bool fileOperationPending = false;
  QMap<QString, int>::Iterator it;
  for(it = m_bagListMap.begin(); it != m_bagListMap.end(); ++it)
  {
    if(it.value() != Errors::FILE_OPERATION_SUCCESS)
    {
      fileOperationPending = true;
      displaySaveOperationResult();
      break;
    }
  }
  if(!fileOperationPending)
  {
    UsbStorageSrv srv;
    srv.unMountUsb();
  }
  return copySuccess;
}

/*!
* @fn       onSaveRemoteArchiveBag
* @param    boost::shared_ptr<BagData> bagdata
* @return   None
* @brief    save the selected remote bag.
*/
void RemoteArchiveBagListHandler::onSaveRemoteArchiveBag(boost::shared_ptr<BagData> bagdata)
{
  QMap<QString, int>::Iterator it;
  if(nullptr != bagdata)
  {
    QString result;
    m_bagfilenamepattern = bagdata.get()->getBagFileName().c_str();
    QString filePath = m_usbArchiveTargetPath + "/" +m_bagfilenamepattern + BAGFILE_DICOSCT_FORMAT + ".dcs";

    int hr = Utility::checkFileFolderPermission(filePath, result);
    QString bagName = m_usbArchiveTargetPath + "/" + QString(bagdata.get()->getBagid().c_str());
    m_bagListMap.insert(bagName, hr);

    if(hr != Errors::FILE_OPERATION_SUCCESS)
    {
      if(hr == Errors::FILE_EXISTS)
      {
        QDir dir(TEMP_BAG_STORAGE);
        if(!dir.exists())
        {
          dir.mkdir(TEMP_BAG_STORAGE);
        }
        result = bagdata.get()->saveUncroppedToFiles(TEMP_BAG_STORAGE, m_bagfilenamepattern.toStdString());
        if(result.isEmpty())
        {
          m_bagListMap.remove(bagName);
          m_bagListMap.insert(QString(m_usbArchiveTargetPath + "/" +m_bagfilenamepattern) , Errors::FILE_EXISTS);
        }else if(result == "File copy failed as Drive is full")
        {
          hr = Errors::USB_DRIVE_FULL;
          m_bagListMap.insert(bagName, hr);
        }
        else
        {
          hr = Errors::FILE_OPERATION_FAILED;
          m_bagListMap.insert(bagName, hr);
        }

      }
    }

    if(hr  == Errors::FILE_OPERATION_SUCCESS)
    {
      result = "";
      result = bagdata.get()->saveUncroppedToFiles(m_usbArchiveTargetPath.toStdString(),
                                          m_bagfilenamepattern.toStdString());
      INFO_LOG("Call for remote save button");
      if(result.isEmpty())
      {
        m_bagListMap.insert(bagName, hr);
      }else if(result == "File copy failed as Drive is full")
      {
        hr = Errors::USB_DRIVE_FULL;
        m_bagListMap.insert(bagName, hr);
      }
      else
      {
        hr = Errors::FILE_OPERATION_FAILED;
        m_bagListMap.insert(bagName, hr);
      }
    }
    bagdata.get()->setState(BagData::DISPOSED);
  }
  else
  {
    ERROR_LOG("bagdata is NULL");
  }
  bool saveOperationCompleted = true;
  for(it = m_bagListMap.begin(); it != m_bagListMap.end(); ++it)
  {
    if(it.value() == -1)
    {
      saveOperationCompleted = false;
      break;
    }
  }
  if(saveOperationCompleted)
  {
    displaySaveOperationResult();
  }
}

/*!
* @fn       getBagData
* @param    boost::shared_ptr<BagData> bag
* @param    const int& rowNumber
* @return   None
* @brief    Fetch the bag data.
*/
bool RemoteArchiveBagListHandler::getBagData(boost::shared_ptr<BagData> bag, const int& rowNumber)
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
void RemoteArchiveBagListHandler::displaySaveOperationResult()
{
  UsbStorageSrv srv;
  QMap<QString, int>::Iterator it;
  QString existingFileList = "";
  int totalBagCount = m_bagListMap.count();
  int totalFailedBagCount = 0;
  for(it = m_bagListMap.begin(); it != m_bagListMap.end(); ++it)
  {
    if(it.value() == Errors::FILE_EXISTS)
    {
      existingFileList = existingFileList + it.key() + ",";
    }
    else if(it.value() == Errors::FILE_PERMISSION_DENIED)
    {
      QString msg = tr("User is not having write permission to this directory.");
      UILayoutManager::getUILayoutManagerInstance()->displayMsg(it.value(), msg, QMLEnums::QML_MESSAGE_MODEL);
      UsbStorageSrv srv;
      srv.unMountUsb();
      return;
    }
    else if(it.value() == Errors::FILE_NOT_PRESENT)
    {
      QString msg = tr("Directory Does not Exists.");
      UILayoutManager::getUILayoutManagerInstance()->displayMsg(it.value(), msg, QMLEnums::QML_MESSAGE_MODEL);
      UsbStorageSrv srv;
      srv.unMountUsb();
      return;
    }
    else if(it.value() == Errors::USB_DRIVE_FULL)
    {
      QString msg = tr("File copy failed as Drive is full.");
      UILayoutManager::getUILayoutManagerInstance()->displayMsg(it.value(), msg, QMLEnums::QML_MESSAGE_MODEL);
      UsbStorageSrv srv;
      srv.unMountUsb();
      return;
    }
    else if(it.value() == Errors::FILE_OPERATION_FAILED)
    {
      QString msg = tr("File copy operation failed.");
      UILayoutManager::getUILayoutManagerInstance()->displayMsg(it.value(), msg, QMLEnums::QML_MESSAGE_MODEL);
      UsbStorageSrv srv;
      srv.unMountUsb();
      return;
    }
    else if(it.value() == Errors::FILE_NOT_FOUND)
    {
      totalFailedBagCount++;
    }
  }
  if(!existingFileList.isEmpty())
  {
    existingFileList.chop(1);
    UILayoutManager::getUILayoutManagerInstance()->displayMsg(Errors::FILE_EXISTS, existingFileList,
                                                              QMLEnums::QML_MESSAGE_MODEL);
  }
  else
  {
    srv.unMountUsb();
    QString msg = QString::number(totalBagCount - totalFailedBagCount) + tr(" out of ") + QString::number(totalBagCount)
        + tr(" Remote Archive bag saved successfully");;
    UILayoutManager::getUILayoutManagerInstance()->displayMsg(Errors::FILE_OPERATION_SUCCESS, msg,
                                                              QMLEnums::QML_MESSAGE_MODEL);
  }
}

}  // end of namespace ws
}  // end of namespace analogic
