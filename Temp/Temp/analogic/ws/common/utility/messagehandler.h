/*!
* @file     messagehandler.h
* @author   Agiliad
* @brief    This file contains functions related to message handling.
* @date     Aug, 03 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QAbstractMessageHandler>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   MessageHandler
 * \brief   This class contains variable and function related to
 *          handling MessageHandler of application.
 */
class MessageHandler : public QAbstractMessageHandler
{
public:
    /*!
    * @fn       MessageHandler
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class MessageHandler.
    */
    MessageHandler();

    /*!
    * @fn       statusMessage
    * @param    None
    * @return   QString -  descriptin
    * @brief    gets status message
    */
    QString statusMessage() const
    {
        return m_description;
    }

    /*!
    * @fn       line
    * @param    None
    * @return   int - line number
    * @brief    gets source code line number
    */
    int line() const
    {
        return m_sourceLocation.line();
    }

    /*!
    * @fn       column
    * @param    None
    * @return   int - clumn number
    * @brief    gets source code column number
    */
    int column() const
    {
        return m_sourceLocation.column();
    }

protected:
    /*!
    * @fn       handleMessage
    * @param    QtMsgType type
    * @param    const QString &description
    * @param    const QUrl &identifier
    * @param    const QSourceLocation &sourceLocation
    * @return   None
    * @brief    handles messages.
    */
    virtual void handleMessage(QtMsgType type, const QString &description,
                               const QUrl &identifier, const QSourceLocation &sourceLocation);

private:
    QtMsgType m_messageType;
    QString m_description;
    QSourceLocation m_sourceLocation;
};
}
}

#endif // MESSAGEHANDLER_H
