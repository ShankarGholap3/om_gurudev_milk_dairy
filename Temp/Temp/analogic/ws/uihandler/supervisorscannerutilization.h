/*!
* @file     supervisorscannerutilization.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling scanner agent utilization data.
* @date     Jun, 16 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORSCANNERUTILIZATION_H
#define SUPERVISORSCANNERUTILIZATION_H

#include <QDebug>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorScannerUtilization
 * \brief   This class contains parameters related to
 *          handling scanner agent utilization data.
 */
class SupervisorScannerUtilization: public QObject
{
    Q_OBJECT

public:
    /*!
      * @fn       SupervisorScannerUtilization
      * @param    None
      * @return   None
      * @brief    constructor for SupervisorScannerUtilization
      */
    explicit SupervisorScannerUtilization(QObject* parent = NULL);

    /*!
      * @fn       SupervisorScannerUtilization
      * @param    SupervisorScannerUtilization&
      * @return   SupervisorScannerUtilization&
      * @brief    copy constructor for SupervisorScannerUtilization
      */
    SupervisorScannerUtilization(const SupervisorScannerUtilization &obj);

    /*!
      * @fn       operator=
      * @param    SupervisorScannerUtilization&
      * @return   SupervisorScannerUtilization&
      * @brief    declaration for private assignment operator.
      */
    SupervisorScannerUtilization &operator=(const SupervisorScannerUtilization &obj);

    /*!
    * @fn       ~SupervisorScannerUtilization
    * @param    None
    * @return   None
    * @brief    Destructor for class SupervisorScannerUtilization.
    */
    virtual ~SupervisorScannerUtilization();

    /*!
     * @fn       getScannerUtilizationData
     * @param    None
     * @return   QString
     * @brief    get utilization data
     */
    float getScannerUtilizationData() const;

    /*!
     * @fn       setScannerUtilizationData
     * @param    None
     * @return   QString
     * @brief    set utilization data
     */
    void setScannerUtilizationData(const float &utilizationData);

private:
    float             m_scannerUtilizationData;               //!< to store Scanner utilization data
};
}
}
#endif // SUPERVISORSCANNERUTILIZATION_H
