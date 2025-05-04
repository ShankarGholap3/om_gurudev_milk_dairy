/*!
* @file     wsscreendao.h
* @author   Agiliad
* @brief    This file contains structures and maps for the UI configurations.
*           This is a data access object generated from UI configuration files.
*           Those classes which needs the serialization suport should implement
*           serialize method of friend class seriliazation::access.
* @date     Sep, 29 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_ULM_WSSCREENDAO_H_
#define ANALOGIC_WS_ULM_WSSCREENDAO_H_

#include <map>
#include <list>
#include <vector>
#include <string>
#include <QtGlobal>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <Logger.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   UIComponentScreen
 * \brief   UIComponentScreen is data object to represent UI component properties like featureid ,
 *          visibility and enable/disable.
 *  \note   this class will be member of stl containers , if sort/compare operations
 *          are to be done then this class should have the copy constructor and = operator
 *          overloaded.
 */
class UIComponentScreen
{
public:
    /*!
    * @fn       UIComponentScreen
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    UIComponentScreen(){
    }

    /*!
    * @fn       UIComponentScreen
    * @param    bool - enable
    * @param    bool - visibilty
    * @param    std::string - objName
    * @return   None
    * @brief    parameterised constructor to populate objects without deserialization.
    */
    UIComponentScreen(bool enable , bool visibilty, std::string objName)
        :m_isEnabled(enable) , m_visibility(visibilty) , m_objectName(objName){}
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
        using boost::serialization::make_nvp;
        ar & make_nvp("ObjectName", m_objectName);
        ar & make_nvp("Enable", m_isEnabled);
        ar & make_nvp("Visibility", m_visibility);
    }
public:
    bool           m_isEnabled;     //!< UI Component enable flag
    bool           m_visibility;    //!< UI Component visibility flag
    std::string    m_objectName;    //!< UI Component object name/id
};

typedef std::vector<UIComponentScreen>            ScreenControlList;  //!< Vector of objectName and UIComponent
typedef std::map< std::string, ScreenControlList> ScreenStateMap;     //!< Map of App Component state & UIComponentMap

/*!
 * \class   ScreenControl
 * \brief   ScreenControl is data object to represent
 *          screen control elements
 */
class ScreenControl
{
public:
    /*!
    * @fn       ScreenControl
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    ScreenControl(){
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
 *           class members which you want to include in serilaized xml archive
 *          should have make_nvp mapping.
 *          The auto serialization of the member varible is skipped here to
 *          avoid the count and class tags.
 */

    void serialize(archive& ar , const unsigned int version)
    {
        Q_UNUSED(version);
        using boost::serialization::make_nvp;
        ar & make_nvp("UIComponent", m_screenControlList);
    }
public:
    ScreenControlList    m_screenControlList;    //!< vector of UI components for control
};

/*!
 * \class   ScreenState
 * \brief   ScreenState is data object to represent screen state elements
 */
class ScreenState
{
public:
    /*!
    * @fn       ScreenState
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    ScreenState()
    {
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
     *           class members which you want to include in serilaized xml archive
     *          should have make_nvp mapping.
     *          The auto serialization of the member varible is skipped here to
     *          avoid the count and class tags.
     */

    void serialize(archive& ar , const unsigned int version)
    {
        Q_UNUSED(version);
        using boost::serialization::make_nvp;
        ar & make_nvp("State", m_screenStateMap);
    }
public:
    ScreenStateMap    m_screenStateMap;    //!< Map of Application Component state and available UIComponents
};

/*!
 * \class   ScreenElement
 * \brief   ScreenElement is data object to represent screen UI components
 */
class ScreenElement
{
public:
    /*!
    * @fn       ScreenElement
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    ScreenElement(){
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
        using boost::serialization::make_nvp;
        ar & make_nvp("ScreenName", m_screenName);
        ar & make_nvp("Controls", m_screenControl);
        ar & make_nvp("States", m_screenState);
    }
public:
    std::string      m_screenName;       //!< workstation screen name / workstation type
    ScreenState      m_screenState;      //!< Map of Application Component state and UIComponentMap
    ScreenControl    m_screenControl;    //!< vector or UIcomponents
};

typedef std::vector<ScreenElement>          workStationElement;    //!< vector of screen elements


/*!
 * \class   WorkstationType
 * \brief   WorkstationType is class that represents type of workstation.
 */
class WorkstationType
{
public:
    /*!
            * @fn       WorkStationUIElementDAO
            * @param    None
            * @return   None
            * @brief    Constructor responsible for initialization of class members, memory and resources.
            */
    WorkstationType(){
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
             *           class members which you want to include in serilaized xml archive
             *          should have make_nvp mapping.
             *          The auto serialization of the member varible is skipped here to
             *          avoid the count and class tags. Serilization map of the component of the
             *          vectors , i.e. Screens is only added.
             */
    void serialize(archive& ar , const unsigned int version)
    {
        Q_UNUSED(version);
        using boost::serialization::make_nvp;
        ar & make_nvp("WSType" , m_wsType );
        ar & make_nvp("Workstation" , m_workstation );
    }
public:
    workStationElement    m_workstation;    //!< vector of screen elements
    std::string           m_wsType;         //!< Type Name of workstation
};


typedef std::vector<WorkstationType>     workstationTypeVector;    //!< Vector of Workstation element
/*!
 * \class   WorkStationUIElementDAO
 * \brief   WorkStationUIElementDAO is data access object to represent workstation state based UI config.
 *          This will take care of creation of the workstation UI element consumable by ULM.
 */

class WorkStationUIElementDAO
{
public:
    /*!
    * @fn       WorkStationUIElementDAO
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    WorkStationUIElementDAO(){
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
     *           class members which you want to include in serilaized xml archive
     *          should have make_nvp mapping.
     *          The auto serialization of the member varible is skipped here to
     *          avoid the count and class tags. Serilization map of the component of the
     *          vectors , i.e. Screens is only added.
     */
    void serialize(archive& ar , const unsigned int version)
    {
        Q_UNUSED(version);
        using boost::serialization::make_nvp;
        ar & make_nvp("Workstations" , m_wsElement );
    }
public:
    workstationTypeVector    m_wsElement;    //!< vector of workstation elements
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_ULM_WSSCREENDAO_H_

