/*!
* @file     volumedisplay.h
* @author   Agiliad
* @brief    Class for displaying volume.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAY_H_
#define ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAY_H_

#include <analogic/ws/common.h>
#include <analogic/ws/ulm/dataobject.h>
#include <analogic/ws/ulm/wsscreendao.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>
#include <analogic/ws/ulm/uilayoutmanager.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/uihandler/view.h>
#include <analogic/ws/uihandler/volumedisplayuihelper.h>
#include <analogic/ws/volume_renderer/VolumeRendererWidget.h>

#include <QUrl>
#include <QMutex>
#include <QTimer>
#include <vector>
#include <QObject>
#include <QProcess>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QQmlContext>
#include <QGridLayout>
#include <QApplication>
#include <QVariantList>
#include <QDesktopWidget>
#include <QAbstractListModel>
#include <boost/shared_ptr.hpp>
#include <QQmlApplicationEngine>
#include <QSortFilterProxyModel>


#include <errors.h>
#include <Logger.h>
#include <frameworkcommon.h>

/**
 * Macro for MAX WIDGET COUNT
 */
#define MAX_WIDGET_COUNT 2

#define VTK_LAYOUT_MARGIN    2
#define VTK_LAYOUT_SPACING   0
#define VTK_WIDGET_HEIGHT  812

namespace analogic
{
namespace ws
{

class VolumeDisplayUIHelper;
/*!
 * \class   VolumeDisplay
 * \brief   This class contains variable and function related to
 *          displaying of volume.
 */
class VolumeDisplay : public QObject
{
  Q_OBJECT

public:
  /*!
    * @fn       VolumeDisplay()
    * @param    View* parent
    * @return   None
    * @brief    Constructor.
    */
  explicit VolumeDisplay(View* parent);

  /*!
    * @fn       VolumeDisplay()
    * @param    View* parent
    * @param    VolumeDisplayUIHelper* ptr
    * @return   None
    * @brief    Constructor only used for mocking.
    */
  explicit VolumeDisplay(View* parent,
                         VolumeDisplayUIHelper* ptr);

  /*!
    * @fn       ~VolumeDisplay()
    * @param    None
    * @return   None
    * @brief    Destructor.
    */
  ~VolumeDisplay();

  /*!
    * @fn       initialize()
    * @param    QQuickView *qmlContainer
    * @param    QWidget *topWidget
    * @param    QMutex* renderMutex
    * @return   None
    * @brief    Create the volume rendering widgets.
    */
  void initialize(QQuickView *qmlContainer, QWidget *topWidget, QMutex* renderMutex);

  /*!
    * @fn       renderVolume()
    * @param    boost::shared_ptr<BagData> bagData
    * @return   None
    * @brief    Render both left and right widgets with provided bag data.
    */
  void renderVolume(boost::shared_ptr<BagData>);

  /*!
       * @fn       getBagRenderedtime
       * @param    None
       * @return   QDateTime
       * @brief    get time when bag rendering start
       */
    QDateTime getBagRenderedtime();

  /*!
    * @fn       setGeometry()
    * @param    QPointF& topLeftCorner
    * @param    double totalWidth
    * @param    double totalHeight
    * @return   None
    * @brief    Set the position and size of container widgets.
    */
  void setGeometry(QPointF& topLeftCorner, double totalWidth, double totalHeight);

  /*!
    * @fn       isRendered()
    * @param    None
    * @return   bool
    * @brief    Tell if this volume is rendered.
    */
  bool isRendered();

  /*!
    * @fn       hasABag()
    * @param    None
    * @return   bool
    * @brief    Tell if this widget has taken a bag, and it's not yet disposed.
    */
  bool hasABag();

  /*!
    * @fn       show()
    * @param    None
    * @return   bool - Operation successful
    * @brief    Make widgets visible.
    */
  bool show();

  /*!
    * @fn       hide()
    * @param    None
    * @return   None
    * @brief    Set visibility of the widgets to false.
    */
  void hide();

  /*!
    * @fn       isVisible()
    * @param    None
    * @return   bool
    * @brief    Tell if this volume is currently visible.
    */
  bool isVisible();

  /*!
    * @fn       clearWidgets()
    * @param    None
    * @return   None
    * @brief    Clear the volume and hide widgets.
    */
  void clearWidgets();

  /*!
    * @fn       uiCommand()
    * @param    QMLEnums::ThreatEvalPanelCommand command
    * @return   None
    * @brief    Responsible for handling user events.
    */
  void uiCommand(QMLEnums::ThreatEvalPanelCommand command);

  /*!
    * @fn       applyDecision()
    * @param    QMLEnums::ThreatEvalPanelCommand
    * @return   None
    * @brief    Apply user decision on bag.
    */
  void applyDecision(QMLEnums::ThreatEvalPanelCommand decision);

  /*!
    * @fn       clearBag()
    * @param    None
    * @return   None
    * @brief    Clears the current viewed bag.
    */
  void clearBag();

  /*!
    * @fn       applyForward()
    * @param    None
    * @return   None
    * @brief    Apply user press of forward button to 3D image.
    */
  void applyForward();

  /*!
    * @fn       setFront()
    * @param    bool in
    * @return   None
    * @brief    Set value of front widget indicator.
    */
  void setFront(bool in);

  /*!
    * @fn       setArchive()
    * @param    bool in
    * @return   None
    * @brief    Set value of archive widget indicator.
    */
  void setArchive(bool in);

  /*!
    * @fn       isFront()
    * @param    None
    * @return   bool - Is this the front widget.
    * @brief    Tell if this is the front widget.
    */
  bool isFront();

  /*!
    * @fn       getViewID()
    * @param    None
    * @return   QString - View ID
    * @brief    Tell view ID
    */
  QString getViewID();

  /*!
    * @fn       setNextThreat()
    * @param    None
    * @return   None
    * @brief    Set Next threat to highlight.
    */
  void setNextThreat();

  /*!
    * @fn       updatePickboxButtonsForTimeout()
    * @param    None
    * @return   None
    * @brief    Update Pick box buttons for user timeout
    */
  void updatePickboxButtonsForTimeout();

  /*!
    * @fn       getDisplayAlarmThreat
    * @param    None
    * @return   bool
    * @brief    get display alarm threat
    */
  bool getDisplayAlarmThreat() const;

  /*!
    * @fn       getEnableThreatColorization()
    * @param    None
    * @return   bool
    * @brief    get threat colorization value
    */
  bool getEnableThreatColorization() const;

  /*!
    * @fn       isVRModeVisible
    * @return   bool
    * @brief    return if vrmode argument needed visible on UI
    */
  bool isVRModeVisible(VREnums::VRViewModeEnum vrmode);

  /*!
    * @fn       isNextThreatAvailable
    * @return   bool
    * @brief    return if next threat is available
    */
  bool isNextThreatAvailable();

  /*!
    * @fn       getSliceCountReceived
    * @return   int
    * @brief    return number of slice count received
    */
  int getSliceCountReceived();

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
    * @fn       canTakeSuspectDecisionOnBag
    * @return   bool
    * @brief    return true sif decsion is taken on all threat
    */
  bool canTakeSuspectDecisionOnBag();

  /*!
    * @fn       canTakeClearDecisionOnBag
    * @return   bool
    * @brief    return true if any threat marked as suspect
    */
  bool canTakeClearDecisionOnBag();

  /*!
    * @fn       isCurrentSelectedEntityMachineThreat
    * @return   bool
    * @brief    return if selected entity is machine threat
    */
  bool isCurrentSelectedEntityMachineThreat();

  /*!
    * @fn       getCurrentVRViewmode()
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    gets current VR view mode(threat, surface or laptop)
    */
  VREnums::VRViewModeEnum getCurrentVRViewmode() const;
  VREnums::SlabViewType getCurrentSlabViewType() const;
  /*!
    * @fn       isViewModeEnabled
    * @return   bool
    * @brief    return if view mode is enabled
    */
  bool isViewModeEnabled();

  /**
    * @fn       addKeystroke()
    * @param    keystroke String definition of the keystroke: 2 Character code
    * @param    alarm_type If pressed while viewing a threat, the type of threat. Otherwise, blank.
    * @brief    recorded keystroke for a bag.
    */
  void addKeystroke(QString keystroke, QString alarm_type);

  /*!
    * @fn       getBHSBagId()
    * @param    None
    * @return   string
    * @brief    Get BHS bag id
    */
  std::string getBHSBagId();

  /*!
    * @fn       getSearchDefaultRightviewModeType
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    gets default mode type
    */
  VREnums::VRViewModeEnum getSearchDefaultRightviewModeType();

  /*!
    * @fn       getRecallDefaultRightviewModeType
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    gets default mode type
    */
  VREnums::VRViewModeEnum getRecallDefaultRightviewModeType();

  /**
     * @brief isTIPBag
     * @return bool
     * @brief Identify if its current bag is TIP bag
     */
  bool isTIPBag();

  /*!
     * @fn       validateOperatorDecisionForTIP
     * @param    decision - suspect, clear, timeout
     * @param    QMLEnums::TIPResult& tipResult
     * @return   QString - message
     * @brief    validate operator decision for TIP
     */
  QString validateOperatorDecisionForTIP(QMLEnums::ThreatEvalPanelCommand userDecision, QMLEnums::TIPResult& tipResult);

  /*!
     * @fn       showTIPThreat
     * @param    decision - suspect, clear, timeout
     * @return   bool - success
     * @brief    show TIP Bag
     */
  bool showTIPThreat();

  /*!
     * @fn       applyDecisionOnThreat
     * @param    QMLEnums::ThreatEvalPanelCommand - bag decision
     * @brief    This function will populate bag decision on particular threats.
     */
  void applyDecisionOnThreat(QMLEnums::ThreatEvalPanelCommand decision);

  /*!
     * @fn       setBagDecisionTime
     * @return   None
     * @brief    sets Bag Decision Time.
     */
  void setBagDecisionTime();

  /*!
     * @fn       getOperatorDecisiontime
     * @param    None
     * @return   float
     * @brief    get time taken to take decision by operator for training simulator
     */
  float getOperatorDecisiontime();

  /*!
     * @fn       autoArchiveBag()
     * @param    None
     * @return   None
     * @brief    Archive the bag after set period as per user config.
     */
  void autoArchiveBag();

  /*!
     * @fn       isDetectionException
     * @return   bool
     * @brief    tells if detection exception for bag
     */
  bool isDetectionException();

  /*!
     * @fn       setEnableDistanceMeasurement
     * @param    bool
     * @return   None
     * @brief    set distance Measurement mode.
     */
  void setEnableDistanceMeasurement(bool setVal);

  /*!
     * @fn       measureVolumeByValue
     * @param    bool
     * @return   None
     * @brief    Display volume estimation of picked object.
     */
  void measureVolumeByValue(bool setVal);

  /*!
     * @fn       setMaterialFilterCutoff
     * @param    double
     * @return   None
     * @brief    Sets matarial cutoff value.
     */
  void setMaterialFilterCutoff(double material_cutoff);

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
   * @fn       updateLeftRightViewerForRecallBag
   * @param    bool
   * @return   None
   * @brief    updates left and right bag viewer according to show all threat status.
   */
  void updateLeftRightViewerForRecallBag(bool setDefaultRecallVrMode=false);

  /*!
     * @fn       isSlabViewThreatModeEnable
     * @param    None
     * @return   bool
     * @brief    return Slab View Threat mode enable/disable status.
     */
  bool isSlabViewThreatModeEnable();

signals:
  /*!
    * @fn       renderVolumeBackgroundThreadSignal()
    * @param    None
    * @return   None
    * @brief    Signal to trigger starting of rendering on background thread.
    */
  void renderVolumeBackgroundThreadSignal();

  /*!
    * @fn       uiCommandBackgroundThreadSignal()
    * @param    QMLEnums::ThreatEvalPanelCommand command
    * @return   None
    * @brief    Signal to trigger starting of UI command on background thread.
    */
  void uiCommandBackgroundThreadSignal(QMLEnums::ThreatEvalPanelCommand);

  /*!
    * @fn       backgroundProcessFinished()
    * @param    None
    * @return   None
    * @brief    Signal to indicate completion of a background thread process.
    */
  void backgroundProcessFinished();

  /*!
    * @fn       updateScreeningPanelEnability()
    * @param    bool
    * @return   None
    * @brief    Enable or disable Screening Panel applicable to the displayed volume.
    */
  void updateScreeningPanelEnability(bool isEnabled);

  /*!
    * @fn       bagRenderedComplete()
    * @param    bool
    * @return   None
    * @brief    Enable or disable Screening Panel applicable to the displayed volume.
    */
  void bagRenderedComplete();


public slots:

  /*!
    * @fn       vrwInitialized()
    * @param    None
    * @return   None
    * @brief    Slot that executes on completion of volume rendering.
    */
  void vrwInitialized();

  /*!
    * @fn       vrwLoadPercent()
    * @param    int percent
    * @return   None
    * @brief    Slot that executes periodically and informs rendering completion percentage.
    */
  void vrwLoadPercent(int percent);

  /*!
    * @fn       renderVolumeBackgroundThread()
    * @param    None
    * @return   None
    * @brief    Render on background thread.
    */
  void renderVolumeBackgroundThread();

  /*!
    * @fn       uiCommandBackgroundThread()
    * @param    QMLEnums::ThreatEvalPanelCommand
    * @return   None
    * @brief    Process a UI command on background thread.
    */
  void uiCommandBackgroundThread(QMLEnums::ThreatEvalPanelCommand);

  /*!
    * @fn       applyExportToDrive()
    * @param    None
    * @return   None
    * @brief    Handle user press of export button.
    */
  void applyExportToDrive();

  /*!
    * @fn       UserThreatBoxDrawn()
    * @param    None
    * @return   None
    * @brief    Fuction update UI after operator threat pick box has been drawn.
    */
  void UserThreatBoxDrawn();

  /*!
    * @fn       OperatorThreatIsActive()
    * @param    bool flag
    * @return   None
    * @brief    Update UI for current threat type as operator threat
    */
  void OperatorThreatIsActive(bool flag);

  /*!
    * @fn       MockMe()
    * @param    analogic::workstation::VolumeRendererConfig*& vrc
    * @param    analogic::workstation::VolumeRendererWidget*& vrw
    * @return   bool flag
    * @brief    Use this function only for mocking
    */
  void MockMe(analogic::workstation::VolumeRendererWidget* vLeft,
              analogic::workstation::VolumeRendererWidget* vRight,
              QMutex* renderMutex);

private:
  /*!
    * @fn       initializeVRW
    * @param    analogic::workstation::VolumeRendererConfig*& vrc
    * @param    analogic::workstation::VolumeRendererWidget*& vrw
    * @param    bool right
    * @return   None
    * @brief    initialize members of class View.
    */
  void initializeVRW(QGridLayout*& vtklayout,
                     analogic::workstation::VolumeRendererConfig*& vrc,
                     analogic::workstation::VolumeRendererWidget*& vrw,
                     bool right);

  /*!
    * @fn       renderVolumeSingle
    * @param    analogic::workstation::VolumeRendererConfig* vrc
    * @param    analogic::workstation::VolumeRendererWidget* vrw
    * @param    bool& isVolumePresentInThisWidget
    * @param    boost::shared_ptr<BagData>
    * @return   None
    * @brief    Render volume in the widget but don't change visibility.
    */
  void renderVolumeSingle(analogic::workstation::VolumeRendererConfig* vrc,
                          analogic::workstation::VolumeRendererWidget* vrw,
                          bool& isVolumePresentInThisWidget, boost::shared_ptr<BagData>);

  /*!
    * @fn       updateUIControlsVisibility()
    * @param    None
    * @return   None
    * @brief    Enable or disable UI controls applicable to the displayed volume.
    */
  void updateUIControlsVisibility();

  int                                             m_currrentThreat;
  int                                             widgetclearcount = 0;      //!< Handle to left and right widget.
  View*                                           m_viewRef;                 //!< Reference to View.
  bool                                            m_vrwLeftInitialized;      //!< Temp till clearVolume() API.
  bool                                            m_vrwRightInitialized;     //!< Temp till clearVolume() API.
  bool                                            m_rendered;                //!< Has volume or is empty.
  bool                                            m_renderingInProgress;     //!< Is rendering is in progress.
  bool                                            m_front;                   //!< Is this the front widget.
  bool                                            m_isArchive;               //!< Is this archived bag.
  bool                                            m_uiOperationInProgress;   //!< Is a UI operation is in progress.
  bool                                            m_hasABag;                 //!< Does this widget have a bag.
  QString                                         m_viewID;                  //!< To help with debugging
  QQuickView*                                     m_qmlContainerRef;         //!< Container Widget for QML.
  VolumeDisplayUIHelper*                          m_uiHelper;                //!< UI Helper for user operations.
  boost::shared_ptr<BagData>                      m_fbagData;                //!< Bag data handle.
  analogic::workstation::VolumeRendererWidget*    m_vrwLeft;                 //!< volume renderer interface.
  analogic::workstation::VolumeRendererWidget*    m_vrwRight;                //!< volume renderer interface.
  analogic::workstation::VolumeRendererConfig*    m_vrcLeft;                 //!< Volume renderer config object.
  analogic::workstation::VolumeRendererConfig*    m_vrcRight;                //!< Volume renderer config object.
  QGridLayout*                                    m_renderviewLayout;        //!< rendererview layout
  QWidget*                                        m_volumerenderTopWidget;       //!< volume renderer widget
};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAY_H_
