/*!
* @file     connectemulator.h
* @author   Agiliad
* @brief    This file contains connect emulator implementing emulatorhelper listener.
*           This class sends command to detection libs and listens for responses.
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/


#ifndef CONNECTEMULATOR_H
#define CONNECTEMULATOR_H

#include <EmulatorHelper.h>
#include <vector>
#include <string>
#include <QObject>
#include <analogic/ws/common.h>
#include <QVariantList>
#include <analogic/ws/rerun/rerunformatconverter.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   ConnectEmulator
 * \brief   This class sends command to detection libs and listens for responses.
 */
class ConnectEmulator : public QObject , public analogic::emulator::EmulatorHelperImplListener
{
    Q_OBJECT

public:

    /*!
   * @fn       ConnectEmulator
   * @param    None
   * @return   None
   * @brief    Constructor for ConnectEmulator
   */
    explicit ConnectEmulator(QObject* parent = NULL);

    /*!
   * @fn       ConnectEmulator
   * @param    None
   * @return   None
   * @brief    copy Constructor for ConnectEmulator
   */
    ConnectEmulator(const ConnectEmulator& orig);

    /*!
   * @fn       ~ConnectEmulator
   * @param    None
   * @return   None
   * @brief    virtual distructor for ConnectEmulator
   */
    virtual ~ConnectEmulator();
    /*!
    * @fn       initialize
    * @param    None
    * @return   None
    * @brief    Create EmulatorHelper and initialize it.
    */
    virtual int initialize(); // Create EmulatorHelper and initialize it.

    /*!
   * @fn       notify
   * @param    analogic::emulator::EHL_MSG_Header * p
   * @return   None
   * @brief    virtual function defined in EmulatorHelperImplListener
   *           and implemented by this class to receive messages sent from EHL
   */
    virtual void notify( analogic::emulator::EHL_MSG_Header * p);
    /*!
   * @fn       get_emulator_helper
   * @param    None
   * @return   analogic::emulator::EmulatorHelper*
   * @brief    get emulator helper object
   */
    virtual analogic::emulator::EmulatorHelper* get_emulator_helper () { return m_p; }

    /*!
   * @fn       start_rerun
   * @param    string inputfileList
   * @param    string resultFolder
   * @param    int algorithIndex
   * @return   None
   * @brief    Executes rerun task
   */
    virtual void start_rerun(string inputfileList , string resultFolder , int algorithIndex);

    /*!
   * @fn       cancel_rerun
   * @param    None
   * @return   None
   * @brief    Cancel rerun task
   */
    virtual void cancel_rerun();
    /*!
   * @fn       release_rerun
   * @param    None
   * @return   None
   * @brief    release rerun emulator resources
   */
    virtual void release_rerun();

    /*!
   * @fn       getAlgorithmsList_rerun
   * @param    None
   * @return   None
   * @brief    get algorithms list from server and save in emulator struct
   */
    virtual void getAlgorithmsList_rerun();

    /*!
    * @fn       checkForFileFormatConversions
    * @param    std::string
    * @return   int
    * @brief    Create EmulatorHelper and initialize it.
    */
    int checkForFileFormatConversions(std::string inputListFileName);

    /*!
       * @fn       copyDataToLocalStorage
       * @param    QString, QString
       * @return   void
       * @brief    to create a local copy of compressed files to decompress it.
       */
    void copyDataToLocalStorage(std::string inputFilename);

    /*!
       * @fn       updateOutputFile
       * @param    std::string , QString
       * @return   none
       * @brief    to updates the path in result file for bag/batch modes.
       */
    void updateOutputFile(QString inputFilename, QString outputFolder);

public slots:

    /*!
   * @fn       onExit
   * @param    None
   * @return   None
   * @brief    notifies about exit.
   */
    virtual void onExit();

signals:
    /*!
   * @fn       exit
   * @param    None
   * @return   None
   * @brief    notifies about exit.
   */
    void exit();
    /*!
   * @fn       rerunResponse
   * @param    QString status
   * @para     QMLEnums::RERUN_RESPONSES
   * @return   None
   * @brief    notifies about status of rerun task.
   */
    void rerunResponse(QString status , QMLEnums::RERUN_RESPONSES);

    /*!
    * @fn       algListResponse
    * @param    QVariant
    * @return   None
    * @brief    signals to notify about algorithlist
    */
    void algListResponse(QStringList list);


private:
    /*!
    * @fn       get_current_alg
    * @param    None
    * @return   string
    * @brief    get algorithm at selected index
    */
    string get_current_alg();
    /*!
    * @fn       handle_algorithms_response
    * @param    analogic::emulator::EHL_MSG_Header * p
    * @return   None
    * @brief    Handle algorithm response messages
    */
    void handle_algorithms_response(analogic::emulator::EHL_MSG_Header * p);
    /*!
    * @fn       handle_rerun_response
    * @param    analogic::emulator::EHL_MSG_Header * p
    * @return   None
    * @brief    Handle rerun response messages
    */
    void handle_rerun_response(analogic::emulator::EHL_MSG_Header * p);
    /*!
    * @fn       handle_cancel_response
    * @param    analogic::emulator::EHL_MSG_Header * p
    * @return   None
    * @brief    Handle cancel response messages
    */
    void handle_cancel_response(analogic::emulator::EHL_MSG_Header * p);
    /*!
    * @fn       handle_reset_response
    * @param    analogic::emulator::EHL_MSG_Header * p
    * @return   None
    * @brief    Handle reset response messages
    */
    void handle_reset_response(analogic::emulator::EHL_MSG_Header * p);

    /*!
     * @fn      get_sub_strings
     * @param   target
     * @param   c_delitmiter
     * @param   v_out
     * @return  bool
     * @brief   Get substring(s) from the given target string using the given c_delitmiter.
     */
    bool get_sub_strings (string target, string c_delitmiter, vector<string> & v_out); // Get substring(s) from the given target string using the given c_delitmiter.

    analogic::emulator::EmulatorHelper *m_p;                   //!< pointer to emulator helper object
    vector<string>                      m_algoritms;           //!< list of available algorithms
    unsigned int                                 m_n_current_al_index;  //!< current selected algorithm index


    RerunFormatConverter *m_rerunFormatConverter;   //!< handle to the Format converter from JpegLs to RAW



};
}
}
#endif /* CONNECTEMULATOR_H */
