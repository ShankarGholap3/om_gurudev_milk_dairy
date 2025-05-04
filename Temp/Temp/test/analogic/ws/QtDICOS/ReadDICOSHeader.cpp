//------------------------------------------------------------------------------
// File: ReadDICOSHeader.cpp
// Purpose: Implimentation of Read/Write DICOS CT files
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "ReadDICOSHeader.h"
#include "SDICOS/SdcsDICOS.h"


std::string g_logFile = "/home/analogic/junk/DICOSErrors.log";

namespace DICOS
{



//------------------------------------------------------------------------------
bool readDICOSCtFile(SDICOS::CTModule& ctModule,
                     const std::string& dicosDataFile,
                     const std::string& dicosLogfile)
{
  SDICOS::ErrorLog errorlog;

  std::string fullName = dicosDataFile;
  std::cout << "Full name of CT file: " << fullName << std::endl;

  SDICOS::Filename ctFilename(fullName);
  ctModule.GetCTImage().DeletePixelData();
  ctModule.Read(ctFilename, errorlog);
  ctModule.GetCTImage().SetImageInfo(420,
                                     620,
                                     16,
                                     14,
                                     14,
                                     SDICOS::ImagePixelMacro::enumUnsignedInteger);


  int imageSizeX = ctModule.GetCTImage().GetColumns();
  int imageSizeY = ctModule.GetCTImage().GetRows();

  std::cout << "right after read  - Image Size X: " << imageSizeX << std::endl;
  std::cout << "right after write - Image Size Y: " << imageSizeY << std::endl;


  if (errorlog.HasErrors())
  {
    SDICOS::Filename log(dicosLogfile.c_str());
    errorlog.WriteLog(log);
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool writeDICOSCtFile(SDICOS::CTModule* ct,
                      analogic::workstation::ImageProperties* iProp,
                      SimpleScanProperties& scanProps,
                      const std::string& dicosDataFile,
                      const std::string& dicosLogFile)
{
  int imageWidth  = iProp->m_imageWidth;
  int imageHeight = iProp->m_imageHeight;
  int imageLength = iProp->m_imageMaxLength;



  /*
   * The CTModule object is the module-level API object that represent the
   * Computed Tomography (CT) Information Object Definition (IOD) in the DICOS Specification Section 7.
   *
   * #include "SdcsCT.h"
   *
   * All objects in the Stratovan DICOS Toolkit are declared in the SDICOS namespace.
   * SDICOS::CTModule is not a thread-safe object.
   * The SDICOS::CTTypes namespace contains classes that implement DICOS modules specific the CT IOD.
   */

  // Objects and data for CT Modules are in the SDICOS::CTTypes namespace
  SDICOS::CTTypes::CTImage &image = ct->GetCTImage();

  /*
   * CT Data is stored as a buffer of unsigned shorts
   * Internally this is implemented as a 1-D array of 2-D arrays so (X,Y) slices and contiguous blocks of memory.
   */
  SDICOS::Array3DLarge<SDICOS::S_UINT16> vnVolume;  // = image.GetPixelData();
  // ARO-NOTE. If we don't want to write data  just set the
  // imageWidth  = 1
  // imageheight = 1
  // imageLength = 1
  imageWidth  = 1;
  imageHeight = 1;
  imageLength = 1;
  SDICOS::S_UINT32 width(imageWidth), height(imageHeight), depth(imageLength);
  vnVolume.SetSize(width, height, depth);

  SDICOS::S_UINT16 val = 123;

  //
  // The 3D array of CT data can be set in multiple ways
  //
  // Method 1: Accessing the array using 3-D indices.
  //
  SDICOS::S_UINT32 x, y, z;
  for (z = 0; z < depth; ++z)
    for (y = 0; y < height; ++y)
      for (x = 0; x < width; ++x)
        vnVolume[z][y][x] = val;        // Implicitly converts data to SDICOS::S_UINT16.

  //
  // Method 2: Accessing the data one plane at a time
  // note that the 3D memory is not a contiguous block. Slices are contiguous blocks.
  //
  for (z = 0; z < depth; ++z)
  {
    SDICOS::Array2D<SDICOS::S_UINT16> &zPlane = vnVolume[z];
    for (y = 0; y < height; ++y)
      for (x = 0; x < width; ++x)
        zPlane[y][x] = val;  // If needed convert raw data to SDICOS::S_UINT16 cConvertData<type>(val);
  }

  //
  // Method 3: Copy data directly into the raw buffers of the array.
  // Requires caller to setup data buffers in the correct format
  //
  SDICOS::S_UINT16 * slice = new SDICOS::S_UINT16[width*height];

  for (z = 0; z < depth; ++z)
  {
    memcpy(vnVolume[z].GetBuffer(), slice, width*height*sizeof(SDICOS::S_UINT16));
  }

  delete [] slice;

  //
  // Method 4: Access data explicitly as 2D planes
  //
  // 3D volume is an array of 2D slices
  // GetBuffer() returns a pointer to an array of slices
  //

  for (SDICOS::S_UINT32 z = 0; z < depth; ++z)
  {
    // one-dimensional array that holds XY plane of width*height dimensions
    SDICOS::S_UINT16 * xyPlane = vnVolume[z].GetBuffer();

    /*
           * Set data for a single slice as if it is a 1-D array.
           */
    for (SDICOS::S_UINT32 i = 0; i < (width*height); ++i)
      xyPlane[i] = z;
  }

  /*
   * Origin and Spacing are stored in the SDICOS::CTTypes::CTMultiframeFunctionalGroupMacros
   */
  SDICOS::Point3D<float> ptOrigin(0.0f, 0.0f, 0.0f);
  SDICOS::Point3D<float> ptSpacing(iProp->m_pixelSizeX,
                                   iProp->m_pixelSizeY,
                                   iProp->m_pixelSizeZ);

  /*
   *      High and low-level API methods generally return true/false to indicate
   *      success and failure.
   */
  bool bRes(true);

  /*
   *      The following code sets up all required attributes in the CT Image IOD (Section &, Table 14)
   *      This shows the minimum set of required attributes needed for a DICOS file to conform
   *      to the DICOS Specification.
   */

  /*
   * DcsdDateTime object takes data as "YYYYMMDD" and time as "HHMMSS"
   */
  SDICOS::DcsDateTime dateTime;
  SDICOS::DcsDate date;
  SDICOS::DcsTime time;
  date.SetToday();
  time.SetNow();
  dateTime.Set(date, time);

  /*
   * The CT Series class represents the CT Series Module (Table 16)
   *
   * #include "SdcsCTSeries.h"
   */
  SDICOS::CTTypes::CTSeries &series = ct->GetSeries();
  bRes = bRes && series.SetAcquisitionStatus(SDICOS::GeneralSeriesModule::enumSuccessful);
  bRes = bRes && series.SetDateAndTime(date, time);

  //
  // Set the Unique ID of the Scan for the CTSeries.
  //
  SDICOS::DcsUniqueIdentifier instanceUID("1.2.276.0.7230010.3.1.3.4126100761.8984.1407370732.880");
  bRes = bRes && series.SetInstanceUID(instanceUID);

  /*
   * The CT Image class represents the CT Image Module (Table 17)
   *
   * See "SdcsCTImage.h"
   *
   * Currently the data is copied from the given array into the
   * CTImage module.
   */

  bRes = bRes && image.SetPixelData(vnVolume);
  bRes = bRes && image.SetAcquisitionDateAndTime(dateTime);
  bRes = bRes && image.SetBurnedInAnnotation(false);

  /*
   * See the documentation for SDICOS::ImagePixelMacro::PHOTOMETRIC_INTERPRETATION
   */
  bRes = bRes && image.SetPhotometricInterpretation(SDICOS::ImagePixelMacro::enumMonochrome2);

  /*
   * The CT Image Type attrib SDICOS::ErrorLog errorlog;
  SDICOS::Filename ctFilename(dicosDataFile.c_str());
  SDICOS::Filename log("/home/analogic/junk/DICOS_log.txt");
  ct->Write(ctFilename, SDICOS::DicosFile::enumLittleEndianExplicit, errorlog);
  errorlog.WriteLog(log);
ute contains 4 values.
   */
  bRes = bRes && image.SetImageTypeCT(SDICOS::CTTypes::CTImage::enumOriginal,
                                      SDICOS::CTTypes::CTImage::enumPhotoelectric,
                                      SDICOS::CTTypes::CTImage::enumVolume,
                                      SDICOS::CTTypes::CTImage::enumNone);
  bRes = bRes && image.SetAcquisitionDuration(30);

  SDICOS::CommonImageDescription * commonImageDescription = image.GetCommonImageDescription();
  bRes = bRes && (S_NULL != commonImageDescription);
  bRes = bRes && commonImageDescription->SetPixelPresentation(SDICOS::CommonImageDescription::enumPP_Monochrome);
  bRes = bRes && commonImageDescription->SetVolumeBasedCalculationTechnique(
        SDICOS::CommonImageDescription::enumVC_None);
  bRes = bRes && commonImageDescription->SetVolumetricProperties(SDICOS::CommonImageDescription::enumVP_Volume);

  /*
   * The CTTypes::CTMultiframeFunctionalGroups class is the CT Specific implementation of the
   * MultiframeFunctionalGroups (Table 109) module
   * with the CT Specific MultiftoLocal8Bitrame Functional Groups in Table 15
   */
  SDICOS::CTTypes::CTMultiframeFunctionalGroups& multiframeFunctionalGroups = ct->GetMultiframeFunctionalGroups();
  bRes = bRes && multiframeFunctionalGroups.SetNumberOfFrames(vnVolume.GetDepth());
  bRes = bRes && multiframeFunctionalGroups.SetContentDateAndTime(date, time);
  bRes = bRes && multiframeFunctionalGroups.SetInstanceNumber(1001);

  /*
   * Passing true to the GetSharedFunctionalGroups(), GetPixelMeasurement(), GetPlaneOrientation(),
   * GetPlanePosition(), GetPixelValueTransformation(), GetImageFrameType() and GetXRayDetails()
   * methods ensures that space is allocated for the PixelMeasurement object.
   */
  multiframeFunctionalGroups.AllocateSharedFunctionalGroups();
  SDICOS::CTTypes::CTMultiframeFunctionalGroupMacros *pmfg = multiframeFunctionalGroups.GetSharedFunctionalGroups();

  //
  // Modules that are marked as 'U' or User optional need to be explicitly allocated
  // by the user to indicate that their attributes are being used.
  //
  pmfg->AllocatePixelMeasurement();
  pmfg->AllocatePlaneOrientation();
  pmfg->AllocatePlanePosition();
  pmfg->AllocatePixelValueTransformation();
  pmfg->AllocateImageFrameType();
  pmfg->AllocateXRayDetails();
  bRes = bRes && pmfg->GetPixelMeasurement();

  /*
   * Set Pixel spacing in MM for X and Y directions.
   * Pixel spacing in the distance in MM between pixel centers.
   */
  SDICOS::PixelMeasurement *pixelMeasurement = pmfg->GetPixelMeasurement();
  bRes = bRes && pixelMeasurement && pixelMeasurement->SetPixelSpacingInMM(ptSpacing.x, ptSpacing.y);

  /*
   * Set slice thickness in MM (Z direction)
   */
  bRes = bRes && pixelMeasurement && pixelMeasurement->SetSliceThicknessInMM(ptSpacing.z);

  /*
   * Set plane orientation using cosines to define frame vectors.
   */
  SDICOS::PlaneOrientation * planeOrientation = pmfg->GetPlaneOrientation();
  bRes = bRes && planeOrientation && planeOrientation->SetImageOrientation(1, 0, 0, 0, 1, 0);

  /*
   * Set plane origin position in millimeters.
   */
  SDICOS::PlanePosition * planePosition = pmfg->GetPlanePosition();
  bRes = bRes && planePosition && planePosition->SetPositionInMM(ptOrigin.x, ptOrigin.y, ptOrigin.z);

  /*
   * The PixelValueTransformation specifies a linear transform applied to the
   * stored values to produce values in the output units.
   */
  SDICOS::CTTypes::CTPixelValueTransformation * pixelValueTransform = pmfg->GetPixelValueTransformation();
  bRes = bRes && pixelValueTransform && pixelValueTransform->SetRescaleIntercept(-1000);
  bRes = bRes && pixelValueTransform && pixelValueTransform->SetRescaleSlope(1);

  /*
   * String value indicating what the output units are after applying pixel transformation
   */
  bRes = bRes && pixelValueTransform && pixelValueTransform->SetRescaleType("HU");

  /*
   * Same as as call to ct.GetCTImage().SetImageTypeCT()
   */
  SDICOS::CTTypes::CTImageFrameType * imageFrameType = pmfg->GetImageFrameType();
  bRes = bRes && imageFrameType && imageFrameType->SetFrameType(SDICOS::CTTypes::CTImage::enumOriginal,
                                                                SDICOS::CTTypes::CTImage::enumPhotoelectric,
                                                                SDICOS::CTTypes::CTImage::enumVolume,
                                                                SDICOS::CTTypes::CTImage::enumNone);

  /*
   * The CommonImageDescription data is shared between the CTImage and CTMultiframeFunctionalGroupMacros
   */
  if (imageFrameType)
  {
    SDICOS::CommonImageDescription& description = imageFrameType->GetCommonImageDescription();
    description = *(ct->GetCTImage().GetCommonImageDescription());
  }

  /*
   * The CTXRayDetails module is required if the first value in the CT Image Type attribute is ORIGINAL or MIXED.
   * See CTImage::SetImageTypeCT which is set above by calling SetImageTypeCT
  */
  SDICOS::CTTypes::CTXRayDetails * xRayDetails = pmfg->GetXRayDetails();
  bRes = bRes && xRayDetails->SetFilterMaterial(SDICOS::CTTypes::CTXRayDetails::enumLead);
  bRes = bRes && xRayDetails->SetFilterType(SDICOS::CTTypes::CTXRayDetails::enumNone);
  bRes = bRes && xRayDetails->SetFocalSpotSizeInMM(0);
  bRes = bRes && xRayDetails->SetKVP(0);

  /*
   * The SDICOS::FrameOfReference class represents the Frame of Reference (Table 107)
   * The Frame of Reference UID uniquely identifies a frame of reference for a series and
   * is a grouping to indicate that multiple series in a scan have the same frame of reference.
   */
  SDICOS::DcsUniqueIdentifier frameOfReferenceUID;

  // Generate a randomized unique identifier
  frameOfReferenceUID.SetGUID();

  SDICOS::FrameOfReference& frame = ct->GetFrameOfReference();
  bRes = bRes && frame.SetFrameOfReferenceUID(frameOfReferenceUID);

  /*
   * The ObjectOfInspectionModule class represents Object Of Inspection Module (OOI) (Section 4.1, Table 10)
   */
  SDICOS::ObjectOfInspectionModule& ooi = ct->GetObjectOfInspection();
  SDICOS::ObjectOfInspectionModule::IdInfo& idInfo = ooi.GetID();
  // * BAGID!!!!

  bRes = bRes && idInfo.SetID(scanProps.m_bagID.c_str());
  bRes = bRes && idInfo.SetIdAssigningAuthority("Issuer-001");
  bRes = bRes && idInfo.SetIdType(SDICOS::ObjectOfInspectionModule::IdInfo::enumText);
  bRes = bRes && ooi.SetType(SDICOS::ObjectOfInspectionModule::enumTypeBaggage);

  /*
   * The GeneralScanModule class represents the General Scan Module (Section 5 Table 12)
   */

  SDICOS::GeneralScanModule& generalScan = ct->GetGeneralScan();
  bRes = bRes && generalScan.SetScanID("Scan-0001");

  // Create unique identifier using static class method
  bRes = bRes && generalScan.SetScanInstanceUID(SDICOS::DcsUniqueIdentifier::CreateGUID());
  bRes = bRes && generalScan.SetStartDateAndTime(date, time);
  bRes = bRes && generalScan.SetType(SDICOS::GeneralScanModule::enumOperational);


  // * The GeneralEquipmentModule class represent the General Equipment Module (Table 70)
  // * Required information about the machine the scan was captured on.

  SDICOS::GeneralEquipmentModule& generalEquipment = ct->GetGeneralEquipment();
  bRes = bRes && generalEquipment.SetCalibrationDateAndTime(date, time);
  bRes = bRes && generalEquipment.SetDeviceSerialNumber("010101-A0001");
  bRes = bRes && generalEquipment.SetMachineAddress("Logan Airport, Bostom MA");

  //---------------------
  // Machine ID
  bRes = bRes && generalEquipment.SetMachineID(scanProps.m_machineID.c_str());

  bRes = bRes && generalEquipment.SetMachineLocation("Check Point 1");
  bRes = bRes && generalEquipment.SetManufacturer("Analogic Corp");
  bRes = bRes && generalEquipment.SetManufacturerModelName("CheckPoint");
  bRes = bRes && generalEquipment.SetSoftwareVersion("1.0.0");


  //  * The SOPCommonModule class represents the SOP Common Module (Table 82)
  //  * which defines attributes for identifying associated Service Object Pair (SOP) instances

  SDICOS::SopCommonModule& sopCommon = ct->GetSopCommon();
  bRes = bRes && sopCommon.SetSopInstanceCreationDateAndTime(date, time);

  SDICOS::DcsUniqueIdentifier sopInstanceUID("1.2.276.0.7230010.3.1.4.4126100761.8984.1407370732.881");
  bRes = bRes && sopCommon.SetSopInstanceUID(sopInstanceUID);

  /*
   * The CommonInstanceReferenceModule class represents the Common Instance Reference Module (Table 87)
   * which describes the relationships between SOP Instances referenced between modules in the same instance.
   * In this example, we have one scan with one series.
   */
  SDICOS::CommonInstanceReferenceModule& commonInstanceRef = ct->GetCommonInstanceReference();
  commonInstanceRef.AllocateReferencedSeries();

  SDICOS::ReferencedSeriesSequence * refSeries = commonInstanceRef.GetReferencedSeries();
  bRes = bRes && (S_NULL != refSeries);

  if (refSeries)
  {
    SDICOS::Array1D<SDICOS::ReferencedSeriesSequence::ReferencedSeries> & referencedSeries =
        refSeries->GetReferencedSeries();
    bRes = bRes && referencedSeries.SetSize(1);

    //
    // This Series Instance UID matches the one for the original CTSeries
    // indicating that the CTSeries is referenced here
    //
    bRes = bRes && referencedSeries[0].SetSeriesInstanceUID(instanceUID);

    SDICOS::Array1D<SDICOS::SopInstanceReference>& referencedInstances =
        referencedSeries[0].GetReferencedInstances();
    bRes = bRes && referencedInstances.SetSize(1);
    bRes = bRes && referencedInstances[0].SetClassUID(SDICOS::SOPClassUID::GetCT());

    // Specify UID with a user defined string
    bRes = bRes && referencedInstances[0].SetInstanceUID("1.2.276.0.7230010.3.1.4.4126100761.8984.1407370732.885");
  }

  SDICOS::ErrorLog errorlog;
  SDICOS::Filename ctFilename(dicosDataFile.c_str());
  SDICOS::Filename log(dicosLogFile.c_str());
  ct->Write(ctFilename, SDICOS::DicosFile::enumLittleEndianExplicit, errorlog);
  errorlog.WriteLog(log);

  return bRes;
}




//------------------------------------------------------------------------------
}   // namespace DICOS
//------------------------------------------------------------------------------
