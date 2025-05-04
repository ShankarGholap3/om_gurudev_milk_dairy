/*!
* @file     archiveaccessinterface.h
* @author   Agiliad
* @brief    This file contains interface to provide archive data from archive service hosted on Admin Server.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_ARCHIVEACCESSINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_ARCHIVEACCESSINTERFACE_H_

#include <rial.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   ArchiveAccessInterface
 * \brief   This class contains variable and function related to
 *           Archive Access
 */
class ArchiveAccessInterface : public Rial
{
public:
    /*!
    * @fn       destroyInstance
    * @param    None
    * @return   None
    * @brief    This function is responsible for release of any resources if req.
    */
    static void destroyInstance();

    /*!
    * @fn       getInstance
    * @param    None
    * @return   ArchiveAccessInterface*
    * @brief    This function creates instance of ArchiveAccessInterface and returns pointer to
    *           it.
    */
    static ArchiveAccessInterface* getInstance();

private:
    /*!
    * @fn       ArchiveAccessInterface
    * @param    None
    * @return   None
    * @brief    Constructor for class ArchiveAccessInterface.
    */
    ArchiveAccessInterface(QObject *parent=0);

    /*!
    * @fn       ~ArchiveAccessInterface
    * @param    None
    * @return   None
    * @brief    Destructor for class ArchiveAccessInterface.
    */
    ~ArchiveAccessInterface();

    /*!
    * @fn       ArchiveAccessInterface
    * @param    const ArchiveAccessInterface&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ArchiveAccessInterface(const ArchiveAccessInterface& saal);

    /*!
    * @fn       operator=
    * @param    const ArchiveAccessInterface&  - class object
    * @return   None
    * @brief    Private declaration of asignment operator for class ArchiveAccessInterface.
    */
    void operator=(const ArchiveAccessInterface& param);

    static ArchiveAccessInterface*   m_aal;    //!< static instance of scanner admin interface

};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_ARCHIVEACCESSINTERFACE_H_

