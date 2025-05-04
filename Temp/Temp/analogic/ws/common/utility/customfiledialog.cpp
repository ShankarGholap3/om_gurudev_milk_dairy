/*!
* @file     customfiledialog.cpp
* @author   Agiliad
* @brief    This file contains changes related to customization
*           file dialog for e.g create file dailog on usb.
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include "customfiledialog.h"

#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QList>
#include <QToolButton>
#include <QDir>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QEvent>
#include <QKeyEvent>
#include <QAbstractButton>
#include <QCompleter>
#include <QAbstractItemView>
#include <QFileInfo>
#include <QDebug>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

CustomFileDialog* CustomFileDialog::m_customFileDInstance = 0;

/*!
* @fn       CustomFileDialog
* @param    None
* @return   None
* @brief    Constructor for CustomFileDialog
*/
CustomFileDialog::CustomFileDialog(QWidget *parent,
                                   const QString &caption,
                                   const QString &directory,
                                   const QString &filter) :
    QFileDialog(parent, caption, directory, filter)
{
    m_customFileDInstance = NULL;
    connect(this, SIGNAL(directoryEntered(QString)), this, SLOT(searchPathTree()));
    connect(this, SIGNAL(directoryEntered(QString)), this, SLOT(filterUsedFiles()));
    connect(this, SIGNAL(directoryEntered(QString)), this, SLOT(validateUpBrowsing()));
    connect(findChild<QToolButton *>("backButton"), SIGNAL(clicked()), this, SLOT(validateUpBrowsing()));
    connect(findChild<QToolButton *>("forwardButton"), SIGNAL(clicked()), this, SLOT(validateUpBrowsing()));
    connect(findChild<QLineEdit *>("fileNameEdit"), SIGNAL(textChanged(QString)), this, SLOT(validateDirectUrl(QString)));
    findChild<QLineEdit *>("fileNameEdit")->installEventFilter(this);
    findChild<QWidget *>("listView")->installEventFilter(this);
    findChild<QWidget *>("treeView")->installEventFilter(this);
    findChild<QLineEdit *>("fileNameEdit")->completer()->popup()->installEventFilter(this);
    setOption(DontUseNativeDialog, true);


}

/*!
* @fn       applyBrowseRules
* @param    None
* @return   None
* @brief    applyBrowseRules
*/
void CustomFileDialog::applyBrowseRules()
{

}

/*!
* @fn       filterUsedFiles
* @param    None
* @return   None
* @brief    This slot filters used files
*/
void CustomFileDialog::filterUsedFiles()
{
    // CustomFileDialog::ExistingFiles
}

/*!
* @fn       eventFilter
* @param    QObject *o
* @param    QEvent *e
* @return   bool
* @brief    applies event filter for file browser
*/
bool CustomFileDialog::eventFilter(QObject *o, QEvent *e)
{
    bool value;
    if (e != NULL && o != NULL) {
    if (e->type() != QEvent::KeyPress)
        return false;
    int key = static_cast<QKeyEvent *>(e)->key();
    if (o->objectName() == "listView" || o->objectName() == "treeView")
    {
        value = (Qt::Key_Backspace == key && !isValidPathRange(directory().absolutePath()));
    }
    else
    {
        if (Qt::Key_Return != key && Qt::Key_Enter != key)
            return false;
        QString text = findChild<QLineEdit *>("fileNameEdit")->text();
        QString path = QDir::cleanPath(directory().absolutePath() + (text.startsWith("/") ? "" : "/") + text);
        bool a = QDir(text).isAbsolute();
        value = !((!a && isValidPathRange(path)) || (a && isValidPathRange(text)));
    }
    }
    return value;
}

/*!
* @fn       setTopDir
* @param    const QString &path
* @return   None
* @brief    set to dir
*/
void CustomFileDialog::setTopDir(const QString &path)
{
    if(path.isEmpty())
    {
      m_topDir =  QDir::home().path();
    }
    if (path == m_topDir)
    {
        setDirectory(m_topDir);
        searchPathTree();
        return;
    }
    if ((!path.isEmpty() && QFileInfo(path).isDir()))
    {
        m_topDir =  path;
    }
    else {
        m_topDir =  QDir::home().path();
    }
    if (!isValidPathRange(path))
    {
        setDirectory(m_topDir);
        searchPathTree();
        validateDirectUrl(findChild<QLineEdit *>("fileNameEdit")->text());
    }
    else
    {
        QLineEdit *ledt = findChild<QLineEdit *>("fileNameEdit");
        ledt->setText(ledt->text());
    }
    findChild<QWidget *>("lookInCombo")->setEnabled(m_topDir.isEmpty());
    findChild<QWidget *>("sidebar")->setEnabled(m_topDir.isEmpty());
    validateUpBrowsing();
}

/*!
* @fn       topDir
* @param    None
* @return   QString
* @brief    get top dir
*/
QString CustomFileDialog::topDir() const
{
    return m_topDir;
}

/*!
* @fn       isValidPathRange
* @param    const QString &path
* @return   bool
* @brief    This function validates the path range from tree
*/
bool CustomFileDialog::isValidPathRange(const QString &path) const
{
  if((fileMode() == DirectoryOnly) ||(fileMode() == Directory))
  {
    return (dirExists(path))&& (m_topDir.isEmpty() || (path.startsWith(m_topDir) && path.length() > m_topDir.length()));
  }
  else if(fileMode() == ExistingFile)
  {
    QString temppath;
    QFileInfo fileinfo(path);
    temppath = fileinfo.path();
    return (dirExists(temppath) && fileExists(path))&& (m_topDir.isEmpty() || (path.startsWith(m_topDir) && path.length() > m_topDir.length()));
  }
  else if(fileMode() == ExistingFiles)
  {
    QString temppath;
    QFileInfo fileinfo(path);
    temppath = fileinfo.path();
    return (dirExists(temppath)) && (m_topDir.isEmpty() || (path.startsWith(m_topDir) && path.length() > m_topDir.length()));
  }
  else if(fileMode() == AnyFile)
  {
    QString temppath;
    QFileInfo fileinfo(path);
    temppath = fileinfo.path();
    return (dirExists(temppath)) && (m_topDir.isEmpty() || (path.startsWith(m_topDir) && path.length() > m_topDir.length()));
  }
  return false;
}


/*!
* @fn       searchPathTree
* @param    None
* @return   None
* @brief    This slot search path tree till the tree root
*/
void CustomFileDialog::searchPathTree()
{
    QStringList folderTree = history();
    for (int i = folderTree.size() - 1; i >= 0; --i)
    {
        if (!isValidPathRange(folderTree.at(i)))
            folderTree.removeAt(i);
    }
    setHistory(folderTree);
}

/*!
* @fn       validateUpBrowsing
* @param    None
* @return   None
* @brief    This slot validates the up directory browse actions
*/
void CustomFileDialog::validateUpBrowsing()
{
    bool isVal = isValidPathRange(directory().absolutePath());
    if (isVal == false)
    {
        setDirectory(m_topDir);
    }
    findChild<QToolButton *>("toParentButton")->setEnabled(isVal);
}

/*!
* @fn       validateDirectUrl
* @param    const QString &text
* @return   None
* @brief    This slot validates direct url given by input box
*/
void CustomFileDialog::validateDirectUrl(const QString &text)
{
    QAbstractButton *buttonClicked = findChild<QDialogButtonBox *>("buttonBox")->buttons().first();
    QString currentPath = QDir::cleanPath(directory().absolutePath() + (text.startsWith("/") ? "" : "/") + text);
    bool isAbsolute = QDir(text).isAbsolute();
    buttonClicked->setEnabled(buttonClicked->isEnabled() && ((!isAbsolute && isValidPathRange(currentPath)) || (isAbsolute && isValidPathRange(text))));
}

/*!
* @fn       setAcceptMode
* @param    AcceptMode mode
* @return   None
* @brief    This function is responsible for setting accept mode save/open
*/
void CustomFileDialog::setAcceptMode(AcceptMode mode)
{
  m_acceptmode = mode;
  QFileDialog::setAcceptMode(mode);
}

/*!
* @fn       getCustomFileDialogInstance
* @param    None
* @return   CustomFileDialog*
* @brief    get instance for CustomFileDialog singletone
*/
CustomFileDialog* CustomFileDialog::getCustomFileDialogInstance(QWidget *parent,
                                                                const QString &caption,
                                                                const QString &directory,
                                                                const QString &filter)
{
    INFO_LOG("Creating CustomFileDialog Instance");
    m_customFileDInstance = new CustomFileDialog(parent, caption, directory, filter);
    m_customFileDInstance->applyBrowseRules();
    std::string root_file_path = "";
    root_file_path = UsbStorageSrv::getUsbfilePath();
#ifdef RERUN
        if (root_file_path.empty() || !(RerunConfig::isRemovableStorage()))
            root_file_path = WorkstationConfig::getInstance()->getDefaultDataPath().toStdString();
#endif
     m_customFileDInstance->setTopDir(QString::fromStdString(root_file_path));
    return m_customFileDInstance;

}

/*!
* @fn       closeFileDialog
* @param    None
* @return   None
* @brief    This function is responsible for release of any resources if req.
*/
void CustomFileDialog::closeFileDialog()
{
    m_customFileDInstance->close();
    m_customFileDInstance->dumpObjectInfo();
    m_customFileDInstance->dumpObjectTree();
    SAFE_DELETE_LATER(m_customFileDInstance)
    /*
    findChild<QLineEdit *>("fileNameEdit")->removeEventFilter(this);
    findChild<QWidget *>("listView")->removeEventFilter(this);
    findChild<QWidget *>("treeView")->removeEventFilter(this);
    findChild<QLineEdit *>("fileNameEdit")->completer()->popup()->removeEventFilter(this);
    disconnect(this, SIGNAL(directoryEntered(QString)), this, SLOT(searchPathTree()));
    disconnect(this, SIGNAL(directoryEntered(QString)), this, SLOT(filterUsedFiles()));
    disconnect(this, SIGNAL(directoryEntered(QString)), this, SLOT(validateUpBrowsing()));
    disconnect(findChild<QToolButton *>("backButton"), SIGNAL(clicked()), this, SLOT(validateUpBrowsing()));
    disconnect(findChild<QToolButton *>("forwardButton"), SIGNAL(clicked()), this, SLOT(validateUpBrowsing()));
    disconnect(findChild<QLineEdit *>("fileNameEdit"), SIGNAL(textChanged(QString)), this, SLOT(validateInputBoxEntry(QString)));
*/

    // SAFE_DELETE(m_customFileDInstance);
    // m_customFileDialogInstanceFlag = false;
}
}  // end of namespace ws
}  // end of namespace analogic
