/*!
* @file     rerunhandler.cpp
* @author   Agiliad
* @brief    This file contains handler, responsible rerun emulation tasks.
* @date     March, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/rerun/rerunhandler.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
   * @fn       RerunHandler
   * @param    None
   * @return   None
   * @brief    Constructor for class RerunHandler.
   */
RerunHandler::RerunHandler()
{   
    TRACE_LOG("");
    m_connectEmulator = new ConnectEmulator;
    m_rerunTaskConfig = new RerunTaskConfig;
    m_rerunBagList = new RerunBagList;
    m_usbStorageService = new UsbStorageSrv;
    connect(m_connectEmulator, SIGNAL(rerunResponse(QString,QMLEnums::RERUN_RESPONSES)),
            this, SLOT(rerunResponseRcv(QString,QMLEnums::RERUN_RESPONSES)));

}

/*!
* @fn       RerunHandler();
* @param    ConnectEmulator pointer
* @param    RerunTaskConfig pointer
* @param    RerunBagList pointer
* @param    UsbStorageSrv pointer
* @return   None
* @brief    Parameterised constructor responsible for initialization of class members, memory and resources.
* @note     Function is Prepared to mock test suites
*/
RerunHandler::RerunHandler(ConnectEmulator* pobjConnectEmulator,
                           RerunTaskConfig* pobjRerunTaskConfig,
                           RerunBagList* pobjRerunBagList,
                           UsbStorageSrv* pobjUsbStorageSrv)
{
    TRACE_LOG("");
    m_connectEmulator   = pobjConnectEmulator;
    m_rerunTaskConfig   = pobjRerunTaskConfig;
    m_rerunBagList      = pobjRerunBagList;
    m_usbStorageService = pobjUsbStorageSrv;
}

/*!
   * @fn       RerunHandler
   * @param    None
   * @return   None
   * @brief    Destructor for class RerunHandler.
   */
RerunHandler::~RerunHandler()
{
    TRACE_LOG("");
    SAFE_DELETE(m_connectEmulator);
    SAFE_DELETE(m_rerunTaskConfig);
    SAFE_DELETE(m_rerunBagList);
    SAFE_DELETE(m_usbStorageService);

}

/*!
* @fn       init();
* @param    None
* @return   int
* @brief    initialize rerun handler.
*/
int RerunHandler::init()
{
    TRACE_LOG("");
    if (m_connectEmulator != NULL)
    {
        int ret = m_connectEmulator->initialize();
        if (ret != -1)
        {
            INFO_LOG("Initialized connect emulator");
            int alg_ret = m_connectEmulator->get_emulator_helper()->get_algorithms();
            INFO_LOG("algorithms number: " << alg_ret);
        }
    }    
    return 0;

}

/*!
* @fn       getConnectEmulator();
* @param    None
* @return   ConnectEmulator*
* @brief    get connect emulator
*/
ConnectEmulator* RerunHandler::getConnectEmulator()
{
    return m_connectEmulator;

}

/*!
* @fn       onUpdateState();
* @param    QMLEnums::AppState state
* @return   void
* @brief    call on update state.
*/
void RerunHandler::onUpdateState(QMLEnums::AppState state)
{
    Q_UNUSED(state);
    TRACE_LOG("");
}


/*!
* @fn       onDeAuthenticate
* @param    None
* @return   None
* @brief    Call for Deauthenticate user
*/
void RerunHandler::onDeAuthenticate()
{
    INFO_LOG("Deauthenticate user");
    getUsbStorageService()->setLoggedIn(false);
    m_usbStorageService->unMountUsb();
}

/*!
* @fn       rerunResponseRcv
* @param    QString status
* @para     QMLEnums::RERUN_RESPONSES
* @return   None
* @brief    recieves status of rerun task.
*/
void RerunHandler::rerunResponseRcv(QString status, QMLEnums::RERUN_RESPONSES response)
{
    Q_UNUSED(status)
    switch(response)
    {
    case QMLEnums::RERUN_RESPONSES::RERUN_RERUN_DONE_RSP:
    {
        QString inputFile = m_rerunTaskConfig->selectedBagListFile();
        QString resultFolder = m_rerunTaskConfig->resultOutputFolder();
        m_connectEmulator->updateOutputFile(inputFile, resultFolder);
        break;
    }
    default:
        break;
    }
}

/*!
* @fn       openBagInputFolder();
* @param    None
* @return   None
* @brief   open bag input folder
*/
void RerunHandler::openBagInputFolder()
{
    /*
    QFileDialog dialog();
    dialog.setFileMode(QFileDialog::AnyFile);

    dialog.exec();

    QString fileName = QFileDialog::getOpenFileName(NULL,
        tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
*/
}

/*!
* @fn       startRerunTask();
* @param    None
* @return   void
* @brief    start rerun task.
*/
void RerunHandler::startRerunTask()
{
    std::string inputFile = m_rerunTaskConfig->selectedBagListFile().toStdString();
    std::string resultFolder = m_rerunTaskConfig->resultOutputFolder().toStdString();
    int alg = m_rerunTaskConfig->getAlgorithm();
    m_connectEmulator->copyDataToLocalStorage(inputFile);
    m_connectEmulator->checkForFileFormatConversions(inputFile);
    m_connectEmulator->start_rerun(inputFile,resultFolder,alg);
}

/*!
* @fn       cancelRerunTask();
* @param    None
* @return   void
* @brief    cancel rerun task.
*/
void RerunHandler::cancelRerunTask()
{
    m_connectEmulator->cancel_rerun();

}

/*!
* @fn       setTaskConfig();
* @param    RerunTaskConfig config
* @return   None
* @brief    sets run task config
*/
void RerunHandler::setTaskConfig(RerunTaskConfig config)
{
    m_rerunTaskConfig->setBagInputFolder(config.bagInputFolder());
    m_rerunTaskConfig->setResultOutputFolder(config.resultOutputFolder());
    m_rerunTaskConfig->setSelectedBagListFile(config.selectedBagListFile());
    m_rerunTaskConfig->setAlgorithm(config.getAlgorithm());
    m_rerunTaskConfig->setIsRun(config.getIsRun());
}

/*!
* @fn       getUsbStorageService();
* @param    None
* @return   UsbStorageSrv*
* @brief    get usbStorageService;
*/
UsbStorageSrv* RerunHandler::getUsbStorageService()
{
    return m_usbStorageService;
}


}  // end of namespace ws
}  // end of namespace analogic
