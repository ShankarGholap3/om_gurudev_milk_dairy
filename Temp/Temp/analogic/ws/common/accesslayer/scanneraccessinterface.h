/*!
* @file     scanneraccessinterface.h
* @author   Agiliad
* @brief    This file contains interface to provide live bag data from scanner using streaming xfer library
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERACCESSINTERFACE_H_
#define ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERACCESSINTERFACE_H_

#include <QQueue>
#include <string>
#include <thread>
#include <QObject>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "CharLS/util.h"
#include "CharLS/interface.h"

//#include <boost/shared_ptr.hpp>

#include <SDICOS/SdcsTypes.h>
#include <SDICOS/SdcsTemplateCT.h>
#include <SDICOS/SdcsArray3DLarge.h>

#include <Logger.h>
#include <analogic/ws/common.h>
#include <analogic/streaming_xfer/Version.h>
#include <analogic/streaming_xfer/AlogStreamingBagSender.h>
#include <analogic/streaming_xfer/AlogStreamingXferServer.h>
#include <analogic/streaming_xfer/AlogStreamingBagReceiver.h>
#include <analogic/streaming_xfer/AlogStreamingBagReceiverListener.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
#define MAX_PART_LEN 25000000

/*!
 * \class   ScannerAccessInterface
 * \brief   This class contains variable and function related to
 *          scanner access interface.
 */
class ScannerAccessInterface :public QObject,
        public analogic::streaming_xfer::AlogStreamingBagReceiverListener
{
    Q_OBJECT
public:
    /*!
    * @fn       Start
    * @param    None
    * @return   Errors::RESULT
    * @brief    Start receiving bag
    */
    Errors::RESULT Start(void);

    /*!
    * @fn       CtModuleReceived
    * @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
    * @param    SDICOS::CTModule*
    * @return   None
    * @brief    Call back for CT Module received
    */
    void CtModuleReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                          SDICOS::CTModule* ct_module, const SDICOS::ErrorLog& error_log) override;

    /*!
    * @fn       TdrModuleReceived
    * @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
    * @param    SDICOS::TDRModule*
    * @return   None
    * @brief    Call Back for TDR Module received
    */
    void TdrModuleReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                           SDICOS::TDRModule* tdr_module, const SDICOS::ErrorLog& error_log) override;

    /*!
    * @fn       PropertyReceived
    * @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
    * @param    const std::string& key
    * @param    const std::string& value
    * @return   None
    * @brief    Call Back for Property received
    *
    */
    void PropertyReceived(analogic::streaming_xfer::AlogStreamingBagReceiver
                          *receiver, const std::string& key, const std::string&
                          value);


    /*!
    * @fn       SliceDataReceived
    * @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
    * @param    void* buffer
    * @param    size_t length
    * @return   None
    * @brief    Call Back for Slice data received
    *
    */
    void SliceDataReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                           void* buffer, size_t length) override;

    /*!
    * @fn       ErrorReceived
    * @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
    * @param    std::string& error
    * @return   None
    * @brief    Call Back for Error received
    *
    */
    void ErrorReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                       const std::string& error) override;

    /*!
    * @fn       EndOfTransferReceived
    * @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
    * @return   None
    * @brief    Call Back for End of transmission received
    *
    */
    void EndOfTransferReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver) override;

    /*!
    * @fn       getInstance
    * @param    None
    * @return   ScannerAccessInterface*
    * @brief    gets singleton instance of class
    *
    */
    static ScannerAccessInterface* getInstance();

    /*!
    * @fn       destroyInstance
    * @param    None
    * @return   None
    * @brief    destroys singleton instance of class
    *
    */
    static void destroyInstance();

    /*!
    * @fn       getBagdata
    * @param    None
    * @return   static BagData *
    * @brief    gets bag data
    *
    */
    static boost::shared_ptr<BagData> getBagdata();

    /*!
    * @fn       setBagdata
    * @param    boost::shared_ptr<BagData> - bagdata
    * @return   None
    * @brief    sets bag data
    *
    */
    static void setBagdata(boost::shared_ptr<BagData> bagdata);

    /*!
    * @fn       errorPopup
    * @param    None
    * @return   None
    * @brief    send error popup in case of error.
    *
    */
    void errorPopup(std::string bagid = "-1");

signals:
    /*!
    * @fn       bagReceived
    * @param    boost::shared_ptr<BagData> - bagdata
    * @param    bool status
    * @param    QString message=""
    * @return   None
    * @brief    notifies that bag data is received
    *
    */
    void bagReceived(boost::shared_ptr<BagData> bagdata, bool status,
                     QString message="");

    /*!
    * @fn       notifyToChangeOrNotScreen
    * @param    bool toBeChanged
    * @param    QString errorMsg
    * @param    int hr
    * @return   None
    * @brief    notifies to change screen or not.
    */
    void notifyToChangeOrNotScreen(bool toBeChanged, QString errorMsg, int hr);


public slots:

    /*!
    * @fn       closeReceiver
    * @param    bool dueToDisconnection
    * @return   None
    * @brief    sets bag data
    *
    */
    static void closeReceiver(bool dueToDisconnection=false);

private:
    /*!
    * @fn       validateBagData
    * @param    None
    * @return   QString
    * @brief    validated received bagdata
    */
    QString validateBagData();

    /*!
    * @fn       ScannerAccessInterface
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class ScannerAccessInterface.
    */
    explicit ScannerAccessInterface(QObject* parent = NULL);

    /*!
    * @fn       ~ScannerAccessInterface
    * @param    None
    * @return   None
    * @brief    Destructor for class ScannerAccessInterface. Private because singleton.
    */
    virtual ~ScannerAccessInterface();

    /*!
    * @fn       createReceiver
    * @param    const std::string& host
    * @param    uint16_t port
    * @param    const std::string& bag_identifier
    * @param    AlogStreamingBagReceiverListener* listener
    * @param    uint64_t max_part_length
    * @return   bool
    * @brief    creates receiver
    *
    */
    static bool createReceiver(const std::string& host, uint16_t port,
                               const std::string& bag_identifier, AlogStreamingBagReceiverListener* listener,
                               uint64_t max_part_length);

    /*!
    * @fn       openReceiver
    * @param    None
    * @return   bool
    * @brief    opens receiver
    *
    */
    static bool openReceiver();

    /*!
    * @fn       deleteReceiver
    * @param    None
    * @return   bool
    * @brief    deletes receiver
    *
    */
    static void deleteReceiver();

    static bool                       m_eof;          //!< static flag for indicating End of file transfer
    static bool                       m_bag_loading;  //!< static flag for bag loading state
    static int                        m_ctcount;      //!< static instance of ct count
    static int                        m_tdrcount;     //!< static instance of threat data count
    static ScannerAccessInterface*    m_instance;     //!< static pointer to scanner access interface
    static boost::shared_ptr<BagData> m_bagdata;      //!< static instance of bag data
    static analogic::streaming_xfer::AlogStreamingBagReceiver* m_receiver;

};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ACCESSLAYER_SCANNERACCESSINTERFACE_H_

