/*!
* @file     tipfilesystemmodel.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to tip fileSystemModel
*           which display and update data to screen.
* @date     Jul 24, 2018
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QUrl>
#include <QTextStream>

#include <analogic/ws/tip/tipfilesystemmodel.h>

namespace analogic
{
namespace ws
{

/*!
* @fn       TipFileSystemModel
* @param    QObject *parent
* @return   None
* @brief    Constructor for class TipFileSystemModel.
*/
TipFileSystemModel::TipFileSystemModel(QObject *parent)
    : QStandardItemModel(parent),
      m_pTipFileSystemUIModel(nullptr),
      m_tipXMLList(),
      m_tipXMLError(""),
      m_ObjConfigXMLData(""),
      m_ObjLibXMLData("")
{
    m_roleNameMapping[TipIsChangeRole] = "TipIsChangeRole";
    m_roleNameMapping[TipPercentageRole] = "TipPercentageRole";
    m_roleNameMapping[TipDescriptionRole] = "TipDescriptionRole";
    m_roleNameMapping[TipFileSystemRole] = "TipFileSystemModelRole";

    m_pTipFileSystemUIModel = NULL;
}

/*!
* @fn       setObjConfigXMLData
* @param    const QString xmlData
* @return   None
* @brief    sets xmlData.
*/
void TipFileSystemModel::setObjConfigXMLData(const QString xmlData)
{
    m_ObjConfigXMLData = xmlData;
}

/*!
* @fn       getObjConfigXMLData
* @param    None
* @return   QString
* @brief    get Xml Data.
*/
QString TipFileSystemModel::getObjConfigXMLData()
{
    return m_ObjConfigXMLData;
}

/*!
* @fn       setObjLibXMLData
* @param    const QString xmlData
* @return   None
* @brief    sets xmlData.
*/
void TipFileSystemModel::setObjLibXMLData(const QString xmlData)
{
    m_ObjLibXMLData = xmlData;
}
/*!
* @fn       getObjLibXMLData
* @param    None
* @return   QString
* @brief    get Xml Data.
*/
QString TipFileSystemModel::getObjLibXMLData()
{
    return m_ObjLibXMLData;
}

/*!
* @fn       buildObjLibTipElement
* @param    const QString
* @param    const QString
* @param    const bool
* @param    QStandardItem pointer
* @return   QStandardItem pointer
* @brief    Add tip file category entry node in tree view model
*/
QStandardItem* TipFileSystemModel::buildObjLibTipElement(const QString tipName,
                                                   const QString tipDescription,
                                                   const bool tipSelected,
                                                   QStandardItem* pSubCategoryHandle)
{
    int iIndex = 0;
    QString tipItemText = "";

    QStandardItem* tipItem = NULL;
    QStandardItem* tempTipItem = NULL;

    while(NULL == tipItem)
    {
        tempTipItem = pSubCategoryHandle->child(iIndex);
        if(NULL != tempTipItem)
        {
            tipItemText = tempTipItem->text();

            if(0 == QString::compare(tipName, tipItemText))
            {
                tipItem = tempTipItem;
            }
        }
        else
        {
            tipItem = new QStandardItem(tipName);
            tipItem->setData(tipSelected, TipIsChangeRole);
            tipItem->setData(tipDescription, TipDescriptionRole);
            pSubCategoryHandle->appendRow(tipItem);
        }
        iIndex++;
    }
    return tipItem;
}

/*!
* @fn       buildObjLibSubCategoryNode
* @param    const QString
* @param    const bool
* @param    QStandardItem pointer
* @return   QStandardItem pointer
* @brief    Add sub category entry node in tree view model
*/
QStandardItem* TipFileSystemModel::buildObjLibSubCategoryNode(const QString subcategory,
                                                        const bool subCategorySelected,
                                                        QStandardItem* pCategoryHandle)
{
    int iIndex = 0;
    QString subCategoryItemText = NULL;
    QStandardItem* subCategoryItem = NULL;
    QStandardItem* tempSubCategoryItem = NULL;

    while(NULL == subCategoryItem)
    {
        tempSubCategoryItem = pCategoryHandle->child(iIndex);
        if(NULL != tempSubCategoryItem)
        {
            subCategoryItemText  = tempSubCategoryItem->text();

            if(0 == QString::compare(subcategory, subCategoryItemText))
            {
                subCategoryItem = tempSubCategoryItem;
            }
        }
        else
        {
            subCategoryItem = new QStandardItem(subcategory);
            subCategoryItem->setData(subCategorySelected, TipIsChangeRole);

            pCategoryHandle->appendRow(subCategoryItem);
        }
        iIndex++;
    }

    return subCategoryItem;
}

/*!
* @fn       buildObjLibCategoryNode
* @param    const QString
* @param    const bool
* @return   QStandardItem pointer
* @brief    Add category entry node in tree view model
*/
QStandardItem* TipFileSystemModel::buildObjLibCategoryNode(const QString category,
                                                         const bool categorySelected)
{
    QStandardItem* categoryItem = NULL;
    auto categoryList = this->findItems(category);
    if (0 < categoryList.count())
    {
        categoryItem = categoryList.at(0);
    }
    else
    {
        categoryItem = new QStandardItem(category);
        categoryItem->setData(categorySelected, TipIsChangeRole);

        this->appendRow(categoryItem);
    }
    return categoryItem;
}

/*!
* @fn       buildObjConfigSubCategoryNode
* @param    const QString
* @param    const QString
* @param    QStandardItem pointer
* @return   QStandardItem pointer
* @brief    Add sub category entry node in tree view model
*/
QStandardItem* TipFileSystemModel::buildObjConfigSubCategoryNode(const QString subcategory,
                                                                const QString subCategoryPercentage,
                                                                QStandardItem* pCategoryHandle)
{
    int iIndex = 0;
    QString subCategoryItemText = NULL;
    QStandardItem* subCategoryItem = NULL;
    QStandardItem* tempSubCategoryItem = NULL;

    while(NULL == subCategoryItem)
    {
        tempSubCategoryItem = pCategoryHandle->child(iIndex);
        if(NULL != tempSubCategoryItem)
        {
            subCategoryItemText  = tempSubCategoryItem->text();

            if(0 == QString::compare(subcategory, subCategoryItemText))
            {
                subCategoryItem = tempSubCategoryItem;
            }
        }
        else
        {
            subCategoryItem = new QStandardItem(subcategory);
            subCategoryItem->setData(subCategoryPercentage, TipPercentageRole);

            pCategoryHandle->appendRow(subCategoryItem);
        }
        iIndex++;
    }

    return subCategoryItem;
}

/*!
* @fn       buildObjConfigCategoryNode
* @param    const QString
* @param    const QString
* @return   QStandardItem pointer
* @brief    Add category entry node in tree view model
*/
QStandardItem* TipFileSystemModel::buildObjConfigCategoryNode(const QString category,
                                                    const QString categoryPercentage)
{
    QStandardItem* categoryItem = NULL;
    auto categoryList = this->findItems(category);
    if (0 < categoryList.count())
    {
        categoryItem = categoryList.at(0);
    }
    else
    {
        categoryItem = new QStandardItem(category);
        categoryItem->setData(categoryPercentage, TipPercentageRole);

        this->appendRow(categoryItem);
    }
    return categoryItem;
}


/*!
* @fn       parseTipObjConfig
* @param    None
* @return   None
* @brief    Read xml data and display into screen
*/
void TipFileSystemModel::parseTipObjConfig()
{
    QString sXmlFilePath = QApplication::applicationDirPath()
            + (QString)FILE_ROOT_FOLDER
            + (QString)TIP_FOLDER
            + (QString)TIP_OBJECT_CONFIG_XML_FILE;

    QDomDocument doc;
    QFile fileXml(sXmlFilePath);

    if (!fileXml.open(QIODevice::ReadOnly) || !doc.setContent(&fileXml))
    {
        ERROR_LOG("Can not open xml file : "<<TIP_OBJECT_CONFIG_XML_FILE);
    }
    else
    {
        QString attrName;
        QString attrPercentage;
        QDomElement elemRoot;
        QDomElement elemCategory;
        QDomElement elemSubCategory;

        QStandardItem* pCategoryHandle;
        elemRoot = doc.documentElement();
        elemCategory = elemRoot.firstChildElement(CATEGORY_NAME);
        while(!elemCategory.isNull())
        {
            attrName = elemCategory.firstChildElement(TAG_NAME).text();
            attrPercentage = elemCategory.firstChildElement(TAG_PERCENTAGE).text();
            pCategoryHandle = buildObjConfigCategoryNode(attrName, attrPercentage);

            elemSubCategory = elemCategory.firstChildElement(SUB_CATEGORY_NAME);
            while(!elemSubCategory.isNull())
            {
                 attrName = elemSubCategory.firstChildElement(TAG_NAME).text();
                 attrPercentage = elemSubCategory.firstChildElement(TAG_PERCENTAGE).text();
                 buildObjConfigSubCategoryNode(attrName, attrPercentage, pCategoryHandle);
                 elemSubCategory = elemSubCategory.nextSiblingElement(SUB_CATEGORY_NAME);
            }
            elemCategory = elemCategory.nextSiblingElement(CATEGORY_NAME);
        }
    }
}

/*!
* @fn       writeTipObjConfig
* @param    None
* @return   bool
* @brief    Write updated screen data to xml and send data to server
*/
bool TipFileSystemModel::writeTipObjConfig()
{
    QDomDocument doc;
    QString sXmlFilePath = QApplication::applicationDirPath()
            + (QString)FILE_ROOT_FOLDER
            + (QString)TIP_FOLDER
            + (QString)TIP_OBJECT_CONFIG_XML_FILE;

    QFile fileXml(sXmlFilePath);

    if (!fileXml.open(QIODevice::ReadOnly) || !doc.setContent(&fileXml))
    {
        ERROR_LOG("Can not open xml file: " << TIP_OBJECT_CONFIG_XML_FILE);
        fileXml.close();
        return false;
    }
    else
    {
        fileXml.close();
        QDomElement elemRoot;
        QDomElement elemCategory;
        QDomElement elemSubCategory;
        QModelIndex parent;

        elemRoot = doc.documentElement();
        elemCategory = elemRoot.firstChildElement(CATEGORY_NAME);
        for(int r = 0; r < m_pTipFileSystemUIModel->rowCount(parent); ++r)
        {
            QModelIndex index = m_pTipFileSystemUIModel->index(r, 0, parent);
            QVariant varPercentage = m_pTipFileSystemUIModel->data(index, TipPercentageRole);
            elemCategory.firstChildElement(TAG_PERCENTAGE).firstChild().setNodeValue(varPercentage.toString());
            if(m_pTipFileSystemUIModel->hasChildren(index))
            {
                elemSubCategory = elemCategory.firstChildElement(SUB_CATEGORY_NAME);
                QModelIndex child =  index;
                for(int j = 0; j < m_pTipFileSystemUIModel->rowCount(child); ++j)
                {
                    QModelIndex newIndex = m_pTipFileSystemUIModel->index(j, 0, child);
                    varPercentage = m_pTipFileSystemUIModel->data(newIndex, TipPercentageRole);
                    elemSubCategory.firstChildElement(TAG_PERCENTAGE).firstChild().setNodeValue(varPercentage.toString());
                    elemSubCategory = elemSubCategory.nextSiblingElement(SUB_CATEGORY_NAME);
                }
            }
            elemCategory = elemCategory.nextSiblingElement(CATEGORY_NAME);
        }
    }
    QByteArray xmlData = doc.toByteArray();
    if (!fileXml.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        ERROR_LOG("Lost contents of xml : " << TIP_OBJECT_CONFIG_XML_FILE);
        return false;
    }
    setObjConfigXMLData(QString::fromStdString(xmlData.toStdString()));
    fileXml.write(xmlData);
    fileXml.close();
    return true;
}


/*!
* @fn       parseTipLibConfig
* @param    None
* @return   None
* @brief    Read xml data and display into screen
*/
void TipFileSystemModel::parseTipLibConfig()
{
    QString sXmlFilePath = QApplication::applicationDirPath()
            + (QString)FILE_ROOT_FOLDER
            + (QString)TIP_FOLDER
            + (QString)TIP_OBJECT_LIB_XML_FILE;
    QDomDocument doc;
    QFile fileXml(sXmlFilePath);

    if (!fileXml.open(QIODevice::ReadOnly) || !doc.setContent(&fileXml))
    {
        ERROR_LOG("Can not open xml file : "<<TIP_OBJECT_LIB_XML_FILE);
    }
    else
    {
        QString attrName;
        QString attrDescription;
        QString attrSelected;
        QDomElement elemRoot;
        QDomElement elemCategory;
        QDomElement elemSubCategory;
        QDomElement elemTipCategory;
        bool bSelected = false;

        QStandardItem* pCategoryHandle;
        QStandardItem* pSubCategoryHandle;

        elemRoot = doc.documentElement();
        elemCategory = elemRoot.firstChildElement(CATEGORY_NAME);
        while(!elemCategory.isNull())
        {
            attrName = elemCategory.firstChildElement(TAG_NAME).text();
            attrSelected = elemCategory.firstChildElement(TAG_SELECTION).text();
            bSelected = ((attrSelected == "true") ? true : false);

            pCategoryHandle = buildObjLibCategoryNode(attrName, bSelected);
            elemSubCategory = elemCategory.firstChildElement(SUB_CATEGORY_NAME);
            while(!elemSubCategory.isNull())
            {
                 attrName = elemSubCategory.firstChildElement(TAG_NAME).text();
                 attrSelected = elemSubCategory.firstChildElement(TAG_SELECTION).text();
                 bSelected = ((attrSelected == "true") ? true : false);

                 pSubCategoryHandle = buildObjLibSubCategoryNode(attrName, bSelected, pCategoryHandle);
                 elemTipCategory = elemSubCategory.firstChildElement(TIP_CATEGORY_NAME);
                 while(!elemTipCategory.isNull())
                 {
                      attrName = elemTipCategory.firstChildElement(TAG_NAME).text();
                      attrDescription = elemTipCategory.firstChildElement(TAG_DESCRIPTION).text();
                      attrSelected = elemTipCategory.firstChildElement(TAG_SELECTION).text();
                      bSelected = ((attrSelected == "true") ? true : false);

                      buildObjLibTipElement(attrName, attrDescription, bSelected, pSubCategoryHandle);
                      elemTipCategory = elemTipCategory.nextSiblingElement(TIP_CATEGORY_NAME);
                 }
                 elemSubCategory = elemSubCategory.nextSiblingElement(SUB_CATEGORY_NAME);
            }
            elemCategory = elemCategory.nextSiblingElement(CATEGORY_NAME);
        }
    }
}

/*!
* @fn       writeTipObjLib
* @param    None
* @return   bool
* @brief    Write updated screen data to xml and send data to server
*/
bool TipFileSystemModel::writeTipObjLib()
{
    QDomDocument doc;
    QString sXmlFilePath = QApplication::applicationDirPath()
            + (QString)FILE_ROOT_FOLDER
            + (QString)TIP_FOLDER
            + (QString)TIP_OBJECT_LIB_XML_FILE;

    QFile fileXml(sXmlFilePath);

    if (!fileXml.open(QIODevice::ReadOnly) || !doc.setContent(&fileXml))
    {
        ERROR_LOG("Can not open xml file: " << TIP_OBJECT_LIB_XML_FILE);
        fileXml.close();
        return false;
    }
    else
    {
        fileXml.close();
        QDomElement elemRoot;
        QDomElement elemCategory;
        QDomElement elemSubCategory;
        QDomElement elemTipCategory;
        QModelIndex parent;

        elemRoot = doc.documentElement();
        elemCategory = elemRoot.firstChildElement(CATEGORY_NAME);
        for(int r = 0; r < m_pTipFileSystemUIModel->rowCount(parent); ++r)
        {
            QModelIndex index = m_pTipFileSystemUIModel->index(r, 0, parent);
            QVariant varChecbox = m_pTipFileSystemUIModel->data(index, TipIsChangeRole);
            elemCategory.firstChildElement(TAG_SELECTION).firstChild().setNodeValue(varChecbox.toString());
            if(m_pTipFileSystemUIModel->hasChildren(index))
            {
                elemSubCategory = elemCategory.firstChildElement(SUB_CATEGORY_NAME);
                QModelIndex child =  index;
                for(int j = 0; j < m_pTipFileSystemUIModel->rowCount(child); ++j)
                {
                    QModelIndex newIndex = m_pTipFileSystemUIModel->index(j, 0, child);
                    varChecbox = m_pTipFileSystemUIModel->data(newIndex, TipIsChangeRole);
                    elemSubCategory.firstChildElement(TAG_SELECTION).firstChild().setNodeValue(varChecbox.toString());
                    if(m_pTipFileSystemUIModel->hasChildren(newIndex))
                    {
                        elemTipCategory = elemSubCategory.firstChildElement(TIP_CATEGORY_NAME);
                        QModelIndex grandChild =  newIndex;
                        for(int k = 0; k < m_pTipFileSystemUIModel->rowCount(grandChild); ++k)
                        {
                            QModelIndex newIndex2 = m_pTipFileSystemUIModel->index(k, 0, grandChild);
                            varChecbox = m_pTipFileSystemUIModel->data(newIndex2, TipIsChangeRole);
                            elemTipCategory.firstChildElement(TAG_SELECTION).firstChild().setNodeValue(varChecbox.toString());
                            elemTipCategory = elemTipCategory.nextSiblingElement(TIP_CATEGORY_NAME);
                        }
                    }
                    elemSubCategory = elemSubCategory.nextSiblingElement(SUB_CATEGORY_NAME);
                }
            }
            elemCategory = elemCategory.nextSiblingElement(CATEGORY_NAME);
        }
    }
    QByteArray xmlData = doc.toByteArray();
    if (!fileXml.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        ERROR_LOG("Lost contents of xml : " << TIP_OBJECT_LIB_XML_FILE);
        return false;
    }
    setObjLibXMLData(QString::fromStdString(xmlData.toStdString()));
    fileXml.write(xmlData);
    fileXml.close();
    return true;
}

/*!
* @fn       validatePercentageVal
* @param    None
* @return   QVariant
* @brief    Validate sum of percentage value should be 100 for each category level.
*/
QVariant TipFileSystemModel::validatePercentageVal()
{
    QModelIndex parent;
    QVariant retVal = "";
    int sumCategory = 0;
    for(int r = 0; r < m_pTipFileSystemUIModel->rowCount(parent); ++r)
    {
        QModelIndex index = m_pTipFileSystemUIModel->index(r, 0, parent);
        QVariant var = m_pTipFileSystemUIModel->data(index, TipPercentageRole);
        QString sPercentage = var.toString();
        if(sPercentage == "")
        {
           retVal = "UndefinedValue";
           return retVal;
        }
        QVariant tagNameCategory = m_pTipFileSystemUIModel->data(index, TipFileSystemRole);
        sumCategory = sumCategory + sPercentage.toInt();
        if(m_pTipFileSystemUIModel->hasChildren(index))
        {
            int sumSubCategory = 0;
            QModelIndex child =  index;
            for(int j = 0; j < m_pTipFileSystemUIModel->rowCount(child); ++j)
            {
                QModelIndex newIndex = m_pTipFileSystemUIModel->index(j, 0, child);
                var = m_pTipFileSystemUIModel->data(newIndex, TipPercentageRole);
                sPercentage = var.toString();
                if(sPercentage == "")
                {
                   retVal = "UndefinedValue";
                   return retVal;
                }
                sumSubCategory = sumSubCategory + sPercentage.toInt();
            }
            if(sumSubCategory != TIP_MAX_PERCENTAGE)
            {
                ERROR_LOG("Sum of all sub category percentage of "
                          << tagNameCategory.toString().toStdString() << " is not 100 percent.");
                retVal = tagNameCategory;
                return retVal;
            }
        }
    }
    if(sumCategory != TIP_MAX_PERCENTAGE)
    {
        ERROR_LOG("Sum of all category percentage is not 100 percent");
        retVal = "CategorySumError";
        return retVal;
    }
    return retVal;
}
/*!
* @fn       setModel
* @param    TipFileSystemModel pointer
* @return   None
* @brief    Set model pointers from View class.
*/
void TipFileSystemModel::setModel(TipFileSystemModel* pUIModel)
{
    m_pTipFileSystemUIModel = pUIModel;
}

/*!
* @fn       updateModelPercentage
* @param    const QModelIndex&
* @param    const QVariant&
* @return   None
* @brief    Update Model percentage text value
*/
void TipFileSystemModel::updateModelPercentage(const QModelIndex &index, const QVariant &value)
{
    m_pTipFileSystemUIModel->setData(index, value, TipPercentageRole);
}

/*!
* @fn       updateChekboxState
* @param    const QModelIndex&
* @param    const QVariant&
* @return   None
* @brief    Update checkbox state value
*/
void TipFileSystemModel::updateChekboxState(const QModelIndex &index, const QVariant &value)
{

    m_pTipFileSystemUIModel->setData(index, value, TipIsChangeRole);
    if(m_pTipFileSystemUIModel->hasChildren(index))
    {
        QModelIndex child =  index;
        for(int j = 0; j < m_pTipFileSystemUIModel->rowCount(child); ++j)
        {
            QModelIndex secondIndex = m_pTipFileSystemUIModel->index(j, 0, child);
            m_pTipFileSystemUIModel->setData(secondIndex, value, TipIsChangeRole);

            if(m_pTipFileSystemUIModel->hasChildren(secondIndex))
            {
                QModelIndex nextChild =  secondIndex;
                for(int k = 0; k < m_pTipFileSystemUIModel->rowCount(nextChild); ++k)
                {
                    QModelIndex thirdIndex = m_pTipFileSystemUIModel->index(k, 0, nextChild);
                    m_pTipFileSystemUIModel->setData(thirdIndex, value, TipIsChangeRole);
                }
            }
        }
    }
}

/*!
* @fn       updateButtonTipObjectConfig
* @param    None
* @return   bool
* @brief    Invoked when update button is clicked from QML screen.
*/
bool TipFileSystemModel::updateButtonTipObjectConfig()
{
    return writeTipObjConfig();
}

/*!
* @fn       updateButtonTipObjectLib
* @param    None
* @return   bool
* @brief    Invoked when update button is clicked from Tip Object Lib screen.
*/
bool TipFileSystemModel::updateButtonTipObjectLib()
{
    return writeTipObjLib();
}

/*!
* @fn       TipFileSystemModel
* @param    None
* @return   None
* @brief    Destructor for class TipFileSystemModel.
*/
TipFileSystemModel::~TipFileSystemModel()
{
    m_pTipFileSystemUIModel = NULL;
}

/*!
* @fn       roleNames
* @param    None
* @return   QHash<int,QByteArray>
* @brief    gives roleNames.
*/
QHash<int,QByteArray> TipFileSystemModel::roleNames() const
{
    return m_roleNameMapping;
}

/*!
* @fn       clearModelData
* @param    None
* @return   None
* @brief    clears all data of TipFileSystemModel.
*/
void TipFileSystemModel::clearModelData()
{
    this->clear();
}

/*!
* @fn       getAllChildElements
* @param    None
* @return   QVariant
* @brief    Get all child elements of tree into a list.
*/
QVariant TipFileSystemModel::getAllChildElements()
{
    QModelIndexList indexes = m_pTipFileSystemUIModel->match(m_pTipFileSystemUIModel->index(0,0),
                                                             Qt::DisplayRole,
                                                             "*",
                                                             -1,
                                                             Qt::MatchWildcard|Qt::MatchRecursive);
    return QVariant::fromValue(indexes);
}

/*!
* @fn       getIndexWiseElements
* @param    QModelIndex
* @return   QVariant
* @brief    Get index wise child elements of tree into a list.
*/
QVariant TipFileSystemModel::getIndexWiseElements(QModelIndex index)
{
    int hitCount = 0;
    getAllChildCount(index, hitCount);
    QModelIndexList indexes = m_pTipFileSystemUIModel->match(index,
                                                             Qt::DisplayRole,
                                                             "*",
                                                             hitCount,
                                                             Qt::MatchWildcard|Qt::MatchRecursive);
    return QVariant::fromValue(indexes);
}

/*!
* @fn       getAllChildCount
* @param    QModelIndex
* @param    int&
* @return   int
* @brief    gets all child count for an index
*/
int TipFileSystemModel::getAllChildCount(QModelIndex parent, int &hitCount)
{
    for(int r = 0; r < m_pTipFileSystemUIModel->rowCount(parent); ++r)
    {
        hitCount++;
        QModelIndex index = m_pTipFileSystemUIModel->index(r, 0, parent);
        if( m_pTipFileSystemUIModel->hasChildren(index) )
        {
            getAllChildCount(index, hitCount);
        }
    }
    return hitCount;
}

}  // namespace ws
}  // namespace analogic
