/*!
* @file     rerunviewerhandler.h
* @author   Agiliad
* @brief    This file contains interface, responsible review after detection run
* @date     March, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_RERUN_RERUNVIEWERHANDLER_H_
#define ANALOGIC_WS_RERUN_RERUNVIEWERHANDLER_H_

#include <QObject>
#include <QMutex>
#include <analogic/ws/rerun/rerunbaglist.h>
#include <analogic/ws/rerun/rerunconfig.h>
#include <analogic/ws/rerun/usbfilesystemmodel.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{

namespace ws
{

enum BagConstructionState
{
  CS_PENDING = 0,
  CS_FAILED,
  CS_SUCCESS
};

typedef struct bagCheckList
{
  QString bagFilename;
  BagConstructionState isConstructed;
  bool isRendering;
}bagCheckList;

static QMutex m_bagCheckListMutex;

/*!
 * \class   RerunViewerHandler
 * \brief   This class responsible for review after detection run
*/

class RerunViewerHandler : public QObject
{
  Q_OBJECT
public:
  /*!
       * @fn       RerunViewerHandler
       * @param    None
       * @return   None
       * @brief    Constructor for class RerunViewerHandler.
       */
  RerunViewerHandler();

  /*!
       * @fn       ~RerunViewerHandler
       * @param    None
       * @return   None
       * @brief    Destructor for class RerunViewerHandler.
       */
  ~RerunViewerHandler();

  /*!
    * @fn       getUsbFileSystemModel
    * @param    None
    * @return   UsbFileSystemModel*
    * @brief    gives UsbFileSystemModel object
    */
  UsbFileSystemModel* getUsbFileSystemModel();

  /*!
    * @fn       init
    * @param    None
    * @return   None
    * @brief    intialization of baglist and selectionmodel.
    */
  void init();

  /*!
    * @fn       setOperatorName
    * @param    QString
    * @return   None
    * @brief    set Operator Name in TDR.
    */
  void setOperatorName(QString operatorName);

  /*!
    * @fn       setUserName
    * @param    QString
    * @return   None
    * @brief    set User Name.
    */
  void setUserName(QString userName);

  /*!
    * @fn       createOQTSummaryReport
    * @param    null
    * @return   None
    * @brief    Update OQT Summary Report and
    *           add Summary line in OQT Detailed Report.
    */
  void createOQTSummaryReport();

  /*!
    * @fn       totalFalseAlarmAndThreatImages
    * @param    null
    * @return   None
    * @brief    Calculate Total FalseAlarm and ThreatImages
    *           in Selected Playlist.
    */
  void totalFalseAlarmAndThreatImages();

signals:
  /*!
    * @fn       notifyToStopShowingBag
    * @param    None
    * @return   None
    * @brief    signals to stop showing bags.
    */
  void notifyToStopShowingBag();

  /*!
    * @fn       notifyToChangeOrNotScreen
    * @param    bool toBeChanged
    * @param    QString erroMsg
    * @param    int hr
    * @return   None
    * @brief    notifies viewmanager to change screen or not.
    */
  void notifyToChangeOrNotScreen(bool toBeChanged, QString erroMsg, int hr);

public slots:
  /*!
    * @fn       onViewSelectedBags
    * @param    QVariantList
    * @param    QModelIndex outputFilePathIndex
    * @param    QString opID
    * @return   None
    * @brief    notifies workstationManager when ViewBags is pressed.
    */
  void onViewSelectedBags(QVariantList selectedBagsList, QModelIndex outputFilePathIndex, QString opID = "");

  /*!
    * @fn       showNextBag
    * @param    None
    * @return   None
    * @brief    sest next bag in Bagdata.
    */
  void showNextBag();

  /*!
    * @fn       saveOperatorDecision
    * @param    QString bagId
    * @param    QMLEnums::ThreatEvalPanelCommand decision
    * @return   None
    * @brief    save operator decision
    */
  void saveOperatorDecision(QString bagId , QMLEnums::ThreatEvalPanelCommand decision, float timeToTakeDecision, QDateTime renderStartTime);

public:

  static QList<bagCheckList*> listOfBags;
private:
  /*!
    * @fn       getBagFromPool
    * @param    QString bagDataFilePath
    * @return   bool
    * @brief    gets bag from BagDataPool.
    */
  bool getBagFromPool(QString bagDataFilePath);

  /*!
    * @fn       checkForLastBag
    * @param    int index
    * @return   bool
    * @brief    checks if currently viwed bag is last
    */
  bool checkForLastBag(int index);

  /*!
    * @fn       checkAvailableDiskSpace
    * @param    QString
    * @return   bool
    * @brief    check enough space available on drive
    */
  bool checkAvailableDiskSpace(QString bagDataFilePath);

  QString                             m_currentlyViewedBag;       //!< handle to bagId of currently viewed bag
  QString                             m_outputFilePath;           //!< handle to output folder path which contains tdr file
  QStringList                         m_selectedBagList;          //!< handle to selected list of bags
  UsbFileSystemModel*                 m_usbfsm;                   //!< UsbFS Handler
  boost::shared_ptr<RerunBagList>     m_rerunBagList;             //!< handle to RerunBagList
  static QString                      m_op_decision_file;         //!< opearator decision file
  static QString                      m_oqt_detailreport_file;         //!OQT Summary Report file
  static QString                      m_oqt_summaryreport_file;         //!< OQT Summary Report file
  int                                 m_currentBagIndex;          //!< Current Bag Index
  QStringList                         m_uniqueBagFolderPathList;  //!< contain unique list of path to check available space
  RerunConfig*                        m_rerunConfigObj;
  QString                             m_userName;
  QString                             m_opID;
  QString                             m_playlistname;
  float                               m_totalTimeOfTest;              //!< Time taken by Operator to complete the Test
  float                               m_score;                        //!< holds the total count of correct decision
  float                               m_pd;                           //!< holds the total count of correct decion incase of threat image
  float                               m_pfa;                          //!< holds the total count of correct decion in case of falsee alarm images.
  float                               m_pdTotal;                      //!< total number of Threat images
  float                               m_pfaTotal;                     //!< total number of False alarm inages
  QString                             m_initialBagRenderTime;         //!< holds the  1st bag redered time for a playlist
  QString                             m_bagRederedDate;
};

}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_RERUN_RERUNVIEWERHANDLER_H_
