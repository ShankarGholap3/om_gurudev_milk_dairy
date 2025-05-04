//------------------------------------------------------------------------------
// File: BagQueueItem.h
// Description: Bag Queue Item  header
// (c) Copyright 2017 - Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_BAGQUEUEITEM_H_
#define TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_BAGQUEUEITEM_H_
//------------------------------------------------------------------------------


#include <QString>
#include <list>

class BagQueueItem
{
public:
  BagQueueItem():
    m_volumeFileName("")
  {
  }

  ~BagQueueItem()
  {
  }

  void setValues(QString volumeFile,
            std::list<QString>& tdrList)
  {
    m_volumeFileName = volumeFile;
    std::list<QString>::iterator it;
    for (it = tdrList.begin(); it != tdrList.end(); it++ )
    {
      QString strI = *it;
      m_tdrList.push_back(strI);
    }
  }

  QString            m_volumeFileName;
  std::list<QString> m_tdrList;
};


//------------------------------------------------------------------------------
#endif  // TEST_ANALOGIC_WS_VOLUME_RENDERER_GUI_BAGQUEUEITEM_H_
//------------------------------------------------------------------------------
