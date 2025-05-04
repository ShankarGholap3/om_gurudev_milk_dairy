/*!
* @file     tipconfigadminaccessinterface.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to tip
*           config admin server and models.
* @date     Aug, 02 2018
*
* (c) Copyright <2018-2019> Analogic Corporation. All Rights Reserved
*/
#include "tipconfigadminaccessinterface.h"
#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
TipConfigAdminAccessInterface* TipConfigAdminAccessInterface::m_raal = NULL;

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void TipConfigAdminAccessInterface::destroyInstance()
{
    SAFE_DELETE(m_raal);
}

/*!
* @fn       getInstance
* @param    None
* @return   boost::shared_ptr<TipConfigAdminAccessInterface> -  pointer to object
* @brief    This function creates instance of TipConfigAdminAccessInterface and returns pointer.
*/
TipConfigAdminAccessInterface* TipConfigAdminAccessInterface::getInstance()
{
    QMutex mutex;
    mutex.lock();
    if(!m_raal)
    {
        DEBUG_LOG("Creating instance of TipConfigAdminAccessInterface");
        m_raal = new TipConfigAdminAccessInterface();
        if(!m_raal)
        {
            ERROR_LOG("Failed to create instance of TipConfigAdminAccessInterface");
            throw QException();
        }
    }
    mutex.unlock();
    return m_raal;
}

/*!
* @fn       TipConfigAdminAccessInterface
* @param    QObject
* @return   None
* @brief    This function is constructor for class TipConfigAdminAccessInterface.
*/
TipConfigAdminAccessInterface::TipConfigAdminAccessInterface(QObject *parent):
    Rial(parent)
{
}

/*!
* @fn       ~TipConfigAdminAccessInterface
* @param    None
* @return   None
* @brief    Private destructor.
*/
TipConfigAdminAccessInterface::~TipConfigAdminAccessInterface()
{
}
}  // end of namespace ws
}  // end of namespace analogic
