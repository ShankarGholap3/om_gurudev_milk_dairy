/*!
* @file     supervisorconfigdatamodel.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           populating group configuration data on UI.
* @date     July, 29 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORCONFIGDATAMODEL_H
#define SUPERVISORCONFIGDATAMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <analogic/ws/uihandler/modelmanagers.h>
#include <QHash>
#include <analogic/ws/uihandler/supervisorscannerutilization.h>
#include <analogic/ws/uihandler/supervisorpvsutilization.h>
#include <analogic/ws/uihandler/supervisorsvsutilization.h>

namespace analogic
{
namespace ws
{
class SupervisorConfigData
{
public:

  /*!
    * @fn       SupervisorConfigData
    * @param    None
    * @return   None
    * @brief    constructor for SupervisorConfigData
    */
  SupervisorConfigData(const QString& groupMemberUUID, const int &groupType, const QString &groupName);

  /*!
    * @fn       SupervisorConfigData
    * @param    None
    * @return   None
    * @brief    destructor for SupervisorConfigData
    */
  ~SupervisorConfigData();

  /*!
    * @fn       groupType
    * @param    None
    * @return   int
    * @brief    return group type
    */
  int groupType() const;

  /*!
    * @fn       groupName
    * @param    None
    * @return   QString
    * @brief    return group name
    */
  QString groupName() const;
  /*!
    * @fn       setGroupType
    * @param    int
    * @return   None
    * @brief    set Group Type
    */
  void setGroupType(const int& groupType);

  /*!
    * @fn       setGroupName
    * @param    QString
    * @return   None
    * @brief    set Group Name
    */
  void setGroupName(const QString& strGroupName);

  /*!
    * @fn       groupMemberUuid
    * @param    None
    * @return   QString
    * @brief    return group uuid
    */
  QString groupMemberUuid() const;

  /*!
    * @fn       setGroupMemberUuid
    * @param    QString
    * @return   None
    * @brief    set Group Member Uuid
    */
  void setGroupMemberUuid(const QString &groupMemberUuid);

private:
  int                   m_groupType;                  //!< holds group type
  QString               m_groupName;                  //!< holds group
  QString               m_groupMemberUuid;            //!< holds group member uuid
};

class SupervisorConfigDataModel : public QAbstractTableModel
{
  Q_OBJECT

private:
  /*!
    * @fn       SupervisorConfigDataModel
    * @param    None
    * @return   None
    * @brief    constructor for SupervisorConfigDataModel
    */
  explicit SupervisorConfigDataModel(QObject *parent = nullptr);

  /*!
    * @fn       SupervisorConfigDataModel
    * @param    None
    * @return   None
    * @brief    destructor for SupervisorConfigDataModel
    */
  virtual ~SupervisorConfigDataModel();

public:

  /*!
    * @enum   None
    * @brief  enum for role names
    */
  enum supervisorConfigdataModelrole
  {
    GroupTypeRole = Qt::UserRole + 53,
    GroupNameRole
  };

  /*!
  * @fn       rowCount
  * @param    const QModelIndex
  * @return   int
  * @brief    returns number of rows.
  */
  int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /*!
  * @fn       columnCount
  * @param    const QModelIndex
  * @return   int
  * @brief    returns number of columns
  */
  int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /*!
  * @fn       data
  * @param    const QModelIndex
  * @param    int
  * @return   QVariant
  * @brief    populates list data.
  */
  QVariant data(const QModelIndex &index, int role =  Qt::DisplayRole) const Q_DECL_OVERRIDE;

  /*!
    * @fn       getSupervisorConfigGroupInstance
    * @param    None
    * @return   None
    * @brief    get Instance of SupervisorConfigDataModel
    */
  static SupervisorConfigDataModel* getSupervisorConfigGroupInstance();

  /*!
    * @fn       getSupervisorConfigMemberInstance
    * @param    None
    * @return   None
    * @brief    get Instance of SupervisorConfigDataModel
    */
  static SupervisorConfigDataModel* getSupervisorConfigMemberInstance();

  /*!
    * @fn       getSupervisorConfigNonMemberInstance
    * @param    None
    * @return   None
    * @brief    get Instance of SupervisorConfigDataModel
    */
  static SupervisorConfigDataModel* getSupervisorConfigNonMemberInstance();

  /*!
  * @fn       destroySupervisorConfigGroupInstance
  * @param    None
  * @return   None
  * @brief    destroy Instance of SupervisorConfigDataModel
  */
  static void destroySupervisorConfigGroupInstance();

  /*!
  * @fn       destroySupervisorConfigMemberInstance
  * @param    None
  * @return   None
  * @brief    destroy Instance of SupervisorConfigDataModel
  */
  static void destroySupervisorConfigMemberInstance();

  /*!
  * @fn       destroySupervisorConfigNonMemberInstance
  * @param    None
  * @return   None
  * @brief    destroy Instance of SupervisorConfigDataModel
  */
  static void destroySupervisorConfigNonMemberInstance();

  /*!
    * @fn       addGroup
    * @param    int
    * @param    QString
    * @return   None
    * @brief    add group data to display in table view
    */
  void addGroup(int, QString);

  /*!
    * @fn       onSupervisorConfigDataReceived
    * @param    int
    * @param    QString
    * @return   None
    * @brief    receive group data through group creation
    */
  Q_INVOKABLE void onSupervisorGroupConfigDataReceived(int, QString);

  /*!
    * @fn       getCurrentSelectedRow
    * @param    None
    * @return   None
    * @brief    get current selected row
    */
  Q_INVOKABLE int getCurrentSelectedRow();

  /*!
    * @fn       setCurrentSelectedRow
    * @param    None
    * @return   None
    * @brief    set current selected row
    */
  Q_INVOKABLE void setCurrentSelectedRow(const int index);

  /*!
    * @fn       deleteSelectedGroupData
    * @param    None
    * @return   None
    * @brief    delete selected group data
    */
  Q_INVOKABLE void deleteSelectedGroupData(int nRow);

  /*!
    * @fn       addMember
    * @param    None
    * @return   None
    * @brief    add members in selected group
    */
  Q_INVOKABLE bool addMember();

  /*!
    * @fn       removeMember
    * @param    int
    * @return   int
    * @brief    remove members in selected group
    */
  Q_INVOKABLE int removeMember(int);

  /*!
    * @fn       getCurrentSelectedRowData
    * @param    None
    * @return   QString
    * @brief    get current selected row data
    */
  Q_INVOKABLE QString getCurrentSelectedRowData();

  /*!
    * @fn       getCurrentSelectedRowMemberType
    * @param    None
    * @return   int
    * @brief    get current selected row member type
    */
  Q_INVOKABLE int getCurrentSelectedRowMemberType();

  /*!
    * @fn       clearAllMembers
    * @param    None
    * @return   None
    * @brief    delete all members
    */
  Q_INVOKABLE void clearAllMembers();

  /*!
    * @fn       resetNonMembers
    * @param    None
    * @return   None
    * @brief    reset non Members
    */
  Q_INVOKABLE void resetNonMemberData();

  /*!
    * @fn       saveMembers
    * @param    None
    * @return   None
    * @brief    save Members
    */
  Q_INVOKABLE void saveMembers();

  /*!
    * @fn       generateUUID
    * @param    None
    * @return   QString
    * @brief    generate uuid
    */
  Q_INVOKABLE static QString generateUUID();

protected:
  /*!
    * @fn       roleNames
    * @param    None
    * @return   QHash<int,QByteArray>
    * @brief    gives roleNames.
    */
  QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
  QList<SupervisorConfigData>                              m_memberData;
  static SupervisorConfigDataModel*                        m_supervisorConfigGroupInstance; //!< Instance of SupervisorConfigDataModel
  static SupervisorConfigDataModel*                        m_supervisorConfigMemberInstance; //!< Instance of SupervisorConfigDataModel
  static SupervisorConfigDataModel*                        m_supervisorConfigNonMemberInstance; //!< Instance of SupervisorConfigDataModel
  int                                                      m_currentRow;                        //!< Holds current row index
  static bool                                              m_sGroupInstanceFlag;                //!< Instance flag to monitor life of instance
  static bool                                              m_sMemberInstanceFlag;      //!< Instance flag to monitor life of instance
  static bool                                              m_sNonMemberInstanceFlag;          //!< Instance flag to monitor life of instance
  static SupervisorConfigData                              m_groupingMemberData;
  QMap<QString, QVector<SupervisorConfigData>>             m_subMemberData;
  QString                                                  m_currentGroupDataUuid;
};
}
}
#endif // SUPERVISORCONFIGDATAMODEL_H
