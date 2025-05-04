/*!
* @file     remotebaglisthandler.h
* @author   Agiliad
* @brief    This file contains class and functions for handling baglist from different sources for remote archive and search .
*           and network server.
* @date     Jan, 24 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef REMOTEBAGLISTHANDLER_H
#define REMOTEBAGLISTHANDLER_H

#include <QList>
#include <QVariantMap>
#include <boost/shared_ptr.hpp>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/baginfo.h>
#include <analogic/ws/common/bagdata/baglisthandler.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class BagData;
class WorkstationManager;
class BagListHandler;

/*!
 * \class   RemoteBagListHandler
 * \brief   This class contains variable and function for handling baglist
 * from remote sources like remote archive and network server for search workstation.
 */
class RemoteBagListHandler: public BagListHandler
{
    Q_OBJECT
public:
    /*!
    * @fn       RemoteBagListHandler();
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    explicit RemoteBagListHandler();

    /*!
    * @fn       ~RemoteBagListHandler();
    * @param    None
    * @return   None
    * @brief    Destructor responsible for releasing the acquired resources.
    */
    virtual ~RemoteBagListHandler();

    /*!
    * @fn       fetchBagInfoList
    * @param    QVariant
    * @return   None
    * @brief    Load bag info list.
    */
    virtual void fetchBagInfoList(QVariant data = 0) = 0;

    /*!
    * @fn       getBagData
    * @param    boost::shared_ptr<BagData> bag
    * @param    const int& rowNumber
    * @return   None
    * @brief    Fetch the bag data.
    */
    bool getBagData(boost::shared_ptr<BagData> bag, const int& rowNumber)
    override;
signals:
    /*!
    * @fn       getBagDataFromServer
    * @param    QMLEnums::ArchiveFilterSrchLoc
    * @param    std::string nss_bag_id
    * @param    QMLEnums::BagsListAndBagDataCommands mode)
    * @return   None
    * @brief    notifies to get BagData from server.
    */
    void getBagDataFromServer(QMLEnums::ArchiveFilterSrchLoc, std::string nss_bag_id , QMLEnums::BagsListAndBagDataCommands mode);
};
}  // end of namespace ws
}  // end of namespace analogic

#endif // REMOTEBAGLISTHANDLER_H
