//*****************************************************************************
/*!
  \file scubalog.cpp
  \brief This file contains the implementation of the ScubaLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "scubalog.h"
#include "scubalogproject.h"
#include "udcfexporter.h"
#include "htmlexporter.h"
#include "equipmentview.h"
#include "personalinfoview.h"
#include "locationview.h"
#include "logview.h"
#include "loglistview.h"
#include "divelogitem.h"
#include "logbook.h"
#include "divelist.h"
#include "integerdialog.h"
#include "debug.h"
#include "config.h"

#include <kstdaction.h>
#include <qdragobject.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h> // global values in kde2
#include <kurl.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kapp.h>
#include <qtabwidget.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qlist.h>
#include <qmessagebox.h>
#include <qlistview.h>
#include <qfiledialog.h>
#include <qregexp.h>
#include <qstrlist.h>
#include <qstring.h>
#include <qkeycode.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qcolor.h>
#include <new>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

//*****************************************************************************
/*!
  Initialise the ScubaLog application GUI. Use \a pzName as the widget name.

  If \a pzLogBook is non-null, it will be attempted loaded as a log-book.
*/
//*****************************************************************************

ScubaLog::ScubaLog(const char* pzName, const char* pzLogBook)
  : KMainWindow(0, pzName),
    m_pcProjectName(0),
    m_pcLogBook(0),
    m_pcRecentMenu(0),
    m_pcViews(0),
    m_pcLogListView(0),
    m_pcLocationView(0),
    m_pcPersonalInfoView(0),
    m_pcEquipmentView(0),
    m_pcKfmUrl(0),
    m_pcKfmFileName(0),
    m_bReadLastUsedProject(true)
{
  KApplication* pcApp = KApplication::kApplication();
  connect(pcApp, SIGNAL(saveYourself()), SLOT(saveConfig()));

  //
  // Parse the configuration settings
  //

  KConfig* pcConfiguration = KGlobal::config();
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

  setAcceptDrops(true);

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
  pcMenu->insertItem(i18n("&New"), this, SLOT(newProject()),
                     KStdAccel::openNew());
  pcMenu->insertItem(i18n("&Open..."), this, SLOT(openProject()),
                     KStdAccel::open());
  pcMenu->insertItem(i18n("Open &recent"), m_pcRecentMenu);
  pcMenu->insertItem(i18n("&Save"), this, SLOT(saveProject()),
                     KStdAccel::save());
  pcMenu->insertItem(i18n("Save &As"), this, SLOT(saveProjectAs()),
                     CTRL + Key_A);
  pcMenu->insertSeparator();
  QPopupMenu* pcExportMenu = new QPopupMenu(0, "exportMenu");
  pcExportMenu->insertItem(i18n("Export &HTML..."), this,
                           SLOT(exportLogBook()));
  pcExportMenu->insertItem(i18n("Export &UDCF..."), this,
                           SLOT(exportLogBookUDCF()));
  pcMenu->insertItem(i18n("&Export"),pcExportMenu);
  pcMenu->insertItem(i18n("&Print..."), this, SLOT(print()),
                     KStdAccel::print());
  pcMenu->insertSeparator();
  pcMenu->insertItem(i18n("&Quit"), pcApp, SLOT(quit()), KStdAccel::quit());
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
                          "Author: André Johansen <andrejoh@c2i.net>\n\n"
                          "Version: %s\n"
                          "Compilation date: %s"), VERSION, __DATE__);
  pcMenuBar->insertItem(i18n("&Help"), this->helpMenu());


  //
  // Setup tool-bar
  //

  KToolBar& cToolBar = *toolBar();
  KIconLoader& cIconLoader = *KGlobal::iconLoader();
  QPixmap cIcon;

  cIcon = cIconLoader.loadIcon("filenew",KIcon::Toolbar);
  cToolBar.insertButton(cIcon, 0,
                        SIGNAL(clicked()), this, SLOT(newProject()),
                        true, i18n("New log-book"));

  cIcon = cIconLoader.loadIcon("fileopen",KIcon::Toolbar);
  cToolBar.insertButton(cIcon, 0,
                        SIGNAL(clicked()), this, SLOT(openProject()),
                        true, i18n("Open log-book"));

  cIcon = cIconLoader.loadIcon("filesave",KIcon::Toolbar);
  cToolBar.insertButton(cIcon, 0,
                        SIGNAL(clicked()), this, SLOT(saveProject()),
                        true, i18n("Save log-book"));

  cToolBar.insertSeparator();

  cIcon = cIconLoader.loadIcon("fileprint",KIcon::Toolbar);
  cToolBar.insertButton(cIcon, 0,
                        SIGNAL(clicked()), this, SLOT(print()),
                        true, i18n("Print log-book"));


  //
  // Setup views
  //

  // Create the tab controller
  m_pcViews = new QTabWidget(this, "tabView");

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
  m_pcPersonalInfoView->connect(m_pcViews, SIGNAL(currentChanged(QWidget*)),
                                SLOT(updateLoggedDiveTime()));

  // Create the equipment view
  m_pcEquipmentView = new EquipmentView(m_pcViews, "equipment");
  m_pcViews->addTab(m_pcEquipmentView, i18n("&Equipment"));

  statusBar();
  setCentralWidget(m_pcViews);
  setCaption("ScubaLog");


  //
  // Read a logbook or create a new one
  //

  // If specified, load the log-book provided as an argument.
  if ( pzLogBook ) {
    bool isOk = readLogBookUrl(pzLogBook, e_DownloadSynchronous);
    if ( isOk ) {
      *m_pcProjectName = QString(pzLogBook);
      const QString cCaption("ScubaLog [" + *m_pcProjectName + "]");
      setCaption(cCaption);
    }
  }
  // Read the recent logbook one, if wanted
  else if ( m_bReadLastUsedProject && m_cRecentProjects.first() ) {
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

  for ( QString* pcRecent = m_cRecentProjects.first();
        pcRecent; pcRecent = m_cRecentProjects.next() ) {
    delete pcRecent;
  }

  DBG(("Destructing the main widget...Done.\n"));
}


//*****************************************************************************
/*!
  Prevent KMainWindow to delete the widget.
  Notice that this function is only called when the window is closed
  through the window manager, not from "Quit"!

  Always returns "false".
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
*/
//*****************************************************************************

void
ScubaLog::saveConfig()
{
  KConfig* pcConfig = KGlobal::config();

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
  if ( m_cRecentProjects.at(nRecentNumber) ) {
    QString cUrlName(*(m_cRecentProjects.at(nRecentNumber)));
    readLogBookUrl(cUrlName, e_DownloadAsynchronous);
  }
}


//*****************************************************************************
/*!
  Create a new project.
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
*/
//*****************************************************************************

void
ScubaLog::openProject()
{
  QStringList cFilters;
  cFilters << i18n("ScubaLog files (*.slb)")
           << i18n("All files (*)");

  QFileDialog cDialog(qApp->mainWidget(), "openDialog", true);
  cDialog.setCaption(i18n("[ScubaLog] Open log book"));
  cDialog.setMode(QFileDialog::ExistingFile);
  cDialog.setFilters(cFilters);
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

    ScubaLogProject cExporter;
    const bool isOk = cExporter.exportLogBook(*m_pcLogBook, cUrlName);
    if ( isOk ) {
      //setUnsavedData(false);
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
*/
//*****************************************************************************

void
ScubaLog::saveProjectAs()
{
  QStringList cFilters;
  cFilters << i18n("ScubaLog files (*.slb)")
           << i18n("All files (*)");

  statusBar()->message(i18n("Writing log book..."));

  QFileDialog cDialog(qApp->mainWidget(), "saveDialog", true);
  cDialog.setCaption(i18n("[ScubaLog] Save log book"));
  cDialog.setMode(QFileDialog::AnyFile);
  cDialog.setFilters(cFilters);
  if ( cDialog.exec() &&
       false == cDialog.selectedFile().isNull() ) {
    QString cProjectName = cDialog.selectedFile();
    if ( -1 == cProjectName.find(".slb") )
      cProjectName += ".slb";
    *m_pcProjectName = cProjectName.copy();
    ScubaLogProject cExporter;
    const bool isOk = cExporter.exportLogBook(*m_pcLogBook, *m_pcProjectName);
    if ( isOk ) {
      //setUnsavedData(false);
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

  If a log book is loaded, the caption and the "recent projects" menu
  are updated.

  Returns "true" if no errors were encountered, "false" otherwise.
  Notice that "true" does not necessarily mean a project was loaded,
  as it might have to be downloaded first.

  \sa readLogBook().
*/
//*****************************************************************************

bool
ScubaLog::readLogBookUrl(const QString& cUrlName, DownloadMode_e eMode)
{
  // Ensure the URL is valid, try local files in current directory too
  KURL cUrl(cUrlName);
  if ( false == cUrl.isValid() ) {
    QString cLocalFile("file:");
    if ( cUrlName.find('/') ) {
      cLocalFile += QDir::currentDirPath() + "/";
    }
    cLocalFile += cUrlName;
    //cUrl.parse(cLocalFile);
    if ( false == cUrl.isValid() ) {
      return false;
    }
  }
    
  // Load local file
  if ( cUrl.isLocalFile() ) {
    QString cProjectName(cUrl.path());
    //KURL::decodeURL(cProjectName);

    bool isOk = readLogBook(cProjectName);
    if ( false == isOk )
      return false;
    *m_pcProjectName = cProjectName.copy();
    updateRecentProjects(cProjectName);
    //setUnsavedData(false);
    const QString cCaption("ScubaLog [" + cProjectName + "]");
    setCaption(cCaption);
  }
  // Load remote file synchronously
  else if ( e_DownloadSynchronous == eMode ) {
    QString cProjectName;
    const bool isDownloadOk = KIO::NetAccess::download(cUrlName, cProjectName, this);
    if ( false == isDownloadOk )
      return false;
    bool isOk = readLogBook(cProjectName);
    KIO::NetAccess::removeTempFile(cProjectName);
    if ( false == isOk )
      return false;
    *m_pcProjectName = cUrlName.copy();
    updateRecentProjects(cUrlName);
    //setUnsavedData(false);
    const QString cCaption("ScubaLog [" + cUrlName + "]");
    setCaption(cCaption);
  }
  // Load remote file asynchronously
  else {
    *m_pcKfmUrl = cUrlName;
    m_pcKfmFileName->sprintf("%s", tmpnam(0));
    KIO::Job* pcIOJob = KIO::file_copy(cUrlName, *m_pcKfmFileName );
    connect(pcIOJob, SIGNAL(result(KIO::Job*)),this,
            SLOT(handleDownloadFinished(KIO::Job*)));
  }

  return true;
}


//*****************************************************************************
/*!
  Try to read a project from the local file \a cFileName.
  Returns "true" on success or "false" on failure.

  This function will update all the GUI elements of the program,
  except the "recent projects" menu and the caption.

  \sa LogBook::readLogBook().
*/
//*****************************************************************************

bool
ScubaLog::readLogBook(const QString& cFileName)
{
  statusBar()->message(i18n("Reading log book..."));

  LogBook* pcLogBook = 0;
  try {
    ScubaLogProject cImporter;
    pcLogBook = cImporter.importLogBook(cFileName);
    if ( pcLogBook ) {
      // Insert the new logbook
      m_pcLogListView->setLogList(&pcLogBook->diveList());
      m_pcLogView->setLogBook(pcLogBook);
      m_pcLocationView->setLogBook(pcLogBook);
      m_pcPersonalInfoView->setLogBook(pcLogBook);
      m_pcEquipmentView->setLogBook(pcLogBook);
      delete m_pcLogBook;
      m_pcLogBook = pcLogBook;

      //setUnsavedData(false);

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
  Export the logbook.

  Currently, a file dialog will be opened asking for an output directory.
  When more export formats are added, a dialog will be added.
*/
//*****************************************************************************

void
ScubaLog::exportLogBookUDCF()
{
  if ( 0 == m_pcLogBook )
    return;


  statusBar()->message(i18n("Exporting log book..."));

  QStringList cFilters;
  cFilters << i18n("UDCF Files (*.xml)")
           << i18n("All files (*)");

  statusBar()->message(i18n("Writing log book..."));

  QFileDialog cDialog(qApp->mainWidget(), "saveDialog", true);
  cDialog.setCaption(i18n("[ScubaLog] Save log book"));
  cDialog.setMode(QFileDialog::AnyFile);
  cDialog.setFilters(cFilters);
  if ( cDialog.exec() && false == cDialog.selectedFile().isNull() )
    {
    QString cProjectName = cDialog.selectedFile();
    if ( -1 == cProjectName.find(".xml") )
      cProjectName += ".xml";


    UDCFExporter cExporter;
    cExporter.exportLogBook(*m_pcLogBook,cProjectName );

    statusBar()->message(i18n("Exporting log book...Done"), 3000);
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
  Edit the location with the name \a cLocationName.
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

  The creator name will be ScubaLog, the document name is the name of the log
  book and the default paper size is A5.

  \todo Handle different paper sizes.
  \todo Create GUI to select different output formats.
  \todo Possibility to print a range of dive logs.
  \todo Possibility to print locations, equipment and personal info.
*/
//*****************************************************************************

void
ScubaLog::print()
{
  assert(m_pcProjectName);
  if ( m_pcProjectName->isEmpty() || 0 == m_pcLogBook )
    return;

  statusBar()->message(i18n("Print log book..."));

  // Setup printer
  QPrinter cPrinter;
  cPrinter.setCreator("ScubaLog");
  cPrinter.setDocName(*m_pcProjectName);
  cPrinter.setPageSize(QPrinter::A5);
  if ( false == cPrinter.setup(this) ) {
    statusBar()->message(i18n("Print log book...Cancelled"), 3000);
    return;
  }

  // Initialise printing
  QPainter cPainter;
  if ( false == cPainter.begin(&cPrinter) ) {
    statusBar()->message(i18n("Print log book...Failed!"), 3000);
    return;
  }

  const QPaintDeviceMetrics cMetrics(&cPrinter);
  printf("Page size in native points is %dx%d\n",
         cMetrics.width(), cMetrics.height());

  // Create fonts to use
  const QFont cFontHeaderType("helvetica", 10, QFont::Bold);
  const QFont cFontHeaderCont("helvetica", 10, QFont::Normal);
  const QFont cFontDescription("helvetica", 9, QFont::Normal);
  const QFont cFontFooter("helvetica", 6, QFont::Normal);

  // Print logs
  const QString cDiverName = m_pcLogBook->diverName();
  QListIterator<DiveLog> iCurrentLog(m_pcLogBook->diveList());
  for ( ; iCurrentLog.current(); ++iCurrentLog ) {
    const DiveLog& cCurrentLog = *iCurrentLog.current();
    QString cLogNumber;
    cLogNumber.setNum(cCurrentLog.logNumber());
    QString cMaxDepth;
    cMaxDepth.setNum(cCurrentLog.maxDepth());
    QString cGasUsage;
    cGasUsage.setNum(cCurrentLog.surfaceAirConsuption());
    QString cDiveDescription = cCurrentLog.diveDescription().stripWhiteSpace();
    const QRegExp cSingleNewLine("[^\n]\n[^\n]");
    int nIndex = 0;
    const int nDescrLength = cDiveDescription.length();
    while ( nIndex != -1 && nIndex < nDescrLength ) {
      nIndex = cDiveDescription.find(cSingleNewLine, nIndex);
      if ( -1 == nIndex ) {
        break;
      }
      cDiveDescription.replace(nIndex+1, 1, " ");
      nIndex += 2;
    }

    // Print header
    cPainter.setFont(cFontHeaderType);
    cPainter.drawText( 10,  20, cDiverName);
    cPainter.drawText(360,  20, "ScubaLog");
    cPainter.drawText( 10,  40, i18n("Log number:"));
    cPainter.drawText(250,  40, i18n("Date:"));
    cPainter.drawText( 10,  50, i18n("Location:"));
    cPainter.drawText( 10,  60, i18n("Buddy:"));
    cPainter.drawText( 10,  70, i18n("Maximum depth:"));
    cPainter.drawText(250,  70, i18n("Dive time:"));
    cPainter.drawText( 10,  80, i18n("Gas type:"));
    cPainter.drawText(250,  80, i18n("Gas usage:"));
    cPainter.setFont(cFontHeaderCont);
    cPainter.drawText(100,  40, cLogNumber);
    cPainter.drawText(330,  40, cCurrentLog.diveDate().toString());
    cPainter.drawText(100,  50, cCurrentLog.diveLocation());
    cPainter.drawText(100,  60, cCurrentLog.buddyName());
    cPainter.drawText(100,  70, cMaxDepth);
    cPainter.drawText(330,  70, cCurrentLog.diveTime().toString());
    cPainter.drawText(100,  80, cCurrentLog.gasType());
    cPainter.drawText(330,  80, cGasUsage);
    cPainter.drawLine( 30,  90, 390,  90);
    cPainter.setFont(cFontDescription);
    cPainter.drawText( 15, 110, 390, 400, WordBreak, cDiveDescription);
    cPainter.setFont(cFontFooter);
    cPainter.drawText( 10, 580, *m_pcProjectName);
    bool bPrintingOk = true;
    if ( false == iCurrentLog.atLast() )
      bPrintingOk = cPrinter.newPage();
    if ( false == bPrintingOk ) {
      cPainter.end();
      statusBar()->message(i18n("Print log book...Failed!"), 3000);
      return;
    }
    if ( cPrinter.aborted() ) {
      cPainter.end();
      statusBar()->message(i18n("Print log book...Cancelled"), 3000);
      return;
    }
  }
  cPainter.end();

  statusBar()->message(i18n("Print log book...Done"), 3000);
}


//*****************************************************************************
/*!
  Handle drag enter event \a pcEvent.
  The event will be accepted if it is a file name.

  \todo Ask user if current log book should be saved when it has been changed.
*/
//*****************************************************************************

void
ScubaLog::dragEnterEvent(QDragEnterEvent* pcEvent)
{
  pcEvent->accept(QUriDrag::canDecode(pcEvent));
}


//*****************************************************************************
/*!
  Handle drop event \a pcEvent.

  The loogbook will be loades using readLogBookUrl() if it is possible.

  \sa readLogBookUrl(), dragEnterEvent().
*/
//*****************************************************************************

void
ScubaLog::dropEvent(QDropEvent* pcEvent)
{
  QStringList list;

  if(!QUriDrag::decodeToUnicodeUris(pcEvent,list))
        return;

  QString cUrlName(list.first());
  if ( cUrlName.isNull() )
    return;

  // Ensure log book is not the same as the one currently loaded
  if ( *m_pcProjectName == cUrlName )
    return;

  // Ask user to save changes if any
  // TODO -- implement!

  readLogBookUrl(cUrlName, e_DownloadSynchronous);
}


//*****************************************************************************
/*!
  KFM is finished downloading the file, close the connection and try to load
  the log book.
*/
//*****************************************************************************

void
ScubaLog::handleDownloadFinished(KIO::Job* pcJob)
{
  if ( pcJob->error() )
    pcJob->showErrorDialog(this);

  // Read the log book
  const QString cUrlName(*m_pcKfmUrl);
  const QString cProjectName(*m_pcKfmFileName);
  bool isOk = readLogBook(cProjectName);
  if ( isOk ) {
    *m_pcProjectName = cUrlName.copy();
    updateRecentProjects(cUrlName);
    //setUnsavedData(false);

    const QString cCaption("ScubaLog [" + cUrlName + "]");
    setCaption(cCaption);
  }

  KIO::del(*m_pcKfmFileName);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
