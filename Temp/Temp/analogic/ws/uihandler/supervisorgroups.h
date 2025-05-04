/*!
* @file     supervisorgroups.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           handling groups data.
* @date     Jun, 10 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORGROUPS_H
#define SUPERVISORGROUPS_H

#include <QObject>
#include <QString>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorGroups
 * \brief   This class contains parameters related to
 *          handling groups data.
 */
class SupervisorGroups: public QObject
{
  Q_OBJECT

public:
  /*!
      * @fn       SupervisorGroups
      * @param    None
      * @return   None
      * @brief    constructor for SupervisorGroups
      */
  explicit SupervisorGroups(QObject* parent = NULL);

  /*!
      * @fn       SupervisorGroups
      * @param    SupervisorGroups&
      * @return   SupervisorGroups
      * @brief    copy constructor for SupervisorGroups
      */
  SupervisorGroups(const SupervisorGroups &obj);

  /*!
      * @fn       operator=
      * @param    SupervisorGroups&
      * @return   SupervisorGroups&
      * @brief    declaration for private assignment operator.
      */
  SupervisorGroups &operator=(const SupervisorGroups &obj);

  /*!
    * @fn       ~SupervisorGroups
    * @param    None
    * @return   None
    * @brief    Destructor for class SupervisorGroups.
    */
  virtual ~SupervisorGroups();

  /*!
     * @fn       getGroupsUuid
     * @param    None
     * @return   QString
     * @brief    get uuid
     */
  QString getGroupsUuid() const;

  /*!
     * @fn       setGroupsUuid
     * @param    None
     * @return   QString
     * @brief    set uuid
     */
  void setGroupsUuid(const QString &uuid);

  /*!
     * @fn       getGroupsType
     * @param    None
     * @return   int
     * @brief    get type
     */
  int getGroupsType() const;

  /*!
     * @fn       setGroupsType
     * @param    const int &type
     * @return   None
     * @brief    set type
     */
  void setGroupsType(const int &type);

  /*!
     * @fn       getGroupsName
     * @param    None
     * @return   QString
     * @brief    get name
     */
  QString getGroupsName() const;

  /*!
     * @fn       setGroupsName
     * @param    const QString &type
     * @return   None
     * @brief    set name
     */
  void  setGroupsName(const QString &name);

private:
  QString                 m_uuid;               //!< uuid of groups
  int                     m_type;               //!< type of groups
  QString                 m_name;               //!< name id of groups
};
}
}

#endif // SUPERVISORGROUPS_H
