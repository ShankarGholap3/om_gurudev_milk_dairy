/*!
* @file     taskinfomodel.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to TaskInfoModel
*           which handles task info.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_TASKINFOMODEL_H_
#define ANALOGIC_WS_UIHANDLER_TASKINFOMODEL_H_

#include <analogic/ws/uihandler/modelupdater.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   TaskInfoModel
 * \brief   This class contains variable and function related to
 *          handles of task info.
 */

class TaskInfoModel : public ModelUpdater
{
public:
    /*!
    * @fn       TaskInfoModel
    * @param    QMLEnums::ModelEnum - model number
    * @param    QString - url
    * @param    QByteArray contentType
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor for class CommandModel.
    */
    TaskInfoModel(QMLEnums::ModelEnum modelnumber, QString strresturl, QByteArray contentType, QObject* parent = NULL);

    /*!
    * @fn       setTaskID
    * @param    QString - sTaskID
    * @return   None
    * @brief    This function is responsible for setting TaskID.
    */
    void setTaskID(QString sTaskID);

    /*!
    * @fn       getTaskID
    * @param    None
    * @return   QString - sTaskID
    * @brief    This function is responsible for getting TaskID.
    */
    QString getTaskID();

    /*!
    * @fn       getrequest();
    * @param    QString - sUrl
    * @return   None
    * @brief    This function is responsible for updating model.
    */
    void getrequest(QString sUrl = "");

    /*!
    * @fn       postrequest();
    * @param    None
    * @return   None
    * @brief    This function is responsible for posting model.
    */
    void postrequest();

private:
    /*!
    * @fn       CommandModel();
    * @param    CommandModel&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    TaskInfoModel(const TaskInfoModel& commandmodel);

    /*!
    * @fn       CommandModel();
    * @param    CommandModel&
    * @return   CommandModel&
    * @brief    declaration for private assignment operator
    */
    TaskInfoModel& operator=(const TaskInfoModel& commandmodel);

    QString         m_sTaskID;
};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_TASKINFOMODEL_H_

