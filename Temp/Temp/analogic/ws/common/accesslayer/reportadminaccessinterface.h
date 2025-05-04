/*!
* @file     reportadminaccessinterface.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to
*           report admin server and models.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef REPORTADMINACCESSINTERFACE_H
#define REPORTADMINACCESSINTERFACE_H

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
 * \class   ReportAdminAccessInterface
 * \brief   This class is a wrapper of rial library.
 */
class ReportAdminAccessInterface : public Rial
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
    * @return   boost::shared_ptr<ReportAdminAccessInterface>  pointer to object
    * @brief    This function creates instance of ReportAdminAccessInterface and returns pointer to
    *           it.
    */
    static ReportAdminAccessInterface* getInstance();

private:
    /*!
    * @fn       ReportAdminAccessInterface
    * @param    QObject* parent
    * @return   None
    * @brief    This function is constructor for class ReportAdminAccessInterface who is responsible for creating instance of Rial.
    */
    explicit ReportAdminAccessInterface(QObject* parent = NULL);

    /*!
    * @fn       ~ReportAdminAccessInterface
    * @param    None
    * @return   None
    * @brief    Private destructor.
    */
    ~ReportAdminAccessInterface();

    /*!
    * @fn       ReportAdminAccessInterface
    * @param    const ReportAdminAccessInterface&
    * @return   None
    * @brief    declaration for private copy constructor.
    */
    ReportAdminAccessInterface(const ReportAdminAccessInterface& raal);

    /*!
    * @fn       operator=
    * @param    const ReportAdminAccessInterface&  - class object
    * @return   None
    * @brief    Private declaration of asignment operator for class ReportAdminAccessInterface.
    */
    void operator = (const ReportAdminAccessInterface& param);

    static ReportAdminAccessInterface*      m_raal;    //!< static instance of user admin interface

};
}  // end of namespace ws
}  // end of namespace analogic

#endif // REPORTADMINACCESSINTERFACE_H
