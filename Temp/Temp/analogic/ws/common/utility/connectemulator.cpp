/*!
* @file     connectemulator.cpp
* @author   Agiliad
* @brief    This file contains connect emulator implementing emulatorhelper listener.
*           This class sends command to detection libs and listens for responses.
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <analogic/ws/common/utility/connectemulator.h>
#include <EmulatorHelper.h>
#include <analogic/ws/common.h>
#include <QDebug>
#include <QDir>

namespace analogic
{
namespace ws
{

/*!
* @fn       ConnectEmulator
* @param    None
* @return   None
* @brief    Constructor for ConnectEmulator
*/
ConnectEmulator::ConnectEmulator(QObject *parent):QObject(parent)
{
    m_rerunFormatConverter = new RerunFormatConverter();
    m_p = NULL;
}

/*!
* @fn       ConnectEmulator
* @param    None
* @return   None
* @brief    copy Constructor for ConnectEmulator
*/
ConnectEmulator::ConnectEmulator(const ConnectEmulator& orig) {
    (void)orig;
}

/*!
* @fn       ~ConnectEmulator
* @param    None
* @return   None
* @brief    Virtual distructor for ConnectEmulator
*/
ConnectEmulator::~ConnectEmulator() {
  SAFE_DELETE(m_rerunFormatConverter);
}

/*!
* @fn       checkForFileFormatConversions
* @param    None
* @return   None
* @brief    Create EmulatorHelper and initialize it.
*/
int ConnectEmulator::checkForFileFormatConversions(std::string inputListFileName)
{
  QStringList inFileList;
  QStringList conversionRequiredFileList;
  int numFilesReqireConversion = m_rerunFormatConverter->checkAndUpdateFileList(inputListFileName , inFileList, conversionRequiredFileList);

  INFO_LOG("Number of files requiring conversion is :  " <<numFilesReqireConversion<<" OF "<<inFileList.size());

  if(numFilesReqireConversion > 0)
  {
      //only if number of file is > 0
      emit rerunResponse(QString::fromStdString("Number of files requiring conversion: " +  \
                                            std::to_string(numFilesReqireConversion) + \
                                            " of " + std::to_string(inFileList.size())), \
                                            QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);

  }

  for(int j=0;j<conversionRequiredFileList.count();j++)
  {
    emit rerunResponse(QString::fromStdString("File conversion in progress : " + conversionRequiredFileList.at(j).toStdString()),QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);

    bool ret = m_rerunFormatConverter->convertArchiveToRaw(conversionRequiredFileList.at(j));
    if(ret == false)
    {
      emit rerunResponse(QString::fromStdString("File conversion Failed : " + conversionRequiredFileList.at(j).toStdString()),QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
      ERROR_LOG("Failed to convert " << conversionRequiredFileList.at(j).toStdString());
    }
    else
    {
      emit rerunResponse(QString::fromStdString("File conversion successful : " + conversionRequiredFileList.at(j).toStdString()),QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
    }
  }
}

/*!
   * @fn       copyDataToLocalStorage
   * @param    QString, QString
   * @return   void
   * @brief    to create a local copy of compressed files and
               decompress it locally instead in media (update for ANSS-1668).
   */
void ConnectEmulator::copyDataToLocalStorage(std::string inputFilename)
{
    QStringList inFileNames = m_rerunFormatConverter->readFromFile(QString(inputFilename.c_str()));
    if(!inFileNames.isEmpty())
    {
        //creating symlinks for folders of input directory.
        QFileInfo srcFile(QString(inputFilename.c_str()));
        QString srcPath = srcFile.absolutePath();

        //creating a directory for rerun symlink to get created.
        //this directory will be deleted while logging out of the emulator.
        QDir rerunDir ;
        QString localDirectory = QDir::tempPath() + "/rerunCompressed";
        bool ret = rerunDir.mkpath(localDirectory);
        if (!ret)
        {
            ERROR_LOG("Failed to create dir: " << localDirectory.toStdString());
        }
        else
        {
            DEBUG_LOG("Directory/location set to: " << localDirectory.toStdString());
        }

        if(ret)
        {
            QString destPath = rerunDir.absoluteFilePath(localDirectory);
            QStringList newInFileNames;

            //creating symlinks for files in local directory.
            for(int i=0; i<inFileNames.size(); ++i)
            {
                QFileInfo bagPath(QString(inFileNames[i]).trimmed());

                //to fetch srcFile name.
                if(!(QString(inFileNames[i]).trimmed().isEmpty()) && bagPath.exists())
                {
                    QString bagFile = QString(inFileNames[i]).replace
                            (bagPath.completeSuffix(), "*" , Qt::CaseSensitive);

                    //to create dest filename for symbolic link
                    QString symLink = "ln -s " + bagFile + " " + destPath ;
                    system(symLink.toStdString().c_str());

                    //updating to localDirectory paths
                    QString newPath = QString(inFileNames[i]).replace(srcPath, destPath, Qt::CaseInsensitive);
                    newInFileNames.append(newPath);
                }
            }

            //create copy of original bag list..
            QString inputFileBackup = QString(inputFilename.c_str()) + "_backup";
            bool success = QFile::copy(QString(inputFilename.c_str()), inputFileBackup);
            if(success)
            {
                DEBUG_LOG("Backup file created successfully");
            }
            else
            {
                ERROR_LOG("Backup file creation failed ");
            }
            m_rerunFormatConverter->writeSanitizedFileList(inputFilename, newInFileNames);
        }
    }
}

/*!
   * @fn       updateOutputFile
   * @param    QString, QString
   * @return   void
   * @brief    to replace the result and baglist text files of rerun detection
               with original paths of the bagFiles (update for ANSS-1668).
   */
void ConnectEmulator::updateOutputFile(QString inputFilename, QString outputFolder)
{
    //check if backup list file exists
    QString inputFileBackup = inputFilename + "_backup";
    QFileInfo backupFile(inputFileBackup);
    if(backupFile.exists())
    {
        //to replace baglist with original baglist;
        bool ret1 = QFile::remove(inputFilename);
        bool ret2 = QFile::rename(inputFileBackup, inputFilename);
        if(ret1 && ret2)
        {
            DEBUG_LOG("BagList updated back to original version");
        }
        else
        {
            ERROR_LOG("Failed to updated BagList back to original version")
        }
    }
    //to fetch file path of the baglist.
    QFileInfo file(inputFilename);
    QString inputFilePath = file.absolutePath();

    //to create QString path of local directory
    //that has to be replaced with original path
    QDir tmpDir(QDir::tempPath());
    QString tmpPath = tmpDir.absoluteFilePath("rerunCompressed");

    //to fetch contents of the rerun.txt file
    // i.e created once detection is completed.
    QDir rerunResultDir(outputFolder);
    QString rerunResultFile = rerunResultDir.absoluteFilePath("rerun.txt");

    //reading the contents of rerun.txt and replacing them with origianl path
    QStringList rerunResultFileContent = m_rerunFormatConverter->readFromFile(rerunResultFile);
    if(rerunResultFileContent.size() > 0)
    {
        for(int i=0;i<rerunResultFileContent.size();i++)
        {
            if(QString(rerunResultFileContent[i]).contains(tmpPath), Qt::CaseSensitive)
            {
                QString updatedPath = QString(rerunResultFileContent[i]).replace(tmpPath, inputFilePath);
                rerunResultFileContent[i] = updatedPath;
            }
        }
        bool ret = m_rerunFormatConverter->writeSanitizedFileList(rerunResultFile.toStdString(), rerunResultFileContent);
        if(ret)
        {
            DEBUG_LOG("rerun.txt is successfully updated at " << rerunResultFile.toStdString())
        }
        else
        {
            ERROR_LOG("Failed to update rerun.txt at " << rerunResultFile.toStdString());
        }
    }
}


/*!
* @fn       initialize
* @param    None
* @return   None
* @brief    Create EmulatorHelper and initialize it.
*/
int ConnectEmulator::initialize()
{
    DEBUG_LOG("start of ConnectEmulator::initialize()");
    int n_ret = -1;
    m_p = new analogic::emulator::EmulatorHelper();
    if(m_p != NULL)
    {
        if((n_ret = m_p->initialize()) == 0)
        {
            // register the this pointer to receive messages from EHL
            DEBUG_LOG("Register the EmulatorHelper pointer to receive messages from EHL ");
            m_p->set_listener(this);
        }
    }
    DEBUG_LOG("End of ConnectEmulator::initialize()");
    return n_ret;
}

/*!
* @fn       handle_algorithms_response
* @param    analogic::emulator::EHL_MSG_Header * p
* @return   None
* @brief     Handle algorithm response messages
*/
void ConnectEmulator::handle_algorithms_response(analogic::emulator::EHL_MSG_Header * p)
{
    analogic::emulator::Rerun_Status_Msg * p_msg = (analogic::emulator::Rerun_Status_Msg * ) p;
    vector<string> new_algoritms;

    DEBUG_LOG("handle_algorithms with type: " << p->m_n_type << "data: "<< p_msg->m_data);
    if(strlen(p_msg->m_data) > 0)
    {
        get_sub_strings(p_msg->m_data, ";", new_algoritms);
        QStringList list;

        if(new_algoritms.size() > 0)
        {
            m_n_current_al_index = 0;
            m_algoritms.clear();

            for(unsigned int i = 0; i < new_algoritms.size(); i++)
            {
                m_algoritms.push_back(new_algoritms.at(i));
                std::string temp = new_algoritms.at(i);
                list.append(QString::fromStdString(temp));
            }
        }
        emit algListResponse(list);
    }
}
/*!
* @fn       handle_rerun_response
* @param    analogic::emulator::EHL_MSG_Header * p
* @return   None
* @brief    Handle rerun response messages
*/
void ConnectEmulator::handle_rerun_response(analogic::emulator::EHL_MSG_Header * p)
{

    analogic::emulator::Rerun_Status_Msg * p_msg = (analogic::emulator::Rerun_Status_Msg * ) p;

    DEBUG_LOG("handle_rerun_response with type: " << p->m_n_type << "data: "<< p_msg->m_data);
    emit rerunResponse(QString::fromStdString(p_msg->m_data),QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);

    if (p_msg->m_n_type == EH_RERUN_IN_PROGRESS_RSP)
    {
        int bagProcessed = p_msg->m_n_bags_files_processed;
        int bagProcessing = p_msg->m_n_bags_files_being_processed;
        float bagProgress = p_msg->m_f_percent_current_bag_sent_to_dcs;
        DEBUG_LOG("rerun command progress processed: " << bagProcessed
                  << "processing : "<< bagProcessing
                  << "current bag progress : " << bagProgress);
        qDebug() << "rerun command progress processed: " << bagProcessed
                 << "processing : "<< bagProcessing
                 << "current bag progress : " << bagProgress;
    }


}
/*!
* @fn       handle_cancel_response
* @param    analogic::emulator::EHL_MSG_Header * p
* @return   None
* @brief    Handle cancel response messages
*/
void ConnectEmulator::handle_cancel_response(analogic::emulator::EHL_MSG_Header * p)
{
    analogic::emulator::Rerun_Status_Msg * p_msg = (analogic::emulator::Rerun_Status_Msg * ) p;

    DEBUG_LOG("handle_cancel_response with type: " << p->m_n_type << "data: "<< p_msg->m_data);
    emit rerunResponse(QString::fromStdString(p_msg->m_data) , QMLEnums::RERUN_RERUN_CANCEL_RSP);
}
/*!
* @fn       handle_reset_response
* @param    analogic::emulator::EHL_MSG_Header * p
* @return   None
* @brief    Handle reset response messages
*/
void ConnectEmulator::handle_reset_response(analogic::emulator::EHL_MSG_Header * p)
{
    analogic::emulator::Rerun_Status_Msg * p_msg = (analogic::emulator::Rerun_Status_Msg * ) p;

    DEBUG_LOG("handle_reset_response with type: " << p->m_n_type << "data: "<< p_msg->m_data);
    emit rerunResponse(QString::fromStdString(p_msg->m_data) ,QMLEnums::RERUN_RESET_RSP);
}

/*!
* @fn       notify
* @param    analogic::emulator::EHL_MSG_Header * p
* @return   None
* @brief    virtual function defined in EmulatorHelperImplListener
*           and implemented by this class to receive messages sent from EHL
*/
void ConnectEmulator::notify( analogic::emulator::EHL_MSG_Header * p)
{
    if(p != NULL)
    {
        int n_type =  p->m_n_type;

        switch (n_type)
        {
        case EH_GET_ALGORITHMS_RSP:
            handle_algorithms_response(p);
            break;
        case EH_RERUN_IN_PROGRESS_RSP:
            handle_rerun_response(p);
            break;
        case EH_RERUN_DONE_RSP:
            handle_rerun_response(p);
            emit rerunResponse(QString::fromStdString("") ,QMLEnums::RERUN_RERUN_DONE_RSP);
            break;
        case EH_RERUN_CANCEL_RSP:
            handle_cancel_response(p);
            break;
        case EH_RESET_RSP:
            handle_reset_response(p);
            break;
        default:
        {
            DEBUG_LOG("Got the unexpected message type: " << n_type);
            emit rerunResponse(QString::fromStdString("Got the unexpected message") , QMLEnums::RERUN_ERROR_RSP);
            break;
        }
        }
        DEBUG_LOG("Deleting the message the message  after processing it to release memory" << p->m_n_type);
        delete p;
    }
}
/*!
 * @fn      get_sub_strings
 * @param   target
 * @param   c_delitmiter
 * @param   v_out
 * @return  bool
 * @brief   Get substring(s) from the given target string using the given c_delitmiter.
 */
bool ConnectEmulator::get_sub_strings (string target, string c_delitmiter, vector<string> & v_out)
{
    if(target.empty())
    {
        DEBUG_LOG("ConnectEmulator:: get_sub_strings Invalid string given");
        return false;
    }
    std::size_t found = target.find_first_of(c_delitmiter);
    int n_pos = 0;

    while (found!=std::string::npos)
    {
        string alg = target.substr(n_pos, found - n_pos);

        if(!alg.empty() && alg.compare("IQP") )
            v_out.push_back(alg);
        n_pos = found+1;
        found=target.find_first_of(c_delitmiter,found+1);
    }

    for(unsigned int i = 0; i < v_out.size(); i++)
    {
        DEBUG_LOG("ConnectEmulator:: get_sub_strings Found substring " << i << " is " << v_out.at(i) );
    }

    return v_out.size() > 0 ? true : false;
}
/*!
* @fn       get_current_alg
* @param    None
* @return   string
* @brief    get algorithm at selected index
*/
string ConnectEmulator::get_current_alg()
{
    string alg = "";
    if(m_algoritms.size() > 0)
    {
        alg = m_algoritms.at(m_n_current_al_index);
        m_n_current_al_index++;
        if(m_n_current_al_index >= m_algoritms.size())
        {
            m_n_current_al_index = 0;
        }
    }
    return alg;
}

/*!
* @fn       start_rerun
* @param    string inputfileList
* @param    string resultFolder
* @param    int algorithIndex
* @return   None
* @brief    Executes rerun task
* @todo     Given two directories containing required bag data files,
*           ie <XXXX_BagID>.vol, <XXXX_BagID>.pro00, <XXXX_BagID>.pro90.
*/
void ConnectEmulator::start_rerun(string inputfileList, string resultFolder, int algorithIndex)
{
    DEBUG_LOG("Executing rerun");
    emit rerunResponse(tr("Executing start rerun.") , QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
    if(m_p != NULL)
    {
        m_p->get_algorithms();
        m_n_current_al_index = algorithIndex;
        string alg = get_current_alg();
        if(!alg.empty())
        {

            DEBUG_LOG("Rerun is using algorithm " << alg );
            DEBUG_LOG("Rerun is using baglist " << inputfileList);
           //  m_p->start_rerun("/run/media/root/public/data/bags/uncompressed/cp/ConneCT00001_20170407_111034/list-ConneCT00001_20170407_111034-20170502072837.txt",
              //                "/run/media/root/public/data/bags/uncompressed/cp/ConneCT00001_20170407_111034","9.00E");
            m_p->start_rerun(inputfileList,resultFolder,alg);


        }
        else
        {
            m_p->get_algorithms();
            DEBUG_LOG("Can't start rerun because the algorithm list is empty." );
            emit rerunResponse(tr("Can't start rerun because the algorithm list is empty")
                               , QMLEnums::RERUN_ERROR_RSP);
        }
    }
    else
    {
        DEBUG_LOG("Invalid Emulator Helper");
        emit rerunResponse(tr("Invalid Emulator Helper"),QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
    }

}

/*!
* @fn       cancel_rerun
* @param    None
* @return   None
* @brief    Cancel rerun task
*/
void ConnectEmulator::cancel_rerun()
{
    if(m_p != NULL)
    {
        m_p->cancel_rerun();
        DEBUG_LOG("Executing cancel");
        emit rerunResponse(tr("Executing cancel"),QMLEnums::RERUN_RERUN_CANCEL_RSP);
    }
    else
    {
        DEBUG_LOG("Invalid Emulator Helper");
        emit rerunResponse(tr("Invalid Emulator Helper"),QMLEnums::RERUN_RERUN_CANCEL_RSP);
    }
}

/*!
* @fn       release_rerun
* @param    None
* @return   None
* @brief    release rerun emulator resources
*/
void ConnectEmulator::release_rerun()
{
    if(m_p != NULL)
    {
        DEBUG_LOG("Executing release rerun");
        m_p->release();
        emit rerunResponse(tr("Executing release rerun"),QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
    }
    else
    {
        DEBUG_LOG("Invalid Emulator Helper");
        emit rerunResponse(tr("Invalid Emulator Helper"),QMLEnums::RERUN_RERUN_IN_PROGRESS_RSP);
    }
}

/*!
* @fn       getAlgorithmsList_rerun
* @param    None
* @return   None
* @brief    get algorithms list from server and save in emulator struct
*/
void ConnectEmulator::getAlgorithmsList_rerun()
{
    if(m_p != NULL)
    {
        DEBUG_LOG("Executing get Algorithm List");
        m_p->get_algorithms();
    }
    else
    {
        DEBUG_LOG("Invalid Emulator Helper");
        emit rerunResponse(tr("Invalid Emulator Helper"),QMLEnums::RERUN_GET_ALGORITHMS_RSP);
    }
}

/*!
* @fn       onExit
* @param    None
* @return   None
* @brief    notifies about exit.
*/
void ConnectEmulator::onExit()
{

}
}
}
