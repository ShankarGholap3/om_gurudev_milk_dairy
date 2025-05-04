/*!
* @file     dataobject.cpp
* @author   Agiliad
* @brief    This file contains the data object as an element in the variant.
* @date     Oct, 14 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common.h>
#include <analogic/ws/ulm/dataobject.h>

/*!
 * @fn      DataObject
 * @param   Qstring - objectName
 * @param   bool - visibility
 * @param   bool -isenable
 * @param   Qobject*
 * @return  None
 * @brief   This function is constructor for class DataObject
 */
DataObject::DataObject(QString objectNm, bool isVisible, bool isEnable, QObject *parent):QObject(parent)
{
    m_isVisible = isVisible;
    m_isEnable = isEnable;
    m_objectNm = objectNm;
}

/*!
 * @fn      setObjectNm
 * @param   const QString  - name
 * @return  None
 * @brief   This function is setter for m_objectNm
 */
void DataObject::setObjectNm(const QString &name)
{
    DEBUG_LOG("Setting object name to: "<<name.toStdString());
    m_objectNm = name;
    emit objectNmChanged();
}

/*!
 * @fn      setIsVisible
 * @param   const bool - isVisible
 * @return  None
 * @brief   This function is setter for m_isVisible
 */
void DataObject::setIsVisible(const bool &isVisiblity)
{
    DEBUG_LOG("Setting object visibility to: "<<isVisiblity);
    m_isVisible = isVisiblity;
    emit isVisibleChanged();
}

/*!
 * @fn      setIsEnable
 * @param   const bool - m_isEnable
 * @return  None
 * @brief   This function is setter for m_isEnable
 */
void DataObject::setIsEnable(const bool &isEnabled)
{
    DEBUG_LOG("Setting Enable : "<<isEnabled);
    m_isEnable = isEnabled;
    emit isEnableChanged();
}

/*!
 * @fn      getObjectNm
 * @param   None
 * @return  QString  - m_objectNm
 * @brief   This function is getter for m_isEnable
 */
QString DataObject::getObjectNm()
{
    return m_objectNm;
}

/*!
 * @fn      getIsVisible
 * @param   None
 * @return  bool - m_isVisible
 * @brief   This function is getter for m_isVisible
 */
bool DataObject::getIsVisible()
{
    TRACE_LOG("");
    return m_isVisible;
}

/*!
 * @fn      getIsEnable
 * @param   None
 * @return  bool - m_isEnable
 * @brief   This function is getter for m_isEnable
 */
bool DataObject::getIsEnable()
{
    TRACE_LOG("");
    return m_isEnable;
}
