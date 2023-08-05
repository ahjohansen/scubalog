//*****************************************************************************
/*!
  \file loglistview.cpp
  \brief This file contains the implementation of the LogListView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Hübert Johansen
*/
//*****************************************************************************

#include "loglistview.h"
#include "divelist.h"
#include "debug.h"

#include <KLocalizedString>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <new>
#include <assert.h>


//*****************************************************************************
/*!
  Create the log list view with \a pcParent as parent widget.
*/
//*****************************************************************************

LogListView::LogListView(QWidget* pcParent)
  : QWidget(pcParent),
    m_pcDiveListWidget(0),
    m_pcNewLog(0),
    m_pcDeleteLog(0),
    m_pcViewLog(0),
    m_pcDiveLogList(0)
{
  m_pcDiveListWidget = new QTableWidget(0, 4, this);
  m_pcDiveListWidget->setSelectionMode(QTableWidget::SingleSelection);
  m_pcDiveListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_pcDiveListWidget->verticalHeader()->hide();
  QTableWidgetItem* h;
  h = new QTableWidgetItem(i18n("Dive"));
  m_pcDiveListWidget->setHorizontalHeaderItem(0, h);
  h = new QTableWidgetItem(i18n("Date"));
  m_pcDiveListWidget->setHorizontalHeaderItem(1, h);
  h = new QTableWidgetItem(i18n("Time"));
  m_pcDiveListWidget->setHorizontalHeaderItem(2, h);
  h = new QTableWidgetItem(i18n("Location"));
  m_pcDiveListWidget->setHorizontalHeaderItem(3, h);

  QHeaderView* header = m_pcDiveListWidget->horizontalHeader();
  header->setStretchLastSection(true);
  //m_pcDiveListWidget->setSortingEnabled(true);
  //m_pcDiveListWidget->sortByColumn(0, Qt::AscendingOrder);
  connect(m_pcDiveListWidget, SIGNAL(cellDoubleClicked(int, int)),
          SLOT(viewLog(int, int)));
  connect(m_pcDiveListWidget, SIGNAL(cellActivated(int, int)),
          SLOT(selectedLogChanged(int, int)));

  m_pcNewLog = new QPushButton(this);
  m_pcNewLog->setText(i18n("&New log entry"));
  m_pcNewLog->setMinimumSize(m_pcNewLog->sizeHint());
  connect(m_pcNewLog, SIGNAL(clicked()), SLOT(createNewLog()));

  m_pcDeleteLog = new QPushButton(this);
  m_pcDeleteLog->setText(i18n("&Delete log"));
  m_pcDeleteLog->setMinimumSize(m_pcDeleteLog->sizeHint());
  m_pcDeleteLog->setEnabled(false);
  connect(m_pcDeleteLog, SIGNAL(clicked()), SLOT(deleteLog()));

  m_pcViewLog = new QPushButton(this);
  m_pcViewLog->setText(i18n("&View log"));
  m_pcViewLog->setMinimumSize(m_pcViewLog->sizeHint());
  m_pcViewLog->setEnabled(false);
  connect(m_pcViewLog, SIGNAL(clicked()), SLOT(viewLog()));

  QVBoxLayout* pcDLVTopLayout = new QVBoxLayout(this);
  pcDLVTopLayout->addWidget(m_pcDiveListWidget, 10);
  QHBoxLayout* pcDLVButtonLayout = new QHBoxLayout();
  pcDLVTopLayout->addLayout(pcDLVButtonLayout);
  pcDLVButtonLayout->addWidget(m_pcNewLog);
  pcDLVButtonLayout->addWidget(m_pcDeleteLog);
  pcDLVButtonLayout->addStretch(10);
  pcDLVButtonLayout->addWidget(m_pcViewLog);
  pcDLVTopLayout->activate();
}


//*****************************************************************************
/*!
  Destroy the log list view.
*/
//*****************************************************************************

LogListView::~LogListView()
{
}


/**
 * Insert the dive log \a log at \a row in the view.
 */

void
LogListView::insertLogAt(int row, const DiveLog* log)
{
  QTableWidgetItem* diveItem = new QTableWidgetItem();
  diveItem->setData(Qt::DisplayRole, log->logNumber());
  QTableWidgetItem* dateItem = new QTableWidgetItem();
  dateItem->setData(Qt::DisplayRole, log->diveDate());
  QTableWidgetItem* timeItem = new QTableWidgetItem();
  timeItem->setData(Qt::DisplayRole, log->diveStart());
  QTableWidgetItem* locationItem = new QTableWidgetItem();
  locationItem->setData(Qt::DisplayRole, log->diveLocation());
  if ( row >= m_pcDiveListWidget->rowCount() ) {
    m_pcDiveListWidget->setRowCount(row+1);
  }
  m_pcDiveListWidget->setItem(row, 0, diveItem);
  m_pcDiveListWidget->setItem(row, 1, dateItem);
  m_pcDiveListWidget->setItem(row, 2, timeItem);
  m_pcDiveListWidget->setItem(row, 3, locationItem);
}


//*****************************************************************************
/*!
  Use \a pcDiveList as the current dive log list.
  If a 0-pointer is passed, the view will be cleared, and no editing will
  be possible.

  The log list view will be cleared, and all the logs from the new list
  will be inserted in the view.

  Notice that this class does not take ownership of the list itself,
  but it can insert and delete logs from it.
*/
//*****************************************************************************

void
LogListView::setLogList(DiveList* pcDiveList)
{
  assert(m_pcDiveListWidget);

  m_pcDiveListWidget->clearContents();
  m_pcDiveLogList = pcDiveList;
  if ( pcDiveList ) {
    int row = 0;
    QListIterator<DiveLog*> iLog(*pcDiveList);
    while ( iLog.hasNext() ) {
      DiveLog* pcLog = iLog.next();
      insertLogAt(row, pcLog);
      ++row;
    }
  }
}


//*****************************************************************************
/*!
  Create a new log entry. The log view will be displayed.

  The log number will be the last logs number plus one, or 1 if this is the
  first log.
*/
//*****************************************************************************

void
LogListView::createNewLog()
{
  assert(m_pcDiveLogList);

  int nDiveNumber = 1;
  const int rows = m_pcDiveListWidget->rowCount();
  if ( rows ) {
    QTableWidgetItem* pcLastItem = m_pcDiveListWidget->item(rows-1, 0);
    nDiveNumber += pcLastItem->text().toInt();
  }

  DiveLog* pcLog = 0;
  try {
    pcLog = new DiveLog();
    pcLog->setLogNumber(nDiveNumber);
    m_pcDiveLogList->append(pcLog);
    insertLogAt(rows, pcLog);
    emit displayLog(pcLog);
  }
  catch ( std::bad_alloc& ) {
    // In case of OOM, delete the log to be sure...
    delete pcLog;
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] New dive log"),
                         i18n("Out of memory when creating a new dive log!"));
  }
}


/**
 * Get the dive with log number \a number from the dive log list.
 * Returns the dive log if found, else NULL.
 */

DiveLog* LogListView::getDiveLogNumber(int number)
{
  assert(m_pcDiveLogList != 0);
  for ( int i = 0; i < m_pcDiveLogList->size(); ++i ) {
    DiveLog* log = m_pcDiveLogList->at(i);
    if ( log->logNumber() == number ) {
      return log;
    }
  }
  return NULL;
}


//*****************************************************************************
/*!
  Delete the current log if one is selected.
  Just before the log is deleted, the signal aboutToDeleteLog() will be
  emitted.

  The log wil be deleted from the log list.
*/
//*****************************************************************************

void
LogListView::deleteLog()
{
  assert(m_pcDiveLogList);
  int row = m_pcDiveListWidget->currentRow();
  const QTableWidgetItem* item = m_pcDiveListWidget->item(row, 0);
  if ( item ) {
    DiveLog* pcLog = getDiveLogNumber(item->data(Qt::DisplayRole).toInt());
    DBG(("About to delete dive log #%d\n", pcLog->logNumber()));
    QString cMessage =
      QString(i18n("Are you sure you want to delete log %1?\n"
                   "(location: '%2')"))
      .arg(pcLog->logNumber())
      .arg(QString(pcLog->diveLocation().data()));
    int nResult = QMessageBox::information(QApplication::topLevelWidgets().at(0),
                                           i18n("[ScubaLog] Delete log"),
                                           cMessage,
                                           i18n("&Yes"), i18n("&No"));
    if ( 0 == nResult ) {
      emit aboutToDeleteLog(pcLog);
      m_pcDiveListWidget->removeRow(row);
      m_pcDiveLogList->removeAll(pcLog);
      delete pcLog;
      DBG(("Deleted dive log...\n"));
    }
  }
}


//*****************************************************************************
/*!
  Display the currently selected log, if any.
*/
//*****************************************************************************

void
LogListView::viewLog()
{
  assert(m_pcDiveLogList);
  int row = m_pcDiveListWidget->currentRow();
  const QTableWidgetItem* item = m_pcDiveListWidget->item(row, 0);
  if ( item ) {
    DiveLog* log = getDiveLogNumber(item->data(Qt::DisplayRole).toInt());
    if ( log ) {
      emit displayLog(log);
    }
  }
}


//*****************************************************************************
/*!
  Display the log corresponding to \a pcItem.
*/
//*****************************************************************************

void
LogListView::viewLog(int row, int col)
{
  int logs = m_pcDiveLogList->count();
  if ( row < logs ) {
    DiveLog* log = m_pcDiveLogList->at(row);
    emit displayLog(log);
  }
}


//*****************************************************************************
/*!
  The current selected log has changed to \a pcItem, update the GUI.
*/
//*****************************************************************************

void
LogListView::selectedLogChanged(int row, int col)
{
  assert(m_pcDeleteLog);
  assert(m_pcViewLog);
  m_pcDeleteLog->setEnabled(true);
  m_pcViewLog->setEnabled(true);
}



// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
