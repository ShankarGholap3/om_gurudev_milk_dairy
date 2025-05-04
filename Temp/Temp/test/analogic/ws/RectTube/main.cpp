//------------------------------------------------------------------------------
// File: main.cpp
// Description:  API Entry for Qt-VTK application.
// Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------
#include "MainWindow.h"
#include <QDebug>
#include <QApplication>

int main( int argc, char* argv[] )
{
  QApplication app( argc, argv );
  MainWindow mainWin;
  mainWin.show();
  return app.exec();
}
