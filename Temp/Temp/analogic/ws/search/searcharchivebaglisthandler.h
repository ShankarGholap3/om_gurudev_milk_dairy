/*!
* @file     searcharchivebaglisthandler.h
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from remote system.
* @date     Feb, 24 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ARCHIVE_SEARCHARCHIVEBAGLISTHANDLER_H_
#define ANALOGIC_WS_COMMON_ARCHIVE_SEARCHARCHIVEBAGLISTHANDLER_H_

#include <analogic/ws/common/bagdata/remotebaglisthandler.h>
#include <analogic/ws/common/bagdata/baginfo.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   SearchArchiveBagListHandler
 * \brief   This class contains variable and function related to
 *          Remote Archive Handler
 */
class SearchArchiveBagListHandler : public RemoteBagListHandler
{
    Q_OBJECT
public:
    /*!
    * @fn       SearchArchiveBagListHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class SearchArchiveBagListHandler.
    */
    SearchArchiveBagListHandler();

    /*!
    * @fn       ~SearchArchiveBagListHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class SearchArchiveBagListHandler.
    */
    ~SearchArchiveBagListHandler();

    /*!
    * @fn       showBag
    * @param    QVariant input
    * @return   None
    * @brief    Show the selected bag.
    */
    virtual void showBag(QVariant input);

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
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ARCHIVE_SEARCHARCHIVEBAGLISTHANDLER_H_

