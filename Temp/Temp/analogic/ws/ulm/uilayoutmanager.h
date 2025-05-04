/*!
* @file     uilayoutmanager.h
* @author   Agiliad
* @brief    This file contains functions related to UI layout management.
* @date     Sep, 29 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_ULM_UILAYOUTMANAGER_H_
#define ANALOGIC_WS_ULM_UILAYOUTMANAGER_H_

// Header Files
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QVariant>
#include <QVariantList>

#include <Logger.h>
#include <analogic/ws/common.h>
#include <analogic/ws/ulm/dataobject.h>
#include <analogic/ws/ulm/wsscreendao.h>
#include <analogic/ws/ulm/accessdefdao.h>
#include <analogic/nss/util/BasicPermissions.h>
#include <analogic/ws/ulm/xmlserializationhandler.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#ifdef WORKSTATION
#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>
#endif
#include <analogic/nss/agent/authentication/AuthenticationResult.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   UILayoutManager
 * \brief   This class contains variable and function related to
 *          UI layout management.ULM is responsible for dynamic updation of
 *          UI component.
 */
class WorkstationConfig;
class UILayoutManager :public QObject
{
    Q_OBJECT

public:

    /*!
    * @brief    qt property that notify on App state changes in qml.
    */
    Q_PROPERTY(  QMLEnums::AppState appstate READ getAppState WRITE onStateChanged NOTIFY updateState )
    /*!

    * @fn       initialize
    * @param    None
    * @return   None
    * @brief    Responsible for intialization of manager and handlers
    */
    void initialize();

    /*!
    * @fn       mapNssFeatureVSWorkstationFeature
    * @param    None
    * @return   None
    * @brief    map nss feature to workstation feature.
    */
    void mapNssFeatureVSWorkstationFeature();

    /*!
    * @fn       validateFeature
    * @param    UIComponentScreen element
    * @return   bool
    * @brief    validate feature
    */
    bool validateFeature(UIComponentScreen element);

    /*!
    * @fn       validateFeature
    * @param    QString objectname
    * @return   bool
    * @brief    validate feature by object name
    */
    Q_INVOKABLE bool validateFeature(QString objectName);


    /*!
    * @fn       getUILayoutManagerInstance
    * @param    None
    * @return   UILayoutManager*
    * @brief    get instance for UILayoutManager singletone
    */
    static UILayoutManager* getUILayoutManagerInstance();

    /*!
    * @fn       cleanupUILayoutManagerInstance
    * @param    None
    * @return   None
    * @brief    memory cleanup for the singleton class
    */
    static void cleanupUILayoutManagerInstance();

    /*!
    * @fn       getAppState
    * @param    None
    * @return   QMLEnums::AppState
    * @brief    This function gets application state
    */
    Q_INVOKABLE QMLEnums::AppState getAppState();

    /*!
    * @fn       getComponentVisibility
    * @param    QString - screenName
    * @return   QVariant
    * @brief    invokable method from qml that send commands via rest APIs using model references.
    */
    Q_INVOKABLE QVariant getComponentVisibility(QString screenName);

    /*!
    * @fn       updateScreenState
    * @param    QMLEnums::ScreenEnum - screenState
    * @return   None
    * @brief    invokable method from qml that send command to update on screen change
    */
    Q_INVOKABLE void updateScreenState(QMLEnums::ScreenEnum screenState);

    /*!
    * @fn       getScreenState
    * @param    None
    * @return   QMLEnums::ScreenEnum - screenState
    * @brief    invokable method from qml to get screen state
    */
    Q_INVOKABLE QMLEnums::ScreenEnum getScreenState();

    /*!
    * @fn       updateStateOnClick
    * @param    QMLEnums::AppState - state
    * @return   None
    * @brief    invokable method from qml that send command to update on button click selectively
    */
    Q_INVOKABLE void updateStateOnClick(QMLEnums::AppState state);

    /*!
    * @fn       getWsScreenName
    * @param    None
    * @return   QString
    * @brief    invokable method from qml to get workstation name
    */
    Q_INVOKABLE QString getWsScreenName();

    /*!
     * @fn      setWorkStationConfig
     * @param   boost::shared_ptr<WorkstationConfig>  - wsConfig
     * @return  None
     * @brief   This function returns the workstaionConfig.
     */
    void setWorkStationConfig(boost::shared_ptr<WorkstationConfig> wsConfig);

    /*!
     * @fn      getWorkStationConfig
     * @param   None
     * @return  boost::shared_ptr<WorkstationConfig>
     * @brief   This function returns the workstaionConfig.
     */
    boost::shared_ptr<WorkstationConfig> getWorkStationConfig();

    /*!
     * @fn      getulmDatalist
     * @param   None
     * @return  QVariant
     * @brief   This function returns a AVariant of the UI component list , QVariant is compatible with qml models
     */
    QVariant getulmDatalist();

    /*!
     * @fn      getScreenCtrlQML
     * @param   None
     * @return  ScreenControlList
     * @brief   This function gets screen ctrl QML
     */
    ScreenControlList getScreenCtrlQML();

signals:
    /*!
    * @fn       userLoginStatusChanged();
    * @param    None
    * @return   None
    * @brief    sends signal to update login state.
    */
    void userLoginStatusChanged();

    /*!
    * @fn       modelChanged();
    * @param    None
    * @return   None
    * @brief    sends signal to update model.
    */
    void modelChanged();

    /*!
    * @fn       updateModelsig
    * @param    QList<QObject*> - qlist
    * @return   None
    * @brief    sends signal to update Models.
    */
    void updateModelsig(QList<QObject*> qlist);

    /*!
    * @fn       updateState
    * @param    QMLEnums::AppState
    * @return   None
    * @brief    sends signal to update application state.
    */
    void updateState(QMLEnums::AppState state);

    /*!
    * @fn       pickboxDrawn
    * @return   None
    * @brief    sends signal to indicate pickbox has been drawn
    */
    void pickboxDrawn();

    /*!
    * @fn       operatorThreatSelected
    * @return   None
    * @brief    sends signal to indicate operator threat is current active
    */
    void operatorThreatSelected(bool flag);
    /*!
     * @fn      displayMsg
     * @param   int - errorcode
     * @param   QString - status
     * @param   int - model
     * @return  void
     * @brief   signal for display msg
     */
    void displayMsg(int hr, QString status, int model);

public slots:
    /*!
    * @fn       onStateChanged
    * @param    QMLEnums::AppState
    * @return   None
    * @brief    calls on application state changed.
    */
    Q_INVOKABLE void onStateChanged(QMLEnums::AppState);

    /*!
    * @fn       onscreenStatusChanged
    * @param    QString - screenState
    * @return   None
    * @brief    calls on screen status changed.
    */
    void onscreenStatusChanged(QString screenState);

    /*!
    * @fn       onuserLoginStatusChanged
    * @param    analogic::nss::AuthenticationResult - authStatus
    * @return   None
    * @brief    calls on Login status changed.
    */
    void onuserLoginStatusChanged(analogic::nss::AuthenticationResult authStatus);

private:
    /*!
    * @fn       UILayoutManager
    * @param    QObject* parent
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    explicit UILayoutManager(QObject* parent = NULL);

    /*!
     * @fn      ~UILayoutManager
     * @param   None
     * @return  None
     * @brief   This function is destructor for class UILayoutManager who is
     *          responsible for release of any resources if required.
     */
    ~UILayoutManager();

    /*!
     * @fn      updateUILayoutConfig
     * @param   std::strind - screenName
     * @return  QList<QObject*>
     * @brief   This function creates the screeen control list as per the screen name and state provided by the view/qlm
     *          screen control list is built step by step from the ULM config maps and vector
     *
     */
    QList<QObject*> updateUILayoutConfig( std::string screenName);

    /*!
     * @fn      checkStateValidity
     * @param   QMLEnums::AppState - currentState
     * @param   QMLEnums::AppState - nextState
     * @return  bool
     * @brief   This function checks the validity of the new state as per the workstation state diagram before
     *          assignment of the state.
     */
    bool checkStateValidity(QMLEnums::AppState currentState , QMLEnums::AppState  nextState);

    // Class member variable
    QString                                     m_wsScreenName;            //!< Qstring for configured workstation screen
    static bool                                 m_ulmInstanceFlag;         //!< Instance flag to monitor life of instance
    QList<QObject*>                             m_ulmDataList;             //!< List of QML mappable UI object
    AccessElementDao                            m_accessElementdao;        //!< ULM dao for acccess level configs
    ScreenControlList                           m_screenCtrlListQML;       //!< Screen Control list to be sent to QML
    QMLEnums::AppState                          m_appState;                //!< State of the application
    QMLEnums::ScreenEnum                        m_screenName;              //!< Current Workstation type screen name
    QMLEnums::ScreenEnum                        m_screenState;             //!< Current screen state
    static UILayoutManager*                     m_uiLaoutManagerInstance;  //!< Static instance of ULM
    WorkStationUIElementDAO                     m_wsulmdao;                //!< ULM data access object for workstation screen
    boost::shared_ptr<XmlSeriliazationHandler>  m_xmlSerHandler;           //!< boost xml serialization helper
    analogic::nss::AuthenticationResult         m_nssauthResult;           //!< authentication result from nss
    std::map<QString, QString>                  m_featuremap;              //!< map qml component object name vs nss feature
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_ULM_UILAYOUTMANAGER_H_

