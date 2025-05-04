/*!
* @file     modelmanagerremotearchive.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to ModelManager
*           which handles loading the data from remote archive server.
* @date     June, 30 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_ModelManagerRemoteArchive_H_
#define ANALOGIC_WS_UIHANDLER_ModelManagerRemoteArchive_H_

#include <QTimer>
#include <analogic/ws/uihandler/modelmanager.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   ModelManagerRemoteArchive
 * \brief   This class contains variable and function related to
 *          loading the data from remote archive server.
 */
class ModelManagerRemoteArchive: public ModelManager
{
    Q_OBJECT
public:
    /*!
    * @fn       ModelManagerRemoteArchive
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ModelManagerRemoteArchive.
    */
    explicit ModelManagerRemoteArchive(QObject *parent = NULL);

    /*!
    * @fn       ModelManagerRemoteArchive
    * @param    None
    * @return   None
    * @brief    Destructor for class ModelManagerRemoteArchive.
    */
    virtual ~ModelManagerRemoteArchive();

    /*!
     * @fn       init
     * @param    None
     * @return   None
     * @brief    on thread start this function will initialize models.
     */
    void init();

    /*!
     * @fn      onExit
     * @param   None
     * @return  None
     * @brief Function will call on exit of threads
     */
    void onExit();

public slots:
    /*!
     * @fn       onUpdateModels
     * @param    QVariantList
     * @param    QMLEnums::ModelEnum model name
     * @return   void
     * @brief    This slot will call on data updation from rest.
     */
    void onUpdateModels(QVariantList map, QMLEnums::ModelEnum modelname);

private:
    /*!
    * @fn       ModelManagerRemoteArchive();
    * @param    ModelManagerRemoteArchive&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ModelManagerRemoteArchive(const ModelManagerRemoteArchive& ModelManagerRemoteArchive);

    /*!
    * @fn       operator=
    * @param    ModelManagerRemoteArchive&
    * @return   ModelManagerRemoteArchive&
    * @brief    declaration for private assignment operator.
    */
    ModelManagerRemoteArchive& operator= (const ModelManagerRemoteArchive& ModelManagerRemoteArchive);

    /*!
     * @fn       initRestMap
     * @param    None
     * @return   None
     * @brief    Initialize model rest mapping.
     */
    void initRestMap();

    QMap<QMLEnums::ModelEnum, QString>      m_restmap;              //!< map for rest URL
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_ModelManagerRemoteArchive_H_


