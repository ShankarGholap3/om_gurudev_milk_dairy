//------------------------------------------------------------------------------
// File: SelectFromListDialog.cpp
// Description:  Impleimentatrion of dialog for List select input.
// (c) Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------
#include "SelectFromListDialog.h"

#define SELECT_ITEM_DIALOG_TITLE  "Selelect item from list"
#define SELECT_ITEM_DIALOG_HEADER "Select"

#define SELECT_ITEM_DIALOG_WIDTH  250
#define SELECT_ITEM_DIALOG_HEIGHT 300

//------------------------------------------------------------------------------
SelectFromListDialog::SelectFromListDialog(QStringList& selectList,
                                           QWidget *parent,
                                           Qt::WindowFlags flags):
  QDialog(parent, flags),
  m_labelHeader(nullptr),
  m_selectedItem(-1),
  m_signalMapper(nullptr)
{
  QVBoxLayout*  mainLayout   = new QVBoxLayout();
  this->setLayout(mainLayout);
  m_labelHeader = new QLabel(this);
  mainLayout->addWidget(m_labelHeader);
  this->setTitleAndHeader(SELECT_ITEM_DIALOG_TITLE,
                          SELECT_ITEM_DIALOG_HEADER);


  for (int i=0; i < selectList.size(); i++)
  {
    QString buttonName = selectList.at(i);
    QPushButton* listButton = new QPushButton(buttonName,
                                              this);
    m_buttonMap[i] = listButton;
    mainLayout->addWidget(listButton);
  }

  m_signalMapper = new QSignalMapper(this);
  std::map<int, QPushButton* >::iterator iter;

  for (iter = m_buttonMap.begin();  iter != m_buttonMap.end(); iter++)
  {
    QPushButton* pBut = iter->second;
    int val = iter->first;
    this->connect(pBut, &QPushButton::clicked,
                  m_signalMapper,
                  static_cast<void (QSignalMapper::*)(void)>(&QSignalMapper::map) );

    m_signalMapper->setMapping(pBut, val);
  }

  this->connect(m_signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped),
                this, &SelectFromListDialog::handleButtonPress);

  this->setMinimumWidth(SELECT_ITEM_DIALOG_WIDTH);
}

//------------------------------------------------------------------------------
SelectFromListDialog::~SelectFromListDialog()
{
}

//------------------------------------------------------------------------------
QSize SelectFromListDialog::sizeHint()
{
   return QSize(SELECT_ITEM_DIALOG_WIDTH,
                SELECT_ITEM_DIALOG_HEIGHT);
}

//------------------------------------------------------------------------------
void SelectFromListDialog::handleButtonPress(int aSelection)
{
  m_selectedItem = aSelection;
  this->accept();
}

//------------------------------------------------------------------------------
void SelectFromListDialog::setTitleAndHeader(QString title, QString header)
{
  m_labelHeader->setText(header);
  this->setWindowTitle(title);
}


//------------------------------------------------------------------------------
int SelectFromListDialog::getSelected()
{
  return m_selectedItem;
}
