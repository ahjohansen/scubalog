//*****************************************************************************
/*!
  \file loglistview.h
  \brief This file contains the definition of the LogListView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Hübert Johansen
*/
//*****************************************************************************

#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include <qwidget.h>

class QTableWidget;
class QTableWidgetItem;
class QPushButton;
class DiveList;
class DiveLog;


//*****************************************************************************
/*!
  \class LogListView
  \brief The LogListView class is used to display all the dive logs.

  A table widget is used to view the logs. This widget is the main part of the
  view. Four parts of a dive log is shown in the view:
  \arg Dive number
  \arg Dive date
  \arg Dive start time
  \arg Location.

  The log list can be sorted on any of these parts.

  \author André Hübert Johansen
*/
//*****************************************************************************

class LogListView : public QWidget {
  Q_OBJECT
public:
  LogListView(QWidget* pcParent);
  virtual ~LogListView();

  void setLogList(DiveList* pcDiveList);

public slots:
  void createNewLog();
  void deleteLog();
  void viewLog();
  void viewLog(int row, int col);
  void selectedLogChanged(int row, int col);

private:
  void insertLogAt(int row, const DiveLog* item);
  DiveLog* getDiveLogNumber(int number);

private:
  //! The dive list widget.
  QTableWidget* m_pcDiveListWidget;
  //! The button used to create a new log.
  QPushButton*  m_pcNewLog;
  //! The button used to delete a log.
  QPushButton*  m_pcDeleteLog;
  //! The button used to view a selected log.
  QPushButton*  m_pcViewLog;
  //! The current dive log list.
  DiveList*     m_pcDiveLogList;

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
// coding: utf-8
// End:
