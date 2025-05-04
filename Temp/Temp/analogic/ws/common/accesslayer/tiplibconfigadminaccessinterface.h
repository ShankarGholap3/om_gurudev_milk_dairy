/*!
* @file     tiplibconfigadminaccessinterface.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to tip
*           lib config admin server and models.
* @date     Aug, 02 2018
*
* (c) Copyright <2018-2019> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_TIPLIBCONFIGADMINACCESSINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_TIPLIBCONFIGADMINACCESSINTERFACE_H_

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
 * \class   TipLibConfigAdminAccessInterface
 * \brief   This class is a wrapper of rial library.
 */

class TipLibConfigAdminAccessInterface : public Rial
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
    * @return   boost::shared_ptr<TipLibConfigAdminAccessInterface>  pointer to object
    * @brief    This function creates instance of TipLibConfigAdminAccessInterface and returns pointer to
    *           it.
    */
    static TipLibConfigAdminAccessInterface* getInstance();

private:
    /*!
    * @fn       TipLibConfigAdminAccessInterface
    * @param    QObject* parent
    * @return   None
    * @brief    This function is constructor for class TipLibConfigAdminAccessInterface who is responsible for creating instance of Rial.
    */
    explicit TipLibConfigAdminAccessInterface(QObject* parent = NULL);

    /*!
    * @fn       ~TipLibConfigAdminAccessInterface
    * @param    None
    * @return   None
    * @brief    Private destructor.
    */
    ~TipLibConfigAdminAccessInterface();

    /*!
    * @fn       TipLibConfigAdminAccessInterface
    * @param    const TipLibConfigAdminAccessInterface&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    TipLibConfigAdminAccessInterface(const TipLibConfigAdminAccessInterface& raal);

    /*!
    * @fn       operator=
    * @param    const TipLibConfigAdminAccessInterface&  - class object
    * @return   None
    * @brief    Private declaration of asignment operator for class TipLibConfigAdminAccessInterface.
    */
    void operator = (const TipLibConfigAdminAccessInterface& param);

    static TipLibConfigAdminAccessInterface*      m_raal;    //!< static instance of user admin interface

};

}  // end of namespace ws
}  // end of namespace analogic

#endif // TIPLIBCONFIGADMINACCESSINTERFACE_H
