//*****************************************************************************
/*!
  \file loglistview.cpp
  \brief This file contains the implementation of the LogListView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "loglistview.h"
#include "divelogitem.h"
#include "divelist.h"
#include "debug.h"

#include <klocale.h>
#include <kapplication.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <q3listview.h>
#include <q3frame.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <new>
#include <assert.h>

//*****************************************************************************
/*!
  Create the log list view with \a pcParent as parent widget.
*/
//*****************************************************************************

LogListView::LogListView(QWidget* pcParent)
  : QWidget(pcParent),
    m_pcDiveListView(0),
    m_pcNewLog(0),
    m_pcDeleteLog(0),
    m_pcViewLog(0),
    m_pcDiveLogList(0)
{
  m_pcDiveListView = new Q3ListView(this, "dives");
  m_pcDiveListView->setFrameStyle(Q3Frame::WinPanel|Q3Frame::Sunken);
  m_pcDiveListView->addColumn(i18n("Dive"));
  m_pcDiveListView->addColumn(i18n("Date"));
  m_pcDiveListView->addColumn(i18n("Time"));
  m_pcDiveListView->addColumn(i18n("Location"));
  m_pcDiveListView->setColumnAlignment(0, Qt::AlignRight);
  m_pcDiveListView->setColumnWidthMode(3, Q3ListView::Maximum);
  m_pcDiveListView->setAllColumnsShowFocus(true);
  m_pcDiveListView->setSorting(0);
  connect(m_pcDiveListView, SIGNAL(doubleClicked(Q3ListViewItem*)),
          SLOT(viewLog(Q3ListViewItem*)));
  connect(m_pcDiveListView, SIGNAL(returnPressed(Q3ListViewItem*)),
          SLOT(viewLog(Q3ListViewItem*)));
  connect(m_pcDiveListView, SIGNAL(selectionChanged(Q3ListViewItem*)),
          SLOT(selectedLogChanged(Q3ListViewItem*)));

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
  pcDLVTopLayout->addWidget(m_pcDiveListView, 10);
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
  assert(m_pcDiveListView);

  m_pcDiveListView->clear();
  m_pcDiveLogList = pcDiveList;
  if ( pcDiveList ) {
    DiveLogItem* pcPreviousItem = 0;
    QListIterator<DiveLog*> iLog(*pcDiveList);
    while ( iLog.hasNext() ) {
      DiveLog* pcLog = iLog.next();
      DiveLogItem* pcItem =
        new DiveLogItem(m_pcDiveListView, pcPreviousItem, pcLog);
      pcPreviousItem = pcItem;
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

  DiveLogItem* pcLastItem =
    dynamic_cast<DiveLogItem*>(m_pcDiveListView->firstChild());
  while ( pcLastItem && pcLastItem->nextSibling() )
    pcLastItem = dynamic_cast<DiveLogItem*>(pcLastItem->nextSibling());
  int nDiveNumber = 1;
  if ( pcLastItem )
    nDiveNumber = pcLastItem->log()->logNumber() + 1;

  DiveLog* pcLog = 0;
  try {
    pcLog = new DiveLog();
    pcLog->setLogNumber(nDiveNumber);
    m_pcDiveLogList->append(pcLog);
    (void)new DiveLogItem(m_pcDiveListView, pcLastItem, pcLog);
    emit displayLog(pcLog);
  }
  catch ( std::bad_alloc& ) {
    // In case the divelogitem causes OOM, delete the log to be sure...
    delete pcLog;
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] New dive log"),
                         i18n("Out of memory when creating a new dive log!"));
  }
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
  DiveLogItem* pcItem =
    dynamic_cast<DiveLogItem*>(m_pcDiveListView->currentItem());
  if ( pcItem ) {
    DiveLog* pcLog = pcItem->log();
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
      delete pcItem;
      emit aboutToDeleteLog(pcLog);
      int i = m_pcDiveLogList->indexOf(pcLog);
      assert(i != -1);
      delete m_pcDiveLogList->takeAt(i);
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
  DiveLogItem* pcCurrentItem =
    dynamic_cast<DiveLogItem*>(m_pcDiveListView->currentItem());
  if ( pcCurrentItem )
    emit displayLog(pcCurrentItem->log());
}


//*****************************************************************************
/*!
  Display the log corresponding to \a pcItem.
*/
//*****************************************************************************

void
LogListView::viewLog(Q3ListViewItem* pcItem)
{
  DiveLogItem* pcLogItem = dynamic_cast<DiveLogItem*>(pcItem);
  assert(pcLogItem);
  emit displayLog(pcLogItem->log());
}


//*****************************************************************************
/*!
  The current selected log has changed to \a pcItem, update the GUI.
*/
//*****************************************************************************

void
LogListView::selectedLogChanged(Q3ListViewItem* pcItem)
{
  assert(m_pcDeleteLog);
  assert(m_pcViewLog);
  m_pcDeleteLog->setEnabled(pcItem ? true : false);
  m_pcViewLog->setEnabled(pcItem ? true : false);
}



// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
