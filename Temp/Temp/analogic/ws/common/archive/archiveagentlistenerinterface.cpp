/*!
* @file     archiveagentlistenerinterface.cpp
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     July, 19 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/archive/archiveagentlistenerinterface.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       ArchiveAgentListenerInterface();
* @param    QObject* - parent
* @return   None
* @brief    Constructor responsible for initialization of class members, memory and resources.
*/
ArchiveAgentListenerInterface::ArchiveAgentListenerInterface()
{
    TRACE_LOG("");
}

/*!
* @fn       ~ArchiveAgentListenerInterface
* @param    None
* @return   None
* @brief    Destructor responsible for deinitialization of members, memory and resources.
*/
ArchiveAgentListenerInterface::~ArchiveAgentListenerInterface()
{
    TRACE_LOG("");
}

/*!
* @fn       KeepAlive
* @param    None
* @return   None
* @brief    Gives alive status of the workstation to the nss service.
*/
void ArchiveAgentListenerInterface::KeepAlive(void) {
    TRACE_LOG("");
}

/*!
* @fn       RegistrationChanged
* @param    analogic::nss::OsrAgent* agent
* @return   None
* @brief    Gives Registration Changed status of the workstation to the nss service.
*/
void ArchiveAgentListenerInterface::RegistrationChanged(analogic::nss::ArchiveAgent* agent) {
    Q_UNUSED(agent);
    TRACE_LOG("");
}

/*!
* @fn       RequestPullBagArchival
* @param    analogic::nss::ArchiveBag*
* @return   None
* @brief    Requests to Pull bag from archive.
*/
void ArchiveAgentListenerInterface::RequestPullBagArchival(analogic::nss::ArchiveBag* bag) {
    TRACE_LOG("");
    if ( NULL != bag)
    {
        DEBUG_LOG("Bag Flow Event :: Bag Notification received for Bag Identifier" <<bag->get_transfer_bag_identifier());
        // notify workstation about the bag
        emit requestBagDecision(bag, QMLEnums::ArchiveFilterSrchLoc::REMOTE_SERVER,"",QMLEnums::BagsListAndBagDataCommands::BLBDC_ARCHIVE_BAG_TO_USB);
    }
}
}
}
