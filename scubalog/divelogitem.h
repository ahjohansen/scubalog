//*****************************************************************************
/*!
  \file divelogitem.h
  \brief This file contains the definition of the DiveLogItem class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef DIVELOGITEM_H
#define DIVELOGITEM_H

#include <qlistview.h>

class DiveLog;
class QString;

//*****************************************************************************
/*!
  \class DiveLogItem
  \brief The DiveLogItem is used to represent a item in the divelog listview.

  By using this class, the listview will automatically be updated whenever
  a dive log changes.

  Each item holds a pointer to the log used. The item must be destroyed
  before a log is destroyed!

  The text used for dive number, date and time are stored in each
  object; for the location, the shared data approach by QString is used.

  \author André Johansen.
*/
//*****************************************************************************

class DiveLogItem : public QListViewItem {
public:
  DiveLogItem(QListView* pcListView, QListViewItem* pcPrevious,
              DiveLog* pcLog);
  virtual ~DiveLogItem();
  virtual QString text(int nColumn) const;
  virtual QString key(int nColumn, bool isAscending) const;

  DiveLog* log() const { return m_pcLog; }

protected:
  //! The log this item represents.
  DiveLog* m_pcLog;
  //! The text used to represent the dive number.
  QString* m_pcDiveNumber;
  //! The text used to represent the dive date.
  QString* m_pcDiveDate;
  //! The text used to represent the dive start time.
  QString* m_pcDiveStart;
};

#endif // DIVELOGITEM_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
