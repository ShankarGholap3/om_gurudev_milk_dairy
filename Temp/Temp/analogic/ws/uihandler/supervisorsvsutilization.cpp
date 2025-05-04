/*!
* @file     supervisorsvsutilization.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling svs utilization data.
* @date     Jun, 16 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDebug>
#include "supervisorsvsutilization.h"

namespace analogic
{
namespace ws
{
 /*!
  * @fn       SupervisorSvsUtilization
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorSvsUtilization
  */
SupervisorSvsUtilization::SupervisorSvsUtilization(QObject *parent) : QObject(parent)
{
  //m_svsUtilizationData          = "0";
}

/*!
  * @fn       SupervisorSvsUtilization
  * @param    None
  * @return   None
  * @brief    copy constructor for SupervisorSvsUtilization
  */
SupervisorSvsUtilization::SupervisorSvsUtilization(const SupervisorSvsUtilization &obj)
{
    m_svsUtilizationData          = obj.m_svsUtilizationData;
}

/*!
  * @fn       operator=
  * @param    SupervisorSvsUtilization&
  * @return   SupervisorSSvsUtilization&
  * @brief    declaration for private assignment operator.
  */
SupervisorSvsUtilization &SupervisorSvsUtilization::operator=(const SupervisorSvsUtilization &obj)
{
    //Checking if the assigned object is same as the current one
    if(this == &obj)
    {
        return *this;
    }
    m_svsUtilizationData    = obj.m_svsUtilizationData;
    return *this;
}

/*!
 * @fn       ~SupervisorSvsUtilization
 * @param    None
 * @return   None
 * @brief    Destructor for class SupervisorSvsUtilization.
 */
SupervisorSvsUtilization::~SupervisorSvsUtilization()
{

}

/*!
 * @fn       getSvsUtilizationData
 * @param    None
 * @return   float
 * @brief    get SvsUtilizationData
 */
float SupervisorSvsUtilization::getSvsUtilizationData() const
{
    return m_svsUtilizationData;
}

/*!
 * @fn       setSvsUtilizationData
 * @param    None
 * @return   float
 * @brief    set SvsUtilizationData
 */
void SupervisorSvsUtilization::setSvsUtilizationData(const float &svsUtilizationData)
{
    m_svsUtilizationData = svsUtilizationData;
}

} // namespace ws
} // namespace analogic
