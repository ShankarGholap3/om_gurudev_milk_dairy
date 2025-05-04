//------------------------------------------------------------------------------
// File: main_writer.cpp
// Description:  API Entery Point fopr Writer
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------
#include "MainWindow_writer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}


