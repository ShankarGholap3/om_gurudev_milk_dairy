/*!
* @file     xmlservice.cpp
* @author   Agiliad
* @brief    This file contains functions related to XmlService
*           which parse and handles xml files.
* @date     July, 27 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/utility/xmlservice.h>
#include <analogic/ws/common/utility/messagehandler.h>
#include <QFile>
#include <QBuffer>

namespace analogic
{
namespace ws
{
QMap <QString , QString>    XmlService::m_simpleTypeMap;
QMap <QString , QString>    XmlService::m_complexTypeMap;
QMap <QString , QString>    XmlService::m_parameterTypeMap;
QMap <QString , RestrictionType>    XmlService::m_restrictionTypeMap;
QMultiMap <QString, QString> XmlService::m_simpleTypeEnums;


/*!
* @fn       XmlService
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class XmlService.
*/
XmlService::XmlService()
{}
/*!
* @fn       parseXml
* @param    QString& errorMsg
* @param    QByteArray xml data
* @param    QVariantList&
* @return   bool
* @brief    function for xml parsing using xml and xsd data in form of bytearray.
*/
bool XmlService::parseXml(QString &errorMsg, QByteArray xmlData, QVariantList& list)
{
  QXmlStreamReader* xmlReader = new QXmlStreamReader(xmlData);
  xmlReader->readNext();
  // read and save root element
  QXmlStreamReader::TokenType token = xmlReader->readNext();
  QXmlStreamReader::TokenType prevTokenType = QXmlStreamReader::StartElement;
  while(!xmlReader->atEnd() && !xmlReader->hasError())
  {
    // Read next element
    token = xmlReader->readNext();
    //! If token is StartElement - read it
    if(token == QXmlStreamReader::StartElement)
    {
      if (prevTokenType == QXmlStreamReader::StartElement)
      {
        list.append(QVariant::fromValue(recursiveParse(xmlReader)));
      }
      prevTokenType = token;
    }
  }
  if(xmlReader->hasError())
  {
    errorMsg = "XML Parse Error: " + xmlReader->errorString();
    return false;
  }
  xmlReader->clear();
  return true;
}

/*!
* @fn       recursiveParse
* @param    QXmlStreamReader*
* @return   ConfigBaseElement
* @brief    function for xml parsing.
*/
ConfigBaseElement XmlService::recursiveParse(QXmlStreamReader *xmlReader)
{
  ConfigBaseElement m_configBaseElement;
  m_configBaseElement.m_tagName = xmlReader->name().toString();
  QString valueType = getTypeFromMap(m_configBaseElement.m_tagName);
  if (valueType.length() > 0)
  {
    if (valueType.compare("xs:string") == 0)
    {
      m_configBaseElement.m_type = QMLEnums::XMLDataTypes::STRINGTYPE;

      if (m_configBaseElement.m_tagName.compare("sc_time_zone") == 0)
      {
        m_configBaseElement.m_type = QMLEnums::XMLDataTypes::ENUMSTYPE;
        analogic::ancp00::TimeZoneRetriever timeZoneService;

        if (timeZoneService.retrieve_time_zones())
        {
          vector<string> zoneLines = timeZoneService.get_lines();
          for (vector<string>::iterator it = zoneLines.begin();
               it != zoneLines.end(); it++)
          {
            m_configBaseElement.m_enumValueList.append(QString::fromStdString(*it));
          }
        }
      }
    }
    else if (valueType.compare("xs:boolean") == 0)
    {
      m_configBaseElement.m_type = QMLEnums::XMLDataTypes::BOOLEANTYPE;
      //            m_configBaseElement.m_enumValueList.append(QString("false"));
      //            m_configBaseElement.m_enumValueList.append(QString("true"));
    }

    else
    {
      m_configBaseElement.m_type = QMLEnums::XMLDataTypes::STRINGTYPE;
    }

    QStringList enumValues = getEnumValuesFromMap(m_configBaseElement.m_tagName);
    for (int j=0; j < enumValues.length() ; j++)
    {
      m_configBaseElement.m_enumValueList.append(enumValues.at(j));
    }
    if (enumValues.length() > 0)
    {
      m_configBaseElement.m_type = QMLEnums::XMLDataTypes::ENUMSTYPE;
    }
  }
  QXmlStreamReader::TokenType prevTokenType = QXmlStreamReader::StartElement;

  while(!xmlReader->atEnd() && !xmlReader->hasError())
  {
    // Read next element
    QXmlStreamReader::TokenType token = xmlReader->readNext();
    //! If token is StartElement - read it
    if(token == QXmlStreamReader::StartElement)
    {
      if (xmlReader->name() == XML_TAG_FILED)
      {
        m_configBaseElement.m_field = xmlReader->readElementText();
      }
      else if (xmlReader->name() == XML_TAG_DESCRIPTION)
      {
        m_configBaseElement.m_description = xmlReader->readElementText();
      }
      else if (xmlReader->name() == XML_TAG_VALUE)
      {
        m_configBaseElement.m_value = xmlReader->readElementText();
      }
      else if (xmlReader->name() == XML_TAG_UNIT)
      {
        m_configBaseElement.m_unit = xmlReader->readElementText();
      }
      else if(xmlReader->name() == XML_TAG_DEPENDENCY)
      {
        QStringList dependencyParam;
        foreach(const QXmlStreamAttribute &attr, xmlReader->attributes())
        {
          //! dependencyParam list holds parameter name at 0th position
          //! dependencyParam list holds parameter value at 1th position
          if(attr.name().toString() == XML_DEPENDENCY_NAME)
          {
            dependencyParam.insert(0, attr.value().toString());
          }
          else if(attr.name().toString() == XML_DEPENDENCY_VALUE)
          {
            dependencyParam.insert(1, attr.value().toString());
          }
        }
        if(dependencyParam.count() > 0)
        {
          m_configBaseElement.m_dependencyList.append(QVariant::fromValue(dependencyParam));
          // m_configBaseElement.m_type = QMLEnums::DEPENDENCYTYPE;
          token = xmlReader->readNext();
        }
      }
      else if (prevTokenType == QXmlStreamReader::StartElement)
      {
        m_configBaseElement.m_values.append(QVariant::fromValue(recursiveParse(xmlReader)));
        m_configBaseElement.m_type = QMLEnums::LISTTYPE;
      }

      QString simpletypename = XmlService::getTypeNameFromMap(m_configBaseElement.m_tagName);
      struct RestrictionType restrictiontype = XmlService::getMinMaxStructFromMap(simpletypename);
      m_configBaseElement.m_valueType = restrictiontype.valueType;
      m_configBaseElement.m_minValue = restrictiontype.minvalue;
      m_configBaseElement.m_maxValue = restrictiontype.maxvalue;
    }
    else if (token == QXmlStreamReader::EndElement)
    {
      return m_configBaseElement;
    }
  }
  return m_configBaseElement;
}


/*!
* @fn       validateXml
* @param    QString& errorMsg
* @param    QByteArray xmlData
* @param    QByteArray xsdData
* @return   bool
* @brief    function for xml validation.
*/
bool XmlService::validateXml(QString &errorMsg, QByteArray xmlData, QByteArray xsdData)
{
  if (xmlData.isEmpty() || xsdData.isEmpty())
  {
    ERROR_LOG("Empty XSD or XML");
    return false;
  }

  MessageHandler messageHandler;

  QXmlSchema schema;
  schema.setMessageHandler(&messageHandler);

  schema.load(xsdData);

  if (!schema.isValid()) {
    return false;
  } else {
    QXmlSchemaValidator validator(schema);
    if (!validator.validate(xmlData))
    {
      errorMsg = tr("Received XML document is invalid against schema. ")+ messageHandler.statusMessage()
          + tr("at Line ") + messageHandler.line() + tr("column No. ") + messageHandler.column();
      ERROR_LOG(errorMsg.toStdString().c_str());
      return false;
    }
    else
    {
      updateSchemaMaps(QString::fromStdString(xsdData.toStdString()));
      INFO_LOG("Received XML document is valid against schema.");
      return true;
    }
  }
}


/*!
* @fn       updateSchemaMaps
* @param    QString
* @return   None
* @brief    sets the schema maps by parsing xsd
*/
void XmlService::updateSchemaMaps(QString xsdData)
{
  m_simpleTypeEnums.clear();
  m_restrictionTypeMap.clear();
  QDomDocument *domDoc =  new QDomDocument();
  domDoc->setContent(xsdData);
  QDomElement root;
  root = domDoc->firstChildElement(PARAMETER_XSD_ROOT);

  QDomElement simpleTypeElement = root.firstChildElement(XSD_SIMPLE_TYPE);
  QDomElement complexTypeElement = root.firstChildElement(XSD_COMPLEX_TYPE);

  while (!simpleTypeElement.isNull())
  {
    QString simpleTypeName = simpleTypeElement.attribute(XML_ATTR_NAME);
    QDomElement restrictionElement = simpleTypeElement.firstChildElement(XSD_RESTRICTION);
    QString simpleBaseName = restrictionElement.attribute(XML_ATTR_BASE);
    if((simpleBaseName.contains("xs:int"))
       ||(simpleBaseName.contains("xs:positiveInteger"))
       ||(simpleBaseName.contains("xs:nonNegativeInteger"))
       )
    {
      if (simpleBaseName.contains("xs:positiveInteger"))
      {
        m_restrictionTypeMap.insert(simpleTypeName,
        {QMLEnums::XMLValueTypes::POSITIVEINT, "", ""});
      }
      else
      {
        m_restrictionTypeMap.insert(simpleTypeName,
        {QMLEnums::XMLValueTypes::INTVALUE, "", ""});
      }
      QDomElement minmaxInclusive = restrictionElement.firstChildElement(XSD_MININCLUSIVE);
      while(!minmaxInclusive.isNull())
      {
        QString minValue = minmaxInclusive.attribute(XML_TAG_VALUE);
        minmaxInclusive = minmaxInclusive.nextSiblingElement(XSD_MAXINCLUSIVE);
        QString maxValue;

        maxValue = minmaxInclusive.attribute(XML_TAG_VALUE);
        if (simpleBaseName.contains("xs:positiveInteger"))
        {

          m_restrictionTypeMap.insert(simpleTypeName,
          {
                                        QMLEnums::XMLValueTypes::POSITIVEINT,
                                        maxValue,
                                        minValue
                                      });
        }
        else
        {
          m_restrictionTypeMap.insert(simpleTypeName,
          {
                                        QMLEnums::XMLValueTypes::INTVALUE,
                                        maxValue,
                                        minValue
                                      });
        }
        minmaxInclusive = minmaxInclusive.nextSiblingElement(XSD_MININCLUSIVE);
      }
    }
    else if((simpleBaseName.contains("xs:float"))
            ||(simpleBaseName.contains("xs:double"))
            )
    {
      m_restrictionTypeMap.insert(simpleTypeName,
      {QMLEnums::XMLValueTypes::FLOATVALUE, "", ""});

      QDomElement minmaxInclusive = restrictionElement.firstChildElement(XSD_MININCLUSIVE);
      while(!minmaxInclusive.isNull())
      {
        QString minValue = minmaxInclusive.attribute(XML_TAG_VALUE);
        minmaxInclusive = minmaxInclusive.nextSiblingElement(XSD_MAXINCLUSIVE);
        QString maxValue;

        maxValue = minmaxInclusive.attribute(XML_TAG_VALUE);
        if (simpleBaseName.contains("xs:float"))
        {
          m_restrictionTypeMap.insert(simpleTypeName,
          {
                                        QMLEnums::XMLValueTypes::FLOATVALUE,
                                        maxValue,
                                        minValue
                                      });
        }
        else
        {
          m_restrictionTypeMap.insert(simpleTypeName,
          {
                                        QMLEnums::XMLValueTypes::FLOATVALUE,
                                        maxValue,
                                        minValue
                                      });
        }
        minmaxInclusive = minmaxInclusive.nextSiblingElement(XSD_MININCLUSIVE);
      }
    }
    else if(simpleTypeName.contains("dottedQuad"))
    {
      m_restrictionTypeMap.insert(simpleTypeName,
      {QMLEnums::XMLValueTypes::IPADDRESSVALUE, "", ""});
      QDomElement ipAddrPattrn = restrictionElement.firstChildElement(XSD_IPADDRPATTRN);
      while(!ipAddrPattrn.isNull())
      {
        QString allowedChar = ipAddrPattrn.attribute(XML_TAG_VALUE);
        m_restrictionTypeMap.insert(simpleTypeName,
        {
                                      QMLEnums::XMLValueTypes::IPADDRESSVALUE,
                                      allowedChar,
                                      ""
                                    });
        ipAddrPattrn = ipAddrPattrn.nextSiblingElement(XSD_RESTRICTION);
      }
    }
    else if(simpleBaseName.contains("xs:string"))
    {
      m_restrictionTypeMap.insert(simpleTypeName,
      {QMLEnums::XMLValueTypes::STRINGVALUE, "", ""});
      QDomElement maxlength = restrictionElement.firstChildElement(XSD_MAXLENGTH);
      while(!maxlength.isNull())
      {
        QString maxLength = maxlength.attribute(XML_TAG_VALUE);
        m_restrictionTypeMap.insert(simpleTypeName,
        {
                                      QMLEnums::XMLValueTypes::STRINGVALUE,
                                      maxLength,
                                      ""
                                    });
        maxlength = maxlength.nextSiblingElement(XSD_RESTRICTION);
      }
      QDomElement enumerationElement = restrictionElement.firstChildElement(XSD_ENUMERATION);
      if(!enumerationElement.isNull())
      {
        m_restrictionTypeMap.insert(simpleTypeName,
        {QMLEnums::XMLValueTypes::ENUMVALUE, "", ""});
      }
    }
    else
    {
      m_restrictionTypeMap.insert(simpleTypeName,
      {QMLEnums::XMLValueTypes::NOVALUE, "", ""});
    }

    m_simpleTypeMap.insert(simpleTypeName, simpleBaseName);

    QDomElement enumerationElement = restrictionElement.firstChildElement(XSD_ENUMERATION);
    while (!enumerationElement.isNull())
    {
      QString enumValue = enumerationElement.attribute(XML_TAG_VALUE);
      m_simpleTypeEnums.insert(simpleTypeName, enumValue);
      enumerationElement = enumerationElement.nextSiblingElement(XSD_ENUMERATION);
    }

    simpleTypeElement = simpleTypeElement.nextSiblingElement(XSD_SIMPLE_TYPE);
  }

  while (!complexTypeElement.isNull())
  {
    QString complexTypeName = complexTypeElement.attribute(XML_ATTR_NAME);
    QDomElement sequenceElement = complexTypeElement.firstChildElement(XSD_SEQUENCE);
    QDomElement elementElement = sequenceElement.firstChildElement(XSD_ELEMENT);

    while (!elementElement.isNull())
    {
      QString elementName = elementElement.attribute(XML_ATTR_NAME);
      QString elementType;
      if (elementName == XML_TAG_VALUE)
      {
        elementType = elementElement.attribute(XML_ATTR_TYPE);
        m_complexTypeMap.insert(complexTypeName, elementType);
      }
      else if (elementName == XML_TAG_FILED)
      {
        // Do nothing for now
      }
      else if (elementName == XML_TAG_DESCRIPTION)
      {
        // Do nothing for now
      }
      else
      {
        elementType = elementElement.attribute(XML_ATTR_TYPE);
        m_parameterTypeMap.insert(elementName, elementType);
      }
      elementElement = elementElement.nextSiblingElement(XSD_ELEMENT);
    }
    complexTypeElement = complexTypeElement.nextSiblingElement(XSD_COMPLEX_TYPE);
  }
}

/*!
* @fn       updateXmlContent
* @param    QVariantList
* @param    QString
* @param    QString
* @param    QString
* @return   bool
* @brief    updates tag value, if duplicate tagNames are present all will be set with same value
*/
bool XmlService::updateXmlContent(QVariantList listModel, QString &xmlData, QString &xsdData, QString &errMsg)
{
  bool retVal = false;
  QString localXML;
  QDomDocument *domDoc =  new QDomDocument();
  domDoc->setContent(xmlData);
  QDomElement root;
  root = domDoc->firstChildElement(PARAMETER_XML_ROOT);

  for(int i = 0 ; i< listModel.size(); i++)
  {
    QMap<QString, QVariant> mapElement = listModel.at(i).toMap();
    if (mapElement[XML_TAG_NAME].toString().compare(TIP_HIT_TAG) == 0)
    {
      WorkstationConfig::getInstance()->setTipHITMsgDialog(mapElement[XML_TAG_VALUE].toString());
    }
    else if (mapElement[XML_TAG_NAME].toString().compare(TIP_MISS_TAG) == 0)
    {
      WorkstationConfig::getInstance()->setTipMISSMsgDialog(mapElement[XML_TAG_VALUE].toString());
    }
    else if (mapElement[XML_TAG_NAME].toString().compare(TIP_NON_TIP_TAG) == 0)
    {
      WorkstationConfig::getInstance()->setTipNONTIPMsgDialog(mapElement[XML_TAG_VALUE].toString());
    }
    QDomNodeList nodeList = domDoc->elementsByTagName(mapElement[XML_TAG_NAME].toString());
    for (int i = 0 ; i < nodeList.count() ; i++)
    {
      QDomNode elm = nodeList.at(i);
      QDomElement valueNode = elm.toElement().firstChildElement(XML_TAG_VALUE);
      if (valueNode.text().isEmpty())
      {
        QDomText child_node = domDoc->createTextNode(mapElement[XML_TAG_VALUE].toString());
        valueNode.appendChild(child_node);
      }
      else
      {
        valueNode.firstChild().setNodeValue(mapElement[XML_TAG_VALUE].toString());
      }

      QDomElement unitNode = elm.toElement().firstChildElement(XML_TAG_UNIT);
      unitNode.firstChild().setNodeValue(mapElement[XML_TAG_UNIT].toString());
    }
  }
  localXML = domDoc->toString();
  if (validateXml(errMsg, localXML.toUtf8(), xsdData.toUtf8()))
  {
    xmlData.clear();
    xmlData = localXML;
    retVal = true;
  }
  SAFE_DELETE(domDoc);
  return retVal;
}


/*!
* @fn       getTypeFromMap
* @param    QString parameter
* @return   QString
* @brief    gets simple type of parameter
*/
QString XmlService::getTypeFromMap(QString parameter)
{
  if(m_parameterTypeMap.size() == 0)
  {
    return "";
  }
  QString typeLeve1 = m_parameterTypeMap.find(parameter).value();

  if (typeLeve1.length() > 0)
  {
    if (m_complexTypeMap.find(typeLeve1) != m_complexTypeMap.end())
    {
      QString typeLevel2 = m_complexTypeMap.find(typeLeve1).value();
      if (typeLevel2.length() > 0)
      {
        QString typeLevel3 = m_simpleTypeMap.find(typeLevel2).value();
        if (typeLevel3.length() > 0)
        {
          return typeLevel3;
        }
      }
    }
    return "";
  }
  return "";
}

/*!
* @fn       getTypeNameFromMap
* @param    QString parameter
* @return   QString
* @brief    gets simple type of parameter
*/
QString XmlService::getTypeNameFromMap(QString parameter)
{
  if(m_parameterTypeMap.size() == 0)
  {
    return "";
  }
  QString typeLeve1 = m_parameterTypeMap.find(parameter).value();

  if (typeLeve1.length() > 0)
  {
    if (m_complexTypeMap.find(typeLeve1) != m_complexTypeMap.end())
    {
      QString typeLevel2 = m_complexTypeMap.find(typeLeve1).value();
      if (typeLevel2.length() > 0)
      {
        return typeLevel2;
      }
    }
    return "";
  }
  return "";
}

/*!
* @fn       getMinMaxStructFromMap
* @param    QString parameter
* @return   QString
* @brief    gets simple type of parameter
*/
struct RestrictionType XmlService::getMinMaxStructFromMap(QString SimpleTypeName)
{
  return m_restrictionTypeMap.value(SimpleTypeName);
}

/*!
* @fn       getEnumValuesFromMap
* @param    QString parameter
* @return   QStringList
* @brief    get enum values for simple type
*/
QStringList XmlService::getEnumValuesFromMap(QString parameter)
{
  QStringList enumList;

  QString typeLeve1 = m_parameterTypeMap.find(parameter).value();

  if (typeLeve1.length() > 0)
  {
    if (m_complexTypeMap.find(typeLeve1) != m_complexTypeMap.end())
    {
      QString typeLevel2 = m_complexTypeMap.find(typeLeve1).value();
      if (typeLevel2.length() > 0)
      {
        enumList = m_simpleTypeEnums.values(typeLevel2);
      }
    }
  }

  return enumList;
}

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
bool XmlService::getXmlAndXsdContent(const QString &xmlfile, const QString &xsdfile,
                                     QString &xmlData, QString &xsdData, QString &error)
{
  DEBUG_LOG("Get xml and xsd content from file ");
  QFile fileXml(xmlfile);
  if (!fileXml.open(QFile::ReadOnly | QFile::Text))
  {
    error = QString("Can not open xml file:") + xmlfile;
    return false;
  }
  QByteArray byteArrXmlData = fileXml.readAll();
  if (byteArrXmlData.size() > 0)
  {
    xmlData = QString::fromStdString(byteArrXmlData.toStdString());
  }
  else
  {
    error = QString("Xml File is empty") + xmlfile;
    return false;
  }
  fileXml.close();

  QFile fileXsd(xsdfile);
  if (!fileXsd.open(QFile::ReadOnly | QFile::Text))
  {
    error = QString("Can not open xsd file:") + xsdfile;
    return false;
  }
  QByteArray byteArrXsdData = fileXsd.readAll();
  if (byteArrXsdData.size() > 0)
  {
    xsdData = QString::fromStdString(byteArrXsdData.toStdString());
  }
  else
  {
    error = QString("Xsd File is empty") + xsdfile;
    return false;
  }
  fileXsd.close();
  return true;
}

/*!
* @fn       validateAndParseXml
* @param    QString xml data
* @param    QString xsd data
* @param    QVariantList& list - return
* @param    QString& error - return
* @return   bool
* @brief    function for xml validation.
*/
bool XmlService::validateAndParseXml(QString xmlData, QString xsdData,
                                     QVariantList& xmlparamlist, QString &error)
{
  QString errMsg;

  if ((XmlService::validateXml(errMsg, xmlData.toUtf8(), xsdData.toUtf8())))
  {
    return parseXml(errMsg, xmlData.toUtf8(), xmlparamlist);
  }
  else
  {
    error = QString("Xml-Xsd validation failed.");
  }
  return false;
}
/*!
* @fn       generateXmlFile
* @param    QString
* @param    const QString
* @param    const QString
* @return   bool
* @brief    write data into xmlFile.
*/
bool XmlService::generateXmlFile(QString xmlFileName, const QString &xmlData, const QString &xsdData)
{
  QString errMsg;
  if (!(XmlService::validateXml(errMsg, xmlData.toUtf8(), xsdData.toUtf8())))
  {
    return false;
  }
  else
  {
    QFile fileXml(xmlFileName);
    if (!fileXml.open(QFile::WriteOnly | QFile::Text))
    {
      return false;
    }
    DEBUG_LOG("Configuration Raw XML file:  " << fileXml.fileName().toStdString());
    int len = xmlData.toStdString().length();
    fileXml.write(xmlData.toStdString().c_str(), len);
    fileXml.close();
    return true;
  }
}

/*!
* @fn       ~XmlService
* @param    None
* @return   None
* @brief    Destructor for class XmlService.
*/
XmlService::~XmlService()
{}
}  // end of namespace ws
}  // end of namespace analogic
