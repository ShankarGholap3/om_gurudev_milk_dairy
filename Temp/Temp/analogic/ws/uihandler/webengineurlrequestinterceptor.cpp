/*!
* @file     webengineurlrequestinterceptor.cpp
* @author   Agiliad
* @brief    Class for implementing a Qt interface class provided for
*           intercepting web requests of WebEngineView so that their
*           attributes can be modified.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/uihandler/webengineurlrequestinterceptor.h>
#include <analogic/ws/common.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
* @fn       WebEngineUrlRequestInterceptor
* @param    QObject* - parent
* @return   None
* @brief    Constructor
*/
WebEngineUrlRequestInterceptor::WebEngineUrlRequestInterceptor(QObject* parent) :
    QWebEngineUrlRequestInterceptor(parent),
    m_authToken("")
{
    TRACE_LOG("");
}

/*!
* @fn       ~WebEngineUrlRequestInterceptor
* @param    None
* @return   None
* @brief    Destructor for class WebEngineUrlRequestInterceptor
*/
WebEngineUrlRequestInterceptor::~WebEngineUrlRequestInterceptor()
{
    TRACE_LOG("");
}

/*!
* @fn       interceptRequest
* @param    QWebEngineUrlRequestInfo& - info
* @return   None
* @brief    implementation of virtual method that would update the request.
*/
void WebEngineUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    TRACE_LOG("");
    info.setHttpHeader(QByteArray("Authorization"), m_authToken.toUtf8());
}

/*!
* @fn       setAuthToken
* @param    QString authToken
* @return   None
* @brief    Setter function for auth token.
*/
void WebEngineUrlRequestInterceptor::setAuthToken(QString authToken)
{
    TRACE_LOG("");
    m_authToken = authToken;
}

/*!
* @fn       getAuthToken
* @param    None
* @return   QString authToken
* @brief    Getter function for auth token.
*/
QString WebEngineUrlRequestInterceptor::getAuthToken()
{
    TRACE_LOG("");
    return m_authToken;
}
}  // namespace ws
}  // namespace analogic
