//*****************************************************************************
/*!
  \file equipmentview.cpp
  \brief This file contains the implementation of the EquipmentView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  Andr� Johansen.
*/
//*****************************************************************************

#include <assert.h>
#include <qwidget.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <kapp.h>
#include "debug.h"
#include "kdatevalidator.h"
#include "kcelleditview.h"
#include "kdateedit.h"
#include "logbook.h"
#include "equipmentlog.h"
#include "equipmentview.h"


//*****************************************************************************
/*!
  Create the view with \a pcParent as parent widget and \a pzName as
  the widget name.

  \author Andr� Johansen.
*/
//*****************************************************************************

EquipmentView::EquipmentView(QWidget* pcParent, const char* pzName)
  : QWidget(pcParent, pzName),
    m_pcLogBook(0), m_pcLogView(0)
{
  QSplitter* pcSplitter =
    new QSplitter(QSplitter::Vertical, this, "splitter");

  QWidget* pcTop = new QWidget(pcSplitter);

  m_pcItemView = new QListBox(pcTop, "equipmentList");
  connect(m_pcItemView, SIGNAL(highlighted(int)), SLOT(itemSelected(int)));
  connect(m_pcItemView, SIGNAL(selected(int)), SLOT(editItemName(int)));

  m_pcNew = new QPushButton(pcTop, "new");
  m_pcNew->setText(i18n("&New"));
  m_pcNew->setEnabled(false);
  connect(m_pcNew, SIGNAL(clicked()), SLOT(newItem()));

  m_pcDelete = new QPushButton(pcTop, "delete");
  m_pcDelete->setText(i18n("Delete"));
  m_pcDelete->setEnabled(false);
  connect(m_pcDelete, SIGNAL(clicked()), SLOT(deleteItem()));

  m_pcMoveUp = new QPushButton(pcTop, "moveUp");
  m_pcMoveUp->setText(i18n("Move &up"));
  m_pcMoveUp->setEnabled(false);
  connect(m_pcMoveUp, SIGNAL(clicked()), SLOT(moveCurrentUp()));

  m_pcMoveDown = new QPushButton(pcTop, "moveDown");
  m_pcMoveDown->setText(i18n("Move &down"));
  m_pcMoveDown->setEnabled(false);
  connect(m_pcMoveDown, SIGNAL(clicked()), SLOT(moveCurrentDown()));

  m_pcItemName = new QLineEdit(pcTop, "itemName");
  m_pcItemName->hide();
  connect(m_pcItemName, SIGNAL(returnPressed()), SLOT(changeItemName()));

  QLabel* pcTypeText = new QLabel(pcTop, "itemTypeText");
  pcTypeText->setText(i18n("Type: "));

  m_pcType = new QLineEdit(pcTop, "itemType");
  connect(m_pcType, SIGNAL(textChanged(const char*)),
          SLOT(itemTypeChanged(const char*)));

  QLabel* pcSerialText = new QLabel(pcTop, "itemSerialText");
  pcSerialText->setText(i18n("Serial number: "));

  m_pcSerial = new QLineEdit(pcTop, "serial");
  connect(m_pcSerial, SIGNAL(textChanged(const char*)),
          SLOT(itemSerialChanged(const char*)));

  QLabel* pcServiceText = new QLabel(pcTop, "serviceText");
  pcServiceText->setText(i18n("Service: "));

  m_pcService = new QLineEdit(pcTop, "service");
  connect(m_pcService, SIGNAL(textChanged(const char*)),
          SLOT(itemServiceChanged(const char*)));

  m_pcLogView = new KCellEditView(2, pcSplitter, "equipmentLog");
  m_pcLogView->setColEditor(0, new KDateEdit(m_pcLogView, "dateEditor"));
  m_pcLogView->setColEditor(1, new QLineEdit(m_pcLogView, "eventEditor"));
  m_pcLogView->setColumnName(0, i18n("Date"));
  m_pcLogView->setColumnName(1, i18n("Event"));
  connect(m_pcLogView, SIGNAL(textChanged(int, int, const QString&)),
          SLOT(logEntryChanged(int, int, const QString&)));


  //
  // Geometry management
  //

  QSize cButtonSize(m_pcMoveDown->sizeHint());
  m_pcItemName->setMinimumSize(m_pcItemName->sizeHint());
  m_pcNew->setMinimumSize(cButtonSize);
  m_pcDelete->setMinimumSize(cButtonSize);
  m_pcMoveUp->setMinimumSize(cButtonSize);
  m_pcMoveDown->setMinimumSize(cButtonSize);
  pcTypeText->setMinimumSize(pcTypeText->sizeHint());
  m_pcType->setMinimumSize(m_pcType->sizeHint());
  pcSerialText->setMinimumSize(pcSerialText->sizeHint());
  m_pcSerial->setMinimumSize(m_pcSerial->sizeHint());
  pcServiceText->setMinimumSize(pcServiceText->sizeHint());
  m_pcService->setMinimumSize(m_pcService->sizeHint());

  QBoxLayout* pcSplitLayout = new QVBoxLayout(this, 5);
  pcSplitLayout->addWidget(pcSplitter);
  pcSplitLayout->activate();

  QVBoxLayout* pcTopLayout    = new QVBoxLayout(pcTop, 5);
  QHBoxLayout* pcButtonLayout = new QHBoxLayout();
  QGridLayout* pcInfoLayout   = new QGridLayout(2, 4);
  pcTopLayout->addWidget(m_pcItemView, 2);
  pcTopLayout->addLayout(pcButtonLayout);
  pcButtonLayout->addWidget(m_pcNew,      0);
  pcButtonLayout->addWidget(m_pcDelete,   0);
  pcButtonLayout->addWidget(m_pcMoveUp,   0);
  pcButtonLayout->addWidget(m_pcMoveDown, 0);
  pcButtonLayout->addWidget(m_pcItemName, 1);
  pcTopLayout->addSpacing(10);
  pcTopLayout->addLayout(pcInfoLayout);
  pcInfoLayout->setColStretch(1, 1);
  pcInfoLayout->setColStretch(3, 1);
  pcInfoLayout->addWidget(pcTypeText,     0, 0);
  pcInfoLayout->addWidget(m_pcType,       0, 1);
  pcInfoLayout->addWidget(pcSerialText,   0, 2);
  pcInfoLayout->addWidget(m_pcSerial,     0, 3);
  pcInfoLayout->addWidget(pcServiceText,  1, 0);
  pcInfoLayout->addMultiCellWidget(m_pcService, 1, 1, 1, 3);
  pcTopLayout->activate();
}


//*****************************************************************************
/*!
  Destroy the view.

  \author Andr� Johansen.
*/
//*****************************************************************************

EquipmentView::~EquipmentView()
{
}


//*****************************************************************************
/*!
  Set the log book to \a pcLogBook.

  This function will update the view. If a null-pointer is passed,
  the view will be cleared and the widgets will be disabled.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::setLogBook(LogBook* pcLogBook)
{
  m_pcLogBook = pcLogBook;
  if ( pcLogBook ) {
    m_pcItemView->clear();
    m_pcItemView->setAutoUpdate(false);
    QList<EquipmentLog>& cEquipment = pcLogBook->equipmentLog();
    EquipmentLog* pcItem = cEquipment.first();
    while ( pcItem ) {
      m_pcItemView->insertItem(pcItem->name());
      pcItem = cEquipment.next();
    }
    m_pcItemView->setAutoUpdate(true);
    m_pcNew->setEnabled(true);
    unsigned int nNumItems = cEquipment.count();
    bool bItemsExist = nNumItems != 0;
    if ( nNumItems )
      m_pcItemView->setCurrentItem(0);
    m_pcDelete->setEnabled(bItemsExist);
    m_pcMoveUp->setEnabled(false);
    m_pcMoveDown->setEnabled(nNumItems >= 2);
    m_pcType->setEnabled(bItemsExist);
    m_pcSerial->setEnabled(bItemsExist);
    m_pcService->setEnabled(bItemsExist);
    if ( false == bItemsExist ) {
      m_pcType->setText("");
      m_pcSerial->setText("");
      m_pcService->setText("");
      m_pcLogView->clear();
    }
  }
  else {
    m_pcItemView->clear();
    m_pcNew->setEnabled(false);
    m_pcDelete->setEnabled(false);
    m_pcMoveUp->setEnabled(false);
    m_pcMoveDown->setEnabled(false);
    m_pcType->setText("");
    m_pcSerial->setText("");
    m_pcService->setText("");
    m_pcLogView->clear();
  }
}


//*****************************************************************************
/*!
  The item with index \a nItemNumber has been selected.

  Update the rest of the GUI.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::itemSelected(int nItemNumber)
{
  assert(m_pcLogBook);
  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(nItemNumber);
  assert(pcLog);

  m_pcMoveUp->setEnabled(nItemNumber > 0);
  m_pcMoveDown->setEnabled((unsigned)nItemNumber < m_pcItemView->count() - 1);
  m_pcType->setText(pcLog->type());
  m_pcSerial->setText(pcLog->serialNumber());
  m_pcService->setText(pcLog->serviceRequirements());
  m_pcLogView->clear();
  QList<EquipmentHistoryEntry>& cHistory = pcLog->history();
  int iRow = 0;
  for ( EquipmentHistoryEntry* pcEntry = cHistory.first();
        pcEntry; pcEntry = cHistory.next() ) {
    QString cDateText(pcEntry->date().toString());
    m_pcLogView->setCellText(iRow, 0, cDateText, true);
    m_pcLogView->setCellText(iRow, 1, pcEntry->comment(), true);
    iRow++;
  }
}


//*****************************************************************************
/*!
  Create a new equipment item.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::newItem()
{
  assert(m_pcLogBook);
  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = new EquipmentLog();
  cEquipmentLogList.append(pcLog);
  m_pcItemView->insertItem("");
  m_pcItemView->setCurrentItem(m_pcItemView->count()-1);
  m_pcItemName->setText("");
  m_pcItemName->show();
  m_pcItemName->setFocus();

  int nCurrent = m_pcItemView->currentItem();

  m_pcDelete->setEnabled(true);
  m_pcMoveUp->setEnabled(nCurrent >= 1);
  m_pcMoveDown->setEnabled((unsigned int)nCurrent < m_pcItemView->count()-1);
  m_pcType->setEnabled(true);
  m_pcSerial->setEnabled(true);
  m_pcService->setEnabled(true);
}


//*****************************************************************************
/*!
  Delete the current equipment item.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::deleteItem()
{
  assert(m_pcLogBook);

  // Find and delete the item
  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  int nCurrentItem = m_pcItemView->currentItem();
  if ( -1 == nCurrentItem )
    return;
  assert(cEquipmentLogList.count() > (unsigned)nCurrentItem);
  m_pcItemView->removeItem(nCurrentItem);
  EquipmentLog* pcLog = cEquipmentLogList.take(nCurrentItem);
  delete pcLog;

  // Update the view with the new current item, if any
  unsigned int nNumItems = m_pcItemView->count();
  int nNewCurrent = m_pcItemView->currentItem();
  if ( -1 == nNewCurrent ) {
    m_pcDelete->setEnabled(false);
    m_pcMoveUp->setEnabled(nNewCurrent >= 1);
    m_pcMoveDown->setEnabled(nNewCurrent < (int)nNumItems-1);
    m_pcType->setEnabled(false);
    m_pcSerial->setEnabled(false);
    m_pcService->setEnabled(false);
  }
}


//*****************************************************************************
/*!
  Move the currently selected item up in the item list.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::moveCurrentUp()
{
  assert(m_pcLogBook);
  assert(m_pcItemView->count() >= 2);
  int nCurrentItem = m_pcItemView->currentItem();
  assert(nCurrentItem >= 1 &&
         (unsigned)nCurrentItem < m_pcItemView->count());

  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.take(nCurrentItem);
  cEquipmentLogList.insert(nCurrentItem-1, pcLog);

  QString cName(m_pcItemView->text(nCurrentItem));
  m_pcItemView->removeItem(nCurrentItem);
  m_pcItemView->insertItem(cName, nCurrentItem-1);
  m_pcItemView->setCurrentItem(nCurrentItem-1);
  if ( m_pcItemView->topItem() > nCurrentItem-1 )
    m_pcItemView->centerCurrentItem();

  m_pcMoveUp->setEnabled(nCurrentItem >= 2);
  m_pcMoveDown->setEnabled(true);
}


//*****************************************************************************
/*!
  Move the currently selected item down in the item list.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::moveCurrentDown()
{
  assert(m_pcLogBook);
  assert(m_pcItemView->count() >= 2);
  int nCurrentItem = m_pcItemView->currentItem();
  assert(nCurrentItem >= 0 &&
         (unsigned)nCurrentItem < m_pcItemView->count()-1);

  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.take(nCurrentItem);
  cEquipmentLogList.insert(nCurrentItem+1, pcLog);

  QString cName(m_pcItemView->text(nCurrentItem));
  m_pcItemView->removeItem(nCurrentItem);
  m_pcItemView->insertItem(cName, nCurrentItem+1);
  m_pcItemView->setCurrentItem(nCurrentItem+1);
  if ( m_pcItemView->topItem()+m_pcItemView->numItemsVisible() <=
       nCurrentItem+1 )
    m_pcItemView->centerCurrentItem();

  m_pcMoveUp->setEnabled(true);
  m_pcMoveDown->setEnabled((unsigned int)nCurrentItem+1 <
                           m_pcItemView->count()-1);
}


//*****************************************************************************
/*!
  Change the name of the current equipemnt item.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::changeItemName()
{
  assert(m_pcLogBook);
  assert(m_pcItemView->count() >= 1);
  int nCurrentItem = m_pcItemView->currentItem();
  assert(nCurrentItem >= 0);
  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(nCurrentItem);
  QString cName(m_pcItemName->text());
  pcLog->setName(cName);
  m_pcItemName->hide();
  m_pcItemView->changeItem(cName, nCurrentItem);
  m_pcItemView->setFocus();
}


//*****************************************************************************
/*!
  Edit the name of the item \a nItemNumber.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::editItemName(int nItemNumber)
{
  assert(m_pcLogBook);
  assert(m_pcItemView->count() > (unsigned)nItemNumber);
  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(nItemNumber);
  m_pcItemName->setText(pcLog->name());
  m_pcItemName->show();
  m_pcItemName->setFocus();
}


//*****************************************************************************
/*!
  The item type has been changed to \a pzType.

  Save the text in the log.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::itemTypeChanged(const char* pzType)
{
  assert(m_pcLogBook);
  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(m_pcItemView->currentItem());
  if ( pcLog ) {
    QString cType(pzType);
    pcLog->setType(cType);
  }
}


//*****************************************************************************
/*!
  The item serial number has been changed to \a pzSerial.

  Save the text in the log.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::itemSerialChanged(const char* pzSerial)
{
  assert(m_pcLogBook);
  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(m_pcItemView->currentItem());
  if ( pcLog ) {
    QString cSerial(pzSerial);
    pcLog->setSerialNumber(cSerial);
  }
}


//*****************************************************************************
/*!
  The item service requirements has been changed to \a pzService.

  Save the text in the log.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::itemServiceChanged(const char* pzService)
{
  assert(m_pcLogBook);
  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(m_pcItemView->currentItem());
  if ( pcLog ) {
    QString cServiceRequirements(pzService);
    pcLog->setServiceRequirements(cServiceRequirements);
  }
}


//*****************************************************************************
/*!
  The log entry in cell (\a nRow, \a nCol) has been changed to \a cText.

  Save the text for later use.

  \author Andr� Johansen.
*/
//*****************************************************************************

void
EquipmentView::logEntryChanged(int nRow, int nCol, const QString& cText)
{
  assert(nRow >= 0 && (0 == nCol || 1 == nCol));
  int nCurrentItem = m_pcItemView->currentItem();
  if ( nCurrentItem < 0 )
    return;

  QList<EquipmentLog>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(nCurrentItem);
  if ( 0 == pcLog )
    return;
  QList<EquipmentHistoryEntry>& cHistory = pcLog->history();
  while ( cHistory.count() <= (unsigned)nRow )
    cHistory.append(new EquipmentHistoryEntry());
  EquipmentHistoryEntry& cEntry = *cHistory.at(nRow);
  // Convert text to date
  if ( 0 == nCol ) {
    QDate cDate(KDateValidator::convertToDate(cText));
    if ( false == cDate.isNull() )
      cEntry.setDate(cDate);
    else
      DBG(("EquipmentView::logEntryChanged(): Invalid date `%s'\n",
           cText.data()));
  }
  else
    cEntry.setComment(cText);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
