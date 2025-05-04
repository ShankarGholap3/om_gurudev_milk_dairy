
/*!
* @file     scanneragentstatushandler.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to scanner agent
*           status handler.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/accesslayer/scanneragentstatushandler.h>
#include <analogic/ws/common.h>

// namespace analogic {
// namespace ws {



/*!
* @fn       ScannerAgentStatusHandler
* @param    QObject* parent
* @return   None
* @brief    constructor for ScannerAgentStatusHandler
*/
ScannerAgentStatusHandler::ScannerAgentStatusHandler(QObject* parent):QObject(parent)
{
    TRACE_LOG("")
}
/*!
* @fn       ScannerAgentStatusHandler
* @param    const ScannerAgentStatusHandler& st
* @return   None
* @brief    constructor for ScannerAgentStatusHandler
*/
ScannerAgentStatusHandler::ScannerAgentStatusHandler(const ScannerAgentStatusHandler& st):QObject(NULL)
{
    TRACE_LOG("")
    m_status.state = st.m_status.state;
    m_status.last_fault_code = st.m_status.last_fault_code;
    m_status.last_image_quality_time = st.m_status.last_image_quality_time;
    m_status.last_image_quality_result = st.m_status.last_image_quality_result;
    m_status.last_fault_time = st.m_status.last_fault_time;
    m_status.last_fault_message = st.m_status.last_fault_message;
    m_status.is_operable = st.m_status.is_operable;
    m_status.operator_mat_state = st.m_status.operator_mat_state;
    m_status.dicos_general_equipment_module = st.m_status.dicos_general_equipment_module;
    m_status.xray_on = st.m_status.xray_on;
}
/*!
* @fn       getscannerstatus
* @param    None
* @return   analogic::nss::ScannerAgentStatus
* @brief    gets scanner status
*/
analogic::nss::ScannerAgentStatus ScannerAgentStatusHandler::getscannerstatus()
{
    TRACE_LOG("")
    return m_status;
}

/*!
* @fn       ScannerAgentStatusHandler
* @param    const analogic::nss::ScannerAgentStatus&
* @return   None
* @brief    constructor for ScannerAgentStatusHandler
*/
ScannerAgentStatusHandler::ScannerAgentStatusHandler(const analogic::nss::ScannerAgentStatus& st)
{
    TRACE_LOG("")
    m_status.state = st.state;
    m_status.last_image_quality_time = st.last_image_quality_time;
    m_status.last_image_quality_result = st.last_image_quality_result;
    m_status.last_fault_code = st.last_fault_code;
    m_status.last_fault_time = st.last_fault_time;
    m_status.last_fault_message = st.last_fault_message;
    m_status.is_operable = st.is_operable;
    m_status.operator_mat_state = st.operator_mat_state;
    m_status.dicos_general_equipment_module = st.dicos_general_equipment_module;
    m_status.xray_on = st.xray_on;
}
/*!
* @fn       operator =
* @param    const analogic::nss::ScannerAgentStatus&
* @return   None
* @brief    assignment operator for ScannerAgentStatusHandler
*/
const ScannerAgentStatusHandler& ScannerAgentStatusHandler::operator = (const analogic::nss::ScannerAgentStatus& st)
{
    TRACE_LOG("")
    m_status.state = st.state;
    m_status.last_image_quality_time = st.last_image_quality_time;
    m_status.last_image_quality_result = st.last_image_quality_result;
    m_status.last_fault_code = st.last_fault_code;
    m_status.last_fault_time = st.last_fault_time;
    m_status.last_fault_message = st.last_fault_message;
    m_status.is_operable = st.is_operable;
    m_status.operator_mat_state = st.operator_mat_state;
    m_status.dicos_general_equipment_module = st.dicos_general_equipment_module;
    m_status.xray_on = st.xray_on;
    return *this;
}
/*!
* @fn       ~ScannerAgentStatusHandler
* @param    None
* @return   None
* @brief    Destructor for ScannerAgentStatusHandler
*/
ScannerAgentStatusHandler::~ScannerAgentStatusHandler()
{
}

/*!
* @fn       setscannerstatus
* @param    analogic::nss::ScannerAgentStatus
* @return   None
* @brief    sets scanner status
*/
void ScannerAgentStatusHandler::setscannerstatus(analogic::nss::ScannerAgentStatus st)
{
    TRACE_LOG("")
    m_status = st;
}
//}
//}
