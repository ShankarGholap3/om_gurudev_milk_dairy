#ifndef LINUXTERMINAL_H
#define LINUXTERMINAL_H

#include <QObject>
#include <QProcess>

class LinuxTerminal : public QObject
{
  Q_OBJECT
public:
  explicit LinuxTerminal(QObject *parent = 0);

  /*!
  * @fn       launchXFCETerminal
  * @param    bool mode
  * @return   void
  * @brief    launch linux xfce or gnome terminal
  */
  void launchXFCETerminal(bool mode);

  /*!
  * @fn       exitXFCETerminal
  * @param    void
  * @return   void
  * @brief    Exit linux xfce or gnome terminal
  */
  void exitXFCETerminal();

signals:

public slots:

private:
  QProcess *m_termProcess;
  qint64 m_procPID;
  bool m_isTerminalLaunched;
  QString m_execStart;
};

#endif // LINUXTERMINAL_H
