/*!
* @file     modelmanagers.cpp
* @author   Agiliad
* @brief    This file contains functions related to ModelManager
*           which handles updating the data for current screen view.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#include <analogic/ws/uihandler/modelmanagers.h>
#include <analogic/ws/uihandler/modelmanagerscanneradmin.h>
#include <analogic/ws/uihandler/modelmanageruseradmin.h>
#include <analogic/ws/uihandler/modelmanagereportadmin.h>
#include <analogic/ws/uihandler/modelmanagertipconfigadmin.h>
#include <analogic/ws/uihandler/modelmanagertiplibconfigadmin.h>
#include <analogic/ws/uihandler/modelmanagerantivirus.h>
#include <analogic/ws/uihandler/modelmanagersupervisor.h>


namespace analogic
{
namespace ws
{
/*!
* @fn       ModelManagers
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class ModelManagers.
*/
ModelManagers::ModelManagers(QObject* parent): QObject(parent)
{
  TRACE_LOG("");
  m_ScannerAdminModelManager = NULL;
  m_userAdminModelManager = NULL;
  m_reportAdminModelManager = NULL;
  m_tipConfigAdminModelManager = NULL;
  m_tipLibConfigAdminModelManager = NULL;
  m_antiVirusModelManager = NULL;
  m_supervisorModelManager = nullptr;
}

/*!
* @fn       ~ModelManager
* @param    None
* @return   None
* @brief    Destructor for class ModelManager.
*/
ModelManagers::~ModelManagers()
{
  TRACE_LOG("");
  DEBUG_LOG("Destroying scanner admin modelmanager instance");
  SAFE_DELETE(m_ScannerAdminModelManager);
  DEBUG_LOG("Destroying user admin modelmanager instance");
  SAFE_DELETE(m_userAdminModelManager);
  DEBUG_LOG("Destroying report admin modelmanager instance");
  SAFE_DELETE(m_reportAdminModelManager);
  DEBUG_LOG("Destroying tip config admin modelmanager instance");
  SAFE_DELETE(m_tipConfigAdminModelManager);
  DEBUG_LOG("Destroying tip lib config admin modelmanager instance");
  SAFE_DELETE(m_tipLibConfigAdminModelManager);
  DEBUG_LOG("Destroying antivirus modelmanager instance");
  SAFE_DELETE(m_antiVirusModelManager);
  DEBUG_LOG("Destroying chart modelmanager instance");
  SAFE_DELETE(m_supervisorModelManager);
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManagers::init()
{
  TRACE_LOG("");
#ifdef WORKSTATION
  DEBUG_LOG("Creating instance of Model manager for scanner admin");
  m_ScannerAdminModelManager = new ModelManagerScannerAdmin();
  THROW_IF_FAILED((m_ScannerAdminModelManager == NULL) ? Errors::E_OUTOFMEMORY:Errors::S_OK);
#endif
  DEBUG_LOG("Creating instance of Model manager for User admin");
  m_userAdminModelManager = new ModelManagerUserAdmin();
  THROW_IF_FAILED((m_userAdminModelManager == NULL) ? Errors::E_OUTOFMEMORY:Errors::S_OK);
#ifdef WORKSTATION
  DEBUG_LOG("Creating instance of Model manager for Report admin");
  m_reportAdminModelManager = new ModelManagerReportAdmin();
  THROW_IF_FAILED((m_reportAdminModelManager == NULL) ? Errors::E_OUTOFMEMORY:Errors::S_OK);
  DEBUG_LOG("Creating instance of Model manager for TipObject Config admin");
  m_tipConfigAdminModelManager = new ModelManagerTipConfigAdmin();
  THROW_IF_FAILED((m_tipConfigAdminModelManager == NULL) ? Errors::E_OUTOFMEMORY:Errors::S_OK);
  DEBUG_LOG("Creating instance of Model manager for TipLib Config admin");
  m_tipLibConfigAdminModelManager = new ModelManagerTipLibConfigAdmin();
  THROW_IF_FAILED((m_tipLibConfigAdminModelManager == NULL) ? Errors::E_OUTOFMEMORY:Errors::S_OK);
  DEBUG_LOG("Creating instance of Model manager for Antivirus");
  m_antiVirusModelManager = new ModelManagerAntiVirus();
  THROW_IF_FAILED((m_antiVirusModelManager == NULL) ? Errors::E_OUTOFMEMORY:Errors::S_OK);
  m_supervisorModelManager  = new ModelManagerSupervisor();
  THROW_IF_FAILED((m_supervisorModelManager == nullptr) ? Errors::E_OUTOFMEMORY:Errors::S_OK);
#endif
  DEBUG_LOG("Initializing scanner admin, user admin and report admin model managers.");
#ifdef WORKSTATION
  m_ScannerAdminModelManager->init();
  m_reportAdminModelManager->init();
  m_tipConfigAdminModelManager->init();
  m_tipLibConfigAdminModelManager->init();
  m_antiVirusModelManager->init();
  m_supervisorModelManager->init();
#endif
  m_userAdminModelManager->init();
  Semaphore::getInstance()->release(1);
}

/*!
* @fn       onCommandButtonEvent
* @param    QMLEnums::ModelEnum  - modelname
* @param    QString data to be post
* @return   void
* @brief    This slot will call on command button clicked.
*/
void ModelManagers::onCommandButtonEvent(QMLEnums::ModelEnum modelname, QString data)
{
  DEBUG_LOG("Command button event for modelname: "<<modelname);
#ifdef WORKSTATION
  if(modelname < QMLEnums::ModelEnum::SCANNERADMIN_MODELS_END)
  {
    DEBUG_LOG("Command for scanner admin models.");
    m_ScannerAdminModelManager->onCommandButtonEvent(modelname, data);
  }
#endif
  if(modelname > QMLEnums::ModelEnum::SCANNERADMIN_MODELS_END &&
     modelname < QMLEnums::ModelEnum::USERADMIN_MODELS_END)
  {
    DEBUG_LOG("Command for user admin models.");
    m_userAdminModelManager->onCommandButtonEvent(modelname, data);
  }
#ifdef WORKSTATION
  if(modelname > QMLEnums::ModelEnum::USERADMIN_MODELS_END &&
     modelname < QMLEnums::ModelEnum::REPORT_ADMIN_MODELS_END)
  {
    DEBUG_LOG("Command for report admin models.");
    m_reportAdminModelManager->onCommandButtonEvent(modelname, data);
  }

  if(modelname > QMLEnums::ModelEnum::REPORT_ADMIN_MODELS_END &&
     modelname < QMLEnums::ModelEnum::TIP_CONFIG_ADMIN_MODELS_END)
  {
    DEBUG_LOG("Command for tip config admin models.");
    m_tipConfigAdminModelManager->onCommandButtonEvent(modelname, data);
  }
  if(modelname > QMLEnums::ModelEnum::TIP_CONFIG_ADMIN_MODELS_END &&
     modelname < QMLEnums::ModelEnum::TIP_LIB_CONFIG_ADMIN_MODELS_END)
  {
    DEBUG_LOG("Command for tip lib config admin models.");
    m_tipLibConfigAdminModelManager->onCommandButtonEvent(modelname, data);
  }
  if(modelname > QMLEnums::ModelEnum::USER_ACCESS_RESET_MODEL &&
     modelname < QMLEnums::ModelEnum::ANTIVIRUS_MODELS_END)
  {
    DEBUG_LOG("Command for tip config admin models.");
    m_antiVirusModelManager->onCommandButtonEvent(modelname, data);
  }
  if(modelname > QMLEnums::ModelEnum::ANTIVIRUS_MODELS_END &&
     modelname < QMLEnums::ModelEnum::SUPERVISOR_MODEL_END)
  {
    DEBUG_LOG("Command for chart config admin models.");
    m_supervisorModelManager->onCommandButtonEvent(modelname, data);
  }
#endif
}

/*!
* @fn      updateChartsCordinates
* @param   QAbstractSeries
* @return  None
* @brief   This function is responsible for updating the received supervisor webservice data to the lineseries(QAbstractSeries)
*/
void ModelManagers::onUpdateChartsCordinates(QAbstractSeries *series,int modelnum)
{
  m_supervisorModelManager->onUpdateChartsCordinates(series, modelnum);
}

/*!
* @fn      onGetSelectedGroup
* @param   QString
* @return  None
* @brief   This function is responsible for getting selected group
*/
void ModelManagers::onGetSelectedGroup(QString selectedGroup)
{
  INFO_LOG("selected group name in ModelManagers::onGetSelectedGroup " << selectedGroup.toStdString());
  m_supervisorModelManager->onGetSelectedGroup(selectedGroup);
}


/*!
* @fn      onGetSelectedGauge
* @param   QString
* @return  None
* @brief   This function is responsible for getting selected gauge
*/
void ModelManagers::onGetSelectedGauge(int selectedGauge)
{
  INFO_LOG("selected Gauge in ModelManagers::onGetSelectedGauge" << selectedGauge);
  m_supervisorModelManager->onGetSelectedGauge(selectedGauge);
}

/*!
 * @fn      onExit
 * @param   None
 * @return  None
 * @brief Function will call on exit
 */
void ModelManagers::onExit()
{
  TRACE_LOG("");
  DEBUG_LOG("Calling exit of Scanner Admin and User Admin model managers.");
#ifdef WORKSTATION
  m_ScannerAdminModelManager->onExit();
  m_reportAdminModelManager->onExit();
  m_tipConfigAdminModelManager->onExit();
  m_tipLibConfigAdminModelManager->onExit();
  m_antiVirusModelManager->onExit();
  m_supervisorModelManager->onExit();
#endif
  m_userAdminModelManager->onExit();
  Semaphore::getInstance()->release(1);
}
}  // end of namespace ws
}  // end of namespace analogic
