//*****************************************************************************
/*!
  \file scubalog.cpp
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
#include <new>
#include <qcolor.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qkeycode.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qfiledialog.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qlist.h>
#include <qprinter.h>
#include <qpainter.h>
#include <kapp.h>
#include <kstdaccel.h>
#include <kiconloader.h>
#include <ktoolbar.h>
#include <kurl.h>
#include <kfm.h>
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



//*****************************************************************************
/*!
  Initialize the ScubaLog application GUI. Use \a pzName as the widget name.

  \author André Johansen.
*/
//*****************************************************************************

ScubaLog::ScubaLog(const char* pzName)
  : KTMainWindow(pzName),
    m_pcProjectName(0),
    m_pcLogBook(0),
    m_pcKfmConnection(0),
    m_pcKfmUrl(0),
    m_pcKfmFileName(0)
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


  m_pcProjectName = new QString();
  m_pcKfmUrl      = new QString();
  m_pcKfmFileName = new QString();


  //
  // Setup drag'n'drop
  //

  KDNDDropZone* pcDnD = new KDNDDropZone(this, DndURL);
  connect(pcDnD, SIGNAL(dropAction(KDNDDropZone*)),
          SLOT(handleDrop(KDNDDropZone*)));


  //
  // Setup menu
  //

  KStdAccel* pcKeyAccel = new KStdAccel(kapp->getConfig());

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
  pcMenu->insertItem(i18n("&New"), this, SLOT(newProject()),
                     pcKeyAccel->openNew());
  pcMenu->insertItem(i18n("&Open..."), this, SLOT(openProject()),
                     pcKeyAccel->open());
  pcMenu->insertItem(i18n("Open &recent"), m_pcRecentMenu);
  pcMenu->insertItem(i18n("&Save"), this, SLOT(saveProject()),
                     pcKeyAccel->save());
  pcMenu->insertItem(i18n("Save &As"), this, SLOT(saveProjectAs()),
                     CTRL + Key_A);
  pcMenu->insertSeparator();
  pcMenu->insertItem(i18n("&Export logbook..."), this, SLOT(exportLogBook()));
  pcMenu->insertItem(i18n("&Print..."), this, SLOT(print()),
                     pcKeyAccel->print());
  pcMenu->insertSeparator();
  pcMenu->insertItem(i18n("&Quit"), pcApp, SLOT(quit()), pcKeyAccel->quit());
  pcMenuBar->insertItem(i18n("&Project"), pcMenu);

  QPopupMenu* pcLogMenu = new QPopupMenu(0, "logMenu");
  pcLogMenu->insertItem(i18n("&Goto log..."), this, SLOT(gotoLog()),
                        CTRL + Key_G);
  pcMenuBar->insertItem(i18n("Log"), pcLogMenu);

  QString cAboutText;
  cAboutText.sprintf(i18n("ScubaLog is a scuba dive logging program,\n"
                          "written for the K Desktop Environment.\n\n"
                          "This program is free software, licensed\n"
                          "under the GNU General Public License.\n\n"
                          "Author: André Johansen <andrej@ifi.uio.no>\n\n"
                          "Compilation date: %s"), __DATE__);
  pcMenuBar->insertItem(i18n("&Help"), pcApp->getHelpMenu(true, cAboutText));


  //
  // Setup tool-bar
  //

  KToolBar& cToolBar = *toolBar();
  KIconLoader& cIconLoader = *kapp->getIconLoader();
  QPixmap cIcon;

  cIcon = cIconLoader.loadIcon("filenew.xpm");
  cToolBar.insertButton(cIcon, 0,
                        SIGNAL(clicked()), this, SLOT(newProject()),
                        true, i18n("New log-book"));

  cIcon = cIconLoader.loadIcon("fileopen.xpm");
  cToolBar.insertButton(cIcon, 0,
                        SIGNAL(clicked()), this, SLOT(openProject()),
                        true, i18n("Open log-book"));

  cIcon = cIconLoader.loadIcon("filefloppy.xpm");
  cToolBar.insertButton(cIcon, 0,
                        SIGNAL(clicked()), this, SLOT(saveProject()),
                        true, i18n("Save log-book"));

  cToolBar.insertSeparator();

  cIcon = cIconLoader.loadIcon("fileprint.xpm");
  cToolBar.insertButton(cIcon, 0,
                        SIGNAL(clicked()), this, SLOT(print()),
                        true, i18n("Print log-book"));


  //
  // Setup views
  //

  // Create the tab controller
  m_pcViews = new KTabControl(this, "tabView");
  m_pcViews->setShape(QTabBar::RoundedAbove);

  // Create the log list view
  m_pcLogListView = new LogListView(m_pcViews, "logListView");
  m_pcViews->addTab(m_pcLogListView, i18n("Log &list"));

  // Create the log view
  m_pcLogView = new LogView(m_pcViews, "logView");
  m_pcLogView->connect(m_pcLogListView,
                       SIGNAL(aboutToDeleteLog(const DiveLog*)),
                       SLOT(deletingLog(const DiveLog*)));
  m_pcViews->addTab(m_pcLogView, i18n("Log &view"));

  connect(m_pcLogListView, SIGNAL(displayLog(DiveLog*)),
          SLOT(viewLog(DiveLog*)));

  // Create the location view
  m_pcLocationView = new LocationView(m_pcViews, "locationView");
  m_pcViews->addTab(m_pcLocationView, i18n("Locations"));
  connect(m_pcLogView, SIGNAL(editLocation(const QString&)),
          SLOT(editLocation(const QString&)));

  // Create the personal info view
  m_pcPersonalInfoView =
    new PersonalInfoView(m_pcViews, "personalInfo");
  m_pcViews->addTab(m_pcPersonalInfoView, i18n("Personal &info"));
  m_pcPersonalInfoView->connect(m_pcViews, SIGNAL(tabSelected(int)),
                                SLOT(updateLoggedDiveTime()));

  // Create the equipment view
  m_pcEquipmentView = new EquipmentView(m_pcViews, "equipment");
  m_pcViews->addTab(m_pcEquipmentView, i18n("&Equipment"));

  enableStatusBar();
  setView(m_pcViews);
  setCaption("ScubaLog");


  //
  // Read a recent logbook or create a new one
  //

  // Read the recent logbook one, if wanted
  if ( m_bReadLastUsedProject && m_cRecentProjects.first() ) {
    bool isOk = readLogBookUrl(*m_cRecentProjects.first(),
                               e_DownloadSynchronous);
    if ( isOk ) {
      *m_pcProjectName = *m_cRecentProjects.first();
      const QString cCaption("ScubaLog [" + *m_pcProjectName + "]");
      setCaption(cCaption);
    }
  }
  // Create a log book if none loaded
  if ( 0 == m_pcLogBook )
    m_pcLogBook = new LogBook();
  // Update editors
  m_pcLogListView->setLogList(&m_pcLogBook->diveList());
  m_pcLogView->setLogBook(m_pcLogBook);
  m_pcLocationView->setLogBook(m_pcLogBook);
  m_pcPersonalInfoView->setLogBook(m_pcLogBook);
  m_pcEquipmentView->setLogBook(m_pcLogBook);

  statusBar()->message(i18n("Welcome to ScubaLog"));
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
  delete m_pcKfmUrl;
  delete m_pcKfmFileName;
  destroyKfmConnection();

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
  if ( m_cRecentProjects.at(nRecentNumber) ) {
    QString cUrlName(*(m_cRecentProjects.at(nRecentNumber)));
    readLogBookUrl(cUrlName, e_DownloadAsynchronous);
  }
}


//*****************************************************************************
/*!
  Create a new project.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::newProject()
{
  try {
    LogBook* pcLogBook = new LogBook();
    m_pcLogListView->setLogList(&pcLogBook->diveList());
    m_pcLogView->setLogBook(pcLogBook);
    m_pcLocationView->setLogBook(pcLogBook);
    m_pcPersonalInfoView->setLogBook(pcLogBook);
    m_pcEquipmentView->setLogBook(pcLogBook);
    delete m_pcLogBook;
    m_pcLogBook = pcLogBook;
    const QString cCaption("ScubaLog");
    setCaption(cCaption);
  }
  catch ( std::bad_alloc ) {
    statusBar()->message(i18n("Out of memory!"), 3000);
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
    i18n("ScubaLog files (*.slb)"),
    i18n("All files (*)"),
    0
  };

  QFileDialog cDialog(qApp->mainWidget(), "openDialog", true);
  cDialog.setCaption(i18n("[ScubaLog] Open log book"));
  cDialog.setMode(QFileDialog::ExistingFile);
  cDialog.setFilters(apzFilters);
  if ( cDialog.exec() &&
       false == cDialog.selectedFile().isNull() ) {
    const QString cProjectName = cDialog.selectedFile();
    readLogBookUrl(cProjectName, e_DownloadSynchronous);
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
  QString cUrlName(*m_pcProjectName);
  KURL cUrl(cUrlName);

  if ( cUrlName.isEmpty() || false == cUrl.isLocalFile() )
    saveProjectAs();
  else {
    statusBar()->message(i18n("Writing log book..."));

    bool isOk = m_pcLogBook->saveLogBook(cUrlName);
    if ( isOk ) {
      setUnsavedData(false);
      statusBar()->message(i18n("Writing log book...Done"), 3000);
    }
    else {
      statusBar()->message(i18n("Writing log book...Failed!"), 3000);
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
    i18n("ScubaLog files (*.slb)"),
    i18n("All files (*)"),
    0
  };

  statusBar()->message(i18n("Writing log book..."));

  QFileDialog cDialog(qApp->mainWidget(), "saveDialog", true);
  cDialog.setCaption(i18n("[ScubaLog] Save log book"));
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
      statusBar()->message(i18n("Writing log book...Done"), 3000);
    }
    else {
      statusBar()->message(i18n("Writing log book...Failed!"), 3000);
    }
  }
  else {
    statusBar()->message(i18n("Writing log book...Aborted"), 3000);
  }
}


//*****************************************************************************
/*!
  Read a project from the URL \a cUrlName with mode \a eMode.

  If the file is local or the download mode is #e_DownloadSynchronous,
  it will be attempted loaded right away.
  If the file is remote and the mode is #e_DownloadAsynchronous,
  a connection to KFM is created, and the final loading
  will happpen when the file is downloaded and handleDownloadFinished()
  is called.

  If a log book is loaded, the caption and the `recent projects' menu
  are updated.

  Returns `true' if no errors were encountered, `false' otherwise.
  Notice that `true' does not necessarily mean a project was loaded,
  as it might have to be downloaded first.

  \sa readLogBook().

  \author André Johansen.
*/
//*****************************************************************************

bool
ScubaLog::readLogBookUrl(const QString& cUrlName, DownloadMode_e eMode)
{
  // Ensure the URL is valid, try local files in current directory too
  KURL cUrl(cUrlName);
  if ( cUrl.isMalformed() ) {
    QString cLocalFile("file:");
    if ( cUrlName.find('/') ) {
      cLocalFile += QDir::currentDirPath() + "/";
    }
    cLocalFile += cUrlName;
    cUrl.parse(cLocalFile);
    if ( cUrl.isMalformed() ) {
      return false;
    }
  }
    
  // Load local file
  if ( cUrl.isLocalFile() ) {
    QString cProjectName(cUrl.path());
    KURL::decodeURL(cProjectName);

    bool isOk = readLogBook(cProjectName);
    if ( false == isOk )
      return false;
    *m_pcProjectName = cProjectName.copy();
    updateRecentProjects(cProjectName);
    setUnsavedData(false);
    const QString cCaption("ScubaLog [" + cProjectName + "]");
    setCaption(cCaption);
  }
  // Load remote file synchronously
  else if ( e_DownloadSynchronous == eMode ) {
    QString cProjectName;
    bool isDownloadOk = KFM::download(cUrlName, cProjectName);
    if ( false == isDownloadOk )
      return false;
    bool isOk = readLogBook(cProjectName);
    KFM::removeTempFile(cProjectName);
    if ( false == isOk )
      return false;
    *m_pcProjectName = cUrlName.copy();
    updateRecentProjects(cUrlName);
    setUnsavedData(false);
    const QString cCaption("ScubaLog [" + cUrlName + "]");
    setCaption(cCaption);
  }
  // Load remote file asynchronously
  else {
    KFM* pcKfmConnection = createKfmConnection();
    if ( 0 == pcKfmConnection )
      return false;

    *m_pcKfmUrl = cUrlName;
    m_pcKfmFileName->sprintf("%s", tmpnam(0));
    m_pcKfmConnection->copy(cUrlName, *m_pcKfmFileName);
  }

  return true;
}


//*****************************************************************************
/*!
  Try to read a project from the local file \a cFileName.
  Returns `true' on success or `false' on failure.

  This function will update all the GUI elements of the program,
  except the `recent projects' menu and the caption.

  \sa LogBook::readLogBook().

  \author André Johansen.
*/
//*****************************************************************************

bool
ScubaLog::readLogBook(const QString& cFileName)
{
  statusBar()->message(i18n("Reading log book..."));

  LogBook* pcLogBook = 0;
  try {
    pcLogBook = new LogBook();
    if ( pcLogBook->readLogBook(cFileName) ) {
      // Insert the new logbook
      m_pcLogListView->setLogList(&pcLogBook->diveList());
      m_pcLogView->setLogBook(pcLogBook);
      m_pcLocationView->setLogBook(pcLogBook);
      m_pcPersonalInfoView->setLogBook(pcLogBook);
      m_pcEquipmentView->setLogBook(pcLogBook);
      delete m_pcLogBook;
      m_pcLogBook = pcLogBook;

      setUnsavedData(false);

      statusBar()->message(i18n("Reading log book...Done"), 3000);
    }
    else {
      delete pcLogBook;
      pcLogBook = 0;
      statusBar()->message(i18n("Reading log book...Failed!"), 3000);
    }
  }
  catch ( std::bad_alloc ) {
    delete pcLogBook;
    pcLogBook = 0;
    statusBar()->message(i18n("Reading log book...Out of memory!"), 3000);
  }

  return pcLogBook != 0;
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

  statusBar()->message(i18n("Exporting log book..."));

  QFileDialog cOutputDialog(qApp->mainWidget(), "outputDialog", true);
  cOutputDialog.setCaption(i18n("[ScubaLog] Select output directory"));
  cOutputDialog.setMode(QFileDialog::Directory);
  if ( cOutputDialog.exec() &&
       false == cOutputDialog.selectedFile().isNull() ) {
    const QString cDirName(cOutputDialog.selectedFile());
    HTMLExporter cExporter;
    cExporter.exportLogBook(*m_pcLogBook, cDirName);

    statusBar()->message(i18n("Exporting log book...Done"), 3000);
  }
  else {
    statusBar()->message(i18n("Exporting log book...Aborted"), 3000);
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
  cDialog.setText(i18n("Goto log"));
  cDialog.setCaption(i18n("[ScubaLog] Goto"));
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
      cText.sprintf(i18n("Couldn't find log %d"), nLogNumber);
      QMessageBox::information(qApp->mainWidget(), i18n("[ScubaLog] Goto"),
                               cText);
      return;
    }
    viewLog(pcLog);
  }
}


//*****************************************************************************
/*!
  Print logbook.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::print()
{
  statusBar()->message(i18n("Print log book..."));

  QPrinter cPrinter;
  if ( cPrinter.setup() ) {
    QPainter cPainter;
    if ( cPainter.begin(&cPrinter) ) {
      cPainter.setFont(QFont("helvetica", 6));
      cPainter.drawText(500, 20, "ScubaLog");
      cPainter.end();
    }
    else {
      statusBar()->message(i18n("Print log book...Failed!"), 3000);
      return;
    }
  }
  statusBar()->message(i18n("Print log book...Done"), 3000);
}


//*****************************************************************************
/*!
  Handle drop action in zone \a pcDropZone.

  The logbook will be loaded if the drop is a local file,
  else KFM will be started to download the file if possible.

  \sa handleDownloadFinished(), createKFMConnection(), KDNDDropZone.

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::handleDrop(KDNDDropZone* pcDropZone)
{
  assert(pcDropZone);
  QStrList& cFileList = pcDropZone->getURLList();
  QString cUrlName(cFileList.getFirst());
  if ( cUrlName.isNull() )
    return;

  // Ensure log book is not the same as the one currently loaded
  if ( *m_pcProjectName == cUrlName )
    return;


  // Ask user to save changes if any
  // TODO -- implement!

  // Ensure the URL is valid, try local files in current directory too
  KURL cUrl(cUrlName);
  if ( cUrl.isMalformed() ) {
    QString cLocalFile("file:");
    if ( cUrlName.find('/') ) {
      cLocalFile += QDir::currentDirPath() + "/";
    }
    cLocalFile += cUrlName;
    cUrl.parse(cLocalFile);
    if ( cUrl.isMalformed() ) {
      statusBar()->message(i18n("An invalid URL was dropped on ScubaLog!"),
                           3000);
      return;
    }
  }

  // Load local file
  if ( cUrl.isLocalFile() ) {
    QString cProjectName(cUrl.path());
    KURL::decodeURL(cProjectName);

    bool isOk = readLogBook(cProjectName);
    if ( isOk ) {
      *m_pcProjectName = cProjectName.copy();
      updateRecentProjects(cProjectName);
      setUnsavedData(false);
      const QString cCaption("ScubaLog [" + cProjectName + "]");
      setCaption(cCaption);
    }
  }
  // Load remote file, use KFM to help us out...
  else {
    KFM* pcKfmConnection = createKfmConnection();
    if ( 0 == pcKfmConnection )
      return;

    *m_pcKfmUrl = cUrlName;
    m_pcKfmFileName->sprintf("%s", tmpnam(0));
    m_pcKfmConnection->copy(cUrlName, *m_pcKfmFileName);
  }
}


//*****************************************************************************
/*!
  Create a new connection to KFM to enable remote loading.
  If #m_pcKfmConnection is set (which means a download is currently in
  progress), 0 will be returned, else the new connection, in which case
  #m_pcKfmConnection will be set to that one.

  Code heavily inspired by KHexEdit by Espen Sand.

  \sa destroyKfmConnection().

  \author André Johansen.
*/
//*****************************************************************************

KFM*
ScubaLog::createKfmConnection()
{
  // Only allow one connection
  if ( m_pcKfmConnection ) {
    statusBar()->message(i18n("A remote download is already in progress!"),
                         3000);
    return 0;
  }

  // Establish connection
  try {
    m_pcKfmConnection = new KFM();
    if ( false == m_pcKfmConnection->isOK() ) {
      statusBar()->message(i18n("Couldn't connect to KFM!"), 3000);
      delete m_pcKfmConnection;
      m_pcKfmConnection = 0;
      return 0;
    }
    connect(m_pcKfmConnection, SIGNAL(finished()),
            SLOT(handleDownloadFinished()));
    connect(m_pcKfmConnection, SIGNAL(error(int, const char*)),
            SLOT(handleDownloadError(int, const char*)));
  }
  catch ( std::bad_alloc ) {
    statusBar()->message(i18n("Out of memory!"), 3000);
    return 0;
  }

  return m_pcKfmConnection;
}


//*****************************************************************************
/*!
  Destroy the current connection to KFM.

  \sa createKfmConnection().

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::destroyKfmConnection()
{
  delete m_pcKfmConnection;
  m_pcKfmConnection = 0;
}


//*****************************************************************************
/*!
  KFM is finished downloading the file, close the connection and try to load
  the log book.

  \sa destroyKFMConnection().

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::handleDownloadFinished()
{
  assert(m_pcKfmConnection);

  // Read the log book
  const QString cUrlName(*m_pcKfmUrl);
  const QString cProjectName(*m_pcKfmFileName);
  bool isOk = readLogBook(cProjectName);
  if ( isOk ) {
    *m_pcProjectName = cUrlName.copy();
    updateRecentProjects(cUrlName);
    setUnsavedData(false);

    const QString cCaption("ScubaLog [" + cUrlName + "]");
    setCaption(cCaption);
  }

  // Remove the temporary file
  ::unlink(*m_pcKfmFileName);
  // Destroy the connection
  destroyKfmConnection();
}


//*****************************************************************************
/*!
  KFM had en error downloading the file, report to user and close the
  connection...
  The error-code is \a nErrorCode, the error-message is \a pzMessage.

  \sa destroyKFMConnection().

  \author André Johansen.
*/
//*****************************************************************************

void
ScubaLog::handleDownloadError(int nErrorCode, const char* pzMessage)
{
  assert(m_pcKfmConnection);

  QString cMessage;
  cMessage.sprintf(i18n("Error downloading log book (%d):\n%s\n(`%s')"),
                   nErrorCode, pzMessage, m_pcKfmUrl->data());

  QMessageBox::warning(qApp->mainWidget(), i18n("[ScubaLog] Download error"),
                       cMessage);

  // Remove the temporary file
  ::unlink(*m_pcKfmFileName);
  // Destroy the connection
  destroyKfmConnection();
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
