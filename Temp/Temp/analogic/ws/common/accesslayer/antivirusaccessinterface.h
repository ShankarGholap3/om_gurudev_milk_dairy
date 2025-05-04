/*!
* @file     antivirusaccessinterface.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           antivirus admin server and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANTIVIRUSACCESSINTERFACE_H
#define ANTIVIRUSACCESSINTERFACE_H

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
 * \class   AnitVirusAccessInterface
 * \brief   This class is a wrapper of rial library.
 */
class AntiVirusAccessInterface : public Rial
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
    * @return   boost::shared_ptr<AntiVirusAccessInterface>  pointer to object
    * @brief    This function creates instance of AntiVirusAccessInterface and returns pointer to
    *           it.
    */
    static AntiVirusAccessInterface* getInstance();

private:
    /*!
    * @fn       AntiVirusAccessInterface
    * @param    QObject* parent
    * @return   None
    * @brief    This function is constructor for class AntiVirusAccessInterface who is responsible for creating instance of Rial.
    */
    explicit AntiVirusAccessInterface(QObject* parent = NULL);

    /*!
    * @fn       ~AntiVirusAccessInterface
    * @param    None
    * @return   None
    * @brief    Private destructor.
    */
    ~AntiVirusAccessInterface();

    /*!
    * @fn       AntiVirusAccessInterface
    * @param    const AntiVirusAccessInterface&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    AntiVirusAccessInterface(const AntiVirusAccessInterface& raal);

    /*!
    * @fn       operator=
    * @param    const AntiVirusAccessInterface&  - class object
    * @return   None
    * @brief    Private declaration of asignment operator for class AntiVirusAccessInterface.
    */
    void operator = (const AntiVirusAccessInterface& param);

    static AntiVirusAccessInterface*      m_raal;    //!< static instance of antivirus interface

};
}  // end of namespace ws
}  // end of namespace analogic

#endif // ANTIVIRUSACCESSINTERFACE_H
