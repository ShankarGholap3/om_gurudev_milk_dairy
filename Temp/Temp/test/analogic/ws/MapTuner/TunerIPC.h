//------------------------------------------------------------------------------
// File: TunerIPC.h
// Description: Header file for  TunerIPC class
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef  TEST_ANALOGIC_WS_MAPTUNER_TUNERIPC_H_
#define  TEST_ANALOGIC_WS_MAPTUNER_TUNERIPC_H_
//------------------------------------------------------------------------------

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <QString>
#include <QDebug>
#include <string.h>
#include <thread>

#define FIFO_CLIENT_TO_SERVER  "/tmp/client_to_server_fifo"
#define FIFO_SERVER_TO_CLIENT  "/tmp/server_to_client_fifo"

namespace analogic
{


namespace workstation
{

enum MAP_TUNE_MESSAGE
{
  NO_MESSAGE                    =  0,
  LOAD_TRANSLUCENT_COLOR_MAP    =  1,
  LOAD_SURFACE_COLOR_MAP        =  2,
  LOAD_TRANSLUCENT_OPACITY_MAP  =  3,
  LOAD_SURFACE_OPACITY_MAP      =  4,
  REDRAW_SCENE                  =  5,
};

enum IPC_COMS_TYPE
{
  READER_SERVER = 0,
  WIRTER_CLIENT = 1,
};

class TunerIPC : public QObject
{
  Q_OBJECT
public:
    explicit TunerIPC(IPC_COMS_TYPE aComsType);
    ~TunerIPC();
    void Start();
    QString findMessage(MAP_TUNE_MESSAGE key);

signals:
    void messageRecieved(int msg);

private:
    void threadProc(int waitSeconds);
    MAP_TUNE_MESSAGE findKeyFromMessage(QString& aMessage);
    void findAndNotifyMessage(QString& aMessage);

    // vars
    IPC_COMS_TYPE m_comsType;
    bool          m_keepRunning;
    int           m_fd_client_to_server;
    int           m_fd_server_to_client;
    std::thread*  m_processThread;
};


//------------------------------------------------------------------------------
}  // namespace workstation

}  // namespace analogic
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif  //  TEST_ANALOGIC_WS_MAPTUNER_TUNERIPC_H_
//------------------------------------------------------------------------------
