/*!
* @file     useradminaccessinterface.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           user admin server and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/accesslayer/useradminaccessinterface.h>
#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
UserAdminAccessInterface* UserAdminAccessInterface::m_uaal = NULL;

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void UserAdminAccessInterface::destroyInstance()
{
    SAFE_DELETE(m_uaal);
}

/*!
* @fn       getInstance
* @param    None
* @return   boost::shared_ptr<UserAdminAccessInterface> -  pointer to object
* @brief    This function creates instance of UserAdminAccessInterface and returns pointer.
*/
UserAdminAccessInterface* UserAdminAccessInterface::getInstance()
{
    if(!m_uaal)
    {
        DEBUG_LOG("Creating instance of UserAdminAccessInterface");
        m_uaal = new UserAdminAccessInterface();
        if(!m_uaal)
        {
            ERROR_LOG("Failed to create instance of UserAdminAccessInterface");
            throw QException();
        }
    }
    return m_uaal;
}

/*!
* @fn       UserAdminAccessInterface
* @param    QObject
* @return   None
* @brief    This function is constructor for class UserAdminAccessInterface.
*/
UserAdminAccessInterface::UserAdminAccessInterface(QObject *parent):
    Rial(parent)
{
}

/*!
* @fn       ~UserAdminAccessInterface
* @param    None
* @return   None
* @brief    Private destructor.
*/
UserAdminAccessInterface::~UserAdminAccessInterface()
{
}
}  // end of namespace ws
}  // end of namespace analogic
