//------------------------------------------------------------------------------
// File: TDRExample.cpp
// Description: Implementation of TDR Example
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------

#include "TunerIPC.h"

namespace analogic
{


namespace workstation
{

char g_serverBuffer[BUFSIZ];


char g_clientBuffer[BUFSIZ];


int g_clientSleepInterval = 2;


std::map< MAP_TUNE_MESSAGE, QString> g_tunerMessageMap =
{
  {LOAD_TRANSLUCENT_COLOR_MAP,   "Load Translucent Color Map"},
  {LOAD_SURFACE_COLOR_MAP,       "Load Surface Color Map"},
  {LOAD_TRANSLUCENT_OPACITY_MAP, "Load Translucent Opacity Map"},
  {LOAD_SURFACE_OPACITY_MAP,     "Load Surface Opacity Map"},
  {REDRAW_SCENE,                 "Redraw Scene"},
};


QStringList g_clientWriteList =
{
 g_tunerMessageMap.at(LOAD_TRANSLUCENT_COLOR_MAP),
 g_tunerMessageMap.at(LOAD_SURFACE_COLOR_MAP),
 g_tunerMessageMap.at(LOAD_TRANSLUCENT_OPACITY_MAP),
 g_tunerMessageMap.at(LOAD_SURFACE_OPACITY_MAP),
 g_tunerMessageMap.at(REDRAW_SCENE),
 g_tunerMessageMap.at(LOAD_TRANSLUCENT_COLOR_MAP),
 g_tunerMessageMap.at(LOAD_SURFACE_COLOR_MAP),
 g_tunerMessageMap.at(LOAD_TRANSLUCENT_OPACITY_MAP),
 g_tunerMessageMap.at(LOAD_SURFACE_OPACITY_MAP),
 g_tunerMessageMap.at(REDRAW_SCENE),
 g_tunerMessageMap.at(LOAD_TRANSLUCENT_COLOR_MAP),
 g_tunerMessageMap.at(LOAD_SURFACE_COLOR_MAP),
 g_tunerMessageMap.at(LOAD_TRANSLUCENT_OPACITY_MAP),
 g_tunerMessageMap.at(LOAD_SURFACE_OPACITY_MAP),
 g_tunerMessageMap.at(REDRAW_SCENE),
 "Exit",
};


//------------------------------------------------------------------------------
TunerIPC::TunerIPC(IPC_COMS_TYPE aComsType):
m_comsType(aComsType),
m_keepRunning(true),
m_fd_client_to_server(-1),
m_fd_server_to_client(-1),
m_processThread(nullptr)
{
}

//------------------------------------------------------------------------------
TunerIPC::~TunerIPC()
{
}

//------------------------------------------------------------------------------
void TunerIPC::Start()
{
  m_processThread = new std::thread(&TunerIPC::threadProc,
                                   this,
                                   g_clientSleepInterval);
  m_processThread->detach();
}


//------------------------------------------------------------------------------
void TunerIPC::threadProc(int waitSeconds)
{
  if ( m_comsType == READER_SERVER )
  {
    //  create the FIFOs (named pipes)
    mkfifo(FIFO_CLIENT_TO_SERVER, 0666);
    mkfifo(FIFO_SERVER_TO_CLIENT, 0666);

    // open, read, and display the message from the FIFO
    m_fd_client_to_server = open(FIFO_CLIENT_TO_SERVER, O_RDONLY);
    m_fd_server_to_client = open(FIFO_SERVER_TO_CLIENT, O_WRONLY);


    while (m_keepRunning)
    {
      read(m_fd_client_to_server, g_serverBuffer, BUFSIZ);

      if (strcmp("Exit", g_serverBuffer) == 0)
      {
        printf("Server OFF.\n");
        qDebug() << "Server OFF!";
        break;
      }

      else if (strcmp("", g_serverBuffer) != 0)
      {
        QString sEnt = g_serverBuffer;
        this->findKeyFromMessage(sEnt);
        QString strRcv = "Recieved:";
        strRcv += sEnt;
        qDebug() << strRcv;

        QString sRet = "Reply: >>>>";
        sRet += sEnt;
        sRet += "<<<<";
        qDebug() << sRet;
        this->findAndNotifyMessage(sEnt);

        write(m_fd_server_to_client,
              sRet.toLocal8Bit().data(),
              sRet.length());
      }

      // Clean up Server buffer of any data.
      memset(g_serverBuffer, 0, sizeof(g_serverBuffer));
    }

  ::close(m_fd_client_to_server);
  ::close(m_fd_server_to_client);

  unlink(FIFO_CLIENT_TO_SERVER);
  unlink(FIFO_SERVER_TO_CLIENT);
  }
  else
  {
    m_fd_client_to_server = open(FIFO_CLIENT_TO_SERVER, O_WRONLY);
    m_fd_server_to_client = open(FIFO_SERVER_TO_CLIENT, O_RDONLY);


    int numStr = g_clientWriteList.size();
    for (int i =0; i < numStr; i++)
    {
      QString strEnt = g_clientWriteList.at(i);
      write(m_fd_client_to_server, strEnt.toLocal8Bit().data(),
            strEnt.length());
      qDebug() <<  strEnt;

      // Verry crude error check.
      perror("Write:");
      memset(g_clientBuffer, 0, sizeof(g_clientBuffer));
      read(m_fd_server_to_client, g_clientBuffer, sizeof(g_clientBuffer));
      QString sResp = "Server resp:";
      sResp += g_clientBuffer;
      qDebug() << sResp;
      this->findAndNotifyMessage(strEnt);
      sleep(waitSeconds);
    }

    ::close(m_fd_client_to_server);
    ::close(m_fd_server_to_client);
  }
}

//------------------------------------------------------------------------------
MAP_TUNE_MESSAGE TunerIPC::findKeyFromMessage(QString& aMessage)
{
  std::map< MAP_TUNE_MESSAGE, QString>::iterator itM;
  MAP_TUNE_MESSAGE retVal = NO_MESSAGE;
  for (itM = g_tunerMessageMap.begin();  itM != g_tunerMessageMap.end(); itM++ )
  {
      MAP_TUNE_MESSAGE key = itM->first;
      QString val = itM->second;
      if (aMessage.compare(val) == 0)
      {
        retVal = key;
        break;
      }
  }
  return retVal;
}

//------------------------------------------------------------------------------
QString TunerIPC::findMessage(MAP_TUNE_MESSAGE key)
{
  QString retVal = g_tunerMessageMap.at(key);
  return retVal;
}

//------------------------------------------------------------------------------
void TunerIPC::findAndNotifyMessage(QString& aMessage)
{
  MAP_TUNE_MESSAGE msgID = this->findKeyFromMessage(aMessage);
  if (msgID != NO_MESSAGE)
  emit this->messageRecieved(msgID);
}



//------------------------------------------------------------------------------
}  //  namespace workstation

}  //  namespace analogic
//------------------------------------------------------------------------------
