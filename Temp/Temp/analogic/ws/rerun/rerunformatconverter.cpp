/*!
* @file     rerunformatconverter.cpp
* @author   Agiliad
* @brief    This file contains interface to related to managing batch file creations.
* @date     Sep, 29 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#include <analogic/ws/rerun/rerunformatconverter.h>

namespace analogic
{
namespace ws
{

/*!
   * @fn       RerunManager
   * @param    None
   * @return   None
   * @brief    Constructor for class RerunFormatConverter.
   */
RerunFormatConverter::RerunFormatConverter(QObject *parent):QObject(parent)
{
}

/*!
   * @fn       RerunManager
   * @param    None
   * @return   None
   * @brief    Destructor for class RerunManager.
   */
RerunFormatConverter::~RerunFormatConverter()
{}


/*!
* @fn       readFromFile
* @param    QString filePathName
* @return   QStringList
* @brief    return the contents of a given file line-by-line
*/
QStringList RerunFormatConverter::readFromFile(QString filePathName)
{
  QStringList list;
  QFile file(filePathName);
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      list << line;
    }
    file.close();
  }
  else
  {
    ERROR_LOG(filePathName.toStdString() + " failed to open.");
  }
  return list;
}


/*!
   * @fn       checkAndUpdateFileList
   * @param    bagListFile
   * @return   boolean
   * @brief    To check if file contains jpeg_ls_Vol files.
   */
int RerunFormatConverter::checkAndUpdateFileList(std::string inputListFileName, QStringList &inFileNames , QStringList &conversionRequiredFileNames)
{
  int archivedCount = 0;
  inFileNames = readFromFile(QString(inputListFileName.c_str()));
  for(int i = 0; i< inFileNames.size(); i++)
  {
    QStringList temp = inFileNames[i].split("/");
    int len = temp.size();
    QString filename = temp.at(len-1);
    //checking for archived bagdata.
    if(filename.contains(BAGFILE_JPEG_VOLUME_FORMAT))
    {
      if(!checkIfProjFilesPresent(filename))  //If any file is missing then add that file to conversion list.
      {
        archivedCount++;
        conversionRequiredFileNames << inFileNames[i];
      }
      QString volUpdatedFile = QString(inFileNames[i]).replace(BAGFILE_JPEG_VOLUME_FORMAT,BAGFILE_VOLUME_FORMAT,
                                                               Qt::CaseInsensitive);
      inFileNames[i] = volUpdatedFile;

    }
    DEBUG_LOG("inFileNames[" <<i <<"] is :" <<
              QString(inFileNames[i]).toStdString());


  }
  if(writeSanitizedFileList(inputListFileName, inFileNames))
  {
    ERROR_LOG("Failed to write in file"<< inputListFileName);
  }

  return archivedCount;
}

/*!
   * @fn       writeSanitizedFileList
   * @param    QStringList
   * @return   bool
   * @brief    Updated the FileList with vol files instead of jpeg_ls_files
   */
bool RerunFormatConverter::writeSanitizedFileList(std::string inputListFileName, QStringList &inFileNames)
{
  QFile file(QString(inputListFileName.c_str()));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    ERROR_LOG(inputListFileName + " failed to open.");
    return false;
  }

  for(int i =0  ;i<inFileNames.size() ; i++)
  {
    file.write(inFileNames.at(i).toStdString().c_str());
    file.write("\n");

  }
  file.close();
  return true;
}


/*!
   * @fn       PopulateBagInfoFromFile
   * @param    QStringList
   * @return   bool
   * @brief    To convert volume files to raw
   */
bool RerunFormatConverter::PopulateBagInfoFromFile(QString filename)
{
  // Read metadata from .properties file.
  QString fname = filename;
  fname.remove(BAGFILE_JPEG_VOLUME_FORMAT);
  fname += BAGFILE_PROPERTY_FORMAT;

  if(m_rbagInfo.constructFromFile(fname))
  {
    INFO_LOG("Reading BagInfo successs : "<<filename.toStdString());
    return true;
  }
  else
  {
    ERROR_LOG("Failed to read BagInfo: "<<filename.toStdString());
    return false;
  }
  return true;
}

/*!
   * @fn       DecompressFile
   * @param    QStringList
   * @return   bool
   * @brief    To convert volume files to raw
   */
bool RerunFormatConverter::DecompressFile(std::string  jpeg_ls_volume_filename , std::string  raw_volume_filename ,
                                          int slice_width, int slice_height )
{
  const uint32_t kMaxUncompressedSliceWidth = 690; //690;
  const uint32_t kMaxUncompressedSliceHeight = 550; //550;
  const uint64_t kMaxUncompressedSliceSize = kMaxUncompressedSliceWidth*kMaxUncompressedSliceHeight*2;
  size_t uncompressed_slice_size_in_bytes = slice_width*slice_height*2;

  struct UncompressedSlice {
    char data[kMaxUncompressedSliceSize];
  };
  std::ifstream in_volumefile;
  in_volumefile.open(jpeg_ls_volume_filename, std::ios::in | std::ios::binary);
  if (!in_volumefile.is_open()) {
    ERROR_LOG("Error opening JPEG-LS volume file: " << jpeg_ls_volume_filename);
    return false;
  }

  std::ofstream out_volumefile;
  out_volumefile.open(raw_volume_filename, std::ios::out | std::ios::binary);
  if (!out_volumefile.is_open()) {
    ERROR_LOG("Error opening raw volume file: " << raw_volume_filename);
    return false;
  }

  char compressed_slice[kMaxUncompressedSliceSize + 1024];
  char uncompressed_slice[kMaxUncompressedSliceSize];

  uint64_t total_read = 0;

  INFO_LOG("Decompressing File In: "<<jpeg_ls_volume_filename<<" Out: "<<raw_volume_filename \
           <<" SliceWidth: "<<slice_width<<" SliceHeight: "<<slice_height);

  // Read and decompress one slice at a time
  while (!in_volumefile.eof()) {
    uint32_t compressed_slice_size_in_bytes;
    in_volumefile.read(reinterpret_cast<char*>(&compressed_slice_size_in_bytes), sizeof(uint32_t));

    if (!in_volumefile) {
      break;
    }
    total_read += compressed_slice_size_in_bytes;

    if (compressed_slice_size_in_bytes == 0) {
      ERROR_LOG( "Error: slice size is zero : "<<jpeg_ls_volume_filename);
    }

    in_volumefile.read(compressed_slice, compressed_slice_size_in_bytes);

    if (!in_volumefile) {
      ERROR_LOG ("Error reading from file "<<jpeg_ls_volume_filename);
    }

    size_t actually_read = (size_t) in_volumefile.gcount();

    if (actually_read != compressed_slice_size_in_bytes) {
      ERROR_LOG ("Error reading actually_read : "<<actually_read <<"  size : "<<compressed_slice_size_in_bytes);
    }

    try {
      JLS_ERROR ret = JpegLsDecode(reinterpret_cast<void*>(uncompressed_slice),
                                   uncompressed_slice_size_in_bytes, compressed_slice, compressed_slice_size_in_bytes, NULL);

      if (ret != JLS_ERROR::OK) {
        ERROR_LOG ("Error while decompressing slice: "<<ret);
        continue;
      }
    } catch (const JlsException& e) {
      ERROR_LOG ("Exception decompressing slice: "<<e._error );
      return false;
    }
    out_volumefile.write(uncompressed_slice, uncompressed_slice_size_in_bytes);
  }
  in_volumefile.close();
  out_volumefile.close();
  return true;
}



/*!
   * @fn       ConvertDicosProjToProj
   * @param    QStringList
   * @return   bool
   * @brief    To convert volume files to raw
   */
bool RerunFormatConverter::ConvertDicosProjToProj(QString in_dicos_proj_file_name, QString out_proj_file_name)
{
  INFO_LOG("Proj Conversion : In : "<<in_dicos_proj_file_name.toStdString()<<"  Out: "<<out_proj_file_name.toStdString());

  SDICOS::Filename filename(in_dicos_proj_file_name.toStdString());
  SDICOS::CTModule ct;
  SDICOS::ErrorLog errorlog;

  if (!ct.Read(filename, errorlog)) {
    ERROR_LOG("Error: Cannot open CT module: " << in_dicos_proj_file_name.toStdString() << ": " << errorlog);
    return false;
  }

  SDICOS::CTTypes::CTImage &image = ct.GetCTImage();

  // Assume the data is 16 bit
  SDICOS::Array3DLarge<SDICOS::S_UINT16> array_data;
  image.GetPixelData(array_data);

  std::streamsize twod_length = array_data.GetWidth() * array_data.GetHeight();

  // Open the output file for writing
  FILE* out = std::fopen(out_proj_file_name.toStdString().c_str(), "wb");
  if (out == NULL) {
    ERROR_LOG("Error: Cannot open output file for writing: " << out_proj_file_name.toStdString());
    return false;
  }

  // For each frame/slice, write the raw data to the file
  for (int i = 0; i < array_data.GetDepth(); ++i) {
    size_t check = fwrite(array_data[i].GetBuffer(), sizeof(SDICOS::S_UINT16), twod_length, out);

    if (check != twod_length) {
      ERROR_LOG("Error: Could not write all bytes to output file" << out_proj_file_name.toStdString());
      return false;
    }
  }

  std::fclose(out);

  INFO_LOG("Successfully extracted data size" << array_data.GetWidth()<< "," <<
           array_data.GetHeight() << "," << array_data.GetDepth() << "]" );
  return true;
}


/*!
   * @fn       CheckAndUpdateProjFileNames
   * @param    QStringList
   * @return   bool
   * @brief    Check which input DICOS proj file is present based on that update the in/out file names
   */
bool RerunFormatConverter::CheckAndUpdateProjFileNames(QString in_jpegls_file_name ,
                                                      QString &in_dicos_proj00_file_name ,
                                                      QString &in_dicos_proj90_file_name ,
                                                      QString &out_proj00_file_name ,
                                                      QString &out_proj90_file_name  )
{
  QString temp = in_jpegls_file_name;
  QString dicos_20_proj00_file_name = in_jpegls_file_name.replace(BAGFILE_JPEG_VOLUME_FORMAT,BAGFILE_DICOSCT_PROJ00_FORMAT,
                                                                       Qt::CaseInsensitive);
  in_jpegls_file_name = temp;
  QString dicos_20_proj90_file_name = in_jpegls_file_name.replace(BAGFILE_JPEG_VOLUME_FORMAT,BAGFILE_DICOSCT_PROJ90_FORMAT,
                                                                       Qt::CaseInsensitive);
  in_jpegls_file_name = temp;
  QString dicos_20A_proj00_file_name = in_jpegls_file_name.replace(BAGFILE_JPEG_VOLUME_FORMAT,BAGFILE_DICOSCT_DCS_PROJ00_FORMAT,
                                                                       Qt::CaseInsensitive);
  in_jpegls_file_name = temp;
  QString dicos_20A_proj90_file_name = in_jpegls_file_name.replace(BAGFILE_JPEG_VOLUME_FORMAT,BAGFILE_DICOSCT_DCS_PROJ90_FORMAT,
                                                                       Qt::CaseInsensitive);
  in_jpegls_file_name = temp;


  bool isDicos20Proj00FilePresent = QFileInfo::exists(dicos_20_proj00_file_name) && QFileInfo(dicos_20_proj00_file_name).isFile();
  bool isDicos20Proj90FilePresent = QFileInfo::exists(dicos_20_proj90_file_name) && QFileInfo(dicos_20_proj90_file_name).isFile();

  bool isDicos20AProj00FilePresent = QFileInfo::exists(dicos_20A_proj00_file_name) && QFileInfo(dicos_20A_proj00_file_name).isFile();
  bool isDicos20AProj90FilePresent = QFileInfo::exists(dicos_20A_proj90_file_name) && QFileInfo(dicos_20A_proj90_file_name).isFile();

  INFO_LOG("Are dicos proj files present : "<<isDicos20Proj00FilePresent << " " << isDicos20Proj90FilePresent \ 
                                            <<isDicos20AProj00FilePresent << " " << isDicos20AProj90FilePresent);
  if(isDicos20Proj00FilePresent && isDicos20Proj90FilePresent)
  {
    in_dicos_proj00_file_name = dicos_20_proj00_file_name;
    in_dicos_proj90_file_name = dicos_20_proj90_file_name;
    out_proj00_file_name = in_jpegls_file_name.replace(BAGFILE_JPEG_VOLUME_FORMAT,BAGFILE_PROJ00_FORMAT, Qt::CaseInsensitive);
    in_jpegls_file_name = temp;
    out_proj90_file_name = in_jpegls_file_name.replace(BAGFILE_JPEG_VOLUME_FORMAT,BAGFILE_PROJ90_FORMAT, Qt::CaseInsensitive);
    in_jpegls_file_name = temp;
    INFO_LOG("Dicos 2.0 proj files are present "<<in_dicos_proj00_file_name.toStdString()<<"  "   \
                                                  <<in_dicos_proj90_file_name.toStdString()<<"  " \
                                                  <<out_proj00_file_name.toStdString()<<"  " \
                                                  <<out_proj90_file_name.toStdString());
    return true;

  }
  else if(isDicos20AProj00FilePresent && isDicos20AProj90FilePresent)
  {
    in_dicos_proj00_file_name = dicos_20A_proj00_file_name;
    in_dicos_proj90_file_name = dicos_20A_proj90_file_name;
    out_proj00_file_name = in_jpegls_file_name.replace(BAGFILE_JPEG_VOLUME_FORMAT, BAGFILE_PROJ00_FORMAT , Qt::CaseInsensitive);
    in_jpegls_file_name = temp;
    out_proj90_file_name = in_jpegls_file_name.replace(BAGFILE_JPEG_VOLUME_FORMAT, BAGFILE_PROJ90_FORMAT , Qt::CaseInsensitive);
    in_jpegls_file_name = temp;
    INFO_LOG("Dicos 2.0A proj files are present "<<in_dicos_proj00_file_name.toStdString()<<"  "   \
                                                  <<in_dicos_proj90_file_name.toStdString()<<"  " \
                                                  <<out_proj00_file_name.toStdString()<<"  " \
                                                  <<out_proj90_file_name.toStdString());
    return true;
  }
  else{
     ERROR_LOG("No Projection files present Expected files : " << dicos_20_proj00_file_name.toStdString() << " "  \
                                                                << dicos_20_proj90_file_name.toStdString() << " "  \
                                                                << dicos_20A_proj00_file_name.toStdString() << " "  \
                                                                << dicos_20A_proj90_file_name.toStdString());
     return false;
  }
}



/*!
   * @fn       convertArchiveToRaw
   * @param    QStringList
   * @return   bool
   * @brief    To convert volume files to raw
   */
bool RerunFormatConverter::convertArchiveToRaw(QString in_jpegls_file_name)
{
  bool ret;
  QString out_raw_file_name = in_jpegls_file_name;
  QString in_dicos_proj00_file_name = in_jpegls_file_name;
  QString in_dicos_proj90_file_name = in_jpegls_file_name;
  QString out_dicos_proj00_file_name = in_jpegls_file_name;
  QString out_dicos_proj90_file_name = in_jpegls_file_name;

  out_raw_file_name = out_raw_file_name.replace
      (BAGFILE_JPEG_VOLUME_FORMAT,BAGFILE_VOLUME_FORMAT,
       Qt::CaseInsensitive);

  ret = CheckAndUpdateProjFileNames(in_jpegls_file_name ,                                            \
                           in_dicos_proj00_file_name , in_dicos_proj90_file_name ,          \
                           out_dicos_proj00_file_name , out_dicos_proj90_file_name  );

  if(ret == false){
    ERROR_LOG("Faild to get the names of proj file or files not present.");
    return false;
  }

  ret = PopulateBagInfoFromFile(in_jpegls_file_name);

  if(ret){
    INFO_LOG("Successfully Read Propertis file for : "<<in_jpegls_file_name.toStdString());
    ret = DecompressFile(in_jpegls_file_name.toStdString() ,out_raw_file_name.toStdString() ,
                         m_rbagInfo.m_volWidth.toInt() , m_rbagInfo.m_volHeight.toInt());
    if(ret){
      INFO_LOG("Successfully Converted file "<<in_jpegls_file_name.toStdString() <<"  To : "<<out_raw_file_name.toStdString());
      int ret1 = ConvertDicosProjToProj(in_dicos_proj00_file_name , out_dicos_proj00_file_name );
      int ret2 = ConvertDicosProjToProj(in_dicos_proj90_file_name , out_dicos_proj90_file_name );
      ret = ret1 && ret2;
    }
    else{
      ERROR_LOG("Failed to Convert File : "<<in_jpegls_file_name.toStdString());
    }
  }
  else{
    ERROR_LOG("Failed to Convert File : "<<in_jpegls_file_name.toStdString());
  }
  return ret;

}

/*!
   * @fn       checkForProjectionFilesPresent
   * @param    QStringList
   * @return   bool
   * @brief    is used to check if proj00 and proj90 is present for the above file
   */
bool RerunFormatConverter::checkIfProjFilesPresent(QString fileName)
{
  QStringList extList ;
  int count=0;
  extList << BAGFILE_VOLUME_FORMAT << BAGFILE_PROJ00_FORMAT << BAGFILE_PROJ90_FORMAT;
  for(int k=0; k<extList.size();k++)
  {
    fileName.replace(BAGFILE_JPEG_VOLUME_FORMAT,extList.at(k));
    QFileInfo tempFileName = fileName;
    if(!tempFileName.exists())
    {
      count++;
    }
  }
  return (!(count>0));
}

} //end ws namespppace
} //end analogic namespace
