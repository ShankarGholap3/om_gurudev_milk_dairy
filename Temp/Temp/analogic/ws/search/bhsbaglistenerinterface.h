/*!
* @file     bhsbaglistenerinterface.h
* @author   Agiliad
* @brief    This file contains classes and its functions related to BhsBagListenerInterface
*           which will contains interface for BHS bag.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef BHSBAGLISTENERINTERFACE_H
#define BHSBAGLISTENERINTERFACE_H
#include <string>
#include <QObject>
#include <analogic/ws/common.h>
#include<bhs/common/WksInternalBhsBagImpl.h>

namespace analogic {
namespace ws {

class ExternalBhsBagInterface;

/*!
 * \class   BhsBagListenerInterface
 * \brief   This class contains variable and function related to
 *          initialization of BhsBagListenerInterface which handles msgs from BHS.
 */
  class BhsBagListenerInterface:public QObject, public analogic::ancp00::WksInternalBhsBagImpl
{
    Q_OBJECT
public:
    /**
     * contstructor for class BhsBagListenerInterface
     */
    BhsBagListenerInterface();

    /**
     * destructor for class BhsBagListenerInterface
     */
    virtual ~BhsBagListenerInterface();

    /**
     * Commands the scanner to set a detection algorithm to be run on the bag.
     *
     * @param bhs_bag_id Bag to which this command applies
     * @param algorithm Algorithm to run
     */
    void AddBagAlgorithm(const std::string& bhs_bag_id, const std::string& algorithm);

    /**
     * Commands the scanner to add passenger information about the bag.
     *
     * @param bhs_bag_id Bag to which this command applies
     * @param info Passenger information regarding this bag
     */
    void AddBagIATAInfo(const std::string& bhs_bag_id, const std::string& info);

    /**
     * Notifies the scanner the BHS is finished adding information about the bag, such as
     * algorithms, IATA info, etc.
     *
     * @param bhs_bag_id Bag to which this command applies
     */
    void FinishedBagInfo(const std::string& bhs_bag_id);

    /**
     * Notifies the scanner that the BHS has automatically rejected the bag and sent it to search.
     * Any PVS decision will be ignored.
     *
     * @param bhs_bag_id Bag to which this command applies
     */
    void ForceRejectBag(const std::string& bhs_bag_id);

    /**
     * Command to a search workstation to request and review this bag.
     *
     * @param bhs_bag_id Bag to which this command applies
     */
    void ReviewBag(const std::string& bhs_bag_id);

    void IssueDiverterDecision(const std::string&, bool, const std::string&);

    void FinishedAddingBagInfo(const std::string&);

    void AddBagInfo(const std::string&, const std::string&,
                                  const std::string&);

    void SetBagAlgorithm(const std::string&, const std::string&);

    void SetExternalBagInterface(ExternalBhsBagInterface*);

    void Start(void);

    void PickupBag(const std::string& bhs_bag_id, const std::string& pickup_type);

    void InvalidateBagByRfid(const std::string& rfid);

signals:
    void sigReviewBag(const std::string& bhs_bag_id);
};

}//Ws

}//ANALOGIC

#endif // BHSBAGLISTENERINTERFACE_H
