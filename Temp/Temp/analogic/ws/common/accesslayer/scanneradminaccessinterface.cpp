/*!
* @file     scanneradminaccessinterface.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           scanner admin server and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/accesslayer/scanneradminaccessinterface.h>
#include <analogic/ws/common.h>

namespace analogic
{
namespace ws
{
ScannerAdminAccessInterface* ScannerAdminAccessInterface::m_saal = NULL;

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void ScannerAdminAccessInterface::destroyInstance()
{
    TRACE_LOG("");
    SAFE_DELETE(m_saal);
}

/*!
* @fn       getInstance
* @param    None
* @return   ScannerAdminAccessInterface* -  pointer to object
* @brief    This function creates instance of ScannerAdminAccessInterface and returns pointer to
*           it and also creates instance of Rial.
*/
ScannerAdminAccessInterface* ScannerAdminAccessInterface::getInstance()
{
    if(!m_saal)
    {
        INFO_LOG("Creating ScannerAdminAccessInterface instance");
        m_saal = new ScannerAdminAccessInterface();
        if(!m_saal)
        {
            ERROR_LOG("Error in allocating memory for ScannerAdminAccessInterface instance");
            throw QException();
        }
         DEBUG_LOG("Created ScannerAdminAccessInterface instance");
    }
    DEBUG_LOG("Return ScannerAdminAccessInterface instance");
    return m_saal;
}
/*!
* @fn       ScannerAdminAccessInterface
* @param    QObject
* @return   None
* @brief    This function is constructor for class ScannerAdminAccessInterface who is responsible for creating instance of Rial.
*/
ScannerAdminAccessInterface::ScannerAdminAccessInterface(QObject *parent):
    Rial(parent)
{
}

/*!
* @fn       ~ScannerAdminAccessInterface
* @param    None
* @return   None
* @brief    private destructor.
*/
ScannerAdminAccessInterface::~ScannerAdminAccessInterface()
{
}
}  // end of namespace ws
}  // end of namespace analogic
