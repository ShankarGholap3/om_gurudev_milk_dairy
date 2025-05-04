/*!
* @file     xmlserializationhandler.cpp
* @author   Agiliad
* @brief    This file have implementation of methods to load and save xml configs
* @date     Act, 04 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/ulm/xmlserializationhandler.h>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <analogic/ws/ulm/wsscreendao.h>
#include <analogic/ws/common.h>
#include <fstream>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * @fn      loadWSdaoConfig
 * @param   const std::string& - file_name
 * @return  WorkStationUIElementDAO
 * @brief   This function deserializes config xml and load objects.
 */
WorkStationUIElementDAO XmlSeriliazationHandler::loadWSdaoConfig(const std::string& file_name)
{
    DEBUG_LOG("Loading ULM configuration for workstation from : " << file_name);
    unsigned int flags = boost::archive::no_tracking ||
            boost::archive::no_header
            ||  boost::archive::no_codecvt ||
            boost::archive::no_xml_tag_checking;
    std::ifstream ifs(file_name);
    WorkStationUIElementDAO sw;
    try{
        boost::archive::xml_iarchive xml(ifs , flags );
        xml >> boost::serialization::make_nvp("wsElement", sw);
    }
    catch(...)
    {
        ERROR_LOG("XML tags count mismatch , please verify xml");
    }

    return sw;
}

/*!
* @fn      loadAccessElementConfig
* @param   const std::string& - file_name
* @return  AccessElementDao
* @brief   This function deserializes config xml and load objects.
*/
AccessElementDao XmlSeriliazationHandler::loadAccessElementConfig(const std::string& file_name)
{
    DEBUG_LOG("Loading ULM configuration for Access Element from : " << file_name);
    unsigned int flags = boost::archive::no_tracking ||
            boost::archive::no_header
            ||  boost::archive::no_codecvt ||
            boost::archive::no_xml_tag_checking;
    std::ifstream ifs(file_name);
    AccessElementDao sw;
    if( !ifs ) {
        ERROR_LOG("Failed to open file: "<< file_name);
        return sw;
    }
    boost::archive::xml_iarchive xml(ifs , flags );
    xml >> boost::serialization::make_nvp("accessLevel", sw);
    return sw;
}
}  // end of namespace ws
}  // end of namespace analogic
