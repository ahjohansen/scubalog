//*****************************************************************************
/*!
  \file ScubaLog/scubalog.h
  \brief This file contains the definition of the ScubaLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************


#ifndef SCUBALOG_H
#define SCUBALOG_H

#include <qlist.h>
#include <ktmainwindow.h>

class DiveLog;
class LogBook;
class QPopupMenu;
class KTabControl;
class LogListView;
class LogView;
class LocationView;
class PersonalInfoView;
class EquipmentView;


//*****************************************************************************
/*!
  \class ScubaLog
  \brief The ScubaLog class is the ScubaLog application GUI.

  This is a singleton class that should be created in main().

  \author André Johansen <andrej@ifi.uio.no>.
*/
//*****************************************************************************

class ScubaLog : public KTMainWindow {
  Q_OBJECT
public:
  ScubaLog(const char* pzName = 0);
  virtual ~ScubaLog();

public slots:
  void gotoLog();
  void saveConfig();

protected:
  virtual bool queryExit();

private slots:
  void openRecent(int nRecentProjectNumber);
  void openProject();
  void saveProject();
  void saveProjectAs();
  void viewLogList();
  void viewLog(DiveLog* pcLog);
  void editLocation(const QString& cLocationName);
  void exportLogBook();

private:
  void updateRecentProjects(const QString& cProjectName);

  //! The name of the current project, or none if not saved yet.
  QString*          m_pcProjectName;
  //! The current log book.
  LogBook*          m_pcLogBook;
  //! The recent menu.
  QPopupMenu*       m_pcRecentMenu;
  //! The tab view.
  KTabControl*      m_pcViews;
  //! The log list view.
  LogListView*      m_pcLogListView;
  //! The log view.
  LogView*          m_pcLogView;
  //! The location view.
  LocationView*     m_pcLocationView;
  //! The personal info view.
  PersonalInfoView* m_pcPersonalInfoView;
  //! The equipment view.
  EquipmentView*    m_pcEquipmentView;

  //
  // Configuration settings
  //

  //! The last visited projects.
  QList<QString>    m_cRecentProjects;
  //! Set to `true' if the last used project should be loaded upon startup.
  bool              m_bReadLastUsedProject;
};

#endif // SCUBALOG_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
