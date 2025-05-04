/*!
* @file     workstationfactory.h
* @author   Agiliad
* @brief    factory for workstation
* @date     Oct, 25 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_APPBUILDER_WORKSTATIONFACTORY_H_
#define ANALOGIC_WS_APPBUILDER_WORKSTATIONFACTORY_H_

#include <frameworkcommon.h>
#include <analogic/ws/workstationmanager.h>
#ifdef WORKSTATION
#include <analogic/ws/osr/osrmanager.h>
#include <analogic/ws/search/searchmanager.h>
#include <analogic/ws/supervisor/supervisormanager.h>
#endif
#ifdef RERUN
#include <analogic/ws/rerun/rerunmanager.h>
#include <analogic/ws/training/trainingmanager.h>
#endif
#include <analogic/ws/appbuilder/abstractworkstationfatcory.h>


namespace analogic
{
namespace ws
{

/*!
 * \class   WorkStationFactory
 * \brief   This is implementation for workstation factory. This is singletone class with private static instance
 *          destructor is public which will reset the instance flag so subsequent get instance will be as first time
 *          request. workstation manager pointer is member of the factory class. The last pointer of the workstation
 *          manager class will be deleted in destructor of factory.
 * \inherits AbstractWorkStationFatcory
 * \implements AbstractWorkStationFatcory
 */
class WorkStationFactory : public AbstractWorkStationFatcory
{
public:
  /*!
    * @fn       WorkStationFactory
    * @param    None
    * @return   None
    * @brief    destructor for the class.
    */
  ~WorkStationFactory();

  /*!
    * @fn       getWorkstationFactoryInstance
    * @param    None
    * @return   WorkStationFactory*
    * @brief    get instance for WorkStationFactory singletone
    */
  static WorkStationFactory* getWorkstationFactoryInstance();

  /*!
    * @fn       destroy
    * @param    None
    * @return   None
    * @brief    This function is responsible for release of any resources if req.
    */
  void destroy();
  /*!
    * @fn       createWorkstationInstance
    * @param    QMLEnums::WSType  - wsType
    * @return   WorkstationManager*
    * @brief    create instance of workstation manager based on type provided.
    */
  WorkstationManager* createWorkstationInstance(QMLEnums::WSType wsType);

private:
  /*!
    * @fn       WorkStationFactory
    * @param    None
    * @return   None
    * @brief    private constructor
    */
  WorkStationFactory();
  static bool                m_workStationFactoryInstanceflag;  //!< instance flag to monitor the life of instance
  static WorkStationFactory *m_workStationFactoryInstance;      //!< instance of WorkStationFactory
  WorkstationManager*        m_workstationManagerInstance;      //!< workstation Manager instance
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  //  ANALOGIC_WS_APPBUILDER_WORKSTATIONFACTORY_H_
