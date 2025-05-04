/*!
* @file     accessdefdao.h
* @author   Agiliad
* @brief    This file contains structures and maps for the UI configurations.
*           This is a data access object generated from UI configuration files.
*           Those classes which needs the serialization suport should implement
*           serialize method of friend class seriliazation::access.
* @date     Oct, 04 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_ULM_ACCESSDEFDAO_H_
#define ANALOGIC_WS_ULM_ACCESSDEFDAO_H_

#include <map>
#include <list>
#include <vector>
#include <string>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <analogic/ws/common.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   UIComponentAccess
 * \brief   UIComponentAccess is data object to represent UI component properties
 *          like featureid , object Name
 * \note    this class will be member of stl containers , if sort/compare operations
 *          are to be done then this class should have the copy constructor and = operator
 *          overloaded.
 *
 */
class UIComponentAccess
{
public:
    /*!
    * @fn       UIComponentAccess
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    UIComponentAccess(){}

    /*!
    * @fn       UIComponentAccess
    * @param    int - fID
    * @param    std::string - objName
    * @return   None
    * @brief    parameterised constructor to populate objects without deserialization.
    */
    UIComponentAccess(int fID , std::string objName):m_featureID(fID), m_objectName (objName) {}

private:
    friend class boost::serialization::access;
    template<class archive>

    /*!
     * @fn      serialize
     * @param   archive& - ar
     * @param   const unsigned int - version
     * @return  None
     * @brief   This function implements the serialization of the class object.Those
     *           class members which you want to include in serilaized xml archive
     *          should have make_nvp mapping.
     *          The auto serialization of the member varible is skipped here to
     *          avoid the count and class tags.
     */
    void serialize(archive& ar, const unsigned int version)
    {
        Q_UNUSED(version);
        TRACE_LOG("");
        using boost::serialization::make_nvp;
        ar & make_nvp("FeatureID", m_featureID);
        ar & make_nvp("ObjectName" , m_objectName);
    }

public:
    int            m_featureID;     //!< feature ID
    std::string    m_objectName;    //!< UI component object Name
};

typedef std::vector<UIComponentAccess>    ScreenList;    //!< Vector of objectName and UIComponent structure

/*!
 * \class   ScreenElementAccess
 * \brief   ScreenElementAccess is data object to represent screen elements
 */
class ScreenElementAccess
{
public:
    /*!
    * @fn       ScreenElementAccess
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    ScreenElementAccess(){
    }

protected:
private:
    friend class boost::serialization::access;
    template<class archive>

    /*!
     * @fn      serialize
     * @param   archive& - ar
     * @param   const unsigned int - version
     * @return  None
     * @brief   This function implements the serialization of the class object.Those
     *           class members which you want to include in serilaized xml archive
     *          should have make_nvp mapping.
     *          The auto serialization of the member varible is skipped here to
     *          avoid the count and class tags.
     */
    void serialize(archive& ar , const unsigned int version)
    {
        Q_UNUSED(version);
        TRACE_LOG("");
        using boost::serialization::make_nvp;
        ar & make_nvp("ScreenName", m_screenName);
        ar & make_nvp("UIComponent", m_screen);
    }
public:
    std::string    m_screenName;    //!< Screen Name
    ScreenList     m_screen;        //!< vector of ui components
};

typedef std::vector<ScreenElementAccess>    accessControlList;    //!< vector of screen elements

/*!
 * \class   AccessElementDao
 * \brief   AccessElementDao is data object to represent access elemenmts dao

 */
class AccessElementDao {
public:
    /*!
    * @fn       AccessElementDao
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    AccessElementDao(){
    }

private:
    friend class boost::serialization::access;
    template<class archive>
    /*!
     * @fn      serialize
     * @param   archive& - ar
     * @param   const unsigned int - version
     * @return  None
     * @brief   This function implements the serialization of the class object.Those
     *          class members which you want to include in serilaized xml archive
     *          should have make_nvp mapping.
     *          The auto serialization of the member varible is skipped here to
     *          avoid the count and class tags. Serilization map of the component of the
     *          vectors , i.e. Screens is only added.
     */
    void serialize(archive& ar , const unsigned int version)
    {
        Q_UNUSED(version);
        TRACE_LOG("");
        using boost::serialization::make_nvp;
        ar & make_nvp("Screens" , m_accessControlList );
    }

public:
    accessControlList    m_accessControlList;    //!< vector of screen elements
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_ULM_ACCESSDEFDAO_H_

