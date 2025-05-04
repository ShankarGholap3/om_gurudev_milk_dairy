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
#include "supervisoragentsdatamodel.h"

namespace analogic
{
namespace ws
{
bool SupervisorAgentsDataModel::m_sAgentDataInstanceFlag = false;
SupervisorAgentsDataModel* SupervisorAgentsDataModel::m_sAgentDataModelInstance = 0;

AgentsData::AgentsData(const QString &type, const QString &location, const QString &user, const QString &status)
  :m_type(type),m_location(location),m_user(user),m_status(status)
{

}

QString AgentsData::type() const
{
  return m_type;
}

QString AgentsData::location() const
{
  return m_location;
}

QString AgentsData::user() const
{
  return m_user;
}

QString AgentsData::status() const
{
  return m_status;
}

void AgentsData::setType(QString type)
{
  m_type = type;
}

void AgentsData::setLocation(QString location)
{
  m_location = location;
}

void AgentsData::setUser(QString user)
{
  m_user = user;
}

void AgentsData::setstatus(QString status)
{
  m_status = status;
}

/*!
  * @fn       SupervisorAgentsDataModel
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorAgentsDataModel
  */
SupervisorAgentsDataModel::SupervisorAgentsDataModel(QObject *parent)
  : QAbstractTableModel(parent), m_currentAgentType(QMLEnums::SCANNER), m_currentRow(0)
{
  insertEmptyRow();
}

/*!
* @fn       SupervisorAgentsDataModel
* @param    None
* @return   None
* @brief    destructor for SupervisorAgentsDataModel
*/
SupervisorAgentsDataModel::~SupervisorAgentsDataModel()
{

}

/*!
* @fn       columnCount
* @param    const QModelIndex
* @return   int
* @brief    returns number of columns
*/
int SupervisorAgentsDataModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 4; //change according to gauge
}

/*!
* @fn       rowCount
* @param    const QModelIndex
* @return   int
* @brief    returns number of rows.
*/
int SupervisorAgentsDataModel::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return m_agentData.size ();
}

/*!
* @fn       data
* @param    const QModelIndex
* @param    int
* @return   QVariant
* @brief    populates table data.
*/
QVariant SupervisorAgentsDataModel::data(const QModelIndex &index, int role) const
{
  if (index.row() < 0 || index.row() >= m_agentData.count())
  {
    return QVariant();
  }
  const AgentsData &agentData = m_agentData[index.row()];
  if (role == TypeRole)
  {
    return agentData.type();
  }
  else if (role == LocationRole)
  {
    return agentData.location();
  }
  else if (role == UserRole)
  {
    return agentData.user();
  }
  else if (role == StatusRole)
  {
    return agentData.status();
  }
  return QVariant();
}

/*!
* @fn       getSupervisorAgentsDataModelInstance
* @param    None
* @return   SupervisorAgentsDataModel*
* @brief    get instance for SupervisorAgentsDataModel singletone
*/
SupervisorAgentsDataModel *SupervisorAgentsDataModel::getSupervisorAgentsDataModelInstance()
{
  if( m_sAgentDataInstanceFlag == false )
  {
    DEBUG_LOG("Creating instance of SupervisorAgentsDataModel");
    m_sAgentDataModelInstance = new SupervisorAgentsDataModel();
    m_sAgentDataInstanceFlag = true;
    DEBUG_LOG("Created new instance of SupervisorAgentsDataModel" << m_sAgentDataModelInstance);
    return m_sAgentDataModelInstance;
  }
  else
  {
    DEBUG_LOG("Return existing instance of SupervisorAgentsDataModel" << m_sAgentDataModelInstance);
    return m_sAgentDataModelInstance;
  }
}

/*!
* @fn       destroySupervisorAgentsDataModelInstance
* @param    None
* @return   None
* @brief    destroy Instance of SupervisorAgentsDataModel
*/
void SupervisorAgentsDataModel::destroySupervisorAgentsDataModelInstance()
{
  if((m_sAgentDataInstanceFlag == true) && (m_sAgentDataModelInstance != nullptr))
  {
    delete m_sAgentDataModelInstance;
    m_sAgentDataModelInstance = nullptr;
    m_sAgentDataInstanceFlag = false;
  }
}


/*!
* @fn       roleNames
* @param    None
* @return   QHash<int,QByteArray>
* @brief    gives roleNames.
*/
QHash<int, QByteArray> SupervisorAgentsDataModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[TypeRole] = "type";
  roles[LocationRole] = "location";
  roles[UserRole] = "user";
  roles[StatusRole] = "status";
  return roles;
}

/*!
* @fn       onAgentsDataRecieved
* @param    QVector
* @return   None
* @brief    to populate supervisoragentsdata class on recieve of agents data.
*/
void SupervisorAgentsDataModel::onAgentsDataReceived(QVector<SupervisorAgents> agentsData)
{
  m_agentDataResponse = agentsData;
  refreshAgentsData();
}

/*!
* @fn       setCurrentSelectedRow
* @param    const int
* @return   None
* @brief    to populate the index of the current selected row
*/
void SupervisorAgentsDataModel::setCurrentSelectedRow(const int index)
{
  m_currentRow = index;
  refreshAgentsData();
}

/*!
* @fn       getCurrentSelectedRow
* @param    None
* @return   int
* @brief    Returns the index of the current selected row
*/
int SupervisorAgentsDataModel::getCurrentSelectedRow()
{
  return m_currentRow;
}
/*!
* @fn       setCurrentAgentUuid
* @param    const int
* @return   None
* @brief    to set the agent uuid of the current selected row
*/
void SupervisorAgentsDataModel::setCurrentAgentUuid()
{
  if((m_agentData.size() > 0) && (m_currentRow < m_agentData.size()) && (m_currentRow >= 0))
  {
      const AgentsData &agentData = m_agentData[m_currentRow];
      QString selectedMachineId = agentData.type();

      for(auto itr = m_agentDataResponse.begin(); itr != m_agentDataResponse.end() ; itr++)
      {
          SupervisorAgents sAgents = *itr;
          QString machineId =  sAgents.getAgentsMachine_id();
          if(machineId == selectedMachineId)
          {
              m_currentAgentUuid = sAgents.getAgentsUuid();
              m_registered = sAgents.getAgentsRegistered();
              emit currentAgentChanged(m_currentAgentUuid);
              break;
          }
      }
  }
}

/*!
* @fn       getCurrentAgentUuid
* @param    None
* @return   QString
* @brief    Returns the agent uuid of the current selected row
*/
QString SupervisorAgentsDataModel::getCurrentAgentUuid()
{
  return m_currentAgentUuid;
}

/*!
* @fn       getCurrentAgentRegistered
* @param    None
* @return   QString
* @brief    Returns the agent registered state of the current selected row
*/
QString SupervisorAgentsDataModel::getCurrentAgentRegistered()
{
  return m_registered;
}

/*!
* @fn       insertEmptyRow
* @param    None
* @return   None
* @brief    insert Empty row to the table
*/
void SupervisorAgentsDataModel::insertEmptyRow()
{
  // added default row to enable default row selection
  beginInsertRows (QModelIndex(), 0, 0);
  m_agentData.append(AgentsData("", "", "", ""));
  m_currentAgentUuid = "";
  endInsertRows();
}

/*!
* @fn       refreshAgentsData
* @param    None
* @return   None
* @brief    to update/refresh the agents data.
*/
void SupervisorAgentsDataModel::refreshAgentsData() //rename this function
{
  updateAgentsDataTable(); //updating the data table.
  updateAgentsDataPanel();
  setCurrentAgentUuid();
}
/*!
* @fn       updateAgentsDataTable
* @param    None
* @return   None
* @brief    to populate the agents data table on the basis of gauge selected.
*/
void SupervisorAgentsDataModel::updateAgentsDataTable()
{
  beginResetModel();
  m_agentData.clear();
  QVector<SupervisorAgents>::iterator itr;
  if(m_agentDataResponse.size() != 0)
  {
    for(itr = m_agentDataResponse.begin(); itr != m_agentDataResponse.end() ; itr++)
    {
      SupervisorAgents supervisorAgent = *itr;
      QString agentType = supervisorAgent.getAgentsMachine_id();
      QString agentLocation = supervisorAgent.getAgentsLocation();
      QString agentUser = supervisorAgent.getAgentsUser();
      QString agentStatus = getAgentStatus(supervisorAgent.getAgentsRegistered(), supervisorAgent.getAgentsReadiness());
      beginInsertRows (QModelIndex(), 0, (m_agentDataResponse.size()-1));
      m_agentData.append(AgentsData(agentType, agentLocation, agentUser, agentStatus));
      endInsertRows();
    }
  }
  else
  {
    insertEmptyRow();
  }
  endResetModel();
  if(m_currentAgentUuid == "")
  {
    setCurrentAgentUuid();
  }
}

/*!
* @fn       setCurrentAgentType
* @param    QString
* @return   None
* @brief    Set the agent type basis of gauge selected.
*/
void SupervisorAgentsDataModel::setCurrentAgentType(int currentAgentType)
{
  m_currentAgentType = currentAgentType;
  refreshAgentsData();
}

/*!
* @fn       getCurrentAgentType
* @param    None
* @return   QString
* @brief    Set the agent type basis of gauge selected.
*/
int SupervisorAgentsDataModel::getCurrentAgentType()
{
  return m_currentAgentType;
}

/*!
* @fn       updateAgentsDataPanel
* @param    None
* @return   None
* @brief    to populate the agents data panel on the basis of row click of agents data table.
*/
void SupervisorAgentsDataModel::updateAgentsDataPanel()
{
  QVector<SupervisorAgents>::iterator itr;
  int agentType = -1;
  QString panelTitle = "";
  QString networkTileStatus = "";
  m_agentDataTile.clear();
  if(!m_agentData.isEmpty())
  {
    if(m_agentData.size () > m_currentRow && m_currentRow!=-1)
    {
      const AgentsData &agentData = m_agentData[m_currentRow];
      QString selectedMachineId = agentData.type();

      for(itr = m_agentDataResponse.begin(); itr != m_agentDataResponse.end() ; itr++)
      {
        SupervisorAgents sAgents = *itr;
        QString machineId = sAgents.getAgentsMachine_id();
        panelTitle = sAgents.getDetailMachine_id();
        panelTitle += (((sAgents.getDetailLocation() != "") && (panelTitle != "")) ? " - " : "");
        panelTitle += sAgents.getDetailLocation();
        panelTitle += (((sAgents.getAgentsSublocation() != "") && (panelTitle != "")) ? " - " : "");
        panelTitle += sAgents.getAgentsSublocation();
        if(machineId == selectedMachineId)
        {
          agentType = sAgents.getAgentsType();
          networkTileStatus = getNetworkStatus(sAgents.getAgentsReadiness());
          if(agentType == QMLEnums::SCANNER)
          {
            updateScannerAgentsData(sAgents);
          }
          else if(agentType == QMLEnums::PVS)
          {
            updatePVSAgentsData(sAgents);
          }
          else if(agentType == QMLEnums::SVS)
          {
            updateSVSAgentsData(sAgents);
          }
          else
            INFO_LOG("Failed to select proper agent type");
          break;
        }
        else
        {
          INFO_LOG("No Machine Id matched");
        }
      }
      if(m_agentDataTile.size() != 0)
      {
        emit agentTileDataReceived(m_agentDataTile,panelTitle,networkTileStatus);
      }
      else
      {
        emit agentTileDataReceived(m_agentDataTile,"","");
        INFO_LOG("Failed to collect proper data of agent type due to connection failure or some error in network connection");
      }
    }
  }
}

/*!
* @fn       updateScannerAgentsData
* @param    reference
* @return   None
* @brief    to append the scanner agent data on the basis of Scanner agent type.
*/
void SupervisorAgentsDataModel::updateScannerAgentsData(const SupervisorAgents &sAgents)
{
  m_agentDataTile.append(getAgentStatus(sAgents.getAgentsRegistered(), sAgents.getAgentsReadiness()));
  m_agentDataTile.append(sAgents.getUtilization());
  m_agentDataTile.append(sAgents.getOnline_time());
  m_agentDataTile.append(sAgents.getTrays_screened());
  m_agentDataTile.append(sAgents.getUnlink_rate());
  m_agentDataTile.append(sAgents.getThroughput());
}

/*!
* @fn       updatePVSAgentsData
* @param    reference
* @return   None
* @brief    to append the PVS agent data on the basis of PVS agent type.
*/
void SupervisorAgentsDataModel::updatePVSAgentsData(const SupervisorAgents &sAgents)
{
  m_agentDataTile.append(getAgentStatus(sAgents.getAgentsRegistered(), sAgents.getAgentsReadiness()));
  m_agentDataTile.append(sAgents.getUtilization());
  m_agentDataTile.append(sAgents.getOnline_time());
  m_agentDataTile.append(sAgents.getTrays_searched());
  m_agentDataTile.append(sAgents.getAvg_decision_time());
  m_agentDataTile.append(sAgents.getReject_rate());
}

/*!
* @fn       updateSVSAgentsData
* @param    reference
* @return   None
* @brief    to append the SVS agent data on the basis of SVS agent type.
*/
void SupervisorAgentsDataModel::updateSVSAgentsData(const SupervisorAgents &sAgents)
{
  m_agentDataTile.append(getAgentStatus(sAgents.getAgentsRegistered(), sAgents.getAgentsReadiness()));
  m_agentDataTile.append(sAgents.getUtilization());
  m_agentDataTile.append(sAgents.getOnline_time());
  m_agentDataTile.append(sAgents.getTrays_searched());
  m_agentDataTile.append(sAgents.getAvg_decision_time());
}

/*!
 * @fn       getUtilizationData
 * @param    reference
 * @return   None
 * @brief    to get the Utilization data of Scanner, PVS and SVS.
 */
void SupervisorAgentsDataModel::getUtilizationData(int agentType)
{
   float utilizationData = 0.0f;
  if(agentType == QMLEnums::PVS)
  {
    utilizationData = (m_pvsUtilization*100);
    emit agentsUtilizationData(utilizationData, QMLEnums::PVS);
  }
  else if(agentType == QMLEnums::SVS)
  {
    utilizationData = (m_svsUtilization*100);
    emit agentsUtilizationData(utilizationData, QMLEnums::SVS);
  }
  else if(agentType == QMLEnums::SCANNER)
  {
    utilizationData = (m_scannerUtilization*100);
    emit agentsUtilizationData(utilizationData, QMLEnums::SCANNER);
  }
  else //Reset the gauge when tomcat is stopped
  {
    emit agentsUtilizationData(utilizationData, QMLEnums::PVS);
    emit agentsUtilizationData(utilizationData, QMLEnums::SVS);
    emit agentsUtilizationData(utilizationData, QMLEnums::SCANNER);
  }
}

/*!
 * @fn       getGroupName
 * @param    None
 * @return   QString
 * @brief    to get the group name.
 */
QString SupervisorAgentsDataModel::getGroupName()
{
  return m_groupName;
}

/*!
 * @fn       setGroupName
 * @param    QString groupName
 * @return   None
 * @brief    to set the group name.
 */
void SupervisorAgentsDataModel::setGroupName(QString groupName)
{
  m_groupName = groupName;
}

/*!
* @fn       getAgentStatus
* @param    bool, QString
* @return   QString
* @brief    generates agent's status
*/
QString SupervisorAgentsDataModel::getAgentStatus(bool registered, int readiness)
{
  QString status;
  if(registered == true)
  {
    if(readiness)
    {
      status = "READY";  //if readiness is 1 , then status is "READY"
    }
    else
    {
      status = "ONLINE"; //if readiness is 0 , then status is "ONLINE"
    }
  }
  else
  {
    status = "OFFLINE";
  }

  return status;
}

/*!
* @fn       getNetworkStatus
* @param    int
* @return   QString
* @brief    generates agent's status
*/
QString SupervisorAgentsDataModel::getNetworkStatus(int readiness)
{
  QString networkStatus;
  if(readiness)
    networkStatus = "Healthy";  //if readiness is 1 , then  is networkStatus "Healthy"
  else
    networkStatus = "Degraded"; //if readiness is 0 , then  is networkStatus "Degraded"

  return networkStatus;
}

/*!
 * @fn       setScannerUtilization
 * @param    float data
 * @return   None
 * @brief    to set the Scanner Utilization
 */
void SupervisorAgentsDataModel::setScannerUtilization(float data)
{
  m_scannerUtilization = data;
}

/*!
  * @fn       setPvsUtilization
  * @param    float data
  * @return   None
  * @brief    to set Pvs Utilization.
  */
void SupervisorAgentsDataModel::setPvsUtilization(float data)
{
  m_pvsUtilization = data;
}

/*!
   * @fn       setSvsUtilization
   * @param    float data
   * @return   None
   * @brief    to set Svs Utilization.
   */
void SupervisorAgentsDataModel::setSvsUtilization(float data)
{
  m_svsUtilization = data;
}

}
}

