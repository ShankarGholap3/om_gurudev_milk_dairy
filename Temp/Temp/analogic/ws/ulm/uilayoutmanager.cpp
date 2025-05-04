/*!
* @file     uilayoutmanager.cpp
* @author   Agiliad
* @brief    This file contains functions related to UI layout management.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/ulm/uilayoutmanager.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

bool UILayoutManager::m_ulmInstanceFlag = false;
UILayoutManager* UILayoutManager::m_uiLaoutManagerInstance = 0;
/*!
* @fn       UILayoutManager
* @param    QObject *parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
UILayoutManager::UILayoutManager(QObject *parent):QObject(parent)
{
    DEBUG_LOG("Initializing ulm data list.");
    m_wsScreenName = WorkstationConfig::getInstance()->getWorkstationName();
    DEBUG_LOG("Initialized members. Screenname to: "<< m_wsScreenName.toStdString());
    m_appState = QMLEnums::AppState::INITIAL;
    mapNssFeatureVSWorkstationFeature();
}



/*!
 * @fn      ~UILayoutManager
 * @param   None
 * @return  None
 * @brief   This function is destructor for class UILayoutManager who is
 *          responsible for release of any resources if required.
 */
UILayoutManager::~UILayoutManager()
{
    TRACE_LOG("");
    for ( int i = 0; i < m_ulmDataList.length(); i++ )
    {
        DataObject* obj=(DataObject*)m_ulmDataList.front();
        m_ulmDataList.pop_front();
        delete obj;
    }
    m_ulmDataList.clear();
    DEBUG_LOG("Finished destroying ulm data list.");
    m_xmlSerHandler.reset();

}

/*!
* @fn       getUILayoutManagerInstance
* @param    None
* @return   UILayoutManager*
* @brief    get instance for UILayoutManager singletone
*/
UILayoutManager* UILayoutManager::getUILayoutManagerInstance()
{
    if( m_ulmInstanceFlag == false )
    {
        DEBUG_LOG("Creating instance of UILayoutManager");
        m_uiLaoutManagerInstance = new UILayoutManager();
        m_ulmInstanceFlag = true;
        DEBUG_LOG("Created new instance of UILayoutManager" << m_uiLaoutManagerInstance);
        return m_uiLaoutManagerInstance;
    }
    else
    {
        DEBUG_LOG("Return existing instance of UILayoutManager" << m_uiLaoutManagerInstance);
        return m_uiLaoutManagerInstance;
    }
}

/*!
* @fn       cleanupUILayoutManagerInstance
* @param    None
* @return   None
* @brief    memory cleanup for the singleton class
*/
void UILayoutManager::cleanupUILayoutManagerInstance()
{
    if( m_ulmInstanceFlag == true && NULL != m_uiLaoutManagerInstance)
    {
        DEBUG_LOG("Deleting instance of UILayoutManager" << m_uiLaoutManagerInstance);
        delete m_uiLaoutManagerInstance;
        m_uiLaoutManagerInstance = NULL;
        m_ulmInstanceFlag = false;
    }
}

/*!
* @fn       initialize
* @param    None
* @return   None
* @brief    Responsible for intialization of manager and handlers
*/
void UILayoutManager::initialize()
{
    TRACE_LOG("");
    DEBUG_LOG("Creating instance of XmlSeriliazationHandler");
    m_xmlSerHandler.reset(new XmlSeriliazationHandler());
    INFO_LOG("Initialize ULM: using " << WS_SCREEN_CONFIG_FILE_NAME);
    m_wsulmdao = m_xmlSerHandler->loadWSdaoConfig
            (WorkstationConfig::getInstance()->getexecutablePath() + "/" + WS_SCREEN_CONFIG_FILE_NAME);
    INFO_LOG("Initialize ULM: using " << ACCESS_SCREEN_CONFIG_FILE_NAME);
    m_accessElementdao = m_xmlSerHandler->loadAccessElementConfig
            (WorkstationConfig::getInstance()->getexecutablePath() + "/" + ACCESS_SCREEN_CONFIG_FILE_NAME);
    INFO_LOG("ULM configuration loaded");
}

/*!
* @fn       mapNssFeatureVSWorkstationFeature
* @param    None
* @return   None
* @brief    map nss feature to workstation feature.
* */
void UILayoutManager::mapNssFeatureVSWorkstationFeature()
{
    TRACE_LOG("");
    if ((WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::OSR_WORKSTATION) ||
            (WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::SEARCH_WORKSTATION) ||
        (WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::SUPERVISOR_WORKSTATION))
    {
        m_featuremap[QString("btSettings")] = QString(analogic::nss::kBasicPermisssionAccessClientConfig);
        m_featuremap[QString("btRecallBag")] = QString(analogic::nss::kBasicPermisssionAccessRecall);
    }
    m_featuremap[QString("btReport")] = QString(analogic::nss::kBasicPermisssionAccessReports);
    m_featuremap[QString("btUserAdmin")] = QString(analogic::nss::kBasicPermisssionAccessUserAdmin);
    m_featuremap[QString("btScannerAdmin")] = QString(analogic::nss::kBasicPermisssionAccessImageQuality);
   // m_featuremap[QString("btRecallBag")] = QString(analogic::nss::kBasicPermisssionAccessRecall);      /* ANSS-1454 */
    m_featuremap[QString("btnRadiationSurvey")] = QString(analogic::nss::kBasicPermisssionAccessRadiationSurvey);
    m_featuremap[QString("btnImageQuality")] = QString(analogic::nss::kBasicPermisssionAccessImageQuality);
    m_featuremap[QString("btnErrorLog")] = QString(analogic::nss::kBasicPermisssionAccessScannerFault);
    m_featuremap[QString("btParameter")] = QString(analogic::nss::kBasicPermisssionAccessScannerParameter);
    m_featuremap[QString("btScannerStatus")] = QString(analogic::nss::kBasicPermisssionAccessScannerStatus);
    m_featuremap[QString("btnAccessOS")] = QString(analogic::nss::kBasicPermisssionAccessOS);
    m_featuremap[QString("saveButton")] = QString(analogic::nss::kBasicPermisssionSaveRecallBag);
    m_featuremap[QString("btuserdbimport")] = QString(analogic::nss::kBasicPermisssionImportUsers);
    m_featuremap[QString("btuserdbexport")] = QString(analogic::nss::kBasicPermisssionExportUsers);
    m_featuremap[QString("btkeyimport")] = QString(analogic::nss::kBasicPermisssionAccessCertificate);
    m_featuremap[QString("btkeyexport")] = QString(analogic::nss::kBasicPermisssionAccessCertificate);
    m_featuremap[QString("btkeygenerate")] = QString(analogic::nss::kBasicPermisssionAccessCertificate);
    m_featuremap[QString("btTIPView")] = QString(analogic::nss::kBasicPermissionAccessTipConfig);
    m_featuremap[QString("btTIPRecall")] = QString(analogic::nss::kBasicPermissionAccessTipRecall);
    m_featuremap[QString("btDetectionAlgorithm")]   = QString(analogic::nss::kBasicPermissionModifyActiveAlgorithm);
    m_featuremap[QString("modifyTimeoutMode")] = QString(analogic::nss::kBasicPermissionModifyTimeoutMode);
    m_featuremap[QString("modifyWsConfig")] = QString(analogic::nss::kBasicPermisssionModifyClientConfig);
    m_featuremap[QString("modifyDefaultAlgorithm")] = QString(analogic::nss::kBasicPermissionModifyDefaultAlgorithm);
    m_featuremap[QString("modifyScannerParameter")] = QString(analogic::nss::kBasicPermissionModifyScannerParameter);
    m_featuremap[QString("maintenance")] = QString(analogic::nss::kBasicPermissionAccessMaintenance);
    //TO DO: Change mapped hard code string to enum
    m_featuremap[QString("btNSSServices")] =  QString(analogic::nss::kBasicPermisssionAccessOS);
    m_featuremap[QString("btSupervisorScreen")] =  QString(analogic::nss::kBasicPermisssionAccessOS);

    if (WorkstationConfig::getInstance()->getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION){ //ANSS-1287 : Level3 should not access these buttons
      m_featuremap[QString("btReportSimulator")] = QString(analogic::nss::kBasicPermisssionAccessReports);
      m_featuremap[QString("btTrainingPlaylist")] = QString(analogic::nss::kBasicPermisssionAccessReports);
      m_featuremap[QString("btCategorize")] = QString(analogic::nss::kBasicPermisssionAccessReports);
      m_featuremap[QString("btSettings")] = QString(analogic::nss::kBasicPermisssionAccessReports);
    }


}

/*!
* @fn       validateFeature
* @param    UIComponentScreen element
* @return   bool
* @brief    validate feature
*/
bool UILayoutManager::validateFeature(UIComponentScreen element)
{
    if (m_featuremap.find(element.m_objectName.c_str()) == m_featuremap.end())
    {
        return element.m_isEnabled;
    }
    else if ( (element.m_objectName.compare("btReport") != 0)
              && (element.m_objectName.compare("btUserAdmin") != 0)
              && (element.m_objectName.compare("btScannerAdmin") != 0)
              && (element.m_objectName.compare("btSettings") != 0)
              && (element.m_objectName.compare("btRecallBag") != 0)
              && (element.m_objectName.compare("btnRadiationSurvey") != 0)
              && (element.m_objectName.compare("btnImageQuality") != 0)
              && (element.m_objectName.compare("btnErrorLog") != 0)
              && (element.m_objectName.compare("btParameter") != 0)
              && (element.m_objectName.compare("btScannerStatus") != 0)
              && (element.m_objectName.compare("btnAccessOS") != 0)
              && (element.m_objectName.compare("saveButton") != 0)
              && (element.m_objectName.compare("btuserdbimport") != 0)
              && (element.m_objectName.compare("btuserdbexport") != 0)
              && (element.m_objectName.compare("btkeyimport") != 0)
              && (element.m_objectName.compare("btkeyexport") != 0)
              && (element.m_objectName.compare("btkeygenerate") != 0)
              && (element.m_objectName.compare("btTIPView") != 0)
              && (element.m_objectName.compare("btTIPRecall") != 0)
              && (element.m_objectName.compare("maintenance") != 0)
              && (element.m_objectName.compare("btDetectionAlgorithm") != 0)
              && (element.m_objectName.compare("btNSSServices") != 0)
              && (element.m_objectName.compare("btReportSimulator") != 0)
              && (element.m_objectName.compare("btTrainingPlaylist") != 0)
              && (element.m_objectName.compare("btCategorize") != 0)
            //  && (element.m_objectName.compare("btSupervisorScreen") != 0)
            //btn is nt enabled if this condition exist as it  goes to below else and there "m_nssauthResult.credential.permissions" is updated from basicpermission file which is in 3PBLD
              )
    {
        return element.m_isEnabled;
    }
    else
    {
        if(m_nssauthResult.credential.permissions.find(m_featuremap[element.m_objectName.c_str()].toStdString().c_str()) !=
                m_nssauthResult.credential.permissions.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

/*!
* @fn       validateFeature
* @param    QString objectname
* @return   bool
* @brief    validate feature by object name
*/
bool UILayoutManager::validateFeature(QString objectName)
{
  if (m_featuremap.find(objectName) == m_featuremap.end())
  {
    return false;
  }
  else
  {
    if(objectName == "btScannerAdmin")
    {
      bool scannerAdminPermission = ((m_nssauthResult.credential.permissions.find(m_featuremap["btnRadiationSurvey"].toStdString().c_str()) != m_nssauthResult.credential.permissions.end())
          ||(m_nssauthResult.credential.permissions.find(m_featuremap["btnImageQuality"].toStdString().c_str()) != m_nssauthResult.credential.permissions.end())
          ||(m_nssauthResult.credential.permissions.find(m_featuremap["btnErrorLog"].toStdString().c_str()) != m_nssauthResult.credential.permissions.end())
          ||(m_nssauthResult.credential.permissions.find(m_featuremap["btParameter"].toStdString().c_str()) != m_nssauthResult.credential.permissions.end())
          ||(m_nssauthResult.credential.permissions.find(m_featuremap["btScannerStatus"].toStdString().c_str()) != m_nssauthResult.credential.permissions.end())
          ||(m_nssauthResult.credential.permissions.find(m_featuremap["btDetectionAlgorithm"].toStdString().c_str()) != m_nssauthResult.credential.permissions.end()));
      return scannerAdminPermission;
    }
    else if(objectName == "btSupervisorScreen")
    {
      return true;
    }
    else if (m_nssauthResult.credential.permissions.find(m_featuremap[objectName].toStdString().c_str()) !=
             m_nssauthResult.credential.permissions.end())
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}


/*!
* @fn       getAppState();
* @param    None
* @return   QMLEnums::AppState
* @brief    This function gets application state
*/
QMLEnums::AppState UILayoutManager::getAppState()
{
    DEBUG_LOG("AppState: "<<m_appState << ": " << QMLEnums::getAppstateString(m_appState));
    return m_appState;
}

/*!
 * @fn      updateUILayoutConfig
 * @param   std::strind - screenName
 * @return  QList<QObject*>
 * @brief   This function creates the screeen control list as per the screen name and state provided by the view/qlm
 *          screen control list is built step by step from the ULM config maps and vector
 *
 */
QList<QObject*> UILayoutManager::updateUILayoutConfig( std::string screenName){
    TRACE_LOG("");
    std::string screenState;
    screenState = QMLEnums::getAppstateString(m_appState);
    DEBUG_LOG("Update request for "<< screenName <<":"<< screenState);

    // local instance of screen element to hold filtered data

    ScreenElement scrEle;
    WorkstationType wsCurrent;

    for (workstationTypeVector::iterator it_wstype = m_wsulmdao.m_wsElement.begin();
         it_wstype != m_wsulmdao.m_wsElement.end() ; ++it_wstype)
    {
        if((*it_wstype).m_wsType.compare(m_wsScreenName.toStdString()) == 0)
        {
            wsCurrent = *it_wstype;
            DEBUG_LOG("Current workstation type : "<<wsCurrent.m_wsType.c_str());
        }
    }

    // get the screen state and select the control invocation for the state;
    for (workStationElement::iterator it_workStationElement = wsCurrent.m_workstation.begin();
         it_workStationElement != wsCurrent.m_workstation.end(); ++it_workStationElement)
    {
        if( (*it_workStationElement).m_screenName.compare(screenName) == 0 )
        {
            scrEle = *it_workStationElement;
            DEBUG_LOG("Current screen element : "<<scrEle.m_screenName.c_str());
            break;
        }
    }
    // for requested screen , get the component list for particular state.

    m_screenCtrlListQML.clear();
    m_screenCtrlListQML.shrink_to_fit();

    m_screenCtrlListQML = scrEle.m_screenControl.m_screenControlList;
    ScreenControlList scrCtrStateList;
    ScreenStateMap::iterator it_screenStateMap = scrEle.m_screenState.m_screenStateMap.find(screenState);
    if ( it_screenStateMap != scrEle.m_screenState.m_screenStateMap.end())
    {
        scrCtrStateList = (*it_screenStateMap).second;
    }

    for (ScreenControlList::iterator i = m_screenCtrlListQML.begin();
         i != m_screenCtrlListQML.end(); ++i)
    {
        for (ScreenControlList::iterator j = scrCtrStateList.begin();
             j != scrCtrStateList.end(); ++j)
        {
            if ((*i).m_objectName.compare((*j).m_objectName) == 0)
            {
                (*i).m_isEnabled = (*j).m_isEnabled;
                (*i).m_visibility = (*j).m_visibility;
                scrCtrStateList.erase(j);
                break;
            }
        }
    }

    m_screenCtrlListQML.insert( m_screenCtrlListQML.end(),
                                scrCtrStateList.begin(), scrCtrStateList.end());



    // generate the QML adaptable objetct from the Screen Control list
    // delete previous members
    for( int i = 0; i < m_ulmDataList.length(); i++ )
    {
        DataObject* obj=(DataObject*)m_ulmDataList.front();
        m_ulmDataList.pop_front();
        delete obj;
    }
    m_ulmDataList.clear();

    for ( ScreenControlList::iterator it_screenControlList = m_screenCtrlListQML.begin();
          it_screenControlList != m_screenCtrlListQML.end(); ++it_screenControlList)
    {
        // following is the QList od Qobject pointers which will be
        // converted to the QVariant object and shared with QML
        m_ulmDataList.append(new DataObject(QString::fromStdString((*it_screenControlList).m_objectName),
                                            (*it_screenControlList).m_visibility, validateFeature((*it_screenControlList))));
    }
    return m_ulmDataList;
}

/*!
* @fn       getComponentVisibility
* @param    QString - screenName
* @return   QVariant
* @brief    invokable method from qml that send commands via rest APIs using model references.
*/
QVariant UILayoutManager::getComponentVisibility( QString screenName)
{
    // get the UI component list based on screen name and state
    // TODO(Agiliad): apply the filter based on user level access
    DEBUG_LOG("screen state: " <<  m_appState
              << "  name: " << screenName.toStdString());
    return QVariant::fromValue(updateUILayoutConfig
                               (screenName.toStdString()));
}

/*!
* @fn       onStateChanged
* @param    QMLEnums::AppState
* @return   None
* @brief    calls on application state changed.
*/
void UILayoutManager::onStateChanged(QMLEnums::AppState state)
{
    DEBUG_LOG("Recieved state change signal : " << QMLEnums::getAppstateString(state)
              << " from : " << QObject::senderSignalIndex());
    if ( checkStateValidity( m_appState, state )    )
    {
        DEBUG_LOG("Updated state to : "
                  << QMLEnums::getAppstateString(state)
                  << "screen state : "
                  << m_screenState);
        m_appState = state;
        INFO_LOG("Updating App state to: "<< QMetaEnum::fromType<QMLEnums::ModelEnum>().valueToKey(m_appState));
        emit updateState(m_appState);
    }
    else
    {
        DEBUG_LOG("Invalid state change signal received :"
                  << QMLEnums::getAppstateString(state));
    }
}

/*!
* @fn       onscreenStatusChanged();
* @param    QString - screenState
* @return   None
* @brief    calls on screen status changed.
*/
void UILayoutManager::onscreenStatusChanged(QString screenState)
{
    DEBUG_LOG("Screen State: " << screenState.toStdString());
}

/*!
* @fn       onuserLoginStatusChanged();
* @param    QMLEnums::WSAuthenticationResult - authStatus
* @return   None
* @brief    calls on Login status changed.
*/
void UILayoutManager::onuserLoginStatusChanged(analogic::nss::AuthenticationResult
                                               authStatus)
{
    if ((getScreenState() == QMLEnums::RELIEVE_USER_LOGIN_PANEL_SCREEN) )
    {
        if(authStatus.result_type == analogic::nss::AuthenticationResult::SUCCESS)
        {
            m_nssauthResult = authStatus;
        }
    }
    else
    {
        m_nssauthResult = authStatus;
    }
}

/*!
 * @fn      checkStateValidity
 * @param   QMLEnums::AppState - currentState
 * @param   QMLEnums::AppState - nextState
 * @return  bool
 * @note    Curently state is validated by direct comparisons , if validations increases
 *          then need to implement state machine TODO
 * @brief   This function checks the validity of the new state as per the workstation state diagram before
 *          assignment of the state.
 */
bool UILayoutManager::checkStateValidity(QMLEnums::AppState currentState , QMLEnums::AppState  nextState)
{

    DEBUG_LOG("currentState: "<< currentState <<" nextState: "<< nextState);
    bool isValid = true;
    if ( currentState == nextState)
    {
        isValid = false;
    }
#ifdef WORKSTATION
    else if ((currentState != QMLEnums::AppState::SET_TO_OFFLINE &&
              currentState != QMLEnums::AppState::INPROGRESS ) ||
             (NSSAgentAccessInterface::getInstance()->getOsrReadyState() == true))
    {
        if( nextState == QMLEnums::AppState::OFFLINE )
            isValid = false;
    }
#endif
    else if ( currentState == QMLEnums::AppState::SET_TO_OFFLINE )
    {
        if ( nextState == QMLEnums::AppState::ONLINE )
            isValid = true;
    }
    else if ( currentState == QMLEnums::AppState::INPROGRESS )
    {
        if ( nextState == QMLEnums::AppState::SET_TO_OFFLINE )
            isValid = false;
    }
    else if ( currentState == QMLEnums::AppState::OFFLINE )
    {
        if ( nextState == QMLEnums::AppState::SET_TO_OFFLINE )
            isValid = false;
    }
    DEBUG_LOG("value of isValid flag: "<<isValid);
    return isValid;
}

/*!
 * @fn      getulmDatalist
 * @param   None
 * @return  QVariant
 * @brief   This function returns a AVariant of the UI component list , QVariant is compatible with qml models
 */
QVariant UILayoutManager::getulmDatalist()
{
    TRACE_LOG("");
    return QVariant::fromValue(m_ulmDataList);
}

/*!
 * @fn      getScreenCtrlQML
 * @param   None
 * @return  ScreenControlList
 * @brief   This function gets screen ctrl QML
 */
ScreenControlList UILayoutManager::getScreenCtrlQML()
{
    TRACE_LOG("");
    return m_screenCtrlListQML;
}

/*!
* @fn       getWsScreenName
* @param    None
* @return   QString
* @brief    invokable method from qml to get workstation name
*/
QString UILayoutManager::getWsScreenName()
{
    DEBUG_LOG("Workstation " << m_wsScreenName.toStdString());
    return m_wsScreenName;
}


/*!
* @fn       updateScreenState
* @param    QMLEnums::ScreenEnum - screenState
* @return   None
* @brief    invokable method from qml that send command to update on screen change
*/
void UILayoutManager::updateScreenState(QMLEnums::ScreenEnum screenState)
{
    DEBUG_LOG("Updating screen state to: "<<screenState);
    m_screenState = screenState;
}

/*!
* @fn       getScreenState
* @param    None
* @return   QMLEnums::ScreenEnum - screenState
* @brief    invokable method from qml to get screen state
*/
QMLEnums::ScreenEnum UILayoutManager::getScreenState()
{
    TRACE_LOG("");
    return m_screenState;
}

/*!
* @fn       updateStateOnClick
* @param   QMLEnums::AppState - state
* @return   None
* @brief    invokable method from qml that send command to update on button click selectively
*/
void UILayoutManager::updateStateOnClick(QMLEnums::AppState state)
{
    DEBUG_LOG("Update state on button click event to: "<< state);
    onStateChanged(state);
}
}  // end of namespace ws
}  // end of namespace analogic

