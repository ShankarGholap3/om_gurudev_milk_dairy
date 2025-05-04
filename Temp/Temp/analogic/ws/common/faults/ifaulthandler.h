/*!
* @file     ifaulthandler.h
* @author   Agiliad
* @brief    This file contains interface, for fault handlers.
* @date     Oct, 07 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_FAULTS_IFAULTHANDLER_H_
#define ANALOGIC_WS_COMMON_FAULTS_IFAULTHANDLER_H_

#include <QObject>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   IFaultHandler
 * \brief   This class contains variable and function related to
 *          Fault Handler interface.
 */
class IFaultHandler: public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       IFaultHandler
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class IFaultHandler.
    */
    explicit IFaultHandler(QObject* parent = NULL);

    /*!
    * @fn       ~IFaultHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class IFaultHandler.
    */
    virtual ~IFaultHandler();

public slots:
    /*!
    * @fn      onExit
    * @param   None
    * @return  None
    * @brief   slot call on exit
    */
    virtual void onExit() = 0;

signals:
    /*!
    * @fn      exit
    * @param   None
    * @return  None
    * @brief   signals about exit
    */
    void exit();
};
}  // end of namespace ws
}  // end of namespace analogic


#endif  // ANALOGIC_WS_COMMON_FAULTS_IFAULTHANDLER_H_

