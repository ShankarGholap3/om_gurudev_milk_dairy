/*!
* @file     messagehandler.cpp
* @author   Agiliad
* @brief    This file contains functions related to message handling.
* @date     Aug, 03 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include "messagehandler.h"

namespace analogic
{
namespace ws
{
/*!
* @fn       MessageHandler
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class MessageHandler.
*/
MessageHandler::MessageHandler(): QAbstractMessageHandler(0)
{

}

/*!
* @fn       handleMessage
* @param    QtMsgType type
* @param    const QString &description
* @param    const QUrl &identifier
* @param    const QSourceLocation &sourceLocation
* @return   None
* @brief    handles messages.
*/
void MessageHandler::handleMessage(QtMsgType type, const QString &description,
                           const QUrl &identifier, const QSourceLocation &sourceLocation)
{
    Q_UNUSED(type);
    Q_UNUSED(identifier);

    m_messageType = type;
    m_description = description;
    m_sourceLocation = sourceLocation;
}
}
}
