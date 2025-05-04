/*!
* @file     workstationconfigdao.cpp
* @author   Agiliad
* @brief    Info about a parameter of scanner configuration.
* @date     July, 26 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/workstationconfigdao.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       WorkstationConfigDao
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class WorkstationConfigDao.
*/
WorkstationConfigDao::WorkstationConfigDao()
{
    TRACE_LOG("");
}

/*!
* @fn       WorkstationConfigDao
* @param    const WorkstationConfigDao &
* @return   None
* @brief    Copy Constructor for class WorkstationConfigDao.
*/
WorkstationConfigDao::WorkstationConfigDao(const WorkstationConfigDao &sbli)
{
    TRACE_LOG("");
    m_tagName = sbli.m_tagName;
    m_field = sbli.m_field;
    m_description = sbli.m_description;
    m_type = sbli.m_type;
    m_value = sbli.m_value;
    m_values = sbli.m_values;
    m_enumValueList = sbli.m_enumValueList;
}

WorkstationConfigDao &WorkstationConfigDao::operator=(const WorkstationConfigDao &sbli)
{
    TRACE_LOG("");
    m_tagName = sbli.m_tagName;
    m_field = sbli.m_field;
    m_description = sbli.m_description;
    m_type = sbli.m_type;
    m_value = sbli.m_value;
    m_values = sbli.m_values;
    m_enumValueList = sbli.m_enumValueList;
    return *this;
}

/*!
* @fn       ~WorkstationConfigDao
* @param    None
* @return   None
* @brief    Destructor for class WorkstationConfigDao.
*/
WorkstationConfigDao::~WorkstationConfigDao()
{
    TRACE_LOG("");
    m_tagName.clear();
    m_field.clear();
    m_description.clear();
    m_value.clear();
    m_values.clear();
    m_enumValueList.clear();
}
}
}
