/*!
* @file     archiveagentlistenerinterface.h
* @author   Agiliad
* @brief    This file contains interface to access Nss agent library.
* @date     July, 19 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_ARCHIVE_ARCHIVEAGENTLISTENERINTERFACE_H_
#define ANALOGIC_WS_COMMON_ARCHIVE_ARCHIVEAGENTLISTENERINTERFACE_H_

#include <analogic/nss/agent/archive/ArchiveAgentListener.h>
#include <analogic/nss/agent/archive/ArchiveBag.h>

#include <analogic/ws/common/accesslayer/agentlistener.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   ArchiveAgentListenerInterface
 * \brief   This class contains variable and function related to
 *           access Nss agent library for RemoteArchive.
 */
class ArchiveAgentListenerInterface : public AgentListener, public analogic::nss::ArchiveAgentListener {
 public:
    /*!
    * @fn       ArchiveAgentListenerInterface();
    * @param    QObject* - parent
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    explicit ArchiveAgentListenerInterface();

    /*!
    * @fn       ~ArchiveAgentListenerInterface
    * @param    None
    * @return   None
    * @brief    Destructor responsible for deinitialization of members, memory and resources.
    */
    ~ArchiveAgentListenerInterface();

    /*!
    * @fn       KeepAlive
    * @param    None
    * @return   None
    * @brief    Gives alive status of the workstation to the nss service.
    */
    virtual void KeepAlive(void);

    /*!
    * @fn       RegistrationChanged
    * @param    analogic::nss::OsrAgent* agent
    * @return   None
    * @brief    Gives Registration Changed status of the workstation to the nss service.
    */
    void RegistrationChanged(analogic::nss::ArchiveAgent* agent);

    /*!
    * @fn       RequestPullBagArchival
    * @param    analogic::nss::ArchiveBag*
    * @return   None
    * @brief    Requests to Pull bag from archive.
    */
    virtual void RequestPullBagArchival(analogic::nss::ArchiveBag* bag);
};
}  // end of namespace ws
}  // end of namespace analogic
#endif  // ANALOGIC_WS_COMMON_ARCHIVE_ARCHIVEAGENTLISTENERINTERFACE_H_
