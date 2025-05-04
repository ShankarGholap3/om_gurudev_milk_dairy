//------------------------------------------------------------------------------
// File: main_reader.cpp
// Description:  API Entery Point for Reader
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------
#include "MainWindow_reader.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}


