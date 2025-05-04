/*!
* @file     scannerfaulthandler.cpp
* @author   Agiliad
* @brief    This file contains interface, responsible for managing scanner faults. It gets scanner fault and display it on screen.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/faults/scannerfaulthandler.h>
#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       ScannerFaultHandler
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class ScannerFaultHandler.
*/
ScannerFaultHandler::ScannerFaultHandler(QObject* parent): IFaultHandler(parent)
{
    DEBUG_LOG("Creating instance of ScannerFaultData");
    scannerFaultData = new ScannerFaultData();

    DEBUG_LOG("insert scanner fault data");
    scannerFaultData->insertScannerFaultData();

    DEBUG_LOG("get scanner fault data");
    m_scannerfaultinfo = scannerFaultData->getScannerFaultData();

    if(WorkstationConfig::getInstance()->getCurrentLanuage() == "English")
    {
      std::string sFilePath = WorkstationConfig::getInstance()->getexecutablePath() + "/" + SCANNER_FAULT_MESSAGE_FILE_NAME;
      QList<FaultMapXMLDataStruct> scannerFaultList = Utility::updateScannerFaultWarningData(sFilePath);
      for(int i = 0; i < scannerFaultList.count(); i++)
      {
        FaultMapXMLDataStruct updatedFaultStructure= scannerFaultList.at(i);
        ScannerFaultInfo faultInfo = m_scannerfaultinfo.value(updatedFaultStructure.faultid);
        if((faultInfo.useraction != updatedFaultStructure.faultAction)
           || (faultInfo.description != updatedFaultStructure.faultMessage))
        {
          faultInfo.description = updatedFaultStructure.faultMessage;
          faultInfo.useraction = updatedFaultStructure.faultAction.remove("\\n");
          m_scannerfaultinfo.insert(updatedFaultStructure.faultid, faultInfo);
        }
      }
    }

    DEBUG_LOG("Creating instance of ScannerAgentStatus for scanner fault handling");
    m_scanneragentstatus.reset(new analogic::nss::ScannerAgentStatus());
    m_scanneragentstatus->last_fault_message = "";

    connect(NSSAgentAccessInterface::getInstance(), &NSSAgentAccessInterface::scannerFaultReceived,
            this, &ScannerFaultHandler::onScannerFaultReceived);

    m_pScannerStateTimer.reset(new QTimer(this));
    THROW_IF_FAILED((m_pScannerStateTimer.use_count() == 0)?Errors::E_OUTOFMEMORY:Errors::S_OK);
    // setting up for status timer
    int timerinterval = WorkstationConfig::getInstance()->getScannerConnectionCheckInterval();
    INFO_LOG("Scanner connection check interval : "<<timerinterval);
    m_pScannerStateTimer->setInterval(timerinterval);
    m_pScannerStateTimer->setTimerType(Qt::PreciseTimer);
    // connecting timer events to proper slot
    connect(m_pScannerStateTimer.get(), &QTimer::timeout, this, &ScannerFaultHandler::onTimeout, Qt::QueuedConnection);
    m_pScannerStateTimer->start();
    m_scanneragentstatus.reset(new analogic::nss::ScannerAgentStatus());
    m_scanneragentstatus->state = "";


}

/*!
* @fn       logTheCurrentStatusReceived()
* @param    ScannerAgentStatus
* @return   void
* @brief    Logs the current received Scanner Status
*/
void ScannerFaultHandler::logTheCurrentStatusReceived(analogic::nss::ScannerAgentStatus const status )
{
  INFO_LOG( "|" <<status.is_operable
         << "|" <<status.dicos_general_equipment_module.GetOperationMode()
         << "|" <<status.last_fault_code
         << "|" <<status.last_fault_message
         << "|" <<status.last_fault_time
         << "|" <<status.last_image_quality_result
         << "|" <<status.last_image_quality_time
         << "|" <<status.operator_mat_state
         << "|" <<status.xray_on
         << "|" <<status.state);
}


/*!
* @fn       isOperatorMatStateOperatorAbsent()
* @param    ScannerAgentStatus
* @return   bool
* @brief    Checks if operator mat status is changed to ABSENT
*/
bool ScannerFaultHandler::isOperatorMatStateOperatorAbsent(analogic::nss::ScannerAgentStatus const status)
{
  if(status.operator_mat_state != m_scanneragentstatus->operator_mat_state){
    if(analogic::nss::OperatorMatState::OPERATOR_MAT_STATE_OPERATOR_ABSENT == status.operator_mat_state){
      INFO_LOG("Operator Mat state changed to ABSENT : "<<status.operator_mat_state );
      return true;
    }
    else{
      INFO_LOG("Operator Mat change detected to : "<<status.operator_mat_state );
    }
  }
  return false;
}

/*!
* @fn       onScannerFaultReceived()
* @param    ScannerAgentStatusHandler
* @return   None
* @brief    slot will get call on scanner fault received.
*/
void ScannerFaultHandler::onScannerFaultReceived(ScannerAgentStatusHandler scannerstatus)
{
    analogic::nss::ScannerAgentStatus status = scannerstatus.getscannerstatus();
    logTheCurrentStatusReceived(status);
    if(status.state.empty())
    {
      INFO_LOG("Scanner State received as empty, Assuming scanner is disconnected and XRays OFF, The status might contain wrong values.");
      m_scanneragentstatus->state = "";
      m_scanneragentstatus->xray_on = false;  
      return;
    }

    if(isOperatorMatStateOperatorAbsent(status)){
      emit operatorAbsentFromMat();
    }
    m_scanneragentstatus->operator_mat_state = status.operator_mat_state;

    if((m_scanneragentstatus != NULL) &&
            ((status.last_fault_code != m_scanneragentstatus->last_fault_code) ||
             ((status.last_fault_message != m_scanneragentstatus->last_fault_message))||
             (status.last_fault_time != m_scanneragentstatus->last_fault_time) ||
             (status.last_image_quality_result !=
              m_scanneragentstatus->last_image_quality_result)||
             (status.last_image_quality_time !=
              m_scanneragentstatus->last_image_quality_time)||
             (status.state != m_scanneragentstatus->state) ||
             status.xray_on != m_scanneragentstatus->xray_on ||
              status.dicos_general_equipment_module.GetOperationMode() !=
              m_scanneragentstatus->dicos_general_equipment_module.GetOperationMode()))
    {
        m_scanneragentstatus.reset(new analogic::nss::ScannerAgentStatus(status));

        int faultcode = QString::fromStdString(m_scanneragentstatus->last_fault_code).toInt();
        std::string faultMessage = getScannerFaultQpidString(faultcode);

        QString useraction = getScannerFaultUserAction(faultMessage.c_str());
        if (useraction == "")
        {
            useraction = "";
        }

        QString description = getScannerFaultDescription(faultMessage.c_str());
        if (description == "")
        {
            useraction = "";
            description = m_scanneragentstatus->last_fault_message.c_str();
        }

        DEBUG_LOG("Received Scanner Fault");

        QString scannerModeText;
        if(SDICOS::GeneralEquipmentModule::enumOperationMode_Other == status.dicos_general_equipment_module.GetOperationMode()){
          scannerModeText = "Maintenance";
        }
        else{
          scannerModeText = "Scan";
        }
        QString XrayState;
        if (status.xray_on == true)
        {
          XrayState = "ON";
        }
        else {
          XrayState = "OFF";
        }

        QString temp = WorkstationConfig::getInstance()->getScannername() + QString("$") +
                QString(m_scanneragentstatus->state.c_str()) + QString("$")+
                QString(m_scanneragentstatus->last_fault_code.c_str())+ QString("$")+
                useraction + QString("$") +
                QString::number(m_scanneragentstatus->last_fault_time) + QString("$") +
                QString(description) + QString("$") +
                QString("scannerfaultevent")+ QString("$") +
                scannerModeText + QString("$") +
                XrayState;

        INFO_LOG("Updating ScannerAgentStatus with : "<< temp.toStdString());
        INFO_LOG("onScannerFaultReceived : Last Image Quality Time   : " << m_scanneragentstatus->last_image_quality_time);
        INFO_LOG("onScannerFaultReceived : Last Image Quality Result : " << m_scanneragentstatus->last_image_quality_result.c_str());
        INFO_LOG("onScannerFaultReceived : Last Fault Message        : " << m_scanneragentstatus->last_fault_message.c_str());
        emit scannerFaultReceived(temp);
    }

}

/*!
* @fn       getScannerFaultDescription
* @param    QString faultMessage
* @return   QString scanner fault description
* @brief    return scanner fault description.
*/
QString ScannerFaultHandler::getScannerFaultDescription(QString faultMessage)
{
    if (faultMessage != "")
    {
        if(m_scannerfaultinfo.size() == 0)
        {
            return "";
        }
        if (m_scannerfaultinfo.contains(faultMessage))
        {
            struct ScannerFaultInfo scannerfault = m_scannerfaultinfo.value(faultMessage);

            if (scannerfault.description != "")
            {
                return scannerfault.description;
            }
        }
        else
        {
            return "";
        }
    }
    return "";
}

/*!
* @fn       getScannerFaultUserAction
* @param    QString faultMessage
* @return   QString useraction
* @brief    return scanner fault user action.
*/
QString ScannerFaultHandler::getScannerFaultUserAction(QString faultMessage)
{
    if (faultMessage != "")
    {
        if(m_scannerfaultinfo.size() == 0)
        {
            return "";
        }

        if (m_scannerfaultinfo.contains(faultMessage))
        {
            struct ScannerFaultInfo scannerfault = m_scannerfaultinfo.value(faultMessage);

            if (scannerfault.useraction != "")
            {
                return scannerfault.useraction;
            }
        }
        else
        {
            return "";
        }
    }
    return "";
}

/*!
* @fn       getScannerFaultQpidString
* @param    int - faultcode
* @return   QString - scanner fault qpid string
* @brief    return scanner fault qpid string.
*/
std::string ScannerFaultHandler::getScannerFaultQpidString(int faultcode)
{
    if ((faultcode != 0)
             && (analogic::ancp00::BLMFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::BLMFault_FAULT_ID val;
        val = (analogic::ancp00::BLMFault_FAULT_ID)faultcode;
        return analogic::ancp00::BLMFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::BhsFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::BhsFault_FAULT_ID val;
        val = (analogic::ancp00::BhsFault_FAULT_ID)faultcode;
        return analogic::ancp00::BhsFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::CommFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::CommFault_FAULT_ID val;
        val = (analogic::ancp00::CommFault_FAULT_ID)faultcode;
        return analogic::ancp00::CommFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::ComputerFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::ComputerFault_FAULT_ID val;
        val = (analogic::ancp00::ComputerFault_FAULT_ID)faultcode;
        return analogic::ancp00::ComputerFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::DrbFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::DrbFault_FAULT_ID val;
        val = (analogic::ancp00::DrbFault_FAULT_ID)faultcode;
        return analogic::ancp00::DrbFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::Fault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::Fault_FAULT_ID val;
        val = (analogic::ancp00::Fault_FAULT_ID)faultcode;
        return analogic::ancp00::Fault_FAULT_ID_Name(val);
    }
    else if (analogic::ancp00::FaultData_FAULT_LEVEL_IsValid(faultcode))
    {
        analogic::ancp00::FaultData_FAULT_LEVEL val;
        val = (analogic::ancp00::FaultData_FAULT_LEVEL)faultcode;
        return analogic::ancp00::FaultData_FAULT_LEVEL_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::FaultFilterCondition_CONDITION_ID_IsValid(faultcode)))
    {
        analogic::ancp00::FaultFilterCondition_CONDITION_ID val;
        val = (analogic::ancp00::FaultFilterCondition_CONDITION_ID)faultcode;
        return analogic::ancp00::FaultFilterCondition_CONDITION_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::GenericFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::GenericFault_FAULT_ID val;
        val = (analogic::ancp00::GenericFault_FAULT_ID)faultcode;
        return analogic::ancp00::GenericFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::InterlockFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::InterlockFault_FAULT_ID val;
        val = (analogic::ancp00::InterlockFault_FAULT_ID)faultcode;
        return analogic::ancp00::InterlockFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::MotorControlFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::MotorControlFault_FAULT_ID val;
        val = (analogic::ancp00::MotorControlFault_FAULT_ID)faultcode;
        return analogic::ancp00::MotorControlFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::NimFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::NimFault_FAULT_ID val;
        val = (analogic::ancp00::NimFault_FAULT_ID)faultcode;
        return analogic::ancp00::NimFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::PowerlinkFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::PowerlinkFault_FAULT_ID val;
        val = (analogic::ancp00::PowerlinkFault_FAULT_ID)faultcode;
        return analogic::ancp00::PowerlinkFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::ReconFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::ReconFault_FAULT_ID val;
        val = (analogic::ancp00::ReconFault_FAULT_ID)faultcode;
        return analogic::ancp00::ReconFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::SafetyIoFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::SafetyIoFault_FAULT_ID val;
        val = (analogic::ancp00::SafetyIoFault_FAULT_ID)faultcode;
        return analogic::ancp00::SafetyIoFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::TempHumidityFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::TempHumidityFault_FAULT_ID val;
        val = (analogic::ancp00::TempHumidityFault_FAULT_ID)faultcode;
        return analogic::ancp00::TempHumidityFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::UpsFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::UpsFault_FAULT_ID val;
        val = (analogic::ancp00::UpsFault_FAULT_ID)faultcode;
        return analogic::ancp00::UpsFault_FAULT_ID_Name(val);
    }
    else if ((faultcode != 0)
             && (analogic::ancp00::XrayControlFault_FAULT_ID_IsValid(faultcode)))
    {
        analogic::ancp00::XrayControlFault_FAULT_ID val;
        val = (analogic::ancp00::XrayControlFault_FAULT_ID)faultcode;
        return analogic::ancp00::XrayControlFault_FAULT_ID_Name(val);
    }

    else
    {
        return "";
    }
}

/*!
* @fn      onScannerStateChanged
* @param   QMLEnums::ScannerState - state
* @return  None
* @brief   slot call on scanner state changed
*/
void ScannerFaultHandler::onScannerStateChanged(QMLEnums::ScannerState state)
{
    INFO_LOG("setting scanner state to :"<<state);
    NSSAgentAccessInterface::getInstance()->setScannerState(state);
}


/*!
* @fn       ~ScannerFaultHandler
* @param    None
* @return   None
* @brief    Destructor for class ScannerFaultHandler.
*/
ScannerFaultHandler::~ScannerFaultHandler()
{
    TRACE_LOG("");
    SAFE_DELETE(scannerFaultData);
}

/*!
* @fn      onExit
* @param   None
* @return  None
* @brief   slot call on exit
*/
void ScannerFaultHandler::onExit()
{
    TRACE_LOG("");
    m_scanneragentstatus.reset();
}

/*!
* @fn       onTimeout
* @param    None
* @return   None
* @brief    slot will call on timeout
*/
void ScannerFaultHandler::onTimeout()
{
    if (m_scanneragentstatus && m_scanneragentstatus->state == "")
    {
        DEBUG_LOG("Update Network State: " << m_scanneragentstatus);
        emit updateScannerNetwork();
    }
}

}  // end of namespace ws
}  // end of namespace analogic
