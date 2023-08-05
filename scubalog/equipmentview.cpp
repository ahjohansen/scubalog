//*****************************************************************************
/*!
  \file equipmentview.cpp
  \brief This file contains the implementation of the EquipmentView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Hübert Johansen
*/
//*****************************************************************************

#include "equipmentview.h"
#include "equipmentlog.h"
#include "logbook.h"
#include "kdateedit.h"
#include "kdatevalidator.h"
#include "dateitem.h"
#include "debug.h"

#include <KLocalizedString>
#include <QHeaderView>
#include <QListWidget>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qsplitter.h>
#include <qwidget.h>
#include <qmenu.h>
#include <assert.h>
#include <stdio.h>

//*****************************************************************************
/*!
  Create the view with \a pcParent as parent widget.
*/
//*****************************************************************************

EquipmentView::EquipmentView(QWidget* pcParent)
  : QWidget(pcParent),
    m_pcLogBook(0),
    m_pcItemView(0),
    m_pcNew(0),
    m_pcDelete(0),
    m_pcMoveUp(0),
    m_pcMoveDown(0),
    m_pcItemName(0),
    m_pcType(0),
    m_pcSerial(0),
    m_pcService(0),
    m_pcLogView(0),
    m_pcLogEntryMenu(0)
{
  QSplitter* pcSplitter =
    new QSplitter(Qt::Vertical, this);

  QWidget* pcTop = new QWidget(pcSplitter);

  m_pcItemView = new QListWidget(pcTop);
  connect(m_pcItemView, SIGNAL(itemSelectionChanged()),
          SLOT(itemSelected()));
  connect(m_pcItemView, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
          SLOT(editItemName(QListWidgetItem*)));

  m_pcNew = new QPushButton(pcTop);
  m_pcNew->setText(i18n("&New"));
  m_pcNew->setEnabled(false);
  connect(m_pcNew, SIGNAL(clicked()), SLOT(newItem()));

  m_pcDelete = new QPushButton(pcTop);
  m_pcDelete->setText(i18n("Delete"));
  m_pcDelete->setEnabled(false);
  connect(m_pcDelete, SIGNAL(clicked()), SLOT(deleteItem()));

  m_pcMoveUp = new QPushButton(pcTop);
  m_pcMoveUp->setText(i18n("Move &up"));
  m_pcMoveUp->setEnabled(false);
  connect(m_pcMoveUp, SIGNAL(clicked()), SLOT(moveCurrentUp()));

  m_pcMoveDown = new QPushButton(pcTop);
  m_pcMoveDown->setText(i18n("Move &down"));
  m_pcMoveDown->setEnabled(false);
  connect(m_pcMoveDown, SIGNAL(clicked()), SLOT(moveCurrentDown()));

  m_pcItemName = new QLineEdit(pcTop);
  m_pcItemName->hide();
  connect(m_pcItemName, SIGNAL(returnPressed()), SLOT(changeItemName()));

  QLabel* pcTypeText = new QLabel(pcTop);
  pcTypeText->setText(i18n("Type: "));

  m_pcType = new QLineEdit(pcTop);
  connect(m_pcType, SIGNAL(textChanged(const QString&)),
          SLOT(itemTypeChanged(const QString&)));

  QLabel* pcSerialText = new QLabel(pcTop);
  pcSerialText->setText(i18n("Serial number: "));

  m_pcSerial = new QLineEdit(pcTop);
  connect(m_pcSerial, SIGNAL(textChanged(const QString&)),
          SLOT(itemSerialChanged(const QString&)));

  QLabel* pcServiceText = new QLabel(pcTop);
  pcServiceText->setText(i18n("Service: "));

  m_pcService = new QLineEdit(pcTop);
  connect(m_pcService, SIGNAL(textChanged(const QString&)),
          SLOT(itemServiceChanged(const QString&)));

  m_pcLogView = new QTableWidget(0, 2, pcSplitter);
  m_pcLogView->setSelectionMode(QTableWidget::SingleSelection);
  m_pcLogView->verticalHeader()->hide();
  QStringList hl;
  hl.append(i18n("Date"));
  hl.append(i18n("Event"));
  m_pcLogView->setHorizontalHeaderLabels(hl);
  connect(m_pcLogView, SIGNAL(itemChanged(QTableWidgetItem*)),
          SLOT(logEntryChanged(QTableWidgetItem*)));
  m_pcLogView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_pcLogView, SIGNAL(customContextMenuRequested(const QPoint&)),
          SLOT(showLogEntryMenu(const QPoint&)));


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

  QBoxLayout* pcSplitLayout = new QVBoxLayout(this);
  pcSplitLayout->addWidget(pcSplitter);
  pcSplitLayout->activate();

  QVBoxLayout* pcTopLayout    = new QVBoxLayout(pcTop);
  QHBoxLayout* pcButtonLayout = new QHBoxLayout();
  QGridLayout* pcInfoLayout   = new QGridLayout();
  pcTopLayout->addWidget(m_pcItemView, 2);
  pcTopLayout->addLayout(pcButtonLayout);
  pcButtonLayout->addWidget(m_pcNew,      0);
  pcButtonLayout->addWidget(m_pcDelete,   0);
  pcButtonLayout->addWidget(m_pcMoveUp,   0);
  pcButtonLayout->addWidget(m_pcMoveDown, 0);
  pcButtonLayout->addWidget(m_pcItemName, 1);
  pcTopLayout->addSpacing(10);
  pcTopLayout->addLayout(pcInfoLayout);
  pcInfoLayout->setColumnStretch(1, 1);
  pcInfoLayout->setColumnStretch(3, 1);
  pcInfoLayout->addWidget(pcTypeText,     0, 0);
  pcInfoLayout->addWidget(m_pcType,       0, 1);
  pcInfoLayout->addWidget(pcSerialText,   0, 2);
  pcInfoLayout->addWidget(m_pcSerial,     0, 3);
  pcInfoLayout->addWidget(pcServiceText,  1, 0);
  pcInfoLayout->addWidget(m_pcService, 1, 1, 1, 3);
  pcTopLayout->activate();
}


//*****************************************************************************
/*!
  Destroy the view.
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
*/
//*****************************************************************************

void
EquipmentView::setLogBook(LogBook* pcLogBook)
{
  m_pcLogBook = pcLogBook;
  if ( pcLogBook ) {
    m_pcItemView->clear();
    m_pcItemView->setUpdatesEnabled(false);
    QList<EquipmentLog*>& cEquipment = pcLogBook->equipmentLog();
    QListIterator<EquipmentLog*> iEquipment(cEquipment);
    while ( iEquipment.hasNext() ) {
      EquipmentLog* pcItem = iEquipment.next();
      m_pcItemView->addItem(new QListWidgetItem(pcItem->name()));
    }
    m_pcItemView->setUpdatesEnabled(true);
    m_pcNew->setEnabled(true);
    unsigned int nNumItems = cEquipment.count();
    bool bItemsExist = nNumItems != 0;
    if ( nNumItems ) {
      m_pcItemView->setCurrentItem(m_pcItemView->item(0));
    }
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
      m_pcLogView->setRowCount(0);
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
    m_pcLogView->setRowCount(0);
  }
}


//*****************************************************************************
/*!
  The item with index \a nItemNumber has been selected.

  Update the rest of the GUI.
*/
//*****************************************************************************

void
EquipmentView::itemSelected()
{
  const int nItemNumber = m_pcItemView->currentRow();
  assert(m_pcLogBook);
  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  if ( cEquipmentLogList.size() <= nItemNumber ) {
    return;
  }
  EquipmentLog* pcLog = cEquipmentLogList.at(nItemNumber);
  assert(pcLog);

  m_pcMoveUp->setEnabled(nItemNumber > 0);
  m_pcMoveDown->setEnabled(nItemNumber < m_pcItemView->count() - 1);
  m_pcType->setText(pcLog->type());
  m_pcSerial->setText(pcLog->serialNumber());
  m_pcService->setText(pcLog->serviceRequirements());

  QList<EquipmentHistoryEntry*>& cHistory = pcLog->history();
  const unsigned int nNumRows = cHistory.count();
  m_pcLogView->setRowCount(nNumRows);
  QList<EquipmentHistoryEntry*>::const_iterator i = cHistory.begin();
  for ( int iRow = 0; i != cHistory.end(); ++i, ++iRow ) {
    const EquipmentHistoryEntry* pcEntry = (*i);
    QString cDateText(pcEntry->date().toString());
    DateItem* pcDateItem = new DateItem(cDateText);
    m_pcLogView->setItem(iRow, 0, pcDateItem);
    m_pcLogView->setItem(iRow, 1, new QTableWidgetItem(pcEntry->comment()));
  }
}


//*****************************************************************************
/*!
  Create a new equipment item.
*/
//*****************************************************************************

void
EquipmentView::newItem()
{
  assert(m_pcLogBook);
  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = new EquipmentLog();
  cEquipmentLogList.append(pcLog);
  m_pcItemView->addItem(new QListWidgetItem());
  m_pcItemView->setCurrentItem(m_pcItemView->item(m_pcItemView->count()-1));
  m_pcItemName->setText("");
  m_pcItemName->show();
  m_pcItemName->setFocus();

  int nCurrent = m_pcItemView->currentRow();

  m_pcDelete->setEnabled(true);
  m_pcMoveUp->setEnabled(nCurrent >= 1);
  m_pcMoveDown->setEnabled(nCurrent < m_pcItemView->count()-1);
  m_pcType->setEnabled(true);
  m_pcSerial->setEnabled(true);
  m_pcService->setEnabled(true);
}


//*****************************************************************************
/*!
  Delete the current equipment item.
*/
//*****************************************************************************

void
EquipmentView::deleteItem()
{
  assert(m_pcLogBook);

  // Find and delete the item
  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  int nCurrentItem = m_pcItemView->currentRow();
  if ( -1 == nCurrentItem )
    return;
  assert(cEquipmentLogList.count() > nCurrentItem);
  delete m_pcItemView->item(nCurrentItem);
  delete cEquipmentLogList.takeAt(nCurrentItem);

  // Update the view with the new current item, if any
  unsigned int nNumItems = m_pcItemView->count();
  int nNewCurrent = m_pcItemView->currentRow();
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
*/
//*****************************************************************************

void
EquipmentView::moveCurrentUp()
{
  assert(m_pcLogBook);
  assert(m_pcItemView->count() >= 2);
  int nCurrentItem = m_pcItemView->currentRow();
  assert(nCurrentItem >= 1 &&
         nCurrentItem < m_pcItemView->count());

  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.takeAt(nCurrentItem);
  cEquipmentLogList.insert(nCurrentItem-1, pcLog);

  QString cName(m_pcItemView->item(nCurrentItem)->text());
  delete m_pcItemView->item(nCurrentItem);
  m_pcItemView->insertItem(nCurrentItem-1, new QListWidgetItem(cName));
  m_pcItemView->setCurrentRow(nCurrentItem-1);

  m_pcMoveUp->setEnabled(nCurrentItem >= 2);
  m_pcMoveDown->setEnabled(true);
}


//*****************************************************************************
/*!
  Move the currently selected item down in the item list.
*/
//*****************************************************************************

void
EquipmentView::moveCurrentDown()
{
  assert(m_pcLogBook);
  assert(m_pcItemView->count() >= 2);
  int nCurrentItem = m_pcItemView->currentRow();
  assert(nCurrentItem >= 0 &&
         nCurrentItem < m_pcItemView->count()-1);

  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.takeAt(nCurrentItem);
  cEquipmentLogList.insert(nCurrentItem+1, pcLog);

  QString cName(m_pcItemView->item(nCurrentItem)->text());
  delete m_pcItemView->item(nCurrentItem);
  m_pcItemView->insertItem(nCurrentItem+1, new QListWidgetItem(cName));
  m_pcItemView->setCurrentRow(nCurrentItem+1);

  m_pcMoveUp->setEnabled(true);
  m_pcMoveDown->setEnabled(nCurrentItem+1 <
                           m_pcItemView->count()-1);
}


//*****************************************************************************
/*!
  Change the name of the current equipment item.
*/
//*****************************************************************************

void
EquipmentView::changeItemName()
{
  assert(m_pcLogBook);
  assert(m_pcItemView->count() >= 1);
  int nCurrentItem = m_pcItemView->currentRow();
  assert(nCurrentItem >= 0);
  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(nCurrentItem);
  QString cName(m_pcItemName->text());
  pcLog->setName(cName);
  m_pcItemName->hide();
  m_pcItemView->item(nCurrentItem)->setText(cName);
  m_pcItemView->setFocus();
}


//*****************************************************************************
/*!
  Edit the name of the item \a nItemNumber.
*/
//*****************************************************************************

void
EquipmentView::editItemName(QListWidgetItem* item)
{
  const int nItemNumber = m_pcItemView->row(item);
  assert(m_pcLogBook);
  assert(m_pcItemView->count() > nItemNumber);
  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(nItemNumber);
  m_pcItemName->setText(pcLog->name());
  m_pcItemName->show();
  m_pcItemName->setFocus();
}


//*****************************************************************************
/*!
  The item type has been changed to \a cType.

  Save the text in the log.
*/
//*****************************************************************************

void
EquipmentView::itemTypeChanged(const QString& cType)
{
  assert(m_pcLogBook);
  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  int i = m_pcItemView->currentRow();
  if ( i >= 0 && i < cEquipmentLogList.count() ) {
    EquipmentLog* pcLog = cEquipmentLogList.at(i);
    if ( pcLog ) {
      pcLog->setType(cType);
    }
  }
}


//*****************************************************************************
/*!
  The item serial number has been changed to \a cSerial.

  Save the text in the log.
*/
//*****************************************************************************

void
EquipmentView::itemSerialChanged(const QString& cSerial)
{
  assert(m_pcLogBook);
  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  int i = m_pcItemView->currentRow();
  if ( i >= 0 && i < cEquipmentLogList.count() ) {
    EquipmentLog* pcLog = cEquipmentLogList.at(i);
    if ( pcLog ) {
      pcLog->setSerialNumber(cSerial);
    }
  }
}


//*****************************************************************************
/*!
  The item service requirements has been changed to \a cService.

  Save the text in the log.
*/
//*****************************************************************************

void
EquipmentView::itemServiceChanged(const QString& cService)
{
  assert(m_pcLogBook);
  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  int i = m_pcItemView->currentRow();
  if ( i >= 0 && i < cEquipmentLogList.count() ) {
    EquipmentLog* pcLog = cEquipmentLogList.at(i);
    if ( pcLog ) {
      pcLog->setServiceRequirements(cService);
    }
  }
}


//*****************************************************************************
/*!
  The log entry in cell containing item \a item has been changed.

  Save the text for later use.
*/
//*****************************************************************************

void
EquipmentView::logEntryChanged(QTableWidgetItem* item)
{
  int nCurrentItem = m_pcItemView->currentRow();
  if ( nCurrentItem < 0 )
    return;

  int nRow = item->row();
  int nCol = item->column();

  QList<EquipmentLog*>& cEquipmentLogList = m_pcLogBook->equipmentLog();
  EquipmentLog* pcLog = cEquipmentLogList.at(nCurrentItem);
  if ( 0 == pcLog )
    return;
  QList<EquipmentHistoryEntry*>& cHistory = pcLog->history();
  while ( cHistory.count() <= nRow )
    cHistory.append(new EquipmentHistoryEntry());
  EquipmentHistoryEntry& cEntry = *cHistory.at(nRow);
  // Convert text to date
  if ( 0 == nCol ) {
    QTableWidgetItem* pcItem = m_pcLogView->item(nRow, nCol);
    assert(pcItem != 0);
    QDate cDate(KDateValidator::convertToDate(pcItem->text()));
    if ( !cDate.isNull() ) {
      cEntry.setDate(cDate);
    }
    else {
      DBG(("EquipmentView::logEntryChanged(): Invalid date \"%s\"\n",
           pcItem->text().toUtf8().constData()));
    }
  }
  else {
    const QString cComment = m_pcLogView->item(nRow, nCol)->text();
    cEntry.setComment(cComment);
  }
}

//*****************************************************************************
/*!
  Create the menu to use in the log entry view.
  This menu is used to insert new entries.

  The created menu is stored in #m_pcLogEntryMenu.

  \sa showLogEntryMenu().
*/
//*****************************************************************************

void
EquipmentView::createLogEntryMenu()
{
  if ( m_pcLogEntryMenu )
    return;

  assert(m_pcLogView);
  m_pcLogEntryMenu = new QMenu(m_pcLogView);
  m_pcLogEntryMenu->addAction(i18n("Insert a &new entry"),
                              this, SLOT(newLogEntry()));
  m_pcLogEntryMenu->addAction(i18n("&Delete the current entry"),
                              this, SLOT(deleteLogEntry()));
}


//*****************************************************************************
/*!
  Show the meny for the entry view.
  The cell that has focus is at the global position is \a i_cPos

  The menu will be created on first use by calling createEntryMenu().
*/
//*****************************************************************************

void
EquipmentView::showLogEntryMenu(const QPoint& i_cPos)
{
  createLogEntryMenu();
  m_pcLogEntryMenu->exec(m_pcLogView->mapToGlobal(i_cPos));
}


//*****************************************************************************
/*!
  Create a new log entry.
  New entries are always inserted at the end.
*/
//*****************************************************************************

void
EquipmentView::newLogEntry()
{
  assert(m_pcLogView);

  const int nNewRow = m_pcLogView->rowCount();

  m_pcLogView->insertRow(nNewRow);
  QString today = QDate::currentDate().toString();
  DateItem* pcDateItem = new DateItem(today);
  m_pcLogView->setItem(nNewRow, 0, pcDateItem);
  m_pcLogView->setCurrentCell(nNewRow, 0);
}


//*****************************************************************************
/*!
  Delete the currently selected log entry.
*/
//*****************************************************************************

void
EquipmentView::deleteLogEntry()
{
  assert(m_pcLogView);
  m_pcLogView->removeRow(m_pcLogView->currentRow());
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
