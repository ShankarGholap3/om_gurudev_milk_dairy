/*!
* @file     configbaseelement.cpp
* @author   Agiliad
* @brief    Info about a parameter of config base element.
* @date     July, 26 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/configbaseelement.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       ConfigBaseElement
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class ConfigBaseElement.
*/
ConfigBaseElement::ConfigBaseElement()
{
  TRACE_LOG("");
}

/*!
* @fn       ConfigBaseElement
* @param    const ConfigBaseElement &
* @return   None
* @brief    Copy Constructor for class ConfigBaseElement.
*/
ConfigBaseElement::ConfigBaseElement(const ConfigBaseElement &sbli)
{
  TRACE_LOG("");
  m_tagName = sbli.m_tagName;
  m_field = sbli.m_field;
  m_description = sbli.m_description;
  m_type = sbli.m_type;
  m_value = sbli.m_value;
  m_unit = sbli.m_unit;
  m_valueType = sbli.m_valueType;
  m_minValue = sbli.m_minValue;
  m_maxValue = sbli.m_maxValue;
  m_values = sbli.m_values;
  m_enumValueList = sbli.m_enumValueList;
  m_dependencyList = sbli.m_dependencyList;
}

ConfigBaseElement &ConfigBaseElement::operator=(const ConfigBaseElement &sbli)
{
  TRACE_LOG("");
  m_tagName = sbli.m_tagName;
  m_field = sbli.m_field;
  m_description = sbli.m_description;
  m_type = sbli.m_type;
  m_value = sbli.m_value;
  m_unit = sbli.m_unit;
  m_valueType = sbli.m_valueType;
  m_minValue = sbli.m_minValue;
  m_maxValue = sbli.m_maxValue;
  m_values = sbli.m_values;
  m_enumValueList = sbli.m_enumValueList;
  m_dependencyList = sbli.m_dependencyList;
  return *this;
}

/*!
* @fn       ~ConfigBaseElement
* @param    None
* @return   None
* @brief    Destructor for class ConfigBaseElement.
*/
ConfigBaseElement::~ConfigBaseElement()
{
  TRACE_LOG("");
  m_tagName.clear();
  m_field.clear();
  m_description.clear();
  m_value.clear();
  m_unit.clear();
  m_minValue.clear();
  m_maxValue.clear();
  m_values.clear();
  m_enumValueList.clear();
  m_dependencyList.clear();
}
}  // end of namespace ws
}  // end of namespace analogic
