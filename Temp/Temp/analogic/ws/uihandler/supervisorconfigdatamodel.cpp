/*!
* @file     supervisorconfigdatamodel.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           populating group configuration data on UI.
* @date     July, 29 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#include "supervisorconfigdatamodel.h"
#include "analogic/ws/common.h"
#include <QUuid>

namespace analogic
{
namespace ws
{
bool SupervisorConfigDataModel::m_sGroupInstanceFlag = false;
bool SupervisorConfigDataModel::m_sMemberInstanceFlag = false;
bool SupervisorConfigDataModel::m_sNonMemberInstanceFlag = false;
SupervisorConfigDataModel* SupervisorConfigDataModel::m_supervisorConfigGroupInstance = nullptr;
SupervisorConfigDataModel* SupervisorConfigDataModel::m_supervisorConfigMemberInstance = nullptr;
SupervisorConfigDataModel* SupervisorConfigDataModel::m_supervisorConfigNonMemberInstance = nullptr;
SupervisorConfigData SupervisorConfigDataModel::m_groupingMemberData("",-1,"");

/*!
  * @fn       SupervisorConfigData
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorConfigData
  */
SupervisorConfigData::SupervisorConfigData(const QString& groupMemberUUID, const int& groupType, const QString &groupname)
  :m_groupMemberUuid(groupMemberUUID), m_groupType(groupType), m_groupName(groupname)
{

}

/*!
  * @fn       SupervisorConfigData
  * @param    None
  * @return   None
  * @brief    destructor for SupervisorConfigData
  */
SupervisorConfigData::~SupervisorConfigData()
{

}

/*!
  * @fn       groupType
  * @param    None
  * @return   int
  * @brief    return group type
  */
int SupervisorConfigData::groupType() const
{
  return m_groupType;
}

/*!
  * @fn       groupName
  * @param    None
  * @return   QString
  * @brief    return group name
  */
QString SupervisorConfigData::groupName() const
{
  return m_groupName;
}

/*!
  * @fn       setGroupType
  * @param    int
  * @return   None
  * @brief    set Group Type
  */
void SupervisorConfigData::setGroupType(const int& groupType)
{
  m_groupType = groupType;
}

/*!
  * @fn       setGroupName
  * @param    QString
  * @return   None
  * @brief    set Group Name
  */
void SupervisorConfigData::setGroupName(const QString& strGroupName)
{
  m_groupName = strGroupName;
}

/*!
  * @fn       groupMemberUuid
  * @param    None
  * @return   QString
  * @brief    return group uuid
  */
QString SupervisorConfigData::groupMemberUuid() const
{
  return m_groupMemberUuid;
}

/*!
  * @fn       setGroupMemberUuid
  * @param    QString
  * @return   None
  * @brief    set group uuid
  */
void SupervisorConfigData::setGroupMemberUuid(const QString& groupMemberUuid)
{
  m_groupMemberUuid = groupMemberUuid;
}

/*!
  * @fn       SupervisorConfigDataModel
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorConfigDataModel
  */
SupervisorConfigDataModel::SupervisorConfigDataModel(QObject *parent) : QAbstractTableModel(parent)
{

}

/*!
  * @fn       roleNames
  * @param    None
  * @return   QHash<int,QByteArray>
  * @brief    gives roleNames.
  */
QHash<int, QByteArray> SupervisorConfigDataModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[GroupTypeRole] = "groupType";
  roles[GroupNameRole] = "groupName";

  return roles;
}

/*!
* @fn       columnCount
* @param    const QModelIndex
* @return   int
* @brief    returns number of columns
*/
int SupervisorConfigDataModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 2;
}

/*!
* @fn       rowCount
* @param    const QModelIndex
* @return   int
* @brief    returns number of rows.
*/
int SupervisorConfigDataModel::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return m_memberData.size();
}

/*!
* @fn       data
* @param    const QModelIndex
* @param    int
* @return   QVariant
* @brief    populates list data.
*/
QVariant SupervisorConfigDataModel::data(const QModelIndex &index, int role) const
{
  if (index.row() < 0 || index.row() >= m_memberData.count())
  {
    return QVariant();
  }
  const SupervisorConfigData &supervisorData = m_memberData[index.row()];
  if (role == GroupTypeRole)
  {
    return supervisorData.groupType();
  }
  else if (role == GroupNameRole)
  {
    return supervisorData.groupName();
  }

  return QVariant();
}

/*!
  * @fn       getSupervisorConfigGroupInstance
  * @param    None
  * @return   None
  * @brief    get Instance of SupervisorConfigDataModel
  */
SupervisorConfigDataModel* SupervisorConfigDataModel::getSupervisorConfigGroupInstance()
{
  if(m_sGroupInstanceFlag == false )
  {
    DEBUG_LOG("Creating instance of SupervisorGroupConfigDataModel");
    m_supervisorConfigGroupInstance = new SupervisorConfigDataModel();
    m_sGroupInstanceFlag = true;
    DEBUG_LOG("Created new instance of SupervisorGroupConfigDataModel" << m_supervisorConfigGroupInstance);
    return m_supervisorConfigGroupInstance;
  }
  else
  {
    DEBUG_LOG("Return existing instance of SupervisorGroupConfigDataModel" << m_supervisorConfigGroupInstance);
    return m_supervisorConfigGroupInstance;
  }
}

/*!
* @fn       destroySupervisorConfigMemberInstance
* @param    None
* @return   None
* @brief    destroy Instance of SupervisorConfigDataModel
*/
void SupervisorConfigDataModel::destroySupervisorConfigGroupInstance()
{
  if((m_sGroupInstanceFlag == true) && (m_supervisorConfigGroupInstance != nullptr))
  {
    delete m_supervisorConfigGroupInstance;
    m_supervisorConfigGroupInstance = nullptr;
    m_sGroupInstanceFlag = false;
  }
}

/*!
  * @fn       getSupervisorConfigMemberInstance
  * @param    None
  * @return   None
  * @brief    get Instance of SupervisorConfigDataModel
  */
SupervisorConfigDataModel* SupervisorConfigDataModel::getSupervisorConfigMemberInstance()
{
  if(m_sMemberInstanceFlag == false )
  {
    DEBUG_LOG("Creating instance of SupervisorConfigMemberDataModel");
    m_supervisorConfigMemberInstance = new SupervisorConfigDataModel();
    m_sMemberInstanceFlag = true;
    DEBUG_LOG("Created new instance of SupervisorConfigMemberDataModel" << m_supervisorConfigGroupInstance);
    return m_supervisorConfigMemberInstance;
  }
  else
  {
    DEBUG_LOG("Return existing instance of SupervisorConfigMemberDataModel" << m_supervisorConfigGroupInstance);
    return m_supervisorConfigMemberInstance;
  }
}

/*!
* @fn       destroySupervisorConfigMemberInstance
* @param    None
* @return   None
* @brief    destroy Instance of SupervisorConfigDataModel
*/
void SupervisorConfigDataModel::destroySupervisorConfigMemberInstance()
{
  if((m_sMemberInstanceFlag == true) && (m_supervisorConfigMemberInstance != nullptr))
  {
    delete m_supervisorConfigMemberInstance;
    m_supervisorConfigMemberInstance = nullptr;
  }
}

/*!
  * @fn       getSupervisorConfigMemberInstance
  * @param    None
  * @return   None
  * @brief    get Instance of SupervisorConfigDataModel
  */
SupervisorConfigDataModel* SupervisorConfigDataModel::getSupervisorConfigNonMemberInstance()
{
  if(m_sNonMemberInstanceFlag == false )
  {
    DEBUG_LOG("Creating instance of SupervisorConfigNonMemberDataModel");
    m_supervisorConfigNonMemberInstance = new SupervisorConfigDataModel();
    m_sNonMemberInstanceFlag = true;
    DEBUG_LOG("Created new instance of SupervisorConfigNonMemberDataModel" << m_supervisorConfigGroupInstance);
    return m_supervisorConfigNonMemberInstance;
  }
  else
  {
    DEBUG_LOG("Return existing instance of SupervisorConfigNonMemberDataModel" << m_supervisorConfigGroupInstance);
    return m_supervisorConfigNonMemberInstance;
  }
}

/*!
* @fn       destroySupervisorConfigNonMemberInstance
* @param    None
* @return   None
* @brief    destroy Instance of SupervisorConfigDataModel
*/
void SupervisorConfigDataModel::destroySupervisorConfigNonMemberInstance()
{
  if((m_sNonMemberInstanceFlag == true) && m_supervisorConfigNonMemberInstance != nullptr)
  {
    delete m_supervisorConfigNonMemberInstance;
    m_supervisorConfigNonMemberInstance = nullptr;
  }
}

/*!
  * @fn       SupervisorConfigDataModel
  * @param    None
  * @return   None
  * @brief    destructor for SupervisorConfigDataModel
  */
SupervisorConfigDataModel::~SupervisorConfigDataModel()
{

}

/*!
  * @fn       getCurrentSelectedRow
  * @param    None
  * @return   None
  * @brief    get current selected row
  */
int SupervisorConfigDataModel::getCurrentSelectedRow()
{
  return m_currentRow;
}

/*!
  * @fn       onSupervisorConfigDataReceived
  * @param    int
  * @param    QString
  * @return   None
  * @brief    receive group data through group creation
  */
void SupervisorConfigDataModel::onSupervisorGroupConfigDataReceived(int groupType, QString strGroupName)
{
  addGroup(groupType, strGroupName);
}

/*!
  * @fn       addGroup
  * @param    int
  * @param    QString
  * @return   None
  * @brief    add group data to display in table view
  */
void SupervisorConfigDataModel::addGroup(int groupType, QString strGroupName)
{
  beginResetModel();
  beginInsertRows (QModelIndex(), rowCount(), rowCount());
  m_memberData.append(SupervisorConfigData(generateUUID(), groupType, strGroupName));
  endInsertRows();
  endResetModel();
}

/*!
  * @fn       setCurrentSelectedRow
  * @param    None
  * @return   None
  * @brief    set current selected row
  */
void SupervisorConfigDataModel::setCurrentSelectedRow(const int index)
{
  m_currentRow = index;
}

/*!
  * @fn       deleteSelectedGroupData
  * @param    None
  * @return   None
  * @brief    delete selected group data
  */
void SupervisorConfigDataModel::deleteSelectedGroupData(int nRow)
{
  if((!m_memberData.isEmpty()) && (nRow >=0) && (nRow < m_memberData.size()))
  {
    beginResetModel();
    beginRemoveRows(QModelIndex(), rowCount(), rowCount()-1);
    m_memberData.removeAt(nRow);
    endRemoveRows();
    endResetModel();
  }
}

/*!
  * @fn       addMember
  * @param    None
  * @return   None
  * @brief    add members in selected group
  */
bool SupervisorConfigDataModel::addMember()
{
  bool add;
  add = !m_groupingMemberData.groupName().isEmpty();
  if(add)
  {
    beginResetModel();
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_memberData.append(m_groupingMemberData);
    endInsertRows();
    endResetModel();
  }
  return add;
}

/*!
  * @fn       removeMember
  * @param    int
  * @return   int
  * @brief    remove members in selected group
  */
int SupervisorConfigDataModel::removeMember(int nRow)
{
  if((!m_memberData.isEmpty()) && (nRow >= 0) && (nRow < m_memberData.size()))
  {
    beginResetModel();
    beginRemoveRows(QModelIndex(), rowCount(), rowCount()-1);
    m_groupingMemberData = m_memberData.at(nRow);
    m_memberData.removeAt(nRow);
    endRemoveRows();
    endResetModel();
  }
  return nRow;
}

/*!
  * @fn       getCurrentSelectedRowData
  * @param    None
  * @return   QString
  * @brief    get current selected row data
  */
QString SupervisorConfigDataModel::getCurrentSelectedRowData()
{
  QString groupName = "";
  if((!m_memberData.isEmpty()) && (m_currentRow >= 0) && (m_currentRow < m_memberData.size()))
  {
    groupName = m_memberData.at(m_currentRow).groupName();
   // m_currentGroupDataUuid = m_memberData.at(m_currentRow).groupMemberUuid();
  }
  return groupName;
}

/*!
  * @fn       getCurrentSelectedRowMemberType
  * @param    None
  * @return   int
  * @brief    get current selected row member type
  */
int SupervisorConfigDataModel::getCurrentSelectedRowMemberType()
{
  int groupType = -1;
  if((!m_memberData.isEmpty()) && (m_currentRow >= 0) && (m_currentRow < m_memberData.size()))
  {
    groupType = m_memberData.at(m_currentRow).groupType();
  }
  return groupType;
}

/*!
  * @fn       clearAllMembers
  * @param    None
  * @return   None
  * @brief    delete all members
  */
void SupervisorConfigDataModel::clearAllMembers()
{
  beginResetModel();
  m_memberData.clear();
  endResetModel();
}

/*!
  * @fn       saveMembers
  * @param    None
  * @return   None
  * @brief    save Members
  */
void SupervisorConfigDataModel::saveMembers()
{

}

/*!
  * @fn       resetNonMembers
  * @param    None
  * @return   None
  * @brief    reset non Members
  */
void SupervisorConfigDataModel::resetNonMemberData()
{
  beginResetModel();
  m_memberData.clear();
  beginInsertRows (QModelIndex(), rowCount(), rowCount());
  m_memberData.append(SupervisorConfigData(generateUUID(), QMLEnums::SupervisorGroupingEnum::GROUPING_ELEMENT, "Scanner01"));
  m_memberData.append(SupervisorConfigData(generateUUID(), QMLEnums::SupervisorGroupingEnum::GROUPING_ELEMENT, "Scanner02"));
  m_memberData.append(SupervisorConfigData(generateUUID(), QMLEnums::SupervisorGroupingEnum::GROUPING_ELEMENT, "PVS01"));
  m_memberData.append(SupervisorConfigData(generateUUID(), QMLEnums::SupervisorGroupingEnum::GROUPING_ELEMENT, "SVS01"));
  m_memberData.append(SupervisorConfigData(generateUUID(), QMLEnums::SupervisorGroupingEnum::GROUPING_ELEMENT, "PVS02"));
  m_memberData.append(SupervisorConfigData(generateUUID(), QMLEnums::SupervisorGroupingEnum::GROUPING_GROUP, "Lane1"));
  m_memberData.append(SupervisorConfigData(generateUUID(), QMLEnums::SupervisorGroupingEnum::GROUPING_GROUP, "Lane2"));
  endInsertRows();
  endResetModel();
}

/*!
  * @fn       generateUUID
  * @param    None
  * @return   QString
  * @brief    generate uuid
  */
QString SupervisorConfigDataModel::generateUUID()
{
  return QUuid::createUuid().toString();
}

}
}
