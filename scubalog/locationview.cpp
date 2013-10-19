//*****************************************************************************
/*!
  \file locationview.cpp
  \brief This file contains the implementation of the LocationView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include <assert.h>
#include <new>
#include <qwidget.h>
#include <qsplitter.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <QTextEdit>
#include <qmessagebox.h>
#include <qlayout.h>
#include <QMenu>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <kapplication.h>
#include <klocale.h>
#include "listbox.h"
#include "logbook.h"
#include "locationlog.h"
#include "locationview.h"


//*****************************************************************************
/*!
  Initialise the view.
*/
//*****************************************************************************

LocationView::LocationView(QWidget* pcParent)
  : QWidget(pcParent),
    m_pcLocations(0),
    m_pcNewLocation(0),
    m_pcDeleteLocation(0),
    m_pcLocationName(0),
    m_pcLocationDescription(0),
    m_pcLogBook(0)
{
  QSplitter* pcSplitter = new QSplitter(Qt::Vertical, this);

  QWidget* pcTop = new QWidget(pcSplitter);

  m_pcLocations = new ListBox(pcTop);
  m_pcLocations->setEnabled(false);
  connect(m_pcLocations, SIGNAL(highlighted(int)),
          SLOT(locationSelected(int)));
  connect(m_pcLocations, SIGNAL(selected(int)),
          SLOT(editLocationName(int)));

  m_pcNewLocation = new QPushButton(pcTop);
  m_pcNewLocation->setEnabled(false);
  m_pcNewLocation->setText(i18n("&New"));
  connect(m_pcNewLocation, SIGNAL(clicked()), SLOT(newLocation()));

  m_pcDeleteLocation = new QPushButton(pcTop);
  m_pcDeleteLocation->setText(i18n("&Delete"));
  m_pcDeleteLocation->setEnabled(false);
  connect(m_pcDeleteLocation, SIGNAL(clicked()), SLOT(deleteLocation()));

  m_pcLocationName = new QLineEdit(pcTop);
  m_pcLocationName->hide();
  m_pcLocationName->setEnabled(false);
  connect(m_pcLocationName, SIGNAL(returnPressed()),
          SLOT(locationNameChanged()));

  m_pcLocationDescription = new QTextEdit(pcSplitter);
  m_pcLocationDescription->setEnabled(false);
  connect(m_pcLocationDescription, SIGNAL(textChanged()),
          SLOT(locationDescriptionChanged()));

  QMenu* pcLocationsMenu = m_pcLocations->getPopupMenu();
  pcLocationsMenu->addAction("&New", this, SLOT(newLocation()));
  pcLocationsMenu->addAction("&Delete", this, SLOT(deleteLocation()));
  pcLocationsMenu->addAction("&Edit name", this,
                             SLOT(editCurrentLocationName()));
  connect(m_pcLocations, SIGNAL(aboutToShowPopup(QMenu*)),
          SLOT(prepareLocationsMenu(QMenu*)));


  //
  // Geometry management
  //

  QSize cButtonSize = m_pcDeleteLocation->sizeHint();
  if ( m_pcNewLocation->sizeHint().width() > cButtonSize.width() ) {
    cButtonSize = m_pcNewLocation->sizeHint();
  }
  QSize cLocationsSize = QSize(cButtonSize.width()*4, cButtonSize.height()*2);
  m_pcLocations->setMinimumSize(cLocationsSize);
  m_pcNewLocation->setMinimumSize(cButtonSize);
  m_pcDeleteLocation->setMinimumSize(cButtonSize);
  m_pcLocationName->setMinimumSize(m_pcLocationName->sizeHint());
  m_pcLocationDescription->resize(100, 100);

  QBoxLayout* pcSplitLayout = new QVBoxLayout(this);
  pcSplitLayout->addWidget(pcSplitter);
  pcSplitLayout->activate();

  QBoxLayout* pcTopLayout    = new QVBoxLayout(pcTop);
  QBoxLayout* pcButtonLayout = new QHBoxLayout();
  pcTopLayout->addWidget(m_pcLocations, 1);
  pcTopLayout->addSpacing(5);
  pcTopLayout->addLayout(pcButtonLayout);
  pcTopLayout->addSpacing(5);
  pcButtonLayout->addWidget(m_pcNewLocation,    0);
  pcButtonLayout->addWidget(m_pcDeleteLocation, 0);
  pcButtonLayout->addWidget(m_pcLocationName,   1);
  pcTopLayout->activate();
}


//*****************************************************************************
/*!
  Destroy the view.
*/
//*****************************************************************************

LocationView::~LocationView()
{
  m_pcLogBook = 0;
}


//*****************************************************************************
/*!
  Set the current log book to \a pcLogBook.

  This function will update the view. If a null-pointer is passed,
  the view will be cleared and the widgets will be disabled.
*/
//*****************************************************************************

void
LocationView::setLogBook(LogBook* pcLogBook)
{
  m_pcLogBook = pcLogBook;

  if ( pcLogBook ) {
    m_pcLocations->clear();
    m_pcLocations->setAutoUpdate(false);
    QList<LocationLog*>& cLocationList = pcLogBook->locationList();
    QListIterator<LocationLog*> iLoc(cLocationList);
    LocationLog* pcLocation;
    while ( iLoc.hasNext() ) {
      pcLocation = iLoc.next();
      m_pcLocations->insertItem(pcLocation->getName());
    }
    m_pcLocations->setAutoUpdate(true);
    m_pcNewLocation->setEnabled(true);
    unsigned int nNumLocations = cLocationList.count();
    bool bLocationsExist = nNumLocations != 0;
    pcLocation = 0;
    if ( bLocationsExist ) {
      m_pcLocations->setCurrentItem(0);
      pcLocation = cLocationList.first();
      assert(pcLocation);
    }
    m_pcLocations->setEnabled(bLocationsExist);
    m_pcDeleteLocation->setEnabled(bLocationsExist);
    m_pcLocationName->setEnabled(bLocationsExist);
    m_pcLocationDescription->setEnabled(bLocationsExist);
    m_pcLocationDescription->setText(bLocationsExist ?
                                     pcLocation->getDescription() :
                                     QString(""));
  }
  else {
    m_pcLocations->clear();
    m_pcNewLocation->setEnabled(false);
    m_pcDeleteLocation->setEnabled(false);
    m_pcLocationDescription->setEnabled(false);
  }
}


//*****************************************************************************
/*!
  The location with the index \a nLocationIndex has been selected.

  Update the rest of the GUI.
*/
//*****************************************************************************

void
LocationView::locationSelected(int nLocationIndex)
{
  if ( nLocationIndex < 0 )
    return;

  assert(m_pcLogBook);
  QList<LocationLog*>& cLocationList = m_pcLogBook->locationList();
  LocationLog* pcLog = cLocationList.at(nLocationIndex);
  assert(pcLog);

  m_pcDeleteLocation->setEnabled(true);
  m_pcLocationDescription->setEnabled(true);
  m_pcLocationDescription->setText(pcLog->getDescription());
}


//*****************************************************************************
/*!
  Edit the name of the item \a nLocationIndex.

  \sa locationNameChanged().
*/
//*****************************************************************************

void
LocationView::editLocationName(int nLocationIndex)
{
  assert(m_pcLogBook);
  assert(m_pcLocations->count() > (unsigned)nLocationIndex);
  QList<LocationLog*>& cLocationList = m_pcLogBook->locationList();
  LocationLog* pcLog = cLocationList.at(nLocationIndex);
  assert(pcLog);
  m_pcLocations->setEnabled(false);
  m_pcNewLocation->setEnabled(false);
  m_pcDeleteLocation->setEnabled(false);
  m_pcLocationDescription->setEnabled(false);
  m_pcLocationName->setText(pcLog->getName());
  m_pcLocationName->show();
  m_pcLocationName->setFocus();
}


//*****************************************************************************
/*!
  Edit the name of the currently selected item, if any.

  \sa editLocationName(), locationNameChanged().
*/
//*****************************************************************************

void
LocationView::editCurrentLocationName()
{
  assert(m_pcLogBook);
  int nLocationIndex = m_pcLocations->currentItem();
  if ( -1 == nLocationIndex ) {
    return;
  }
  editLocationName(nLocationIndex);
}


//*****************************************************************************
/*!
  Edit the location \a cLocationName. If it does not exist, create it.
*/
//*****************************************************************************

void
LocationView::editLocation(const QString& cLocationName)
{
  assert(m_pcLogBook);

  QList<LocationLog*>& cLocations = m_pcLogBook->locationList();

  // First, try to find the location
  QListIterator<LocationLog*> iLog(cLocations);
  int nLocationIndex = 0;
  bool isFound = false;
  for ( ; iLog.hasNext(); ++nLocationIndex ) {
    if ( iLog.next()->getName() == cLocationName ) {
      isFound = true;
      break;
    }
  }
  if ( isFound ) {
    m_pcLocations->setCurrentItem(nLocationIndex);
    locationSelected(nLocationIndex);
    return;
  }

  // If not found, create a new location, and edit that one.
  LocationLog* pcLog = 0;
  try {
    pcLog = new LocationLog();
  }
  catch ( std::bad_alloc ) {
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] New location"),
                         i18n("Out of memory when creating "
                              "a new location log!"));
    return;
  }

  pcLog->setName(cLocationName);
  cLocations.append(pcLog);
  m_pcLocations->insertItem(cLocationName);
  m_pcLocations->setCurrentItem(m_pcLocations->count()-1);
  m_pcLocationName->setText(cLocationName);
  m_pcLocationDescription->setText("");

  m_pcLocations->setEnabled(true);
  m_pcNewLocation->setEnabled(true);
  m_pcDeleteLocation->setEnabled(true);
  m_pcLocationName->setEnabled(false);
  m_pcLocationDescription->setEnabled(true);

  m_pcLocationName->hide();
  m_pcLocationDescription->setFocus();
}


//*****************************************************************************
/*!
  Create a new location.
*/
//*****************************************************************************

void
LocationView::newLocation()
{
  assert(m_pcLogBook);

  LocationLog* pcLog = 0;
  try {
    pcLog = new LocationLog();
  }
  catch ( std::bad_alloc ) {
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] New location"),
                         i18n("Out of memory when creating "
                              "a new location log!"));
    return;
  }

  QList<LocationLog*>& cLocations = m_pcLogBook->locationList();
  cLocations.append(pcLog);
  m_pcLocations->insertItem("");
  m_pcLocations->setCurrentItem(m_pcLocations->count()-1);
  m_pcLocationName->setText("");
  m_pcLocationDescription->setText("");

  m_pcLocations->setEnabled(false);
  m_pcNewLocation->setEnabled(false);
  m_pcDeleteLocation->setEnabled(false);
  m_pcLocationName->setEnabled(true);
  m_pcLocationDescription->setEnabled(false);

  m_pcLocationName->show();
  m_pcLocationName->setFocus();
}


//*****************************************************************************
/*!
  Delete the currently selected location.

  The user will be asked before going on.
*/
//*****************************************************************************

void
LocationView::deleteLocation()
{
  assert(m_pcLogBook);

  // Find and delete the location
  QList<LocationLog*>& cLocationList = m_pcLogBook->locationList();
  int nCurrentItem = m_pcLocations->currentItem();
  if ( -1 == nCurrentItem )
    return;
  assert(cLocationList.count() > (unsigned)nCurrentItem);

  LocationLog* pcLog = cLocationList.at(nCurrentItem);
  assert(pcLog);

  QString cMessage =
    QString(i18n("Are you sure you want to delete the location\n"
                 "'%1'?")).arg(QString(pcLog->getName().data()));
  const int nResult =
    QMessageBox::information(QApplication::topLevelWidgets().at(0),
                             i18n("[ScubaLog] Delete location"),
                             cMessage, i18n("&Yes"), i18n("&No"));
  if ( 1 == nResult )
    return;

  m_pcLocations->removeItem(nCurrentItem);
  cLocationList.removeAt(nCurrentItem);

  // Update the view with the new current location, if any
  const int nNewCurrent = m_pcLocations->currentItem();
  if ( -1 == nNewCurrent ) {
    m_pcLocations->setEnabled(false);
    m_pcDeleteLocation->setEnabled(false);
    m_pcLocationDescription->setText("");
    m_pcLocationDescription->setEnabled(false);
  }
}


//*****************************************************************************
/*!
  Change the location name to \a pzName.
*/
//*****************************************************************************

void
LocationView::locationNameChanged()
{
  assert(m_pcLogBook);
  assert(m_pcLocations->count() >= 1);

  int nCurrentItem = m_pcLocations->currentItem();
  assert(nCurrentItem >= 0);
  QList<LocationLog*>& cLocations = m_pcLogBook->locationList();
  LocationLog* pcLocation = cLocations.at(nCurrentItem);
  QString cName(m_pcLocationName->text());
  pcLocation->setName(cName);

  m_pcLocationName->hide();
  m_pcLocations->changeItem(cName, nCurrentItem);

  m_pcLocations->setEnabled(true);
  m_pcNewLocation->setEnabled(true);
  m_pcDeleteLocation->setEnabled(true);
  m_pcLocationDescription->setEnabled(true);
  m_pcLocationDescription->setFocus();
}


//*****************************************************************************
/*!
  The location description has changed. Save it in the logbook.

  Notice that this function might be called when the text is changed
  from the code too, so be safe...
*/
//*****************************************************************************

void
LocationView::locationDescriptionChanged()
{
  assert(m_pcLogBook);

  int nCurrentItem = m_pcLocations->currentItem();
  if ( -1 == nCurrentItem )
    return;
  QList<LocationLog*>& cLocations = m_pcLogBook->locationList();
  LocationLog* pcLocation = cLocations.at(nCurrentItem);
  if ( 0 == pcLocation )
    return;
  QString cDescription(m_pcLocationDescription->toPlainText());
  pcLocation->setDescription(cDescription);
}


//*****************************************************************************
/*!
  Preparte the locations menu \a pcMenu to be shown
*/
//*****************************************************************************

void
LocationView::prepareLocationsMenu(QMenu* pcMenu)
{
  assert(pcMenu);

  bool bHasSelectedLocation = (m_pcLocations->currentItem() != -1);
  pcMenu->actions().at(1)->setEnabled(bHasSelectedLocation);
  pcMenu->actions().at(2)->setEnabled(bHasSelectedLocation);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
