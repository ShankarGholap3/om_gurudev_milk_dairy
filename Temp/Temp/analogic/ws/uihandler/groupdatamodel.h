/*!
* @file     groupoverviewdatamodel.h
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           populating group data on UI.
* @date     May, 13 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef GROUPDATAMODEL_H
#define GROUPDATAMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "supervisorgroups.h"

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   GroupDataModel
 * \brief   This class contains parameters related to
 *          handling group data.
 */
class GroupDataModel: public QAbstractListModel
{
  Q_OBJECT

  /*!
  * @fn       GroupDataModel
  * @param    None
  * @return   None
  * @brief    constructor for GroupDataModel
  */
  explicit GroupDataModel(QObject *parent = 0);

  /*!
  * @fn       ~GroupDataModel
  * @param    None
  * @return   None
  * @brief    virtual destructor for GroupDataModel
  */
  virtual ~ GroupDataModel();

public:

  /*!
  * @enum     GroupDataModelRole
  * @brief    Represents defined role for list model.
  */
  enum GroupDataModelRole
  {
    GroupRole = Qt::UserRole + 51
  };

  /*!
  * @enum     GroupType
  * @brief    Represents group type.
  */
  enum GroupType
  {
    REGULAR_GROUP = 0,
    ROLLUP_GROUP
  };

  /*!
  * @fn       getGroupDataModelInstance
  * @param    None
  * @return   None
  * @brief    get Instance of GroupDataModel
  */
  static GroupDataModel *getGroupDataModelInstance();

  /*!
  * @fn       destroyGroupDataModelInstance
  * @param    None
  * @return   None
  * @brief    destroy Instance of GroupDataModel
  */
  static void destroyGroupDataModelInstance();
  /*!
  * @fn       addGroupData
  * @param    const QString&
  * @return   None
  * @brief    to populate data in list model.
  */
  void addGroupData(const QString &grpData);

  /*!
  * @fn       rowCount
  * @param    const QModelIndex
  * @return   int
  * @brief    returns number of rows.
  */
  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /*!
  * @fn       data
  * @param    const QModelIndex
  * @param    int
  * @return   QVariant
  * @brief    populates list data.
  */
  QVariant data(const QModelIndex &index, int role =  Qt::DisplayRole) const Q_DECL_OVERRIDE;

public slots:
  /*!
  * @fn       onGroupDataReceived
  * @param    QVector
  * @return   None
  * @brief    to populate SupervisorGroups class on receive of group data.
  */
  void onGroupDataReceived(const QVector<SupervisorGroups> &groupData);

protected:
  /*!
  * @fn       roleNames
  * @param    None
  * @return   QHash<int,QByteArray>
  * @brief    gives roleNames.
  */
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
  QList<QString>                         m_groupList;      //!< list of  QString data type
  static GroupDataModel*                 m_sGrpDataModelInstance;       //!< static data member of pointer GroupOverviewDataModel type
};
}
}

#endif // GROUPDATAMODEL_H
