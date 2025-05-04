/*!
* @file     rerunbatchmanager.cpp
* @author   Agiliad
* @brief    This file contains interface to related to managing batch file creations.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include <analogic/ws/rerun/rerunbatchmanager.h>

namespace analogic
{
namespace ws
{
/*!
   * @fn       RerunManager
   * @param    None
   * @return   None
   * @brief    Constructor for class RerunBatchManager.
   */
RerunBatchManager::RerunBatchManager(QObject *parent):QObject(parent)
{
  m_taskList.clear();
}

/*!
   * @fn       RerunManager
   * @param    None
   * @return   None
   * @brief    Destructor for class RerunManager.
   */
RerunBatchManager::~RerunBatchManager()
{}

/*!
* @fn       getBatchFileFromInputFolder
* @param    QString folderPathName
* @param    QString nameFilter
* @return   QStringList
* @brief    returns list of files recursively for given name filter
*/
QString RerunBatchManager::getBatchFileFromInputFolder(QString folderPathName , QString batchFileName)
{
  QDir dir(folderPathName);
  qDebug()<<"folderPathName"<<folderPathName;
  dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
  dir.setSorting(QDir::NoSort);
  QFileInfoList folderList = dir.entryInfoList();
  foreach (QFileInfo folders, folderList)
  {
    qDebug()<<"folderlist is : "<< folders.absoluteFilePath();
  }
  QString tempBagListFile = getBagListFromFolder(folderPathName);
  qDebug()<<tempBagListFile<< "::list of file at line 48";
  QFileInfo file(tempBagListFile);
  if(file.exists())
  {
    qDebug()<< "Temp Bag List File Is:" << tempBagListFile;
    batchOutputList(tempBagListFile,batchFileName);
    Utility::appendToFile(batchFileName, tempBagListFile);
    Utility::appendToFile(batchFileName, "\n");
    RerunTaskConfig tempConfig;
    tempConfig.setBagInputFolder(folderPathName);
    tempConfig.setSelectedBagListFile(tempBagListFile);
    tempConfig.setEditBagListFile(tempBagListFile);
    qDebug()<<"Task list";
    qDebug()<<tempConfig.bagInputFolder();
    qDebug()<<tempConfig.selectedBagListFile();
    qDebug()<<tempConfig.editBagListFile();
    tempConfig.setIsRun(false);
    m_taskList.append(tempConfig);
  }

  foreach (QFileInfo file, folderList)
  {
    qDebug()<< "folderList obtained in batch file from input folder : " << file.absoluteFilePath();
    if(file.isDir()) // this will check each file/folder that is stored in folderList
    {
      QString batchFileRecursive = getBatchFileFromInputFolder(file.absoluteFilePath(), batchFileName);
      if(batchFileRecursive.length() > 0)
      {
        qDebug()<<batchFileRecursive<<": batchFileRecursive";
        //appendToFile(batchFileName,batchFileRecursive);
        //appendToFile(batchFileName,"\n");
      }
    }
    else
    {
      QString filePath = file.absoluteFilePath();
      QString tempBagListFile = getBagListFromFolder(filePath);
      if(tempBagListFile.length() > 0)
      {
        //  Utility::appendToFile(batchFileName, tempBagListFile);
        //  Utility::appendToFile(batchFileName, "\n");
      }
    }
  }

  return batchFileName;
}

/*!
* @fn       getBagListFromInputFolder
* @param    QString folderPath
* @return   QStringList
* @brief    returns list of .vol bag files recursively
*/
QString RerunBatchManager::getBagListFromFolder(QString folderPath)
{
  QDir dir(folderPath);
  qDebug()<<"FolderPath"<< folderPath;
  QString bagListFile = "";
  QString volFileFilters =  "*.vol,*.jpeg_ls_vol";
  QStringList volFormatList = volFileFilters.split(",");
  qDebug()<<"volformatList"<<volFormatList;
  dir.setNameFilters(volFormatList);
  QFileInfoList bagList = dir.entryInfoList(dir.nameFilters(),
                                            QDir::NoDot | QDir::NoDotDot | QDir::AllDirs | QDir::Files);

  foreach(QFileInfo volFile, bagList)
  {
    qDebug()<<"bag list"<<volFile.absoluteFilePath();
  }
  //this I am using for all vol file to get stored in the bagList variable*/
  if(!bagList.isEmpty())
  {
    QString dt = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString bagTextFile = "list-"+ dir.dirName()+"_"+dt+".txt";
    qDebug() << "name of text files : " << bagTextFile;
    bagListFile = folderPath + "/" + bagTextFile;
    foreach(QFileInfo bagFile, bagList)
    {
      if(bagFile.isFile())
      {
        qDebug()<<"absolutePath : "<<bagFile.absoluteFilePath();
        QString bagFileName = bagFile.absoluteFilePath();
        Utility::appendToFile(bagListFile,bagFileName);
        Utility::appendToFile(bagListFile,"\n");
        //this will take the filename of the .vol file
      }
    }

  }
  else {
    bagListFile = "";

  }
  return bagListFile;
}


/*!
* @fn       createBatchOutputFolder
* @param    QString folderPath
* @return   QStringList
* @brief    returns list of output folder for batch file
*/

void RerunBatchManager::createBatchOutputFolder(QString outputFolderPath, QString batchList )
{
  QString tempOutputFolderPath = outputFolderPath;
  QFileInfo batchFile(batchList);
  QString baseDirPath = batchFile.absolutePath();
  QString baseDirName = QDir(baseDirPath).dirName();

  for(int i=0;i<m_taskList.size();i++)
  {
    RerunTaskConfig temp = m_taskList.at(i);
    QString outputPath = outputFolderPath;
    QString tempInput = temp.bagInputFolder();
    QDir tempInputQdir(tempInput);
    QDir baseDirPathDir(baseDirPath);
    if (tempInputQdir.absolutePath().compare(baseDirPathDir.absolutePath()) == 0)
    {
      outputPath = outputPath + "/"+ baseDirName;
    }
    else
    {
      tempInput.replace(baseDirPath,baseDirName);
      outputPath = outputPath + "/"+ tempInput;
    }

    QDir dir(outputPath);
    if (!dir.exists())
      dir.mkpath(".");
    m_taskList[i].setResultOutputFolder(outputPath);

  }

}


/*!
* @fn       rerunTaskConfigList
* @param    QString folderPath
* @return   QStringList
* @brief    returns list of output folder for batch file
*/
void RerunBatchManager::batchOutputList(QString tempBagList, QString inputFolderPath)
{
  qDebug()<<"Input Folder Path "<< inputFolderPath;


}

/*!
* @fn       getRerunTaskList
* @param    none
* @return   QList<RerunTaskConfig>
* @brief    returns list of Rerun Task Configs
*/
QVariant RerunBatchManager::getRerunTaskList(int i)
{
  QVariantList list;
  QVariant conf;
  list.reserve(m_taskList.size());
  for(auto val : m_taskList)
  {
    // list.append(QVariant::fromValue(val));
  }
  RerunTaskConfig temp = m_taskList.at(i);
  //  conf.fromValue(temp);
  // return QVariant::fromValue(m_taskList.at(i));
  return conf;
}

QList<RerunTaskConfig> RerunBatchManager::getRerunTaskList2()
{
  return m_taskList;
}

/*!
  * @fn       getPropertyValue
  * @param    QString property
  * @param    int index
  * @return   QString
  * @brief    get property with index
  */
QString RerunBatchManager::getPropertyValue(QString property, int index)
{
  RerunTaskConfig temp = m_taskList.at(index);
  if(property.compare("bagInputFolder")==0)
  {
    return temp.bagInputFolder();
  }
  else if(property.compare("resultOutputFolder") == 0)
  {
    return temp.resultOutputFolder();
  }
  else if(property.compare("editBagListFile") == 0)
  {
    return temp.editBagListFile();
  }
  else if(property.compare("selectedBagListFile") == 0)
  {
    return temp.selectedBagListFile();
  }
  else if(property.compare("isrunc") == 0)
  {
    bool te = temp.getIsRun();
    if(te == true)
      return "true";
    else
      return "false";
  }
  else
  {
    return "";
  }

}


/*!
  * @fn       setPropertyValue
  * @param    QString property
  * @param    QString value
  * @param    int index
  * @return   bool
  * @brief    set property with index
  */
bool RerunBatchManager::setPropertyValue(QString property, QString value, int index)
{
  if(property.compare("bagInputFolder")==0)
  {
    m_taskList[index].setBagInputFolder(value);
  }
  else if(property.compare("resultOutputFolder") == 0)
  {
    m_taskList[index].setResultOutputFolder(value);
  }
  else if(property.compare("editBagListFile") == 0)
  {
    m_taskList[index].setEditBagListFile(value);
  }
  else if(property.compare("isrunc") == 0)
  {
    if(value.compare("true")==0)
    {
      m_taskList[index].setIsRun(true);
    }
    else
    {
      m_taskList[index].setIsRun(false);
    }
  }
}

/*!
  * @fn       getRerunTaskCount
  * @param    None
  * @return   int
  * @brief    get task count
  */
int RerunBatchManager::getRerunTaskCount()
{
  return m_taskList.count();
}

/*!
  * @fn       clearTaskList
  * @param    None
  * @return   None
  * @brief    Clears task list
  */
void RerunBatchManager::clearTaskList()
{
  m_taskList.clear();
}

/*!
  * @fn       batchListEditorTextAreaValidity
  * @param    None
  * @return   None
  * @brief    checks if edited batch list is valid or not.
  */
QString RerunBatchManager::batchListEditorTextAreaValidity(QString batchFileEditorContents)
{
  QString result = "";
  QStringList batchFileEditorContentsList = batchFileEditorContents.split("\n");
  int len = batchFileEditorContentsList.size();
  for(int i=0;i<len;i++)
  {
    //batchFileEditorContentsList[i].remove('/');
    qDebug()<<batchFileEditorContentsList[i]<<"at position"<<i;
    if(batchFileEditorContentsList[i] != "")
    {
      //QFileInfo batchFileInfo(batchFileEditorContentsList[i]);
      bool fileExists = QFileInfo::exists(batchFileEditorContentsList[i]);
      if(fileExists == false)
      {
        qDebug()<<batchFileEditorContentsList[i]<<" file does not exist";
        result += batchFileEditorContentsList[i];
        result +="\n";
        qDebug()<< result << ":Result";
      }
    }

  }
  return result;
}

/*!
  * @fn       synchornizeTaskList
  * @param    batchFilePath
  * @return   None
  * @brief    checks if edited batch list is valid or not.
  */
void RerunBatchManager::synchronizeTaskList(QString batchFileContents)
{
  QList<RerunTaskConfig> tempList;// = new QList<RerunTaskConfig>();
  QStringList batchFileUpdatedContents = batchFileContents.split(",");
  if(!m_taskList.isEmpty())
  {
    for(int i=0; i<m_taskList.size(); i++)
    {
      RerunTaskConfig temp = m_taskList.at(i);
      tempList.append(temp);
    }
  }
  clearTaskList();
  if(!tempList.isEmpty())
  {
    for (int i=0; i<batchFileUpdatedContents.count(); i++)
    {
      QString tempFile = batchFileUpdatedContents[i];
      bool present =false;
      for(int j=0; j<tempList.size(); j++)
      {
        RerunTaskConfig temp = tempList.at(j);
        if (temp.selectedBagListFile().compare(tempFile) == 0)
        {
          m_taskList.append(temp);
          present = true;
          break;
        }
      }
      if(present == false)
      {
        RerunTaskConfig tempConfig;
        QFileInfo tempF(tempFile);
        tempConfig.setBagInputFolder(tempF.absolutePath());
        tempConfig.setSelectedBagListFile(tempFile);
        tempConfig.setEditBagListFile(tempFile);
        tempConfig.setIsRun(false);
        m_taskList.append(tempConfig);
      }
    }
  }
  else
  {
    for (int i=0; i<batchFileUpdatedContents.count(); i++)
    {
      QString tempFile = batchFileUpdatedContents[i];
      RerunTaskConfig tempConfig;
      QFileInfo tempF(tempFile);
      tempConfig.setBagInputFolder(tempF.absolutePath());
      tempConfig.setSelectedBagListFile(tempFile);
      tempConfig.setEditBagListFile(tempFile);
      tempConfig.setIsRun(false);
      m_taskList.append(tempConfig);
    }
  }
}
}
}





