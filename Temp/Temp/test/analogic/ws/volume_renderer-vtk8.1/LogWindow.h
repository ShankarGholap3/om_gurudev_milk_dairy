//------------------------------------------------------------------------------
// File: LogWindow.h
// Description: Log Window Header
// (c) Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_LOGWINDOW_H_
#define TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_LOGWINDOW_H_
//------------------------------------------------------------------------------

#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <fstream>
#define CONSOLE_TEXT_FILE_FILTER "Text File (*.txt)"

#include "ConsoleWidget.h"


class LogWindow : public QWidget
{
  Q_OBJECT
 public:
    explicit LogWindow(QIcon& winIcon,
                       QIcon& clearIcon,
                       bool* parentClosed,
                       QWidget* parent);

    virtual ~LogWindow();
    virtual void closeEvent(QCloseEvent* event);
    void logString(const QString& logStr);
    void clear();
    void enableSave(const QString& savePath);

 public slots:
    void onClearConsole();
    void onSafveToFile();

 private:
    void createMenu();
    void createToolBar();
    void saveConsole(QString& saveFile);

    QToolBar*        m_toolBar;
    QAction*         m_clearIconAction;
    QIcon            m_clearIcon;
    QMenuBar*        m_menuBar;
    QMenu*           m_fileMenu;
    QMenu*           m_actionMenu;
    QAction*         m_clearAction;
    QAction*         m_saveAction;
    ConsoleWidget*   m_console;
    QPlainTextEdit*  m_plaintTextEdit;
    QString          m_defaultSavePath;
    bool             m_saveEnabled;
    bool*            m_parentCloseFlag;
};


//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_LOGWINDOW_H_
//------------------------------------------------------------------------------
