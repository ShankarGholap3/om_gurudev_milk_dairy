#ifndef RERUNFORMATHANDLER_H
#define RERUNFORMATHANDLER_H

#include <QObject>
#include <QList>
#include <QDebug>
#include <Logger.h>
#include <analogic/ws/common.h>
#include <analogic/ws/common/bagdata/bagdata.h>

#include "CharLS/interface.h"
#include "CharLS/util.h"
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{


class RerunFormatConverter : public QObject
{
    Q_OBJECT
public:
    enum ProjFileType{
      PROJ_FILE_0_DEGREE,
      PROJ_FILE_90_DEGREE
    }ProjFileType;



    /*!
    * @fn       Rerun Format Converter
    * @param    None
    * @return   None
    * @brief    Constructor for class Rerun Format Converter.
    */

    explicit RerunFormatConverter(QObject* parent = NULL);

    /*!
    * @fn       ~Rerun Format Converter
    * @param    None
    * @return   None
    * @brief    Destructor for class Rerun Format Converter.
    */
    ~RerunFormatConverter();

    /*!
       * @fn       checkAndUpdateFileList
       * @param    bagListFile
       * @return   boolean
       * @brief    To check if file contains jpeg_ls_Vol files.
       */
    int checkAndUpdateFileList(std::string inputListFileName, QStringList &inFileNames , QStringList &conversionRequiredFileNames);

    /*!
       * @fn       readFromFile
       * @param    bagListFile
       * @return   boolean
       * @brief   Read from input file and populate a QStringLlist
       */
    QStringList readFromFile(QString filePathName);


    /*!
       * @fn       writeSanitizedFileList
       * @param    bagListFile
       * @return   boolean
       * @brief    Write the List to an file
       */
    bool writeSanitizedFileList(std::string inputListFileName, QStringList &inFileNames);


    /*!
       * @fn       convertArchiveToRaw
       * @param    QStringList
       * @return   bool
       * @brief    To convert volume files to raw
       */
    bool convertArchiveToRaw(QString in_jpegls_file_name);

    /*!
       * @fn       ConvertDicosProjToProj
       * @param    QString in_dicos_proj_file_name
       * @return   QString out_proj_file_name
       * @brief    To convert dicos CT Proj images to non dicos proj images
       */
    bool ConvertDicosProjToProj(QString in_dicos_proj_file_name, QString out_proj_file_name);


private:

    /*!
       * @fn       CheckAndUpdateProjFileNames
       * @param    QStringList
       * @return   bool
       * @brief    Check which input DICOS proj file is present based on that update the in/out file names
       */
    bool CheckAndUpdateProjFileNames(QString in_jpegls_file_name ,
                                                        QString &in_dicos_proj00_file_name ,
                                                        QString &in_dicos_proj90_file_name ,
                                                        QString &out_proj00_file_name ,
                                                        QString &out_proj90_file_name  );
    /*!
       * @fn       PopulateBagInfoFromFile
       * @param    QStringList
       * @return   bool
       * @brief    To convert volume files to raw
       */
    bool PopulateBagInfoFromFile(QString filename);


    /*!
       * @fn       DecompressFile
       * @param    QStringList
       * @return   bool
       * @brief    Performs the actual decompression
       */
    bool DecompressFile(std::string  jpeg_ls_volume_filename , std::string  raw_volume_filename , int slice_width, int slice_height );

    /*!
       * @fn       checkIfProjFilesPresent
       * @param    QStringList
       * @return   bool
       * @brief    is used to check if proj00 and proj90 is present for the above file
       */
    bool checkIfProjFilesPresent(QString fileName);

    BagInfo m_rbagInfo;

};

}  // end of namespace ws
}  // end of namespace analogic

#endif // RERUNFORMATHANDLER_H
