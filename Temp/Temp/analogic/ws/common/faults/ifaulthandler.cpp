/*!
* @file     ifaulthandler.cpp
* @author   Agiliad
* @brief    This file contains interface, for fault handlers.
* @date     Oct, 07 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include <analogic/ws/common/faults/ifaulthandler.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       IFaultHandler
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class IFaultHandler.
*/
IFaultHandler::IFaultHandler(QObject* parent):QObject(parent)
{
    connect (this, &IFaultHandler::exit, this, &IFaultHandler::onExit,
             Qt::QueuedConnection);
}

/*!
* @fn       ~IFaultHandler
* @param    None
* @return   None
* @brief    Destructor for class IFaultHandler.
*/
IFaultHandler::~IFaultHandler()
{
}
}  // end of namespace ws
}  // end of namespace analogic

