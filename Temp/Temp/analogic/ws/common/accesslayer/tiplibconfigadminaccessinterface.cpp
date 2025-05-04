/*!
* @file     tiplibconfigadminaccessinterface.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to tip
*           lib config admin server and models.
* @date     Aug, 02 2018
*
* (c) Copyright <2018-2019> Analogic Corporation. All Rights Reserved
*/
#include "tiplibconfigadminaccessinterface.h"
#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
TipLibConfigAdminAccessInterface* TipLibConfigAdminAccessInterface::m_raal = NULL;

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void TipLibConfigAdminAccessInterface::destroyInstance()
{
    SAFE_DELETE(m_raal);
}

/*!
* @fn       getInstance
* @param    None
* @return   boost::shared_ptr<TipLibConfigAdminAccessInterface> -  pointer to object
* @brief    This function creates instance of TipLibConfigAdminAccessInterface and returns pointer.
*/
TipLibConfigAdminAccessInterface* TipLibConfigAdminAccessInterface::getInstance()
{
    QMutex mutex;
    mutex.lock();
    if(!m_raal)
    {
        DEBUG_LOG("Creating instance of TipLibConfigAdminAccessInterface");
        m_raal = new TipLibConfigAdminAccessInterface();
        if(!m_raal)
        {
            ERROR_LOG("Failed to create instance of TipLibConfigAdminAccessInterface");
            throw QException();
        }
    }
    mutex.unlock();
    return m_raal;
}

/*!
* @fn       TipLibConfigAdminAccessInterface
* @param    QObject
* @return   None
* @brief    This function is constructor for class TipLibConfigAdminAccessInterface.
*/
TipLibConfigAdminAccessInterface::TipLibConfigAdminAccessInterface(QObject *parent):
    Rial(parent)
{
}

/*!
* @fn       ~TipLibConfigAdminAccessInterface
* @param    None
* @return   None
* @brief    Private destructor.
*/
TipLibConfigAdminAccessInterface::~TipLibConfigAdminAccessInterface()
{
}
}  // end of namespace ws
}  // end of namespace analogic
