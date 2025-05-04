/*!
* @file     bhsbaglistenerinterface.cpp
* @author   Agiliad
* @brief    This file contains classes and its functions related to BhsBagListenerInterface
*           which will contains interface for BHS bag.
* @date     Sep, 26 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include "bhsbaglistenerinterface.h"
namespace analogic
{
namespace ws
{
/**
 * contstructor for class BhsBagListenerInterface
 */
BhsBagListenerInterface::BhsBagListenerInterface()
{

}
/**
 * destructor for class BhsBagListenerInterface
 */
BhsBagListenerInterface::~BhsBagListenerInterface()
{

}

/**
 * Commands the scanner to set a detection algorithm to be run on the bag.
 *
 * @param bhs_bag_id Bag to which this command applies
 * @param algorithm Algorithm to run
 */
void BhsBagListenerInterface::AddBagAlgorithm(const std::string& bhs_bag_id, const std::string& algorithm)
{
    Q_UNUSED(bhs_bag_id);
    Q_UNUSED(algorithm);
}

/**
 * Commands the scanner to add passenger information about the bag.
 *
 * @param bhs_bag_id Bag to which this command applies
 * @param info Passenger information regarding this bag
 */
void BhsBagListenerInterface::AddBagIATAInfo(const std::string& bhs_bag_id, const std::string& info)
{
    Q_UNUSED(bhs_bag_id);
    Q_UNUSED(info);
}

/**
 * Notifies the scanner the BHS is finished adding information about the bag, such as
 * algorithms, IATA info, etc.
 *
 * @param bhs_bag_id Bag to which this command applies
 */
void BhsBagListenerInterface::FinishedBagInfo(const std::string& bhs_bag_id)
{
    Q_UNUSED(bhs_bag_id);
}

/**
 * Notifies the scanner that the BHS has automatically rejected the bag and sent it to search.
 * Any PVS decision will be ignored.
 *
 * @param bhs_bag_id Bag to which this command applies
 */
void BhsBagListenerInterface::ForceRejectBag(const std::string& bhs_bag_id)
{
    Q_UNUSED(bhs_bag_id);
}

/**
 * Command to a search workstation to request and review this bag.
 *
 * @param bhs_bag_id Bag to which this command applies
 */
void BhsBagListenerInterface::ReviewBag(const std::string& bhs_bag_id)
{
    INFO_LOG("Review bag request from BHS:" << bhs_bag_id.c_str())
    emit sigReviewBag(bhs_bag_id);
}

void BhsBagListenerInterface::IssueDiverterDecision(const std::string&, bool, const std::string&)
{

}

void BhsBagListenerInterface::FinishedAddingBagInfo(const std::string&)
{

}

void BhsBagListenerInterface::AddBagInfo(const std::string &, const std::string &, const std::string &)
{

}

void BhsBagListenerInterface::SetBagAlgorithm(const std::string &, const std::string &)
{

}

void BhsBagListenerInterface::SetExternalBagInterface(ExternalBhsBagInterface *)
{

}

void BhsBagListenerInterface::Start()
{

}

void BhsBagListenerInterface::PickupBag(const std::string &bhs_bag_id, const std::string& pickup_type)
{

}

void BhsBagListenerInterface::InvalidateBagByRfid(const std::string &rfid)
{

}
}
}
