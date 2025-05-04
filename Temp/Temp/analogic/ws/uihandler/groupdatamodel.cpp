/*!
* @file     groupoverviewdatamodel.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           populating group data on UI.
* @date     May, 13 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDebug>
#include "analogic/ws/common.h"
#include "groupdatamodel.h"

namespace analogic
{
namespace ws
{

GroupDataModel* GroupDataModel::m_sGrpDataModelInstance = nullptr;

/*!
  * @fn       GroupDataModel
  * @param    None
  * @return   None
  * @brief    constructor for GroupDataModel
  */
GroupDataModel::GroupDataModel(QObject *parent)
  : QAbstractListModel(parent)
{

}

/*!
  * @fn       getGroupDataModelInstance
  * @param    None
  * @return   None
  * @brief    get Instance of GroupDataModel
  */
GroupDataModel* GroupDataModel::getGroupDataModelInstance()
{
  if(!m_sGrpDataModelInstance)
  {
    m_sGrpDataModelInstance = new GroupDataModel();
  }
  return m_sGrpDataModelInstance;
}

/*!
* @fn       destroyGroupDataModelInstance
* @param    None
* @return   None
* @brief    destroy Instance of GroupDataModel
*/
void GroupDataModel::destroyGroupDataModelInstance()
{
  if(m_sGrpDataModelInstance != nullptr)
  {
    delete m_sGrpDataModelInstance;
    m_sGrpDataModelInstance = nullptr;
  }
}

/*!
  * @fn       ~GroupDataModel
  * @param    None
  * @return   None
  * @brief    virtual destructor for GroupDataModel
  */
GroupDataModel::~GroupDataModel()
{

}

/*!
  * @fn       addGroupData
  * @param    const QString&
  * @return   None
  * @brief    to populate data into list model
  */
void GroupDataModel::addGroupData(const QString &grpData)
{
  if(grpData != NULL)
  {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_groupList.append(grpData);
    endInsertRows();
  }
}

/*!
  * @fn       data
  * @param    const QModelIndex
  * @return   int
  * @brief    returns number of rows.
  */
int GroupDataModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_groupList.count();
}

/*!
  * @fn       data
  * @param    const QModelIndex
  * @param    int
  * @return   QVariant
  * @brief    populates list model data.
  */
QVariant GroupDataModel::data(const QModelIndex &index, int role) const
{
  if (index.row() < 0 || index.row() >= m_groupList.count())
  {
    return QVariant();
  }
  const QString &groupData = m_groupList[index.row()];
  if (role == GroupRole)
  {
    return groupData;
  }
  return QVariant();
}

/*!
  * @fn       roleNames
  * @param    None
  * @return   QHash<int,QByteArray>
  * @brief    gives roleNames.
  */
QHash<int, QByteArray> GroupDataModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[GroupDataModelRole::GroupRole] = "group";
  return roles;
}

/*!
  * @fn       onGroupDataReceived
  * @param    QVector
  * @return   None
  * @brief    to populate GroupDataModel class on recieve of group name data.
  */
void GroupDataModel::onGroupDataReceived(const QVector<SupervisorGroups> &groupData)
{
  beginResetModel();
  m_groupList.clear();
  if(!groupData.isEmpty())
  {
    foreach(SupervisorGroups groupIndex, groupData )
    {
      if(groupIndex.getGroupsType() == GroupType::ROLLUP_GROUP)
      {
        addGroupData(groupIndex.getGroupsName());
      }
    }
  }
  endResetModel();
}
}
}

