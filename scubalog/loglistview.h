//*****************************************************************************
/*!
  \file ScubaLog/loglistview.h
  \brief This file contains the definition of the LogListView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************


#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include <qwidget.h>

class QListView;
class QPushButton;
class QListViewItem;
class DiveList;
class DiveLog;


//*****************************************************************************
/*!
  \class LogListView
  \brief The LogListView class is used to display all the dive logs.

  A listview is used to view the logs. This widget is the main part of the
  view. Four parts of a dive log is shown in the view:
  \arg Dive number
  \arg Dive date
  \arg Dive start time
  \arg Location.

  The log list can be sorted on any of theese parts.

  \sa QListView.

  \author André Johansen.
*/
//*****************************************************************************

class LogListView : public QWidget {
  Q_OBJECT
public:
  LogListView(QWidget* pcParent, const char* pzName);
  virtual ~LogListView();

  void setLogList(DiveList* pcDiveList);

public slots:
  void createNewLog();
  void deleteLog();
  void viewLog();
  void viewLog(QListViewItem* pcItem);
  void selectedLogChanged(QListViewItem* pcItem);

private:
  //! The dive list view.
  QListView*   m_pcDiveListView;
  //! The button used to create a new log.
  QPushButton* m_pcNewLog;
  //! The button used to delete a log.
  QPushButton* m_pcDeleteLog;
  //! The button used to view a selected log.
  QPushButton* m_pcViewLog;
  //! The current dive log list.
  DiveList*    m_pcDiveLogList;

signals:
  //! This signal is emitted when the log \a pcLog should be displayed.
  void displayLog(DiveLog* pcLog);
  //! This signal is emitted just before the log \a pcLog will be deleted.
  void aboutToDeleteLog(const DiveLog* pcLog);
};


#endif // LOGLISTVIEW_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
