/*!
* @file     scanneradminaccessinterface.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           scanner admin server and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERADMINACCESSINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERADMINACCESSINTERFACE_H_

#include <rial.h>



//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   ScannerAdminAccessInterface
 * \brief   This class is a wrapper of rial library.
 */
class ScannerAdminAccessInterface : public Rial
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
    * @return   ScannerAdminAccessInterface*
    * @brief    This function creates instance of ScannerAdminAccessInterface and returns pointer to
    *           it.
    */
    static ScannerAdminAccessInterface* getInstance();

private:
    /*!
    * @fn       ScannerAdminAccessInterface
    * @param    QObject* parent
    * @return   None
    * @brief    This function is constructor for class ScannerAdminAccessInterface who is responsible for creating instance of Rial.
    */
    explicit ScannerAdminAccessInterface(QObject* parent = NULL);

    /*!
    * @fn       ~ScannerAdminAccessInterface
    * @param    None
    * @return   None
    * @brief    private destructor.
    */
    ~ScannerAdminAccessInterface();

    /*!
    * @fn       ScannerAdminAccessInterface
    * @param    const ScannerAdminAccessInterface&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ScannerAdminAccessInterface(const ScannerAdminAccessInterface& saal);

    /*!
    * @fn       operator=
    * @param    const ScannerAdminAccessInterface&  - class object
    * @return   None
    * @brief    Private declaration of asignment operator for class ScannerAdminAccessInterface.
    */
    void operator=(const ScannerAdminAccessInterface& param);

    static ScannerAdminAccessInterface*   m_saal;    //!< static instance of scanner admin interface
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERADMINACCESSINTERFACE_H_

