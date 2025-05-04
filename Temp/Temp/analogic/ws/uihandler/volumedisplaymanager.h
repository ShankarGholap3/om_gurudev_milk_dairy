/*!
* @file     volumedisplaymanager.h
* @author   Agiliad
* @brief    Class for managing the contents and visibility of the volumes.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAYMANAGER_H_
#define ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAYMANAGER_H_

#include <QPointF>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>
class QTimer;
class QQuickView;
class QMutex;

namespace analogic
{
namespace ws
{

class View;
class VolumeDisplay;

/*!
 * \class   VolumeDisplayManager
 * \brief   This class contains variable and function related to
 *          managing the contents and visibility of the volumes.
 */
class VolumeDisplayManager :public QObject
{
  Q_OBJECT

public:
  /*!
    * @fn       VolumeDisplayManager()
    * @param    View *parent
    * @return   None
    * @brief    constructor.
    */
  explicit VolumeDisplayManager(View *parent);

  /*!
    * @fn       VolumeDisplayManager()
    * @param    View *parent
    * @param    VolumeDisplay* ptr
    * @return   None
    * @brief    Constructor used only for mocking.
    */
  explicit VolumeDisplayManager(View *parent,
                                VolumeDisplay* ptr);

  /*!
    * @fn       ~VolumeDisplayManager()
    * @param    None
    * @return   None
    * @brief    destructor.
    */
  ~VolumeDisplayManager();

  /*!
    * @fn       initialize
    * @param    QQuickView *qmlContainer
    * @param    QWidget *centralWidget
    * @return   None
    * @brief    initialize.
    */
  void initialize(QQuickView *qmlContainer, QWidget *centralWidget);

  /*!
    * @fn       screenChanged();
    * @param    QMLEnums::ScreenEnum prevScreen
    * @param    QMLEnums::ScreenEnum newScreen
    * @return   None
    * @brief    Handling of change in screen.
    */
  void screenChanged(QMLEnums::ScreenEnum prevScreen, QMLEnums::ScreenEnum newScreen);

  /*!
    * @fn       isFrontWidgetVolumeRendered();
    * @param    None
    * @return   bool
    * @brief    Tell if the front widget volume is rendered.
    */
  bool isFrontWidgetVolumeRendered();

  /*!
    * @fn       vtkScreenLoaded()
    * @param    None
    * @return   None
    * @brief    Called when QML screen is loaded. Shows vtkContainer widget.
    */
  Q_INVOKABLE void vtkScreenLoaded();

  /*!
    * @fn       vtkSettingsChanged()
    * @param    QMLEnums::ThreatEvalPanelCommand command
    * @return   None
    * @brief    Responsible for handling user events.
    */
  Q_INVOKABLE void vtkSettingsChanged(QMLEnums::ThreatEvalPanelCommand command);

  /*!
    * @fn       getTotalVisibleAlarmThreats
    * @param    None
    * @return   int
    * @brief    get total alarm threat.
    */
  Q_INVOKABLE int getTotalVisibleAlarmThreats();

  /*!
        * @fn       getBagRenderedtime
        * @param    None
        * @return   float
        * @brief    get time taken to take decision by operator for training simulator
        */
  Q_INVOKABLE QDateTime getBagRenderedtime();

  /*!
    * @fn       getTotalNonThreats
    * @param    None
    * @return   int
    * @brief    get total non alarm threat.
    */
  Q_INVOKABLE int getTotalNoAlarmThreats();

  /*!
    * @fn       getOperatorAlarmThreat
    * @param    None
    * @return   int
    * @brief    get number of operator alarm threats
    */
  Q_INVOKABLE int getOperatorAlarmThreat();

  /*!
    * @fn       isBagError
    * @param    None
    * @return   bool
    * @brief    tells if bag has error.
    */
  Q_INVOKABLE bool isBagError();

  /*!
    * @fn       flushBagDataPool
    * @param    None
    * @return   None
    * @brief    flush BagDataPool as network is either reconnecting or unregistered
    */
  Q_INVOKABLE void flushBagDataPool();

  /*!
    * @fn       getIsAutoclearEnabled
    * @param    None
    * @return   bool
    * @brief    check if auto clear enabled
    */
  Q_INVOKABLE bool getIsAutoclearEnabled();

  /*!
    * @fn       getIsThreatOnlyMode
    * @param    None
    * @return   bool
    * @brief    gets if threat only mode on threat volume rendered
    */
  Q_INVOKABLE bool getIsThreatOnlyMode();

  /*!
    * @fn       getEnableThreatColorization()
    * @param    None
    * @return   bool
    * @brief    get threat colorization value
    */
  Q_INVOKABLE bool getEnableThreatColorization();

  /*!
    * @fn       isCurrentThreatOperator
    * @param    None
    * @return   bool
    * @brief    return true if current threat is operator
    */
  Q_INVOKABLE bool isCurrentThreatOperator();

  /*!
    * @fn       isDetectionException
    * @param    None
    * @return   bool
    * @brief    tells if bag has detection exception.
    */
  Q_INVOKABLE bool isDetectionException();

#ifdef WORKSTATION

  /*!
    * @fn       SetObviousThreatFlag
    * @param    bool - flag
    * @return   None
    * @brief    set obvious threat flag
    */
  Q_INVOKABLE void setObviousThreatFlag(bool flag);
#endif

  /*!
    * @fn       isViewModeEnabled
    * @return   bool
    * @brief    return if view mode is enabled
    */
  Q_INVOKABLE bool isViewModeEnabled();

  /*!
    * @fn       isVRModeVisible
    * @return   bool
    * @brief    return if vrmode argument needed visible on UI
    */
  Q_INVOKABLE bool isVRModeVisible(VREnums::VRViewModeEnum vrmode);

  /*!
    * @fn       isNextThreatAvailable
    * @return   bool
    * @brief    return if next threat is available
    */
  Q_INVOKABLE bool isNextThreatAvailable();

  /*!
    * @fn       isCurrentSelectedEntityMachineThreat
    * @return   bool
    * @brief    return if selected entity is machine threat
    */
  Q_INVOKABLE  bool isCurrentSelectedEntityMachineThreat();

  /*!
    * @fn       getCurrentVRViewmode()
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    gets current VR view mode(threat, surface or laptop)
    */
  Q_INVOKABLE VREnums::VRViewModeEnum getCurrentVRViewmode() const;

  /*!
    * @fn       getCurrentSlabViewType()
    * @param    None
    * @return   VREnums::SlabViewType
    * @brief    gets current Slab Type
    */
  Q_INVOKABLE VREnums::SlabViewType getCurrentSlabViewType() const;


  /**
    * @fn       addKeystroke()
    * @param    keystroke String definition of the keystroke: 2 Character code
    * @param    alarm_type If pressed while viewing a threat, the type of threat. Otherwise, blank.
    * @brief    recorded keystroke for a bag.
    */
  Q_INVOKABLE void addKeystroke(QString keystroke, QString alarm_type);

  /*!
    * @fn       getBHSBagId()
    * @param    None
    * @return   string
    * @brief    Get BHS bag id
    */
  Q_INVOKABLE QString getBHSBagId();

  /*!
    * @fn       getSearchDefaultRightviewModeType
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    gets default mode type
    */
  Q_INVOKABLE VREnums::VRViewModeEnum getSearchDefaultRightviewModeType();

  /*!
    * @fn       getRecallDefaultRightviewModeType
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    gets default mode type
    */
  Q_INVOKABLE VREnums::VRViewModeEnum getRecallDefaultRightviewModeType();

  /**
     * @brief isTIPBag
     * @return bool
     * @brief Identify if its current bag is TIP bag
     */
  Q_INVOKABLE bool isTIPBag();

  /*!
    * @fn       showTipThreat
    * @param    QMLEnums::ThreatEvalPanelCommand command
    * @return   None
    * @brief    Show Tip Threat
    */
  Q_INVOKABLE void showTipThreat(QMLEnums::ThreatEvalPanelCommand command);

  /*!
    * @fn       validateUserDecisionForNonTIPBag
    * @param    QMLEnums::ThreatEvalPanelCommand command
    * @return   bool
    * @brief    validate user decision for Non TIP Bag
    */
  Q_INVOKABLE bool validateUserDecisionForNonTIPBag(QMLEnums::ThreatEvalPanelCommand command);

  /*!
    * @fn       canApplyClearDecisionOnCurrentThreat
    * @return   bool
    * @brief    return if clear decision applied on current threat or not
    */

  Q_INVOKABLE bool canApplyClearDecisionOnCurrentThreat();

  /*!
    * @fn       getSliceCountReceived
    * @return   int
    * @brief    return number of slice count received
    */

  Q_INVOKABLE int getSliceCountReceived();

  /*!
    * @fn       canApplyDecisionOnCurrentThreat
    * @return   bool
    * @brief    return if decision applied on current threat or not
    */
  Q_INVOKABLE bool canApplyDecisionOnCurrentThreat();

  /*!
    * @fn       canTakeSuspectDecisionOnBag
    * @return   bool
    * @brief    return true if decsion is taken on all threat
    */
  Q_INVOKABLE bool canTakeSuspectDecisionOnBag();

  /*!
    * @fn       canTakeClearDecisionOnBag
    * @return   bool
    * @brief    return true if any threat marked as suspect
    */
  Q_INVOKABLE bool canTakeClearDecisionOnBag();

  /*!
    * @fn       setBagDecisionTime
    * @return   None
    * @brief    sets Bag Decision Time.
    */
  Q_INVOKABLE void setBagDecisionTime();

  /*!
    * @fn       getOperatorDecisiontime
    * @param    None
    * @return   float
    * @brief    get time taken to take decision by operator for training simulator
    */
  Q_INVOKABLE float getOperatorDecisiontime();

  /*!
    * @fn       getCheckpointMinimumSlices
    * @param    None
    * @return   int
    * @brief    get number of minimum slices required for checkpoint scanner
    */
  Q_INVOKABLE int getCheckpointMinimumSlices();

  /*!
    * @fn       setEnableDistanceMeasurement
    * @param    bool
    * @return   None
    * @brief    set distance Measurement mode.
    */
  Q_INVOKABLE void setEnableDistanceMeasurement(bool setVal);

  /*!
    * @fn       measureVolumeByValue
    * @param    bool
    * @return   None
    * @brief    Display volume estimation of picked object.
    */
  Q_INVOKABLE void measureVolumeByValue(bool setVal);

  /*!
    * @fn       setMaterialFilterCutoff
    * @param    double
    * @return   None
    * @brief    Sets matarial cutoff value.
    */
  Q_INVOKABLE void setMaterialFilterCutoff(double material_cutoff);

  /*!
       * @fn       isSlabViewThreatModeEnable
       * @param    None
       * @return   bool
       * @brief    return Slab View Threat mode enable/disable status.
       */
  Q_INVOKABLE bool isSlabViewThreatModeEnable();

public slots:
  /*!
    * @fn       volumeDisplayBackgroundProcessFinished()
    * @param    None
    * @return   None
    * @brief    Slot executes on main thread on completion of a background process. Update UI.
    */
  void volumeDisplayBackgroundProcessFinished();

  /*!
    * @fn       bagDataReceived
    * @param    boost::shared_ptr<BagData> bagdata
    * @param    bool status
    * @param    QString message
    * @return   None
    * @brief    This function will call on bag data received
    */
  void bagDataReceived(boost::shared_ptr<BagData> bagdata,
                       bool status, QString message);


private slots:
  /*!
    * @fn       pollForBagsTimerSlot()
    * @param    None
    * @return   None
    * @brief    Gets triggered at fixed time interval.
    */
  void pollForBagsTimerSlot();


  /*!
    * @fn       autoClearBag()
    * @param    None
    * @return   None
    * @brief    Auto clear the bag after set period as per user config.
    */
  void autoClearBag();

signals:
  /*!
    * @fn       bagDecision()
    * @param    None
    * @return   None
    * @brief    Signal emitted when user takes a decision on a bag.
    */
  void bagDecision();

  /*!
    * @fn       updateScreeningPanelEnability
    * @param    bool
    * @return   None
    * @brief    Enable or disable Screening Panel applicable to the displayed volume.
    */
  void updateScreeningPanelEnability(bool isEnabled);

  /*!
    * @fn       pickboxDrawn
    * @param    bool
    * @return   None
    * @brief    Signal to indicate Drawing of PickBox to indicate operator threat is done
    */
  void pickboxDrawn();

  /*!
    * @fn       bagRenderedComplete
    * @param    bool
    * @return   None
    * @brief    bag rendered complete
    */
  void bagRenderedComplete();

  /*!
    * @fn       showTIPResult
    * @param    QString message
    * @return   None
    * @brief    show TIP Result
    */
  void showTIPResult(QString message, QMLEnums::TIPResult result);


private:
  /*!
    * @fn       showBag
    * @param    None
    * @return   None
    * @brief    Make front widget visible if all conditions met.
    */
  void showBag();

  /*!
    * @fn       showNextBag
    * @param    None
    * @return   None
    * @brief    Clear front bag, bring back to front.
    */
  void showNextBag();

  /*!
    * @fn       applyDecision
    * @param    QMLEnums::ThreatEvalPanelCommand command
    * @return   None
    * @brief    Apply user decision
    */
  void applyDecision(QMLEnums::ThreatEvalPanelCommand decision);


  /*!
    * @fn       showNextRerunBag()
    * @param    None
    * @return   None
    * @brief    Apply user press of NextBag button.
    */
  void showNextRerunBag();

  /*!
    * @fn       stopShowingRerunBag()
    * @param    None
    * @return   None
    * @brief    Apply user press of StopBag button.
    */
  void stopShowingRerunBag();

  /*!
    * @fn       changeBagScrnToBagListScrn()
    * @param    None
    * @return   None
    * @brief    change Screen To Recall Archive Or Search screen
    */
  void changeBagScrnToBagListScrn();

  /*!
    * @fn       applyForward()
    * @param    None
    * @return   None
    * @brief    Apply user press of forward button to 3D image.
    */
  void applyForward();

  /*!
    * @fn       enableCurView()
    * @param    None
    * @return   None
    * @brief    Enable / Disable cut view
    */
  void enableCutView(bool status);

  bool            m_vtkScreenDisplayed;               //!< Flag indicating if widget is currently displayed
  qreal           m_displayAreaWidth;                 //!< Width of the volume area.
  qreal           m_displayAreaHeight;                //!< Height of the volume area.
  QMutex*         m_renderMutex;                      //!< Mutex to serialize BG rendering;
  QTimer*         m_AutoClearTimer;                   //!< Timer to clear bags automatically if no user action
  QTimer*         m_timerPollForBags;                 //!< Timer to poll for bags availability
  QPointF         m_topLeftCorner;                    //!< Left top corner coordinates of the volume area.
  QQuickView*     m_qmlContainerRef;                  //!< Container Widget for QML
  VolumeDisplay*  m_volumeDisplay;                    //!< Front bag which is visible.
  bool            m_isArchiveOrSearchScreenDisplayed; //!< Flag indicating if displayed volume is from archive/search.

};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAYMANAGER_H_
