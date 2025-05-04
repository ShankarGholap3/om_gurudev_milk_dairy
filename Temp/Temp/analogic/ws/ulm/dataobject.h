/*!
* @file     dataobject.h
* @author   Agiliad
* @brief    This file contains the data object as an element in the variant.
* @date     Oct, 14 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_ULM_DATAOBJECT_H_
#define ANALOGIC_WS_ULM_DATAOBJECT_H_

#include <QObject>

/*!
 * \class   DataObject
 * \brief   This is data object as an element in the variant.
*           passed to the QML.Each transferable member have the Q_PROPERTY mapping.
*           ULM will convert/create this class object before sending the ui component visibility
*           list to QML.
* \attention Please be careful while refactoring the members of this class , respective refactoring
* should be done in QML code.
* \
 */
class DataObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString m_objectNm READ getObjectNm WRITE setObjectNm NOTIFY objectNmChanged)
    Q_PROPERTY(bool m_isVisible READ getIsVisible WRITE setIsVisible NOTIFY isVisibleChanged)
    Q_PROPERTY(bool m_isEnable READ getIsEnable WRITE setIsEnable NOTIFY isEnableChanged)
public:
    /*!
     * @fn      DataObject
     * @param   Qstring - objectName
     * @param   bool - visibility
     * @param   bool -isenable
     * @param   Qobject*
     * @return  None
     * @brief   This function is constructor for class DataObject
     */
    explicit DataObject(QString, bool, bool m_isEnable, QObject *parent = 0);

    /*!
     * @fn      setObjectNm
     * @param   const QString  - name
     * @return  None
     * @brief   This function is setter for m_objectNm
     */
    void setObjectNm(const QString &name);

    /*!
     * @fn      setIsVisible
     * @param   const bool - isVisible
     * @return  None
     * @brief   This function is setter for m_isVisible
     */
    void setIsVisible(const bool &m_isVisible);

    /*!
     * @fn      setIsEnable
     * @param   const bool - m_isEnable
     * @return  None
     * @brief   This function is setter for m_isEnable
     */
    void setIsEnable(const bool &m_isEnable);

    /*!
     * @fn      getObjectNm
     * @param   None
     * @return  QString  - m_objectNm
     * @brief   This function is getter for m_isEnable
     */
    QString getObjectNm();

    /*!
     * @fn      getIsVisible
     * @param   None
     * @return  bool - m_isVisible
     * @brief   This function is getter for m_isVisible
     */
    bool getIsVisible();

    /*!
     * @fn      getIsEnable
     * @param   None
     * @return  bool - m_isEnable
     * @brief   This function is getter for m_isEnable
     */
    bool getIsEnable();

signals:
    void objectNmChanged();
    void isVisibleChanged();
    void isEnableChanged();

public slots:

private:
    bool       m_isVisible;    //!< visibility flag of UI Component
    bool       m_isEnable;     //!< enable flag of UI Component
    QString    m_objectNm;     //!< object name of UI Component
};

#endif  // ANALOGIC_WS_ULM_DATAOBJECT_H_


