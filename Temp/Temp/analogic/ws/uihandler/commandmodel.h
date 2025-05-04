/*!
* @file     commandmodel.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to CommandModel
*           which is the part of control screen.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_COMMANDMODEL_H_
#define ANALOGIC_WS_UIHANDLER_COMMANDMODEL_H_

#include <analogic/ws/common.h>
#include <analogic/ws/uihandler/modelupdater.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   CommandModel
 * \brief   This class contains variable and function related to
 *          CommandModel.
 */
class CommandModel:public ModelUpdater
{
  Q_OBJECT
public:
    /*!
    * @fn       CommandModel
    * @param    QMLEnums::ModelEnum - model number
    * @param    QString - url
    * @param    QByteArray contentType
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor for class CommandModel.
    */
    CommandModel(QMLEnums::ModelEnum modelnumber, QString strresturl,
                 QByteArray contentType, QObject* parent = NULL);

    /*!
    * @fn       postrequest();
    * @param    QString data
    * @param    QString sUrl
    * @return   None
    * @brief    This function is responsible for posting model.
    */
    void postrequest(QString data, QString sUrl = "");

public slots:
    /*!
    * @fn       onmodelresponse
    * @param    int- Return result
    * @param    QString- strReplyData
    * @return   None
    * @brief    This slot will call when model query response will be received from server.
    */
    void onmodelresponse(int hr, QString strReplyData);
#if defined WORKSTATION || defined RERUN
    /*!
    * @fn       onsslavailable
    * @param    int- Return result
    * @param    QString- strReplyData
    * @return   None
    * @brief    This slot will call when ssl signal is received.
    */
    void onsslavailable(int hr, QString strReplyData);
#endif

private:
    /*!
    * @fn       CommandModel();
    * @param    CommandModel&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    CommandModel(const CommandModel& commandmodel);

    /*!
    * @fn       CommandModel();
    * @param    CommandModel&
    * @return   CommandModel&
    * @brief    declaration for private assignment operator
    */
    CommandModel& operator=(const CommandModel& commandmodel);
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_COMMANDMODEL_H_

