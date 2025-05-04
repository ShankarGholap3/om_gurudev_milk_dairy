/*!
* @file     workstationfactory.cpp
* @author   Agiliad
* @brief    factory for workstation
* @date     Oct, 25 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/appbuilder/workstationfactory.h>

namespace analogic
{
namespace ws
{

bool WorkStationFactory::m_workStationFactoryInstanceflag = false;
WorkStationFactory* WorkStationFactory::m_workStationFactoryInstance = 0;


/*!
* @fn       WorkStationFactory
* @param    None
* @return   None
* @brief    private constructor
*/
WorkStationFactory::WorkStationFactory()
{
  m_workstationManagerInstance = NULL;
}

/*!
* @fn       getWorkstationFactoryInstance
* @param    None
* @return   WorkStationFactory*
* @brief    get instance for WorkStationFactory singletone
*/
WorkStationFactory* WorkStationFactory::getWorkstationFactoryInstance()
{
  if ( m_workStationFactoryInstanceflag == false )
  {
    DEBUG_LOG("Creating workStationFactory Instance");
    m_workStationFactoryInstance = new WorkStationFactory();
    m_workStationFactoryInstanceflag = true;
    return m_workStationFactoryInstance;
  }
  else
  {
    return m_workStationFactoryInstance;
  }
}

/*!
* @fn       ~WorkStationFactory
* @param    None
* @return   None
* @brief    destructor for the class.
*/
WorkStationFactory::~WorkStationFactory() {
}

/*!
* @fn       destroy
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void WorkStationFactory::destroy()
{
  SAFE_DELETE(m_workstationManagerInstance);
  SAFE_DELETE(m_workStationFactoryInstance);
  m_workStationFactoryInstanceflag = false;
}

/*!
* @fn       createWorkstationInstance
* @param    QMLEnums::WSType  - wsType
* @return   WorkstationManager*
* @brief    create instance of workstation manager based on type provided.
*/
WorkstationManager* WorkStationFactory::createWorkstationInstance(QMLEnums::WSType wsType) {
  DEBUG_LOG("initialize workstation manager");
#ifdef WORKSTATION
  if (wsType == QMLEnums::WSType::OSR_WORKSTATION)
  {
    DEBUG_LOG("Creating OSR workstation");
    m_workstationManagerInstance = new OsrManager();
  }
  else if (wsType == QMLEnums::WSType::SEARCH_WORKSTATION)
  {
    DEBUG_LOG("Creating  Search workstation");
    m_workstationManagerInstance = new SearchManager();
  }
  else if (wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION)
  {
    DEBUG_LOG("Creating Supervisor workstation");
    m_workstationManagerInstance = new SupervisorManager();
  }
#endif
#ifdef RERUN
  if (wsType == QMLEnums::WSType::RERUN_EMULATOR)
  {
    DEBUG_LOG("Creating  Rerun workstation");
    m_workstationManagerInstance = new RerunManager();
  }
  else if (wsType == QMLEnums::WSType::TRAINING_WORKSTATION)
  {
    DEBUG_LOG("Creating  Training workstation");
    m_workstationManagerInstance = new TrainingManager();
  }
#endif
  else
  {
    ERROR_LOG("Invalid WorkstationType; Check INI file");
  }
  return m_workstationManagerInstance;
}
}  // end of namespace ws
}  // end of namespace analogic
