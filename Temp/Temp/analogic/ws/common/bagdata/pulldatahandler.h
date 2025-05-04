/*!
* @file     pulldatahandler.h
* @author   Agiliad
* @brief    This file contains functionality related to pulling bag, when bag will be ready from scanner for processing.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_COMMON_BAGDATA_PULLDATAHANDLER_H_
#define ANALOGIC_WS_COMMON_BAGDATA_PULLDATAHANDLER_H_


#include <QString>
#include <QObject>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdataacqhandler.h>
#include <analogic/ws/common/accesslayer/scanneraccessinterface.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
#define DEFAULT_BAGDATA_SERVER_IP "localhost"
#define DEFAULT_BAGDATA_SERVER_PORT 8282

/*!
 * \class   PullDataHandler
 * \brief   This class contains variable and function related to
 *          Pull Data Handler.
 */
class PullDataHandler: public BagDataAcqHandler
{
    Q_OBJECT
public:
    /*!
    * @fn       PullDataHandler
    * @parma    QThread *thread
    * @param    QObject *parent - parent
    * @return   None
    * @brief    Constructor for class PullDataHandler.
    */
    explicit PullDataHandler(QThread* thread, QObject* parent = NULL);

    /*!
    * @fn       ~PullDataHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class PullDataHandler.
    */
    ~PullDataHandler();

    /*!
    * @fn       startBagRecv
    * @param    None
    * @return   Errors::RESULT
    * @brief    start bag data receive.
    */
    Errors::RESULT startBagRecv();

    /*!
    * @fn       stopToReceiveBag
    * @param    None
    * @return   None
    * @brief    notify scannerAccessInterface to stop receiving current bag as network is disconnected.
    */
    virtual void stopToReceiveBag();

public slots:

    /*!
    * @fn       bagDataReceived
    * @param    boost::shared_ptr<BagData> bagdata
    * @param    bool status
    * @param    QString message
    * @return   None
    * @brief    This function will call on bag data received
    */
    void bagDataReceived(boost::shared_ptr<BagData> bagdata, bool status,
                         QString message);

    /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   None
    * @brief    call on update state.
    */
    void onUpdateState(QMLEnums::AppState state);

signals:
    /*!
    * @fn       stateChanged
    * @param    AppState
    * @return   None
    * @brief    This function will call on app state change
    */
    void stateChanged(QMLEnums::AppState);

private:
    ScannerAccessInterface*    m_sal;    //!< handle to scanner access interface
};
}  // end of namespace ws
}  // end of namespace analogic


#endif  // ANALOGIC_WS_COMMON_BAGDATA_PULLDATAHANDLER_H_

