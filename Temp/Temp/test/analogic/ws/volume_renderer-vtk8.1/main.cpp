//------------------------------------------------------------------------------
// File: main.cpp
// Description: API Entrypoint of Qt Application for  testing integrated
//   Volume Rendereing
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
#include <QApplication>
#include <QDebug>
#include <string>
#include "MainWindow.h"
#include "VRUtils.h"

MY_LOGGER_CREATE;

int main(int argc, char *argv[])
{
  std::string resourceFolder;
  analogic::workstation::getResourcesFolder(resourceFolder);
  std::string logIniFile = resourceFolder + VOLUME_RENDERER_GUI_INI_FILE;
  MY_LOGGER.init(logIniFile);
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
