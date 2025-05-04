/*!
* @file     supervisorscannerutilization.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling scanner utilization data.
* @date     Jun, 16 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDebug>
#include "analogic/ws/uihandler/supervisorscannerutilization.h"

namespace analogic
{
namespace ws
{
 /*!
  * @fn       SupervisorScannerUtilization
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorScannerUtilization
  */
SupervisorScannerUtilization::SupervisorScannerUtilization(QObject *parent):QObject(parent)
{
    //m_scannerUtilizationData          = 0.0F;
}

/*!
  * @fn       SupervisorScannerUtilization
  * @param    None
  * @return   None
  * @brief    copy constructor for SupervisorScannerUtilization
  */
SupervisorScannerUtilization::SupervisorScannerUtilization(const SupervisorScannerUtilization &obj)
{
    m_scannerUtilizationData          = obj.m_scannerUtilizationData;
}

/*!
  * @fn       operator=
  * @param    SupervisorScannerUtilization&
  * @return   SupervisorScannerUtilization&
  * @brief    declaration for private assignment operator.
  */
SupervisorScannerUtilization &SupervisorScannerUtilization::operator=(const SupervisorScannerUtilization &obj)
{
    //Checking if the assigned object is same as the current one
    if(this == &obj)
    {
        return *this;
    }
    m_scannerUtilizationData    = obj.m_scannerUtilizationData;
    return *this;
}

/*!
 * @fn       ~SupervisorScannerUtilization
 * @param    None
 * @return   None
 * @brief    Destructor for class SupervisorScannerUtilization.
 */
SupervisorScannerUtilization::~SupervisorScannerUtilization()
{

}

/*!
 * @fn       getScannerUtilizationData
 * @param    None
 * @return   float
 * @brief    get ScannerUtilizationData
 */
float SupervisorScannerUtilization::getScannerUtilizationData() const
{
    return m_scannerUtilizationData;
}

/*!
 * @fn       setScannerUtilizationData
 * @param    None
 * @return   float
 * @brief    set ScannerUtilizationData
 */
void SupervisorScannerUtilization::setScannerUtilizationData(const float &scannerUtilizationData)
{
    m_scannerUtilizationData = scannerUtilizationData;
}
//

} // namespace ws
} // namespace analogic
