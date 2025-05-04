/*!
* @file     modelmanagerremotearchive.cpp
* @author   Agiliad
* @brief    This file contains functions related to ModelManager
*           which handles loading the data from remote archive server.
* @date     June, 30 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/modelmanagerremotearchive.h>
#include <analogic/ws/uihandler/authenticationmodel.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       ModelManagerRemoteArchive
* @param    QObject* - parent
* @return   None
* @brief    Constructor for class ModelManagerRemoteArchive.
*/
ModelManagerRemoteArchive::ModelManagerRemoteArchive(QObject* parent): ModelManager(parent),
    m_restmap()
{
    m_modelEnumOffset = QMLEnums::USERADMIN_MODELS_END + 1;
}

/*!
* @fn       ~ModelManagerRemoteArchive
* @param    None
* @return   None
* @brief    Destructor for class ModelManagerRemoteArchive.
*/
ModelManagerRemoteArchive::~ModelManagerRemoteArchive()
{
}

/*!
* @fn       init
* @param    None
* @return   None
* @brief    on thread start this function will initialize models.
*/
void ModelManagerRemoteArchive::init()
{
    DEBUG_LOG("Initializing modelmanager");
    ModelManager::init();

    DEBUG_LOG("Initializing rest map");
    initRestMap();

    DEBUG_LOG("Creating instance for handling array of models");
    m_modelsUpdater = new ModelUpdater*[QMLEnums::REMOTEARCHIVE_MODELS_END-m_modelEnumOffset];
    THROW_IF_FAILED((m_modelsUpdater == NULL)?Errors::E_OUTOFMEMORY:Errors::S_OK);

    DEBUG_LOG("Creating instance of authentication model");
    m_modelsUpdater[QMLEnums::REMOTEARCHIVE_AUTHENTICATION_MODEL - m_modelEnumOffset] = new Authenticationmodel
            (QMLEnums::REMOTEARCHIVE_AUTHENTICATION_MODEL, m_restmap[QMLEnums::REMOTEARCHIVE_AUTHENTICATION_MODEL]);
    THROW_IF_FAILED(
                (m_modelsUpdater[QMLEnums::REMOTEARCHIVE_AUTHENTICATION_MODEL - m_modelEnumOffset] == NULL)?
                Errors::E_OUTOFMEMORY : Errors::S_OK);

    connect(m_modelsUpdater[QMLEnums::REMOTEARCHIVE_AUTHENTICATION_MODEL - m_modelEnumOffset],
            SIGNAL(authstatuschanged(int, QString, int)),
            this,
            SLOT(onauthstatuschanged(int, QString, int)));

    DEBUG_LOG("Creating modelupdater for modelnum: "<<QMLEnums::REMOTEARCHIVE_BAGLIST_MODEL << " with resturi: "<< "urlfromserver");
    m_modelsUpdater[QMLEnums::REMOTEARCHIVE_BAGLIST_MODEL-m_modelEnumOffset] = new ModelUpdater(
                QMLEnums::REMOTEARCHIVE_BAGLIST_MODEL, m_restmap[QMLEnums::REMOTEARCHIVE_BAGLIST_MODEL]);
    THROW_IF_FAILED((m_modelsUpdater[QMLEnums::REMOTEARCHIVE_BAGLIST_MODEL-m_modelEnumOffset] == NULL)?
                Errors::E_OUTOFMEMORY:Errors::S_OK);

    DEBUG_LOG("Connecting modelupdater signal with its handler.");
    THROW_IF_FAILED((m_modelsUpdater[1] == NULL)?Errors::E_POINTER:Errors::S_OK);
    if (m_modelsUpdater[1] != NULL)
    {
        connect(m_modelsUpdater[1], SIGNAL(
                    updateModel(QVariantList, QMLEnums::ModelEnum)), this, SLOT(
                    onUpdateModels(QVariantList, QMLEnums::ModelEnum)));

        connect(m_modelsUpdater[1], SIGNAL(
                    authstatuschanged(int, QString, int )), this, SLOT(
                    onauthstatuschanged(int, QString, int)));
    }
}

/*!
 * @fn       initRestMap();
 * @param    None
 * @return   None
 * @brief    Initialize model rest mapping
 */
void ModelManagerRemoteArchive::initRestMap()
{
    DEBUG_LOG("Initialize rest map for all models");
    m_restmap[QMLEnums::REMOTEARCHIVE_AUTHENTICATION_MODEL]                = "credentials";
    m_restmap[QMLEnums::REMOTEARCHIVE_BAGLIST_MODEL]                       = "bagrequest/query";
}

/*!
 * @fn       onUpdateModels
 * @param    QVariantList
 * @param    QMLEnums::ModelEnum model name
 * @return   void
 * @brief    This slot will call on data updation from rest.
 */
void ModelManagerRemoteArchive::onUpdateModels(QVariantList list, QMLEnums::ModelEnum  modelname)
{
    DEBUG_LOG("Send update signal for modelname: "<< modelname << " with list count: "<< list.count());
    emit updateModels(list, modelname);
}

/*!
 * @fn      onExit
 * @param   None
 * @return  None
 * @brief Function will call on exit of threads
 */
void ModelManagerRemoteArchive::onExit()
{
    DEBUG_LOG("Calling exit on ModelManager");
    ModelManager::onExit();
    DEBUG_LOG("Destroying modelupdater array");
    for (int i = QMLEnums::REMOTEARCHIVE_AUTHENTICATION_MODEL; i < QMLEnums::REMOTEARCHIVE_MODELS_END; i++){
        SAFE_DELETE( m_modelsUpdater[QMLEnums::ModelEnum(i)-m_modelEnumOffset]);
    }
    SAFE_DELETE( m_modelsUpdater);
}
}  // end of namespace ws
}  // end of namespace analogic
