/*!
* @file     supervisorpvsutilization.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling Pvs utilization data.
* @date     Jun, 16 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDebug>
#include "supervisorpvsutilization.h"

namespace analogic
{
namespace ws
{
 /*!
  * @fn       SupervisorPvsUtilization
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorPvsUtilization
  */
SupervisorPvsUtilization::SupervisorPvsUtilization(QObject *parent) : QObject(parent)
{
  //m_pvsUtilizationData          = "0";
}

/*!
  * @fn       SupervisorPvsUtilization
  * @param    None
  * @return   None
  * @brief    copy constructor for SupervisorPvsUtilization
  */
SupervisorPvsUtilization::SupervisorPvsUtilization(const SupervisorPvsUtilization &obj)
{
    m_pvsUtilizationData          = obj.m_pvsUtilizationData;
}

/*!
  * @fn       operator=
  * @param    SupervisorPvsUtilization&
  * @return   SupervisorSPvsUtilization&
  * @brief    declaration for private assignment operator.
  */
SupervisorPvsUtilization &SupervisorPvsUtilization::operator=(const SupervisorPvsUtilization &obj)
{
    //Checking if the assigned object is same as the current one
    if(this == &obj)
    {
        return *this;
    }
    m_pvsUtilizationData    = obj.m_pvsUtilizationData;
    return *this;
}

/*!
 * @fn       ~SupervisorPvsUtilization
 * @param    None
 * @return   None
 * @brief    Destructor for class SupervisorPvsUtilization.
 */
SupervisorPvsUtilization::~SupervisorPvsUtilization()
{

}

/*!
 * @fn       getPvsUtilizationData
 * @param    None
 * @return   float
 * @brief    get PvsUtilizationData
 */
float SupervisorPvsUtilization::getPvsUtilizationData() const
{
    return m_pvsUtilizationData;
}

/*!
 * @fn       setPvsUtilizationData
 * @param    None
 * @return   float
 * @brief    set PvsUtilizationData
 */
void SupervisorPvsUtilization::setPvsUtilizationData(const float &PvsUtilizationData)
{
    m_pvsUtilizationData = PvsUtilizationData;
}

} // namespace ws
} // namespace analogic
