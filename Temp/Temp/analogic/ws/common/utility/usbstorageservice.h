/*!
* @file     usbstorageservice.h
* @author   Agiliad
* @brief    This file contains serving utilities for usb detection and handling
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef USBSTORAGESERVICE_H
#define USBSTORAGESERVICE_H

#include <QStorageInfo>
#include <libudev.h>
#include <QTimer>
#include <pthread.h>
#include <qfilesystemwatcher.h>
#include <QDebug>
#include <unistd.h>
#include <sys/mount.h>
#include <analogic/ws/rerun/rerunconfig.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{
namespace ws
{
#define MAX_USB_MOUNT_RETRY_COUNT 5
enum
{
    SUCCESS = 0,
    PERMISSION=1,
    SYSTEM_ERROR=2,
    INTERNAL_MOUNT_BUG = 4,
    USER_INTERRUPT=8,
    ALREADY_MOUNTED=16,
    MOUNT_FAILED = 32,
    SOME_MOUNT_SUCCEEDED = 64
};

/*!
 * \class   UsbStorageSrv
 * \brief   This class contains serving utilities for usb detection and handling
 */
class UsbStorageSrv : public QObject
{
    Q_OBJECT
public:
    /*!
    * @fn       UsbStorageSrv
    * @param    None
    * @return   None
    * @brief    Constructor for class UsbStorageSrv.
    */
    explicit UsbStorageSrv(QObject* parent = NULL);

    /*!
    * @fn       ~UsbStorageSrv
    * @param    None
    * @return   None
    * @brief    Destructor for class UsbStorageSrv.
    */
    ~UsbStorageSrv();

    /*!
    * @fn       getUsbfilePath
    * @param    None
    * @return   std::string
    * @brief    get usb filesystem mount path
    */
    static std::string getUsbfilePath();

    /*!
    * @fn       startUsbHotPlugListener
    * @param    None
    * @return   None
    * @brief    Start the directory change listener for usb hot plug actions.
    * It will primariely listen for usb file descriptor folder.
    * Though does not gurrantee of detecting every hotplug action.
    */
    void startUsbHotPlugListener();

    /*!
    * @fn       retriveStorageMountInfo
    * @param    None
    * @return   std::string
    * @brief    Retrive the storage system info and returns mount path.
    */
    std::string retriveStorageMountInfo();

    /*!
    * @fn       unMountUsb
    * @param    None
    * @return   int
    * @brief    Mount the drive
    */
    int unMountUsb();

    /*!
    * @fn       setLoggedIn
    * @param    bool
    * @return   None
    * @brief    set logged in status
    */
    void setLoggedIn(bool isloggedin);

    /*!
    * @fn       getErrorMsg
    * @param    int - error code
    * @return   QString - Error msg according to error code
    * @brief    get error msg from error code
    */
    QString getErrorMsg(int ErrorCode);

    /*!
    * @fn       sizeofdrive
    * @param    No
    * @return   qint64
    * @brief    Returns free bytes on disk
    */

    qint64 sizeofdrive(QString path);


public slots:

   /*!
    * @fn       showModified
    * @param    const QString& str
    * @return   None
    * @brief    slot executed on target dir change signal
    */
    void showModified(const QString& str);

signals:
    /*!
   * @fn       usbMountevent
   * @param    QString status
   * @return   None
   * @brief    notifies about usb mount.
   */
    void usbMountevent(QString status , QMLEnums::RERUN_RESPONSES);

private:
    /*!
    * @fn       scanUsbStorageDevices
    * @param    None
    * @return   char*
    * @brief    scan usb storage devices using sysfs enumerates.
    */
    char*  scanUsbStorageDevices();

    /*!
    * @fn       getSubDeviceByType
    * @param    struct udev* udev
    * @param    struct udev_device* parent
    * @param    const char* subsystem
    * @return   struct udev_device*
    * @brief    get the child subdevice by parent bus and device type like char or block.
    */
    struct udev_device* getSubDeviceByType(struct udev* udev, struct udev_device* parent, const char* subsystem);

    /*!
    * @fn       mountDrive
    * @param    std::string source
    * @param    std::string target
    * @return   None
    * @brief    Mount the drive
    */
    void mountDrive(std::string source, std::string target);

    static std::string      m_usbFilePath;         //!< usb file path mounted
    static std::string      m_usbNodePath;         //!< usb node path
    static bool             m_isMounted;           //!< usb mount flag
    static QString          m_usbNode;             //!< base usb node
    QFileSystemWatcher      *m_fsWatcher;          //!< file system watcher for hotplug
    static bool             m_isloggedIn;          //!< is user logged in   
};
}  // end of namespace ws
}  // end of namespace analogi
#endif // USBSTORAGESERVICE_H
