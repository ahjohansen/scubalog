//*****************************************************************************
/*!
  \file ScubaLog/scubalog.cpp
  \brief This file contains the implementation of the ScubaLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <limits.h>
#include <assert.h>
#include <qcolor.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qkeycode.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qlist.h>
#include <kapp.h>

#include "debug.h"
#include "htmlexporter.h"
#include "integerdialog.h"
#include "ktabcontrol.h"
#include "divelist.h"
#include "logbook.h"
#include "divelogitem.h"
#include "loglistview.h"
#include "logview.h"
#include "locationview.h"
#include "personalinfoview.h"
#include "equipmentview.h"
#include "scubalog.h"
#include "scubalog.moc"


//*****************************************************************************
/*!
  Initialize the ScubaLog application GUI. Use \a pzName as the widget name.

  \author André Johansen.
*/
//*****************************************************************************

ScubaLog::ScubaLog(const char* pzName)
  : KTMainWindow(pzName), m_pcProjectName(0)
{
  KApplication* pcApp = KApplication::getKApplication();
  connect(pcApp, SIGNAL(saveYourself()), SLOT(saveConfig()));

  //
  // Parse the configuration settings
  //

  KConfig* pcConfiguration = pcApp->getConfig();
  pcConfiguration->setGroup("Project");
  QString acRecentProjects[5];

  acRecentProjects[0] = pcConfiguration->readEntry("Recent1");
  acRecentProjects[1] = pcConfiguration->readEntry("Recent2");
  acRecentProjects[2] = pcConfiguration->readEntry("Recent3");
  acRecentProjects[3] = pcConfiguration->readEntry("Recent4");
  acRecentProjects[4] = pcConfiguration->readEntry("Recent5");
  for ( int iRecent = 0; iRecent < 5; iRecent++ ) {
    if ( false == acRecentProjects[iRecent].isEmpty() )
      m_cRecentProjects.append(new QString(acRecentProjects[iRecent]));
  }
  pcConfiguration->setGroup("Settings");
  m_bReadLastUsedProject =
    pcConfiguration->readBoolEntry("AutoOpenLast", true);


  m_pcProjectName = new QString;


  //
  // Setup menu
  //

  KMenuBar* pcMenuBar = menuBar();
  m_pcRecentMenu = new QPopupMenu(0, "recentMenu");
  int nRecentNumber = 1;
  for ( QString* pcRecent = m_cRecentProjects.first();
        pcRecent; nRecentNumber++, pcRecent = m_cRecentProjects.next() ) {
    QString cText;
    cText.sprintf("[&%d] %s", nRecentNumber, pcRecent->data());
    m_pcRecentMenu->insertItem(cText);
  }
  connect(m_pcRecentMenu, SIGNAL(activated(int)), SLOT(openRecent(int)));

  QPopupMenu* pcMenu = new QPopupMenu(0, "projectMenu");
  pcMenu->insertItem("&Open...", this, SLOT(openProject()), CTRL + Key_O);
  pcMenu->insertItem("Open &recent", m_pcRecentMenu);
  pcMenu->insertItem("&Save", this, SLOT(saveProject()), CTRL + Key_S);
  pcMenu->insertItem("Save &As", this, SLOT(saveProjectAs()), CTRL + Key_A);
  pcMenu->insertSeparator();
  pcMenu->insertItem("&Export logbook...", this, SLOT(exportLogBook()));
  pcMenu->insertSeparator();
  pcMenu->insertItem("&Quit", pcApp, SLOT(quit()), CTRL + Key_Q);
  pcMenuBar->insertItem("&Project", pcMenu);

  QPopupMenu* pcLogMenu = new QPopupMenu(0, "logMenu");
  pcLogMenu->insertItem("&Goto log...", this, SLOT(gotoLog()), CTRL + Key_G);
  pcMenuBar->insertItem("Log", pcLogMenu);

  QString cAboutText;
  cAboutText.sprintf("ScubaLog is a scuba dive logging program,\n"
                     "written for the K Desktop Environment.\n\n"
                     "This program is free software, licensed\n"
                     "under the GNU General Public License.\n\n"
                     "Author: André Johansen <andrej@ifi.uio.no>\n\n"
                     "Compilation date: %s", __DATE__);
  pcMenuBar->insertItem("&Help", pcApp->getHelpMenu(true, cAboutText));


  // Create a log book
  m_pcLogBook = new LogBook();

  // Read the recent one, if wanted
  if ( m_bReadLastUsedProject && m_cRecentProjects.first() ) {
    bool isOk = m_pcLogBook->readLogBook(*m_cRecentProjects.first());
    if ( isOk ) {
      *m_pcProjectName = *m_cRecentProjects.first();
      const QString cCaption("ScubaLog [" + *m_pcProjectName + "]");
      setCaption(cCaption);
    }
  }

  // Create the tab controller
  m_pcViews = new KTabControl(this, "tabView");
  m_pcViews->setShape(QTabBar::RoundedAbove);

  // Create the log list view
  m_pcLogListView = new LogListView(m_pcViews, "logListView");
  m_pcLogListView->setLogList(&m_pcLogBook->diveList());
  m_pcViews->addTab(m_pcLogListView, "Log &list");

  // Create the log view
  m_pcLogView = new LogView(m_pcViews, "logView");
  m_pcLogView->setLogList(&m_pcLogBook->diveList());
  m_pcLogView->connect(m_pcLogListView,
                       SIGNAL(aboutToDeleteLog(const DiveLog*)),
                       SLOT(deletingLog(const DiveLog*)));
  m_pcViews->addTab(m_pcLogView, "Log &view");

  connect(m_pcLogListView, SIGNAL(displayLog(DiveLog*)),
          SLOT(viewLog(DiveLog*)));

  // Create the location view
  m_pcLocationView = new LocationView(m_pcViews, "locationView");
  m_pcLocationView->setLogBook(m_pcLogBook);
  m_pcViews->addTab(m_pcLocationView, "Locations");
  connect(m_pcLogView, SIGNAL(editLocation(const QString&)),
          SLOT(editLocation(const QString&)));

  // Create the personal info view
  m_pcPersonalInfoView =
    new PersonalInfoView(m_pcViews, "personalInfo");
  m_pcPersonalInfoView->setLogBook(m_pcLogBook);
  m_pcViews->addTab(m_pcPersonalInfoView, "Personal &info");
  m_pcPersonalInfoView->connect(m_pcViews, SIGNAL(tabSelected(int)),
                                SLOT(updateLoggedDiveTime()));

  // Create the equipment view
  m_pcEquipmentView = new EquipmentView(m_pcViews, "equipment");
  m_pcEquipmentView->setLogBook(m_pcLogBook);
  m_pcViews->addTab(m_pcEquipmentView, "&Equipment");

  enableStatusBar();
  setView(m_pcViews);

  statusBar()->message("Welcome to ScubaLog");
}


//*****************************************************************************
/*!
  Destroy the application GUI.

  \author André Johansen.
*/
//*****************************************************************************

ScubaLog::~ScubaLog()
{
  DBG(("Destructing the main widget...\n"));

  hide();
  if ( m_pcLogBook )
    m_pcLogBook->diveList().clear();
  delete m_pcLogBook;
  delete m_pcProjectName;

  for ( QString* pcRecent = m_cRecentProjects.first();
        pcRecent; pcRecent = m_cRecentProjects.next() ) {
    delete pcRecent;
  }

  DBG(("Destructing the main widget...Done.\n"));
}


//*****************************************************************************
/*!
  Prevent KTMainWindow to delete the widget.
  Notice that this function is only called when the window is closed
  through the window manager, not from `Quit'!

  Always returns `false'.

  \author André Johansen.
*/
//*****************************************************************************

bool
ScubaLog::queryExit()
{
  return false;
}


//*****************************************************************************
/*!
  The program is about to exit, save the settings.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::saveConfig()
{
  KConfig* pcConfig = KApplication::getKApplication()->getConfig();

  pcConfig->setGroup("Project");
  int nRecentNumber = 0;
  for ( QString* pcRecent = m_cRecentProjects.first();
        pcRecent && nRecentNumber < 5; pcRecent = m_cRecentProjects.next() ) {
    if ( false == pcRecent->isEmpty() ) {
      nRecentNumber++;
      QString cRecentText;
      cRecentText.sprintf("Recent%d", nRecentNumber);
      pcConfig->writeEntry(cRecentText, pcRecent->data());
    }
  }

  pcConfig->setGroup("Settings");
  pcConfig->writeEntry("AutoOpenLast", m_bReadLastUsedProject);
}


//*****************************************************************************
/*!
  Open the recent project \a nRecentNumber.

  \sa openProject().

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::openRecent(int nRecentNumber)
{
  QString* pcProjectName = m_cRecentProjects.at(nRecentNumber);
  if ( pcProjectName ) {
    *m_pcProjectName = pcProjectName->copy();

    statusBar()->message("Reading log book...");

    LogBook* pcLogBook = new LogBook();
    if ( pcLogBook->readLogBook(*m_pcProjectName) ) {
      // Insert the new logbook
      m_pcLogListView->setLogList(&pcLogBook->diveList());
      m_pcLogView->setLogList(&pcLogBook->diveList());
      m_pcLocationView->setLogBook(pcLogBook);
      m_pcPersonalInfoView->setLogBook(pcLogBook);
      m_pcEquipmentView->setLogBook(pcLogBook);
      delete m_pcLogBook;
      m_pcLogBook = pcLogBook;

      const QString cCaption("ScubaLog [" + *pcProjectName + "]");
      setCaption(cCaption);

      updateRecentProjects(*pcProjectName);
      setUnsavedData(false);

      statusBar()->message("Reading log book...Done", 3000);
    }
    else {
      delete pcLogBook;

      statusBar()->message("Reading log book...Failed!", 3000);
    }
  }
}


//*****************************************************************************
/*!
  Open a new project. A file requester will open asking the user for a new
  project.

  \sa saveProject(), saveProjectAs().

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::openProject()
{
  const char* apzFilters[] = {
    "ScubaLog files (*.slb)",
    "All files (*)",
    0
  };

  statusBar()->message("Reading log book...");

  QFileDialog cDialog(qApp->mainWidget(), "openDialog", true);
  cDialog.setCaption("[ScubaLog] Open log book");
  cDialog.setMode(QFileDialog::ExistingFile);
  cDialog.setFilters(apzFilters);
  if ( cDialog.exec() &&
       false == cDialog.selectedFile().isNull() ) {
    const QString cProjectName = cDialog.selectedFile();
    *m_pcProjectName = cProjectName.copy();

    LogBook* pcLogBook = new LogBook();
    if ( pcLogBook->readLogBook(*m_pcProjectName) ) {
      // Insert the new logbook
      m_pcLogListView->setLogList(&pcLogBook->diveList());
      m_pcLogView->setLogList(&pcLogBook->diveList());
      m_pcLocationView->setLogBook(pcLogBook);
      m_pcPersonalInfoView->setLogBook(pcLogBook);
      m_pcEquipmentView->setLogBook(pcLogBook);
      delete m_pcLogBook;
      m_pcLogBook = pcLogBook;

      const QString cCaption("ScubaLog [" + cProjectName + "]");
      setCaption(cCaption);

      updateRecentProjects(cProjectName);
      setUnsavedData(false);

      statusBar()->message("Reading log book...Done", 3000);
    }
    else {
      delete pcLogBook;

      statusBar()->message("Reading log book...Failed!", 3000);
    }
  }
  else {
    statusBar()->message("Reading log book...Aborted", 3000);
  }
}


//*****************************************************************************
/*!
  Save the current project.

  \sa openProject(), saveProjectAs().

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::saveProject()
{
  if ( m_pcProjectName->isEmpty() )
    saveProjectAs();
  else {
    statusBar()->message("Writing log book...");

    bool isOk = m_pcLogBook->saveLogBook(*m_pcProjectName);
    if ( isOk ) {
      setUnsavedData(false);
      statusBar()->message("Writing log book...Done", 3000);
    }
    else {
      statusBar()->message("Writing log book...Failed!", 3000);
    }
  }
}


//*****************************************************************************
/*!
  Save the current project to a new file. A filedialog will pop up asking
  the user for a filename.

  \sa openProject(), saveProject().

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::saveProjectAs()
{
  const char* apzFilters[] = {
    "ScubaLog files (*.slb)",
    "All files (*)",
    0
  };

  statusBar()->message("Writing log book...");

  QFileDialog cDialog(qApp->mainWidget(), "saveDialog", true);
  cDialog.setCaption("[ScubaLog] Save log book");
  cDialog.setMode(QFileDialog::AnyFile);
  cDialog.setFilters(apzFilters);
  if ( cDialog.exec() &&
       false == cDialog.selectedFile().isNull() ) {
    QString cProjectName = cDialog.selectedFile();
    if ( -1 == cProjectName.find(".slb") )
      cProjectName += ".slb";
    *m_pcProjectName = cProjectName.copy();
    bool isOk = m_pcLogBook->saveLogBook(*m_pcProjectName);
    if ( isOk ) {
      setUnsavedData(false);
      updateRecentProjects(cProjectName);

      const QString cCaption("ScubaLog [" + cProjectName + "]");
      setCaption(cCaption);
      statusBar()->message("Writing log book...Done", 3000);
    }
    else {
      statusBar()->message("Writing log book...Failed!", 3000);
    }
  }
  else {
    statusBar()->message("Writing log book...Aborted", 3000);
  }
}


//*****************************************************************************
/*!
  Put the project \a cProjectName in front of the recently opened
  projects. If found longer down in the list, remove that entry.

  Only the five last entries will be remembered.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::updateRecentProjects(const QString& cProjectName)
{
  assert(false == cProjectName.isEmpty());

  bool isFound = false;

  // If found among recent ones, pop to front
  for ( QString* pcRecent = m_cRecentProjects.first(); pcRecent;
        pcRecent = m_cRecentProjects.next() ) {
    if ( cProjectName == *pcRecent ) {
      m_cRecentProjects.remove();
      m_cRecentProjects.insert(0, pcRecent);
      isFound = true;
      break;
    }
  }

  // Insert at front if not found
  if ( false == isFound )
    m_cRecentProjects.insert(0, new QString(cProjectName));

  // Trim the recent list to only contain 5 entries
  while ( m_cRecentProjects.count() > 5 ) {
    QString* pcLast = m_cRecentProjects.last();
    delete pcLast;
    m_cRecentProjects.remove();
  }

  // Update the menu
  m_pcRecentMenu->clear();
  int nRecentNumber = 0;
  for ( QString* pcRecent = m_cRecentProjects.first(); pcRecent;
        pcRecent = m_cRecentProjects.next() ) {
    nRecentNumber++;
    QString cText;
    cText.sprintf("[&%d] %s", nRecentNumber, pcRecent->data());
    m_pcRecentMenu->insertItem(cText);
  }
}


//*****************************************************************************
/*!
  Export the logbook.

  Currently, a file dialog will be opened asking for an output directory.
  When more export formats are added, a dialog will be added.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::exportLogBook()
{
  if ( 0 == m_pcLogBook )
    return;

  statusBar()->message("Exporting log book...");

  QFileDialog cOutputDialog(qApp->mainWidget(), "outputDialog", true);
  cOutputDialog.setCaption("[ScubaLog] Select output directory");
  cOutputDialog.setMode(QFileDialog::Directory);
  if ( cOutputDialog.exec() &&
       false == cOutputDialog.selectedFile().isNull() ) {
    const QString cDirName(cOutputDialog.selectedFile());
    HTMLExporter cExporter;
    cExporter.exportLogBook(*m_pcLogBook, cDirName);

    statusBar()->message("Exporting log book...Done", 3000);
  }
  else {
    statusBar()->message("Exporting log book...Aborted", 3000);
  }
}


//*****************************************************************************
/*!
  Switch to the log view, displaying the log \a pcLog.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::viewLog(DiveLog* pcLog)
{
  assert(pcLog);
  assert(m_pcLogView);
  m_pcLogView->viewLog(pcLog);
  m_pcViews->showPage(m_pcLogView);
}


//*****************************************************************************
/*!
  Edit the location with the name \a cLocationName.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::editLocation(const QString& cLocationName)
{
  assert(m_pcLocationView);
  m_pcLocationView->editLocation(cLocationName);
  m_pcViews->showPage(m_pcLocationView);
}


//*****************************************************************************
/*!
  Switch to the log list view.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::viewLogList()
{
  m_pcViews->showPage(m_pcLogListView);
}


//*****************************************************************************
/*!
  Ask the user for a log to view.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::gotoLog()
{
  // Ensure we have a lookbook
  if ( 0 == m_pcLogBook )
    return;

  // Find the smallest and largest available log numbers
  int nMinLogNumber = INT_MAX;
  int nMaxLogNumber = INT_MIN;
  QListIterator<DiveLog> iCurrentLog(m_pcLogBook->diveList());
  for ( ; iCurrentLog.current(); ++iCurrentLog ) {
    if ( iCurrentLog.current()->logNumber() < nMinLogNumber )
      nMinLogNumber = iCurrentLog.current()->logNumber();
    if ( iCurrentLog.current()->logNumber() > nMaxLogNumber )
      nMaxLogNumber = iCurrentLog.current()->logNumber();
  }
  if ( nMinLogNumber <= 0 || INT_MAX == nMinLogNumber ||
       nMaxLogNumber <= 0 || INT_MIN == nMaxLogNumber ||
       nMinLogNumber > nMaxLogNumber )
    return;

  IntegerDialog cDialog(this, "gotoLogDialog", true);
  cDialog.setValue(nMinLogNumber);
  cDialog.setMinValue(nMinLogNumber);
  cDialog.setMaxValue(nMaxLogNumber);
  cDialog.setText("Goto log");
  cDialog.setCaption("[ScubaLog] Goto");
  if ( cDialog.exec() ) {
    cDialog.hide();
    DiveLog* pcLog = 0;
    const int nLogNumber = cDialog.getValue();
    QListIterator<DiveLog> iLog(m_pcLogBook->diveList());
    for ( ; iLog.current(); ++iLog ) {
      if ( iLog.current()->logNumber() == nLogNumber ) {
        pcLog = iLog.current();
        break;
      }
    }
    if ( 0 == pcLog ) {
      QString cText;
      cText.sprintf("Couldn't find log %d", nLogNumber);
      QMessageBox::information(qApp->mainWidget(), "[ScubaLog] Goto",
                               cText);
      return;
    }
    viewLog(pcLog);
  }
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
