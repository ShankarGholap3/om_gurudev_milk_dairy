/*!
* @file     bagdata.h
* @author   Agiliad
* @brief    This file encapsulate data structures and bag data processing details.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_BAGDATA_BAGDATA_H_
#define ANALOGIC_WS_COMMON_BAGDATA_BAGDATA_H_

#include <QDir>
#include <QQueue>
#include <QMutex>
#include <stdio.h>
#include <QDateTime>
#include <QStringList>

#include <SDICOS/SdcsTypes.h>
#include <SDICOS/SdcsArray1D.h>
#include <SDICOS/SdcsTemplateCT.h>
#include <SDICOS/SdcsArray3DLarge.h>
#include <SDICOS/SdcsSopClassUID.h>

#ifdef WORKSTATION
#include <proto/Bhs.pb.h>
#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>
#endif

#ifdef RERUN
#include <analogic/ws/rerun/rerunconfig.h>
#endif

#include <analogic/ws/common.h>

#include <Logger.h>

#include <analogic/nss/common/NssBag.h>
#include <analogic/nss/agent/osr/OsrLiveBag.h>
#include <analogic/ws/common/bagdata/baginfo.h>
#include <analogic/nss/agent/search/SearchBag.h>
#include <analogic/ws/volume_renderer/VRUtils.h>
#include <analogic/ws/search/workstationsearchbag.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <utility.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

#define DEFAULT_WIDTH 512
#define DEFAULT_VALUE_VR_VIEW_MODE  VREnums::VIEW_AS_THREAT
#define DEFAULT_VALUE_CURRENT_THREAT -1
#define DEFAULT_VALUE_SHOW_ALL_THREAT false
#define DEFAULT_VALUE_RECALL_SHOW_ALL_THREAT true
#define DEFAULT_VALUE_SAVE_VOLUME_FILE false

/*!
 * \enum   TIPDecision
 * \brief  enum for TIP Decision.
 */
enum TIPDecision {
  TIP_NONE,
  TIP_HIT,
  TIP_MISS,
  TIP_NON_TIP_ALARM
};

/*!
 * \enum   TIPPhase
 * \brief  enum for TIP phase.
 */
enum TIPPhase {
  TIP_NONE_PHASE,
  TIP_INITIAL_DECISION_PHASE,
  TIP_ANALYSIS_PHASE
};

/*!
 * \class   SliceDataStruct
 * \brief   This structure holds the slice data and length.
 */
class SliceDataStruct
{
public:
  /*!
    * @fn       SliceDataStruct
    * @param    None
    * @return   None
    * @brief    constructor for SliceDataStruct
    */
  SliceDataStruct()
  {
    int width = WorkstationConfig::getInstance()->getBagDataResolution().width();
    int height = WorkstationConfig::getInstance()->getBagDataResolution().height();
    m_buffer = new unsigned char[height* width* 2];
  }
  /*!
    * @fn       ~SliceDataStruct
    * @param    None
    * @return   None
    * @brief    destructor for SliceDataStruct
    */
  ~SliceDataStruct()
  {
    delete[] m_buffer;
  }

  /*!
    * @fn       SliceDataStruct
    * @param    const SliceDataStruct& st
    * @return   None
    * @brief    copy constructor for SliceDataStruct
    */
  SliceDataStruct(const SliceDataStruct& st)
  {
    m_buffer = st.m_buffer;
    m_length = st.m_length;
  }

  /*!
    * @fn       buffer
    * @param    None
    * @return   None
    * @brief    get buffer pointer
    */
  unsigned char *buffer() const;

  /*!
    * @fn       setBuffer
    * @param    void *buffer
    * @return   None
    * @brief    set buffer pointer
    */
  void setBuffer(unsigned char *buffer);

  /*!
    * @fn       length
    * @param    None
    * @return   size_t
    * @brief    get length
    */
  size_t length() const;

  /*!
    * @fn       setLength
    * @param    const size_t &length
    * @return   None
    * @brief    set length
    */
  void setLength(const size_t &length);
  /*!
    * @fn       SliceNo
    * @param    None
    * @return   unsigned int
    * @brief    gets slice no
    */
  unsigned int SliceNo() const;
  /*!
    * @fn       setSliceNo
    * @param    unsigned int
    * @return   None
    * @brief    gets slice no
    */
  void setSliceNo(unsigned int SliceNo);

private:
  unsigned char*    m_buffer;         //!< buffer pointer
  size_t   m_length;         //!< length of buffer
  int      m_SliceNo;        //!< Slice no
};


/*!
 * \class   BagData
 * \brief   This class contains variable and function related to
 *          bag data.
 */
class BagData:public QObject
{
  Q_OBJECT
public:
  enum BagState
  {
    ARCHIVE_STATE = 0,
    INITIALISED = 1,
    CONSTRUCTION = 2,
    READYTORENDER = 4,
    RENDERING = 8,
    DISPOSED = 16,
    ERROR = 32,
    EMPTY = 64,
  };

  std::map<BagState,std::string> BagStateText
  {
    {ARCHIVE_STATE,"ARCHIVE_STATE"},
    {INITIALISED,"INITIALISED"},
    {CONSTRUCTION,"CONSTRUCTION"},
    {READYTORENDER,"READYTORENDER"},
    {RENDERING,"RENDERING"},
    {DISPOSED,"DISPOSED"},
    {ERROR,"ERROR"},
    {EMPTY,"EMPTY"}
  };


  /*!
    * @fn       BagData
    * @param    None
    * @return   None
    * @brief    Constructor for class BagData.
    */
  explicit BagData(int m_Columns = 630, int m_Rows = 430, int n_max_slices = 1000);

  /*!
    * @fn       ~BagData
    * @param    None
    * @return   None
    * @brief    Destructor for class BagData.
    */
  ~BagData();

  /*!
    * @fn       isBagEmpty
    * @param    None
    * @return   bool - Is the bag empty?
    * @brief    Tells if the bag is empty.
    */
  bool isBagEmpty();

  /*!
    * @fn       getCtModules
    * @param    None
    * @return   SDICOS::Array1D<SDICOS::CTModule>
    * @brief    get CT data
    */
  SDICOS::Array1D<SDICOS::CTModule> getCtModules();

  /*!
    * @fn       addCtModule
    * @param    SDICOS::CTModule *
    * @return   None
    * @brief    Add a CTModule.
    */
  void addCtModule(SDICOS::CTModule *ctmodule);

  /*!
    * @fn       getTdrModule
    * @param    None
    * @return   SDICOS::Array1D<SDICOS::TDRModule>
    * @brief    get TDR data
    */
  std::vector<SDICOS::TDRModule*> getTdrModules();

  /*!
    * @fn       addTdrModule
    * @param    SDICOS::TDRModule *value
    * @param    TdrGeneratorType
    * @return   SDICOS::TDRModule *
    * @brief    Add Tdr Module
    */
  SDICOS::TDRModule *addTdrModule(SDICOS::TDRModule *value);

  /*!
    * @fn       getBagid
    * @param    None
    * @return   std::string - bagid
    * @brief    get bag id
    */
  std::string getBagid() const;

  /*!
    * @fn       setBagid
    * @param    const std::string &bagid
    * @return   None
    * @brief    set bag id
    */
  void setBagid(const std::string &bagid);

  /*!
    * @fn       getvolumedata
    * @param    None
    * @return   const unsigned char* - volume buffer
    * @brief    gets volume data buffer
    */
  const unsigned char* getvolumedata() const;
  /*!
    * @fn       setVolumeSliceData
    * @param    void *buffer - contains slice data
    * @param    size_t length
    * @return   None
    * @brief    set volume slice data
    */
  void setVolumeSliceData(void* buffer, size_t length);

  /*!
    * @fn       getLivebag
    * @param    None
    * @return   analogic::nss::NssBag *
    * @brief    gets live bag data
    */
  analogic::nss::NssBag* getLivebag();
  /*!
    * @fn       setLivebag
    * @param    None
    * @return   analogic::nss::NssBag *
    * @param    set live bag
    */
  void setLivebag(analogic::nss::NssBag *livebag);

  /**
    * @fn       addKeystroke()
    * @param    keystroke String definition of the keystroke: 2 Character code
    * @param    alarm_type If pressed while viewing a threat, the type of threat. Otherwise, blank.
    * @brief    recorded keystroke for a bag.
    */
  void addKeystroke(QString keystroke, QString alarm_type);

  /*!
    * @fn       saveToFiles
    * @param    const std::string& folderPath
    * @param    const std::string& filenamePrefix
    * @return   Success or failure.
    * @param    Save bag data to files.
    */
  QString saveToFiles(const std::string& folderPath, const std::string&
                      filenamePrefix);

  /*!
  * @fn       saveUncroppedToFiles
  * @param    const std::string& folderPath
  * @param    const std::string& filenamePrefix
  * @return   Success or failure
  * @param    Save bag data to files.
  */
  QString saveUncroppedToFiles(const std::string& folderPath, const std::string&
                               filenamePrefix);

  /*!
    * @fn       constructFromFiles
    * @param    QString bagDataFilePath
    * @param    QString tdrFilePath
    * @return   bool success or failure
    * @brief    Construct BagData object from archived files.
    */
  bool constructFromFiles(QString bagDataFilePath, QString tdrFilePath);

  /*!
    * @fn       getState
    * @param    None
    * @return   BagState
    * @param    gets Bag state
    */
  BagState getState();

  /*!
    * @fn       setState
    * @param    const BagState & - state
    * @return   None
    * @param    Sets Bag state
    */
  void setState(const BagState &state);

  /*!
    * @fn       getData_size
    * @param    None
    * @return   size_t
    * @param    gets bag data size
    */
  size_t getData_size() const;

  /*!
    * @fn       setData_size
    * @param    const size_t & - data_size
    * @return   None
    * @param    Sets bag data size.
    */
  void setData_size(const size_t &data_size);

  /*!
    * @fn       setRowsColumns
    * @param    rows - number of rows in volume data.
    * @return   columns - number of columns in volume data.
    * @brief    Set number of rows and columns.
    */
  void setRowsColumns(int rows, int columns);

  /*!
    * @fn       applyDecision
    * @param    decision - suspect, clear, timeout
    * @return   bool - success
    * @brief    Send bag decision to NSS
    */
  bool applyDecision(QMLEnums::ThreatEvalPanelCommand decision);

  /*!
    * @fn       cropVolumeAccordingtoROI
    * @param    None
    * @return   None
    * @param    crop volume according to ROI
    */
  bool cropVolumeAccordingtoROI();

  /*!
    * @fn       setvolumeROIStartX
    * @param    float ROI startx
    * @return   None
    * @brief    sets volume ROI StartX
    */
  void setvolumeROIStartX(float startx);

  /*!
    * @fn       setvolumeROIStartY
    * @param    float ROI starty
    * @return   None
    * @brief    sets volume ROI starty
    */
  void setvolumeROIStartY(float starty);

  /*!
    * @fn       setvolumeROIStartZ
    * @param    float ROI endz
    * @return   None
    * @brief    sets volume ROI startz
    */
  void setvolumeROIStartZ(float startz);

  /*!
    * @fn       setvolumeROIEndX
    * @param    float ROI endx
    * @return   None
    * @brief    sets volume ROI EndX
    */
  void setvolumeROIEndX(float endx);

  /*!
    * @fn       setvolumeROIEndY
    * @param    float ROI endy
    * @return   None
    * @brief    sets volume ROI EndY
    */
  void setvolumeROIEndY(float endy);

  /*!
    * @fn       setvolumeROIEndZ
    * @param    float ROI endz
    * @return   None
    * @brief    sets volume ROI EndZ
    */
  void setvolumeROIEndZ(float endz);


  /*!
    * @fn       setvolumeWidth
    * @param    float width
    * @return   None
    * @brief    sets volume widht
    */
  void setvolumeWidth(float val);

  /*!
    * @fn       setvolumeHeight
    * @param    float height
    * @return   None
    * @brief    sets volume height
    */
  void setvolumeHeight(float val);

  /*!
    * @fn       getVolumeDimension
    * @param    None
    * @return   QVector3D - dimension
    * @brief    gets volume dimensions
    */
  QVector3D getVolumeDimension();

  /*!
    * @fn       setvolumeSliceCount
    * @param    float slice
    * @return   None
    * @brief    sets volume slice count
    */
  void setvolumeSliceCount(float val);

  /*!
    * @fn       setvolumeFormat
    * @param    QString - format
    * @return   None
    * @brief    sets volume slice format
    */
  void setvolumeFormat(QString val);

  /*!
    * @fn       getvolumeFormat
    * @param    None
    * @return   QString - format
    * @brief    gets volume slice format
    */
  QString getvolumeFormat();

  /*!
    * @fn       getvolumeStartROI
    * @param    None
    * @return   QVector3D - start ROI
    * @brief    gets volume ROI
    */
  QVector3D getvolumeStartROI();

  /*!
    * @fn       getvolumeEndROI
    * @param    None
    * @return   QVector3D - start ROI
    * @brief    gets volume End ROI
    */
  QVector3D getvolumeEndROI();

  /*!
    * @fn       getLocalArchivedPath
    * @param    None
    * @return   QString
    * @brief    gets Local Archived Path
    */
  QString getLocalArchivedPath();

  /*!
    * @fn       getBagFileName
    * @param    None
    * @return   string - bag file name pattern
    *           MachineID_YYYYMMDD_hhmmss_BagID
    * @brief    get bag file name pattern
    */
  std::string getBagFileName() const;
  /*!
    * @fn       setVolumeDimension
    * @param    QVector3D dimension
    * @return   void - set volume dimension
    * @brief    set volume dimension
    */
  void setVolumeDimension(QVector3D dimension);


  /*!
    * @fn       getTotalAvailableAlarmThreats
    * @param    None
    * @return   int
    * @brief    returns total available alarm threat.
    */
  int getTotalAvailableAlarmThreats();

  /*!
    * @fn       getTotalVisibleAlarmThreats
    * @param    None
    * @return   int
    * @brief    returns total visible alarm threat.
    */
  int getTotalVisibleAlarmThreats();

  /*!
    * @fn       getTotalNoAlarmThreats
    * @param    None
    * @return   int
    * @brief    returns total alarm threat.
    */
  int getTotalNoAlarmThreats();

  /*!
    * @fn       getTotalAvailableThreats
    * @param    None
    * @return   int
    * @brief    returns total available threat alarm and non alarm.
    */
  int getTotalAvailableThreats();


  /*!
    * @fn       getOperatorAlarmThreat
    * @param    None
    * @return   int
    * @brief    returns total operator alarm threat
    */
  int getOperatorAlarmThreat();

  /*!
    * @fn       refreshAttributes
    * @param    None
    * @return   None
    * @brief    refresh all bag related attributes
    */
  void refreshAttributes();

  /*!
    * @fn       isBagError
    * @param    None
    * @return   bool
    * @brief    Tells if bag has error.
    */
  bool isBagError();

  /*!
    * @fn       setError
    * @param    bool value
    * @return   void
    * @brief    sets bag as error prone
    */
  void setError(bool value);

  /*!
    * @fn       addErrorMsg
    * @param    QString - msg
    * @return   None
    * @brief    add Error Msg.
    */
  void addErrorMsg(QString msg);

  /*!
    * @fn       clearErrorMsg
    * @param    None
    * @return   None
    * @brief    clear Error Msg.
    */
  void clearErrorMsg();

  /*!
    * @fn       getErrorMsg
    * @param    None
    * @return   QString - msg
    * @brief    get Error Msg.
    */
  QString getErrorMsg();


  /*!
    * @fn       getSliceCountReceived
    * @param    None
    * @return   int
    * @brief    get slice count received.
    */
  int getSliceCountReceived();

  /*!
    * @fn       setBagReqLoc
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @return   void
    * @brief    sets bag req location.
    */
  void setBagReqLoc(QMLEnums::ArchiveFilterSrchLoc loc);

  /*!
    * @fn       getBagReqLoc
    * @param    void
    * @return   QMLEnums::ArchiveFilterSrchLoc
    * @brief    gets bag req location.
    */
  QMLEnums::ArchiveFilterSrchLoc  getBagReqLoc();

  /*!
    * @fn       setBHSBagId
    * @param    string - bhs bagid
    * @return   void
    * @brief    sets BHS bag id
    */
  void setBHSBagId(std::string id);

  /*!
    * @fn       getBHSBagId
    * @param    void
    * @return   string - bhs id
    * @brief    gets bhs bag id.
    */
  std::string  getBHSBagId();

  /*!
    * @fn       getSearchReason
    * @param    None
    * @return   std::string - search reason
    * @brief    get search reason
    */
  std::string getSearchReason() const;

  /*!
    * @fn       setSearchReason
    * @param    const std::string &searchReaso
    * @return   None
    * @brief    set search reason
    */
  void setSearchReason(const std::string &searchReason);


  /*!
    * @fn       enqueueSlice
    * @param    SliceDataStruct*
    * @return   void
    * @brief    enqueue slice to queue
    */
  void  enqueueSlice(SliceDataStruct* slice);


  /*!
    * @fn       dequeueSlice
    * @param    void
    * @return   SliceDataStruct*
    * @brief     dequeue slice from queue
    */
  SliceDataStruct* dequeueSlice();

  /*!
    * @fn       isSliceQueueEmpty
    * @param    void
    * @return   bool
    * @brief    return slice queue empty status.
    */
  bool isSliceQueueEmpty();


  /*!
    * @fn       getSliceProcessed
    * @param    None
    * @return   int
    * @brief    gets number slice processed
    */
  int getSliceProcessed() const;

  /*!
    * @fn       setSliceProcessed
    * @param    int sliceProcessed
    * @return   None
    * @brief    sets number slice processed
    */
  void setSliceProcessed(int sliceProcessed);

  /*!
    * @fn       setSliceCountReceived
    * @param    int sliceCountReceived
    * @return   None
    * @brief    sets number slice received
    */
  void setSliceCountReceived(int sliceCountReceived);

  /*!
   * @fn       getBagTypeInfo
   * @param    None
   * @return   QString
   * @brief    gets bag type info
   */
  QString getBagTypeInfo() const;

  /*!
    * @fn       setBagTypeInfo
    * @param    const QString &BagTypeInfo
    * @return   None
    * @brief    sets bag type info
    */
  void setBagTypeInfo(const QString &BagTypeInfo);

  /*!
    * @fn       setArchiveSaveMode
    * @param    bool mode
    * @return   None
    * @brief    sets Archive Save Mode
    */
  void setArchiveSaveMode(bool mode);
  /*!
    * @fn       getArchiveSaveMode
    * @param    none
    * @return   bool mode
    * @brief    get Archive Save Mode
    */
  bool getArchiveSaveMode();
#ifdef WORKSTATION
  /*!
    * @fn       setObviousThreatFlag
    * @param    const bool flag
    * @param    bool fromBagServer
    * @return   None
    * @brief    sets obvious threat flag
    */
  void setObviousThreatFlag(bool flag, bool fromBagServer = false);

#endif
  /*!
    * @fn       getObviousThreatFlag
    * @param    None
    * @return   bool
    * @brief    gets obvious threat flag
    */
  bool getObviousThreatFlag() const;


  /*!
   * @fn       setVolumeSliceData
   * @param    void *buffer - contains slice data
   * @param    size_t length
   * @param    int - slice count
   * @return   None
   * @brief    set volume slice data
   */
  void setVolumeSliceData(void *buffer, size_t length, int sliceCount);

  /*!
   * @fn       increamentSliceProcessed
   * @param    None
   * @return   None
   * @brief    increament number slice processed by 1
   */
  void increamentSliceProcessed();

  /*!
   * @fn       getOperatorThreat
   * @param    None
   * @return   bool
   * @param    Returns if bag has operator threat
   */
  bool getOperatorThreat();

  /*!
   * @fn       readSupplementaryTDR
   * @param    None
   * @return   None
   * @param    Read supplementary TDR information and add it in current TDR
   */
  void readSupplementaryTDR();


  /*!
   * @fn       addOperatorTDRModule
   * @param    None
   * @return   None
   * @param    Add Operator TDR module
   */
  SDICOS::TDRModule *addOperatorTDRModule();

  /*!
   * @fn       populateTDRWithError
   * @param    Nome
   * @return   SDICOS::TDRModule
   * @brief    This function populate TDR in case error while transfering bag.
   */
  SDICOS::TDRModule populateTDRWithError(void);


  /*!
   * @fn       getUserDecision
   * @param    None
   * @return   QString
   * @brief    Gets user decision.
   */
  QString getUserDecision() const;

  /*!
   * @fn       setUserDecision
   * @param    const QString &
   * @return   None
   * @brief    Sets user decision.
   */
  void setUserDecision(const QString &userDecision);

  /*!
   * @fn       populateErrorMsgFromTDR
   * @param    None
   * @return   None
   * @brief    Populate msg in bag data if there is any tdr error.
   */
  void populateErrorMsgFromTDR();

  /*!
   * @fn       getOperatorTdrModule
   * @param    None
   * @return   SDICOS::TDRModule*
   * @brief    returns operator tdr module
   */
  SDICOS::TDRModule* getOperatorTdrModule();
  // TODO(Agiliad) Revisit when separating rerun and training #ifdef WORKSTATION
  /*!
   * @fn       addPickBoxThreat
   * @param    QVector3D& start
   * @param    QVector3D& end
   * @return   int
   * @brief    Create new operator tdr module if doesnt exists already and add new threat for given threat bounds
   */
  int addPickBoxThreat(QVector3D& start, QVector3D& end,
                       QMLEnums::ThreatEvalPanelCommand eThreatType);
  // #endif
  /*!
   * @fn       removePickBoxThreat
   * @param    unsigned int - local 0 index basedd threat id inside operator tdr module
   * @return
   * @brief    remove pick box threat from threat id
   */
  int removePickBoxThreat(unsigned int threatid);


  /*!
   * @fn       getVolumeCtModule
   * @param    None
   * @return   SDICOS::CTModule*
   * @brief    get CT data
   */
  SDICOS::CTModule* getVolumeCtModule();


  /*!
   * @fn       setVolumeCtModule
   * @param    SDICOS::CTModule *
   * @return   None
   * @brief    set CT data
   */
  void setVolumeCtModule(SDICOS::CTModule * ct);

  /*!
   * @fn       isValid
   * @param    None
   * @return   bool
   * @brief    validated received bagdata
   */
  bool isValid();

  /*!
   * @fn       getOperatorName
   * @param    None
   * @return   Operator Name
   * @brief    get Operator Name.
   */
  QString getOperatorName() const;

  /*!
   * @fn       setOperatorName
   * @param    Operator Name
   * @return   None
   * @brief    set Operator Name.
   */
  void setOperatorName(const QString &operatorName);

  /*!
   * @fn       getCurrrentThreat
   * @param    None
   * @return   int
   * @brief    get Current Threat count
   */
  int getCurrrentThreat() const;

  /*!
   * @fn       setCurrentThreat
   * @param    int
   * @return   None
   * @brief    set Current Threat count
   */
  void setCurrentThreat(int currrentThreat);
  /*!
   * @fn       setNextThreat()
   * @param    None
   * @return   bool
   * @brief    Set Next threat to highlight.
   */
  bool setNextThreat();

  /*!
   * @fn       setCurrentVRViewmode()
   * @param    const VREnums::VRViewModeEnum
   * @return   None
   * @brief    sets current VR view mode(threat, surface or laptop)
   */
  void setCurrentVRViewmode(const VREnums::VRViewModeEnum &VRViewmode);

  /*!
   * @fn       getCurrentVRViewmode()
   * @param    None
   * @return   VREnums::VRViewModeEnum
   * @brief    gets current VR view mode(threat, surface or laptop)
   */
  VREnums::VRViewModeEnum getCurrentVRViewmode() const;

  /*!
   * @fn       setSlabViewType()
   * @param    const VREnums::SlabViewType
   * @return   None
   * @brief    sets current SlabViewType
   */
  void setCurrentSlabViewType(const VREnums::SlabViewType &SlabViewType);

  /*!
   * @fn       getCurrentSlabViewType()
   * @param    None
   * @return   VREnums::SlabViewType
   * @brief    gets current SlabViewType
   */
  VREnums::SlabViewType getCurrentSlabViewType() const;


  /*!
   * @fn       getCurrenttdrModule
   * @param    None
   * @return   None
   * @brief    gets current tdr module
   */
  SDICOS::TDRModule *getCurrenttdrModule() const;

  /*!
   * @fn       setCurrenttdrModule
   * @param    SDICOS::TDRModule *
   * @return   None
   * @brief    sets current tdr module
   */
  void setCurrenttdrModule(SDICOS::TDRModule *);

  /*!
   * @fn       isCurrentThreatOperator
   * @param    None
   * @return   bool
   * @brief    retrurn true if Current Threat operator
   */
  bool isCurrentThreatOperator();

  /*!
  * @fn       isCurrentThreatOperatorAddedInTrainig
  * @param    None
  * @return   bool
  * @brief    retrurn true if Current Threat is operator added after the bag rendered.
  *           the operator threat added in pvs screen is neglected
  *           it acts as machine threat for training simulator.
  */
  bool isCurrentThreatOperatorAddedInTrainig();

  /*!
   * @fn       isOperatorTdr
   * @param    SDICOS::TDRModule* tdrModule
   * @return   bool
   * @brief    retrurn true if tdr is operator
   */
  bool isOperatorTdr(SDICOS::TDRModule* tdrModule);

  /*!
  * @fn       isOperatorTdr
  * @param    SDICOS::TDRModule* tdrModule
  * @return   bool
  * @brief    retrurn true if tdr is operator added in tdr2
  *           the operator threat added in pvs screen is neglected
  *           it acts as machine threat for training simulator.
  */
  bool isOperatorTdrInTraining(SDICOS::TDRModule* tdrModule);


  /*!
   * @fn       copyBagFiles
   * @param    QString dest
   * @param    QString src
   * @param    QString bagFileNamePattern
   * @return   int
   * @brief    copy bagdata file to file
   */
  static int copyBagFiles(QString dest, QString src, QString bagFileNamePattern);

  /*!
   * @fn       removeBagFiles
   * @param    QString src
   * @param    QString bagFileNamePattern
   * @return   bool
   * @brief    remove bagdata file to file
   */
  static bool removeBagFiles(QString src, QString bagFileNamePattern);

  /*!
   * @fn       notifyReadyforPushTransfer
   * @param    None
   * @return   void
   * @brief    Notifying NSS VM for ready for push transfer
   */
  void notifyReadyforPushTransfer();

  /*!
   * @fn       getBagReadytoRenderTime
   * @param    None
   * @return   QDateTime
   * @brief    gets bag ready to render time
   */
  QDateTime getBagReadytoRenderTime() const;

  /*!
   * @fn       isViewModeEnabled
   * @return   bool
   * @brief    return if view mode is enabled
   */
  bool isViewModeEnabled();

  /*!
   * @fn       isCurrentSelectedEntityMachineThreat
   * @return   bool
   * @brief    return if selected entity is machine threat
   */
  bool isCurrentSelectedEntityMachineThreat();

  /*!
   * @fn       isNextThreatAvailable
   * @return   bool
   * @brief    return if next threat is available
   */
  bool isNextThreatAvailable();

  /*!
   * @fn       canApplyClearDecisionOnCurrentThreat
   * @return   bool
   * @brief    return if clear decision applied on current threat or not
   */
  bool canApplyClearDecisionOnCurrentThreat();

  /*!
   * @fn       canApplyDecisionOnCurrentThreat
   * @return   bool
   * @brief    return if decision applied on current threat or not
   */
  bool canApplyDecisionOnCurrentThreat();

  /*!
   * @fn       isVRModeVisible
   * @return   bool
   * @brief    return if vrmode argument needed visible on UI
   */
  bool isVRModeVisible(VREnums::VRViewModeEnum vrmode);

  /*!
   * @fn       getCurrentVisibleEntityCount
   * @param    None
   * @return   int
   * @brief    return total number of visible entity(Machine Threat/ Operator Threat/ Laptop)
   */
  int getCurrentVisibleEntityCount();

  /*!
   * @fn       getCurrentVisibleEntityIndex
   * @param    None
   * @return   int
   * @brief    Gets Currently visible entity index
   */
  int getCurrentVisibleEntityIndex();

  /*!
   * @fn       setCurrentVisibleEntityIndex
   * @param    int
   * @return   None
   * @brief    sets Currently visible entity index
   */
  void setCurrentVisibleEntityIndex(int index);

  /*!
   * @fn       decrementCurrentVisibleEntityIndex
   * @param    None
   * @return   None
   * @brief    decreament current visible entity index
   */
  void decrementCurrentVisibleEntityIndex();

  /*!
   * @fn       decrementCurrentVisibleEntityIndex
   * @param    None
   * @return   None
   * @brief    increament current visible entity index
   */
  void increamentCurrentVisibleEntityIndex();

  /*!
   * @fn       applyDecisionOnThreat
   * @param    None
   * @return   None
   * @brief    Apply Decision on threat.
   */
  void applyDecisionOnThreat();

  /*!
   * @fn       isTIPBag
   * @param    void
   * @return   bool
   * @brief    identify if its tip bag.
   */
  bool isTIPBag() const;

  /*!
   * @fn       setTIPBag
   * @param    bool
   * @return   void
   * @brief    set TIP Bag
   */
  void setTIPBag(const bool tipBag);

  /*!
   * @fn       addSliceDataToQueueForDecompression
   * @param    void* buffer
   * @param    size_t length
   * @return   None
   * @brief    Add slice data buffer to queue for de-compression
   *
   */
  void addSliceDataToQueueForDecompression(void *buffer, size_t length);

  /*!
   * @fn       validateOperatorDecisionForTIP
   * @param    decision - suspect, clear, timeout
   * @param    QMLEnums::TIPResult& tipResult
   * @return   QString - messsage
   * @brief    validate operator decision for TIP
   */
  QString validateOperatorDecisionForTIP(QMLEnums::ThreatEvalPanelCommand userDecision,
                                         QMLEnums::TIPResult &tipResult);

  /*!
   * @fn       updateTIPThreat()
   * @param    None
   * @return   bool
   * @brief    update TIP Threat
   */
  bool updateTIPThreat();

  /*!
   * @fn       getIsTIPPhase()
   * @param    None
   * @return   TIPPhase
   * @brief    gets TIP Phase
   */
  TIPPhase getTIPPhase() const;

  /*!
   * @fn       setIsTIPPhase
   * @param    const TIPPhase
   * @return   None
   * @brief    sets TIP Phase
   */
  void setTIPPhase(const TIPPhase &isTIPPhase);

  /*!
   * @fn       populateDecisionOnParticularThreat
   * @param    QMLEnums::ThreatEvalPanelCommand - bag decision
   * @brief    This function will populate bag decision on particular threat.
   */
  void populateDecisionOnParticularThreat(QMLEnums::ThreatEvalPanelCommand decision);

  /*!
   * @fn       getCurrentThreatUserDecision
   * @param    unsigned int threatIndex
   * @return   ASSESSMENT_FLAG: enumNoThreat, enumThreat, enumUnknown
   * @brief    This function will return user decision for particular threat on operator TDR.
   */
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG getCurrentThreatUserDecision(
      unsigned int threatIndex);

  /*!
   * @fn       getUserDecisionfromPrimaryTdr
   * @param    unsigned int threatIndex
   * @return   ASSESSMENT_FLAG: enumNoThreat, enumThreat, enumUnknown
   * @brief    This function will return user decision for particular threat on operator TDR.
   */
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG getUserDecisionfromPrimaryTdr(
      unsigned int threatIndex);

  /*!
   * @fn       getUserDecisionfromOperatorTdr
   * @param    unsigned int threatIndex
   * @return   ASSESSMENT_FLAG: enumNoThreat, enumThreat, enumUnknown
   * @brief    This function will return user decision for particular threat on operator TDR.
   */
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG getUserDecisionfromOperatorTdr(
      unsigned int threatIndex);

  /*!
   * @fn       getUserDecisionFromTdr
   * @param    SDICOS::TDRModule* tdr, unsigned int threatIndex
   * @return   ASSESSMENT_FLAG: enumNoThreat, enumThreat, enumUnknown
   * @brief    This function will return user decision for particular threat on given TDR.
   */
  SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG getUserDecisionFromTdr(
      SDICOS::TDRModule* tdr, unsigned int threatIndex);

  /*!
   * @fn       canTakeSuspectDecisionOnBag
   * @return   bool
   * @brief    return true if decsion is taken on all threat
   */
  bool canTakeSuspectDecisionOnBag();

  /*!
   * @fn       canTakeClearDecisionOnBag
   * @return   bool
   * @brief    return true if any threat marked as suspect
   */
  bool canTakeClearDecisionOnBag();

  /*!
   * @fn       initializeOperatorTDR
   * @param    none
   * @return   none
   * @brief    This function will initialize operator TDR with initial unknown decsion.
   */
  void initializeOperatorTDR();

  /*!
   * @fn       isValidVisibleEntity
   * @param    SDICOS::TDRModule *tdrModule
   * @param    unsigned int localThreatIndex
   * @param    VREnums::ThreatType& entityType
   * @return   boolean
   * @brief    This function will validate visible entity for given tdr
   */
  bool isValidVisibleEntity(SDICOS::TDRModule *tdrModule, unsigned int localThreatIndex,
                            VREnums::ThreatType& entityType);

  /*!
   * @fn       getTotalEffectiveAvailableAlarmThreats
   * @param    None
   * @return   int
   * @brief    returns total effective available alarm threat.
   */
  int getTotalEffectiveAvailableAlarmThreats();

  /*!
   * @fn       setBagDecisionTime
   * @param    None
   * @return   None
   * @brief    sets Bag Decision Time.
   */
  void setBagDecisionTime();

  /*!
   * @fn       getBagRenderTimeOnTraining
   * @param    None
   * @return   float
   * @brief    get bag render time on  training viewer screen
   */
  QDateTime getBagRenderTimeOnTraining();
  /*!
   * @fn       getOperatorDecisiontime
   * @param    None
   * @return   float
   * @brief    get time taken to take decision by operator for training simulator
   */
  float getOperatorDecisiontime();

  /*!
   * @fn       setBagRenderedTime
   * @param    QDateTime
   * @return   None
   * @brief    Sets bag rendered time.
   */
  void setBagRenderedTime(const QDateTime &bagRenderedTime);

  /*!
   * @fn       identifyDisplayableThreat
   * @return   std::vector< std::pair<int, std::string> >
   * @brief    gets displayable threat information
   */
  std::vector<std::pair<int, string> > identifyDisplayableThreat();

  /*!
   * @fn       isDetectionException
   * @return   bool
   * @brief    gets detection exception
   */
  bool isDetectionException();
  /*!
   * @fn       setDetectionException
   * @param   bool
   * @brief    sets detection exception
   */
  void setDetectionException(bool val);

  /*!
  * @fn       toggleShowAllThreat
  * @return   None
  * @brief    toggles show all threat flag.
  */
  void toggleShowAllThreat();

  /*!
     * @fn       setShowAllThreat
     * @param    bool
     * @return   None
     * @brief    Sets show all threat.
     */
  void setShowAllThreat(bool showAllThreat);

  /*!
     * @fn       getShowAllThreat
     * @param    None
     * @return   bool
     * @brief    returns show all threat.
     */
  bool getShowAllThreat() const;

  /*!
     * @fn       isSlabViewThreatModeEnable
     * @param    None
     * @return   bool
     * @brief    return Slab View Threat mode enable/disable status.
     */
  bool isSlabViewThreatModeEnable();

  /*!
     * @fn       getCurrentThreatAlgorithm
     * @param    None
     * @return   QString
     * @brief    return current bag threat detection algorithm.
     */
  QString getCurrentThreatAlgorithm();
  /*!
     * @fn       getRerunBagFilename
     * @param    None
     * @return   QString
     * @brief    return current bag file name for rerun.
     */
  QString getRerunBagFilename();


signals:

  /*!
    * @fn       bagFinished
    * @param    NssBag*
    * @return   None
    * @brief    This function sends bag finished signal to nss.
    */
  void bagFinished(analogic::nss::NssBag*);
  /*!
    * @fn       compressedSliceReceived
    * @param    None
    * @return   None
    * @brief    notifies the compressed slice data to compression thread
    *
    */
  void compressedSliceReceived();
#ifdef WORKSTATION

  /*!
    * @fn       sendBagRspToBHS
    * @param    const std::string& bhs_bag_id
    * @param    BagStatusUpdate decision
    * @return   None
    * @brief    This function sends bag finished signal to nss.
    */
  void sigSendBagRspToBHS(const std::string& bhs_bag_id,
                          const analogic::ancp00::BagStatusUpdate::BAG_DISPOSITION decision);

  /*!
    * @fn       sigSendObviousThreat
    * @param    const std::string& bhs_bag_id
    * @param    bool obviousThreat
    * @return   None
    * @brief    This function sends obvious threat decision
    */
  void sigSendObviousThreat(const std::string& bhs_bag_id, bool obviousThreat);

#endif

private:
  /*!
    * @fn       populateCommonTdrParts
    * @param    SDICOS::TDRModule* tdr
    * @param    QMLEnums::ThreatEvalPanelCommand decision
    * @param    SDICOS::CTModule* ctmodule
    * @return   bool
    * @brief    This function applies TDR to bag
    */
  bool populateCommonTdrParts(SDICOS::TDRModule* tdr,
                              QMLEnums::ThreatEvalPanelCommand decision,
                              SDICOS::CTModule* ctmodule);
  /*!
    * @fn       populateDecisionOnAllthreat
    * @param    SDICOS::TDRModule* tdr
    * @param    QMLEnums::ThreatEvalPanelCommand decision
    * @return   bool - result
    * @brief    This function will populate bag decision on all threats.
    */
  void populateDecisionOnAllthreat(SDICOS::TDRModule* tdr,
                                   QMLEnums::ThreatEvalPanelCommand decision);

  /*!
    * @fn       populateDecisionOnParticularThreat
    * @param    int - threat id
    * @param    ASSESSMENT_FLAG - enumNoThreat, enumThreat, enumUnknown
    * @return   None
    * @brief    This function will polulate particular threat on tdr with decision
    */
  void populateDecisionOnParticularThreat(unsigned int threatcount,
                                          SDICOS::TDRTypes::AssessmentSequence::AssessmentItem::ASSESSMENT_FLAG flag);



  /*!
    * @fn       populateThreatsOnTDR
    * @param    SDICOS::TDRModule& tdr
    * @param    SDICOS::CTModule *
    * @return   bool - result
    * @brief    This function will add all threat data to given operator TDR module.
    */
  bool populateThreatsOnTDR(SDICOS::TDRModule& tdr,
                            SDICOS::CTModule * ctmodule);

  /*!
  * @fn       isCtModuleProj00
  * @param    const SDICOS::CTModule *ct_module
  * @return   true or false
  * @brief    Check if CT Module is Proj 00 or not
  */
  bool isCtModuleProj00(const SDICOS::CTModule *ct_module);

  /*!
  * @fn       isCtModuleProj90
  * @param    const SDICOS::CTModule *ct_module
  * @return   true or false
  * @brief    Check if CT Module is Proj 90 or not
  */
  bool isCtModuleProj90(const SDICOS::CTModule *ct_module);



  bool                                 m_isTIPBag;            //!< Is it TIP Bag
  bool                                 m_detectionException;  //!< Detection exception flag
  bool                                 m_showAllThreat;       //!< Flag to display operator clear Alarm for recall bag
  TIPPhase                             m_TIPPhase;            //!< TIP Bag Phase
  int                                  m_volRows;             //!< no of rows of volume
  int                                  m_volColumns;          //!< no of columns of volume
  int                                  m_max_slices;          //!< max slices
  int                                  m_uncroppedVolColumns; //!< no of uncropped columns of volume
  int                                  m_uncroppedVolRows;    //!< no of uncropped rows of volume
  int                                  m_uncropped_max_slices; //!< uncropped max slices
  int                                  m_sliceCountReceived;  //!< Slice count received.
  unsigned char*                       m_volume = NULL;       //!< Volume string
  unsigned char*                       m_uncroppedvolume = NULL;     //!< temporary volume
  size_t                               m_data_size;           //!< data size
  size_t                               m_uncropped_data_size;  //!< data size
  QString                              m_localArchivePath;    //!< Local archive properties fileName
  QString                              m_BagTypeInfo;         //!< Bag type info
  BagInfo                              m_bagInfo;             //!< Info about bag
  QString                              m_volumeformat;        //!< bag volume format
  BagState                             m_state;               //!< Bag state enum
  QVector3D                            m_volumedimension;     //!< bag volume dimension
  QVector3D                            m_uncroppedVolDimension;     //!< bag volume dimension
  QVector3D                            m_roiStart;            //!< bag volume ROI start
  QVector3D                            m_roiEnd;              //!< bag volume ROI end
  std::string                          m_bagid;               //!< Bag Id
  analogic::nss::NssBag*               m_livebag;             //!< Live bag object
  std::vector<SDICOS::TDRModule*>      m_tdrmodule;           //!< vector of threat data report module
  std::vector<SDICOS::TDRModule*>      m_uncropped_tdrmodule; //!< vector of threat data report module
  SDICOS::Array1D<SDICOS::CTModule>    m_ctmodule;            //!< array of CT module objects
  SDICOS::CTModule                     *m_volumectmodule;     //!< array of CT module objects
  int                                  m_availablealarmcount;  //!< Total available alarm count represents number of
  //! machine threat + operator threat
  int                                  m_effectiveavailablealarmcount;    //!< Total effective available alarm count
  //! represent number of threats on
  //! which decision is not taken
  int                                  m_visiblealarmcount;   //!< Visible alarm count represents visible alarm entity
  //! based on view mode selected
  int                                  m_noAlarmcount;        //!< No alarm count represents number of laptop
  bool                                 m_isBagError;          //!< Check error occurred
  QString                              m_errorMsg;            //!< Bag Error Msg
  QMLEnums::ArchiveFilterSrchLoc       m_bagReqLoc;           //!< Bag req location
  std::string                          m_bhsBagId;            //!< Bag req BHS Id
  QQueue<SliceDataStruct*>*            m_sliceQueue;          //!< Slice queue
  int                                  m_sliceProcessed;      //!< slice processed
  std::string                          m_searchReason;        //!< Bag search reason
  QString                              m_userDecision;        //!< User decision from primary workstation
  bool                                 m_obviousThreat;       //!< Set obvious threat
  QDateTime                            m_bagRenderedTime;     //!< Bag rendered start time
  QDateTime                            m_bagReadytoRenderTime;     //!< Bag ready to render time
  QDateTime                            m_bagDecisionTime;     //! Bag Decision Time
  QDateTime                            m_bagDecisionTimeTraining;     //!< Bag start start time for Training
  QDateTime                            m_bagRenderedTimeTraining;     //!< Bag rendered start time for Training
  QMutex                               * m_slicequeueMutex;   //!< Bag slice queue mutex
  QString                              m_operatorName;        //!< Operator Name
  SDICOS::TDRModule*                   m_operatorTdr;         //!< Operator tdr
  int                                  m_currrentThreat;      //!< Current threat number
  int                                  m_visibleCurrentThreatIndex;  //!< Visible current threat index
  VREnums::VRViewModeEnum              m_VRViewmode;          //!< Right window view mode
  VREnums::SlabViewType                m_slabViewType;        //!< slab view type
  SDICOS::TDRModule*                   m_currenttdrModule;    //!< Current tdr module
  bool                                 m_archiveSaveMode;     //!< Flag to handle view/save functions on remote archive
  VREnums::ThreatType                  m_CurrentThreatType;   //!< current Threat Type
  SDICOS::TDRModule*                   m_primaryOperatorTdr;  //!< Operator tdr
  QString                              m_currentThreatAlgorithm; //!< rerun algorithm version
  QString                              m_rerunBagFilename;       //!< rerun bag file name
  int                                  m_numberOfThreatInTDR1; //!< number of threats present in tdr 1 total machine+laptop+operator threat (added at PVS)
  int                                  m_bagObjectCounter;          //!< Maintains the BagData object count, usually should not be more than MAX_BAG_ALLOWED
  QMutex                               m_bagStateMutex;        //!< Mutex for synchronizing bag sate changes.
};
}  // end of namespace ws
}  // end of namespace analogic

Q_DECLARE_METATYPE(boost::shared_ptr<analogic::ws::BagData>)
#endif  // ANALOGIC_WS_COMMON_BAGDATA_BAGDATA_H_
