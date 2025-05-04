/*!
* @file     SupervisorAgentsDataModeldatamodel.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           populating agents data on UI.
* @date     Apr, 28 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISOREVENTSDATAMODEL_H
#define SUPERVISOREVENTSDATAMODEL_H

#include <QAbstractTableModel>
#include <QTimer>
#include "supervisorrecentevents.h"

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

class EventsData
{
public:
  EventsData(const QString &eventType, const QString &createdTime);
  //![0]

  QString eventType() const;
  QString createdTime() const;

  void setEventType(QString eventType);
  void setCreatedTime(QString createdTime);

private:
  QString m_eventType;
  QString m_createdTime;
};

/*!
 * \class   SupervisorEventsDataModel
 * \brief   This class contains parameters related to
 *          handling agents data.
 */
class SupervisorEventsDataModel: public QAbstractTableModel
{
  Q_OBJECT

  /*!
  * @fn       SupervisorEventsDataModel
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorEventsDataModel
  */
  explicit SupervisorEventsDataModel(QObject *parent = 0);

  /*!
  * @fn       SupervisorEventsDataModel
  * @param    None
  * @return   None
  * @brief    destructor for SupervisorEventsDataModel
  */
  virtual ~SupervisorEventsDataModel();

public:

  /*!
  * @enum     SupervisorEventsDataModelRole
  * @brief    Represents defined role for table view column.
  */
  enum SupervisorEventsDataModelRole
  {
    EventTypeRole = Qt::UserRole + 48,
    CreatedTimeRole,
  };

  /*!
  * @fn       getSupervisorEventsDataModelInstance
  * @param    None
  * @return   None
  * @brief    get Instance of EventsDataModelInstance
  */
  static SupervisorEventsDataModel *getSupervisorEventsDataModelInstance();

  /*!
  * @fn       destroySupervisorEventsDataModel
  * @param    None
  * @return   None
  * @brief    destroy Instance of EventsDataModelInstance
  */
  void destroySupervisorEventsDataModel();

  /*!
  * @fn       addEventsData
  * @param    AgentsData
  * @return   None
  * @brief    to populate agentsData list.
  */
  void addEventsData(const EventsData &eventsData);

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
  * @return   int
  * @brief    returns number of rows.
  */
  int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;


  /*!
  * @fn       data
  * @param    const QModelIndex
  * @param    int
  * @return   QVariant
  * @brief    populates table data.
  */
  QVariant data(const QModelIndex &index, int role =  Qt::DisplayRole) const Q_DECL_OVERRIDE;

public slots:


  /*!
    * @fn       onEventsDataRecieved
    * @param    QVector
    * @return   None
    * @brief    to populate supervisoreventsdata class on recieve of events data.
    */
  void onRecenteventsdatarecieved(QVector<SupervisorRecentEvents> eventsData);


protected:

  /*!
    * @fn       roleNames
    * @param    None
    * @return   QHash<int,QByteArray>
    * @brief    gives roleNames.
    */
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
  QList<EventsData>                 m_eventsData;      //!< agent data type
  QTimer                            m_timer;
  QStringList                       m_stringlist;
  static SupervisorEventsDataModel* m_supEvntModelInstance;

};
}
}
#endif // SUPERVISOREVENTSDATAMODEL_H
