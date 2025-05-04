#include "linuxterminal.h"
#include <QThread>

LinuxTerminal::LinuxTerminal(QObject *parent) : QObject(parent)
{
  m_termProcess = new QProcess();
  m_isTerminalLaunched = false;
  m_execStart = "xfce4-terminal --geometry=100x47+540+100 --title OS_Access --hide-menubar --hide-borders";
  //  m_execStart = "gnome-terminal --geometry=114x42+526+84 --title \"OS_Access\" ";

}

/*!
* @fn       launchXFCETerminal
* @param    bool mode
* @return   void
* @brief    launch linux xfce or gnome terminal
*/
void LinuxTerminal::launchXFCETerminal(bool mode)
{
  if (mode == true)
  {
    if (m_isTerminalLaunched == false)
    {
      if (m_termProcess)
      {
        // QString exec = "xfce4-terminal --geometry=100x39+542+92 --title OS_Access"; // --color-bg=white --color-text=red";
        m_termProcess->start(m_execStart);
        m_procPID = m_termProcess->processId();

        m_isTerminalLaunched = true;
      }
    }
    else if (m_termProcess->processId() == 0)
    {
      m_termProcess->start(m_execStart);
      m_procPID = m_termProcess->processId();

      m_isTerminalLaunched = true;
    }
  }
  else
  {
    if (m_isTerminalLaunched == true && m_termProcess->processId() != 0)
    {
      if (m_termProcess)
      {
        QThread::usleep(100);
        QString exec1 = "wmctrl -a OS_Access";
        bool revokeState = QProcess::startDetached(exec1);
        if(revokeState == false)
        {
          m_termProcess->execute("kill " + QString::number(m_procPID));
          m_termProcess->close();
          QThread::usleep(100);
          m_termProcess->start(m_execStart);
          m_procPID = m_termProcess->processId();
          m_isTerminalLaunched = true;
        }
      }
    }
  }
}

/*!
* @fn       exitXFCETerminal
* @param    void
* @return   void
* @brief    Exit linux xfce or gnome terminal
*/
void LinuxTerminal::exitXFCETerminal()
{
  if (m_isTerminalLaunched  == true)
  {
    if (m_termProcess)
    {
      m_termProcess->execute("kill " + QString::number(m_procPID));
      m_termProcess->kill();
      m_termProcess->close();
    }
  }
  m_isTerminalLaunched = false;
}
