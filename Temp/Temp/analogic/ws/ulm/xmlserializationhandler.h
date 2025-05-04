/*!
* @file     xmlserializationhandler.h
* @author   Agiliad
* @brief    This is header for load and save xml configs
* @date     Act, 04 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#ifndef ANALOGIC_WS_ULM_XMLSERIALIZATIONHANDLER_H_
#define ANALOGIC_WS_ULM_XMLSERIALIZATIONHANDLER_H_

#include <Logger.h>
#include <analogic/ws/ulm/wsscreendao.h>
#include <analogic/ws/ulm/accessdefdao.h>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{
namespace ws
{

/*!
 * \class   XmlSeriliazationHandler
 * \brief   This class handles the xml serialization load and save methods.
 * \note    Need to find the efficient place to set these properties,
 *          BOOST_CLASS_IMPLEMENTATION(WorkStationUIElementDAO, boost::serialization::object_serializable )
 *          BOOST_CLASS_TRACKING(WorkStationUIElementDAO, boost::serialization::track_never )
 *          BOOST_CLASS_IMPLEMENTATION(ScreenElement, boost::serialization::object_serializable )
 *          BOOST_CLASS_TRACKING(ScreenElement, boost::serialization::track_never )
 *          BOOST_CLASS_IMPLEMENTATION(ScreenState, boost::serialization::object_serializable )
 *          BOOST_CLASS_TRACKING(ScreenState, boost::serialization::track_never )
*/
class XmlSeriliazationHandler
{
public:
    /*!
   * @fn      loadAccessElementConfig
   * @param   const std::string& file_name
   * @return  AccessElementDao
   * @brief   This function deserializes config xml and load objects.
   */
    AccessElementDao loadAccessElementConfig(const std::string& file_name);


    /*!
   * @fn      loadWSdaoConfig
   * @param   const std::string& - file_name
   * @return  WorkStationUIElementDAO
   * @brief   This function deserializes config xml and load objects.
   */
    WorkStationUIElementDAO loadWSdaoConfig(const std::string& file_name);
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_ULM_XMLSERIALIZATIONHANDLER_H_

