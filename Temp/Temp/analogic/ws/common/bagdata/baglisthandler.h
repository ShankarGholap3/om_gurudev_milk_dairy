/*!
* @file     baglisthandler.h
* @author   Agiliad
* @brief    This file contains class and functions for handling baglist from different sources for local/remote archive
*           and network server.
* @date     Jan, 19 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef ANALOGIC_WS_COMMON_BAGDATA_BAGLISTHANDLER_H_
#define ANALOGIC_WS_COMMON_BAGDATA_BAGLISTHANDLER_H_

#include <QDir>
#include <QList>
#include <QMap>
#include <QObject>

#include <QJsonObject>
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonParseError>

#include <boost/shared_ptr.hpp>

#include <Logger.h>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/baginfo.h>
#include <analogic/ws/common/bagdata/bagdata.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <analogic/ws/common/utility/usbstorageservice.h>
#include <utility.h>

#ifdef WORKSTATION
#include <analogic/ws/osr/osrmanager.h>
#endif
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
 * \class   BagListHandler
 * \brief   This class contains variable and function for handling baglist
 * from different sources for local/remote archive and network server.
 */
class BagListHandler : public QObject
{
  Q_OBJECT
public:
  /*!
    * @fn       BagListHandler();
    * @param    None
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
  explicit BagListHandler();

  /*!
    * @fn       ~BagListHandler();
    * @param    None
    * @return   None
    * @brief    Destructor responsible for releasing the acquired resources.
    */
  virtual ~BagListHandler();

  /*!
    * @fn       bagListCommand
    * @param    QMLEnums::BagsListAndBagDataCommands command
    * @param    QVariant input1
    * @param    QVariantList& results
    * @return   None
    * @brief    Command to process.
    */
  void bagListCommand(QMLEnums::BagsListAndBagDataCommands command,
                      QVariant input1, QVariantList& results, QMLEnums::ArchiveFilterSrchLoc loc);


protected:
  /*!
    * @fn       deleteBag
    * @param    QVariant input
    * @return   bool - success or failure
    * @brief    Delete the selected archived bag.
    */
  virtual bool deleteBag(QVariant input);

  /*!
    * @fn       fetchBagInfoList
    * @param    QVariant
    * @return   None
    * @brief    Fetch the bag info list.
    */
  virtual void fetchBagInfoList(QVariant data = 0) = 0;

  /*!
    * @fn       getBagData
    * @param    boost::shared_ptr<BagData> bag
    * @param    const int& rowNumber
    * @return   None
    * @brief    Fetch the bag data.
    */
  virtual bool getBagData(boost::shared_ptr<BagData> bag, const int&
                          rowNumber) = 0;

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
    * @brief    Save the selected bag.
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
    * @fn       prepareBagListForDisplay
    * @param    QVariantList& results
    * @return   None
    * @brief    Put list data into results variable for sending to QML.
    */
  void prepareBagListForDisplay(QVariantList &results);

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
    * @fn       sort
    * @param    SortOrder sortOrder
    * @return   None
    * @brief    Sort the bag info list.
    */
  void sort(SortOrder sortOrder);


  /*!
    * @fn       sortList
    * @param    QVariant type
    * @return   None
    * @brief    Archive the bag to USB.
    */
  void sortList(QVariant type);

  /*!
    * @fn       applySearchCriteria
    * @param    const QVariant searchCriteria
    * @return   None
    * @brief    Apply the search criteria.
    */
  void applySearchCriteria(const QVariant searchCriteria);

private:
  /*!
    * @fn       showList
    * @param    const QVariant data
    * @param    bool refresh
    * @param    QVariantList& results
    * @return   None
    * @brief    Show the list of local archives on UI.
    */
  void showList(const QVariant data, bool refresh,
                QVariantList &results, QMLEnums::ArchiveFilterSrchLoc loc );

  /*!
    * @fn       archiveToConfiguredLocation
    * @param    BagData* bagData
    * @return   None
    * @brief    Archive bag data to configured path.
    */
  void archiveToConfiguredLocation(BagData* bagData);

signals:

  /*!
    * @fn       bagArchivedStatus
    * @param    int - errorcode
    * @param    QString - status
    * @param    int - modelno ( //QMLEnums::ModelEnum::QML_MESSAGE_MODEL)
    * @return   None
    * @brief    This function sends bag archived status signal to
    *           UI.
    */
  void bagArchivedStatus(int hr, QString status, int modelno);

  /*!
    * @fn       notifyToChangeOrNotScreen
    * @param    bool toBeChanged
    * @param    QString errorMsg
    * @param    int hr
    * @return   None
    * @brief    notifies to change screen or not.
    */
  void notifyToChangeOrNotScreen(bool toBeChanged, QString errorMsg, int hr);

protected:
  QList<BagInfo>  m_bagInfoList;              //!< Bag info list.
  QMap<QString, int> m_bagListMap;

private:
  SortOrder       m_bagInfoListSortOrder;     //!< Bag Info List sort order.
};

}  // end of namespace ws
}  // end of namespace analogic

#endif  // ANALOGIC_WS_COMMON_BAGDATA_BAGLISTHANDLER_H_
