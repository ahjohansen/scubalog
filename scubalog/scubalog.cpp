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
#include <kstandardshortcut.h>
#include <q3dragobject.h>
#include <kmenu.h>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <kio/deletejob.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h> // global values in kde2
#include <kurl.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kapplication.h>
#include <qtabwidget.h>
#include <qpainter.h>
#include <qprintdialog.h>
#include <qprinter.h>
#include <q3paintdevicemetrics.h>
#include <qlist.h>
#include <qmessagebox.h>
#include <q3listview.h>
#include <qregexp.h>
#include <q3strlist.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qnamespace.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qpushbutton.h>
#include <q3frame.h>
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
  : KMainWindow(NULL),
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
  setObjectName(pzName);

  KApplication* pcApp = KApplication::kApplication();
  connect(pcApp, SIGNAL(saveYourself()), SLOT(saveConfig()));

  //
  // Parse the configuration settings
  //

  KConfigGroup configuration = KGlobal::config()->group("Project");
  QString acRecentProjects[5];
  acRecentProjects[0] = configuration.readEntry("Recent1");
  acRecentProjects[1] = configuration.readEntry("Recent2");
  acRecentProjects[2] = configuration.readEntry("Recent3");
  acRecentProjects[3] = configuration.readEntry("Recent4");
  acRecentProjects[4] = configuration.readEntry("Recent5");
  for ( int iRecent = 0; iRecent < 5; iRecent++ ) {
    if ( false == acRecentProjects[iRecent].isEmpty() )
      m_cRecentProjects.append(acRecentProjects[iRecent]);
  }
  KConfigGroup settings = KGlobal::config()->group("Settings");
  m_bReadLastUsedProject =
    settings.readEntry("AutoOpenLast", true);

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

  QMenu* pcProjMenu = pcMenuBar->addMenu(i18n("&Project"));
  pcProjMenu->addAction(i18n("&New"), this, SLOT(newProject()),
                        KStandardShortcut::openNew().primary());
  pcProjMenu->addAction(i18n("&Open..."), this, SLOT(openProject()),
                        KStandardShortcut::open().primary());
  m_pcRecentMenu = pcMenuBar->addMenu(i18n("Open &recent"));
  pcProjMenu->addAction(i18n("&Save"), this, SLOT(saveProject()),
                        KStandardShortcut::save().primary());
  pcProjMenu->addAction(i18n("Save &As"), this, SLOT(saveProjectAs()),
                        Qt::CTRL + Qt::Key_A);
  pcProjMenu->addSeparator();
  QMenu* pcExportMenu = pcMenuBar->addMenu(i18n("&Export"));
  pcExportMenu->addAction(i18n("Export &HTML..."), this,
                          SLOT(exportLogBook()));
  pcExportMenu->addAction(i18n("Export &UDCF..."), this,
                          SLOT(exportLogBookUDCF()));
  pcProjMenu->addAction(i18n("&Print..."), this, SLOT(print()),
                        KStandardShortcut::print().primary());
  pcProjMenu->addSeparator();
  pcProjMenu->addAction(i18n("&Quit"), pcApp, SLOT(quit()),
                        KStandardShortcut::quit().primary());

  QMenu* pcLogMenu = pcMenuBar->addMenu(i18n("Log"));
  pcLogMenu->addAction(i18n("&Goto log..."), this, SLOT(gotoLog()),
                       Qt::CTRL + Qt::Key_G);

  QString cAboutText;
  QTextStream s(&cAboutText);
  s << i18n("ScubaLog is a scuba dive logging program,\n"
            "written for the K Desktop Environment.\n\n"
            "This program is free software, licensed\n"
            "under the GNU General Public License.\n\n"
            "Author: André Johansen <andrejoh@gmail.com>\n\n"
            "Version: ") << VERSION << "\n"
    << i18n("Compilation date: ") << __DATE__;
  pcMenuBar->addMenu(this->helpMenu());

  updateRecentProjectsMenu();
  connect(m_pcRecentMenu, SIGNAL(triggered(QAction*)), SLOT(openRecent(QAction*)));


  //
  // Setup tool-bar
  //

  KToolBar& cToolBar = *toolBar();
  KIconLoader& cIconLoader = *KIconLoader::global();
  QPixmap cIcon;

  cIcon = cIconLoader.loadIcon("filenew", KIconLoader::Toolbar);
  cToolBar.addAction(cIcon, i18n("New log-book"),
                     this, SLOT(newProject()));

  cIcon = cIconLoader.loadIcon("fileopen", KIconLoader::Toolbar);
  cToolBar.addAction(cIcon, i18n("Open log-book"),
                     this, SLOT(openProject()));

  cIcon = cIconLoader.loadIcon("filesave", KIconLoader::Toolbar);
  cToolBar.addAction(cIcon, i18n("Save log-book"),
                     this, SLOT(saveProject()));

  cToolBar.addSeparator();

  cIcon = cIconLoader.loadIcon("fileprint", KIconLoader::Toolbar);
  cToolBar.addAction(cIcon, i18n("Print log-book"),
                     this, SLOT(print()));


  //
  // Setup views
  //

  // Create the tab controller
  m_pcViews = new QTabWidget(this);

  // Create the log list view
  m_pcLogListView = new LogListView(m_pcViews);
  m_pcViews->addTab(m_pcLogListView, i18n("Log &list"));

  // Create the log view
  m_pcLogView = new LogView(m_pcViews);
  m_pcLogView->connect(m_pcLogListView,
                       SIGNAL(aboutToDeleteLog(const DiveLog*)),
                       SLOT(deletingLog(const DiveLog*)));
  m_pcViews->addTab(m_pcLogView, i18n("Log &view"));

  connect(m_pcLogListView, SIGNAL(displayLog(DiveLog*)),
          SLOT(viewLog(DiveLog*)));

  // Create the location view
  m_pcLocationView = new LocationView(m_pcViews);
  m_pcViews->addTab(m_pcLocationView, i18n("Locations"));
  connect(m_pcLogView, SIGNAL(editLocation(const QString&)),
          SLOT(editLocation(const QString&)));

  // Create the personal info view
  m_pcPersonalInfoView = new PersonalInfoView(m_pcViews);
  m_pcViews->addTab(m_pcPersonalInfoView, i18n("Personal &info"));
  m_pcPersonalInfoView->connect(m_pcViews, SIGNAL(currentChanged(QWidget*)),
                                SLOT(updateLoggedDiveTime()));

  // Create the equipment view
  m_pcEquipmentView = new EquipmentView(m_pcViews);
  m_pcViews->addTab(m_pcEquipmentView, i18n("&Equipment"));

  statusBar();
  setCentralWidget(m_pcViews);
  setAutoSaveSettings();


  //
  // Read a logbook or create a new one
  //

  // If specified, load the log-book provided as an argument.
  if ( pzLogBook ) {
    bool isOk = readLogBookUrl(KUrl(pzLogBook), e_DownloadSynchronous);
    if ( isOk ) {
      *m_pcProjectName = QString(pzLogBook);
      setCaption(*m_pcProjectName);
    }
  }
  // Read the recent logbook one, if wanted
  else if ( m_bReadLastUsedProject && !m_cRecentProjects.empty() ) {
    bool isOk = readLogBookUrl(m_cRecentProjects.first(),
                               e_DownloadSynchronous);
    if ( isOk ) {
      *m_pcProjectName = m_cRecentProjects.first();
      setCaption(*m_pcProjectName);
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

  statusBar()->showMessage(i18n("Welcome to ScubaLog"));
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
  delete m_pcLogBook;
  delete m_pcProjectName;
  delete m_pcKfmUrl;
  delete m_pcKfmFileName;

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
  KSharedConfig::Ptr config = KGlobal::config();
  KConfigGroup projectGroup  = config->group("Project");
  KConfigGroup settingsGroup = config->group("Settings");

  int nRecentNumber = 0;
  QList<QString>::const_iterator i = m_cRecentProjects.begin();
  for ( ; i != m_cRecentProjects.end() && nRecentNumber < 5; ++i ) {
    const QString& recentName = *i;
    nRecentNumber++;
    QString cRecentText;
    cRecentText.sprintf("Recent%d", nRecentNumber);
    projectGroup.writeEntry(cRecentText, recentName);
  }

  settingsGroup.writeEntry("AutoOpenLast", m_bReadLastUsedProject);
}


//*****************************************************************************
/*!
  Open the recent project triggered by the menu action \a action.

  \sa openProject().
*/
//*****************************************************************************

void
ScubaLog::openRecent(QAction* action)
{
  int index = action->data().toInt();
  assert(index >= 0 && index < m_cRecentProjects.size());
  if ( index >= 0 && index < m_cRecentProjects.size() ) {
    QString cUrlName(m_cRecentProjects.at(index));
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
    setCaption(i18n("Untitled"));
    *m_pcProjectName = "";
  }
  catch ( std::bad_alloc& ) {
    statusBar()->showMessage(i18n("Out of memory!"), 3000);
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
  const QString cFilters("*.slb|" + i18n("ScubaLog projects (*.slb)") + "\n" +
                         "*|"     + i18n("All files (*)"));

  const KUrl cProjectName =
    KFileDialog::getOpenUrl(KUrl(), cFilters, this,
                            i18n("Open log book"));
  if ( false == cProjectName.isEmpty() ) {
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
  KUrl cUrl(cUrlName);

  if ( cUrlName.isEmpty() || false == cUrl.isLocalFile() )
    saveProjectAs();
  else {
    statusBar()->showMessage(i18n("Writing log book..."));

    ScubaLogProject cExporter;
    const bool isOk = cExporter.exportLogBook(*m_pcLogBook, cUrlName);
    if ( isOk ) {
      //setUnsavedData(false);
      statusBar()->showMessage(i18n("Writing log book...Done"), 3000);
    }
    else {
      statusBar()->showMessage(i18n("Writing log book...Failed!"), 3000);
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
  const QString cFilters("*.slb|" + i18n("ScubaLog files (*.slb)") + "\n" +
                         "*|"     + i18n("All files (*)"));

  statusBar()->showMessage(i18n("Writing log book..."));

  QString cProjectName =
    KFileDialog::getSaveFileName(KUrl(), cFilters, this,
                                 i18n("Save log book"));
  if ( false == cProjectName.isEmpty() ) {
    if ( !cProjectName.endsWith(".slb") )
      cProjectName += ".slb";
    *m_pcProjectName = cProjectName;
    ScubaLogProject cExporter;
    const bool isOk = cExporter.exportLogBook(*m_pcLogBook, *m_pcProjectName);
    if ( isOk ) {
      //setUnsavedData(false);
      updateRecentProjects(cProjectName);

      setCaption(cProjectName);
      statusBar()->showMessage(i18n("Writing log book...Done"), 3000);
    }
    else {
      statusBar()->showMessage(i18n("Writing log book...Failed!"), 3000);
    }
  }
  else {
    statusBar()->showMessage(i18n("Writing log book...Aborted"), 3000);
  }
}


//*****************************************************************************
/*!
  Read a project from the URL \a cUrl with mode \a eMode.

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
ScubaLog::readLogBookUrl(const KUrl& cUrl, DownloadMode_e eMode)
{
  // Load local file
  if ( cUrl.isLocalFile() ) {
    QString cProjectName(cUrl.path());
    //KURL::decodeURL(cProjectName);

    bool isOk = readLogBook(cProjectName);
    if ( false == isOk )
      return false;
    *m_pcProjectName = cProjectName;
    updateRecentProjects(cProjectName);
    //setUnsavedData(false);
    setCaption(cProjectName);
  }
  // Load remote file synchronously
  else if ( e_DownloadSynchronous == eMode ) {
    QString cProjectName;
    const bool isDownloadOk =
      KIO::NetAccess::download(cUrl, cProjectName, this);
    if ( false == isDownloadOk )
      return false;
    bool isOk = readLogBook(cProjectName);
    KIO::NetAccess::removeTempFile(cProjectName);
    if ( false == isOk )
      return false;
    *m_pcProjectName = cUrl.url();
    updateRecentProjects(cUrl.url());
    //setUnsavedData(false);
    setCaption(cUrl.url());
  }
  // Load remote file asynchronously
  else {
    *m_pcKfmUrl = cUrl.url();
    m_pcKfmFileName->sprintf("%s", tmpnam(0));
    KIO::Job* pcIOJob = KIO::file_copy(cUrl, *m_pcKfmFileName );
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
  statusBar()->showMessage(i18n("Reading log book..."));

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

      statusBar()->showMessage(i18n("Reading log book...Done"), 3000);
    }
    else {
      delete pcLogBook;
      pcLogBook = 0;
      statusBar()->showMessage(i18n("Reading log book...Failed!"), 3000);
    }
  }
  catch ( std::bad_alloc& ) {
    delete pcLogBook;
    pcLogBook = 0;
    statusBar()->showMessage(i18n("Reading log book...Out of memory!"), 3000);
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

  m_cRecentProjects.removeOne(cProjectName);
  m_cRecentProjects.insert(0, cProjectName);

  // Trim the recent list to only contain 5 entries
  while ( m_cRecentProjects.count() > 5 ) {
    m_cRecentProjects.pop_back();
  }

  updateRecentProjectsMenu();
}


//*****************************************************************************
/*!
  Update the recent projects menu from the list of recent projects
  #m_cRecentProjects.
*/
//*****************************************************************************

void
ScubaLog::updateRecentProjectsMenu()
{
  assert(m_pcRecentMenu);
  m_pcRecentMenu->clear();
  int nRecentNumber = 0;
  QList<QString>::const_iterator i = m_cRecentProjects.begin();
  for ( ; i != m_cRecentProjects.end(); ++i ) {
    nRecentNumber++;
    QString cText;
    QTextStream(&cText) << "[&" << nRecentNumber << "] " << *i;
    QAction* a = m_pcRecentMenu->addAction(cText);
    a->setData(QVariant(nRecentNumber-1));
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

  statusBar()->showMessage(i18n("Exporting log book..."));

  const QString cDirName =
    KFileDialog::getExistingDirectory(KUrl(), this,
                                      i18n("Select output directory"));
  if ( false == cDirName.isEmpty() ) {
    HTMLExporter cExporter;
    cExporter.exportLogBook(*m_pcLogBook, cDirName);
    statusBar()->showMessage(i18n("Exporting log book...Done"), 3000);
  }
  else {
    statusBar()->showMessage(i18n("Exporting log book...Aborted"), 3000);
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


  statusBar()->showMessage(i18n("Exporting log book..."));

  const QString cFilters("*.xml|" + i18n("UDCF Files (*.xml)") + "\n" +
                         "*|"     + i18n("All files (*)"));

  statusBar()->showMessage(i18n("Writing log book..."));

  QString cProjectName =
    KFileDialog::getSaveFileName(KUrl(), cFilters, this,
                                 i18n("Save log book"));
  if ( false == cProjectName.isEmpty() ) {
    if ( !cProjectName.endsWith(".xml") ) {
      cProjectName += ".xml";
    }

    UDCFExporter cExporter;
    cExporter.exportLogBook(*m_pcLogBook, cProjectName);
    statusBar()->showMessage(i18n("Exporting log book...Done"), 3000);
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
  m_pcViews->setCurrentWidget(m_pcLogView);
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
  m_pcViews->setCurrentWidget(m_pcLocationView);
}


//*****************************************************************************
/*!
  Switch to the log list view.
*/
//*****************************************************************************

void
ScubaLog::viewLogList()
{
  m_pcViews->setCurrentWidget(m_pcLogListView);
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
  const DiveList& dives = m_pcLogBook->diveList();
  DiveList::const_iterator i = dives.begin();
  for ( ; i != dives.end(); ++i ) {
    const DiveLog& dive = *(*i);
    int current = dive.logNumber();
    if ( current < nMinLogNumber )
      nMinLogNumber = current;
    if ( current > nMaxLogNumber )
      nMaxLogNumber = current;
  }
  if ( nMinLogNumber <= 0 || INT_MAX == nMinLogNumber ||
       nMaxLogNumber <= 0 || INT_MIN == nMaxLogNumber ||
       nMinLogNumber > nMaxLogNumber )
    return;

  IntegerDialog cDialog(this, true);
  cDialog.setValue(nMinLogNumber);
  cDialog.setMinValue(nMinLogNumber);
  cDialog.setMaxValue(nMaxLogNumber);
  cDialog.setText(i18n("Goto log"));
  // ### TODO: FIXME
  /*
  cDialog.setCaption(i18n("[ScubaLog] Goto"));
  */
  if ( cDialog.exec() ) {
    cDialog.hide();
    const int nLogNumber = cDialog.getValue();
    DiveLog* pcLog = NULL;
    i = dives.begin();
    for ( ; i != dives.end(); ++i ) {
      DiveLog& dive = *(*i);
      if ( dive.logNumber() == nLogNumber ) {
        pcLog = &dive;
        break;
      }
    }
    if ( 0 == pcLog ) {
      QString cText =
        QString(i18n("Couldn't find log %1"))
        .arg(nLogNumber);
      QMessageBox::information(qApp->topLevelWidgets().at(0),
                               i18n("[ScubaLog] Goto"), cText);
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

  statusBar()->showMessage(i18n("Print log book..."));

  // Setup printer
  QPrinter cPrinter;
  cPrinter.setCreator("ScubaLog");
  cPrinter.setDocName(*m_pcProjectName);
  cPrinter.setPageSize(QPrinter::A5);
  QPrintDialog printDialog(&cPrinter, this);
  if ( printDialog.exec() != QDialog::Accepted ) {
    statusBar()->showMessage(i18n("Print log book...Cancelled"), 3000);
    return;
  }

  // Initialise printing
  QPainter cPainter;
  if ( false == cPainter.begin(&cPrinter) ) {
    statusBar()->showMessage(i18n("Print log book...Failed!"), 3000);
    return;
  }

  const Q3PaintDeviceMetrics cMetrics(&cPrinter);
  printf("Page size in native points is %dx%d\n",
         cMetrics.width(), cMetrics.height());

  // Create fonts to use
  const QFont cFontHeaderType("helvetica", 10, QFont::Bold);
  const QFont cFontHeaderCont("helvetica", 10, QFont::Normal);
  const QFont cFontDescription("helvetica", 9, QFont::Normal);
  const QFont cFontFooter("helvetica", 6, QFont::Normal);

  // Print logs
  const QString cDiverName = m_pcLogBook->diverName();
  const DiveList& dives = m_pcLogBook->diveList();
  DiveList::const_iterator iCurrentLog = dives.begin();
  for ( ; iCurrentLog != dives.end(); ++iCurrentLog ) {
    const DiveLog& cCurrentLog = **iCurrentLog;
    QString cLogNumber;
    cLogNumber.setNum(cCurrentLog.logNumber());
    QString cMaxDepth;
    cMaxDepth.setNum(cCurrentLog.maxDepth());
    QString cGasUsage;
    cGasUsage.setNum(cCurrentLog.surfaceAirConsuption());
    QByteArray d;
    d = QByteArray(cCurrentLog.diveDescription().toUtf8()).trimmed();
    QString cDiveDescription(QString::fromUtf8(d.data()));
    const QRegExp cSingleNewLine("[^\n]\n[^\n]");
    cDiveDescription.replace(cSingleNewLine, " ");

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
    cPainter.drawText( 15, 110, 390, 400, Qt::TextWordWrap, cDiveDescription);
    cPainter.setFont(cFontFooter);
    cPainter.drawText( 10, 580, *m_pcProjectName);
    bool bPrintingOk = true;
    if ( (iCurrentLog + 1) != dives.end() )
      bPrintingOk = cPrinter.newPage();
    if ( false == bPrintingOk ) {
      cPainter.end();
      statusBar()->showMessage(i18n("Print log book...Failed!"), 3000);
      return;
    }
    if ( cPrinter.printerState() == QPrinter::Aborted ) {
      cPainter.end();
      statusBar()->showMessage(i18n("Print log book...Cancelled"), 3000);
      return;
    }
  }
  cPainter.end();

  statusBar()->showMessage(i18n("Print log book...Done"), 3000);
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
  if ( pcEvent->mimeData()->hasUrls() ) {
    pcEvent->acceptProposedAction();
  }
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

  if(!Q3UriDrag::decodeToUnicodeUris(pcEvent,list))
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
    *m_pcProjectName = cUrlName;
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
