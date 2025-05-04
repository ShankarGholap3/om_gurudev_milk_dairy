/*!
* @file     archive.h
* @author   Agiliad
* @brief    This file contains interface to save or retrieve bag list.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ARCHIVE_ARCHIVE_H_
#define ANALOGIC_WS_COMMON_ARCHIVE_ARCHIVE_H_

#include <QObject>
#include <QList>
#include <QVariantMap>
#include <boost/shared_ptr.hpp>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/baginfo.h>
#include <analogic/ws/common/bagdata/remotebaglisthandler.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
class BagData;
class WorkstationManager;

/*!
 * \class   Archive
 * \brief   This class contains variable and function related to
 *          Archive Handler
 */
class Archive: public RemoteBagListHandler
{
    Q_OBJECT
public:
    /*!
    * @fn       Archive
    * @param    WorkstationManager* wsm
    * @return   None
    * @brief    Constructor for class Archive.
    */
    Archive(WorkstationManager* wsm);

    /*!
    * @fn       ~Archive
    * @param    None
    * @return   None
    * @brief    Destructor for class Archive.
    */
    virtual ~Archive();

    /*!
    * @fn       archiveCommand
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @param    QStringList input number 1
    * @param    BagData* bagData
    * @param    QVariantMap& results
    * @return   None
    * @brief    Command to process.
    */
    void archiveCommand(QMLEnums::BagsListAndBagDataCommands command, QStringList input1, BagData* bagData,
                        QVariantMap& results);

protected:
    /*!
    * @fn       deleteBag
    * @param    QStringList input
    * @return   bool - success or failure
    * @brief    Delete the selected archived bag.
    */
    virtual bool deleteBag(QStringList input) = 0;

    /*!
    * @fn       fetchBagInfoList
    * @param    None
    * @return   None
    * @brief    Fetch the bag info list.
    */
    virtual void fetchBagInfoList() = 0;

    /*!
    * @fn       getBagData
    * @param    boost::shared_ptr<BagData> bag
    * @param    const int& rowNumber
    * @return   None
    * @brief    Fetch the bag data.
    */
    virtual bool getBagData(boost::shared_ptr<BagData> bag, const int& rowNumber) = 0;

private:
    /*!
     * \brief   Enum for the current order in which bag info list is sorted.
     */
    enum SortOrder
    {
        NONE,
        ASC_BAGID,
        DEC_BAGID,
        ASC_TIME,
        DEC_TIME
    };

    /*!
    * @fn       archiveToConfiguredLocation
    * @param    BagData* bagData
    * @return   None
    * @brief    Archive bag data to configured path.
    */
    void archiveToConfiguredLocation(BagData* bagData);

    /*!
    * @fn       showList
    * @param    const QStringList& searchCriteria
    * @param    bool refresh
    * @param    QVariantMap& results
    * @return   None
    * @brief    Show the list of local archives on UI.
    */
    void showList(const QStringList& searchCriteria, bool refresh, QVariantMap& results);

    /*!
    * @fn       applySearchCriteria
    * @param    const QStringList& searchCriteria
    * @return   None
    * @brief    Apply the search criteria.
    */
    void applySearchCriteria(const QStringList& searchCriteria);

    /*!
    * @fn       showBag
    * @param    QStringList input
    * @return   None
    * @brief    Show the selected bag.
    */
    void showBag(QStringList input);

    /*!
    * @fn       archiveToUSB
    * @param    None
    * @return   None
    * @brief    Archive the bag to USB.
    */
    void archiveToUSB();

    /*!
    * @fn       sortList
    * @param    QStringList& type
    * @return   None
    * @brief    Archive the bag to USB.
    */
    void sortList(QStringList& type);

    /*!
    * @fn       prepareBagListForDisplay
    * @param    QVariantMap& results
    * @return   None
    * @brief    Put list data into results variable for sending to QML.
    */
    void prepareBagListForDisplay(QVariantMap& results);

    /*!
    * @fn       sort
    * @param    SortOrder sortOrder
    * @return   None
    * @brief    Sort the bag info list.
    */
    void sort(SortOrder sortOrder);

protected:
    QList<BagInfo>                      m_bagInfoList;              //!< Bag info list.
//    boost::shared_ptr<BagListHandler>   m_bagListHandler;

private:
    WorkstationManager*      m_workstationManagerRef;    //!< Ref to workstation manager.
    SortOrder                m_bagInfoListSortOrder;     //!< Bag Info List sort order.
};
}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_ARCHIVE_ARCHIVE_H_

