//*****************************************************************************
/*!
  \file scubalog.h
  \brief This file contains the definition of the ScubaLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************


#ifndef SCUBALOG_H
#define SCUBALOG_H

#include <qlist.h>
#include <kmainwindow.h>
#include <kio/job.h>

class QTabWidget;
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

class ScubaLog : public KMainWindow {
  Q_OBJECT

  //! Download modes.
  enum DownloadMode_e {
    //! Synchronous mode (i.e. don't return until finished).
    e_DownloadSynchronous,
    //! Asynchronous mode (i.e. return immediately, download in background).
    e_DownloadAsynchronous
  };

public:
  ScubaLog(const char* pzName, const char* pzLogBook);
  virtual ~ScubaLog();

public slots:
  void gotoLog();
  void saveConfig();

protected:
  virtual bool queryExit();

private slots:
  void newProject();
  void openRecent(int nRecentProjectNumber);
  void openProject();
  void saveProject();
  void saveProjectAs();
  void print();
  void handleDownloadFinished(KIO::Job* pcJob);
  void viewLogList();
  void viewLog(DiveLog* pcLog);
  void editLocation(const QString& cLocationName);
  void exportLogBook();
  void exportLogBookUDCF();

private:
  void dragEnterEvent(QDragEnterEvent* pcEvent);
  void dropEvent(QDropEvent* pcEvent);
  bool readLogBookUrl(const QString& cUrlName, DownloadMode_e eMode);
  bool readLogBook(const QString& cFileName);

  void updateRecentProjects(const QString& cProjectName);

  //! The name of the current project, or none if not saved yet.
  QString*          m_pcProjectName;
  //! The current log book.
  LogBook*          m_pcLogBook;
  //! The recent menu.
  QPopupMenu*       m_pcRecentMenu;
  //! The tab view.
  QTabWidget*       m_pcViews;
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

  //! The current URL being processed by KFM.
  QString*          m_pcKfmUrl;
  //! The current temporary file from KFM.
  QString*          m_pcKfmFileName;

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
