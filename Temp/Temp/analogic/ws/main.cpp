/*!
* @file     main.cpp
* @author   Agiliad
* @brief    This file contains functions related to initializing the appcontroller.
* @date     Sept, 10 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <QLabel>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QTranslator>
#include <analogic/ws/appcontroller.h>
#include <analogic/ws/wsconfiguration/workstationconfig.h>
#include <qopenglcontext.h>
#include <analogic/ws/uihandler/application.h>
using analogic::ws::WorkstationConfig;
using analogic::ws::AppController;


MY_LOGGER_CREATE

void qmlMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  switch (type)
  {
  case QtDebugMsg:
    DEBUG_LOG(QString(context.file).toStdString() << " -line-"
              << QString::number(context.line).toStdString()
              << " " << QString(context.function).toStdString()
              << " " << msg.toStdString());
    break;
  case QtWarningMsg:
    WARN_LOG(msg.toStdString());
    break;
  case QtCriticalMsg:
    break;
  case QtFatalMsg:
    FATAL_LOG(QString(context.file).toStdString() << " -line-"
              << QString::number(context.line).toStdString()
              << " " << QString(context.function).toStdString()
              << " " << msg.toStdString());
    break;
  case QtInfoMsg:
    INFO_LOG(msg.toStdString());
    break;
  }
}

/*!
* @fn       main(int argc, char *argv[])
* @param    argc - command line argument count
* @param    argv - command line arguments
* @return   None
* @brief    Responsible for initialization of AppController.
*/


int main(int argc, char *argv[])
{
  //---------------------------------------------------------------'
  // ARO-TEST FOR KEYBOARD FOCUS
  // QSurfaceFormat defaultFormat = QVTKOpenGLWidget::defaultFormat();
  // defaultFormat.setSamples(0);
  // defaultFormat.setStencilBufferSize(8);
  // QSurfaceFormat::setDefaultFormat(defaultFormat);
  // std::cout << "Setting QSurfaceFormat to QVTKOpenGLWidget::defaultFormat()" << std::endl;
  //---------------------------------------------------------------'

  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_NativeWindows);
  Errors::init();
  try
  {
    // Initialize QML enums
    QMLEnums::init();

    // initalize config handler
    DEBUG_LOG("Initialize config handler");

    QMLEnums::WSType wsType = WorkstationConfig::getInstance()->
        getWorkstationNameEnum();
    QString version = "";
    if ((wsType == QMLEnums::WSType::OSR_WORKSTATION) ||
        (wsType == QMLEnums::WSType::SEARCH_WORKSTATION) ||
        (wsType == QMLEnums::WSType::SUPERVISOR_WORKSTATION) ||
        (wsType == QMLEnums::WSType::TRAINING_WORKSTATION))
    {
      if(wsType == QMLEnums::WSType::TRAINING_WORKSTATION)
      {
        version = QString("TRAINING Workstation VERSION:") + RERUN_EMULATOR_VERSION;
      }
      else
      {
        version = QString("Workstation Client VERSION:") + WORKSTATION_VERSION;
      }
      //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
      //qputenv("QT_VIRTUALKEYBOARD_STYLE", QByteArray("analogic_vkb_style"));
    }
    else if((wsType == QMLEnums::WSType::RERUN_EMULATOR))
    {
      version = QString("Rerun Emulator VERSION:") + RERUN_EMULATOR_VERSION;
    }

    qInstallMessageHandler(qmlMessageOutput);

    // initalize GUI application
    Application app(argc, argv);

#ifdef WORKSTATION
    MY_LOGGER.init(WorkstationConfig::getInstance()->getexecutablePath() + "/" + INI_FILE_NAME);
#endif
#ifdef RERUN
    MY_LOGGER.init(WorkstationConfig::getInstance()->getexecutablePath() + "/" + RERUN_INI_FILE_NAME);
#endif

    INFO_LOG( "================================================");
    INFO_LOG( "========| " << version.toStdString() << " |=============");
    INFO_LOG( "================================================");

    INFO_LOG("Initializing/Loading Language translator");


    QString language = WorkstationConfig::getInstance()->getCurrentLanuage();

    QTranslator qtTranslator;
    QTranslator translator;
    if(language == "French")
    {
      qtTranslator.load("qt_fr", QApplication::applicationDirPath() + "/translations");
      translator.load(QApplication::applicationDirPath() + "/language/translating-qml_fr.qm");
    }
    else if(language == "German")
    {
      qtTranslator.load("qt_de", QApplication::applicationDirPath() + "/translations");
      translator.load(QApplication::applicationDirPath() + "/language/translating-qml_ge.qm");
    }
    else if(language == "Italian")
    {
      qtTranslator.load("qt_it", QApplication::applicationDirPath() + "/translations");
      translator.load(QApplication::applicationDirPath() + "/language/translating-qml_it.qm");
    }
    else if(language == "Spanish")
    {
      qtTranslator.load("qt_es", QApplication::applicationDirPath() + "/translations");
      translator.load(QApplication::applicationDirPath() + "/language/translating-qml_sp.qm");
    }
    else if(language == "Japanese")
    {
      qtTranslator.load("qt_ja", QApplication::applicationDirPath() + "/translations");
      translator.load(QApplication::applicationDirPath() + "/language/translating-qml_jp.qm");
    }
    else if(language == "Turkish")
    {
      qtTranslator.load("qt_tr", QApplication::applicationDirPath() + "/translations");
      translator.load(QApplication::applicationDirPath() + "/language/translating-qml_tr.qm");
    }
    else
    {
      qtTranslator.load("qt_en", QApplication::applicationDirPath() + "/translations");
      translator.load(QApplication::applicationDirPath() + "/language/translating-qml_en.qm");
    }
    app.installTranslator(&qtTranslator);
    app.installTranslator(&translator);

    INFO_LOG("Initialize GUI application");
    AppController appCtrl;
    appCtrl.initialize();

    return app.exec();
  }
  catch(Errors::RESULT& hr)
  {
    ERROR_LOG(Errors::geterrorstring(hr));
  }
  catch(QException &e)
  {
    ERROR_LOG("Exception occured. Exiting from binary");
    ERROR_LOG(e.what());
  }
  catch(...)
  {
    ERROR_LOG("Unknown Exception occured. Exiting from binary");
  }
}
