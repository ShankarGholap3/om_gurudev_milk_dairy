#ifndef RERUNDATAACQHANDLER_H
#define RERUNDATAACQHANDLER_H

#include <QTimer>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <semaphore.h>
#include <analogic/ws/workstationmanager.h>
#include <analogic/ws/common/bagdata/bagdataqueue.h>

#include <analogic/ws/rerun/rerunbaglist.h>
#include <analogic/ws/rerun/rerunconfig.h>
#include <analogic/ws/rerun/usbfilesystemmodel.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{

namespace ws
{
class RerunDataAcqHandler:public QObject
{
  Q_OBJECT
public:
  /*!
  * @fn       BagDataAcqHandler
  * @param    QThread* - thread
  * @param    QObject *parent - parent
  * @return   None
  * @brief    Constructor for class BagDataAcqHandler.
  */
  explicit RerunDataAcqHandler();

  /*!
  * @fn       ~BagDataAcqHandler
  * @param    None
  * @return   None
  * @brief    Destructor for class BagDataAcqHandler.
  */
  virtual ~RerunDataAcqHandler();

  /*!
  * @fn       getBagDataacqHdlrThread
  * @param    None
  * @return   QThread* thread handle
  * @brief    Returns handle to BagDataAcqHandler thread.
  */
  QThread* getRerunDataacqHdlrThread();

  /*!
  * @fn       initRerunDataAcqHandler
  * @param    None
  * @return   None
  * @brief    init for class BagDataAcqHandler.
  */
  void initRerunDataAcqHandler();

signals:
    /*!
    * @fn       exit
    * @param    None
    * @return   None
    * @brief    signals exit
    */
    void exit();

    /*!
    * @fn       viewbags
    * @param    None
    * @return   None
    * @brief    signals to view bag list
    */
    void viewbags();

    /*!
    * @fn       notifyToChangeOrNotScreen
    * @param    bool toBeChanged
    * @param    QString erroMsg
    * @param    int hr
    * @return   None
    * @brief    notifies viewmanager to change screen or not.
    */
    void notifyToChangeOrNotScreen(bool toBeChanged, QString erroMsg, int hr);

    /*!
    * @fn       showNextBag
    * @param    None
    * @return   None
    * @brief    signals to show next bag
    */
    void showNextBag();

public slots:
    /*!
    * @fn       onExit
    * @param    None
    * @return   None
    * @brief    calls on exit
    */
    void onExit();

    /*!
    * @fn       onThreadStarted
    * @param    None
    * @return   void
    * @brief    on thread start this function will initialize models.
    */
    void onThreadStarted();

    /*!
    * @fn       getBagData
    * @param    None
    * @return   void
    * @brief    get Bag data
    */
    void getBagData();

    /*!
    * @fn       onViewSelectedBags
    * @param    QVariantList
    * @param    QModelIndex outputFilePathIndex
    * @param    QString opID
    * @return   None
    * @brief    notifies workstationManager when ViewBags is pressed.
    */
    void onViewSelectedBags(QVariantList selectedBagsList, QModelIndex outputFilePathIndex, QString opID = "");

public:
    static int                         count;                      //!< number of bags in list

protected:
    QTimer*                            m_pUpdateTimer;  //!< handle for timer
    QThread*                           m_Thread;        //!< handle for thread
    BagsDataPool*                      m_bagqueue;
    QStringList                        m_selectedBagList;          //!< handle to selected list of bags
    int                                m_currentBagIndex;          //!< Current Bag Index
    QString                            m_outputFilePath;           //!< handle to output folder path which contains

};
}  // end of namespace ws
}  // end of namespace analogic

#endif // RERUNDATAACQHANDLER_H
