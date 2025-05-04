/*!
* @file     scanneragentstatushandler.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to scanner agent
*           status handler.
* @date     Sep, 30 2016
*
* (c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERAGENTSTATUSHANDLER_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERAGENTSTATUSHANDLER_H_



#include <analogic/nss/agent/scanner/ScannerAgentStatus.h>
#include <QObject>
//  namespace analogic
//  {
//  namespace ws {

/*!
 * \class   ScannerAgentStatusHandler
 * \brief   This class handles scanner agent status
 */
class ScannerAgentStatusHandler:public QObject
{
    Q_OBJECT

public:
    /*!
    * @fn       ScannerAgentStatusHandler
    * @param    QObject* parent
    * @return   None
    * @brief    constructor for ScannerAgentStatusHandler
    */
    explicit ScannerAgentStatusHandler(QObject* parent = NULL);
    /*!
    * @fn       ScannerAgentStatusHandler
    * @param    const analogic::nss::ScannerAgentStatus&
    * @return   None
    * @brief    constructor for ScannerAgentStatusHandler
    */
    explicit ScannerAgentStatusHandler(const analogic::nss::ScannerAgentStatus& st);
    /*!
    * @fn       ScannerAgentStatusHandler
    * @param    const ScannerAgentStatusHandler& st
    * @return   None
    * @brief    constructor for ScannerAgentStatusHandler
    */
    ScannerAgentStatusHandler(const ScannerAgentStatusHandler& st);
    /*!
    * @fn       operator =
    * @param    const analogic::nss::ScannerAgentStatus&
    * @return   None
    * @brief    assignment operator for ScannerAgentStatusHandler
    */
    const ScannerAgentStatusHandler& operator = (const analogic::nss::ScannerAgentStatus& st);

    /*!
    * @fn       ~ScannerAgentStatusHandler
    * @param    None
    * @return   None
    * @brief    Destructor for ScannerAgentStatusHandler
    */
    virtual ~ScannerAgentStatusHandler();
    /*!
    * @fn       setscannerstatus
    * @param    analogic::nss::ScannerAgentStatus
    * @return   None
    * @brief    sets scanner status
    */
    void setscannerstatus(analogic::nss::ScannerAgentStatus);

    /*!
    * @fn       getscannerstatus
    * @param    None
    * @return   analogic::nss::ScannerAgentStatus
    * @brief    gets scanner status
    */
    analogic::nss::ScannerAgentStatus getscannerstatus();

private:
    analogic::nss::ScannerAgentStatus    m_status;    //!< instance of scanner agent status
};



//}
//}
Q_DECLARE_METATYPE(ScannerAgentStatusHandler)
#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERAGENTSTATUSHANDLER_H_
