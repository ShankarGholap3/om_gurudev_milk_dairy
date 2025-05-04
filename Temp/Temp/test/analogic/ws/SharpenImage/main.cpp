//------------------------------------------------------------------------------
// File: main.cpp
// Descrioption:  API entry point
// Copyright 2017 Analogic corp.
//------------------------------------------------------------------------------
#include "MainWindow.h"

#include <QApplication>
#include <QVTKOpenGLWidget.h>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}

