/*!
* @file     view.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to View
*           which handles models of views.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_VIEW_H_
#define ANALOGIC_WS_UIHANDLER_VIEW_H_

#include <boost/shared_ptr.hpp>
#include <QDir>
#include <QUrl>
#include <QProcess>
#include <QDateTime>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQuickView>
#include <QQuickWidget>
#include <QFileDialog>
#include <QQmlContext>
#include <QApplication>
#include <QVariantList>
#include <QDebug>
#include <QDesktopWidget>
#include <QSharedPointer>
#include <QtQml/QQmlContext>
#include <QAbstractListModel>
#include <QPropertyAnimation>
#include <QQmlApplicationEngine>
#include <QSortFilterProxyModel>
#include <QParallelAnimationGroup>
#include <QtQml/QQmlApplicationEngine>
#include <QPrinter>
#include <QPrintDialog>
#include <analogic/ws/common.h>
#include <analogic/ws/filesystemitemmodel.h>
#include <analogic/ws/uihandler/rawimage.h>
#include <analogic/nss/common/NssBag.h>
#include <analogic/ws/rerun/usbfilesystemmodel.h>
#include <analogic/ws/common/utility/usbstorageservice.h>
#include <analogic/ws/common/utility/linuxterminal.h>
#include <analogic/ws/dashboardscreenmodel.h>
#include <analogic/ws/uihandler/supervisoragentsdatamodel.h>
#include <analogic/ws/uihandler/supervisoreventsdatamodel.h>
#include <analogic/ws/uihandler/groupdatamodel.h>
#include <analogic/ws/uihandler/supervisorconfigdatamodel.h>

#ifdef WORKSTATION
#include<analogic/ws/tip/tipfilesystemmodel.h>
#include <analogic/ws/common/accesslayer/scanneraccessinterface.h>
#include <analogic/ws/common/nssevent/nsseventinfo.h>
#endif

#ifdef RERUN
#include <analogic/ws/rerun/reruntaskconfig.h>
#include <analogic/ws/rerun/rerunconfig.h>
#include <analogic/ws/rerun/rerunbatchmanager.h>
#include <analogic/ws/rerun/rerunformatconverter.h>
#endif
#include <analogic/ws/common/utility/customfiledialog.h>
#include <analogic/ws/common/bagdata/baglisthandler.h>

#ifdef WEBENGINE
#include <QQuickWebEngineProfile>
#include <QtWebEngine/qtwebengineglobal.h>
#include <QWebEngineUrlRequestInterceptor>
#include <analogic/ws/uihandler/webengineurlrequestinterceptor.h>
#endif

#include <QtCharts/QAbstractSeries>
QT_CHARTS_USE_NAMESPACE

#define DEFAULT_OPERATORLIST_WINDOW_HEIGHT 335
#define DEFAULT_OPERATORLIST_WINDOW_WIDTH 200

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class VolumeDisplayManager;
class UILayoutManager;
class IdleScreenDetector;
class BagData;
#define VOL_FILE_EXT ".vol"
#define PROJ0_FILE_EXT ".proj00"
#define PROJ90_FILE_EXT ".proj90"
#define DICOS_PROJ00_FILE_EXT ".dicos_ct_proj00"
#define DICOS_PROJ90_FILE_EXT ".dicos_ct_proj90"
#define DE_FILE_EXT ".de"



/*!
 * \class   View
 * \brief   This class contains variable and function related to
 *          view models.
 */
class View :public QObject
{
  Q_OBJECT

public:
  /*!
    * @fn       View
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
  explicit View(QObject* parent = NULL);

  /*!
    * @fn       ~View
    * @param    None
    * @return   None
    * @brief    Destructor for class View
    */
  ~View();

  /*!
    * @fn       initialize
    * @param    None
    * @return   None
    * @brief    initialize members of class View.
    */
  void initialize();

  /*!
    * @fn       registerULMcontext
    * @param    UILayoutManager*
    * @return   None
    * @brief    registers ULM context.
    */
  void registerULMcontext(UILayoutManager* ulmObject);

  /*!
    * @fn       registerUsbFSModel
    * @param    UsbFileSystemModel*
    * @return   None
    * @brief    registers UsbFS context.
    */
  void registerUsbFSModel(UsbFileSystemModel* usbObject);

  /*!
    * @brief    qt property that updates on screen chanes in qml.
    */
  Q_PROPERTY(  int currentView  READ getCurrentView WRITE setCurrentView NOTIFY currentViewChanged );

  /*!
    * @fn       setCurrentView
    * @param    int sView of type QMLEnums::ScreenEnum
    * @return   None
    * @brief    called internally when currentView changes.
    */
  void setCurrentView(int sView);

  /*!
    * @fn       getCurrentView
    * @param    None
    * @return   int sView of type QMLEnums::ScreenEnum
    * @brief    gets current view
    */
  int getCurrentView() const;

  /*!
    * @fn       setUlmDataList
    * @param    UILayoutManager*
    * @return   None
    * @brief    sets Ulm Data List.
    */
  void setUlmDataList(QList<QObject*> inputDataList);

  /*!
    * @brief    qt property that updates on screen chanes in qml.
    */
  Q_PROPERTY(  int prevView READ getPrevView WRITE setPrevView NOTIFY prevViewChanged );

  /*!
    * @fn       setPrevView
    * @param    int
    * @return   None
    * @brief    called internally when previous View changes.
    */
  void setPrevView(int sView);

  /*!
    * @fn       getPrevView
    * @param    None
    * @return   int
    * @brief    gets previous view
    */
  int getPrevView() const;

  /*!
    * @fn       updatemodellist();
    * @param    QMLEnums::ModelEnum sModelName
    * @param    QVariantList data - model data
    * @return   None
    * @brief    sends signal to update Models.
    */
  void updatemodellist(QMLEnums::ModelEnum sModelName , QVariantList oModel);

  /*!
    * @fn       sendRawdataDownloadfinshedSignal();
    * @param    QString sDownloadPath
    * @param    QString sDownloadError
    * @param    QMLEnums::ModelEnum sModelName
    * @return   None
    * @brief    sends signal  that download is finished.
    */
  void sendfileDownloadfinshedSignal(QString sDownloadPath, QString sDownloadError, QMLEnums::ModelEnum modelnum);

  /*!
    * @fn       refreshUiModels
    * @param    None
    * @return   None
    * @brief    refresh UI Models.
    */
  void refreshUiModels();

  /*!
    * @fn      moveFile
    * @param   QString dst
    * @param   QString src
    * @return  int
    * @brief   Function will call on move file.
    */
  Q_INVOKABLE int moveFile(QString dst, QString src);

  /*!
    * @fn      mountUsb
    * @param   None
    * @return  QString
    * @brief   Function will mount usb.
    */
  Q_INVOKABLE QString mountUsb();

  /*!
    * @fn      unMountUsb
    * @param   None
    * @return  None
    * @brief   Function will unmount usb.
    */
  Q_INVOKABLE void unMountUsb();

  /*!
    * @fn       onCommandClick
    * @param    QMLEnums::ModelEnum modelname: Model that needs to be refered while sending commands to the Rest APIs.
    * @param    QString data : Data that needs to be sent.
    * @return   None
    * @brief    invokable method from qml that send commands via rest APIs using model references.
    */
  Q_INVOKABLE void onCommandClick(QMLEnums::ModelEnum modelname, QString data);

  /*!
  * @fn      updateCordinates
  * @param   QAbstractSeries
  * @return  None
  * @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
  */
  Q_INVOKABLE void updateCordinates(QAbstractSeries *series,int modelnum);

  /*!
  * @fn       getModelData
  * @param    QMLEnums::ModelEnum modelname: Index of Model for which data neds to be returned.
  * @return   QVariantMap: a map data
  * @brief    invokable method from qml that send commands via rest APIs using model references.
  */
  Q_INVOKABLE QVariantList getModelData(QMLEnums::ModelEnum modelname);

  /*!
    * @fn       readImageQualityFile
    * @param    QString -file
    * @return   QStringList
    * @brief    parse image quality file and return final result.
    */
  Q_INVOKABLE QString readImageQualityFile(QString file);

  /*!
    * @fn       getWorkstationNameEnum
    * @param    None
    * @return   int
    * @brief    get workstation name from config.
    */
  Q_INVOKABLE int getWorkstationNameEnum();

  /*!
    * @fn      onGetSelectedGroup
    * @param   QString
    * @return  None
    * @brief   This function is responsible for getting selected group
    */
  Q_INVOKABLE void onGetSelectedGroup(QString selectedGroup);

  /*!
    * @fn      onGetSelectedGauge
    * @param   QString
    * @return  None
    * @brief   This function is responsible for getting selected gauge
    */
  Q_INVOKABLE void onGetSelectedGauge(int selectedGauge);


#if defined WORKSTATION || defined RERUN
  /*!
    * @fn       getUserAdminServer
    * @param    None
    * @return   QString
    * @brief    get UserAdminServer IP from config.
    */
  Q_INVOKABLE QString getUserAdminServer();
#endif

  /*!
    * @fn       onauthstatuschanged
    * @param    int - errorcode
    * @param    QString - status
    * @param    QMLEnums::ModelEnum modelno
    * @return   None
    * @brief    called on authentication status changed
    */
  void onauthstatuschanged(int hr, QString status, int modelno);
#if defined WORKSTATION || defined RERUN
  /*!
    * @fn       onsslerrorreceived
    * @param    int - errorcode
    * @param    QString - status
    * @param    QMLEnums::ModelEnum modelno
    * @return   None
    * @brief    called on onsslerrorreceived
    */
  void onsslerrorreceived(int hr, QString status, int modelno);
#endif
  /*!
    * @fn       exitApp();
    * @param    None
    * @return   None
    * @brief    Function will call on close application
    */
  Q_INVOKABLE void exitApp();

  /*!
    * @fn      saveFile
    * @param   QString dst
    * @param   QString src
    * @return  int
    * @brief   Function will call on save file.
    */
  Q_INVOKABLE int saveFile(QString dst, QString src);

  /*!
     * @fn       getModelString
     * @param    QMLEnums::ModelEnum
     * @return   QString
     * @brief    returns modelString
     */
  Q_INVOKABLE QString getModelString(QMLEnums::ModelEnum modelno);

  /*!
    * @fn      createAndGetDir
    * @param   int modelindex
    * QString  filename
    * @return  QString
    * @brief   This function is responsible for create and get rawdir.
    */
  Q_INVOKABLE QString createAndGetDir(int modelindex, QString filename);

  /*!
    * @fn      onopenfile
    * @param   QString- rawdata filename
    * @return
    * @brief   This function is responsible for opening raw data file
    */
  Q_INVOKABLE QByteArray onopenfile(QString filename);


  /*!
    * @fn      readErrorLogData
    * @param   QString - filename
    * @return  QVariantList - error logs
    * @brief   Function will call on save file.
    */
  Q_INVOKABLE QVariantList readErrorLogData(QString strPath);

  /*!
    * @fn      getulmDataList
    * @param   None
    * @return  QVariant
    * @brief   Function gets ulm DataList.
    */
  Q_INVOKABLE QVariant getulmDataList();

  /*!
    * @fn      shutDownSystem
    * @param   None
    * @return  None
    * @brief   This function is responsible for shutting down system
    */
  Q_INVOKABLE void shutDownSystem();

  /*!
    * @fn      restartSystem
    * @param   None
    * @return  None
    * @brief   This function is responsible for restarting system
    */
  Q_INVOKABLE void restartSystem();

  /*!
    * @fn      scannerSetState
    * @param   QMLEnums::ScannerState state
    * @return  None
    * @brief   This function is responsible for setting scanner state
    */
  Q_INVOKABLE void scannerSetState(QMLEnums::ScannerState state);

  /*!
    * @fn      getScanneradminview
    * @param   None
    * @return  QMLEnums::ScannerAdminScreenEnum
    * @brief   This function is responsible for getting current scanner admin view
    */
  Q_INVOKABLE QMLEnums::ScannerAdminScreenEnum getScanneradminview() const;

  /*!
    * @fn      setScanneradminview
    * @param   const int& scanneradminview
    * @return  None
    * @brief   This function is responsible for setting current scanner admin view
    */
  Q_INVOKABLE void setScanneradminview(const int& scanneradminview);

  /*!
    * @fn      createScannerFaultStatusScreen
    * @param   None
    * @return  None
    * @brief   This function is responsible for creating Scanner Fault Status Screen.
    */
  Q_INVOKABLE void createScannerFaultStatusScreen();

  /*!
    * @fn      createOperatorThreatPopupScreen
    * @param   None
    * @return  None
    * @brief   This function is responsible for creating Operator threat Screen.
    */
  Q_INVOKABLE void createOperatorThreatPopupScreen();

  /*!
    * @fn      createNavigationMenuPopupScreen
    * @param   None
    * @return  None
    * @brief   This function is responsible for creating Navigation Menu Screen.
    */
  Q_INVOKABLE void createNavigationMenuPopupScreen();

  /*!
    * @fn      createModalFrame
    * @param   None
    * @return  None
    * @brief   This function is responsible for creating Modal Framefor screen.
    */
  Q_INVOKABLE void createModalFrame();

  /*!
    * @fn      createNotificationBalloonPopup
    * @param   None
    * @return  None
    * @brief   This function is responsible for creating Ballon popup.
    */
  Q_INVOKABLE void createNotificationBalloonPopup();

  /*!
    * @fn      animateScannerFaultStatus
    * @param   None
    * @return  None
    * @brief   This function is responsible for animating x property Scanner Fault Status Screen.
    */
  Q_INVOKABLE void animateScannerFaultStatus();

  /*!
    * @fn      showHideScannerFaultStatus
    * @param   bool flag
    * @return  None
    * @brief   This function is responsible for showing/ hiding Scanner Fault Status Screen.
    */
  Q_INVOKABLE void showHideScannerFaultStatus(bool flag);

  /*!
    * @fn      showHideOperatorThreatOptions
    * @param   bool flag
    * @return  None
    * @brief   This function is responsible for showing/ hiding operator threat options
    */
  Q_INVOKABLE void showHideOperatorThreatOptions(bool flag);

  /*!
    * @fn      showHideNavigationMenu
    * @param   bool flag
    * @return  None
    * @brief   This function is responsible for showing/ hiding Navigation Menu Screen
    */
  Q_INVOKABLE void showHideNavigationMenu(bool flag);

  /*!
    * @fn      resizeNavigationMenu
    * @param   None
    * @return  None
    * @brief   This function will call to resize the navigation menu panel when its width changed and
    *          also responsible for showing/ hiding Navigation Menu Screen
    */
  Q_INVOKABLE void resizeNavigationMenu();

  /*!
    * @fn      showHideBalloonPopup
    * @param   bool flag
    * @return  None
    * @brief   This function is responsible for showing/ hiding Balloon popup.
    */
  Q_INVOKABLE void showHideBalloonPopup(bool flag);

  /*!
    * @fn      getRootWindowWidth
    * @param   int
    * @return  None
    * @brief   This function returns width of rootWindow.
    */
  Q_INVOKABLE int getRootWindowWidth();

  /*!
    * @fn      getRootWindowHeight
    * @param   int
    * @return  None
    * @brief   This function returns height of rootWindow.
    */
  Q_INVOKABLE int getRootWindowHeight();

  /*!
    * @fn      getNavigationPanelWidth
    * @param   int
    * @return  None
    * @brief   This function returns width of Navigation Panel.
    */
  Q_INVOKABLE int getNavigationPanelWidth();

  /*!
  * @fn       getLanguageCode
  * @param    None
  * @return   string  - Language
  * @brief    gets Language Code
  */
  Q_INVOKABLE QString getLanguageCode();

  /*!
  * @fn       getCurrentLanguageLocale
  * @param    None
  * @return   string  - Language
  * @brief    gets Language locale
  */
  Q_INVOKABLE QString getCurrentLanguageLocale();

  /*!
    * @brief    qt property that updates on modal frame change in qml.
    */
  Q_PROPERTY(  int currModalFrame READ getCurrModalFrame WRITE setCurrModalFrame/* NOTIFY prevViewChanged */);

  /*!
    * @fn       setCurrModalFrame
    * @param    int
    * @return   None
    * @brief    called internally when current Modal Frame changes.
    */
  void setCurrModalFrame(int sMF);

  /*!
    * @fn       getCurrModalFrame
    * @param    None
    * @return   int
    * @brief    gets Current Modal Frame
    */
  int getCurrModalFrame() const;

  /*!
    * @fn      setModalFrame
    * @param   QMLEnums::ModalFrameComp comp
    * @param   QString errorMsgText=NULL
    * @return  int
    * @brief   This function sets and then show component of ModalFrame for a error
    */
  Q_INVOKABLE int setModalFrame(int modalComp, QString errorMsgText, int nextScreen = -1);

  /*!
    * @fn      hideModalFrame
    * @param   None
    * @return  None
    * @brief   This function is responsible for hiding and disabling ModalFrame
    */
  Q_INVOKABLE void hideModalFrame();

#ifdef WEBENGINE
  /*!
    * @fn       webEngineViewUserAdminLoaded
    * @param    none
    * @return   none
    * @brief    Called from QML when web view loads.
    */
  Q_INVOKABLE void webEngineViewUserAdminLoaded();
#endif

  /*!
    * @fn       onBLBDCCommand
    * @param    QMLEnums::ArchiveFilterSrchLoc srchLoc
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @param    QVariant data : Data that needs to be sent.
    * @return   None
    * @brief    invokable method from qml that send BagsListAndBagData commands to viewManager.
    */
  Q_INVOKABLE void onBLBDCCommand(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                                  QMLEnums::BagsListAndBagDataCommands command, QVariant data);

  /*!
    * @fn       getLoginStatus
    * @param    entity
    * @return   int
    * @brief    gets login status for entity
    */
  Q_INVOKABLE int getLoginStatus(int entity);

#ifdef RERUN
  /*!
    * @fn       rerunEmulatorCommand
    * @param    QMLEnums::RERUN_COMMANDS
    * @param    RerunTaskConfig* test
    * @return   none
    * @brief    send rerun command from UI
    */
  Q_INVOKABLE void rerunEmulatorCommand(QMLEnums::RERUN_COMMANDS cmd, RerunTaskConfig* test);

  /*!
    * @fn       getPlaylistPath
    * @param    None
    * @return   QString
    * @brief    Return Training Playlist Path
    */
  Q_INVOKABLE QString getPlaylistPath();

  /*!
    * @fn       getPlaylistDecisionPath
    * @param    None
    * @return   QString
    * @brief    Return Training Playlist Decision Path
    */
  Q_INVOKABLE QString getPlaylistDecisionPath();

  /*!
    * @fn       getReportPath
    * @param    None
    * @return   QString
    * @brief    Return Report Path
    */
  Q_INVOKABLE QString getReportResultPath(QString m_userID);

  /*!
    * @fn       getPlaylistFileNames
    * @param    None
    * @return   QString
    * @brief    Return Training Playlist File Names
    */
  Q_INVOKABLE QStringList getPlaylistFileNames();

  /*!
  * @fn       setUserID
  * @param    None
  * @return   QString
  * @brief    update User ID selected in training report screen
  */
  Q_INVOKABLE QString setUserID(QString m_userID);

  /*!
  * @fn       getCsvReportFileNames
  * @param    None
  * @return   QStringList
  * @brief    Return Training Report File Name for specified userID
  */
  Q_INVOKABLE QStringList getCsvReportFileNames(QString reportTypeSelected, QString m_userID);
#endif

  /*!
    * @fn       customFiledialog
    * @param    int mode,int type,int acceptmode, QString title, QString path, QString nameFilter
    * @return   QStringList
    * @brief    send rerun config from UI
    */
  Q_INVOKABLE QString customFiledialog(int mode, int type, int acceptmode, QString title,
                                       QString path, QString nameFilter);

  /*!
    * @fn       viewSelectedBags
    * @param    QVariantList
    * @param    QModelIndex outputFilePathIndex
    * @param    QString opID
    * @return   None
    * @brief    called when ViewBags is pressed.
    */
  Q_INVOKABLE void viewSelectedBags(QVariantList selectedBagsList,
                                    QModelIndex outputFilePathIndex,
                                    QString opID = "");

  /*!
    * @fn       readFromFile
    * @param    QString filePathName
    * @return   QStringList
    * @brief    return the contents of a given file line-by-line
    */
  Q_INVOKABLE QStringList readFromFile(QString filePathName);

  /*!
    * @fn       fileExists();
    * @param    None
    * @return   bool
    * @brief    Function will check file exists or not
    */
  Q_INVOKABLE bool folderExists(QString filepathName);

  /*!
    * @fn       fileExists();
    * @param    QString
    * @return   bool
    * @brief    Function will check file exists or not
    */
  Q_INVOKABLE bool fileExists(QString filenamepath);

  /*!
    * @fn       checkFileFolderPermission();
    * @param    QString
    * @param    QString
    * @return   int
    * @brief    Function will check file exists or not
    */
  Q_INVOKABLE int checkFileFolderPermission(QString filenamepath, QString errStr);

  /*!
    * @fn       writeInFile
    * @param    QString filePathName
    * @param    QString
    * @return   bool
    * @brief    writes the contents in a given file line-by-line
    */
  Q_INVOKABLE bool writeInFile(QString filePathName, QString content);

  /*!
    * @fn       getFilterFilesFromInputFolder
    * @param    QString folderPathName
    * @param    QString nameFilter
    * @return   QStringList
    * @brief    returns list of files for given name filter
    */
  Q_INVOKABLE QStringList getFilterFilesFromInputFolder(QString folderPathName, QString nameFilter);

  /*!
    * @fn       getFilterFilesRecursivelyFromInputFolder
    * @param    QString folderPathName
    * @param    QString nameFilter
    * @return   QStringList
    * @brief    returns list of files recursively for given name filter
    */
  Q_INVOKABLE QStringList getFilterFilesRecursivelyFromInputFolder(QString folderPathName, QString nameFilter);

  /*!
    * @fn       getFilterFilesNonRecursivelyFromInputFolder
    * @param    QString folderPathName
    * @param    QString nameFilter
    * @return   QStringList
    * @brief    returns list of files recursively for given name filter
    */
  Q_INVOKABLE QStringList getFilterFilesNonRecursivelyFromInputFolder(QString folderPathName, QString nameFilter);

  /*!
    * @fn       validateRerunEmulatorList
    * @param    QString baglist
    * @param    QString ext
    * @return   QString - error msg
    * @brief    validate input bag data list
    */
  Q_INVOKABLE QString validateRerunEmulatorBagList(QStringList baglist);
  /*!
    * @fn       prepareRerunBagList
    * @param    QString filename
    * @param    QString folderPathName
    * @param    QString nameFilter
    * @return   QStringList
    * @brief    returns rerun Bag List
    */
  Q_INVOKABLE QStringList prepareRerunBagList(QString filename, QString folderPathName, QString nameFilter);

  /*!
   * @fn       removeTemporaryDirectory;
   * @param    none
   * @return   None
   * @brief    function to remove local storage created during rerun detection.
   */
  Q_INVOKABLE void removeTemporaryDirectory();

#ifdef WEBENGINE
  /*!
    * @fn       initializeWebEngine
    * @param    void
    * @return   void
    * @brief    initialize web engine.
    */
  Q_INVOKABLE void initializeWebEngine();

  /*!
    * @fn       setAuthTokenToWebview
    * @param    None
    * @return   None
    * @brief    set auth token
    */
  Q_INVOKABLE void setAuthTokenToWebview();
#endif

  /*!
    * @fn      printFile
    * @param   QString filename
    * @return  None
    * @brief   Print file to printer
    */
  Q_INVOKABLE void printFile(QString filename);

  /*!
     * @fn       setMainContainerFocus
     * @param    None
     * @return   None
     * @brief    set Main Container focus
     */
  Q_INVOKABLE void setMainContainerFocus();

  /*!
    * @fn       getContainer
    * @param    None
    * @return   None
    * @brief    return qmlcontainer.
    */
  QQuickView* getContainer();

  /*!
    * @fn       deleteFiledialogInstance
    * @param    None
    * @return   None
    * @brief    delete file dialog instance
    */
  Q_INVOKABLE void deleteFiledialogInstance();

  /*!
    * @fn       hideFiledialogInstance
    * @param    None
    * @return   None
    * @brief    hide file dialog instance
    */
  Q_INVOKABLE void hideFiledialogInstance();

  /*!
    * @fn      convertValue
    * @param   QMLEnums::conversionEnum conversion
    * @param   double inputValue
    * @return  double
    * @brief   Function will convert value according to unit.
    */
  Q_INVOKABLE double convertValue(QMLEnums::conversionEnum conversion, double inputValue);

  /*!
    * @fn      getOperatorName
    * @param   none
    * @return  Qstring
    * @brief   Function will return current operator name
    */
  Q_INVOKABLE QString getOperatorName();


  /*!
    * @fn      checkFreeSpace
    * @param   none
    * @return  qint64
    * @brief    returns for free bytes available on disk
    */

  Q_INVOKABLE qint64 checkFreeSpace();

  /*!
    * @fn      setOperatorName
    * @param   Qstring
    * @return  none
    * @brief   Function will set current operator name
    */
  Q_INVOKABLE void setOperatorName(QString username);

  /*!
    * @fn      getFileDialogBrowsePath
    * @param   none
    * @return  QString
    * @brief   returns file dialog browsepath
    */
  Q_INVOKABLE QString getFileDialogBrowsePath();

  /*!
  * @fn       getScannerFaultScrnShown
  * @param    None
  * @return   None
  * @brief    get Scanner fault windows is visible.
  */
  Q_INVOKABLE bool getScannerFaultScrnShown() const;

  /*!
  * @fn       getBalloonPopupShown
  * @param    None
  * @return   None
  * @brief    get Balloon popup windows is visible.
  */
  Q_INVOKABLE bool getBalloonPopupShown() const;

  /*!
  * @fn       getExpiredAuthMethod
  * @param    None
  * @return   QMLEnums::AuthMethodType
  * @brief    get Expired Auth Method Enum.
  */
  Q_INVOKABLE int getExpiredAuthMethod() const;

  /*!
  * @fn       getPasswordExpirationDays
  * @param    None
  * @return   int
  * @brief    returns number of days remaning for password to expire.
  */
  Q_INVOKABLE int getPasswordExpirationDays() const;

  /*!
  * @fn       SetMaintenanceMode
  * @param    bool
  * @return   bool
  * @brief    enable/disable scanner maintenance mode.
  */
  Q_INVOKABLE bool setMaintenanceMode(bool enabled);

  /*!
  * @fn       reportEventToNssAgent
  * @param    QMLEnums::NssEventEnum eventId
  * @param    QString eventDescription
  * @return   bool
  * @brief    Report Event to NSS VM.
  */
  Q_INVOKABLE bool reportEventToNssAgent(QMLEnums::NssEventEnum eventId, QString eventDescription);

  /*!
  * @fn       setDensitySliderValue
  * @param    double val
  * @return   void
  * @brief    set densssity slider value for non blocking call
  */
  Q_INVOKABLE void setDensitySliderValue(double val);

  /*!
  * @fn       launchLinuxTerminal
  * @param    bool mode
  * @return   void
  * @brief    launch linux xfce or gnome terminal
  */
  Q_INVOKABLE void launchLinuxTerminal(bool mode);

  /*!
  * @fn       exitLinuxTerminal
  * @param    void
  * @return   void
  * @brief    exit linux xfce or gnome terminal
  */
  Q_INVOKABLE void exitLinuxTerminal();
  /*!
  * @fn       getNSSUserList
  * @return   QStringList
  * @brief    Gets list of users from nss services.
  */


#ifdef WORKSTATION
  Q_INVOKABLE QStringList getNSSUserList();

  /*!
  * @fn       getNSSAgentList
  * @return   QStringList
  * @brief    Gets list of nss agents from nss services.
  */
  Q_INVOKABLE QStringList getNSSAgentList();
#endif 

public slots:
  /*!
    * @fn       onUpdateState
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
  void onUpdateState(QMLEnums::AppState state);

  /*!
    * @fn       notifyToStopShowingBag
    * @param    None
    * @return   None
    * @brief    signals to stop showing bags.
    */
  void notifyToStopShowingBag();

  /*!
       * @fn       onUpdateConfigParameters;
       * @param    None
       * @return   None
       * @brief    updates configuration parameters
       */
  void onUpdateConfigParameters();

  /*!
    * @fn       initAndGetViewInstance
    * @param    None
    * @return   View*
    * @brief    Initalize and get view instance singletone
    */
  static View* initAndGetViewInstance();

  /*!
    * @fn       onMousueClickedEvent
    * @param    None
    * @return   None
    * @brief    slot called when mouse click event occurs.
    */
  void onMousueClickedEvent();

  /*!
   * @fn       onProcessUpdateModels
   * @param    None
   * @return   None
   * @brief    process models.
   */
  void onProcessUpdateModels();

signals:
  /*!
    * @fn      scannerStateChanged
    * @param   QMLEnums::ScannerState state
    * @return  None
    * @brief   Signal gets emit on scanner state changed
    */
  void scannerStateChanged(QMLEnums::ScannerState state);

  /*!
    * @fn      densitsliderChanged
    * @param   double val
    * @return  None
    * @brief   Signal gets emit on density slider change for non blockinf call
    */
  void densitsliderChanged(double val);

  /*!
    * @fn       scannerFaultReceived
    * @param    QString scannerstate
    * @return   None
    * @brief    siganl emits when scanner fault changes.
    */
  void scannerFaultReceived(QString scannerstate);

  /*!
    * @fn       currentViewChanged
    * @param    None
    * @return   None
    * @brief    responsible for changing current view.
    */
  void currentViewChanged(int currentView);

  /*!
    * @fn       prevViewChanged
    * @param    None
    * @return   None
    * @brief    responsible for changing prev view.
    */
  void prevViewChanged();

  /*!
    * @fn       modelChanged
    * @param    QMLEnums::ModelEnum sModelName
    * @param    QVariantMap - model data
    * @return   None
    * @brief    sends signal to update Models.
    */
  void modelChanged(QMLEnums::ModelEnum  sModelName , QVariantMap oModel);

  /*!
    * @fn       commandButtonEvent
    * @param    QMLEnums::ModelEnum modelname: Index of Model for which data neds to be returned.
    * @param    QString data
    * @return   void
    * @brief    responsible for changing models.
    */
  void commandButtonEvent(QMLEnums::ModelEnum modelname, QString data);

  /*!
  * @fn      getSupervisorAgentsData
  * @param   QString
  * @return  None
  * @brief   This function is responsible for getting agents API
  */
  void getSupervisorAgentsData(QString sAPI);

  /*!
  * @fn      updateChartsCordinates
  * @param   QAbstractSeries
  * @return  None
  * @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
  */
  void updateChartsCordinates(QAbstractSeries *series,int modelnum);

  /*!
    * @fn       refreshUi
    * @param    None
    * @return   None
    * @brief    responsible for refreshing UI.
    */
  void refreshUi();

  /*!
    * @fn       authstatuschanged
    * @param    int - errorcode
    * @param    QString - status
    * @param    int - modelno
    * @return   None
    * @brief    signal will call on authentication status changed
    */
  void authstatuschanged(int hr, QString status, int modelno);

  /*!
    * @fn       sslErrorReceived
    * @param    QString scannerstate
    * @return   None
    * @brief    siganl emits when scanner fault changes.
    */
  void sslerrorreceived(int hr, QString status,int modelno);

  /*!
    * @fn       requestBagPickupsig
    * @param    const QString id_assigning_authority
    * @param    const QString primary bag  ID
    * @param    const QString rfid
    * @param    const QString pickup_type
    * @param    QMLEnums::BhsDiverterDecision
    * @param    const QString diverter decision string
    * @return   None
    * @brief    Signal to notify back is ready for pick up.
    */
  void requestBagPickupsig(const QString id_assigning_authority, const QString &primary_bag_id,
                           const QString &rfid, const QString& pickup_type, QMLEnums::BhsDiverterDecision
                           diverter_decision_code, const QString &diverter_decision_string, const QString message, const QString userAction);

  /*!
    * @fn       exit();
    * @param    None
    * @return   None
    * @brief    Signal will call on close application
    */
  void exit();

  /*!
    * @fn       rawdataDownloadFinished;
    * @param    QString sDownloadPath
    * @param    QString sDownloadError
    * @return   None
    * @brief    Signal will call when rawdata Download finished
    */
  void rawdataDownloadFinished(QString sDownloadPath , QString sDownloadError);

  /*!
    * @fn       detectorManualfileDownloadFinish;
    * @param    QString sDownloadPath
    * @param    QString sDownloadError
    * @return   None
    * @brief    Signal will call when detector manual file download is finished.
    */
  void detectorManualfileDownloadFinish(QString sDownloadPath , QString sDownloadError);

  /*!
    * @fn       detectorMapfileDownloadFinish;
    * @param    QString sDownloadPath
    * @param    QString sDownloadError
    * @return   None
    * @brief    Signal will call when detector map file download is finished.
    */
  void detectorMapfileDownloadFinish(QString sDownloadPath , QString sDownloadError);

  /*!
     * @fn       exportlogfileDownloadFinish;
     * @param    QString sDownloadPath
     * @param    QString sDownloadError
     * @return   None
     * @brief    Signal will call when export log file download is finished.
     */
  void exportLogfileDownloadFinish(QString sDownloadPath , QString sDownloadError);

  /*!
      * @fn       exportlogfileDownloadFinish;
      * @param    QString sDownloadPath
      * @param    QString sDownloadError
      * @return   None
      * @brief    Signal will call when image quality reports file download is finished.
      */
  void imgQltyTestfileDownloadFinish(QString sDownloadpath, QString sDownloadError);

  /*!
      * @fn       reportDownloadFinished;
      * @param    QString sDownloadPath
      * @param    QString sDownloadError
      * @return   None
      * @brief    Signal will call when bags reports file download is finished.
      */
  void reportDownloadFinished(QString sDownloadpath, QString sDownloadError);

  /*!
       * @fn       errorlogdownloaded;
       * @param    QString sDownloadPath
       * @param    QString sDownloadError
       * @return   None
       * @brief    Signal will call when image error log file download is finished.
       */
  void errorlogdownloaded(QString sDownloadpath, QString sDownloadError);


  /*!
       * @fn       filedownloadfinished;
       * @param    QString sDownloadPath
       * @param    QString sDownloadError
       * @return   None
       * @brief    Signal will call when file download is finished.
       */
  void filedownloadfinished(QString sDownloadpath, QString sDownloadError, QMLEnums::ModelEnum modelnum);

  /*!
       * @fn       networkStatus
       * @param    QMLEnums::NetworkState status
       * @return   None
       * @brief    signal calls on notification about status of network from viewManager.
       */
  void networkStatus(QMLEnums::NetworkState status);

  /*!
       * @fn      setItemCompInModalFrame
       * @param   int modalComp
       * @param   QString errorMsgText=NULL
       * @param   int nextScreen
       * @return  None
       * @brief   Signal gets emit on call to set and then show Component
       *          in ModalFrame
       */
  void setItemCompInModalFrame(int modalComp, QString errorMsgText,
                               int nextScreen);

  /*!
       * @fn      setReliveUserScreen
       * @param   int modalComp
       * @param   QString errorMsgText=NULL
       * @param   int nextScreen
       * @return  None
       * @brief   Signal gets emit on call to load ReliveUserScreen in
       *          MainScreen
       */
  void setReliveUserScreen(int modalComp, QString errorMsgText,
                           int nextScreen);

  /*!
       * @fn       registScannerInDiscovered
       * @param    int
       * @return   None
       * @brief    This function is responsible to notify qml about registered
       *           scanner found in Discovered Scanner.
       */
  void registScannerInDiscovered(int ret);

  /*!
       * @fn       bagListCommand
       * @param    QMLEnums::ArchiveFilterSrchLoc srchLoc,
       * @param    QMLEnums::BagsListAndBagDataCommands command
       * @param    QVariant - data
       * @return   None
       * @brief    Generic signal for archive processing from Model to View.
       */
  void bagListCommand(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                      QMLEnums::BagsListAndBagDataCommands command,
                      QVariant data);

  /*!
       * @fn       bagListCommandResults
       * @param    QMLEnums::ArchiveFilterSrchLoc srchLoc
       * @param    QMLEnums::BagsListAndBagDataCommands command
       * @param    QVariant results
       * @return   None
       * @brief    Generic signal for archive processing from View to Model.
       */
  void bagListCommandResults(QMLEnums::ArchiveFilterSrchLoc srchLoc,
                             QMLEnums::BagsListAndBagDataCommands command,
                             QVariant results);

  /*!
       * @fn       notifyToChangeOrNotScreen
       * @param    bool toBeChanged
       * @param    QString erroMsg
       * @param    int hr
       * @return   None
       * @brief    notifies qml to change screen to show Search bag.
       */
  void notifyToChangeOrNotScreen(bool toBeChanged, QString erroMsg, int hr);

  /*!
       * @fn       parametersXMLReceived;
       * @param    QVariantList
       * @param    QMLEnums::ModelEnum
       * @return   None
       * @brief    Signal will call when parameter xml will be received and parsed.
       */
  void parametersXMLReceived(QVariantList data, QMLEnums::ModelEnum sModelName);

  /*!
   * @fn       chartsAverageData
   * @param    QVector
   * @return   None
   * @brief    to send average data to line chart qml.
   */
  void chartsAverageData( QVariantList averageList);
#ifdef RERUN
  /*!
       * @fn       rerunCommand
       * @param    QMLEnums::RERUN_COMMANDS
       * @param    RerunTaskConfig taskConfig
       * @return   None
       * @brief    notifies rerun command request from UI
       */
  void rerunCommand(QMLEnums::RERUN_COMMANDS cmd, RerunTaskConfig taskConfig);
#endif
  /*!
       * @fn       displayRerunStatus
       * @param    QString status
       * @param    QMLEnums::RERUN_RESPONSES typeRep
       * @return   None
       * @brief    send rerun status to the UI footer
       */
  void displayRerunStatus(QString status , QMLEnums::RERUN_RESPONSES typeRep);

  /*!
       * @fn       notifyViewSelectedBags
       * @param    QVariantList
       * @param    QModelIndex outputFilePathIndex
       * @return   None
       * @brief    notifies workstationManager when ViewBags is pressed.
       */
  void notifyViewSelectedBags(QVariantList, QModelIndex, QString opID = "");

  /*!
       * @fn       notifytoShowNextRerunBag
       * @param    None
       * @return   None
       * @brief    notifies workstationManager to show nextBag in Rerun
       */
  void notifytoShowNextRerunBag();

  /*!
       * @fn       notifyAlgoList
       * @param    QVariant
       * @return   None
       * @brief    signals to notify about algorithlist
       */
  void notifyAlgoList(QStringList list);

  /*!
       * @fn       notifyOperatorDecision
       * @param    QString bagId
       * @param    QMLEnums::ThreatEvalPanelCommand decision
       * @return   None
       * @brief    notifies workstationManager about operator decision
       */
  void notifyOperatorDecision(QString bagId , QMLEnums::ThreatEvalPanelCommand decision, float timeToTakeDecision,QDateTime renderStartTime);

  /*!
    * @fn       bagRetain
    * @param    None
    * @return   None
    * @brief    signal to retain bag
    */
  void bagRetain();

  /*!
    * @fn       notifyBagIsPickedUpsig
    * @param    QMLEnums::WSBasicCommandResult
    * @return   None
    * @brief    Signal To Notify Bag Is Picked Up.
    */
  Q_INVOKABLE void notifyBagIsPickedUpsig(QMLEnums::WSBasicCommandResult diverter_decision_code);

  /*!
   * @fn       setAuthenticatedUserDetailsToScanner
   * @param    None
   * @return   void
   * @brief    Signal to set Authenticated user details to scanner agent.
   */
  Q_INVOKABLE void setAuthenticatedUserDetailsToScanner();

  /*!
   * @fn       startScannerHeartBeat
   * @param    bool start
   * @return   void
   * @brief    Signal to start heart beat request.
   */
  void startScannerHeartBeat(bool start);

  /*!
   * @fn       operatorAbsentFromMat()
   * @param    None
   * @return   None
   * @brief    siganl emits when operator absent from Mat is detected
   */
  void operatorAbsentFromMat();

  /*!
   * @fn       getSelectedGroup()
   * @param    QString
   * @return   None
   * @brief    siganl emits when group is selected
   */
  void getSelectedGroup(QString selectedGroup);

  /*!
   * @fn       getSelectedGauge()
   * @param    QString
   * @return   None
   * @brief    signal emits when gauge is selected
   */
  void getSelectedGauge(int selectedGauge);

  /*!
   * @fn       processUpdateModels
   * @param    None
   * @return   None
   * @brief    process models.
   */
  void processUpdateModels();

public:


private:
  /*!
    * @fn       View();
    * @param    View&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
  View(const View& view);

  /*!
    * @fn       operator=
    * @param    View&
    * @return   View&
    * @brief    declaration for private assignment operator.
    */
  View& operator= (const View& view);

  /*!
    * @fn       isEqualVariantList
    * @param    const QVariantList prevList
    * @param    const QVariantList currList2
    * @return   bool
    * @brief    declaration for private assignment operator.
    */
  bool isEqualVariantList(const QVariantList prevList, const QVariantList currList2);

  /*!
    * @fn       registerDataTypesToQml
    * @param    None
    * @return   None
    * @brief    Register CPP Datatypes to qml.
    */
  void registerDataTypesToQml();

  bool                                        m_ScannerFaultScrnShown;        //!< Is scannerfault screen shown
  bool                                        m_OperatorThreatScrnShown;      //!< Is operator threat screen shown
  bool                                        m_NavigationMenuScrnShown;      //!< Is Navigation Menu screen shown
  bool                                        m_balloonPopupShown;             //!< Is ballon popup shown
  int                                         m_ePrevView;                    //!< Prev view of screen
  int                                         m_sCurentView;                  //!< Current view of screen
  int                                         m_sCurentModalFrame;            //!< Current component of Modal Frame
  int                                         m_durationofAniamtion;          //!< Animation duration
  QSize                                       m_rootWindow;                   //!< Contains size of rootWindow
  QSize                                       m_navigationPanel;              //!< Contains size of navigationPanel
  QSize                                       m_navigationMenuPanel;              //!< Contains size of navigationPanel
  QObject*                                    m_nwConnectionModel;            //!< Network Connection model
  QObject*                                    m_navigationModel;              //!< navigation Model
  QObject*                                    m_scannerFaultScrnModel;        //!< Scanner fault screen model
  QObject*                                    m_threatEvalPanelmodel;         //!< navigation model
  QObject*                                    m_NavigationPanelModel;         //!< navigation model
  QObject*                                    m_networkConnectionModel;
  QObject*                                    m_appContainerModel;            //!< App Container model
  QObject*                                    m_balloonPopupModel;            //!< Balloon Popup Model
  QWidget*                                    m_mainWindow;                   //!< Outermost widget
  QWidget*                                    m_centralWidget;                //!< Handle to central widget
  QWidget*                                    m_containerModalFrame;          //!< Container Widget for Modal Frame
  QWidget*                                    m_containerScannerFault;        //!< Handle to scanner fault container
  QWidget*                                    m_containerOperatorThreats;     //!< Handle to Operator Threats
  QWidget*                                    m_containerNavigationMenuScrn;  //!< Handle to Navigation Menu Screen
  QWidget*                                    m_qmlContainerWidget;           //!< Handle to Operator Threats
  QWidget*                                    m_containerBalloonPopup;        //!< Handle to Balloon Popup
  QQuickView*                                 m_qmlContainer;                 //!< Container Widget for QML
  QQuickView*                                 m_qmlModalFrameContainer;       //!< Container Widget for Modal Frame
  QQuickView*                                 m_qmlScannerFaultContainer;     //!< Container for Scanner Fault Status
  QQuickView*                                 m_qmlOperatorThreatOptions;     //!< Container for Operator Threat Options
  QQuickView*                                 m_qmlNavigationMenu;            //!< Container for Navigation Menu qml
  QQuickView*                                 m_qmlBalloonPopup;               //!< Container for ballon popup qml
  QFileDialog*                                m_fileDlg;                      //!< Handle For File Dialog
  QList<QObject*>                             m_ulmDataList;                  //!< Data object to be shared with QML
  IdleScreenDetector*                         m_hidleState;                   //!< handle to check idel screen
  VolumeDisplayManager*                       m_volDispMgr;                   //!< 3D image display view
  Utility*                                    m_utility;                      //!< Utility class
  WorkstationConfig*                          m_workstationConfig;            //!< WorkstationConfig class
  QParallelAnimationGroup*                    m_groupShow;                    //!< Group to show
  QParallelAnimationGroup*                    m_groupHide;                    //!< Group to hide
  QMLEnums::UserAdminScreenEnum               m_useradminview;                //!< Scanner admin screen
  QMLEnums::ScannerAdminScreenEnum            m_scanneradminview;             //!< Scanner admin screen
  QMLEnums::SupervisorScreenEnum              m_supervisorView;                   //!< DashBoard screen
  QMLEnums::WSAuthenticationResult            m_useradminloginStatus;         //!< maintains user login status
  QMLEnums::WSAuthenticationResult            m_reportadminloginStatus;       //!< maintains report login status
  QMLEnums::WSAuthenticationResult            m_rerunloginStatus;             //!< maintains user login status
  QMLEnums::WSAuthenticationResult            m_trainingloginstatus;          //!< maintains user login status
  QMLEnums::WSAuthenticationResult            m_networkloginStatus;           //!< maintains network connection status
  QMLEnums::WSAuthenticationResult            m_scannerloginStatus;           //!< maintains scanner status
  QMLEnums::WSAuthenticationResult            m_tipconfigadminloginStatus;    //!< maintains tip login status
  QMLEnums::WSAuthenticationResult            m_tiplibconfigadminloginStatus;  //!< maintains tip lib login status
  QMLEnums::WSAuthenticationResult            m_antivirusserviceloginStatus;  //!< maintains antivirus service login status
  QMLEnums::WSAuthenticationResult            m_supervisorLoginStatus;
  QMap<QMLEnums::ModelEnum, QVariantList>     m_allUiDataModels;              //!< data per model
  QPrintDialog                                *m_printdlg;                    //!< print dialog
  UsbStorageSrv                               m_usb;                          //!< Handle For Usb Storage
  static View                                 *m_view;                        //!< singleton object
  QString                                     currentOperatorName;            //!< Current operator name
  FileSystemItemModel                         *m_fileSystemItemModel;         //!< Holds Instance of FileSystemItemModel
  DashBoardScreenModel                        *m_dashBoardScreenModel;        //!< Holds Instance of DashBoardModel
  SupervisorAgentsDataModel                   *m_agentsDataModel;             //!< Holds Instance of agentsDataModel
  SupervisorEventsDataModel                   *m_recentEventsDataModel;       //!< Holds Instance of recentEventsDataModel
  GroupDataModel                              *m_groupDataModel;              //!< Holds Instance of GroupOverviewDataModel
  SupervisorConfigDataModel                   *m_supervisorConfigGroupModel; //!< Holds Instance of supervisorConfigurationModel
  SupervisorConfigDataModel                   *m_supervisorConfigMemberModel;
  SupervisorConfigDataModel                   *m_supervisorConfigNonMemberModel;
#ifdef WEBENGINE
  QWebEngineUrlRequestInterceptor*            m_requestInterceptor;           //!< Interceptor for updating request
  bool                                        m_isWebengineInitialized;       //!< web engine initialized
  QString                                     m_authToken;                    //!< Auth token
#endif
  QWidget*                                    m_VRTopwidget;                  //!< VR Top Widget
#ifdef RERUN
  RerunBatchManager*                          m_rerunBatchManager;          //!< Rerun batch manager model
  RerunFormatConverter*                       m_rerunFormatConverter;        //!< Rerun volume image to Raw image converter class
  RerunConfig*                                m_rerunConfigObj;
  QString                                     reportTypeSelected;
#endif
#ifdef WORKSTATION
  TipFileSystemModel*                         m_pTipFileSystemUIModel;        //!< Tip file system UI model
  TipFileSystemModel*                         m_pTipFileSystemOrginalModel;   //!< Tip file system original model
#endif
  LinuxTerminal*                              m_linuxTerminal;                //!< Linux terminal manager
  QString                                     m_userID;
  QString                                     m_RecentEventInfoReceive;

};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_VIEW_H_

