/*!
* @file     supervisoragentsdatamodel.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           populating agents data on UI.
* @date     Apr, 28 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QDebug>
#include "analogic/ws/common.h"
#include "supervisoreventsdatamodel.h"

namespace analogic
{
namespace ws
{
SupervisorEventsDataModel* SupervisorEventsDataModel::m_supEvntModelInstance = 0;
EventsData::EventsData(const QString &eventType, const QString &createdTime)
  :m_eventType(eventType),m_createdTime(createdTime)
{

}

QString EventsData::eventType() const
{
  return m_eventType;
}

QString EventsData::createdTime() const
{    
  return m_createdTime;
}
void EventsData::setEventType(QString eventType)
{
  m_eventType = eventType;
}

void EventsData::setCreatedTime(QString createdTime)
{
  m_createdTime = createdTime;
}

/*!
  * @fn       SupervisorEventsDataModel
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorEventsDataModel
  */
SupervisorEventsDataModel::SupervisorEventsDataModel(QObject *parent)
  : QAbstractTableModel(parent)
{

}

/*!
* @fn       SupervisorEventsDataModel
* @param    None
* @return   None
* @brief    destructor for SupervisorEventsDataModel
*/
SupervisorEventsDataModel:: ~SupervisorEventsDataModel()
{

}


SupervisorEventsDataModel* SupervisorEventsDataModel::getSupervisorEventsDataModelInstance()
{
  if(!m_supEvntModelInstance)
  {
    m_supEvntModelInstance = new SupervisorEventsDataModel();
  }
  return m_supEvntModelInstance;
}

/*!
* @fn       destroySupervisorEventsDataModel
* @param    None
* @return   None
* @brief    destroy Instance of SupervisorEventsDataModel
*/
void SupervisorEventsDataModel::destroySupervisorEventsDataModel()
{
  if(m_supEvntModelInstance != nullptr)
  {
    delete m_supEvntModelInstance;
    m_supEvntModelInstance = nullptr;
  }
}

void SupervisorEventsDataModel::addEventsData(const EventsData &eventsData)
{
  beginInsertRows (QModelIndex(), rowCount(), rowCount());
  m_eventsData.append (eventsData);
  endInsertRows();
}

int SupervisorEventsDataModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 1;
}

int SupervisorEventsDataModel::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return m_eventsData.size();
}

/*!
* @fn       data
* @param    const QModelIndex
* @param    int
* @return   QVariant
* @brief    populates table data.
*/
QVariant SupervisorEventsDataModel::data(const QModelIndex &index, int role) const
{
  if (index.row() < 0 || index.row() >= m_eventsData.count())
  {
    return QVariant();
  }
  const EventsData &eventsData = m_eventsData[index.row()];

  if (role == EventTypeRole)
  {
    return "Event:"+eventsData.eventType() + " Observed "+eventsData.createdTime();
  }
  else
  {
    Q_UNUSED(role);
  }

  return QVariant();
}

/*!
* @fn       roleNames
* @param    None
* @return   QHash<int,QByteArray>
* @brief    gives roleNames.
*/
QHash<int, QByteArray> SupervisorEventsDataModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[EventTypeRole] = "eventType";
  roles[CreatedTimeRole] = "createdTime";
  return roles;
}

/*!
* @fn       onAgentsDataRecieved
* @param    QVector
* @return   None
* @brief    to populate supervisoragentsdata class on recieve of agents data.
*/
void SupervisorEventsDataModel::onRecenteventsdatarecieved(QVector<SupervisorRecentEvents> eventsData)
{
  beginResetModel ();
  m_eventsData.clear ();

  QVector<SupervisorRecentEvents>::iterator itr;
  if(eventsData.size() != 0)
  {
    for(itr = eventsData.begin(); itr != eventsData.end(); itr++)
    {
      SupervisorRecentEvents events = *itr;
      QString eventName = events.getRecentEventsEventName();
      QString creatTime = events.getRecentEventsCreatedTime();
      addEventsData(EventsData(eventName,creatTime));
    }
  }
  endResetModel ();
}
}
}

