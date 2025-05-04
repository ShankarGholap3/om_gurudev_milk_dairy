//------------------------------------------------------------------------------
// File: main.cpp
// Description: API Entry point
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QString bagFilePath = "";
  if (argc == 2)
  {
    // We Have a path to the baglist
    bagFilePath = argv[1];
  }

  MainWindow w(bagFilePath);
  w.show();

  return a.exec();
}

