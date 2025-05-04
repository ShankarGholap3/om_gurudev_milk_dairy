/*!
* @file     remotearchive.h
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list from remote system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ARCHIVE_REMOTEARCHIVE_H_
#define ANALOGIC_WS_COMMON_ARCHIVE_REMOTEARCHIVE_H_

#include <analogic/ws/common/archive/archive.h>
#include <QObject>
#include <QList>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   RemoteArchive
 * \brief   This class contains variable and function related to
 *          Remote Archive Handler
 */
class RemoteArchive : public Archive
{
public:
    /*!
    * @fn       RemoteArchive
    * @param    WorkstationManager* wsm
    * @return   None
    * @brief    Constructor for class RemoteArchive.
    */
    RemoteArchive(WorkstationManager* wsm);

    /*!
    * @fn       ~RemoteArchive
    * @param    None
    * @return   None
    * @brief    Destructor for class RemoteArchive.
    */
    ~RemoteArchive();

private:
    /*!
    * @fn       deleteBag
    * @param    QStringList input
    * @return   bool - success or failure
    * @brief    Delete the selected archived bag.
    */
    bool deleteBag(QStringList input) override;

    /*!
    * @fn       fetchBagInfoList
    * @param    None
    * @return   None
    * @brief    Load bag info list.
    */
    void fetchBagInfoList() override;

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

#endif  // ANALOGIC_WS_COMMON_ARCHIVE_REMOTEARCHIVE_H_

