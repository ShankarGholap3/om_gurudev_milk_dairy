/*!
* @file     abstractworkstationfatcory.h
* @author   Agiliad
* @brief    abstract class for workstation factory
* @date     Oct, 25 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_APPBUILDER_ABSTRACTWORKSTATIONFATCORY_H_
#define ANALOGIC_WS_APPBUILDER_ABSTRACTWORKSTATIONFATCORY_H_

#include <analogic/ws/workstationmanager.h>

namespace analogic
{
namespace ws
{

/*!
 * \class   AbstractWorkStationFatcory
 * \brief   This is interface for workstation factory
 */
class AbstractWorkStationFatcory
{
public:
  /*!
    * @fn       AbstractWorkStationFatcory
    * @param    None
    * @return   None
    * @brief    virtual destructor for base class.
    */
  virtual ~AbstractWorkStationFatcory() {
  }

  /*!
    * @fn       createWorkstationInstance
    * @param    QMLEnums::WSType  - wsType
    * @return   WorkstationManager*
    * @brief    create instance of workstation manager based on type provided.
    */
  virtual WorkstationManager* createWorkstationInstance(QMLEnums::WSType wsType) = 0;

  /*!
    * @fn       destroy
    * @param    None
    * @return   None
    * @brief    This function is responsible for release of any resources if req.
    */
  virtual void destroy() = 0;
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_APPBUILDER_ABSTRACTWORKSTATIONFATCORY_H_
