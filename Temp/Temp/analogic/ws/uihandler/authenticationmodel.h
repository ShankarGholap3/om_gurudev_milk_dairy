/*!
* @file     authenticationmodel.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to Authenticationmodel
*           which handles authentication of models.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_AUTHENTICATIONMODEL_H_
#define ANALOGIC_WS_UIHANDLER_AUTHENTICATIONMODEL_H_

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
 * \class   Authenticationmodel
 * \brief   This class contains variable and function related to
 *          handling the authentication.
 */

class Authenticationmodel:public ModelUpdater
{
    Q_OBJECT
public:
    /*!
    * @fn       Authenticationmodel
    * @param    QMLEnums::ModelEnum - model number
    * @param    QString - url
    * @param    QByteArray contentType
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor for class Authenticationmodel.
    */
    Authenticationmodel(QMLEnums::ModelEnum modelnumber, QString strresturl,
                        QByteArray contentType, QObject* parent = NULL);

    /*!
    * @fn       postrequest();
    * @param    QString data
    * @param    QString sUrl
    * @return   None
    * @brief    This function is responsible for posting model.
    */
    void postrequest(QString data , QString sUrl = "");

#ifdef WORKSTATION
    /*!
    * @fn       validateandStartServiceTool
    * @param    QString data
    * @return   Errors::RESULT
    * @brief    This function is responsible for validating user credential for service tool.
    */
    Errors::RESULT  validateandStartServiceTool(QString data );
#endif

public slots:

    /*!
    * @fn       onmodelresponse
    * @param    int - Result
    * @param    QString data
    * @return   None
    * @brief    This function is responsible for updating model slot.
    */
    void onmodelresponse(int hr, QString data);
#ifdef WORKSTATION
    /*!
    * @fn       onsslavailable
    * @param    int - Result
    * @param    QString data
    * @return   None
    * @brief    This function is responsible for handling ssl error.
    */
    void onsslavailable(int hr, QString data);
#endif
private:
    /*!
    * @fn       Authenticationmodel();
    * @param    Authenticationmodel&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    Authenticationmodel(const Authenticationmodel& authenticationmodel);

    /*!
    * @fn       operator=
    * @param    Authenticationmodel&
    * @return   Authenticationmodel&
    * @brief    declaration for private assignment operator
    */
    Authenticationmodel& operator=(const Authenticationmodel& authenticationmodel);
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_UIHANDLER_AUTHENTICATIONMODEL_H_

