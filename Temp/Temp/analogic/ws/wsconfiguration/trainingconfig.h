/*!
* @file     trainingconfig.h
* @author   Agiliad
* @brief    This file contains interface having configuration for screens of Training Workstation.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_WSCONFIGURATION_TRAININGCONFIG_H_
#define ANALOGIC_WS_WSCONFIGURATION_TRAININGCONFIG_H_

#include <analogic/ws/wsconfiguration/workstationconfig.h>

namespace analogic
{
namespace ws
{
/*!
 * \class   TrainingConfig
 * \brief   This class contains variable and function related to
 *          configuration for screens of Training Workstation.
 */
class TrainingConfig : public WorkstationConfig
{
public:
    /*!
    * @fn       TrainingConfig
    * @param    None
    * @return   None
    * @brief    Constructor for class TrainingConfig.
    */
    TrainingConfig();

    /*!
    * @fn       ~TrainingConfig
    * @param    None
    * @return   None
    * @brief    Destructor for class TrainingConfig.
    */
    ~TrainingConfig();
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_WSCONFIGURATION_TRAININGCONFIG_H_

