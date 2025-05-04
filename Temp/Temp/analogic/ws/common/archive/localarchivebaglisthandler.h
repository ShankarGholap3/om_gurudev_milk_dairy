/*!
* @file     localarchivebaglisthandler.h
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from local system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ARCHIVE_LOCALARCHIVEBAGLISTHANDLER_H_
#define ANALOGIC_WS_COMMON_ARCHIVE_LOCALARCHIVEBAGLISTHANDLER_H_

#include <analogic/ws/common/bagdata/baglisthandler.h>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   LocalArchiveBagListHandler
 * \brief   This class contains variable and function related to
 *          Local Archive Handler
 */
class LocalArchiveBagListHandler : public BagListHandler
{
  Q_OBJECT
public:
    /*!
    * @fn       LocalArchiveBagListHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class LocalArchiveBagListHandler.
    */
    LocalArchiveBagListHandler();

    /*!
    * @fn       ~LocalArchiveBagListHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class LocalArchiveBagListHandler.
    */
    ~LocalArchiveBagListHandler();

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

private:
    /*!
    * @fn       deleteBag
    * @param    QVariant input
    * @return   bool - success or failure
    * @brief    Delete the selected archived bag.
    */
    bool deleteBag(QVariant input) override;

    /*!
    * @fn       fetchBagInfoList
    * @param    None
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
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ARCHIVE_LOCALARCHIVEBAGLISTHANDLER_H_

