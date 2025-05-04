/*!
* @file     supervisorSvsutilization.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling Svs agent utilization data.
* @date     Jun, 16 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORSVSUTILIZATION_H
#define SUPERVISORSVSUTILIZATION_H

#include <QObject>
#include <QDebug>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorSvsUtilization
 * \brief   This class contains parameters related to
 *          handling Svs agent utilization data.
 */
class SupervisorSvsUtilization : public QObject
{
    Q_OBJECT
public:
/*!
  * @fn       SupervisorSvsUtilization
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorSvsUtilization
  */
  explicit SupervisorSvsUtilization(QObject *parent = NULL);



/*!
  * @fn       SupervisorSvsUtilization
  * @param    SupervisorSvsUtilization&
  * @return   SupervisorSvsUtilization&
  * @brief    copy constructor for SupervisorSvsUtilization
  */
  SupervisorSvsUtilization(const SupervisorSvsUtilization &obj);

/*!
  * @fn       operator=
  * @param    SupervisorSvsUtilization&
  * @return   SupervisorSvsUtilization&
  * @brief    declaration for private assignment operator.
  */
  SupervisorSvsUtilization &operator=(const SupervisorSvsUtilization &obj);

/*!
  * @fn       ~SupervisorSvsUtilization
  * @param    None
  * @return   None
  * @brief    Destructor for class SupervisorSvsUtilization.
  */
  virtual ~SupervisorSvsUtilization();


/*!
  * @fn       getSvsUtilizationData
  * @param    None
  * @return   float
  * @brief    get utilization data
  */
  float getSvsUtilizationData() const;


/*!
  * @fn       setSvsUtilizationData
  * @param    None
  * @return   float
  * @brief    set utilization data
  */
  void setSvsUtilizationData(const float &utilizationData);


private:
  float             m_svsUtilizationData;               //!< to store Svs utilization data

};
}
}
#endif // SUPERVISORSVSUTILIZATION_H
