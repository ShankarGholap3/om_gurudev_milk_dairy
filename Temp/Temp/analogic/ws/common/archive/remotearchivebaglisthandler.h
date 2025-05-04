/*!
* @file     remotearchivebaglisthandler.h
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from remote system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ARCHIVE_REMOTEARCHIVEBAGLISTHANDLER_H_
#define ANALOGIC_WS_COMMON_ARCHIVE_REMOTEARCHIVEBAGLISTHANDLER_H_

#include <analogic/ws/common/bagdata/remotebaglisthandler.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   RemoteArchiveBagListHandler
 * \brief   This class contains variable and function related to
 *          Remote Archive Handler
 */
class RemoteArchiveBagListHandler : public RemoteBagListHandler
{
  Q_OBJECT
public:
    /*!
    * @fn       RemoteArchiveBagListHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class RemoteArchiveBagListHandler.
    */
    RemoteArchiveBagListHandler();

    /*!
    * @fn       ~RemoteArchiveBagListHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class RemoteArchiveBagListHandler.
    */
    ~RemoteArchiveBagListHandler();

    /*!
    * @fn       showBag
    * @param    QVariant input
    * @return   None
    * @brief    Show the selected bag.
    */
    virtual void showBag(QVariant input);

    /*!
    * @fn       saveBag
    * @param    QVariant input
    * @return   int: Error code
    * @brief    save the selected bag.
    */
    virtual int saveBag(QVariant input);

    /*!
    * @fn       overWriteFile
    * @param    QVariant input
    * @return   bool
    * @brief    over wrirte the selected bag.
    */
    virtual bool overWriteFile(QVariant input);

    /*!
    * @fn       deleteBag
    * @param    QVariant input
    * @return   None
    * @brief    delete the selected bag.
    */
    virtual bool deleteBag(QVariant input);

public slots:
    /*!
    * @fn       onSaveRemoteArchiveBag
    * @param    boost::shared_ptr<BagData> bagdata
    * @return   None
    * @brief    save the selected remote bag.
    */
void onSaveRemoteArchiveBag(boost::shared_ptr<BagData> bagdata);

private:
    /*!
    * @fn       fetchBagInfoList
    * @param    QVariant
    * @return   None
    * @brief    Load bag info list.
    */
    void fetchBagInfoList(QVariant data = 0) override;

    /*!
    * @fn       getBagData
    * @param    boost::shared_ptr<BagData> bag
    * @param    const int& rowNumber
    * @return   None
    * @brief    Fetch the bag data.
    */
    bool getBagData(boost::shared_ptr<BagData> bag, const int& rowNumber) override;

    /*!
    * @fn       displaySaveOperationResult
    * @param    None
    * @return   None
    * @brief    display save operation result.
    */
    void displaySaveOperationResult();

    QString m_usbArchiveTargetPath;            //!< usb archive target path
    QString m_bagfilenamepattern;
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ARCHIVE_REMOTEARCHIVEBAGLISTHANDLER_H_

