/*!
* @file     tipconfigadminaccessinterface.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to tip config
*           which handles communication between server and models.
* @date     Aug, 02 2018
*
* (c) Copyright <2018-2019> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_TIPCONFIGADMINACCESSINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_TIPCONFIGADMINACCESSINTERFACE_H_
#include <rial.h>
#include <QMutex>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   TipConfigAdminAccessInterface
 * \brief   This class is a wrapper of rial library.
 */

class TipConfigAdminAccessInterface : public Rial
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
    * @return   boost::shared_ptr<TipConfigAdminAccessInterface>  pointer to object
    * @brief    This function creates instance of TipConfigAdminAccessInterface and returns pointer to
    *           it.
    */
    static TipConfigAdminAccessInterface* getInstance();

private:
    /*!
    * @fn       TipConfigAdminAccessInterface
    * @param    QObject* parent
    * @return   None
    * @brief    This function is constructor for class TipConfigAdminAccessInterface who is responsible for creating instance of Rial.
    */
    explicit TipConfigAdminAccessInterface(QObject* parent = NULL);

    /*!
    * @fn       ~TipConfigAdminAccessInterface
    * @param    None
    * @return   None
    * @brief    Private destructor.
    */
    ~TipConfigAdminAccessInterface();

    /*!
    * @fn       TipConfigAdminAccessInterface
    * @param    const TipConfigAdminAccessInterface&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    TipConfigAdminAccessInterface(const TipConfigAdminAccessInterface& raal);

    /*!
    * @fn       operator=
    * @param    const TipConfigAdminAccessInterface&  - class object
    * @return   None
    * @brief    Private declaration of asignment operator for class TipConfigAdminAccessInterface.
    */
    void operator = (const TipConfigAdminAccessInterface& param);

    static TipConfigAdminAccessInterface*      m_raal;    //!< static instance of user admin interface

};

}  // end of namespace ws
}  // end of namespace analogic

#endif // TIPCONFIGADMINACCESSINTERFACE_H
