/*!
* @file     rerunhandler.h
* @author   Agiliad
* @brief    This file contains handler, responsible rerun emulation tasks.
* @date     March, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_RERUN_RERUNHANDLER_H_
#define ANALOGIC_WS_RERUN_RERUNHANDLER_H_

#include <analogic/ws/common.h>
#include <analogic/ws/rerun/rerunbaglist.h>
#include <QFileDialog>
#include <analogic/ws/common/utility/connectemulator.h>
#include <analogic/ws/rerun/reruntaskconfig.h>
#include <analogic/ws/common/utility/usbstorageservice.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   RerunHandler
 * \brief   This class contains handler, responsible rerun emulation tasks.
*/

class RerunHandler : public QObject
{
    Q_OBJECT
public:
    /*!
       * @fn       RerunHandler
       * @param    None
       * @return   None
       * @brief    Constructor for class RerunHandler.
       */
    RerunHandler();

    /*!
    * @fn       RerunHandler();
    * @param    ConnectEmulator pointer
    * @param    RerunTaskConfig pointer
    * @param    RerunBagList pointer
    * @param    UsbStorageSrv pointer
    * @return   None
    * @brief    Parameterised constructor responsible for initialization of class members, memory and resources.
    * @note     Function is Prepared to mock test suites
    */
    RerunHandler(ConnectEmulator* pobjConnectEmulator,
                       RerunTaskConfig* pobjRerunTaskConfig,
                       RerunBagList* pobjRerunBagList,
                       UsbStorageSrv* pobjUsbStorageSrv);

    /*!
       * @fn       RerunHandler
       * @param    None
       * @return   None
       * @brief    Destructor for class RerunHandler.
       */
    ~RerunHandler();
    /*!
    * @fn       init();
    * @param    None
    * @return   int
    * @brief    initialize rerun handler.
    */
    int init();
    /*!
    * @fn       startRerunTask();
    * @param    None
    * @return   void
    * @brief    start rerun task.
    */
    void startRerunTask();

    /*!
    * @fn       cancelRerunTask();
    * @param    None
    * @return   void
    * @brief    cancel rerun task.
    */
    void cancelRerunTask();

    /*!
     * @fn       openBagInputFolder();
     * @param    None
     * @return   None
     * @brief   open bag input folder
     */
    void openBagInputFolder();

    /*!
    * @fn       setTaskConfig();
    * @param    RerunTaskConfig config
    * @return   None
    * @brief    sets run task config
    */
    void setTaskConfig(RerunTaskConfig config);

    /*!
    * @fn       getConnectEmulator();
    * @param    None
    * @return   ConnectEmulator*
    * @brief    get connect emulator
    */
    ConnectEmulator* getConnectEmulator();

    /*!
    * @fn       getUsbStorageService();
    * @param    None
    * @return   UsbStorageSrv*
    * @brief    get usbStorageService;
    */
    UsbStorageSrv* getUsbStorageService();

public slots:
    /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    void onUpdateState(QMLEnums::AppState state);

    /*!
    * @fn       onDeAuthenticate
    * @param    None
    * @return   None
    * @brief    Call for Deauthenticate user
    */
    void onDeAuthenticate();

 private slots:

    /*!
   * @fn       rerunResponseRcv
   * @param    QString status
   * @para     QMLEnums::RERUN_RESPONSES
   * @return   None
   * @brief    recieves about status of rerun task.
   */
    void rerunResponseRcv(QString status , QMLEnums::RERUN_RESPONSES response);

private:
    RerunBagList      *m_rerunBagList;        //!< handle to RerunBagList
    RerunTaskConfig   *m_rerunTaskConfig;     //!< handle to rerun task config
    ConnectEmulator   *m_connectEmulator;     //!< handle to connect emulator
    UsbStorageSrv     *m_usbStorageService;   //!< handle to usb service
};

}  // end of namespace ws
}  // end of namespace analogic
#endif // ANALOGIC_WS_RERUN_RERUNHANDLER_H_
