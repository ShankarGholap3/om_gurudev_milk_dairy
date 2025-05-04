/*!
* @file     xmlservice.h
* @author   Agiliad
* @brief    This file contains functions related to XmlService
*           which parse and handles xml files.
* @date     July, 27 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_UTILITY_XMLSERVICE_H_
#define ANALOGIC_WS_COMMON_UTILITY_XMLSERVICE_H_

#include <QObject>
#include <QDebug>
#include <QVariant>
#include <QString>
#include <Logger.h>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QXmlStreamReader>
#include <QXmlItem>
#include <QtXml/QDomDocument>
#include <analogic/ws/uihandler/configbaseelement.h>
#include <TimeZoneRetriever.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

#define XML_TAG_NAME "tagName"
#define XML_TAG_FILED "field"
#define XML_TAG_DESCRIPTION "description"
#define XML_TAG_VALUE "value"
#define XML_TAG_UNIT "unit"
#define XML_TAG_DEPENDENCY "dependency"
#define XML_DEPENDENCY_NAME "parametername"
#define XML_DEPENDENCY_VALUE "parametervalue"
#define XML_ATTR_NAME "name"
#define XML_ATTR_TYPE "type"
#define XML_ATTR_BASE "base"
#define XSD_MAXLENGTH "xs:maxLength"
#define PARAMETER_XML_ROOT "ConnectConf"
#define PARAMETER_XSD_ROOT "xs:schema"
#define XSD_SIMPLE_TYPE "xs:simpleType"
#define XSD_COMPLEX_TYPE "xs:complexType"
#define XSD_RESTRICTION "xs:restriction"
#define XSD_MININCLUSIVE "xs:minInclusive"
#define XSD_MAXINCLUSIVE "xs:maxInclusive"
#define XSD_IPADDRPATTRN "xs:pattern"
#define XSD_SEQUENCE "xs:sequence"
#define XSD_ELEMENT "xs:element"
#define XSD_ENUMERATION "xs:enumeration"


#define TIP_HIT_TAG "TIP_Hit"
#define TIP_MISS_TAG "TIP_Miss"
#define TIP_NON_TIP_TAG "Non_Tip_Alarm"

/*!
 * \class   RestrictionType
 * \brief   This class contains restriction on tag Min and Max value
 */
struct RestrictionType
{
  QMLEnums::XMLValueTypes valueType;
  QString maxvalue;
  QString minvalue;
};

/*!
 * \class   XmlService
 * \brief   This class contains variable and function related to
 *          handling XmlService of application.
 */
class XmlService:public QObject
{
  Q_OBJECT
public:
  /*!
    * @fn       XmlService
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class XmlService.
    */
  XmlService();


  /*!
    * @fn       parseXml
    * @param    QString& errorMsg
    * @param    QByteArray xml data
    * @param    QVariantList&
    * @return   bool
    * @brief    function for xml parsing.
    */
  static bool parseXml(QString &errorMsg, QByteArray xmlData, QVariantList& list);

  /*!
    * @fn       validateXml
    * @param    QString& errorMsg
    * @param    QByteArray xmlData
    * @param    QByteArray xsdData
    * @return   bool
    * @brief    function for xml validation.
    */
  static bool validateXml(QString &errorMsg, QByteArray xmlData, QByteArray xsdData);

  /*!
    * @fn       getXmlAndXsdContent
    * @param    const QString& xmlfile
    * @param    const QString& xsdfile
    * @param    QString& xmlData - return
    * @param    QString& xsdData - return
    * @param    QString& error - return
    * @return   bool
    * @brief    get xml and xsd content from xmlFile.
    */
  static bool getXmlAndXsdContent(const QString &xmlfile, const QString &xsdfile,
                                  QString &xmlData, QString &xsdData, QString &error);
  /*!
    * @fn       validateAndParseXml
    * @param    QString xml data
    * @param    QString xsd data
    * @param    QVariantList& list - return
    * @param    QString& error - return
    * @return   bool
    * @brief    function for xml validation.
    */
  static bool validateAndParseXml(QString xmlData, QString xsdData,
                                  QVariantList& xmlparamlist, QString &error);
  /*!
    * @fn       updateXmlContent
    * @param    QVariantList
    * @param    QString
    * @param    QString
    * @param    QString
    * @return   bool
    * @brief    updates tag value, if duplicate tagNames are present all will be set with same value
    */
  static bool updateXmlContent(QVariantList listModel, QString &xmlData, QString &xsdData, QString &errMsg);

  /*!
    * @fn       generateXmlFile
    * @param    QString
    * @param    const QString
    * @param    const QString
    * @return   bool
    * @brief    write data into xmlFile.
    */
  static bool generateXmlFile(QString xmlFileName, const QString &xmlData, const QString &xsdData);

  /*!
    * @fn       ~XmlService
    * @param    None
    * @return   None
    * @brief    Destructor for class XmlService.
    */
  virtual ~XmlService();

private:
  /*!
    * @fn       getTypeFromMap
    * @param    QString parameter
    * @return   QString
    * @brief    gets simple type of parameter
    */
  static QString getTypeFromMap(QString parameter);

  /*!
    * @fn       getTypeNameFromMap
    * @param    QString parameter
    * @return   QString
    * @brief    gets simple type of parameter
    */
  static QString getTypeNameFromMap(QString parameter);

  /*!
    * @fn       getMinMaxStructFromMap
    * @param    QString parameter
    * @return   QString
    * @brief    gets simple type of parameter
    */
  static struct RestrictionType getMinMaxStructFromMap(QString parameter);
  /*!
    * @fn       recursiveParse
    * @param    QXmlStreamReader*
    * @return   ConfigBaseElement
    * @brief    function for xml parsing.
    */
  static ConfigBaseElement recursiveParse(QXmlStreamReader* xmlReader);

  /*!
    * @fn       updateSchemaMaps
    * @param    QString
    * @return   None
    * @brief    sets the schema maps by parsing xsd
    */
  static void updateSchemaMaps(QString xsdData);

  /*!
    * @fn       getEnumValuesFromMap
    * @param    QString parameter
    * @return   QStringList
    * @brief    get enum values for simple type
    */
  static QStringList getEnumValuesFromMap(QString parameter);

  static QMap <QString, QString>           m_simpleTypeMap;            //!< m_simpleTypeMap
  static QMap <QString, QString>           m_complexTypeMap;           //!< m_complexTypeMap
  static QMap <QString, QString>           m_parameterTypeMap;         //!< map for parameter type
  static QMap <QString, RestrictionType>   m_restrictionTypeMap;       //!< m_restrictionTypeMap
  static QMultiMap <QString, QString>      m_simpleTypeEnums;          //!< m_parameterTypeMap
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_UTILITY_XMLSERVICE_H_
