//*****************************************************************************
/*!
  \file ScubaLog/divelogitem.cpp
  \brief This file contains the implementation of the DiveLogItem class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <assert.h>
#include <qstring.h>
#include "debug.h"
#include "divelog.h"
#include "divelogitem.h"


//*****************************************************************************
/*!
  Create an item in the listview \a pcListView, inserting it after
  the item \a pcPrevious using and \a pcLog as the log to be displayed.
*/
//*****************************************************************************

DiveLogItem::DiveLogItem(QListView* pcListView, QListViewItem* pcPrevious,
                         DiveLog* pcLog)
  : QListViewItem(pcListView, pcPrevious),
    m_pcLog(pcLog), m_pcDiveNumber(0), m_pcDiveDate(0), m_pcDiveStart(0)
{
  assert(pcLog);

  m_pcDiveNumber = new QString;
  m_pcDiveDate   = new QString;
  m_pcDiveStart  = new QString;

  QString cNumber;
  cNumber.sprintf("%d", pcLog->logNumber());
  setText(0, cNumber);
  setText(1, pcLog->diveDate().toString());
  setText(2, pcLog->diveStart().toString());
  setText(3, pcLog->diveLocation());
}


//*****************************************************************************
/*!
  Destroy the item.
  All resources are freed.

  Since the log is not owned by this object, only used, it is not
  destroyed.
*/
//*****************************************************************************

DiveLogItem::~DiveLogItem()
{
  delete m_pcDiveNumber;
  delete m_pcDiveDate;
  delete m_pcDiveStart;
}


//*****************************************************************************
/*!
  Get the sort key for column \a nColumn.
  \a isAscending indicated whether the sorting should be in ascending
  order or not (not used int this function).

  The returned key is saved in a static variable, due to poor design of the
  QListViewItem::key() interface in Qt 1.
*/
//*****************************************************************************

#if QT_VERSION >= 200
QString
#else  // Qt 1
const char*
#endif // QT_VERSION
DiveLogItem::key(int nColumn, bool/* isAscending*/) const
{
#if QT_VERSION >= 200
  QString cKey;
#else  // Qt 1
  static QString cKey;
#endif // QT_VERSION
  if ( 0 == nColumn )
    cKey.sprintf("%05d", m_pcLog->logNumber());
  else if ( 1 == nColumn ) {
    QDateTime cStart(QDate(1950, 1, 1));
    cKey.sprintf("%06d", cStart.daysTo(m_pcLog->diveDate()));
  }
  else
    cKey = QString(text(nColumn));
  DBG(("The sorting key for column %d is `%s'\n", nColumn, cKey.data()));
  return cKey;
}


//*****************************************************************************
/*!
  Get the text to be used in column \a nColumn, counting from 0.

  The text is copied to the internal strings if needed.
*/
//*****************************************************************************

#if QT_VERSION >= 200
QString
#else  // Qt 1
const char*
#endif // QT_VERSION
DiveLogItem::text(int nColumn) const
{
  if ( 0 == nColumn ) {
    m_pcDiveNumber->sprintf("%d", m_pcLog->logNumber());
    return m_pcDiveNumber->data();
  }
  else if ( 1 == nColumn ) {
    *m_pcDiveDate = m_pcLog->diveDate().toString();
    return m_pcDiveDate->data();
  }
  else if ( 2 == nColumn ) {
    *m_pcDiveStart = m_pcLog->diveStart().toString();
    return m_pcDiveStart->data();
  }
  else if ( 3 == nColumn ) {
    return m_pcLog->diveLocation();
  }
  return 0;                     // Unknown column
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
