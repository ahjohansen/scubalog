//*****************************************************************************
/*!
  \file ScubaLog/loglistview.cpp
  \brief This file contains the implementation of the LogListView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <assert.h>
#include <new>
#include <qframe.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include "debug.h"
#include "divelist.h"
#include "divelogitem.h"
#include "loglistview.h"



//*****************************************************************************
/*!
  Create the log list view with \a pcParent as parent widget and \a pzName
  as widget name.

  \author André Johansen.
*/
//*****************************************************************************

LogListView::LogListView(QWidget* pcParent, const char* pzName)
  : QWidget(pcParent, pzName)
{
  m_pcDiveListView = new QListView(this, "dives");
  m_pcDiveListView->setFrameStyle(QFrame::WinPanel|QFrame::Sunken);
  m_pcDiveListView->addColumn("Dive");
  m_pcDiveListView->addColumn("Date");
  m_pcDiveListView->addColumn("Time");
  m_pcDiveListView->addColumn("Location");
  m_pcDiveListView->setColumnAlignment(0, AlignRight);
  m_pcDiveListView->setColumnWidthMode(3, QListView::Maximum);
  m_pcDiveListView->setAllColumnsShowFocus(true);
  m_pcDiveListView->setSorting(0);
  connect(m_pcDiveListView, SIGNAL(doubleClicked(QListViewItem*)),
          SLOT(viewLog(QListViewItem*)));
  connect(m_pcDiveListView, SIGNAL(returnPressed(QListViewItem*)),
          SLOT(viewLog(QListViewItem*)));
  connect(m_pcDiveListView, SIGNAL(selectionChanged(QListViewItem*)),
          SLOT(selectedLogChanged(QListViewItem*)));

  m_pcNewLog = new QPushButton(this, "new");
  m_pcNewLog->setText("&New log entry");
  m_pcNewLog->setMinimumSize(m_pcNewLog->sizeHint());
  connect(m_pcNewLog, SIGNAL(clicked()), SLOT(createNewLog()));

  m_pcDeleteLog = new QPushButton(this, "delete");
  m_pcDeleteLog->setText("&Delete log");
  m_pcDeleteLog->setMinimumSize(m_pcDeleteLog->sizeHint());
  m_pcDeleteLog->setEnabled(false);
  connect(m_pcDeleteLog, SIGNAL(clicked()), SLOT(deleteLog()));

  m_pcViewLog = new QPushButton(this, "view");
  m_pcViewLog->setText("&View log");
  m_pcViewLog->setMinimumSize(m_pcViewLog->sizeHint());
  m_pcViewLog->setEnabled(false);
  connect(m_pcViewLog, SIGNAL(clicked()), SLOT(viewLog()));

  QVBoxLayout* pcDLVTopLayout = new QVBoxLayout(this, 5);
  pcDLVTopLayout->addWidget(m_pcDiveListView, 10);
  QHBoxLayout* pcDLVButtonLayout = new QHBoxLayout();
  pcDLVTopLayout->addLayout(pcDLVButtonLayout);
  pcDLVButtonLayout->addWidget(m_pcNewLog);
  pcDLVButtonLayout->addWidget(m_pcDeleteLog);
  pcDLVButtonLayout->addStretch(10);
  pcDLVButtonLayout->addWidget(m_pcViewLog);
  pcDLVTopLayout->activate();

  m_pcDiveLogList = 0;
}


//*****************************************************************************
/*!
  Destroy the log list view.

  \author André Johansen.
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

  \author André Johansen.
*/
//*****************************************************************************

void
LogListView::setLogList(DiveList* pcDiveList)
{
  assert(m_pcDiveListView);

  m_pcDiveListView->clear();
  m_pcDiveLogList = pcDiveList;
  if ( pcDiveList ) {
    DiveLog* pcLog = 0;
    DiveLogItem* pcPreviousItem = 0;
    for (  pcLog = pcDiveList->first(); pcLog; pcLog = pcDiveList->next() ) {
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

  \author André Johansen.
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
  catch ( std::bad_alloc ) {
    // In case the divelogitem causes OOM, delete the log to be sure...
    delete pcLog;
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] New dive log",
                         "Out of memory when creating a new dive log!");
  }
}


//*****************************************************************************
/*!
  Delete the current log if one is selected.
  Just before the log is deleted, the signal aboutToDeleteLog() will be
  emitted.

  The log wil be deleted from the log list.

  \author André Johansen.
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
    QString cMessage;
    cMessage.sprintf("Are you sure you want to delete log %d?\n"
                     "(location: `%s')", pcLog->logNumber(),
                     pcLog->diveLocation().data());
    int nResult = QMessageBox::information(qApp->mainWidget(),
                                           "[ScubaLog] Delete log",
                                           cMessage, "&Yes", "&No");
    if ( 0 == nResult ) {
      delete pcItem;
      emit aboutToDeleteLog(pcLog);
      m_pcDiveLogList->remove(pcLog); // This will delete the log too!
      DBG(("Deleted dive log...\n"));
    }
  }
}


//*****************************************************************************
/*!
  Display the currently selected log, if any.

  \author André Johansen.
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

  \author André Johansen.
*/
//*****************************************************************************

void
LogListView::viewLog(QListViewItem* pcItem)
{
  DiveLogItem* pcLogItem = dynamic_cast<DiveLogItem*>(pcItem);
  assert(pcLogItem);
  emit displayLog(pcLogItem->log());
}


//*****************************************************************************
/*!
  The current selected log has changed to \a pcItem, update the GUI.

  \author André Johansen.
*/
//*****************************************************************************

void
LogListView::selectedLogChanged(QListViewItem* pcItem)
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
// End:
