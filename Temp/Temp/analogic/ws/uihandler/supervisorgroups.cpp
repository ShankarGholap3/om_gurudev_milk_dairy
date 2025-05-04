/*!
* @file     supervisorgroups.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling groups data.
* @date     Jun, 10 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
# include "supervisorgroups.h"

namespace analogic
{
namespace ws
{
/*!
  * @fn       SupervisorGroups
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorGroups
  */
SupervisorGroups::SupervisorGroups(QObject *parent):QObject(parent)
{

}

/*!
  * @fn       SupervisorGroups
  * @param    SupervisorGroups&
  * @return   SupervisorGroups
  * @brief    copy constructor for SupervisorGroups
  */
SupervisorGroups::SupervisorGroups(const SupervisorGroups &obj)
{
  m_uuid             = obj.m_uuid;
  m_name             = obj.m_name;
  m_type             = obj.m_type;
}

/*!
  * @fn       operator=
  * @param    SupervisorGroups&
  * @return   SupervisorGroups&
  * @brief    declaration for private assignment operator.
  */
SupervisorGroups &SupervisorGroups::operator=(const SupervisorGroups &obj)
{
  //Checking if the assigned object is same as the current one
  if(this == &obj)
  {
    return *this;
  }
  m_uuid             = obj.m_uuid;
  m_name             = obj.m_name;
  m_type             = obj.m_type;

  return *this;
}

/*!
 * @fn       ~SupervisorGroups
 * @param    None
 * @return   None
 * @brief    Destructor for class SupervisorGroups.
 */
SupervisorGroups::~SupervisorGroups()
{

}

/*!
   * @fn       getGroupsUuid
   * @param    None
   * @return   QString
   * @brief    get uuid
   */
QString SupervisorGroups::getGroupsUuid() const
{
  return m_uuid;
}

/*!
   * @fn       setGroupsUuid
   * @param    QString
   * @return   None
   * @brief    set uuid
   */
void SupervisorGroups::setGroupsUuid(const QString &uuid)
{
  m_uuid = uuid;
}

/*!
   * @fn       getGroupsType
   * @param    None
   * @return   int
   * @brief    get type
   */
int SupervisorGroups::getGroupsType() const
{
  return m_type;
}

/*!
   * @fn       setGroupsType
   * @param    int
   * @return   None
   * @brief    set type
   */
void SupervisorGroups::setGroupsType(const int &type)
{
  m_type = type;
}

/*!
   * @fn       getGroupsName
   * @param    None
   * @return   QString
   * @brief    get name
   */
QString SupervisorGroups::getGroupsName() const
{
  return m_name;
}

/*!
   * @fn       setGroupsName
   * @param    QString
   * @return   None
   * @brief    set name
   */
void SupervisorGroups::setGroupsName(const QString &name)
{
  m_name = name;
}

} // namespace ws
} // namespace analogic
