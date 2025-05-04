/*!
* @file     supervisorpvsutilization.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling Pvs agent utilization data.
* @date     Jun, 16 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORPVSUTILIZATION_H
#define SUPERVISORPVSUTILIZATION_H

#include <QObject>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorPvsUtilization
 * \brief   This class contains parameters related to
 *          handling Pvs agent utilization data.
 */
class SupervisorPvsUtilization : public QObject
{
    Q_OBJECT
public:
/*!
  * @fn       SupervisorPvsUtilization
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorPvsUtilization
  */
  explicit SupervisorPvsUtilization(QObject* parent = NULL); //QObject *parent = nullptr);

/*!
  * @fn       SupervisorPvsUtilization
  * @param    SupervisorPvsUtilization&
  * @return   SupervisorPvsUtilization&
  * @brief    copy constructor for SupervisorPvsUtilization
  */
  SupervisorPvsUtilization(const SupervisorPvsUtilization &obj);

/*!
  * @fn       operator=
  * @param    SupervisorPvsUtilization&
  * @return   SupervisorPvsUtilization&
  * @brief    declaration for private assignment operator.
  */
  SupervisorPvsUtilization &operator=(const SupervisorPvsUtilization &obj);

/*!
  * @fn       ~SupervisorPvsUtilization
  * @param    None
  * @return   None
  * @brief    Destructor for class SupervisorPvsUtilization.
  */
  virtual ~SupervisorPvsUtilization();

/*!
  * @fn       getPvsUtilizationData
  * @param    None
  * @return   float
  * @brief    get utilization data
  */
  float getPvsUtilizationData() const;

/*!
  * @fn       setPvsUtilizationData
  * @param    None
  * @return   float
  * @brief    set utilization data
  */
  void setPvsUtilizationData(const float &utilizationData);

private:
  float             m_pvsUtilizationData;               //!< to store Pvs utilization data

};
}
}
#endif // SUPERVISORPVSUTILIZATION_H
