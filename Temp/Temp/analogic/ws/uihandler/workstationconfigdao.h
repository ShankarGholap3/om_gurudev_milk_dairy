
/*!
* @file     workstationconfigdao.h
* @author   Agiliad
* @brief    Info about a parameter of Workstation configuration.
* @date     July, 26 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_UIHANDLER_WORKSTATIONCONFIGDAO_H_
#define ANALOGIC_WS_UIHANDLER_WORKSTATIONCONFIGDAO_H_


#include <QObject>
#include <QVariant>
#include <QString>

#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
/*!
 * \class   WorkstationConfigDao
 * \brief   Information about a parameter of Workstation Configurations.
 */
class WorkstationConfigDao
{
    Q_GADGET
    Q_PROPERTY(QString tagName READ tagName)
    Q_PROPERTY(QString field READ field)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QMLEnums::XMLDataTypes type READ type)
    Q_PROPERTY(QString value READ value)
    Q_PROPERTY(QVariantList values READ values)
    Q_PROPERTY(QStringList enumvaluelist READ enumvaluelist)

public:
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
    * @fn       WorkstationConfigDao
    * @param    None
    * @return   None
    * @brief    Constructor for class WorkstationConfigDao.
    */
    WorkstationConfigDao();

    /*!
    * @fn       WorkstationConfigDao
    * @param    const WorkstationConfigDao &
    * @return   None
    * @brief    Copy Constructor for class WorkstationConfigDao.
    */
    WorkstationConfigDao(const WorkstationConfigDao &);

    /*!
    * @fn       operator=
    * @param    const WorkstationConfigDao &
    * @return   WorkstationConfigDao&
    * @brief    assignment for class WorkstationConfigDao.
    */
    WorkstationConfigDao &operator= (const WorkstationConfigDao &);

    /*!
    * @fn       ~WorkstationConfigDao
    * @param    None
    * @return   None
    * @brief    Destructor for class WorkstationConfigDao.
    */
    ~WorkstationConfigDao();

    QString                 m_tagName;           //!< tagName of the parent element
    QString                 m_field;             //!< field of the parameter
    QString                 m_description;       //!< description of the parameter.
    QString                 m_value;             //!< value of the parameter.
    QVariantList            m_values;            //!< sublists of the parameter.
    QMLEnums::XMLDataTypes  m_type;              //!< data type of the parameter.
    QStringList             m_enumValueList;
};
}
}
Q_DECLARE_METATYPE(analogic::ws::WorkstationConfigDao)

#endif // ANALOGIC_WS_UIHANDLER_WORKSTATIONCONFIGDAO_H
