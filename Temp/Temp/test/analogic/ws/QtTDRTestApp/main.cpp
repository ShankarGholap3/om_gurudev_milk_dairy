//------------------------------------------------------------------------------
// File: main.cpp
// Description:  API Entery Point.
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------
#include "MainWindow.h"
#include <QApplication>

std::string g_logINIFile = "/home/analogic/devIni/QtTDRTestApp.ini";


MY_LOGGER_CREATE;

int main(int argc, char *argv[])
{
  MY_LOGGER.init(g_logINIFile);
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}


