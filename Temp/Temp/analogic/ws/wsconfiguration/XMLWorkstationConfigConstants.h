/*!
* @file     XMLWorkstationConfigConstants.h
* @author   tvu
* @brief    This file contains constant related to workstation
* @date     August 14, 2017 
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   xml_workstation_config_constants.h
 * Author: tvu
 *
 * Created on August 14, 2017, 3:12 PM
 */

#ifndef XML_WORKSTATION_CONFIG_CONSTANTS_H
#define XML_WORKSTATION_CONFIG_CONSTANTS_H

namespace analogic
{
namespace ancp00
{
    
// Element Names Constant Strings
#define parent_element_connect "ConnectWorkstationConf" 
#define element_workstation_name "workstation_id"
#define element_workstation_manufacturer "manufacturer"
#define element_workstation_serial_number "workstation_serial_number"
#define element_workstation_manufacturer_model_number "manufacturer_model_number"
#define element_workstation_inactivity_timeout "inactivity_timeout"
       
#define element_scanner_name "scanner_id"

#define element_scanner_admin_server_ip "scanner_admin_server_ip"
#define element_scanner_admin_server_port "scanner_admin_server_port"    

#define element_user_admin_server_ip "user_admin_server_ip"
#define element_user_admin_server_port "user_admin_server_port"            

#define element_nss_server_ip "nss_server_ip"
#define element_nss_server_port "nss_server_port"     
    
#define element_bhs_operation "bhs_operation"
#define element_bhs_bin_type "bhs_bin_type"
#define element_lm_soap_client_endpoint "lm_soap_client_endpoint"    
#define element_sws_soap_server_port "sws_soap_server_port"   

#define element_unpredictable_screening_protocol_percent "unpredictable_screening_protocol_percent"
        
    
}
}

#endif /* XML_WORKSTATION_CONFIG_CONSTANTS_H */

