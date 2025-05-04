/*!
* @file     webengineurlrequestinterceptor.h
* @author   Agiliad
* @brief    Class for implementing a Qt interface class provided for
*           intercepting web requests of WebEngineView so that their
*           attributes can be modified.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_WEBENGINEURLREQUESTINTERCEPTOR_H_
#define ANALOGIC_WS_UIHANDLER_WEBENGINEURLREQUESTINTERCEPTOR_H_

#include <QWebEngineUrlRequestInterceptor>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   WebEngineUrlRequestInterceptor
 * \brief   Update the url request that goes out from QML
 */
class WebEngineUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    /*!
    * @fn       WebEngineUrlRequestInterceptor
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor
    */
    explicit WebEngineUrlRequestInterceptor(QObject* parent = NULL);

    /*!
    * @fn       ~WebEngineUrlRequestInterceptor
    * @param    None
    * @return   None
    * @brief    Destructor for class WebEngineUrlRequestInterceptor
    */
    ~WebEngineUrlRequestInterceptor();

    /*!
    * @fn       interceptRequest
    * @param    QWebEngineUrlRequestInfo& - info
    * @return   None
    * @brief    implementation of virtual method that would update the request.
    */
    void interceptRequest(QWebEngineUrlRequestInfo &info);

    /*!
    * @fn       setAuthToken
    * @param    QString authToken
    * @return   None
    * @brief    Setter function for auth token.
    */
    void setAuthToken(QString authToken);

    /*!
    * @fn       getAuthToken
    * @param    None
    * @return   QString authToken
    * @brief    Getter function for auth token.
    */
    QString getAuthToken();

private:
    QString                 m_authToken;     //!< Authorization token to be passed on with http request.
};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_WEBENGINEURLREQUESTINTERCEPTOR_H_

