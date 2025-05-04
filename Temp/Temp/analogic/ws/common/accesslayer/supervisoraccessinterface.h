/*!
* @file     supervisoraccessinterface.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           supervisor and models.
* @date     Nov, 12 2021
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_SUPERVISORACCESSINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_SUPERVISORACCESSINTERFACE_H_

#include <rial.h>



//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   SupervisorAccessInterface
 * \brief   This class is a wrapper of rial library.
 */
class SupervisorAccessInterface : public Rial
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
    * @return   SupervisorAccessInterface*
    * @brief    This function creates instance of SupervisorAccessInterface and returns pointer to
    *           it.
    */
    static SupervisorAccessInterface* getInstance();

private:
    /*!
    * @fn       SupervisorAccessInterface
    * @param    QObject* parent
    * @return   None
    * @brief    This function is constructor for class SupervisorAccessInterface who is responsible for creating instance of Rial.
    */
    explicit SupervisorAccessInterface(QObject* parent = NULL);

    /*!
    * @fn       ~SupervisorAccessInterface
    * @param    None
    * @return   None
    * @brief    private destructor.
    */
    ~SupervisorAccessInterface();

    /*!
    * @fn       SupervisorAccessInterface
    * @param    const SupervisorAccessInterface&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    SupervisorAccessInterface(const SupervisorAccessInterface& saal);

    /*!
    * @fn       operator=
    * @param    const SupervisorAccessInterface&  - class object
    * @return   None
    * @brief    Private declaration of asignment operator for class SupervisorAccessInterface.
    */
    void operator=(const SupervisorAccessInterface& param);

    static SupervisorAccessInterface*   m_pSupervisorAccessInterface;    //!< static instance of supervisor access interface
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_SUPERVISORACCESSINTERFACE_H_

