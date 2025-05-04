/*!
* @file     workstationbuilder.h
* @author   Agiliad
* @brief    builder for workstation
* @date     Mar, 03 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_APPBUILDER_WORKSTATIONBUILDER_H_
#define ANALOGIC_WS_APPBUILDER_WORKSTATIONBUILDER_H_

#include <QObject>
#include <frameworkcommon.h>
#include <analogic/ws/appcontroller.h>
#include <analogic/ws/uihandler/view.h>
#include <analogic/ws/ulm/uilayoutmanager.h>
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
#include <analogic/ws/uihandler/viewmanager.h>
#include <analogic/ws/uihandler/modelmanagers.h>

namespace analogic
{
namespace ws
{

/*!
 * \class   WorkstationBuilder
 * \brief   This is implementation for workstation builder. This is singletone class with private static instance
 *          destructor is public which will reset the instance flag so subsequent get instance will be as first time
 *          request.
 */
class WorkstationBuilder: public QObject
{
public:
  /*!
    * @fn       WorkstationBuilder
    * @param    None
    * @return   None
    * @brief    destructor for the class.
    */
  ~WorkstationBuilder();

  /*!
    * @fn       workstationTypeBasedQmlRegister
    * @param    QMLEnums::WSType  - wsType
    * @param    boost::shared_ptr<ViewManager> viewManager
    * @param    boost::shared_ptr<WorkstationManager> workstationManager
    * @return   None
    * @brief    make connection based on type provided.
    */
  static void workstationTypeBasedQmlRegister(QMLEnums::WSType wsType,
                                              boost::shared_ptr<ViewManager> viewManager,
                                              boost::shared_ptr<WorkstationManager> workstationManager);

  /*!
    * @fn       workstationTypeBasedConnection
    * @param    QMLEnums::WSType  - wsType
    * @param    boost::shared_ptr<ViewManager> viewManager
    * @param    boost::shared_ptr<WorkstationManager> workstationManager
    * @return   None
    * @brief    make connection based on type provided.
    */
  static void workstationTypeBasedConnection(QMLEnums::WSType wsType,
                                             boost::shared_ptr<ViewManager> viewManager,
                                             boost::shared_ptr<WorkstationManager> workstationManager,
                                             boost::shared_ptr<ModelManagers> modalmanager,
                                             UILayoutManager* uilayoutmanager );

private:
  /*!
    * @fn       WorkstationBuilder
    * @param    QObject *parent
    * @return   None
    * @brief    private constructor
    */
  explicit WorkstationBuilder(QObject *parent = 0) {
    Q_UNUSED(parent);
  }
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_APPBUILDER_WORKSTATIONBUILDER_H_
