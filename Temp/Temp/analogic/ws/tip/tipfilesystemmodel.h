/*!
* @file     tipfilesystemmodel.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to tip fileSystemModel
*           which display and update data to screen.
* @date     Jul 24, 2018
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_TipFileSystemModel_H_
#define ANALOGIC_WS_UIHANDLER_TipFileSystemModel_H_

#include <qmap.h>
#include <qdebug.h>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QApplication>
#include <analogic/ws/common/utility/xmlservice.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

#define ZERO 0
#define CATEGORY_NAME               "tipcategory"
#define TAG_NAME                    "name"
#define TAG_PERCENTAGE              "percentage"
#define TAG_SELECTION               "selection"
#define TAG_DESCRIPTION             "description"
#define SUB_CATEGORY_NAME           "tipsubcategory"
#define TIP_CATEGORY_NAME           "tip"

#define TIP_MAX_PERCENTAGE          100

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   TipFileSystemModel
 * \brief   This class contains variable and function related to
 *          tip file system model
 */
class TipFileSystemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    /*!
    * @fn       TipFileSystemModel
    * @param    QObject *parent
    * @return   None
    * @brief    Constructor for class TipFileSystemModel.
    */
    explicit TipFileSystemModel(QObject *parent = Q_NULLPTR);

    /*!
    * @fn       TipFileSystemModel
    * @param    None
    * @return   None
    * @brief    Destructor for class TipFileSystemModel.
    */
    ~TipFileSystemModel();


    /*!
    * @enum     TipModelRoles
    * @brief    Represents defined role for tree view column.
    */
    enum TipModelRoles
    {
        TipIsChangeRole     =   Qt::CheckStateRole,
        TipFileSystemRole   =   Qt::DisplayRole,
        TipPercentageRole   =   Qt::WhatsThisRole,
        TipDescriptionRole  =   Qt::AccessibleDescriptionRole,
    };

    Q_ENUM(TipModelRoles)

    /*!
    * @fn       roleNames
    * @param    None
    * @return   QHash<int,QByteArray>
    * @brief    gives roleNames.
    */
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /*!
    * @fn       updateButtonTipObjectConfig
    * @param    None
    * @return   bool
    * @brief    Invoked when update button is clicked from Tip Object Config screen.
    */
    Q_INVOKABLE bool updateButtonTipObjectConfig();

    /*!
    * @fn       updateButtonTipObjectLib
    * @param    None
    * @return   bool
    * @brief    Invoked when update button is clicked from Tip Object Lib screen.
    */
     Q_INVOKABLE bool updateButtonTipObjectLib();

    /*!
    * @fn       setModel
    * @param    TipFileSystemModel pointer
    * @return   None
    * @brief    Set model pointers from View class.
    */
    void setModel(TipFileSystemModel*);

    /*!
    * @fn       updateModelPercentage
    * @param    const QModelIndex&
    * @param    const QVariant&
    * @return   None
    * @brief    Update Model percentage text value
    */
    Q_INVOKABLE void updateModelPercentage(const QModelIndex &index, const QVariant &value);

    /*!
    * @fn       updateChekboxState
    * @param    const QModelIndex&
    * @param    const QVariant&
    * @return   None
    * @brief    Update checkbox state value
    */
    Q_INVOKABLE void updateChekboxState(const QModelIndex &index, const QVariant &value);

    /*!
    * @fn       parseTipObjConfig
    * @param    None
    * @return   None
    * @brief    Read xml data and display into screen
    */
    Q_INVOKABLE void parseTipObjConfig();

    /*!
    * @fn       parseTipLibConfig
    * @param    None
    * @return   None
    * @brief    Read xml data and display into screen
    */
    Q_INVOKABLE void parseTipLibConfig();


    /*!
    * @fn       clearModelData
    * @param    None
    * @return   None
    * @brief    clears all data of TipFileSystemModel.
    */
    Q_INVOKABLE void clearModelData();

    /*!
    * @fn       getObjConfigXMLData
    * @param    None
    * @return   QString
    * @brief    get Xml Data.
    */
    Q_INVOKABLE QString getObjConfigXMLData();

    /*!
    * @fn       getObjLibXMLData
    * @param    None
    * @return   QString
    * @brief    get Xml Data.
    */
    Q_INVOKABLE QString getObjLibXMLData();



    /*!
    * @fn       validatePercentageVal
    * @param    None
    * @return   QVariant
    * @brief    Validate sum of percentage value should be 100 for each category level.
    */
    Q_INVOKABLE QVariant validatePercentageVal();

    /*!
    * @fn       getAllChildElements
    * @param    None
    * @return   QVariant
    * @brief    Get all child elements of tree into a list.
    */
    Q_INVOKABLE QVariant getAllChildElements();

    /*!
    * @fn       getIndexWiseElements
    * @param    QModelIndex
    * @return   QVariant
    * @brief    Get index wise child elements of tree into a list.
    */
    Q_INVOKABLE QVariant getIndexWiseElements(QModelIndex index);

    /*!
    * @fn       getAllChildCount
    * @param    QModelIndex
    * @param    int&
    * @return   int
    * @brief    gets all child count for an index
    */
    Q_INVOKABLE int getAllChildCount(QModelIndex parent, int &hitCount);


private:

    /*!
    * @fn       writeTipObjConfig
    * @param    None
    * @return   bool
    * @brief    Write updated screen data to xml and send data to server
    */
    bool writeTipObjConfig();

    /*!
    * @fn       writeTipObjLib
    * @param    None
    * @return   bool
    * @brief    Write updated screen data to xml and send data to server
    */
    bool writeTipObjLib();

    /*!
    * @fn       buildObjConfigCategoryNode
    * @param    const QString
    * @param    const QString
    * @return   QStandardItem pointer
    * @brief    Add category entry node in tree view model
    */
    QStandardItem* buildObjConfigCategoryNode(const QString category,
                                            const QString categoryPercentage);

    /*!
    * @fn       buildObjConfigSubCategoryNode
    * @param    const QString
    * @param    const QString
    * @param    QStandardItem pointer
    * @return   QStandardItem pointer
    * @brief    Add sub category entry node in tree view model
    */
    QStandardItem* buildObjConfigSubCategoryNode(const QString subcategory,
                                                const QString subCategoryPercentage,
                                                QStandardItem* pCategoryHandle);

    /*!
    * @fn       buildObjLibCategoryNode
    * @param    const QString
    * @param    const bool
    * @return   QStandardItem pointer
    * @brief    Add category entry node in tree view model
    */
    QStandardItem* buildObjLibCategoryNode(const QString category,
                                        const bool categorySelected);


    /*!
    * @fn       buildObjLibSubCategoryNode
    * @param    const QString
    * @param    const bool
    * @param    QStandardItem pointer
    * @return   QStandardItem pointer
    * @brief    Add sub category entry node in tree view model
    */
    QStandardItem* buildObjLibSubCategoryNode(const QString subcategory,
                                            const bool subCategorySelected,
                                            QStandardItem* pCategoryHandle);

    /*!
    * @fn       buildObjLibTipElement
    * @param    const QString
    * @param    const QString
    * @param    const bool
    * @param    QStandardItem pointer
    * @return   QStandardItem pointer
    * @brief    Add tip file category entry node in tree view model
    */
    QStandardItem* buildObjLibTipElement(const QString tipName,
                                        const QString tipDescription,
                                        const bool tipSelected,
                                        QStandardItem* pSubCategoryHandle);

    /*!
    * @fn       setObjConfigXMLData
    * @param    const QString xmlData
    * @return   None
    * @brief    sets xmlData.
    */
    void setObjConfigXMLData(const QString xmlData);

    /*!
    * @fn       setObjLibXMLData
    * @param    const QString xmlData
    * @return   None
    * @brief    sets xmlData.
    */
    void setObjLibXMLData(const QString xmlData);

    TipFileSystemModel*     m_pTipFileSystemUIModel;        //!< file system ui model
    QHash<int, QByteArray>  m_roleNameMapping;              //!< role names
    QVariantList            m_tipXMLList  ;                 //!< handle to xml file data
    QString                 m_tipXMLError ;                 //!< xml file error
    QString                 m_ObjConfigXMLData;             //!< received obj config xml data
    QString                 m_ObjLibXMLData;                //!< received obj lib xml data

};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_TipFileSystemModel_H_


