/*!
* @file     usbfilesystemmodel.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to usb fileSystemModel
*           which displays usb file system.
* @date     Apr 18, 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_USBFILESYSTEMMODEL_H_
#define ANALOGIC_WS_UIHANDLER_USBFILESYSTEMMODEL_H_

#include <QFileSystemModel>
#include <analogic/ws/rerun/rerunconfig.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   UsbFileSystemModel
 * \brief   This class contains variable and function related to
 *          usb file system model
 */
class UsbFileSystemModel : public QFileSystemModel {
    Q_OBJECT
public:
    /*!
    * @fn       UsbFileSystemModel
    * @param    QObject *parent
    * @return   None
    * @brief    Constructor for class UsbFileSystemModel.
    */
    explicit UsbFileSystemModel(QObject *parent = Q_NULLPTR);

    /*!
    * @fn       UsbFileSystemModel
    * @param    None
    * @return   None
    * @brief    Destructor for class UsbFileSystemModel.
    */
    ~UsbFileSystemModel();

    enum Roles  {
        BagCountRole = Qt::UserRole + 4,
        AlarmCountRole = Qt::UserRole + 5,
        AlarmRateRole = Qt::UserRole + 6,
        DescriptionRole = Qt::UserRole + 7
    };
    Q_ENUM(Roles)

    /*!
    * @fn       initialize
    * @param    None
    * @return   None
    * @brief    intialization of usb fs.
    */
    void initialize();

    /*!
    * @fn       data
    * @param    const QModelIndex &index
    * @param    int role = Qt::DisplayRole
    * @return   QVariant
    * @brief    Gives data for current role and index.
    */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /*!
    * @fn       roleNames
    * @param    None
    * @return   QHash<int,QByteArray>
    * @brief    gives roleNames.
    */
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /*!
    * @fn       bagSelectionList
    * @param    const QModelIndex &index
    * @return   QVariant
    * @brief    Gives bagList for current index.
    */
    Q_INVOKABLE QVariant bagSelectionList(const QModelIndex &index);

    /*!
    * @fn       trainingDetailedReportdataList
    * @param    None
    * @return   QVariantList
    * @brief    populates the OQT Detailed Report table.
    */
    Q_INVOKABLE QVariant trainingSummaryReportdataList(QString selectedUserID, QString selectedUserReport);

    /*!
    * @fn       trainingSummaryReportdataList
    * @param    None
    * @return   QVariantList
    * @brief    populates the OQT Summary Report table.
    */
    Q_INVOKABLE QVariant trainingDetailedReportdataList(QString selectedUserID, QString selectedUserReport);

    /*!
    * @fn       bagIDList
    * @param    const QString playlistname
    * @return   QVariant
    * @brief    Gives bagList for current playlistname.
    */
   Q_INVOKABLE QVariant bagIDList(const QString playlistname);

    /*!
    * @fn       readFromDecisionCSV
    * @param    const QString playlistname
    * @return   QVariant
    * @brief    Gives bagList for current playlistname.
    */
   Q_INVOKABLE QVariant readFromDecisionCSV(const QString playlistname);

    /*!
    * @fn       setRootDirPath
    * @param    None
    * @return   QModelIndex
    * @brief    sets rootPath and Gives modelIndex of current rootPath.
    */
    Q_INVOKABLE QModelIndex setRootDirPath();

    /*!
    * @fn       setPlaylistFilePath
    * @param    None
    * @return   QModelIndex
    * @brief    sets playlist file path from configuration file.
    */
    Q_INVOKABLE QModelIndex setPlaylistFilePath();

    /*!
    * @fn       getPlayListName
    * @param    None
    * @return   QString
    * @brief    sets playlist file name from configuration file.
    */
    Q_INVOKABLE QString getPlayListName();

    /*!
    * @fn       readFromFile
    * @param    QString filePathName
    * @return   bool
    * @brief    return the contents of a given file line-by-line
    */
    Q_INVOKABLE bool readFromCsvFile(QString filePathName);

    /*!
    * @fn       readFromFile
    * @param    none
    * @return   QMap<QString,QString>
    * @brief    retrun the playlist categorise file details to comapre an create OQT Reports
    */
    Q_INVOKABLE QMap<QString,QString> getPlaylistDetails();


    /*!
    * @fn       getAllChildrenIndexes
    * @param    None
    * @return   QVariant
    * @brief    Gives modelIndexes of all child folders & files.
    */
    Q_INVOKABLE QVariant getAllChildrenIndexes();


    /*!
    * @fn       getRootIndex
    * @param    None
    * @return   QModelIndex
    * @brief    return playlist file path from configuration file.
    */
    Q_INVOKABLE QModelIndex getRootIndex();

private:
    /*!
    * @fn       customFileInfo
    * @param    const QFileInfo &fi
    * @return   bool
    * @brief    Gives customFileInfo of current file.
    */
    bool customFileInfo(const QFileInfo &fi) const;

    /*!
    * @fn       getChildrenIndexes
    * @param    QString
    * @param    QVariantList& - indexlist
    * @return   None
    * @brief    Gives modelIndexes of child folders & files.
    */
    void getChildrenIndexes(QString parentPath, QVariantList& indexList);

    /*!
    * @fn       getPlaylistChildrenIndexes
    * @param    QString
    * @param    QVariantList& - indexlist
    * @return   None
    * @brief    Gives modelIndexes of child Playlist files.
    */
    void getPlaylistChildrenIndexes(QString parentPath, QVariantList& indexList);

    uint                                   m_bagCount;            //!< handle to bag count
    uint                                   m_alarmCount;          //!< handle to alarm count
    uint                                   m_alarmRate;           //!< handle to alarm rate
    QString                                m_description;         //!< handle to description
    RerunConfig*                           m_rerunConfigObj;
    QString                                selectedUserID;
    QString                                selectedUserReport;
    QString                                m_playlistname;
    QMap<QString,QString>  m_playlistCategorizeFileDetails;
};

/*!
 * \class   BagFileModel
 * \brief   This class contains variable and function related to
 *         bag file model
 */
class BagFileModel
{
    Q_GADGET
    Q_PROPERTY(QString bagDataFilePath READ bagDataFilePath)
    Q_PROPERTY(QString bagId READ bagId)
    Q_PROPERTY(QString machineDecison READ machineDecison)

public:
    QString bagDataFilePath() const { return m_bagDataFilePath; }
    QString bagId() const { return m_bagId; }
    QString machineDecison() const { return m_machineDecison; }

    QString     m_bagDataFilePath;
    QString     m_bagId;
    QString     m_machineDecison;

};

/*!
 * \class   PlaylistDetails
 * \brief   This class contains variable and function related to
 *         Playlist File Details
 */
class PlaylistDetails{
  Q_GADGET
  Q_PROPERTY(QString threatCategory READ threatCategory)
  Q_PROPERTY(QString machineCorrectDecison READ machineCorrectDecison)
  Q_PROPERTY(QString bagID READ bagID)

  public:
  QString threatCategory() const { return m_threatCategory; }
  QString machineCorrectDecison() const { return m_machineCorrectDecison; }
  QString bagID() const { return m_bagId; }

  QString     m_bagId;
  QString     m_threatCategory;
  QString     m_machineCorrectDecison;
};

class ReportFileModel
{
  Q_GADGET
  Q_PROPERTY(QString userID READ userID)
  Q_PROPERTY(QString userName READ userName)
  Q_PROPERTY(QString siteCode READ siteCode)
  Q_PROPERTY(QString date READ date)
  Q_PROPERTY(QString time READ time)
  Q_PROPERTY(QString testName READ testName)
  Q_PROPERTY(QString pd READ pd)
  Q_PROPERTY(QString pfa READ pfa)
  Q_PROPERTY(QString score READ score)
  Q_PROPERTY(QString elapsedTime READ elapsedTime)
  Q_PROPERTY(QString userDecision READ userDecision)
  Q_PROPERTY(QString expected READ expected)
  Q_PROPERTY(QString result READ result)
  Q_PROPERTY(QString bagID READ bagID)
  Q_PROPERTY(QString category READ category)


public:
  QString userID() const { return m_useName; }
  QString userName() const { return m_userId; }
  QString siteCode() const { return m_siteCode; }
  QString date() const { return m_date; }
  QString testName() const { return m_testName; }
  QString pd() const { return m_pd; }
  QString pfa() const { return m_pfa; }
  QString score() const { return m_score; }
  QString time() const { return m_time; }
  QString elapsedTime() const { return m_elapsedTime; }
  QString userDecision() const { return m_userDecision; }
  QString expected() const { return m_expected; }
  QString result() const { return m_Result; }
  QString bagID() const { return m_BagID; }
  QString category() const { return m_Category; }


  QString     m_useName;
  QString     m_userId;
  QString     m_siteCode;
  QString     m_date;
  QString     m_time;
  QString     m_testName;
  QString     m_pd;
  QString     m_pfa;
  QString     m_score;
  QString     m_elapsedTime;
  QString     m_userDecision;
  QString     m_expected;
  QString     m_Result;
  QString     m_BagID;
  QString     m_Category;


};
}  // namespace ws
}  // namespace analogic
Q_DECLARE_METATYPE(analogic::ws::BagFileModel)
Q_DECLARE_METATYPE(analogic::ws::PlaylistDetails)
#endif  // ANALOGIC_WS_UIHANDLER_USBFILESYSTEMMODEL_H_

