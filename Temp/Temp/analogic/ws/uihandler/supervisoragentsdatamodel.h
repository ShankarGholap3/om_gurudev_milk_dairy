/*!
* @file     SupervisorAgentsDataModeldatamodel.cpp
* @author   Agiliad
* @brief    This file contains functions and parameters related to
*           populating agents data on UI.
* @date     Apr, 28 2022
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef SUPERVISORAGENTSDATAMODEL_H
#define SUPERVISORAGENTSDATAMODEL_H

#include <QAbstractTableModel>
#include <QTimer>
#include "supervisoragents.h"
#include <analogic/ws/uihandler/modelmanagers.h>
#include <analogic/ws/uihandler/supervisorscannerutilization.h>
#include <analogic/ws/uihandler/supervisorpvsutilization.h>
#include <analogic/ws/uihandler/supervisorsvsutilization.h>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

class AgentsData
{
public:
  AgentsData(const QString &type, const QString &location,const QString &user, const QString &status);
  //![0]

  QString type() const;
  QString location() const;
  QString user() const;
  QString status() const;

  void setType(QString type);
  void setLocation(QString location);
  void setUser(QString user);
  void setstatus(QString status);

private:
  QString m_type;
  QString m_location;
  QString m_user;
  QString m_status;
};

/*!
 * \class   SupervisorAgentsDataModel
 * \brief   This class contains parameters related to
 *          handling agents data.
 */
class SupervisorAgentsDataModel: public QAbstractTableModel
{
  Q_OBJECT

  /*!
  * @fn       SupervisorAgentsDataModel
  * @param    None
  * @return   None
  * @brief    constructor for SupervisorAgentsDataModel
  */
  SupervisorAgentsDataModel(QObject *parent = 0);

  /*!
  * @fn       SupervisorAgentsDataModel
  * @param    None
  * @return   None
  * @brief    destructor for SupervisorAgentsDataModel
  */
  virtual ~SupervisorAgentsDataModel();

public:

  /*!
  * @enum     SupervisorAgentsDataRole
  * @brief    Represents defined role for table view column.
  */
  enum SupervisorAgentsDataRoles
  {
    TypeRole = Qt::UserRole + 40,
    LocationRole,
    UserRole,
    StatusRole
  };

  /*!
  * @fn       columnCount
  * @param    const QModelIndex
  * @return   int
  * @brief    returns number of columns
  */
  int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /*!
  * @fn       rowCount
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

  /*!
  * @fn       getSupervisorAgentsDataModelInstance
  * @param    None
  * @return   SupervisorAgentsDataModel*
  * @brief    get instance for SupervisorAgentsDataModel singletone
  */
  static SupervisorAgentsDataModel* getSupervisorAgentsDataModelInstance();

  /*!
  * @fn       destroySupervisorAgentsDataModelInstance
  * @param    None
  * @return   None
  * @brief    destroy Instance of SupervisorAgentsDataModel
  */
  static void destroySupervisorAgentsDataModelInstance();

  /*!
  * @fn       updateAgentsDataPanel
  * @param    None
  * @return   None
  * @brief    to populate the agents data panel on the basis of row click of agents data table.
  */
  void updateAgentsDataPanel();

  /*!
  * @fn       updateScannerAgentsData
  * @param    reference
  * @return   None
  * @brief    to append the scanner agent data on the basis of Scanner agent type.
  */
  void updateScannerAgentsData(const SupervisorAgents &sAgents);

  /*!
  * @fn       updatePVSAgentsData
  * @param    reference
  * @return   None
  * @brief    to append the PVS agent data on the basis of PVS agent type.
  */
  void updatePVSAgentsData(const SupervisorAgents &sAgents);

  /*!
  * @fn       updateSVSAgentsData
  * @param    reference
  * @return   None
  * @brief    to append the SVS agent data on the basis of SVS agent type.
  */
  void updateSVSAgentsData(const SupervisorAgents &sAgents);  

  /*!
   * @fn       getUtilizationData
   * @param    reference
   * @return   None
   * @brief    to get the Utilization data of Scanner, PVS and SVS.
   */
  void getUtilizationData(int agentType);

  /*!
   * @fn       getGroupName
   * @param    None
   * @return   QString
   * @brief    to get the group name.
   */
   QString getGroupName();

  /*!
   * @fn       setGroupName
   * @param    QString groupName
   * @return   None
   * @brief    to set the group name.
   */
   void setGroupName(QString groupName);

   /*!
   * @fn       getNetworkStatus
   * @param    int
   * @return   QString
   * @brief    generates agent's status
   */
   QString getNetworkStatus(int readiness);
  /*!
   * @fn       setScannerUtilization
   * @param    float data
   * @return   None
   * @brief    to set the Scanner Utilization
   */
   void setScannerUtilization(float data);

   /*!
    * @fn       setPvsUtilization
    * @param    float data
    * @return   None
    * @brief    to set Pvs Utilization.
    */
    void setPvsUtilization(float data);

    /*!
     * @fn       setSvsUtilization
     * @param    float data
     * @return   None
     * @brief    to set Svs Utilization.
     */
     void setSvsUtilization(float data);

    /*!
      * @fn       setCurrentAgentUuid
      * @param    const int
      * @return   None
      * @brief    to set the agent uuid of the current selected row
      */
      void setCurrentAgentUuid();

      /*!
      * @fn       getCurrentAgentUuid
      * @param    None
      * @return   QString
      * @brief    Returns the agent uuid of the current selected row
      */
      QString getCurrentAgentUuid();

      /*!
      * @fn       getCurrentAgentRegistered
      * @param    None
      * @return   QString
      * @brief    Returns the agent registered state of the current selected row
      */
      QString getCurrentAgentRegistered();

      /*!
      * @fn       insertEmptyRow
      * @param    const int
      * @return   None
      * @brief    inserts empty row to the table
      */
      void insertEmptyRow();

signals:

   /*!
   * @fn       updateAgentsDataPanel for Agent Type
   * @param    QVariantList
   * @return   None
   * @brief    to populate the agents data panel on the basis emitting signal by collecting Agent Type tile data.
   */
   void agentTileDataReceived(QVariantList tileData,QString panelTitle,QString networkTileStatus);

   /*!
   * @fn       updateAgentsDataPanel for Agent Type
   * @param    QVariantList
   * @return   None
   * @brief    to populate the agents data panel on the basis emitting signal by collecting Agent Type tile data.
   */
   void agentTableDataReceived();

   /*!
    * @fn      agentsUtilizationData
    * @param   float scannerUtilizationValue
    * @param   float pvsUtilizationValue
    * @param   float svsUtilizationValue
    * @param   int agentType
    * @return  None
    * @brief   to populate Utilization data on the AnalogicGaugeControl
    */
   void agentsUtilizationData(float utilizationValue,int agentType);

   /*!
    * @fn      agentDataTypeChanged
    * @param   QString agentType
    * @return  None
    * @brief   to notify change in AnalogicGauge Selection
    */
   void agentDataTypeChanged(QString agentType);
   /*!
    * @fn      currentAgentChanged
    * @param   QString
    * @return  None
    * @brief   to populate current agent uuid
    */
   void currentAgentChanged(QString agentUuid);

public slots:

  /*!
    * @fn       onAgentsDataRecieved
    * @param    QVector
    * @return   None
    * @brief    to populate supervisoragentsdata class on recieve of agents data.
    */
   void onAgentsDataReceived(QVector<SupervisorAgents> agentsData);

  /*!
  * @fn       setCurrentSelectedRow
  * @param    const int
  * @return   None
  * @brief    to populate the index of the current selected row
  */
  Q_INVOKABLE void setCurrentSelectedRow(const int index);

  /*!
  * @fn       getCurrentSelectedRow
  * @param    None
  * @return   int
  * @brief    Returns the index of the current selected row
  */
  Q_INVOKABLE int getCurrentSelectedRow();

  /*!
  * @fn       updateAgentsDataTable
  * @param    None
  * @return   None
  * @brief    to populate the agents data table on the basis of gauge selected.
  */
  Q_INVOKABLE void updateAgentsDataTable();

  /*!
  * @fn       refreshAgentsData
  * @param    None
  * @return   None
  * @brief    to update/refresh the agents data.
  */
  void refreshAgentsData();

  /*!
  * @fn       setCurrentAgentType
  * @param    QString
  * @return   None
  * @brief    Set the agent type basis of gauge selected.
  */
  Q_INVOKABLE void setCurrentAgentType(int currentAgentType);

  /*!
  * @fn       getCurrentAgentType
  * @param    None
  * @return   QString
  * @brief    Set the agent type basis of gauge selected.
  */
  Q_INVOKABLE int getCurrentAgentType();

  /*!
  * @fn       getAgentStatus
  * @param    bool, QString
  * @return   QString
  * @brief    generates agent's status
  */
  QString getAgentStatus(bool registered, int readiness);

protected:

  /*!
    * @fn       roleNames
    * @param    None
    * @return   QHash<int,QByteArray>
    * @brief    gives roleNames.
    */
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
  QList<AgentsData>                 m_agentData;            //!< agent data type
  QVector<SupervisorAgents>         m_agentDataResponse;    //!< agent Data response from server
  static bool                       m_sAgentDataInstanceFlag;     //!< Instance flag to monitor life of instance
  static SupervisorAgentsDataModel* m_sAgentDataModelInstance;  //!< Static instance of ULM
  int                               m_currentAgentType;     //!< holds the value of Agent Type data to be displayed.
  int                               m_currentRow;           //!< holds value of current selected row index
  QVariantList                      m_agentDataTile;  
  boost::shared_ptr<ModelManagers>  m_modelManagers;        //!< handle to model managers
  QString                           m_groupName;            //!< holds value of group name
  float                             m_scannerUtilization;   //!< holds scanner utilization data
  float                             m_pvsUtilization;       //!< holds pvs utilization data
  float                             m_svsUtilization;       //!< holds svs utilization data
  QString                           m_currentAgentUuid;     //!< hold a current agent uuid
  QString                           m_registered;           //!< hold the current agent registered state

};
}
}
#endif // SUPERVISORAGENTSDATAMODEL_H
