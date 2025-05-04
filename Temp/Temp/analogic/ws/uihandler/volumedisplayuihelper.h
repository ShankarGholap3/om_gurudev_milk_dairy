/*!
* @file     volumedisplayuihelper.h
* @author   Agiliad
* @brief    Class for performing UI operations on the displayed volume.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAYUIHELPER_H_
#define ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAYUIHELPER_H_


#include <QDir>
#include <QUrl>
#include <QTimer>
#include <QScreen>
#include <QObject>
#include <QProcess>
#include <QPrinter>
#include <QtGlobal>
#include <QDateTime>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QPrintDialog>
#include <QVariantList>
#include <QApplication>
#include <QDesktopWidget>
#include <QAbstractListModel>
#include <boost/shared_ptr.hpp>
#include <QQmlApplicationEngine>
#include <QSortFilterProxyModel>

#include <Logger.h>
#include <errors.h>
#include <frameworkcommon.h>
#include <analogic/ws/common.h>
#ifdef WORKSTATION
#include <analogic/ws/osr/osrconfig.h>
#endif
#include <analogic/ws/uihandler/view.h>
#include <analogic/ws/ulm/dataobject.h>
#include <analogic/ws/ulm/wsscreendao.h>
#include <analogic/ws/ulm/uilayoutmanager.h>
#include <analogic/ws/volume_renderer/VRUtils.h>
#include <analogic/ws/uihandler/volumedisplay.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>
#include <analogic/ws/uihandler/volumedisplaymanager.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/volume_renderer/VolumeRendererWidget.h>


namespace analogic {namespace workstation { class VolumeRendererWidget; }}

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{
#define DEFAULT_VALUE_ZOOM 1
#define DEFAULT_VALUE_INVERSE false
#define DEFAULT_VALUE_COLOR_METAL true
#define DEFAULT_VALUE_COLOR_ORGANIC true
#define DEFAULT_VALUE_COLOR_INORGANIC true
#define DEFAULT_VALUE_RENDER_WITH_COLOR false
#define DEFAULT_VALUE_RENDER_WITH_LOW_DENSITY false
#define DEFAULT_VALUE_CUT_PERCENTAGE 0.0
#define DEFAULT_VALUE_SLAB_PERCENTAGE 0.0
#define DEFAULT_VALUE_VARIABLE_DENSITY 0.0
#define DEFAULT_VALUE_AXIS VREnums::Y
#define DEFAULT_VALUE_SLABTYPE VREnums::BAG_VIEW
#define DEFAULT_VALUE_THREAT_REGION true
#define DEFAULT_VALUE_ENABLE_THREAT_COLORIZATION false

class VolumeDisplay;


/*!
 * \class   VolumeDisplayUIHelper
 * \brief   This class contains variable and function related to
 *          perform UI operations on the displayed volume.
 */

class VolumeDisplayUIHelper : public QObject
{
    Q_OBJECT

public:
    /*!
    * @fn       VolumeDisplayUIHelper()
    * @param    None
    * @return   None
    * @brief    Constructor.
    */
    VolumeDisplayUIHelper();

    /*!
    * @fn       ~VolumeDisplayUIHelper()
    * @param    None
    * @return   None
    * @brief    Destructor.
    */
    virtual ~VolumeDisplayUIHelper();

    /*!
    * @fn       initialize()
    * @param    QQuickView *qmlContainer
    * @param    QWidget *centralWidget
    * @param    VolumeDisplay* vd
    * @param    analogic::workstation::VolumeRendererWidget* left
    * @param    analogic::workstation::VolumeRendererWidget* right
    * @return   None
    * @brief    Initialize UI helper. Provide required references.
    */
    void initialize(QQuickView *qmlContainer,
                    QWidget *centralWidget,
                    VolumeDisplay* vd,
                    analogic::workstation::VolumeRendererWidget* left,
                    analogic::workstation::VolumeRendererWidget* right);

    /*!
    * @fn       uiCommand()
    * @param    QMLEnums::ThreatEvalPanelCommand command
    * @return   None
    * @brief    Responsible for handling user events.
    */
    void uiCommand(QMLEnums::ThreatEvalPanelCommand command);

    /*!
    * @fn       applyReload()
    * @param    None
    * @return   None
    * @brief    Apply user press of reload button to 3D image.
    */
    void applyReload();

    /*!
    * @fn       AddPickBoxThreat()
    * @param    QVector3D& start
    * @param    QVector3D& end
    * @param    QMLEnums::ThreatEvalPanelCommand eThreatType
    * @return   void
    * @brief    Create new tdr module and add it in bagdata for given threat bounds
    */
    void AddPickBoxThreat(QVector3D& start, QVector3D& end,
                            QMLEnums::ThreatEvalPanelCommand eThreatType);

    /*!
    * @fn       getDisplayAlarmThreat
    * @param    None
    * @return   bool
    * @brief    get display alarm threat
    */
    bool getDisplayAlarmThreat() const;

    /*!
    * @fn       pickboxAccept()
    * @param    QMLEnums::ThreatEvalPanelCommand command
    * @return   int
    * @brief    Accept changes to pickbox instrumentation graphics
    */
    int pickboxAccept(QMLEnums::ThreatEvalPanelCommand command);



    /*!
    * @fn       writeViewMode
    * @param    VREnums::VRViewModeEnum modeenum
    * @return   None
    * @brief    Write values of Right view mode
    */
    void writeViewMode(VREnums::VRViewModeEnum modeenum);

    /*!
    * @fn       setDefaultVRMode
    * @param    None
    * @return   None
    * @brief    Set default right view mode
    */
    void setDefaultVRMode();

    /*!
    * @fn       getDefaultVRMode
    * @param    None
    * @return   VREnums::VRViewModeEnum
    * @brief    Get default right view mode
    */
    VREnums::VRViewModeEnum getDefaultVRMode();

    /*!
    * @fn       SetRightViewerDisplayMode()
    * @param    None
    * @return   None
    * @brief    set right viewer display mode
    */
    void SetRightViewerDisplayMode();

    /*!
    * @fn       getEnableThreatColorization()
    * @param    None
    * @return   bool
    * @brief    get threat colorization value
    */
    bool getEnableThreatColorization() const;

public slots:

    /*!
    * @fn       onViewChanged
    * @param    int - mode
    * @return   None
    * @brief    Write values of view mode
    */
    void onViewChanged(VREnums::VRViewModeEnum aMode);

    /*!
    * @fn       onSlabPositionChanged
    * @param    double - slabOffset value
    * @return   None
    * @brief    Write slab percentage
    */
    void onSlabPositionChanged(double slabOffset);

private slots:
    /*!
    * @fn       screenshot()
    * @param    None
    * @return   None
    * @brief    Take screenshot.
    */
    void screenshot();

private:
    /*!
    * @fn       pickboxDelete
    * @param    None
    * @return   None
    * @brief    delete current pickbox
    */
    void pickboxDelete();

    /*!
    * @fn       readColorCheckboxOIM()
    * @param    None
    * @return   bool - Tell is anything has changed
    * @brief    Read values of organic, inorganic, metal checkboxes from UI.
    */
    bool readColorCheckboxOIM();

    /*!
    * @fn       writeColorCheckboxOIM()
    * @param    None
    * @return   None
    * @brief    Write values of organic, inorganic, metal checkboxes to UI.
    */
    void writeColorCheckboxOIM();

    /*!
    * @fn       writeQMLSliderValues()
    * @param    None
    * @return   None
    * @brief    Write values of sliders to UI.
    */
    void writeQMLSliderValues();

    /*!
    * @fn       applyColorCheckboxOIM()
    * @param    None
    * @return   None
    * @brief    Apply user changes in organic, inorganic, metal checkboxes to 3D image.
    */
    void applyColorCheckboxOIM();

    /*!
    * @fn       applySliderContrast()
    * @param    None
    * @return   None
    * @brief    Apply user changes in contrast slider to 3D image.
    */
    void applySliderContrast();

    /*!
    * @fn       applySliderBrightness()
    * @param    None
    * @return   None
    * @brief    Apply user changes in brightness slider to 3D image.
    */
    void applySliderBrightness();

    /*!
    * @fn       applySliderOpacity()
    * @param    None
    * @return   None
    * @brief    Apply user changes in opacity slider to 3D image.
    */
    void applySliderOpacity();

    /*!
    * @fn       applyZoomIn()
    * @param    None
    * @return   None
    * @brief    Apply user press of zoomin button to 3D image.
    */
    void applyZoomIn();

    /*!
    * @fn       applyZoomOut()
    * @param    None
    * @return   None
    * @brief    Apply user press of zoomout button to 3D image.
    */
    void applyZoomOut();


    /*!
    * @fn       applyPaintDrop()
    * @param    None
    * @return   None
    * @brief    Apply user press of paint drop to 3D image.
    */
    void applyPaintDrop();

    /*!
    * @fn       applyImageInversion()
    * @param    None
    * @return   None
    * @brief    Apply user press of image inversion button to 3D image.
    */
    void applyImageInversion();

    /*!
    * @fn       applyFrontalViewButtonsAndBlankScreen()
    * @param    None
    * @return   None
    * @brief    Apply user press of frontal view button to 3D image.
    */
    void applyFrontalViewButtonsAndBlankScreen();

    /*!
    * @fn       applyPrinter()
    * @param    None
    * @return   None
    * @brief    Handle user press of print button.
    */
    void applyPrinter();

    /*!
    * @fn       applyDisplayAlarmThreatonly()
    * @param    None
    * @return   None
    * @brief    Apply user press of display alarm threat only.
    */
    void applyDisplayAlarmThreatonly();

    /*!
    * @fn       applyToggleThreatColorization
    * @param    None
    * @return   None
    * @brief    Toggles threat colorization.
    */
    void applyToggleThreatColorization();

    /*!
    * @fn       applyCutViewPercentageChange()
    * @param    None
    * @return   None
    * @brief    Apply user changes in cut view slider to 3D image.
    */
    void applyCutViewPercentageChange();

    /*!
    * @fn       applyAxisChange()
    * @param    None
    * @return   None
    * @brief    Apply user changes in axis to volume
    */
    void applyAxisChange();

    /*!
    * @fn       writeCutViewAxis()
    * @param    None
    * @return   None
    * @brief    Write values of cut view axis
    */
    void writeCutViewAxis();

    /*!
    * @fn       writeCutViewPercentage()
    * @param    None
    * @return   None
    * @brief    Write values of cut view percentage
    */
    void writeCutViewPercentage();

    /*!
    * @fn       applySlabAxisChange()
    * @param    None
    * @return   None
    * @brief    Change value of slab axis
    */
    void applySlabAxisChange();

    /*!
    * @fn       applySlabPercentageChange()
    * @param    None
    * @return   None
    * @brief    Change value of slab percentage
    */
    void applySlabPercentageChange();

    /*!
    * @fn       applySlabTypeChange()
    * @param    None
    * @return   None
    * @brief    Change value of slab type
    */
    void applySlabTypeChange();

    /*!
    * @fn       writeSlabbingPercentage()
    * @param    None
    * @return   None
    * @brief    Write values of slab percentage
    */
    void writeSlabbingPercentage();

    /*!
    * @fn       writeSlabAxis()
    * @param    None
    * @return   None
    * @brief    Write values of cut view axis
    */
    void writeSlabAxis();

    /*!
    * @fn       writeSlabType()
    * @param    None
    * @return   None
    * @brief    Write values of slab type
    */
    void writeSlabType();

    /*!
    * @fn       writeVariableDensityPercentage()
    * @param    None
    * @return   None
    * @brief    Write values of variable density percentage
    */
    void writeVariableDensityPercentage();

    /*!
    * @fn       writeShowAllThreatStateChange
    * @param    None
    * @return   None
    * @brief    Write values of show all threat
    */
    void writeShowAllThreatStateChange();

    /*!
    * @fn       pickboxOn()
    * @param    None
    * @return   None
    * @brief    Start pickbox instrumentation graphics
    */
    void pickboxOn();


    /*!
    * @fn       pickboxCancel()
    * @param    None
    * @return   None
    * @brief    Cancel existing pick box if in draw mode.
    */
    void pickboxCancel();
    /*!
    * @fn       resetPresetButtons()
    * @param    None
    * @return   None
    * @brief    resetPresetButtons
    */
    void resetPresetButtons();
    /*!
    * @fn       presetColorOn()
    * @param    None
    * @return   None
    * @brief    Preset color on
    */
    void presetColorOn();

    /*!
    * @fn       presetMetalOnly()
    * @param    None
    * @return   None
    * @brief    Preset Metal only display graphics
    */
    void presetMetalOnly();

    /*!
    * @fn       presetOrganicOnly()
    * @param    None
    * @return   None
    * @brief    Preset Organic only display graphics
    */
    void presetOrganicOnly();

    /*!
    * @fn       presetInorganicOnly()
    * @param    None
    * @return   None
    * @brief    Preset Inorganic only display graphics
    */
    void presetInorganicOnly();

    /*!
    * @fn       presetLowdensityOnly()
    * @param    None
    * @return   None
    * @brief    Preset Low Density only display graphics
    */
    void presetLowdensityOnly();

    /*!
    * @fn       presetGreyScale()
    * @param    None
    * @return   None
    * @brief    Preset Grey scale graphics
    */
    void presetGreyScale();

    /*!
    * @fn       presetInverseImage()
    * @param    None
    * @return   None
    * @brief    Preset Inverse image
    */
    void presetInverseImage();

    /*!
    * @fn       presetShowRightImage()
    * @param    None
    * @return   None
    * @brief    Preset show right side image without transparency
    */
    void presetShowRightImage();

    /*!
    * @fn       presetAllMaterials()
    * @param    None
    * @return   None
    * @brief    Preset to all materials display graphics
    */
    void presetAllMaterials();

    /*!
    * @fn       resetMeasureBoxButtons
    * @param    None
    * @return   None
    * @brief    Resets all measurebox buttons.
    */
    void resetMeasureBoxButtons();



    bool                                           m_inverse;                //!< Normal or inverse state
    bool                                           m_currentColorMetal;      //!< Current state of metal color
    bool                                           m_currentColorOrganic;    //!< Current state of organic color
    bool                                           m_currentColorInorganic;  //!< Current state of inorganic color
    bool                                           m_displayAlarmThreat;     //!< display alarm threat only.
    bool                                           m_enableThreatColorization;//!< enable threat colorization.
    double                                         m_currentOpacity;         //!< Current state of opacity
    double                                         m_currentContrast;        //!< Current state of contrast
    double                                         m_currentCutPercentage;   //!< Cut view percentage
    double                                         m_currentZoomLevel;       //!< Current state of zoom level
    double                                         m_currentBrightness;      //!< Current state of brightness
    double                                         m_currentDensityPercentage;//!< Current variable density percentage
    QWidget*                                       m_parentWidgetRef;        //!< Outer container widget
    QPrinter*                                      m_printer;                //!< Object for printing screenshot
    QQuickView*                                    m_qmlContainerRef;        //!< Container Widget for QML
    VolumeDisplay*                                 m_vdRef;                  //!< VolumeDisplay whose helper this is
    analogic::workstation::VolumeRendererWidget*   m_vrwLeftRef;             //!< Volume renderer interface
    analogic::workstation::VolumeRendererWidget*   m_vrwRightRef;            //!< Volume renderer interface
    QPrintDialog                                   *m_printDialog;
    bool                                           m_currentRenderWithColor;  //! Render with color or not
    bool                                           m_currentWithLowDensity;   //! Render with low density or not
    VREnums::AXIS                                  m_axis;                    //!< axis for unpacking
    QString                                        m_operator_threat_type;    //!< operator threat type
    VREnums::SlabViewType                          m_slabtype;                //!< slabbing type: BAG_VIEW or THREAT_VIEW
    VREnums::AXIS                                  m_slabaxis;                //!< axis for slabbing
    double                                         m_currentSlabbingPercentage;//!< slabbing percentage
};
}   // namespace ws
}   // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_VOLUMEDISPLAYUIHELPER_H_
