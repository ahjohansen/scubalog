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
#include <qlayout.h>
#include <qmessagebox.h>
#include <qlist.h>
#include <kapp.h>

#include "debug.h"
#include "htmlexporter.h"
#include "ktabcontrol.h"
#include "divelist.h"
#include "logbook.h"
#include "divelogitem.h"
#include "loglistview.h"
#include "logview.h"
#include "personalinfoview.h"
#include "equipmentview.h"
#include "scubalog.h"
#include "scubalog.moc"


//*****************************************************************************
/*!
  Initialize the ScubaLog application GUI. Use \a pzName as the widget name.
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
    m_pcLogBook->readLogBook(*m_cRecentProjects.first());
    *m_pcProjectName = *m_cRecentProjects.first();
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
  connect(m_pcLogView, SIGNAL(viewLogListView()), SLOT(viewLogList()));
  m_pcViews->addTab(m_pcLogView, "Log &view");

  connect(m_pcLogListView, SIGNAL(displayLog(DiveLog*)),
          SLOT(viewLog(DiveLog*)));

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
*/
//*****************************************************************************

ScubaLog::~ScubaLog()
{
  DBG(("Destructing the main widget...\n"));
  saveConfig();

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
  The program is about to exit, save the settings.
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
*/
//*****************************************************************************

void
ScubaLog::openRecent(int nRecentNumber)
{
  QString* pcProjectName = m_cRecentProjects.at(nRecentNumber);
  if ( pcProjectName ) {
    *m_pcProjectName = pcProjectName->copy();

    LogBook* pcLogBook = new LogBook();
    if ( pcLogBook->readLogBook(*m_pcProjectName) ) {
      // Insert the new logbook
      m_pcLogListView->setLogList(&pcLogBook->diveList());
      m_pcLogView->setLogList(&pcLogBook->diveList());
      m_pcPersonalInfoView->setLogBook(pcLogBook);
      m_pcEquipmentView->setLogBook(pcLogBook);
      delete m_pcLogBook;
      m_pcLogBook = pcLogBook;

      updateRecentProjects(*pcProjectName);
    }
    else {
      QString cText;
      cText += "Couldn't read a log book from the file\n";
      cText += "`";
      cText += *pcProjectName;
      cText += "'";
      QMessageBox::warning(this, "ScubaLog error", cText, 1, 0);
      delete pcLogBook;
    }

    setUnsavedData(false);
  }
}


//*****************************************************************************
/*!
  Open a new project. A file requester will open asking the user for a new
  project.

  \sa saveProject(), saveProjectAs().
*/
//*****************************************************************************

void
ScubaLog::openProject()
{
  QString cProjectName = QFileDialog::getOpenFileName(*m_pcProjectName);
  if ( false == cProjectName.isEmpty() ) {
    *m_pcProjectName = cProjectName.copy();

    LogBook* pcLogBook = new LogBook();
    if ( pcLogBook->readLogBook(*m_pcProjectName) ) {
      // Insert the new logbook
      m_pcLogListView->setLogList(&pcLogBook->diveList());
      m_pcLogView->setLogList(&pcLogBook->diveList());
      m_pcPersonalInfoView->setLogBook(pcLogBook);
      m_pcEquipmentView->setLogBook(pcLogBook);
      delete m_pcLogBook;
      m_pcLogBook = pcLogBook;

      updateRecentProjects(cProjectName);
    }
    else {
      QString cText;
      cText += "Couldn't read a log book from the file\n";
      cText += "`";
      cText += cProjectName;
      cText += "'";
      QMessageBox::warning(this, "ScubaLog error", cText, 1, 0);
      delete pcLogBook;
    }

    setUnsavedData(false);
  }
}


//*****************************************************************************
/*!
  Save the current project.

  \sa openProject(), saveProjectAs().
*/
//*****************************************************************************

void
ScubaLog::saveProject()
{
  if ( m_pcProjectName->isEmpty() )
    saveProjectAs();
  else {
    m_pcLogBook->saveLogBook(*m_pcProjectName);
    setUnsavedData(false);
  }
}


//*****************************************************************************
/*!
  Save the current project to a new file. A filedialog will pop up asking
  the user for a filename.

  \sa openProject(), saveProject().
*/
//*****************************************************************************

void
ScubaLog::saveProjectAs()
{
  QString cProjectName = QFileDialog::getSaveFileName(*m_pcProjectName);
  if ( false == cProjectName.isEmpty() ) {
    *m_pcProjectName = cProjectName.copy();
    m_pcLogBook->saveLogBook(*m_pcProjectName);
    setUnsavedData(false);
    updateRecentProjects(cProjectName);
  }
}


//*****************************************************************************
/*!
  Put the project \a cProjectName in front of the recently opened
  projects. If found longer down in the list, remove that entry.

  Only the five last entries will be remembered.
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
*/
//*****************************************************************************

void
ScubaLog::exportLogBook()
{
  if ( 0 == m_pcLogBook )
    return;

  QFileDialog cOutputDialog(this, "outputDialog", true);
  cOutputDialog.setCaption("[ScubaLog] Select output directory");
  cOutputDialog.setMode(QFileDialog::Directory);
  if ( cOutputDialog.exec() ) {
    QString cDirName(cOutputDialog.selectedFile());
    if ( cDirName.isNull() )
      return;
    HTMLExporter cExporter;
    cExporter.exportLogBook(*m_pcLogBook, cDirName);
  }
}


//*****************************************************************************
/*!
  Switch to the log view, displaying the log \a pcLog.
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
  Switch to the log list view.
*/
//*****************************************************************************

void
ScubaLog::viewLogList()
{
  m_pcViews->showPage(m_pcLogListView);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
