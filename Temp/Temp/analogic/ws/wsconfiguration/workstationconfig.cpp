/*!
* @file     workstationconfig.cpp
* @author   Agiliad
* @brief    This file contains interface, responsible for managing UI configurations of system.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/wsconfiguration/workstationconfig.h>


//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
QMap<QString, QVariant> WorkstationConfig::m_WsConfigMap;
WorkstationConfig *WorkstationConfig::m_workstationConfig;



/*!
* @fn       WorkstationConfig
* @param    None
* @return   None
* @brief    Constructor for class WorkstationConfig.
*/
WorkstationConfig::WorkstationConfig()
{
  TRACE_LOG("");
  QString st =  QDir::currentPath();
  setexecutablePath(st.toStdString());
  m_workstationVersion = "TBD";
  m_OSVersion = "TBD";
#ifdef WORKSTATION
  setConfigInifilename(QString(getexecutablePath().c_str()) + "/" + INI_FILE_NAME);
  initIni();
  setConfigxmlfilename(CONFIG_XML_FILE_NAME);
  setConfigxsdfilename(CONFIG_XSD_FILE_NAME);
  m_tipHITMsg = tr("You have correctly identified a fictional 'XXX' threat.<br />Press 'BTN' to remove this Message.<br />Check the bag to make sure that no more threats are present.");
  m_tipMISSMsg = tr("You did not correctly identify a fictional 'XXX' threat.<br />Press 'BTN' to remove this Message.<br />Check the bag to make sure that no more threats are present.");
  m_tipNonTIPMsg = tr("You are not being tested. No fictional threat was projected.<br />Please follow the airport security screening procedures to make sure that there are no threats in the bag.");
#endif
#ifdef RERUN
  setConfigInifilename(QString(getexecutablePath().c_str()) + "/" + RERUN_INI_FILE_NAME);
  initIni();
  QString workstationType = getWorkstationName();
  if(workstationType == "Rerun Emulator")
  {
    setConfigxmlfilename(EMULATOR_CONFIG_XML_FILE_NAME);
    setConfigxsdfilename(EMULATOR_CONFIG_XSD_FILE_NAME);
  }
  if(workstationType == "Training Workstation")
  {
    setConfigxmlfilename(SIMULATOR_CONFIG_XML_FILE_NAME);
    setConfigxsdfilename(SIMULATOR_CONFIG_XSD_FILE_NAME);
  }
#endif
  initXml();
}

/*!
* @fn       ~WorkstationConfig
* @param    None
* @return   None
* @brief    Destructor for class WorkstationConfig.
*/
WorkstationConfig::~WorkstationConfig()
{
}

/*!
* @fn       getAuthenticateWithNss
* @param    None
* @return   bool
* @brief    return SDSWCLient needs to be authenticated with NSS.
*/
bool WorkstationConfig::getAuthenticateWithNss() const
{
  return m_authenticateWithNss;
}

/*!
* @fn       getInstance
* @param    None
* @return   WorkstationConfig
* @brief    returns Instance of WorkstationConfig
*/
WorkstationConfig *WorkstationConfig::getInstance()
{
  if(m_workstationConfig == nullptr)
  {
    m_workstationConfig = new WorkstationConfig();
    THROW_IF_FAILED((m_workstationConfig == nullptr) ? Errors::E_OUTOFMEMORY : Errors::S_OK);
  }
  return m_workstationConfig;
}

/*!
* @fn       initXml
* @param    None
* @return   None
* @brief    initialization for Xml configuration.
*/
void WorkstationConfig::initXml()
{
  TRACE_LOG("");
  DEBUG_LOG("Initializing configuration from xml");
  QString error;
  if (!validateAndParseXml(error, m_configxmlfilename, m_configxsdfilename))
  {
    ERROR_LOG("Config xml file parsing failed. Xml File:" <<
              m_configxmlfilename.toStdString().c_str() << " Xsd File:" <<
              m_configxsdfilename.toStdString().c_str() <<
              "Error:" << error.toStdString().c_str());
  }
}

/*!
* @fn       initIni
* @param    None
* @return   None
* @brief    initialization for ini configuration.
*/
void WorkstationConfig::initIni()
{
  TRACE_LOG("");
  DEBUG_LOG("Initializing configuration from ini");
  QString error;
  if (!validateAndParseIni(m_configinifilename.toStdString()))
  {
    ERROR_LOG("Config ini file parsing failed. ini File:" <<
              m_configinifilename.toStdString().c_str()<<
              "Error:" << error.toStdString().c_str());
  }
  m_workstationVersion = workstationVersion();
  m_OSVersion = OSVersion();
}

/*!
* @fn       validateAndParseIni
* @param    const QString - ini file
* @return   bool
* @brief    parse ini file
*/
bool WorkstationConfig::validateAndParseIni(const std::string ini_file)
{
  //==========================================================================================
  DEBUG_LOG("Initializing configuration from ini");
  if (ini_file.length() > 0 && boost::filesystem::exists(ini_file))
  {
    DEBUG_LOG("Reading configuration from ini file: " << ini_file);
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);
#ifdef WORKSTATION

    try
    {
      int timeout = pt.get<int>("ScannerAdminServer.AsyncTaskRefreshInterval");
      m_scanneradminasynctaskrefreshinterval = timeout;
    }
    catch (...)
    {
      m_scanneradminasynctaskrefreshinterval = DEFAULT_TASK_TIMEOUT;
    }
    DEBUG_LOG("Setting scanner admin service async task refresh inteval: "
              << m_scanneradminasynctaskrefreshinterval);

    try
    {
      int timeout = pt.get<int>("ScannerAdminServer.AsyncTaskTimeoutCheckInterval");
      m_scanneradminasynctasktimeoutinterval = timeout;
    }
    catch (...)
    {
      m_scanneradminasynctasktimeoutinterval = DEFAULT_TASK_TIMEOUT;
    }
    DEBUG_LOG("Setting scanner admin service async task timeout inteval: "
              << m_scanneradminasynctasktimeoutinterval);
    try
    {
      bool sslauth = pt.get<bool>("ScannerAdminServer.SSLAuth");
      m_strsslauth = sslauth;
    }
    catch (...)
    {
      m_strsslauth = DEFAULT_SSL_AUTH;
    }
    DEBUG_LOG("Setting scanner admin service ssl auth : "
              << m_strsslauth);
    try
    {
      std::string ServerPassPhrase = pt.get<std::string>("ScannerAdminServer.ServerPassPhrase");
      m_strserverpassphrase = ServerPassPhrase.c_str();
    }
    catch (...)
    {
      m_strserverpassphrase = DEFAULT_SERVER_PASS_PHRASE;
    }
    DEBUG_LOG("Setting scanner admin service pass phrase");
    try
    {
      std::string ServerCert = pt.get<std::string>("ScannerAdminServer.ServerCert");
      m_strservercertificate = ServerCert.c_str();
    }
    catch (...)
    {
      m_strservercertificate = SERVER_CERTIFICATE;
    }
    DEBUG_LOG("Setting scanner admin service certificate : "
              << m_strservercertificate.toStdString());
    try
    {
      std::string NssServerCert = pt.get<std::string>("ScannerAdminServer.NssServerCert");
      m_strnssservercertificate = NssServerCert.c_str();
    }
    catch (...)
    {
      m_strnssservercertificate = SERVER_CERTIFICATE_NSS;
    }
    DEBUG_LOG("Setting nss certificate : "
              << m_strnssservercertificate.toStdString());

    try
    {
      int bagTimeout = pt.get<int>("OsrConfiguration.BagTransferTimeoutInSec");
      m_bagTransferTimeoutInSec = bagTimeout;
    }
    catch (...)
    {
      m_bagTransferTimeoutInSec = DEFAULT_BAG_TRANSFER_TIMEOUT_IN_SEC;
    }
    DEBUG_LOG("Setting Bag Transfer Timeout to: "
              << m_bagTransferTimeoutInSec);
#endif
    try
    {
      std::string ls = pt.get<std::string>("Compress.Unzip");
      m_strunzipcommand  = ls.c_str();
    }
    catch (...)
    {
      m_strunzipcommand = DEFAULT_UNZIP;
    }
    DEBUG_LOG("Setting unzip command: " << m_strunzipcommand.toStdString());

    try
    {
      std::string ls = pt.get<std::string>("NSSServer.NetworkCheckInterval");
      m_nssCheckInterval  = QString::fromStdString(ls).toInt();
    }
    catch (...)
    {
      m_nssCheckInterval = DEFAULT_TIMEOUT;
    }
    DEBUG_LOG("Setting time interval of checking nss server to: " << m_nssCheckInterval);

    try
    {
      std::string ln = pt.get<std::string>("Workstation.Name");
      m_workstationName = ln.c_str();
    }
    catch (...)
    {
      m_workstationName = DEFAULT_WORKSTATION_NAME;
    }
    DEBUG_LOG("Setting workstation to: " << m_workstationName.toStdString());
    try
    {
      std::string ls = pt.get<std::string>("Scanner.ScannerConnectionCheckInterval");
      m_scannerCheckInterval  = QString::fromStdString(ls).toInt();
    }
    catch (...)
    {
      m_scannerCheckInterval = DEFAULT_TIMEOUT;
    }
    DEBUG_LOG("Setting time interval of checking scanner connection to: " << m_scannerCheckInterval);

    try
    {
      std::string ls = pt.get<std::string>("ScannerAdminServer.NetworkReconnectInterval");
      m_networkreconnecttimer  = QString::fromStdString(ls).toInt();
    }
    catch (...)
    {
      m_networkreconnecttimer = DEFAULT_NETWORK_RECONNECT;
      WARN_LOG("Network Reconnect interval is not defined in ini. Using default.");
    }
    DEBUG_LOG("Setting Network Reconnect interval to: " << m_networkreconnecttimer);

    try
    {
      std::string ls = pt.get<std::string>("ScannerAdminServer.NetworkTimeout");
      m_networktimeout  = QString::fromStdString(ls).toInt();
    }
    catch (...)
    {
      m_networktimeout = DEFAULT_NETWORK_TIMEOUT;
      WARN_LOG("Network Timeout is not defined in ini. Using default.");
    }
    DEBUG_LOG("Setting Network Timeout to: " << m_networktimeout);

    //--------------
    try
    {
      bool use_shading_formain = pt.get<bool>("RenderParameter.TranslucentPanelShading");
      m_useshadingformainviewer = use_shading_formain;
    }
    catch (...)
    {
      m_useshadingformainviewer = DEFAULT_RENDERPARAMETER_TRANSLUCENTSHADING;
    }
    DEBUG_LOG("Setting RenderParameter.UseShadingForMainViewer: " << m_useshadingformainviewer);
    try
    {
      bool use_shading_forright = pt.get<bool>("RenderParameter.SurfacePanelShading");
      m_useshadingforrightviewer = use_shading_forright;
    }
    catch (...)
    {
      m_useshadingforrightviewer = DEFAULT_RENDERPARAMETER_SURFACESHADING;
    }
    DEBUG_LOG("Setting RenderParameter.UseShadingForRighViewer: " << m_useshadingforrightviewer);
    //----------
    // Interpolation for Left Viewer
    try
    {
      bool translucent_interpolation_linear = pt.get<bool>("RenderParameter.TranslucentInterpolationLinear");
      m_traslucentInterpolationLinear = translucent_interpolation_linear;
    }
    catch (...)
    {
      m_traslucentInterpolationLinear = DEFAULT_RENDERPARAMETER_TRANSLUCENT_INTERP_LINEAR;
    }
    DEBUG_LOG("Setting RenderParameter.TranslucentInterpolationLinear: " << m_traslucentInterpolationLinear);
    //----------
    //----------
    // Interpolation for Right Viewer
    try
    {
      bool surface_interpolation_linear = pt.get<bool>("RenderParameter.SurfaceInterpolationLinear");
      m_surfaceInterpolationLinear = surface_interpolation_linear;
    }
    catch (...)
    {
      m_surfaceInterpolationLinear = DEFAULT_RENDERPARAMETER_SURFACE_INTERP_LINEAR;
    }
    DEBUG_LOG("Setting RenderParameter.SurfaceInterpolationLinear: " << m_surfaceInterpolationLinear);
    //----------
    //----------
    // Surface Organic removable
    try
    {
      bool surface_organic_removable = pt.get<bool>("RenderParameter.SurfaceOrganicRemovable");
      m_surfaceOrganicRemovable = surface_organic_removable;
    }
    catch (...)
    {
      m_surfaceOrganicRemovable = DEFAULT_RENDERPARAMETER_SURFACE_ORGANICREMOVAL;
    }
    DEBUG_LOG("Setting RenderParameter.SurfaceOrganicRemovable: " << m_surfaceOrganicRemovable);
    //----------
    //----------
    // Surface Inorganic removable
    try
    {
      bool surface_inorganic_removable = pt.get<bool>("RenderParameter.SurfaceInorganicRemovable");
      m_surfaceInorganicRemovable = surface_inorganic_removable;
    }
    catch (...)
    {
      m_surfaceInorganicRemovable = DEFAULT_RENDERPARAMETER_SURFACE_INORGANICREMOVAL;
    }
    DEBUG_LOG("Setting RenderParameter.SurfaceInorganicRemovable: " << m_surfaceInorganicRemovable);
    //----------
    //----------
    // Surface Metal removable
    try
    {
      bool surface_metal_removable = pt.get<bool>("RenderParameter.SurfaceMetalRemovable");
      m_surfaceMetalRemovable = surface_metal_removable;
    }
    catch (...)
    {
      m_surfaceMetalRemovable = DEFAULT_RENDERPARAMETER_SURFACE_METALREMOVAL;
    }
    DEBUG_LOG("Setting RenderParameter.SurfaceMetalRemovable: " << m_surfaceMetalRemovable);
    //----------
    //----------
    // Translucent Organic Removable
    try
    {
      bool translucent_organic_removable = pt.get<bool>("RenderParameter.TranslucentOrganicRemovable");
      m_translucentOrganicRemovable = translucent_organic_removable;
    }
    catch (...)
    {
      m_translucentOrganicRemovable = DEFAULT_RENDERPARAMETER_TRANSLUCENT_ORGANICREMOVAL;
    }
    DEBUG_LOG("Setting RenderParameter.TranslucentOrganicRemovable: " << m_translucentOrganicRemovable);
    //----------
    //----------
    // Translucent Inorganic Removable
    try
    {
      bool translucent_inorganic_removable = pt.get<bool>("RenderParameter.TranslucentInorganicRemovable");
      m_translucentInorganicRemovable = translucent_inorganic_removable;
    }
    catch (...)
    {
      m_translucentInorganicRemovable = DEFAULT_RENDERPARAMETER_TRANSLUCENT_INORGANICREMOVAL;
    }
    DEBUG_LOG("Setting RenderParameter.TranslucentInorganicRemovable: " << m_translucentInorganicRemovable);
    //----------
    //----------
    // Translucent Inorganic Removable
    try
    {
      bool translucent_metal_removable = pt.get<bool>("RenderParameter.TranslucentMetalRemovable");
      m_translucentMetalRemovable = translucent_metal_removable;
    }
    catch (...)
    {
      m_translucentMetalRemovable = DEFAULT_RENDERPARAMETER_TRANSLUCENT_METALREMOVAL;
    }
    DEBUG_LOG("Setting RenderParameter.TranslucentMetalRemovable: " << m_translucentMetalRemovable);
    //----------
    //----------
    // Translucent Ambient Lighting
    try
    {
      double translucent_ambient_lighting = pt.get<double>("RenderParameter.TranslucentAmbientLighting");
      m_translucentAmbientLighting = translucent_ambient_lighting;
    }
    catch (...)
    {
      m_translucentAmbientLighting = DEFAULT_RENDERPARAMETER_TRANSLUCENT_AMBIENT_LIGHTING;
    }
    DEBUG_LOG("Setting RenderParameter.TranslucentAmbientLighting: " << m_translucentAmbientLighting);
    //----------
    //----------
    // Translucent Diffuse Lighting
    try
    {
      double translucent_diffuse_lighting = pt.get<double>("RenderParameter.TranslucentDiffuseLighting");
      m_translucentDiffuseLighting = translucent_diffuse_lighting;
    }
    catch (...)
    {
      m_translucentDiffuseLighting = DEFAULT_RENDERPARAMETER_TRANSLUCENT_DIFFUSE_LIGHTING;
    }
    DEBUG_LOG("Setting RenderParameter.TranslucentDiffuseLighting: " << m_translucentDiffuseLighting);
    //----------
    //----------
    // Translucent Specular Lighting
    try
    {
      double translucent_specular_lighting = pt.get<double>("RenderParameter.TranslucentSpecularLighting");
      m_translucentSpecularLighting = translucent_specular_lighting;
    }
    catch (...)
    {
      m_translucentSpecularLighting = DEFAULT_RENDERPARAMETER_TRANSLUCENT_SPECULAR_LIGHTING;
    }
    DEBUG_LOG("Setting RenderParameter.TranslucentSpecularLighting: " << m_translucentSpecularLighting);
    //----------
    //----------
    // Surface Ambient Lighting
    try
    {
      double surface_ambient_lighting = pt.get<double>("RenderParameter.SurfaceAmbientLighting");
      m_surfaceAmbientLighting = surface_ambient_lighting;
    }
    catch (...)
    {
      m_surfaceAmbientLighting = DEFAULT_RENDERPARAMETER_SURFACE_AMBIENT_LIGHTING;
    }
    DEBUG_LOG("Setting RenderParameter.SurfaceAmbientLighting: " << m_surfaceAmbientLighting);
    //----------
    //----------
    // Surface diffuse Lighting
    try
    {
      double surface_diffuse_lighting = pt.get<double>("RenderParameter.SurfaceDiffuseLighting");
      m_surfaceDiffuseLighting = surface_diffuse_lighting;
    }
    catch (...)
    {
      m_surfaceDiffuseLighting = DEFAULT_RENDERPARAMETER_SURFACE_DIFFUSE_LIGHTING;
    }
    DEBUG_LOG("Setting RenderParameter.SurfaceDiffuseLighting: " << m_surfaceDiffuseLighting);
    //----------
    //----------
    // Surface Specular Lighting
    try
    {
      double surface_specular_lighting = pt.get<double>("RenderParameter.SurfaceSpecularLighting");
      m_surfaceSpecularLighting = surface_specular_lighting;
    }
    catch (...)
    {
      m_surfaceSpecularLighting = DEFAULT_RENDERPARAMETER_SURFACE_SPECULAR_LIGHTING;
    }
    DEBUG_LOG("Setting RenderParameter.SurfaceSpecularLighting: " << m_surfaceSpecularLighting);
    //----------
    //----------
    // Volume Measurement start min value
    try
    {
      uint16_t vol_tool_min_val = pt.get<uint16_t>("RenderParameter.VOLUME_TOOL_MIN");
      m_volumeMeasuremtStartMin = vol_tool_min_val;
    }
    catch (...)
    {
      m_volumeMeasuremtStartMin = DEFAULT_RENDERPARAMETER_VOLUME_TOOL_MIN;
    }
    DEBUG_LOG("Setting RenderParameter.VOLUME_TOOL_MIN: " << m_volumeMeasuremtStartMin);
    //----------
    //----------
    // Volume Measurement start max value
    try
    {
      uint16_t vol_tool_max_val = pt.get<uint16_t>("RenderParameter.VOLUME_TOOL_MAX");
      m_volumeMeasuremtStartMax = vol_tool_max_val;
    }
    catch (...)
    {
      m_volumeMeasuremtStartMax = DEFAULT_RENDERPARAMETER_VOLUME_TOOL_MAX;
    }
    DEBUG_LOG("Setting RenderParameter.VOLUME_TOOL_MAX: " << m_volumeMeasuremtStartMax);
    //----------
    //----------
    try
    {
      m_authenticateWithNss = pt.get<bool>("SDSWClient.AuthenticateWithNSS");
    }
    catch (...)
    {
      m_authenticateWithNss = DEFAULT_AUTHENTICATE_WITH_NSS;
    }
    DEBUG_LOG("Setting SDSWClient Authentication with nss user: " << m_authenticateWithNss);
    try
    {
      std::string path = pt.get<std::string>("SDSWClient.Path");
      m_serviceclientbinarypath = path.c_str();
    }
    catch (...)
    {
      m_serviceclientbinarypath = DEFAULT_SDSW_CLIENT_PATH;
    }
    DEBUG_LOG("Setting SDSWClient.Path: " <<m_serviceclientbinarypath.toStdString());

    try
    {
      std::string ls = pt.get<std::string>("ImageQuality.BagFileFormat");
      m_iqBagFileFormat  = ls.c_str();
    }
    catch (...)
    {
      m_iqBagFileFormat = DEFAULT_IQBAG_FILEFORMAT;
    }
    DEBUG_LOG("Setting IQ BagFileFormat to: " << m_iqBagFileFormat.toStdString());

    try
    {
      std::string ls = pt.get<std::string>("ImageQuality.ReportFileFormat");
      m_iqReportFileFormat  = ls.c_str();
    }
    catch (...)
    {
      m_iqReportFileFormat = DEFAULT_IQREPORT_FILEFORMAT;
    }
    DEBUG_LOG("Setting IQ ReportFileFormat to: " << m_iqReportFileFormat.toStdString());

    try
    {
      std::string ls = pt.get<std::string>("ImageQuality.NistFileFormat");
      m_iqNistFileFormat  = ls.c_str();
    }
    catch (...)
    {
      m_iqNistFileFormat = DEFAULT_IQNIST_FILEFORMAT;
    }
    DEBUG_LOG("Setting IQ NistFileFormat to: " << m_iqNistFileFormat.toStdString());

    try
    {
      float_t ls = pt.get<float_t>("BagData.width");
      m_bagdataresolution.setWidth(ls);
    }
    catch (...)
    {
      m_bagdataresolution.setWidth(DEFAULT_BAG_WIDTH);
    }
    DEBUG_LOG("Setting Bag Resolution width to: " << m_bagdataresolution.width());

    try
    {
      float_t ls = pt.get<float_t>("BagData.height");
      m_bagdataresolution.setHeight(ls);
    }
    catch (...)
    {
      m_bagdataresolution.setHeight(DEFAULT_BAG_HEIGHT);
    }
    DEBUG_LOG("Setting Bag Resolution width to: " << m_bagdataresolution.height());

    try
    {
      bool ls = pt.get<bool>("BagData.EnableThreatHandling");
      m_enableThreatHandling = ls;
    }
    catch (...)
    {
      m_enableThreatHandling = DEFAULT_RENDERPARAMETER_ENABLE_THREAT_HANDLING;
    }
    DEBUG_LOG("Setting Enable Threat Handling to: " << m_enableThreatHandling);

    try
    {
      int ls = pt.get<int>("BagData.NoofBagPreProcessingThreads");
      m_NoofBagPreprocessingThreads = ls;
    }
    catch (...)
    {
      m_NoofBagPreprocessingThreads = DEFAULT_BAG_PRE_PROCESSING_THREAD;
    }
    DEBUG_LOG("Setting No of bag pre processing threads: " << m_NoofBagPreprocessingThreads);

    try
    {
      int ls = pt.get<int>("BagData.MinSlicesNeededToAllowBagClearing");
      if ((ls > 0) && (ls < 1001))
      {
        m_minimumRequiredSliceCount = ls;
      }
      else
      {
        ERROR_LOG("MinSlicesNeededToAllowBagClearing value out of range (1 to 1000),"
                  " Setting defaults " << DEFAULT_MINIMUM_REQUIRED_SLICE_COUNT);
        m_minimumRequiredSliceCount = DEFAULT_MINIMUM_REQUIRED_SLICE_COUNT;
      }
    }
    catch (...)
    {
      m_minimumRequiredSliceCount = DEFAULT_MINIMUM_REQUIRED_SLICE_COUNT;
    }
    DEBUG_LOG("Setting minimum sclice count required: " << m_minimumRequiredSliceCount);

    try
    {
      int ls = pt.get<int>("BagData.max_slices");
      if(ls > 0)
      {
        m_max_slices = ls;
      }
      else
      {
        ERROR_LOG("m_max_slices set to 1000");
        m_max_slices = 1000;
      }
    }
    catch (...)
    {
      m_max_slices = 1000;
      ERROR_LOG("Caught an exception and m_max_slices set to 1000");
    }
    INFO_LOG("Setting m_max_slices : " << m_max_slices);

    try
    {
      std::string ls = pt.get<std::string>("SoftwareInformation.Path");
      m_infoFilePath = ls.c_str();
    }
    catch(...)
    {
      m_infoFilePath = DEFAULT_INFO_FILEPATH;
    }
    DEBUG_LOG("Setting Information PDF path: " << m_infoFilePath.toStdString());
    try
    {
            std::string versionPath = (getWorkstationNameEnum() == QMLEnums::WSType::TRAINING_WORKSTATION )?("Version.SimulatorVersionPath"):("Version.WorkstationVersionPath");
            std::string ls = pt.get<std::string>(versionPath);
      m_WSVersionFilePath = ls.c_str();
    }
    catch(...)
    {
      m_WSVersionFilePath = DEFAULT_WSVERSION_FILEPATH;
    }
    DEBUG_LOG("Setting Workstation Version path: " << m_WSVersionFilePath.toStdString());
    try
    {
      std::string ls = pt.get<std::string>("Version.OsVersionPath");
      m_OSVersionFilePath = ls.c_str();
    }
    catch(...)
    {
      m_OSVersionFilePath = DEFAULT_OSVERSION_FILEPATH;
    }
    DEBUG_LOG("Setting OS Version path: " << m_OSVersionFilePath.toStdString());
    try
    {
      std::string ls = pt.get<std::string>("Storage.MountPath");
      m_mountPath = ls.c_str();
    }
    catch(...)
    {
      m_mountPath = DEFAULT_MOUNTPATH;
    }
    DEBUG_LOG("Setting Mount path: " << m_mountPath);
    try
    {
      std::string ls = pt.get<std::string>("Storage.DefaultDataPath");
      m_defaultdatapath = ls.c_str();
    }
    catch(...)
    {
      m_defaultdatapath = DEFAULT_DATA_PATH;
    }
    DEBUG_LOG("Setting Default Mount path: " << m_defaultdatapath);
    try
    {
      std::string ln = pt.get<std::string>("Logo.LogoPath");
      m_workstationLogoPath = ln.c_str();
    }
    catch (...)
    {
      m_workstationLogoPath = DEFAULT_LOGOPATH;
    }
    DEBUG_LOG("Setting Logo Path to: " << m_workstationLogoPath.toStdString());
    try
    {
      std::string ln = pt.get<std::string>("Language.CurrentLanguage");
      m_currentLanguage = ln.c_str();
    }
    catch (...)
    {
      m_currentLanguage = DEFAULT_LANGUAGE;
    }
    DEBUG_LOG("Setting Current Language: " << m_currentLanguage.toStdString());
    try
    {
      bool ssiBannerFlag = pt.get<bool>("ShowSSIBanner.EnableSSIBanner");
      m_ssibanner = ssiBannerFlag;
    }
    catch (...)
    {
      m_ssibanner = DEFAULT_SSI_BANNER;
    }
    DEBUG_LOG("Setting SSI Banner Enablity to: " << m_ssibanner);

  }
  return true;
}

void WorkstationConfig::log_params()
{
  INFO_LOG("m_max_slices: " << m_max_slices);
  INFO_LOG("Bag Resolution width: " << m_bagdataresolution.width());
  INFO_LOG("Bag Resolution height: " << m_bagdataresolution.height());
}

/*!
* @fn       workstationVersion
* @param    None
* @return   QString
* @brief    get workstationVersion
*/
QString WorkstationConfig::workstationVersion()
{
  QString value;
  QFile file(m_WSVersionFilePath);
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      value = line;
    }
    file.close();
  }
  else
  {
    ERROR_LOG(m_WSVersionFilePath.toStdString().c_str() << " failed to open.");
  }
  return value;
}

/*!
* @fn       OSVersion
* @param    None
* @return   QString
* @brief    get OSVersionNumber
*/
QString WorkstationConfig::OSVersion()
{
  QString version="TBD";
  QString value;
  QFile file(m_OSVersionFilePath);
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      value = line;
      INFO_LOG("OS Version file read as : "<<value.toStdString().c_str()); 
      break;     
    }
    file.close();
  }
  else
  {
    ERROR_LOG(m_OSVersionFilePath.toStdString().c_str() << " failed to open.");
  }

  QStringList list = value.split("=");
  if(list.size() >= 2){
    version = list.at(1);
    INFO_LOG("OS Version found out as : "<<version.toStdString().c_str()); 
  }
  else{
    INFO_LOG("OS Version NOT Found : "<<value.toStdString().c_str()); 
  }
  return version;
}

/*!
* @fn       validateAndParseXml
* @param    const QString - xml file
* @param    const QString - xsd file
* @return   bool
* @brief    parse xml file
*/
bool WorkstationConfig::validateAndParseXml(QString& error, const QString xmlfile, const QString xsdfile)
{
  DEBUG_LOG("Parsing configuration from xml");
  RETURN_IF_FAILED((xmlfile.length() > 0)?Errors::S_OK:Errors::E_FAIL);
  RETURN_IF_FAILED((xsdfile.length() > 0)?Errors::S_OK:Errors::E_FAIL);

  QString configXMLData;
  QString configXSDData;
  QVariantList list;

  if (!(XmlService::getXmlAndXsdContent(xmlfile, xsdfile, configXMLData, configXSDData, error)))
  {
    return false;
  }
  else
  {
    if (!(XmlService::validateAndParseXml(configXMLData, configXSDData, list, error)))
    {
      return false;
    }
    else
    {
      for (int i = 0;  i < list.count(); i++)
      {
        ConfigBaseElement config = qvariant_cast<ConfigBaseElement>(list.at(i));
        updateWorkstationConfig(config);
      }
      return true;
    }
  }
  return false;
}
#ifdef WORKSTATION
/*!
* @fn       getscanneradminseveripaddress
* @param    None
* @return   string- ipadress
* @brief    gets scanner admin sever ip address.
*/
QString WorkstationConfig::getscanneradminseveripaddress()
{
  QString ipAddess = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("scanner_admin_server_ip")));
  if (ipAddess.length() < 1)
  {
    ipAddess = DEFAULT_IP_ADDRESS;
  }
  return ipAddess;
}

/*!
* @fn       getscanneradminserverportno
* @param    None
* @return   QString- portno
* @brief    gets scanner admin server portno.
*/
QString WorkstationConfig::getscanneradminserverportno()
{
  QString portNo = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("scanner_admin_server_port")));
  if (portNo.length() < 1)
  {
    portNo = DEFAULT_PORT;
  }
  return portNo;
}

/*!
* @fn       getpassphrase
* @param    None
* @return   QString- password
* @brief    gets pass phrase which will use to decrypt certificate.
*/
QString WorkstationConfig::getpassphrase()
{
  QString strpassphrase = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("pass_phrase")));
  if (strpassphrase.length() < 1)
  {
    strpassphrase = DEFAULT_PASS_PHRASE;
  }
  return strpassphrase;
}
/*!
* @fn       getsslauth
* @param    None
* @return   bool- auth
* @brief    gets server ssl auth */
bool WorkstationConfig::getsslauth()
{
  return m_strsslauth;
}
/*!
* @fn       getservercertificate
* @param    None
* @return   string- certifcate
* @brief    gets server certificate
*/
QString WorkstationConfig::getservercertificate()
{
  return m_strservercertificate;
}
/*!
* @fn       getnssservercertificate
* @param    None
* @return   string- certifcate
* @brief    gets nss server certificate
*/
QString WorkstationConfig::getnssservercertificate()
{
  return m_strnssservercertificate;
}
/*!
* @fn       getserverpassphrase
* @param    None
* @return   string- password
* @brief    gets pass phrase which will use to decrypt certificate.
*/
QString WorkstationConfig::getserverpassphrase()
{
  return m_strserverpassphrase;
}

#endif
/*!
* @fn       getinactivitytimeout
* @param    None
* @return   int - timeout
* @brief    time out in case on keyboard or mouse inactivity
*/
int WorkstationConfig::getinactivitytimeout()
{
  int inactivityTimeout = qvariant_cast<int>(m_WsConfigMap.value(QString::fromStdString("inactivity_timeout")));
  if (inactivityTimeout == 0)
  {
    inactivityTimeout = DEFAULT_TIMEOUT;
  }
  return inactivityTimeout;
}



/*!
* @fn       getNssipaddress
* @param    None
* @return   QString - ip
* @brief    config for nss server
*/
QString WorkstationConfig::getNssipaddress()
{
  QString ipAddess = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("nss_server_ip")));
  if (ipAddess.length() < 1)
  {
    ipAddess = DEFAULT_NSS_IP_ADDRESS;
  }
  return ipAddess;
}
/*!
* @fn       setNssipaddress
* @param    const QString& - ip address
* @return   None
* @brief    config for scanner bag data server
*/
void WorkstationConfig::setNssipaddress(const QString &nssipaddress)
{
  Q_UNUSED(nssipaddress);
  //  m_nssipaddress = nssipaddress;
}

/*!
* @fn       getNssportno
* @param    None
* @return   QString - port
* @brief    config for scanner nss server port
*/
QString WorkstationConfig::getNssportno()
{
  QString portNo = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("nss_server_port")));
  if (portNo.length() < 1)
  {
    portNo = DEFAULT_NSS_PORT;
  }
  return portNo;
}
/*!
* @fn       getWorkstationLogoPath
* @param    None
* @return   QString - WorkstationLogoPath
* @brief    config for workstation Logo Path
*/
QString WorkstationConfig::getWorkstationLogoPath()
{
  INFO_LOG("Getting Workstation logo path:" << m_workstationLogoPath.toStdString());
  return m_workstationLogoPath;
}

/*!
* @fn       getCurrentLanguage
* @param    None
* @return   bool
* @brief    Getting Current Language
*/
QString WorkstationConfig::getCurrentLanuage()
{
  return m_currentLanguage;
}

/*!
* @fn       getLanguageCode
* @param    None
* @return   string  - Language
* @brief    gets Language Code
*/
QString WorkstationConfig::getLanguageCode()
{
  if(m_currentLanguage == "German") {
    return "ge";
  }
  else if(m_currentLanguage == "French") {
    return "fr";
  }
  else if(m_currentLanguage == "Italian") {
    return "it";
  }
  else if(m_currentLanguage == "Spanish") {
    return "sp";
  }
  else if(m_currentLanguage == "Japnese") {
    return "jp";
  }
  else if(m_currentLanguage == "Turkish") {
    return "tr";
  }
  else {
    return "en";
  }
}

/*!
* @fn       getCurrentLanguageLocale
* @param    None
* @return   string  - Language
* @brief    gets Language locale
*/
QString WorkstationConfig::getCurrentLanguageLocale()
{
  if(m_currentLanguage == "German") {
    return "de_DE";
  }
  else if(m_currentLanguage == "French") {
    return "fr_FR";
  }
  else if(m_currentLanguage == "Italian") {
    return "it_IT";
  }
  else if(m_currentLanguage == "Spanish") {
    return "es_ES";
  }
  else if(m_currentLanguage == "Japnese") {
    return "ja_JP";
  }
  else if(m_currentLanguage == "Turkish") {
    return "tr_TR";
  }
  else {
    return "en_EN";
  }
}

/*!
* @fn       setNssportno
* @param    const QString& - port
* @return   None
* @brief    config for nss server port
*/
void WorkstationConfig::setNssportno(const QString &nssportno)
{
  Q_UNUSED(nssportno);
  // m_nssportno = nssportno;
}

/*!
* @fn       getNSSServerCheckInterval
* @param    None
* @return   int - timeout
* @brief    time interval to check NSS server availaibility
*/
int WorkstationConfig::getNSSServerCheckInterval()
{
  return m_nssCheckInterval;
}

/*!
* @fn       getScannerConnectionCheckInterval
* @param    None
* @return   int - timeout
* @brief    time interval to check scanner availaibility
*/
int WorkstationConfig::getScannerConnectionCheckInterval()
{
  return m_scannerCheckInterval;
}

/*!
* @fn       getScannerAdminAsyncTaskRefreshInterval
* @param    None
* @return   int - timeout
* @brief    scanner admin service asynchronous task interval.
*/
int WorkstationConfig::getScannerAdminAsyncTaskRefreshInterval()
{
  return m_scanneradminasynctaskrefreshinterval;
}


/*!
* @fn       getScannerAdminAsyncTaskTimeoutInterval
* @param    None
* @return   int - timeout
* @brief    scanner admin service asynchronous task timeout interval.
*/
int WorkstationConfig::getScannerAdminAsyncTaskTimeoutInterval()
{
  return m_scanneradminasynctasktimeoutinterval;
}

/*!
* @fn       getAutoArchiveEnabled
* @param    None
* @return   int - timeout
* @brief    sets auto archive enabled/ disabled.
*/
bool WorkstationConfig::getAutoArchiveEnabled()
{
  bool autoarchive_enabled;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("auto_archive_enabled")));
  if (boolString.length() < 1)
  {
    autoarchive_enabled = DEFAULT_AUTOARCHIVE_ENABLED;
  }
  else if (boolString.compare("true") == 0)
  {
    autoarchive_enabled = true;
  }
  else
  {
    autoarchive_enabled = false;
  }
  return autoarchive_enabled;
}

/*!
* @fn       getScannername
* @param    None
* @return   QString - scannername
* @brief    config for scanner name
*/
QString WorkstationConfig::getScannername()
{
  QString scannerId = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("scanner_id")));
  if (scannerId.length() < 1)
  {
    scannerId = DEFAULT_SCANNERNAME;
  }
  DEBUG_LOG("Getting scanner name : " << scannerId.toStdString());
  return scannerId;
}

/*!
* @fn       setScannername
* @param    const QString - scannername
* @return   None
* @brief    config for scanner name
*/
void WorkstationConfig::setScannername(const QString &scannername)
{
  Q_UNUSED(scannername);
  // LOG(INFO) <<"Setting Scannername to" << m_scannername.toStdString();
  //  m_scannername = scannername;
}

/*!
* @fn       setWorkstationName
* @param    const QString& - workstationName
* @return   None
* @brief    config for workstation name
*/
void WorkstationConfig::setWorkstationName(const QString &workstationName){
  INFO_LOG("Setting Workstation name to: " << workstationName.toStdString());
  m_workstationName = workstationName;
}

/*!
* @fn       getWorkstationName
* @param    None
* @return   QString - WorkstationName
* @brief    config for workstation name
*/
QString WorkstationConfig::getWorkstationName(){
  INFO_LOG("Getting Workstation name: " << m_workstationName.toStdString());
  return m_workstationName;
}

/*!
* @fn       getWorkstationNameEnum
* @param    None
* @return   QMLEnums::WSType
* @brief    config for workstation name
*/
QMLEnums::WSType WorkstationConfig::getWorkstationNameEnum(){
  DEBUG_LOG("Getting Workstation name enum: "
            << QMLEnums::getWSTypeEnum(m_workstationName.toStdString()));
  return QMLEnums::getWSTypeEnum(m_workstationName.toStdString());
}

/**
 * @brief WorkstationConfig::getEnableThreathandling
 * @return
 * Get status of threat view Enable/Disable
 */
bool WorkstationConfig::getEnableThreathandling()
{
  bool enableThreatHandling;
  UILayoutManager* ulm = UILayoutManager::getUILayoutManagerInstance();
  if ( (ulm->getScreenState() == QMLEnums::ScreenEnum::BAGSCREENING_SCREEN)
       || (ulm->getScreenState() == QMLEnums::ScreenEnum::TRAINING_BAG_SCREEN))
  {
    enableThreatHandling = m_enableThreatHandling;
  }
  else
  {
    enableThreatHandling = DEFAULT_RENDERPARAMETER_ENABLE_THREAT_HANDLING;
  }
  return enableThreatHandling;
}

/**
 * @brief getCanUserClearAlarmBag
 * @return bool
 * @brief Get status of if user can clear alarm bag
 */
bool WorkstationConfig::getCanUserClearAlarmBag()
{
    bool CanClearMachineAlarmBag;
    QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("can_clear_machine_alarm_bag")));
    if (boolString.length() < 1)
    {
        CanClearMachineAlarmBag = DEFAULT_CAN_CLEAR_ALARM_BAG;
    }
    else if (boolString.compare("true") == 0)
    {
        CanClearMachineAlarmBag = true;
    }
    else
    {
        CanClearMachineAlarmBag = false;
    }
    return CanClearMachineAlarmBag;
}

/**
 * @brief WorkstationConfig::getEnableVtkSharpenSplash
 * @return
 * Get status of sharpened VTK Splash Screen
 */
bool WorkstationConfig::getEnableVtkSharpenSplash()
{
  bool enableVTKSplashSharpen;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("vtk_sharpen_splash_screen")));
  if (boolString.length() < 1)
  {
    enableVTKSplashSharpen = DEFAULT_RENDERPARAMETER_ENABLE_VTK_SHARPENED_SPLASH;
  }
  else if (boolString.compare("true") == 0)
  {
    enableVTKSplashSharpen = true;
  }
  else if (boolString.compare("false") == 0)
  {
    enableVTKSplashSharpen = false;
  }
  else
  {
    enableVTKSplashSharpen = false;
  }
  return enableVTKSplashSharpen;
}

/**
 * @brief WorkstationConfig::setEnableVtkSharpenSplash
 * Set status of sharpened VTK Splash Screen.
 */
void WorkstationConfig::setEnableVtkSharpenSplash(bool doSharpen)
{
  Q_UNUSED(doSharpen);
  // m_enableVTKSplashSharpen = doSharpen;
}

/**
 * @brief WorkstationConfig::getEnableVtkDynamicSharpenSplash
 * @return
 */
bool WorkstationConfig::getEnableVtkDynamicSharpenSplash()
{
  bool enableVTKDynamicSplashSharpen;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("vtk_dynamic_splash_screen")));
  if (boolString.length() < 1)
  {
    enableVTKDynamicSplashSharpen = DEFAULT_RENDERPARAMETER_ENABLE_VTK_DYNAMIC_SPLASH;
  }
  else if (boolString.compare("true") == 0)
  {
    enableVTKDynamicSplashSharpen = true;
  }
  else
  {
    enableVTKDynamicSplashSharpen = false;
  }
  return enableVTKDynamicSplashSharpen;
}

/*!
  * @fn       getWorkstationDisplayUnitSystem
  * @param    None
  * @return   VREnums::DispalyUnitSystemEnum
  * @brief    config for Workstation Display Unit System
  */
VREnums::DispalyUnitSystemEnum  WorkstationConfig::getWorkstationDisplayUnitSystem()
{
  m_workstationDisplayUnitSystem = VREnums::METRIC;
  QString unitString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("ws_display_unit_system")));
  if (unitString.length() < 1)
  {
    unitString = DEFAULT_WS_DISPLAY_UNIT_SYSTEM;
  }
  else if (unitString == "METRIC")
  {
    m_workstationDisplayUnitSystem =  VREnums::METRIC;
  }
  else if (unitString == "US_IMPERIAL")
  {
    m_workstationDisplayUnitSystem =  VREnums::US_IMPERIAL;
  }
  return m_workstationDisplayUnitSystem;
}

/*!
  * @fn       setWorkstationDisplayUnitSystem
  * @param    VREnums::DispalyUnitSystemEnum
  * @return   None
  * @brief    set config for Workstation Display Unit System
  */
void WorkstationConfig::setWorkstationDisplayUnitSystem(QString unitSystemStr)
{
  m_workstationDisplayUnitSystem = VREnums::METRIC;
  if (unitSystemStr.compare("METRIC") == 0)
  {
    m_workstationDisplayUnitSystem =  VREnums::METRIC;
  }
  else if(unitSystemStr.compare("US_IMPERIAL") == 0)
  {
    m_workstationDisplayUnitSystem =  VREnums::US_IMPERIAL;
  }
}

/**
 * @brief WorkstationConfig::setEnableVtkDynamicSharpenSplash
 * @param doSharpen
 */
void WorkstationConfig::setEnableVtkDynamicSharpenSplash(bool doSharpen)
{
  Q_UNUSED(doSharpen);
  // m_enableVTKDynamicSplashSharpen = doSharpen;
}


/**
 * @brief WorkstationConfig::setEnableThreatHandling
 * @param setVal
 * Enable/Disable the viewing of threats
 */
void WorkstationConfig::setEnableThreatHandling(bool setVal)
{
  Q_UNUSED(setVal);
  // m_enableThreatHandling = setVal;
}


/*!
* @fn       getUseShadingforMainViewer
* @param    None
* @return   bool
* @brief    config for Shading for MainViewer
*/
bool WorkstationConfig::getUseShadingforMainViewer()
{
  return m_useshadingformainviewer;
}

/*!
* @fn       setUseShadingforMainViewer
* @param    bool
* @return   None
* @brief    config for Shading for MainViewer
*/
void WorkstationConfig::setUseShadingforMainViewer(bool useShading)
{
  m_useshadingformainviewer = useShading;
}

/*!
* @fn       getUseShadingforRightViewer
* @param    None
* @return   bool
* @brief    config for Shading for Right MainViewer
*/
bool WorkstationConfig::getUseShadingforRightViewer()
{
  return m_useshadingforrightviewer;
}

/*!
* @fn       setUseShadingforRightViewer
* @param    bool useShading
* @return   None
* @brief    config for Shading for Right MainViewer
*/
void WorkstationConfig::setUseShadingforRightViewer(bool useShading)
{
  m_useshadingforrightviewer = useShading;
}

/**
 * @brief WorkstationConfig::getTranslucentLinearInterpolation
 * @return is the translucent viewer using linear interpolation
 */
bool WorkstationConfig::getTranslucentLinearInterpolation()
{
  return m_traslucentInterpolationLinear;
}

/**
 * @brief WorkstationConfig::setTranslucentLinearInterpolation
 * @param doLinear
 * Make the translucent viewer use iliear interpolation. Choices are
 * linear or nearest.
 */
void WorkstationConfig::setTranslucentLinearInterpolation(bool doLinear)
{
  m_traslucentInterpolationLinear = doLinear;
}

/**
 * @brief WorkstationConfig::getSurfaceLinearInterpolation
 * @return is the Surface viewer using linear interpolation
 */
bool WorkstationConfig::getSurfaceLinearInterpolation()
{
  return m_surfaceInterpolationLinear;
}

/**
 * @brief WorkstationConfig::setSurfaceLinearInterpolation
 * @param doLinear
 * Make the Surface viewer use iliear interpolation. Choices are
 * linear or nearest.
 */
void WorkstationConfig::setSurfaceLinearInterpolation(bool doLinear)
{
  m_surfaceInterpolationLinear = doLinear;
}

/*!
 * @brief getOrganicRemovableForRightViewer
 * @return bool allow surface viewer to remove Organics
 */
bool WorkstationConfig::getOrganicRemovableForRightViewer()
{
  return m_surfaceOrganicRemovable;
}

/*!
 * @brief setOrganicRemovableForRightViewer
 * @param organicRemovable
*/
void WorkstationConfig::setOrganicRemovableForRightViewer(bool organicRemovable)
{
  m_surfaceOrganicRemovable = organicRemovable;
}


/*!
 * @brief getInorganicRemovableForRightViewer
 * @return bool allow surface viewer to remove Inorganics
 */
bool WorkstationConfig::getInorganicRemovableForRightViewer()
{
  return m_surfaceInorganicRemovable;
}

/*!
 * @brief setInorganicRemovableForRightViewer
 * @param inorganicRemovable
*/
void WorkstationConfig::setInorganicRemovableForRightViewer(bool inorganicRemovable)
{
  m_surfaceInorganicRemovable = inorganicRemovable;
}
/*!
* @fn       setMountPath
* @param    std::string mountpath
* @return   void
* @brief    return mountpath
*/
void WorkstationConfig::setMountPath(std::string mountpath)
{
  m_mountPath = mountpath;
}

/*!
* @fn       getUsbMountPath
* @param    None
* @return   QString
* @brief    return mountpath
*/
QString WorkstationConfig::getUsbMountPath()
{
  return m_mountPath.c_str();
}

/*!
* @fn       setDefaultDataPath
* @param    std::string defaultdatapath
* @return   void
* @brief    None
*/
void WorkstationConfig::setDefaultDataPath(std::string defaultdatapath)
{
  m_defaultdatapath = defaultdatapath;
}

/*!
* @fn       getDefaultDataPath
* @param    None
* @return   std::string
* @brief    return defaultdatapath
*/
QString WorkstationConfig::getDefaultDataPath()
{
  return m_defaultdatapath.c_str();
}

/**
 * @brief WorkstationConfig::getMetalRemovableForRightViewer
 * @return bool is metal remvable in surface viewer
 */
bool WorkstationConfig::getMetalRemovableForRightViewer()
{
  return m_surfaceMetalRemovable;
}

/**
 * @brief WorkstationConfig::setMetalRemovableForRightViewer
 * @param enable/disable metal removal in right viewer
 */
void WorkstationConfig::setMetalRemovableForRightViewer(bool enableMetalremoval)
{
  m_surfaceMetalRemovable = enableMetalremoval;
}

/*!
 * @brief getOrganicRemovableForRightViewer
 * @return bool allow surface viewer to remove Organics
 */
bool WorkstationConfig::getOrganicRemovableForLeftViewer()
{
  return m_translucentOrganicRemovable;
}

/*!
 * @brief setOrganicRemovableForLeftViewer
 * @param organicRemovable
*/
void WorkstationConfig::setOrganicRemovableForLeftViewer(bool organicRemovable)
{
  m_translucentOrganicRemovable = organicRemovable;
}


/*!
 * @brief getInorganicRemovableForLeftViewer
 * @return bool allow left viewer to remove Inorganics
 */
bool WorkstationConfig::getInorganicRemovableForLeftViewer()
{
  return m_translucentInorganicRemovable;
}

/*!
 * @brief setInorganicRemovableForleftViewer
 * @param inorganicRemovable
*/
void WorkstationConfig::setInorganicRemovableForLeftViewer(bool inorganicRemovable)
{
  m_translucentInorganicRemovable = inorganicRemovable;
}



/**
 * @brief WorkstationConfig::getMetalRemovableForleftViewer
 * @return bool is metal remvable in left viewer
 */
bool WorkstationConfig::getMetalRemovableForLeftViewer()
{
  return m_translucentMetalRemovable;
}

/**
 * @brief WorkstationConfig::setMetalRemovableForLeftViewer
 * @param enable/disable metal removal in left viewer
 */
void WorkstationConfig::setMetalRemovableForLeftViewer(bool enableMetalremoval)
{
  m_translucentMetalRemovable = enableMetalremoval;
}


/**
 * @brief WorkstationConfig::getTranslucentAmbientLighting
 * @return
 */
double WorkstationConfig::getTranslucentAmbientLighting()
{
  return m_translucentAmbientLighting;
}

/**
 * @brief WorkstationConfig::setTranslucentAmbientLighting
 * @param ambientLight
 */
void WorkstationConfig::setTranslucentAmbientLighting(double ambientLight)
{
  m_translucentAmbientLighting = ambientLight;
}

/**
 * @brief WorkstationConfig::getTranslucentDiffuseLighting
 * @return
 */
double WorkstationConfig::getTranslucentDiffuseLighting()
{
  return m_translucentDiffuseLighting;
}

/**
 * @brief WorkstationConfig::setTranslucentDiffuseLighting
 * @param diffuseLight
 */
void WorkstationConfig::setTranslucentDiffuseLighting(double diffuseLight)
{
  m_translucentDiffuseLighting = diffuseLight;
}

/**
 * @brief WorkstationConfig::getTranslucentSpecularLighting
 * @return
 */
double WorkstationConfig::getTranslucentSpecularLighting()
{
  return m_translucentSpecularLighting;
}

/**
 * @brief WorkstationConfig::setTranslucentSpecularLighting
 * @param specularLight
 */
void WorkstationConfig::setTranslucentSpecularLighting(double specularLight)
{
  m_translucentSpecularLighting = specularLight;
}

/**
 * @brief WorkstationConfig::getSurfaceAmbientLighting
 * @return
 */
double WorkstationConfig::getSurfaceAmbientLighting()
{
  return m_surfaceAmbientLighting;
}

/**
 * @brief WorkstationConfig::setSurfaceAmbientLighting
 * @param ambientLight
 */
void WorkstationConfig::setSurfaceAmbientLighting(double ambientLight)
{
  m_surfaceAmbientLighting = ambientLight;
}

/**
 * @brief WorkstationConfig::getSurfaceDiffuseLighting
 * @return
 */
double WorkstationConfig::getSurfaceDiffuseLighting()
{
  return m_surfaceDiffuseLighting;
}

/**
 * @brief WorkstationConfig::setSurfaceDiffuseLighting
 * @param diffuseLight
 */
void WorkstationConfig::setSurfaceDiffuseLighting(double diffuseLight)
{
  m_surfaceDiffuseLighting = diffuseLight;
}

/**
 * @brief WorkstationConfig::getSurfaceSpecularLighting
 * @return
 */
double WorkstationConfig::getSurfaceSpecularLighting()
{
  return m_surfaceSpecularLighting;
}

/**
 * @brief WorkstationConfig::setSurfaceSpecularLighting
 * @param specularLight
 */
void WorkstationConfig::setSurfaceSpecularLighting(double specularLight)
{
  m_surfaceSpecularLighting = specularLight;
}


/**
 * @brief getVolumeToolStartMin
 * @return Volume measurrement tool minimum start point.
 */
uint16_t WorkstationConfig::getVolumeToolStartMin()
{
  return m_volumeMeasuremtStartMin;
}

/**
 * @brief getVolumeToolStartMax
 * @return Volume measurrement tool maximum start point.
 */
uint16_t WorkstationConfig::getVolumeToolStartMax()
{
  return m_volumeMeasuremtStartMax;
}

/**
 * @brief setVolumeToolStartMin
 * @param setVal
 */
void WorkstationConfig::setVolumeToolStartMin(uint16_t setVal)
{
  m_volumeMeasuremtStartMin = setVal;
}


/**
 * @brief setVolumeToolStartMax
 * @param setVal
 */
void WorkstationConfig::setVolumeToolStartMax(uint16_t setVal)
{
  m_volumeMeasuremtStartMax = setVal;
}

//------------------------------------

/**
 * @brief WorkstationConfig::setScannerDirectionLeftToRight
 * @param dirLeftToRight
 */
void WorkstationConfig::setScannerDirectionLeftToRight(bool dirLeftToRight)
{
  Q_UNUSED(dirLeftToRight);
  // m_scannerDirectionLeftToRight = dirLeftToRight;
}

/**
 * @brief WorkstationConfig::getScannerDirectionLeftToRight
 */
bool WorkstationConfig::getScannerDirectionLeftToRight()
{
  bool scannerDirectionLeftToRight;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("scan_direction_left_right")));
  if (boolString.length() < 1)
  {
    scannerDirectionLeftToRight = DEFAULT_RENDERPARAMETER_SCANNED_DIR_LEFT_TO_RIGHT;
  }
  else if (boolString.compare("true") == 0)
  {
    scannerDirectionLeftToRight = true;
  }
  else
  {
    scannerDirectionLeftToRight = false;
  }
  return scannerDirectionLeftToRight;
}


/*!
* @fn       getExportBagDataPath
* @param    None
* @return   QString - path
* @brief    Folder location where exported bad data would be saved.
*/
QString WorkstationConfig::getExportBagDataPath()
{
  QString exportBagDataPath = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("export_bagdata_path")));
  if (exportBagDataPath.length() < 1)
  {
    exportBagDataPath = DEFAULT_EXPORT_BAG_DATA_PATH;
  }
  return exportBagDataPath;
}


/*!
* @fn       setExportBagDataPath
* @param    None
* @return   QString - path
* @brief    Folder location where exported bad data would be saved.
*/
void WorkstationConfig::setExportBagDataPath(const QString& in)
{
  Q_UNUSED(in);
  // m_exportBagDataPath = in;
}

/*!
* @fn       getExportBagDataMaxCount
* @param    None
* @return   int - max count
* @brief    Maximum number of bags that should be saved locally.
*/
int WorkstationConfig::getExportBagDataMaxCount()
{
  int exportBagDataMaxCount = qvariant_cast<int>(m_WsConfigMap.value(QString::fromStdString("export_bagdata_count")));
  if (exportBagDataMaxCount == 0)
  {
    exportBagDataMaxCount = DEFAULT_EXPORT_BAG_DATA_MAX_COUNT;
  }
  return exportBagDataMaxCount;
}

/*!
* @fn       getWorkstationMachineId
* @param    None
* @return   QString - Id
* @brief    gets workstation machine ID
*/
QString WorkstationConfig::getWorkstationMachineId()
{
  QString workstationMachineId = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("workstation_id")));
  if (workstationMachineId.length() < 1)
  {
    workstationMachineId = DEFAULT_WORKSTATION_NAME;
  }
  return workstationMachineId;
}

/*!
* @fn       setWorkstationMachineId
* @param    QString - Id
* @return   None
* @brief    sets workstation machine ID
*/
void WorkstationConfig::setWorkstationMachineId(const QString &workstationMachineId)
{
  Q_UNUSED(workstationMachineId);
  // m_workstationMachineId = workstationMachineId;
}

/*!
* @fn       setexecutablePath
* @param    std::string path
* @return   None
* @brief    sets the executable dir string
*/
void WorkstationConfig::setexecutablePath(std::string path){
  m_executablePath = path;
}

/*!
 * @fn       getexecutablePath
 * @param    None
 * @return   std::string
 * @brief    gets the executable dir string
 */
std::string WorkstationConfig::getexecutablePath(){
  return m_executablePath;
}

#ifdef WORKSTATION
/*!
* @fn       getSDSWClientBinarypath
* @param    None
* @return   QString - path with filename
* @brief    gets SDSW client binary path with binary. Example /opt/SDSWClient/dist/SDSWClient
*/
QString WorkstationConfig::getSDSWClientBinarypath()
{
  return m_serviceclientbinarypath;
}

/*!
* @fn       setSDSWClientBinarypath
* @param    QString - path with filename
* @return   None
* @brief    sets SDSW client binary path with binary. Example /opt/SDSWClient/dist/SDSWClient
*/
void WorkstationConfig::setSDSWClientBinarypath(const QString &path)
{
  DEBUG_LOG("Getting :" <<getuseradminseveripaddress().toStdString());
  m_serviceclientbinarypath = path;
}

#endif

/*!
* @fn       getuseradminseveripaddress
* @param    None
* @return   string- ipadress
* @brief    gets user admin sever ip address.
*/
QString WorkstationConfig::getuseradminseveripaddress()
{
  QString ipAddess = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("user_admin_server_ip")));
  if (ipAddess.length() < 1)
  {
    ipAddess = DEFAULT_IP_ADDRESS;
  }
  DEBUG_LOG("Getting user admin server ipaddress: " <<ipAddess.toStdString());
  return ipAddess;
}

/*!
* @fn       getuseradminserverportno
* @param    None
* @return   QString- portno
* @brief    gets user admin server portno.
*/
QString WorkstationConfig::getuseradminserverportno()
{
  QString portNo = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("user_admin_server_port")));
  if (portNo.length() < 1)
  {
    portNo = DEFAULT_PORT;
  }
  DEBUG_LOG("Getting user admin server port no: " <<portNo.toStdString());
  return portNo;
}
// TODO Revisit when separating rerun and training

/*!
* @fn       getWsVersion
* @param    None
* @return   QString-
* @brief    get workstationVersion
*/
QString WorkstationConfig::getWsVersion()
{
  return m_workstationVersion;
}
/*!
* @fn       getOsVersion
* @param    None
* @return   QString-
* @brief    get OsVersion
*/
QString WorkstationConfig::getOsVersion()
{
  return m_OSVersion;
}
// TODO Revisit when separating rerun and training
#ifdef WORKSTATION
/*!
* @fn       getreportadminseveripaddress
* @param    None
* @return   string- ipadress
* @brief    gets report admin sever ip address.
*/
QString WorkstationConfig::getreportadminseveripaddress()
{
  QString ipAddess = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("report_admin_server_ip")));
  if (ipAddess.length() < 1)
  {
    ipAddess = DEFAULT_IP_ADDRESS;
  }
  DEBUG_LOG("Getting report admin server ipaddress: " <<ipAddess.toStdString());
  return ipAddess;
}

/*!
* @fn       getreportadminserverportno
* @param    None
* @return   QString- portno
* @brief    gets report admin server portno.
*/
QString WorkstationConfig::getreportadminserverportno()
{
  QString portNo = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("report_admin_server_port")));
  if (portNo.length() < 1)
  {
    portNo = DEFAULT_PORT;
  }
  DEBUG_LOG("Getting report admin server port no: " <<portNo.toStdString());
  return portNo;
}

/*!
* @fn       getantivirusadminseveripaddress
* @param    None
* @return   string- ipadress
* @brief    gets report antivirus ip address.
*/
QString WorkstationConfig::getantivirusseveripaddress()
{
  QString ipAddess = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("network_admin_service_ip")));
  if (ipAddess.length() < 1)
  {
    ipAddess = DEFAULT_IP_ADDRESS;
  }
  DEBUG_LOG("Getting antivirus server ipaddress: " <<ipAddess.toStdString());
  return ipAddess;
}

/*!
* @fn       getreportadminserverportno
* @param    None
* @return   QString- portno
* @brief    gets report admin server portno.
*/
QString WorkstationConfig::getantivirusserverportno()
{
  QString portNo = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("network_admin_service_port")));
  if (portNo.length() < 1)
  {
    portNo = DEFAULT_PORT;
  }
  DEBUG_LOG("Getting antivirus server port no: " <<portNo.toStdString());
  return portNo;
}

/*!
* @fn       getSupervisorSeverIPAddress
* @param    None
* @return   string- ipadress
* @brief    gets supervisor sever ip address.
*/
QString WorkstationConfig::getSupervisorSeverIPAddress()
{
    QString ipAddess = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("supervisor_admin_server_ip")));
    if (ipAddess.length() < 1)
    {
        ipAddess = DEFAULT_IP_ADDRESS;
    }
    return ipAddess;
}

/*!
* @fn       getSupervisorSeverPortNo
* @param    None
* @return   QString- portno
* @brief    gets supervisor server portno.
*/
QString WorkstationConfig::getSupervisorSeverPortNo()
{
    QString portNo = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("supervisor_admin_server_port")));
    if (portNo.length() < 1)
    {
        portNo = DEFAULT_PORT;
    }
    return portNo;
}


#endif

/*!
* @fn       getOperatorCompany
* @param    None
* @return   QString
* @brief    gets operator organization name
*/
QString WorkstationConfig::getOperatorCompany()
{
  if (m_operatorCompany.length() > 0)
    return m_operatorCompany;
  else
    return DEFAULT_OPERATOR_COMPANY_NAME;
}

/*!
* @fn       setOperatorCompany
* @param    QString
* @return   none
* @brief    sets operator organization name
*/
void WorkstationConfig::setOperatorCompany(QString name)
{
  if (name.length() > 0)
    m_operatorCompany = name;
}


/*!
* @fn       getIQbagfileformat
* @param    None
* @return   String - Image Quality bagfileformat
* @brief    config for Image Quality bagfileformat
*/
QString WorkstationConfig::getIQbagfileformat()
{
  return m_iqBagFileFormat;
}

/*!
* @fn       getIQreportfileformat
* @param    None
* @return   String - Image Quality reportfileformat
* @brief    config for Image Quality reportfileformat
*/
QString WorkstationConfig::getIQreportfileformat()
{
  return m_iqReportFileFormat;
}

/*!
* @fn       getIQNistfileformat
* @param    None
* @return   String - Image Quality Nist file format
* @brief    config for Image Quality Nist file format
*/
QString WorkstationConfig::getIQNistfileformat()
{
  return m_iqNistFileFormat;
}

/*!
* @fn       getBagDataResolution
* @param    None
* @return   QSizeF
* @brief    gets bag data resolution
*/
QSizeF WorkstationConfig::getBagDataResolution()
{
  return m_bagdataresolution;
}

/*!
* @fn       getNetworkReconnectTime
* @param    None
* @return   int- Reconnect Time interval
* @brief    gets reconnect time interval.
*/
int WorkstationConfig::getNetworkReconnectTime()
{
  return m_networkreconnecttimer;
}

/*!
* @fn       getNetworkTimeout
* @param    None
* @return   int- Network Timeout
* @brief    gets Network Timeout.
*/
int WorkstationConfig::getNetworkTimeout()
{
  return m_networktimeout;
}

/*!
* @fn       getSoapClientEndPoint
* @param    None
* @return   std::string soap client end point
* @brief    gets soap client end point
*/
std::string WorkstationConfig::getSoapClientEndPoint()
{
  QString soapClientEndPoint = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("lm_soap_client_endpoint")));
  if (soapClientEndPoint.length() < 1)
  {
    soapClientEndPoint = DEFAULT_SOAP_CLIENT_END_POINT;
  }
  DEBUG_LOG("Getting SOAP Client Endpoint: " << soapClientEndPoint.toStdString());
  return soapClientEndPoint.toStdString();
}
/*!
* @fn       getSoapServerPort
* @param    None
* @return   uint16_t server port
* @brief    gets soap server port
*/
uint16_t WorkstationConfig::getSoapServerPort()
{
  uint16_t soapServerPort = qvariant_cast<uint16_t>(m_WsConfigMap.value(QString::fromStdString("sws_soap_server_port")));
  if (soapServerPort == 0)
  {
    soapServerPort = DEFAULT_SOAP_PORT;
  }
  DEBUG_LOG("Getting SOAP Server Port: " << soapServerPort);
  return soapServerPort;
}

/*!
* @fn       getSwsBhsServerIpAddr
* @param    None
* @return   std::string sws bhs server ip address
* @brief    gets sws bhs server ip address
*/
std::string WorkstationConfig::getSwsBhsServerIpAddr()
{
  QString swsBhsServerIpAddr = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("sws_bhs_server_ip_addr")));
  if (swsBhsServerIpAddr.length() < 1)
  {
    swsBhsServerIpAddr = DEFAULT_SWS_BHS_SERVER_IP_ADDR;
  }
  DEBUG_LOG("Getting SWS BHS Server IP Address: " << swsBhsServerIpAddr.toStdString());
  return swsBhsServerIpAddr.toStdString();
}

/*!
* @fn       getSwsBhsServerPort
* @param    None
* @return   uint16_t sws bhs server port
* @brief    gets sws bhs server port
*/
uint16_t WorkstationConfig::getSwsBhsServerPort()
{
  uint16_t swsBhsServerPort = qvariant_cast<uint16_t>(m_WsConfigMap.value(QString::fromStdString("sws_bhs_server_port")));
  if (swsBhsServerPort == 0)
  {
    swsBhsServerPort = DEFAULT_SWS_BHS_SERVER_PORT;
  }
  DEBUG_LOG("Getting SWS BHS Server Port: " << swsBhsServerPort);
  return swsBhsServerPort;
}

/*!
* @fn       getRfidReaderIpAddr
* @param    None
* @return   std::string rfid reader ip address
* @brief    gets rfid reader ip address
*/
std::string WorkstationConfig::getRfidReaderIpAddr()
{
  QString rfidReaderIpAddr = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("rfid_reader_ip_addr")));
  if (rfidReaderIpAddr.length() < 1)
  {
    rfidReaderIpAddr = DEFAULT_RFID_READER_IP_ADDR;
  }
  DEBUG_LOG("Getting RFID Reader IP Address: " << rfidReaderIpAddr.toStdString());
  return rfidReaderIpAddr.toStdString();
}

/*!
* @fn       getBhsType
* @param    None
* @return   std::string BHS type
* @brief    gets BHS type
*/
std::string WorkstationConfig::getBhsType()
{
  QString bhsType = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("bhs_operation")));
  if (bhsType.length() < 1)
  {
    bhsType = "None";
  }
  DEBUG_LOG("Getting BHS type (operation): " << bhsType.toStdString());
  return bhsType.toStdString();
}

/*!
* @fn       getBhsBinType
* @param    None
* @return   std::string BHS bin type
* @brief    gets BHS bin type
*/
std::string WorkstationConfig::getBhsBinType()
{
  QString bhsBinType = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("bhs_bin_type")));
  if (bhsBinType.length() < 1)
  {
    bhsBinType = "None";
  }
  DEBUG_LOG("Getting BHS bin type (operation): " << bhsBinType.toStdString());
  return bhsBinType.toStdString();
}

/*!
* @fn       getRfidReaderPort
* @param    None
* @return   uint16_t rfid reader port
* @brief    gets rfid reader port
*/
uint16_t WorkstationConfig::getRfidReaderPort()
{
  uint16_t rfidReaderPort = qvariant_cast<uint16_t>(m_WsConfigMap.value(QString::fromStdString("rfid_reader_port")));
  if (rfidReaderPort == 0)
  {
    rfidReaderPort = DEFAULT_RFID_READER_PORT;
  }
  DEBUG_LOG("Getting RFID Reader Port: " << rfidReaderPort);
  return rfidReaderPort;
}

/**
 * @brief getNoofBagPreprocessingThreads
 * @return int
 * @brief get no of bag processing thread
 */
int WorkstationConfig::getNoofBagPreprocessingThreads()
{
  return m_NoofBagPreprocessingThreads;
}

/**
 * @brief setNoofBagPreprocessingThreads
 * @return int
 * @brief set no of bag processing thread
 */
void WorkstationConfig::setNoofBagPreprocessingThreads(int NoofBagPreprocessingThreads)
{
  m_NoofBagPreprocessingThreads = NoofBagPreprocessingThreads;
}


/**
 * @brief getMinimumRequiredSliceCount
 * @return int
 * @brief get number slice required to enable decision
 */
int WorkstationConfig::getMinimumRequiredSliceCount()
{
  return m_minimumRequiredSliceCount;
}

/**
 * @brief setMinimumRequiredSliceCount
 * @return int
 * @brief set number slice required to enable decision
 */
void WorkstationConfig::setMinimumRequiredSliceCount(int minimumRequiredSliceCount)
{
  m_minimumRequiredSliceCount = minimumRequiredSliceCount;
}

/**
 * @brief getBagDecisionTimeout
 * @return int
 * @brief get user decision timeout
 */
int WorkstationConfig::getBagDecisionTimeout()
{
  int bagdecisionTimeout = qvariant_cast<int>(m_WsConfigMap.value(QString::fromStdString("user_decision_timeout")));
  if((bagdecisionTimeout >= 0) && (bagdecisionTimeout <= 3600))
  {
    bagdecisionTimeout = bagdecisionTimeout*1000;   //  converting to milisecond
  }
  else
  {
    bagdecisionTimeout = 0;
  }
  return bagdecisionTimeout;
}

/**
 * @brief showDialogForUserDecisionTimeout
 * @return bool
 * @brief is popup needed for user decision timeout
 */
bool WorkstationConfig::showDialogForUserDecisionTimeout()
{
  bool showDialogForUserDecisionTimeout;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("show_user_decision_timout_confirm")));
  if (boolString.length() < 1)
  {
    showDialogForUserDecisionTimeout = DEFAULT_SHOW_USER_DECISION_TIMEOUT_CONFIRMATION;
  }
  else if (boolString.compare("true") == 0)
  {
    showDialogForUserDecisionTimeout = true;
  }
  else
  {
    showDialogForUserDecisionTimeout = false;
  }
  return showDialogForUserDecisionTimeout;
}

/**
 * @brief getSlabbingThickness
 * @return
 */
double WorkstationConfig::getSlabbingThickness()
{
  double slab_thickness = qvariant_cast<double>(m_WsConfigMap.value(QString::fromStdString("slab_thickness")));
  if (slab_thickness == 0.0)
  {
    slab_thickness = DEFAULT_SLABBING_THICKNESS;
  }
  return slab_thickness;
}

/*!
* @fn       updateProperty
* @param    QString name
* @param    QString value
* @param    QString type
* @return   None
* @brief    update properties in map
*/
void WorkstationConfig::updateProperty(QString name, QString value , QString type)
{
  Q_UNUSED(type);
  m_WsConfigMap.insert(name, value);
}

/*!
* @fn       getConfigInifilename
* @param    None
* @return   QString
* @brief    gets config ini file name
*/
QString WorkstationConfig::getConfigInifilename()
{
  return m_configinifilename;
}

/*!
* @fn       setConfigInifilename
* @param    QString
* @return   None
* @brief    sets config ini file name
*/
void WorkstationConfig::setConfigInifilename(const QString &configinifilename)
{
  m_configinifilename = configinifilename;
}

/*!
* @fn       getConfigxsdfilename
* @param    None
* @return   QString
* @brief    gets config xsd file name
*/
QString WorkstationConfig::getConfigxsdfilename()
{
  return m_configxsdfilename;
}

/*!
* @fn       setConfigxsdfilename
* @param    QString
* @return   None
* @brief    sets config xsd file name
*/
void WorkstationConfig::setConfigxsdfilename(const QString &configxsdfilename)
{
  m_configxsdfilename = configxsdfilename;
}

/*!
* @fn       getConfigxmlfilename
* @param    None
* @return   QString
* @brief    gets config xml file name
*/
QString WorkstationConfig::getConfigxmlfilename()
{
  return m_configxmlfilename;
}

/*!
* @fn       setConfigxmlfilename
* @param    QString
* @return   None
* @brief    sets config xml file name
*/
void WorkstationConfig::setConfigxmlfilename(const QString &configxmlfilename)
{
  m_configxmlfilename = configxmlfilename;
}

/*!
* @fn       updateWorkstationConfig
* @param    ConfigBaseElement config
* @return   None
* @brief    update Workstation Config
*/
void WorkstationConfig::updateWorkstationConfig(ConfigBaseElement config)
{
  QString propertyName;
  QString propertyValue;
  if (config.m_values.size() == 0)
  {
    propertyName = config.tagName();
    propertyValue = config.value();
    updateProperty(propertyName, propertyValue, propertyValue);
  }
  else if (config.m_values.size() > 0)
  {
    for (int i=0; i < config.m_values.size() ; i++)
    {
      ConfigBaseElement config_rec = qvariant_cast<ConfigBaseElement>(config.m_values.at(i));
      updateWorkstationConfig(config_rec);
    }
  }
}

/*!
* @fn       getWorkstationManufactId
* @param    None
* @return   QString
* @brief    gets workstation Manufacturer Id
*/
QString WorkstationConfig::getWorkstationManufactId()
{
  QString manufactId = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("manufacturer")));
  if (manufactId.length() < 1)
  {
    manufactId = DEFAULT_MANUFACTURER;
  }
  DEBUG_LOG("Getting manufacturer:" <<manufactId.toStdString());
  return manufactId;
}

/*!
* @fn       getManufacturerModelNumber
* @param    None
* @return   QString
* @brief    gets workstation Manufacturer model number
*/
QString WorkstationConfig::getManufacturerModelNumber()
{
  QString temp = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("manufacturer_model_number")));
  if (temp.length() < 1)
  {
    temp = DEFAULT_MODEL_NUMBER;
  }
  DEBUG_LOG("Getting manufacture model numver:" <<temp.toStdString().c_str());
  return temp;
}

/*!
* @fn       getWorkstationSerialNumber
* @param    None
* @return   QString
* @brief    gets workstation serial number
*/
QString WorkstationConfig::getWorkstationSerialNumber()
{
  QString temp = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("workstation_serial_number")));
  if (temp.length() < 1)
  {
    temp = DEFAULT_SERIAL_NUMBER;
  }
  DEBUG_LOG("Getting device serial number:" <<temp.toStdString().c_str());
  return temp;
}

/*!
* @fn       getOrganizationName
* @param    None
* @return   QString
* @brief    gets workstation organization name
*/
QString WorkstationConfig::getOrganizationName()
{
  QString temp = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("organization")));
  if (temp.length() < 1)
  {
    temp = DEFAULT_ORGANIZATION_NAME;
  }
  DEBUG_LOG("Getting Organization name:" <<temp.toStdString().c_str());
  return temp;
}

/*!
* @fn       getMachineLocation
* @param    None
* @return   QString
* @brief    gets workstation Machie location
*/
QString WorkstationConfig::getMachineLocation()
{
  QString temp = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("machinelocation")));
  if (temp.length() < 1)
  {
    temp = DEFAULT_STRING_VALUE;
  }
  DEBUG_LOG("Getting Machine Location:" <<temp.toStdString().c_str());
  return temp;
}

/*!
* @fn       getMachineAddress
* @param    None
* @return   QString
* @brief    gets workstation Machine address
*/
QString WorkstationConfig::getMachineAddress()
{
  QString temp = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("machineaddress")));
  if (temp.length() < 1)
  {
    temp = DEFAULT_STRING_VALUE;
  }
  DEBUG_LOG("Getting Machine Address:" <<temp.toStdString().c_str());
  return temp;
}

/*!
* @fn       getInfoFilePath
* @param    None
* @return   QString
* @brief    gets info file path from ini
*/
QString WorkstationConfig::getInfoFilePath()
{
  return m_infoFilePath;
}

/*!
* @fn       isBagPickupEnabled
* @param    None
* @return   bool
* @brief    gets pickup enable satus
*/
bool WorkstationConfig::isBagPickupEnabled()
{
  bool temp;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("tray_pickup_enable")));
  if (boolString.length() < 1)
  {
    temp = DEFAULT_SLABBINGMODE;
  }
  else if (boolString.compare("true") == 0)
  {
    temp = true;
  }
  else
  {
    temp = false;
  }
  return temp;
}

/*!
* @fn       isStopTheCheckVisible
* @param    None
* @return   bool
* @brief    Enable/Disable it to show/Hide
*/
bool WorkstationConfig::isStopTheCheckVisible()
{
  bool temp;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("stop_the_check_visible")));
  if (boolString.length() < 1)
  {
    temp = true;  // By Default set it to enable
  }
  else if (boolString.compare("true") == 0)
  {
    temp = true;
  }
  else
  {
    temp = false;
  }
  return temp;
}

/*!
* @fn       isDisplayOperatorClearedAlarmEnabled
* @param    None
* @return   bool
* @brief    Enable/Disable it to show/Hide User Decision on SVS image overlay
*/
bool WorkstationConfig::isDisplayOperatorClearedAlarmEnabled()
{
  bool temp;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("display_operator_cleared_alarm_enable")));
  if (boolString.length() < 1)
  {
    temp = true;  // By Default set it to enable
  }
  else if (boolString.compare("true") == 0)
  {
    temp = true;
  }
  else
  {
    temp = false;
  }
  return temp;
}

/*!
* @fn       getSearchDefaultRightviewModeType
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets default mode type
*/
VREnums::VRViewModeEnum WorkstationConfig::getSearchDefaultRightviewModeType()
{
  QString modeType = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("rightviewer_search_operation")));
  if(modeType.length() > 1)
  {
    if(modeType.compare("Threat") == 0)
      return VREnums::VIEW_AS_THREAT;
    else if(modeType.compare("Surface") == 0)
      return VREnums::VIEW_AS_SURFACE;
    else if(modeType.compare("Surface Camera") == 0)
      return VREnums::VIEW_AS_SURFACE_CAMERA;
    else
      return VREnums::VIEW_AS_THREAT;  // By default
  }
  else if (modeType.length() < 1)
  {
    return VREnums::VIEW_AS_THREAT;
  }
  DEBUG_LOG("Getting Right Viewer Search mode type (default): " << modeType.toStdString());
  return VREnums::VIEW_AS_THREAT;
}

/*!
* @fn       getRecallDefaultRightviewModeType
* @param    None
* @return   VREnums::VRViewModeEnum
* @brief    gets default mode type
*/
VREnums::VRViewModeEnum WorkstationConfig::getRecallDefaultRightviewModeType()
{
  QString modeType = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("rightviewer_recall_operation")));
  if(modeType.length() > 1)
  {
    if(modeType.compare("Threat") == 0)
      return VREnums::VIEW_AS_THREAT;
    else if(modeType.compare("Surface") == 0)
      return VREnums::VIEW_AS_SURFACE;
    else if(modeType.compare("Surface Camera") == 0)
      return VREnums::VIEW_AS_SURFACE_CAMERA;
    else
      return VREnums::VIEW_AS_THREAT;  // By default
  }
  else if (modeType.length() < 1)
  {
    return VREnums::VIEW_AS_THREAT;
  }
  DEBUG_LOG("Getting Right Viewer Recall mode type (default): " << modeType.toStdString());
  return VREnums::VIEW_AS_SURFACE;
}
/*!
* @fn       getTipConfigIpAddress
* @param    None
* @return   QString - ip
* @brief    tip config server
*/
QString WorkstationConfig::getTipConfigIpAddress()
{
  QString ipAddess = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("tip_config_server_ip")));
  if (ipAddess.length() < 1)
  {
    ipAddess = DEFAULT_NSS_IP_ADDRESS;
  }
  return ipAddess;
}

/*!
* @fn       getTipConfigPortNo
* @param    None
* @return   QString - port
* @brief    Tip config server port
*/
QString WorkstationConfig::getTipConfigPortNo()
{
  QString portNo = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("tip_config_server_port")));
  if (portNo.length() < 1)
  {
    portNo = DEFAULT_NSS_PORT;
  }
  return portNo;
}

/*!
* @fn       getTipLibConfigIpAddress
* @param    None
* @return   QString - ip
* @brief    tip lib server
*/
QString WorkstationConfig::getTipLibConfigIpAddress()
{
  QString ipAddess = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("tip_lib_server_ip")));
  if (ipAddess.length() < 1)
  {
    ipAddess = DEFAULT_NSS_IP_ADDRESS;
  }
  return ipAddess;
}

/*!
* @fn       getTipLibConfigPortNo
* @param    None
* @return   QString - port
* @brief    Tip config server port
*/
QString WorkstationConfig::getTipLibConfigPortNo()
{
  QString portNo = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("tip_lib_server_port")));
  if (portNo.length() < 1)
  {
    portNo = DEFAULT_NSS_PORT;
  }
  return portNo;
}

/*!
* @fn       getTipInitialDecisionTimeout
* @param    None
* @return   int - timeout value
* @brief    Tip Initial Decision Timeout value
*/
int WorkstationConfig::getTipInitialDecisionTimeout()
{
  int tipInitialDecisionTimeout = qvariant_cast<int>(m_WsConfigMap.value(QString::fromStdString("tip_initial_decision_timeout")));
  return tipInitialDecisionTimeout*1000;  // return in miliisecond
}

/*!
* @fn       getTipAnalysisTimeout
* @param    None
* @return   int - timeout value
* @brief    Tip Analysis Timeout value
*/
int WorkstationConfig::getTipAnalysisTimeout()
{
  int tipAnalysisTimeout = qvariant_cast<int>(m_WsConfigMap.value(QString::fromStdString("tip_analysis_timeout")));
  return tipAnalysisTimeout*1000;  // return in miliisecond
}

/*!
* @fn       getTipHITMsgDialog
* @param    None
* @return   QString - message
* @brief    Tip message for user decision
*/
QString WorkstationConfig::getTipHITMsgDialog()
{
  return m_tipHITMsg;
}

/*!
* @fn       setTipHITMsgDialog
* @param    QString - message
* @return
* @brief    set Tip message for user decision
*/
void WorkstationConfig::setTipHITMsgDialog(QString msg)
{
  m_tipHITMsg = msg;
}

/*!
* @fn       getTipMISSMsgDialog
* @param    None
* @return   QString - message
* @brief    Tip message for user decision
*/
QString WorkstationConfig::getTipMISSMsgDialog()
{
  return m_tipMISSMsg;
}

/*!
* @fn       setTipMISSMsgDialog
* @param    QString - message
* @return   None
* @brief    set Tip message for user decision
*/
void WorkstationConfig::setTipMISSMsgDialog(QString msg)
{
  m_tipMISSMsg = msg;
}

/*!
* @fn       getTipNONTIPMsgDialog
* @param    None
* @return   QString - message
* @brief    Tip message for user decision
*/
QString WorkstationConfig::getTipNONTIPMsgDialog()
{
  return m_tipNonTIPMsg;
}

/*!
* @fn       setTipNONTIPMsgDialog
* @param    QString - message
* @return   None
* @brief    Tset ip message for user decision
*/
void WorkstationConfig::setTipNONTIPMsgDialog(QString msg)
{
  m_tipNonTIPMsg = msg;
}

/*!
* @fn       getHighThreatUpperText
* @param    None
* @return   QString
* @brief    get high threat upper text value
*/
QString WorkstationConfig::getHighThreatUpperText()
{
  QString temp = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("high_threat_upper_text")));
  if (temp.length() < 1)
  {
    temp = DEFAULT_STRING_VALUE;
  }
  DEBUG_LOG("Getting high threat upper text:" <<temp.toStdString().c_str());
  return temp;
}

/*!
* @fn       getHighThreatLowerText
* @param    None
* @return   QString
* @brief    get high threat lower text value
*/
QString WorkstationConfig::getHighThreatLowerText()
{
  QString temp = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("high_threat_lower_text")));
  if (temp.length() < 1)
  {
    temp = DEFAULT_STRING_VALUE;
  }
  DEBUG_LOG("Getting high threat lower text:" <<temp.toStdString().c_str());
  return temp;
}

/*!
* @fn       getNonTipDialogEnable
* @param    None
* @return   bool
* @brief    get non tip enable text value
*/
bool WorkstationConfig::getNonTipDialogEnable()
{
  bool nonTipEnable;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("show_non_tip_dialog")));
  if (boolString.length() < 1)
  {
    nonTipEnable = DEFAULT_NON_TIP_ENABLE;
  }
  else if (boolString.compare("true") == 0)
  {
    nonTipEnable = true;
  }
  else
  {
    nonTipEnable = false;
  }
  DEBUG_LOG("Getting show non tip dialog enable text:" <<boolString.toStdString().c_str());
  return nonTipEnable;
}

/*!
* @fn       setDataPath
* @param    QString
* @return   None
* @brief    set data path variable
*/
void WorkstationConfig::setDataPath(const QString dataPath)
{
  if(dataPath.isEmpty())
  {
    m_dataPath = getDefaultDataPath();
  }
  else
  {
    m_dataPath = dataPath;
  }
}

/*!
* @fn       getDataPath
* @param    None
* @return   QString
* @brief    get data path
*/
QString WorkstationConfig::getDataPath()
{
  return m_dataPath;
}

/*!
* @fn       clearWSConfigMap
* @param    None
* @return   None
* @brief    clear map data
*/
void WorkstationConfig::clearWSConfigMap()
{
  m_WsConfigMap.clear();
}

/*!
* @fn       getBhsNotificationTimeout
* @param    None
* @return   int
* @brief    return bhs bag notification popup timeout.
*/
int WorkstationConfig::getBhsNotificationTimeout()
{
  int bhsNotificationTimeout = qvariant_cast<int>(m_WsConfigMap.value(QString::fromStdString("bag_notification_popup_timeout")));
  if((bhsNotificationTimeout >= 0) && (bhsNotificationTimeout <= 3600))
  {
    bhsNotificationTimeout = bhsNotificationTimeout*1000;   //  converting to milisecond
  }
  else
  {
    bhsNotificationTimeout = -1;
  }
  return bhsNotificationTimeout;
}

/*!
* @fn       getssibanner
* @param    None
* @return   bool- banner stauts
* @brief    gets ssi banner status flag*/
bool WorkstationConfig::getssibanner()
{
  return m_ssibanner;
}

/**
 * @brief showMachineAlarmDecision
 * @return bool
 * @brief is popup needed for machine alarm visiblity decision
 */
bool WorkstationConfig::showMachineAlarmDecision()
{
  bool showMachineAlarmDecision;
  QString boolString = qvariant_cast<QString>(m_WsConfigMap.value(QString::fromStdString("show_machine_alarm")));
  if (boolString.length() < 1)
  {
    showMachineAlarmDecision = DEFAULT_MACHINE_ALARM_DECISION;
  }
  else if (boolString.compare("true") == 0)
  {
    showMachineAlarmDecision = true;
  }
  else
  {
    showMachineAlarmDecision = false;
  }
  return showMachineAlarmDecision;
}


}  // end of namespace ws
}  // end of namespace analogic
