#ifndef RERUNBATCHMANAGER_H
#define RERUNBATCHMANAGER_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QList>
#include <QDateTime>
#include <utility.h>
#include <analogic/ws/rerun/reruntaskconfig.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{


class RerunBatchManager : public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       Rerun Batch Manager
    * @param    None
    * @return   None
    * @brief    Constructor for class Rerun Batch Manager.
    */

    explicit RerunBatchManager(QObject* parent = NULL);

    /*!
    * @fn       ~Rerun Batch Manager
    * @param    None
    * @return   None
    * @brief    Destructor for class Rerun Batch Manager.
    */
    ~RerunBatchManager();

    /*!
    * @fn       getBatchFileFromInputFolder
    * @param    QString folderPathName
    * @return   QString
    * @brief    returns list of files recursively for given name filter
    */
    Q_INVOKABLE QString getBatchFileFromInputFolder(QString folderPathName, QString batchFileName);

    /*!
    * @fn       getBagListFromInputFolder
    * @param    QString folderPath
    * @return   QStringList
    * @brief    returns list of .vol bag files recursively
    */
    Q_INVOKABLE QString getBagListFromFolder(QString folderPath);


    /*!
    * @fn       createBatchOutputFolder
    * @param    QString folderPath
    * @return   QStringList
    * @brief    returns list of output folder for batch file
    */

    Q_INVOKABLE void createBatchOutputFolder(QString outputFolderPath, QString batchList);

    /*!
    * @fn       rerunTaskConfigList
    * @param    QString folderPath
    * @return   QStringList
    * @brief    returns list of output folder for batch file
    */
    Q_INVOKABLE void batchOutputList(QString tempBagList, QString inputFolderPath);


    /*!
    * @fn       getPropertyValue
    * @param    QString property
    * @param    int index
    * @return   QString
    * @brief    get property with index
    */
    Q_INVOKABLE QString getPropertyValue(QString property, int index);

    /*!
    * @fn       setPropertyValue
    * @param    QString property
    * @param    QString value
    * @param    int index
    * @return   bool
    * @brief    set property with index
    */
    Q_INVOKABLE bool setPropertyValue(QString property, QString value, int index);


    Q_INVOKABLE QVariant getRerunTaskList(int i);
    Q_INVOKABLE QList<RerunTaskConfig> getRerunTaskList2();

    /*!
    * @fn       getRerunTaskCount
    * @param    None
    * @return   int
    * @brief    get task count
    */
    Q_INVOKABLE int getRerunTaskCount();

    /*!
    * @fn       clearTaskList
    * @param    None
    * @return   None
    * @brief    Clears task list
    */
    Q_INVOKABLE void clearTaskList();

    /*!
      * @fn       batchListEditorTextAreaValidity
      * @param    None
      * @return   None
      * @brief    checks if edited batch list is valid or not.
      */
    Q_INVOKABLE QString batchListEditorTextAreaValidity(QString batchFileEditorContents);

    /*!
      * @fn       synchornizeTaskList
      * @param    batchFilePath
      * @return   None
      * @brief    to synchornize Task List.
      */
    Q_INVOKABLE void synchronizeTaskList(QString batchFileContents);

    QList<RerunTaskConfig> m_taskList;     //!< handle to rerun task config

    QList<RerunTaskConfig> m_rerunTaskConfigList;

};

}  // end of namespace ws
}  // end of namespace analogic
#endif // RERUNBATCHMANAGER_H
