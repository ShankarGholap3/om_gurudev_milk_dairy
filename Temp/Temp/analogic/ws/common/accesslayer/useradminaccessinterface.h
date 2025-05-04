/*!
* @file     useradminaccessinterface.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           user admin server and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_USERADMINACCESSINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_USERADMINACCESSINTERFACE_H_

#include <rial.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   UserAdminAccessInterface
 * \brief   This class is a wrapper of rial library.
 */
class UserAdminAccessInterface : public Rial
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
    * @return   boost::shared_ptr<UserAdminAccessInterface>  pointer to object
    * @brief    This function creates instance of UserAdminAccessInterface and returns pointer to
    *           it.
    */
    static UserAdminAccessInterface* getInstance();

private:
    /*!
    * @fn       UserAdminAccessInterface
    * @param    QObject* parent
    * @return   None
    * @brief    This function is constructor for class UserAdminAccessInterface who is responsible for creating instance of Rial.
    */
    explicit UserAdminAccessInterface(QObject* parent = NULL);

    /*!
    * @fn       ~UserAdminAccessInterface
    * @param    None
    * @return   None
    * @brief    Private destructor.
    */
    ~UserAdminAccessInterface();

    /*!
    * @fn       UserAdminAccessInterface
    * @param    const UserAdminAccessInterface&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    UserAdminAccessInterface(const UserAdminAccessInterface& uaal);

    /*!
    * @fn       operator=
    * @param    const UserAdminAccessInterface&  - class object
    * @return   None
    * @brief    Private declaration of asignment operator for class UserAdminAccessInterface.
    */
    void operator = (const UserAdminAccessInterface& param);

    static UserAdminAccessInterface*      m_uaal;    //!< static instance of user admin interface
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_USERADMINACCESSINTERFACE_H_
