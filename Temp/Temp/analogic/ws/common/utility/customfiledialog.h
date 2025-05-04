/*!
* @file     customfiledialog.h
* @author   Agiliad
* @brief    This file contains changes related to customization
*           file dialog for e.g create file dailog on usb.
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef CUSTOMFILEDIALOG_H
#define CUSTOMFILEDIALOG_H
class QEvent;

#include <QFileDialog>
#include <QString>
#include <analogic/ws/common.h>
#include <analogic/ws/common/utility/usbstorageservice.h>
//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{
/*!
 * \class   CustomFileDialog
 * \brief   This file contains changes related to customization
 *          file dialog for e.g create file dailog on usb.
 */
class CustomFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    /*!
    * @fn       CustomFileDialog
    * @param    None
    * @return   None
    * @brief    Constructor for CustomFileDialog
    */
    explicit CustomFileDialog(QWidget *parent = nullptr,
                              const QString &caption = QString(),
                              const QString &directory = QString(),
                              const QString &filter = QString());

    /*!
    * @fn       eventFilter
    * @param    QObject *o
    * @param    QEvent *e
    * @return   bool
    * @brief    applies event filter for file browser
    */
    bool eventFilter(QObject *o, QEvent *e);

    /*!
    * @fn       setTopDir
    * @param    const QString &path
    * @return   None
    * @brief    set to dir
    */
    void setTopDir(const QString &path);

    /*!
    * @fn       applyBrowseRules
    * @param    None
    * @return   None
    * @brief    applyBrowseRules
    */
    void applyBrowseRules();

    /*!
    * @fn       topDir
    * @param    None
    * @return   QString
    * @brief    get top dir
    */
    QString topDir() const;
    /*!
    * @fn       getCustomFileDialogInstance
    * @param    None
    * @return   CustomFileDialog*
    * @brief    get instance for CustomFileDialog singletone
    */
    static CustomFileDialog* getCustomFileDialogInstance(QWidget *parent = Q_NULLPTR,
                                                         const QString &caption = QString(),
                                                         const QString &directory = QString(),
                                                         const QString &filter = QString());
    /*!
    * @fn       closeFileDialog
    * @param    None
    * @return   None
    * @brief    This function is responsible for release of any resources if req.
    */
    void closeFileDialog();

    /*!
    * @fn       setAcceptMode
    * @param    AcceptMode mode
    * @return   None
    * @brief    This function is responsible for setting accept mode save/open
    */
    void setAcceptMode(AcceptMode mode);
private slots:
    /*!
    * @fn       searchPathTree
    * @param    None
    * @return   None
    * @brief    This slot search path tree till the tree root
    */
    void searchPathTree();
    /*!
    * @fn       validateUpBrowsing
    * @param    None
    * @return   None
    * @brief    This slot validates the up directory browse actions
    */
    void validateUpBrowsing();

    /*!
    * @fn       validateDirectUrl
    * @param    const QString &text
    * @return   None
    * @brief    This slot validates direct url given by input box
    */
    void validateDirectUrl(const QString &text);

    /*!
    * @fn       filterUsedFiles
    * @param    None
    * @return   None
    * @brief    This slot filters used files
    */
    void filterUsedFiles();
private:
    /*!
    * @fn       isValidPathRange
    * @param    const QString &path
    * @return   bool
    * @brief    This function validates the path range from tree
    */
    bool isValidPathRange(const QString &path) const;
    AcceptMode                  m_acceptmode;
    QString                    m_topDir;                        //!< top dir path
    static CustomFileDialog    *m_customFileDInstance;          //!< instance of CustomFileDialog
};

}  // end of namespace ws
}  // end of namespace analogic
#endif // CUSTOMFILEDIALOG_H
