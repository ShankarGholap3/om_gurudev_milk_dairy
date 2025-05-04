/*!
* @file     configbaseelement.h
* @author   Agiliad
* @brief    Info about a parameter of config base element.
* @date     July, 26 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_CONFIGBASEELEMENT_H_
#define ANALOGIC_WS_UIHANDLER_CONFIGBASEELEMENT_H_

#include <QObject>
#include <QVariant>
#include <QString>

#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
/*!
 * \class   ConfigBaseElement
 * \brief   Information about a parameter of config base element.
 */
class ConfigBaseElement
{
  Q_GADGET
  Q_PROPERTY(QString tagName READ tagName)
  Q_PROPERTY(QString field READ field)
  Q_PROPERTY(QString description READ description)
  Q_PROPERTY(QMLEnums::XMLDataTypes type READ type)
  Q_PROPERTY(QString value READ value)
  Q_PROPERTY(QString unit READ unit)
  Q_PROPERTY(QVariantList values READ values)
  Q_PROPERTY(QStringList enumvaluelist READ enumvaluelist)
  Q_PROPERTY(QMLEnums::XMLValueTypes valueType READ valueType)
  Q_PROPERTY(QString minValue READ minValue)
  Q_PROPERTY(QString maxValue READ maxValue)
  Q_PROPERTY(QVariantList dependencyList READ dependencyList)

public:
  /*!
    * @fn       valueType
    * @param    None
    * @return   QMLEnums::XMLDataTypes
    * @brief    value data type of xsd field
    */
  QMLEnums::XMLValueTypes valueType() const { return m_valueType; }

  /*!
    * @fn       minValue
    * @param    None
    * @return   QString
    * @brief    minValue of xsd field
    */
  QString minValue() const { return m_minValue; }

  /*!
    * @fn       maxValue
    * @param    None
    * @return   QString
    * @brief    maxValue of xsd field
    */
  QString maxValue() const { return m_maxValue; }

  /*!
    * @fn       unit
    * @param    None
    * @return   QString - unit of xml field
    * @brief    returns unit of xml field
    */
  QString unit() const { return m_unit; }
  /*!
    * @fn       tagName
    * @param    None
    * @return   QString - tagname
    * @brief    returns tag name
    */
  QString tagName() const { return m_tagName; }
  /*!
    * @fn       field
    * @param    None
    * @return   QString - field
    * @brief    returns xml field
    */
  QString field() const { return m_field; }
  /*!
    * @fn       description
    * @param    None
    * @return   QString - description
    * @brief    returns xml description
    */
  QString description() const { return m_description; }

  /*!
    * @fn       type
    * @param    None
    * @return   XMLDataTypes
    * @brief    returns xml type
    */
  QMLEnums::XMLDataTypes type() const { return m_type;}

  /*!
    * @fn       value
    * @param    None
    * @return   QString - value of xml field
    * @brief    returns value of xml field
    */
  QString value() const { return m_value; }

  /*!
    * @fn       values
    * @param    None
    * @return   QString - values of xml field
    * @brief    returns values of xml field
    */
  QVariantList values() const { return m_values; }

  /*!
    * @fn       enumvaluelist
    * @param    None
    * @return   QStringList - values of enums
    * @brief    returns values of enums field
    */
  QStringList enumvaluelist() const { return m_enumValueList; }

  /*!
    * @fn       dependencyList
    * @param    None
    * @return   QVariantList
    * @brief    returns name and value list of dependency tag
    */
  QVariantList dependencyList() const { return m_dependencyList; }

  /*!
    * @fn       ConfigBaseElement
    * @param    None
    * @return   None
    * @brief    Constructor for class ConfigBaseElement.
    */
  ConfigBaseElement();

  /*!
    * @fn       ConfigBaseElement
    * @param    const ConfigBaseElement &
    * @return   None
    * @brief    Copy Constructor for class ConfigBaseElement.
    */
  ConfigBaseElement(const ConfigBaseElement &);

  /*!
    * @fn       operator=
    * @param    const ConfigBaseElement &
    * @return   ConfigBaseElement&
    * @brief    assignment for class ConfigBaseElement.
    */
  ConfigBaseElement &operator= (const ConfigBaseElement &);

  /*!
    * @fn       ~ConfigBaseElement
    * @param    None
    * @return   None
    * @brief    Destructor for class ConfigBaseElement.
    */
  ~ConfigBaseElement();

  QString                 m_tagName;           //!< tagName of the parent element
  QString                 m_field;             //!< field of the parameter
  QString                 m_description;       //!< description of the parameter.
  QString                 m_value;             //!< value of the parameter.
  QString                 m_unit;              //!< unit for config
  QVariantList            m_values;            //!< sublists of the parameter.
  QMLEnums::XMLDataTypes  m_type;              //!< data type of the parameter.
  QMLEnums::XMLValueTypes m_valueType;         //!< data type of value.
  QString                 m_minValue;          //!< minimum allowed value
  QString                 m_maxValue;          //!< maximum allowed value
  QStringList             m_enumValueList;
  QVariantList            m_dependencyList;    //!< dependency list of the parameter.
};
}  // end of namespace ws
}  // end of namespace analogic
Q_DECLARE_METATYPE(analogic::ws::ConfigBaseElement)
#endif  // ANALOGIC_WS_UIHANDLER_CONFIGBASEELEMENT_H_
