/*!
* @file     usbstorageservice.cpp
* @author   Agiliad
* @brief    This file contains serving utilities for usb detection and handling
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/utility/usbstorageservice.h>
#include <QProcess>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{
namespace ws
{
std::string UsbStorageSrv::m_usbFilePath;
std::string UsbStorageSrv::m_usbNodePath;
bool UsbStorageSrv::m_isMounted = false;
QString UsbStorageSrv::m_usbNode;
bool UsbStorageSrv::m_isloggedIn = false;
/*!
    * @fn       UsbStorageSrv
    * @param    None
    * @return   None
    * @brief    Constructor for class UsbStorageSrv.
    */
UsbStorageSrv::UsbStorageSrv(QObject *parent):QObject(parent)
{
    m_fsWatcher = new QFileSystemWatcher();
    if (m_fsWatcher == NULL)
    {
        ERROR_LOG("Unable to allocate memory for file system watcher.");
        return;
    }
    QObject::connect(m_fsWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(showModified(QString)));
    startUsbHotPlugListener();
}

/*!
    * @fn       ~UsbStorageSrv
    * @param    None
    * @return   None
    * @brief    Destructor for class UsbStorageSrv.
    */
UsbStorageSrv::~UsbStorageSrv()
{
    delete m_fsWatcher;
}

/*!
* @fn       startUsbHotPlugListener
* @param    None
* @return   None
* @brief    Start the directory change listener for usb hot plug actions.
* It will primariely listen for usb file descriptor folder.
* Though does not gurrantee of detecting every hotplug action.
*/
void UsbStorageSrv::startUsbHotPlugListener()
{
    if (!m_fsWatcher->directories().contains(QString::fromStdString("/dev")))
    {
        m_fsWatcher->addPath("/dev");
    }
    QStringList directoryList = m_fsWatcher->directories();
    DEBUG_LOG("Directories getting watch for auto plug:");
    for(int i =0; i< directoryList.size(); i++)
    {
        DEBUG_LOG(directoryList.at(i).toStdString());
    }


}
/*!
* @fn       retriveStorageMountInfo
* @param    None
* @return   std::string
* @brief    Retrive the storage system info and returns mount path.
*/
std::string UsbStorageSrv::retriveStorageMountInfo()
{
    std::string filePath;
    if(m_isloggedIn)
    {
#ifdef RERUN
        if(RerunConfig::isRemovableStorage() )
#endif
        {
            scanUsbStorageDevices();
        }
        if (m_usbNode.isEmpty())
        {
            DEBUG_LOG("value of m_mounted retriveStorageMountInfo"<< m_isMounted);
            //            m_isMounted = false;
            return filePath;
        }
    }
    return filePath;
}
/*!
* @fn       setLoggedIn
* @param    bool
* @return   None
* @brief    set logged in status
*/
void UsbStorageSrv::setLoggedIn(bool isloggedin)
{
    m_isloggedIn = isloggedin;
}

/*!
 * @fn       showModified
 * @param    const QString& str
 * @return   None
 * @brief    slot executed on target dir change signal
 */
void UsbStorageSrv::showModified(const QString& str)
{
    Q_UNUSED(str)
    retriveStorageMountInfo();
    WorkstationConfig::getInstance()->setDataPath(getUsbfilePath().c_str());
}

/*!
* @fn       scanUsbStorageDevices
* @param    struct udev* udev
* @return   char*
* @brief    scan usb storage devices using sysfs enumerates.
*/
char*  UsbStorageSrv::scanUsbStorageDevices()
{
    DEBUG_LOG("start of UsbStorageSrv::scanUsbStorageDevices()");
    int count = 0;
    char *devNode = NULL;
    QString msg;
    bool flag = false;
    do
    {

        struct udev* deviceManager = udev_new();
        struct udev_enumerate* enumerate = udev_enumerate_new(deviceManager);
        udev_enumerate_add_match_subsystem(enumerate, "scsi");
        //  udev_enumerate_add_match_property(enumerate, "DEVTYPE", "scsi_device");
        udev_enumerate_scan_devices(enumerate);

        struct udev_list_entry *scsiDevices = udev_enumerate_get_list_entry(enumerate);
        struct udev_list_entry *entry;

        udev_list_entry_foreach(entry, scsiDevices)
        {
            const char* nodePath = udev_list_entry_get_name(entry);
            // Opaque object representing one kernel sys device.
            struct udev_device* scsiKernelObj = udev_device_new_from_syspath(deviceManager, nodePath);

            struct udev_device* blockFs = getSubDeviceByType(deviceManager, scsiKernelObj, "block");
            struct udev_device* scsiDiskFs = getSubDeviceByType(deviceManager, scsiKernelObj, "scsi_disk");

            struct udev_device* usbFs = udev_device_get_parent_with_subsystem_devtype(scsiKernelObj, "usb", "usb_device");

            if (blockFs && scsiDiskFs && usbFs) {

                //            struct udev_list_entry *temp =  udev_device_get_sysattr_list_entry
                //                    (usbFs);

                devNode = (char*)udev_device_get_devnode(blockFs);

                if (strlen(devNode) >0)
                {
                    flag = true;
                    DEBUG_LOG("value of devNode "<< devNode);

                    m_usbFilePath = WorkstationConfig::getInstance()->getUsbMountPath().toStdString();
                    if(m_usbNode.contains(devNode))
                    {
                        m_usbFilePath = WorkstationConfig::getInstance()->getUsbMountPath().toStdString();
                    }
                    else
                    {
                        QFile file;
                        std::string devpath(devNode);

                        QString pathDev = QString::fromStdString(devpath + "1");
                        if (file.exists(pathDev))
                        {
                            devpath = devpath + "1";
                        }
                        mountDrive(devpath.c_str() , WorkstationConfig::getInstance()->getUsbMountPath().toStdString());
                    }
                    break;
                }


            }

            if (blockFs)
                udev_device_unref(blockFs);

            if (scsiDiskFs)
                udev_device_unref(scsiDiskFs);

            udev_device_unref(scsiKernelObj);
        }
        if(!flag && m_isMounted)
        {
            m_usbNode = "";
            QString stat = tr("USB got unplugged");
            INFO_LOG("Mount : USB got unplugged");
#ifdef RERUN
            emit usbMountevent(stat , QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
#endif

            m_isMounted = false;
            break;
        }
        udev_enumerate_unref(enumerate);
        udev_unref(deviceManager);
        count++;
        if(!m_isMounted)
        {
          INFO_LOG("USB is not mounted, retry count " << count);
          sleep(1); //Time for the dev entry to be added before retrying
        }
    } while((count < MAX_USB_MOUNT_RETRY_COUNT) && (!m_isMounted));
    return devNode;
}

/*!
* @fn       getSubDeviceByType
* @param    struct udev* udev
* @param    struct udev_device* parent
* @param    const char* subsystem
* @return   struct udev_device*
* @brief    get the child subdevice by parent bus and device type like char or block.
*/
struct udev_device* UsbStorageSrv::getSubDeviceByType(
        struct udev* udev, struct udev_device* parent, const char* subsystem)
{
    struct udev_device* child = NULL;
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_parent(enumerate, parent);
    udev_enumerate_add_match_subsystem(enumerate, subsystem);
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices) {
        const char *path = udev_list_entry_get_name(entry);
        child = udev_device_new_from_syspath(udev, path);
        break;
    }

    udev_enumerate_unref(enumerate);
    return child;
}

/*!
* @fn       getUsbfilePath
* @param    None
* @return   std::string
* @brief    get usb filesystem mount path
*/
std::string UsbStorageSrv::getUsbfilePath()
{
    return m_usbFilePath;
}

/*!
* @fn       mountDrive
* @param    std::string source
* @param    std::string target
* @return   None
* @brief    Mount the drive
*/
void UsbStorageSrv::mountDrive(std::string source , std::string target)
{
    QString sUrl = "sudo mount " + QString(source.c_str()) + " " + QString(target.c_str()) + " -o uid=analogic,gid=analogic";
    QString msg;

    bool mountres = false;

    int ret = QProcess::execute(sUrl);

    if((ret == SUCCESS) || (ret == ALREADY_MOUNTED) || (ret == MOUNT_FAILED))
    {
        mountres = true;
        if((ret == MOUNT_FAILED))
        {
          QFile myfile("/proc/mounts");
          myfile.open(QIODevice::ReadOnly);
          QTextStream in(&myfile);
          const QString content = in.readAll();
          if((content.contains(m_usbFilePath.c_str())) && (content.contains(source.c_str())))
          {
            msg = tr("Mount: USB is already mounted");
          }
          else
          msg = tr("Mount: USB mounting failed:Reason: Device is busy");
    }
    else
        msg = tr("Mount: ")+ getErrorMsg(ret);
#ifdef RERUN
        emit usbMountevent(msg, QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
#endif

    }
    else if ((ret == PERMISSION) ||(ret == SYSTEM_ERROR)||
             (ret == INTERNAL_MOUNT_BUG) ||(ret == USER_INTERRUPT)
             )
    {

        msg = tr("Mount:USB Mounting failed. Reason: ") + getErrorMsg(ret);
        mountres = false;
#ifdef RERUN
        emit usbMountevent(msg, QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
#endif

    }
    else
    {
        msg = tr("Mount:USB Mounting failed:Reason: Unknown");
        mountres = false;
    }

    DEBUG_LOG(msg.toStdString().c_str());

    if(mountres)
    {
        m_usbFilePath = target;
        m_isMounted = true;
        m_usbNode = source.c_str();
    }
    else
    {
        m_usbFilePath = "";
        m_isMounted = false;
        m_usbNode = "";
    }

}


/*!
* @fn       unMountUsb
* @param    None
* @return   None
* @brief    Mount the drive
*/
int UsbStorageSrv::unMountUsb()
{
    int ret = -1;
    QString msg;
    INFO_LOG("Going for unmount" << m_isMounted);
    if (m_isMounted == true)
    {
        for(int i=0;i<MAX_USB_MOUNT_RETRY_COUNT;i++)
        {
            QString sUrl = "sudo umount " + QString(m_usbFilePath.c_str());

            ret = QProcess::execute(sUrl);

            if ((ret == SUCCESS) ||
                    (ret == ALREADY_MOUNTED))
            {
                msg = tr("Unmount: USB unmounted from path ") + m_usbFilePath.c_str();
                m_usbFilePath = "";
                m_isMounted = false;
                m_usbNode = "";
                emit usbMountevent(msg , QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
                break;
            }
            else
            {

                msg = tr("Unmount:USB unmount failed.Reason:") + getErrorMsg(ret);
                emit usbMountevent(msg , QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
            }
        }

    }
    else
    {
        INFO_LOG("USB is not mounted.");
        ret = 0;
    }
    m_usbNode = "";
    return ret;
}

/*!
* @fn       getErrorMsg
* @param    int - error code
* @return   QString - Error msg according to error code
* @brief    get error msg from error code
*/
QString UsbStorageSrv::getErrorMsg(int ErrorCode)
{
    QString msg;
    switch (ErrorCode) {
    case SUCCESS:
        msg = tr(" Success.");
        break;
    case ALREADY_MOUNTED:
        msg = tr(" USB is already mounted");
        break;
    case PERMISSION:
        msg = tr("Incorrect invocation or permissions");
        break;
    case SYSTEM_ERROR:
        msg = tr("System error (out of memory, cannot fork, no more loop devices");
        break;
    case USER_INTERRUPT:
        msg = tr("User interrupted");
        break;
    case MOUNT_FAILED:
        msg = tr("Device is busy");
        break;
    default:
        msg = tr("Unknown failure");
        break;
    }
    return msg;
}

/*!
* @fn       sizeofdrive
* @param    No
* @return   qint64
* @brief    Returns free bytes on disk
*/

qint64 UsbStorageSrv::sizeofdrive(QString path)
{
    QStorageInfo info = QStorageInfo(path);
    return info.bytesFree();
}

}  // end of namespace ws
}  // end of namespace analogic
