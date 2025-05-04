//------------------------------------------------------------------------------
// File: main.cpp
// Description:  API entry point
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include "MainWindow.h"
#include <QApplication>


std::string  g_logfile = "/home/analogic/devIni/qt_dicos_testapp.ini";

MY_LOGGER_CREATE;


//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  MY_LOGGER.init(g_logfile);
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
