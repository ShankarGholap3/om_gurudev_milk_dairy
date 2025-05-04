/*!
* @file     scanneraccessinterface.cpp
* @author   Agiliad
* @brief    This file contains interface to provide live bag data from scanner using streaming xfer library
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/accesslayer/scanneraccessinterface.h>
#include <analogic/ws/common/utility/usbstorageservice.h>


namespace analogic
{
namespace ws
{
boost::shared_ptr<BagData> ScannerAccessInterface::m_bagdata;
ScannerAccessInterface* ScannerAccessInterface::m_instance = NULL;
analogic::streaming_xfer::AlogStreamingBagReceiver* ScannerAccessInterface::m_receiver = NULL;
int ScannerAccessInterface::m_ctcount;
int ScannerAccessInterface::m_tdrcount;
bool ScannerAccessInterface::m_bag_loading = false;
bool ScannerAccessInterface::m_eof = false;

/*!
* @fn       ScannerAccessInterface
* @param    QObject *parent - parent
* @return   None
* @brief    Constructor for class ScannerAccessInterface.
*/
ScannerAccessInterface::ScannerAccessInterface(QObject * parent) :
  QObject(parent),
  analogic::streaming_xfer::AlogStreamingBagReceiverListener()
{
  m_tdrcount = 0;
  m_ctcount = 0;
  m_bag_loading = false;
  m_eof = false;
}

/*!
* @fn       getBagdata
* @param    None
* @return   static BagData *
* @brief    gets bag data
*/
boost::shared_ptr<BagData> ScannerAccessInterface::getBagdata()
{
  return m_bagdata;
}

/*!
* @fn       setBagdata
* @param    boost::shared_ptr<BagData> - bagdata
* @return   None
* @brief    sets bag data
*
*/
void ScannerAccessInterface::setBagdata(boost::shared_ptr<BagData> bagdata)
{
  m_bagdata = bagdata;
}

/*!
* @fn       getInstance
* @param    None
* @return   boost::shared_ptr<ScannerAccessInterface>
* @brief    gets singleton instance of class
*
*/
ScannerAccessInterface* ScannerAccessInterface::getInstance()
{
  TRACE_LOG("");
  ANALOGIC_STREAMING_XFER_VERIFY_VERSION;
  INFO_LOG("Streaming Transfer library version: "
           << analogic::streaming_xfer::GetAlogStreamingXferLibraryVersion());
  if(!m_instance)
  {
    INFO_LOG("Creating ScannerAccessInterface instance");
    m_instance = new ScannerAccessInterface();
    THROW_IF_FAILED((m_instance == NULL) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
  }
  return m_instance;
}

/*!
* @fn       errorPopup
* @param    None
* @return   None
* @brief    send error popup in case of error.
*
*/
void ScannerAccessInterface::errorPopup(std::string bagid)
{
  if ( m_bagdata.get() != NULL)
  {
    if (m_bagdata->getLivebag())
    {
      bagid = m_bagdata->getLivebag()->get_transfer_bag_identifier();
    }

    QString msg;
    if(m_bagdata->getBagReqLoc() == QMLEnums::BHS_SERVER)
    {
      bagid = m_bagdata->getBHSBagId();
      DEBUG_LOG("BHS Bag ID (RFID) For the Live bag is " << m_bagdata->getBHSBagId().c_str());
      msg = tr("RFID: ") + QString(bagid.c_str()) + tr(" cannot be retrieved.<br />");
    }
    else
    {
      msg = tr("Bag ID: ") + bagid.c_str() + tr(" not found.<br />");
    }
    ERROR_LOG(msg.toStdString());
    msg.append(m_bagdata->getErrorMsg());
    m_bagdata->clearErrorMsg();
    bool status = false;
    UsbStorageSrv srv;
    srv.unMountUsb();
    emit bagReceived(m_bagdata, status, msg);
  }
}

/*!
* @fn       closeReceiver
* @param    bool dueToDisconnection
* @return   None
* @brief    sets bag data
*
*/
void ScannerAccessInterface::closeReceiver(bool dueToDisconnection)
{
  if (m_receiver != NULL) {
    m_receiver->Close();
    m_bag_loading = false;
  }
  if (dueToDisconnection && (m_bagdata != NULL)) {
    m_bagdata->setState(BagData::ERROR);
  }
}
/*!
* @fn       PropertyReceived
* @param    analogic::streaming_xfer::AlogStreamingBagReceiver* - receiver
* @param    std::string- key
* @param    std::string - value
* @return   None
* @brief    property received
*
*/
void ScannerAccessInterface::PropertyReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                                              const std::string& key, const std::string& value)
{
  Q_UNUSED(receiver);
  TRACE_LOG("");

  INFO_LOG("PropertyReceived " << key << ":" <<value);
  if(m_bagdata == NULL)
  {
    QString msg = tr("Bag data is not set");
    ERROR_LOG(msg.toStdString());
    return;
  }
  else if(value.size()==0 && m_bagdata->getLivebag())
  {
    QString msg = tr("Property received. Key:") +
        key.c_str() + tr(" value:") + value.c_str();
    ERROR_LOG("Error in Bag data property received:"<<msg.toStdString());
    return;
  }

  if(key.compare("volume_roi_start_x") == 0)
  {
    m_bagdata->setvolumeROIStartX(std::stof(value));
  }
  else if(key.compare("volume_roi_start_y") == 0)
  {
    m_bagdata->setvolumeROIStartY(std::stof(value));
  }
  else if(key.compare("volume_roi_start_z") == 0)
  {
    m_bagdata->setvolumeROIStartZ(std::stof(value));
  }
  else if(key.compare("volume_roi_end_x") == 0)
  {
    m_bagdata->setvolumeROIEndX(std::stof(value));
  }
  else if(key.compare("volume_roi_end_y") == 0)
  {
    m_bagdata->setvolumeROIEndY(std::stof(value));
  }
  else if(key.compare("volume_roi_end_z") == 0)
  {
    m_bagdata->setvolumeROIEndZ(std::stof(value));
  }
  else if(key.compare("volume_width") == 0)
  {
    m_bagdata->setvolumeWidth(std::stof(value));
  }
  else if(key.compare("volume_height") == 0)
  {
    m_bagdata->setvolumeHeight(std::stof(value));
  }
  else if(key.compare("slice_count") == 0)
  {
    m_bagdata->setvolumeSliceCount(std::stof(value));
  }
  else if(key.compare("slice_format") == 0)
  {
    m_bagdata->setvolumeFormat(value.c_str());
  }
  else if(key.compare("InfoType") == 0)
  {
    m_bagdata->setBagTypeInfo(value.c_str());
  }
  else
  {
    ERROR_LOG("Unknown PropertyReceived " << key << ":" <<value);
  }
}

/*!
* @fn       destroyInstance
* @param    None
* @return   None
* @brief    destroys singleton instance of class
*
*/
void ScannerAccessInterface::destroyInstance()
{
  if(m_instance)
  {
    DEBUG_LOG("Destroying Scanner AccessInterface");
    delete m_instance;
    m_instance = NULL;
  }
}

/*!
* @fn       Start
* @param    None
* @return   Errors::RESULT
* @brief    Start receiving bag
*/
Errors::RESULT ScannerAccessInterface::Start(void)
{  
  TRACE_LOG("");

  m_eof = false;
  Errors::RESULT result = Errors::RESULT::E_FAIL;
  if(m_bag_loading)
  {
    return Errors::RESULT::E_FAIL;
  }

  m_bag_loading = true;
  m_tdrcount = 0;
  m_ctcount = 0;
  uint64_t max_part_length_ = MAX_PART_LEN;
  if (m_bagdata.get() != NULL)
  {
    analogic::nss::NssBag * livebag = m_bagdata->getLivebag();
    if(livebag != NULL)
    {
      INFO_LOG("Requesting bag " << livebag->get_transfer_bag_identifier());
      m_bagdata->setState(BagData::CONSTRUCTION);

      std::cout << "OsrSimulator::RunPullProcessThread: start: " <<
                   livebag->get_transfer_bag_identifier() << std::endl;
      if(livebag->get_server_transfer_option() == NULL)
      {
        ERROR_LOG("livebag->get_server_transfer_option() is null.");

        //        m_bagdata->addErrorMsg(tr("Server Transfer option is null."));
        errorPopup();
        livebag->Finished(analogic::nss::BASIC_COMMAND_RESULT_IO_ERROR);
        SAFE_DELETE(livebag);
        m_bagdata->setState(BagData::ERROR);
        m_bag_loading = false;
        return Errors::E_FAIL;
      }
      INFO_LOG("Bag location: " << livebag->get_server_transfer_option()->get_host() << ":" <<
                     livebag->get_server_transfer_option()->get_port());

      // Create Receiver
      if (!createReceiver(livebag->get_server_transfer_option()->get_host(),
                          livebag->get_server_transfer_option()->get_port(),
                          livebag->get_transfer_bag_identifier(), this,
                          max_part_length_))
      {
        m_bagdata->addErrorMsg(tr("Failed to create receiver. Please check scanner connection."));
        errorPopup();
        livebag->Finished(analogic::nss::BASIC_COMMAND_RESULT_IO_ERROR);
        m_bagdata->setState(BagData::ERROR);
       SAFE_DELETE(livebag);
        m_bag_loading = false;
        return Errors::E_FAIL;
      }

      // Open Receiver
      if (!openReceiver())
      {
        m_bagdata->addErrorMsg(tr("Failed to open receiver. Please check scanner connection."));
        errorPopup();
        if (m_bagdata->getLivebag())
        {
          analogic::nss::NssBag * livebag = m_bagdata->getLivebag();
          livebag->Finished(analogic::nss::BASIC_COMMAND_RESULT_IO_ERROR);
          SAFE_DELETE(livebag);
        }
        m_bagdata->setState(BagData::ERROR);
        m_bag_loading = false;
        return Errors::E_FAIL;
      }

      std::string bagid;
      bagid = livebag->get_transfer_bag_identifier();

      // Wait while receiving all data
      while((m_receiver != NULL) && m_receiver->is_open() && m_receiver->ReceiveNextPart())
      {
      }

      if ( m_bagdata.get() != NULL)
      {
        if ((!m_eof) || (!m_bagdata->isValid()))
        {
          result = Errors::E_FAIL;
          m_bag_loading = false;
          if (!m_eof)
          {
            m_bagdata->addErrorMsg(tr("Failed to receive complete bag data. Please check scanner connection."));
            ERROR_LOG("Failed to receive complete bag data. Bag timedout or scanner connection lost");
          }


          if (m_bagdata->getLivebag() != NULL)
          {
            m_bagdata->getLivebag()->NotifyBagTransferResult
                (analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_IO_ERROR);
            m_bagdata->getLivebag()->Finished(analogic::nss::BASIC_COMMAND_RESULT_IO_ERROR);
            m_bagdata->populateTDRWithError();
            m_bagdata->applyDecision(QMLEnums::ThreatEvalPanelCommand::TEPC_ERROR);
          }

          errorPopup(bagid);
          m_bagdata->setState(BagData::ERROR);
          if (m_bagdata->getLivebag() != NULL)
          {
            livebag =  m_bagdata->getLivebag();
            SAFE_DELETE(livebag);
          }
        }
        else
        {
          if (m_bagdata->getLivebag() != NULL)
          {
            m_bagdata->getLivebag()->NotifyBagTransferResult
                (analogic::nss::BasicCommandResult::BASIC_COMMAND_RESULT_SUCCESS);
            m_bagdata->cropVolumeAccordingtoROI();
            m_bagdata->readSupplementaryTDR();
            m_bagdata->refreshAttributes();
            if(m_bagdata->getArchiveSaveMode() == false)
            {
              emit notifyToChangeOrNotScreen(true, "", Errors::S_OK);
              m_bagdata->setState(BagData::READYTORENDER);
            }
            else
            {
              m_bagdata->setState(BagData::ARCHIVE_STATE);
            }

            emit bagReceived(m_bagdata, true);

            result = Errors::S_OK;
          }
        }
      }
      // Close Receiver
      closeReceiver();
      deleteReceiver();
    }
    else
    {
      ERROR_LOG("Live bag is null");
    }
  }
  else
  {
    ERROR_LOG("BagData is null");
  }
  return result;
}

/*!
* @fn       CtModuleReceived
* @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
* @param    SDICOS::CTModule*
* @return   None
* @brief    Call back for CT Module received
*/
void ScannerAccessInterface::CtModuleReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                                              SDICOS::CTModule* ct_module,
                                              const SDICOS::ErrorLog& error_log) {
  TRACE_LOG("");
  INFO_LOG("Ct Module received.");
  if ( m_bagdata.get() != NULL)
  {
    if (receiver != NULL && !error_log.HasErrors())
    {
      DEBUG_LOG("For Bag:" << receiver->get_bag_identifier());
    }
    else
    {
      QString msg = tr("Error in ctmodule received") + error_log.GetError(1).Get();
      ERROR_LOG(msg.toStdString());
      m_bagdata->addErrorMsg(msg);
    }
    if (ct_module)
    {
      DEBUG_LOG("Bag Flow Event:: CtModule received for " << m_bagdata->getBagid());
      m_bagdata->addCtModule(ct_module);

      m_ctcount++;

      SDICOS::CTTypes::CTImage::CT_PIXEL_DATA_CHARACTERISTICS pixel;
      SDICOS::CTTypes::CTImage::OOI_IMAGE_CHARACTERISTICS ooi;
      SDICOS::CTTypes::CTImage::IMAGE_FLAVOR flavor;
      SDICOS::CTTypes::CTImage::DERIVED_PIXEL_CONTRAST contrast;
      ct_module->GetCTImage().GetImageTypeCT(pixel, ooi, flavor, contrast);
      if (flavor == SDICOS::CTTypes::CTImage::enumVolume)
      {
        DEBUG_LOG("Volume CT file");
        SDICOS::TipImage* tipImage = ct_module->GetTipImage();
        if (tipImage == nullptr)
        {
          DEBUG_LOG("TipImage: null");
        }
        else
        {
          DEBUG_LOG("TipImage: allocated");
          DEBUG_LOG("TipImageType: " << tipImage->GetTipImageTypeAsString().Get());
          SDICOS::TipImage::TIP_IMAGE_TYPE tipType = tipImage->GetTipImageType();
          if (tipType == SDICOS::TipImage::enumComposite)
          {
            m_bagdata->setTIPBag(true);
          }
        }
      }
    }

  }
}

/*!
* @fn       TdrModuleReceived
* @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
* @param    SDICOS::TDRModule*
* @return   None
* @brief    Call Back for TDR Module received
*/
void ScannerAccessInterface::TdrModuleReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                                               SDICOS::TDRModule* tdr_module,
                                               const SDICOS::ErrorLog& error_log) {
  TRACE_LOG("");

  INFO_LOG("TDR Module received.");
  if ( m_bagdata.get() != NULL)
  {
    if (receiver != NULL && !error_log.HasErrors())
    {
      LOG(TRACE) << "For Bag: " << receiver->get_bag_identifier();
    }
    else
    {
      QString msg = tr("Error in tdr received") + error_log.GetError(1).Get();
      ERROR_LOG(msg.toStdString());
      m_bagdata->addErrorMsg(msg);
    }
    if (tdr_module && m_bagdata->getLivebag())
    {
      m_bagdata->addTdrModule(tdr_module);
      m_tdrcount++;
    }

    // Set debug flag to true while debugging to see contents of TDR data
    bool debug_log = false;
    if (debug_log) {

      // Read input threat module and threat objects parent inside it
      SDICOS::TDRTypes::ThreatSequence &ts = tdr_module->GetThreatSequence();
      SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects =
          ts.GetPotentialThreatObject();

      int num_threats = threatObjects.GetSize();
      int num_supplement_tdr = 0;
      int num_supplement_threats = 0;
      if (m_bagdata->getLivebag()) {
        num_supplement_tdr = m_bagdata->getLivebag()->get_supplemental_tdr_count();
        m_bagdata->getLivebag()->get_extended_property_count();
        for (int i=0;i<num_supplement_tdr;++i) {
          SDICOS::TDRModule* tdr_module_s = m_bagdata->getLivebag()->get_supplemental_tdr(i);

          // Read input threat module and threat objects preent inside it
          SDICOS::TDRTypes::ThreatSequence &ts_s = tdr_module_s->GetThreatSequence();
          SDICOS::Array1D<SDICOS::TDRTypes::ThreatSequence::ThreatItem>& threatObjects_s =
              ts_s.GetPotentialThreatObject();

          int num_threats_s = threatObjects_s.GetSize();
          num_supplement_threats += num_threats_s;
        }
      }

      INFO_LOG("BAG ID: " << m_bagdata->getBagid()
               << " machine TDR Modules Count : " << m_tdrcount
               << " threats in machine TDR : " << num_threats);
      INFO_LOG("Supplement TDR Modules Count : " << num_supplement_tdr
               << " total threats in supplement TDR " << num_supplement_threats);
    }
  }
}

/*!
* @fn       SliceDataReceived
* @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
* @param    void* buffer
* @param    size_t length
* @return   None
* @brief    Call Back for Slice data received
*
*/
void ScannerAccessInterface::SliceDataReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                                               void* buffer, size_t length) {
  TRACE_LOG("");
  if (receiver != NULL)
  {
    DEBUG_LOG("For Bag:" << receiver->get_bag_identifier());
  }
  if ( m_bagdata.get() != NULL)
  {
    int sliceCount = m_bagdata->getSliceCountReceived();
    if ( (m_bagdata.get()->getvolumeFormat().compare(VOLUME_FORMAT_JPEG) == 0))
    {
      m_bagdata.get()->addSliceDataToQueueForDecompression(buffer, length);
    }
    else
    {
      DEBUG_LOG("Uncompressed slice data received; size [" << length << "]");
      m_bagdata->setVolumeSliceData(buffer, length);
      m_bagdata->setSliceCountReceived(++sliceCount);
    }
    int totalSlices = m_bagdata->getVolumeDimension().z();
    if((sliceCount == 1) || (sliceCount == totalSlices))
    {
      DEBUG_LOG("Slice data received; SliceNo: "<<sliceCount<< " size [" << length << "]");
    }
    else
    {
      DEBUG_LOG("Slice data received; SliceNo: "<<sliceCount<< " size [" << length << "]");
    }

  }
}

/*!
* @fn       ErrorReceived
* @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
* @param    std::string& error
* @return   None
* @brief    Call Back for Error received
*
*/
void ScannerAccessInterface::ErrorReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver,
                                           const std::string& error) {
  Q_UNUSED(receiver);
  QString msg = tr("Error received. Error: ") + error.c_str();

  if(m_bagdata!=NULL)
  {
    m_bagdata->addErrorMsg(msg);
    errorPopup();
    INFO_LOG("sending bag finished with error");
    analogic::nss::NssBag * livebag = m_bagdata->getLivebag();
    if (livebag)
    {
      livebag->NotifyBagTransferResult
          (analogic::nss::BASIC_COMMAND_RESULT_IO_ERROR);
      livebag->Finished(analogic::nss::BASIC_COMMAND_RESULT_IO_ERROR);
      SAFE_DELETE(livebag);
      m_bagdata->setState(BagData::ERROR);
    }

  }
  ERROR_LOG( msg.toStdString().c_str());
}

/*!
* @fn       EndOfTransferReceived
* @param    analogic::streaming_xfer::AlogStreamingBagReceiver*
* @return   None
* @brief    Call Back for End of transmission received
*
*/
void ScannerAccessInterface::EndOfTransferReceived(analogic::streaming_xfer::AlogStreamingBagReceiver* receiver)
{
  TRACE_LOG("");
  if (receiver != NULL)
  {
    INFO_LOG("For Bag: " << receiver->get_bag_identifier());
  }

  if ( m_bagdata.get() != NULL)
  {
    INFO_LOG("Bag Flow Event:: EndOfTransferReceived received for " << m_bagdata->getBagid());
    while (!m_bagdata.get()->isSliceQueueEmpty() &&
           m_bagdata->getSliceProcessed() != m_bagdata->getVolumeDimension().z())
    {
      DEBUG_LOG("Waiting for Data processing threads to complete."
                <<" Slice processed : "<<m_bagdata->getSliceProcessed()
                <<" Slice Received : "<<m_bagdata->getSliceCountReceived()
                <<" Total slices : " <<m_bagdata->getVolumeDimension().z());
      usleep(10 * 1000);  // sleep for 10ms
    }
    INFO_LOG("Slice processed : "<<m_bagdata->getSliceProcessed()
             << " Slice Received : "<<m_bagdata->getSliceCountReceived()
             << " Total slices : " <<m_bagdata->getVolumeDimension().z());
    m_eof = true;
    //changing bag properties volume format to uncompressed
    m_bagdata.get()->setvolumeFormat(VOLUME_FORMAT_UNCOMPRESSED);
  }
}



/*!
* @fn       ~ScannerAccessInterface
* @param    None
* @return   None
* @brief    Destructor for class ScannerAccessInterface.
*/
ScannerAccessInterface::~ScannerAccessInterface()
{
  m_bagdata.reset();
}

/*!
* @fn       createReceiver
* @param    const std::string& host
* @param    uint16_t port
* @param    const std::string& bag_identifier
* @param    AlogStreamingBagReceiverListener* listener
* @param    uint64_t max_part_length
* @return   bool
* @brief    creates receiver
*
*/
bool ScannerAccessInterface::createReceiver(const std::string &host,
                                            uint16_t port, const std::string &bag_identifier,
                                            streaming_xfer::AlogStreamingBagReceiverListener *listener,
                                            uint64_t max_part_length)
{
  DEBUG_LOG("Create receiver. BagId:" <<bag_identifier << " host" << host << " port"  << port);
  analogic::streaming_xfer::AlogStreamingBagReceiverConfig config;
  config.max_part_length = max_part_length;
  config.ssl_pkcs12_path = "certificate/alogTransfer.p12";
  config.ssl_pkcs12_password = "PhOeNiX";
  config.read_timeout = WorkstationConfig::getInstance()->getBagTransferTimeoutInSec();

  m_receiver = new analogic::streaming_xfer::AlogStreamingBagReceiver(
        config, host, port, bag_identifier, listener);
  if(m_receiver == NULL)
  {
    ERROR_LOG("Unable to create analogic::streaming_xfer::AlogStreamingBagReceiver");
    ERROR_LOG("Cert File : "<< config.ssl_pkcs12_path);
    ERROR_LOG("Part Length : "<< config.max_part_length);
    ERROR_LOG("Bag Timeout : "<< config.read_timeout);
    return false;
  }
  {
    INFO_LOG("ScannerAccessInterface::createReceiver : Success : " << m_receiver->get_last_error());
    INFO_LOG("Cert File : "<< config.ssl_pkcs12_path);
    INFO_LOG("Part Length : "<< config.max_part_length);
    INFO_LOG("Bag Timeout : "<< config.read_timeout);
  }
  return true;
}

/*!
* @fn       openReceiver
* @param    None
* @return   bool
* @brief    opens receiver
*
*/
bool ScannerAccessInterface::openReceiver()
{
  bool return_val = false;
  if (m_receiver)
  {
    return_val = m_receiver->Open();
    if(return_val != true){
      m_bag_loading = false;
      ERROR_LOG("Failed to open receiver connection. Value returned: " << m_receiver->get_last_error());
      SAFE_DELETE( m_receiver);
      return false;
    }
  }
  else
  {
    return false;
  }
  return true;
}

/*!
* @fn       deleteReceiver
* @param    None
* @return   bool
* @brief    deletes receiver
*
*/
void ScannerAccessInterface::deleteReceiver()
{
  TRACE_LOG("");
  SAFE_DELETE( m_receiver);
}
}  // end of namespace ws
}  // end of namespace analogic

