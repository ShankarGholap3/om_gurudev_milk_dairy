#ifndef TRAININGHANDLER_H
#define TRAININGHANDLER_H

#include <boost/shared_ptr.hpp>
#include <analogic/ws/common.h>
#include <analogic/ws/nsshandler.h>
#include <analogic/ws/common/accesslayer/nssagentaccessinterface.h>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   TrainingHandler
 * \brief   This class contains functionality related to OSR handler - scanner agent ,
 *          osr agent , OSR state and bag notification
 */
class TrainingHandler: public NssHandler
{
    Q_OBJECT
public:
    /*!
    * @fn       TrainingHandler
    * @param    None
    * @return   None
    * @brief    Constructor for class TrainingHandler.
    */
    TrainingHandler();

    /*!
    * @fn       TrainingHandler
    * @param    NSSAgentAccessInterface*
    * @return   None
    * @brief    Constructor for class TrainingHandler , needed for mocking nss in unit testing.
    */
    explicit TrainingHandler(NSSAgentAccessInterface* nssaccess);

    /*!
    * @fn       ~TrainingHandler
    * @param    None
    * @return   None
    * @brief    Destructor for class NetworkFaultHandler.
    */
    ~TrainingHandler();

    /*!
    * @fn       setOsrReadyState
    * @param    bool
    * @return   int
    * @brief    Sets OSR ready state.
    */
    int setOsrReadyState(bool state);



signals:
    /*!
    * @fn       stateChanged
    * @param    AppState
    * @return   None
    * @brief    signal state changed
    */
    void stateChanged(QMLEnums::AppState);

public slots:
    /*!
    * @fn       onUpdateState();
    * @param    QMLEnums::AppState state
    * @return   void
    * @brief    call on update state.
    */
    void onUpdateState(QMLEnums::AppState state);

protected:
private:
};
}  // end of namespace ws
}  // end of namespace analogic


#endif // TRAININGHANDLER_H
