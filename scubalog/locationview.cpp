//*****************************************************************************
/*!
  \file ScubaLog/locationview.cpp
  \brief This file contains the implementation of the LocationView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <assert.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmultilinedit.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qapplication.h>
#include "logbook.h"
#include "locationlog.h"
#include "locationview.h"
#include "locationview.moc"


//*****************************************************************************
/*!
  Initialize the view.

  \author André Johansen.
*/
//*****************************************************************************

LocationView::LocationView(QWidget* pcParent, const char* pzName)
  : QWidget(pcParent, pzName),
    m_pcLocations(0),
    m_pcNewLocation(0),
    m_pcDeleteLocation(0),
    m_pcLocationName(0),
    m_pcLocationDescription(0),
    m_pcLogBook(0)
{
  m_pcLocations = new QListBox(this, "locations");
  m_pcLocations->setEnabled(false);
  connect(m_pcLocations, SIGNAL(highlighted(int)),
          SLOT(locationSelected(int)));
  connect(m_pcLocations, SIGNAL(selected(int)),
          SLOT(editLocationName(int)));

  m_pcNewLocation = new QPushButton(this, "newLocation");
  m_pcNewLocation->setEnabled(false);
  m_pcNewLocation->setText("&New");
  m_pcNewLocation->setMinimumSize(m_pcNewLocation->sizeHint());
  connect(m_pcNewLocation, SIGNAL(clicked()), SLOT(newLocation()));

  m_pcDeleteLocation = new QPushButton(this, "deleteLocation");
  m_pcDeleteLocation->setText("&Delete");
  m_pcDeleteLocation->setEnabled(false);
  m_pcDeleteLocation->setMinimumSize(m_pcDeleteLocation->sizeHint());
  connect(m_pcDeleteLocation, SIGNAL(clicked()), SLOT(deleteLocation()));

  m_pcLocationName = new QLineEdit(this, "locationName");
  m_pcLocationName->hide();
  m_pcLocationName->setEnabled(false);
  m_pcLocationName->setMinimumSize(m_pcLocationName->sizeHint());
  connect(m_pcLocationName, SIGNAL(returnPressed()),
          SLOT(locationNameChanged()));

  m_pcLocationDescription = new QMultiLineEdit(this, "locationDescription");
  m_pcLocationDescription->setEnabled(false);
  connect(m_pcLocationDescription, SIGNAL(textChanged()),
          SLOT(locationDescriptionChanged()));


  //
  // Geometry management using layout engines
  //

  QVBoxLayout* pcTopLayout    = new QVBoxLayout(this, 5);
  QHBoxLayout* pcButtonLayout = new QHBoxLayout();
  pcTopLayout->addWidget(m_pcLocations, 4);
  pcTopLayout->addLayout(pcButtonLayout);
  pcButtonLayout->addWidget(m_pcNewLocation);
  pcButtonLayout->addWidget(m_pcDeleteLocation);
  pcButtonLayout->addWidget(m_pcLocationName, 10);
  pcTopLayout->addSpacing(10);
  pcTopLayout->addWidget(m_pcLocationDescription, 10);
  pcTopLayout->activate();
}


//*****************************************************************************
/*!
  Destroy the view.

  \author André Johansen.
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

  \author André Johansen.
*/
//*****************************************************************************

void
LocationView::setLogBook(LogBook* pcLogBook)
{
  m_pcLogBook = pcLogBook;

  if ( pcLogBook ) {
    m_pcLocations->clear();
    m_pcLocations->setAutoUpdate(false);
    QList<LocationLog>& cLocationList = pcLogBook->locationList();
    LocationLog* pcLocation = cLocationList.first();
    while ( pcLocation ) {
      m_pcLocations->insertItem(pcLocation->getName());
      pcLocation = cLocationList.next();
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

  \author André Johansen.
*/
//*****************************************************************************

void
LocationView::locationSelected(int nLocationIndex)
{
  assert(m_pcLogBook);
  QList<LocationLog>& cLocationList = m_pcLogBook->locationList();
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

  \author André Johansen.
*/
//*****************************************************************************

void
LocationView::editLocationName(int nLocationIndex)
{
  assert(m_pcLogBook);
  assert(m_pcLocations->count() > (unsigned)nLocationIndex);
  QList<LocationLog>& cLocationList = m_pcLogBook->locationList();
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
  Edit the location \a cLocationName. If it does not exist, create it.

  \author André Johansen.
*/
//*****************************************************************************

void
LocationView::editLocation(const QString& cLocationName)
{
  assert(m_pcLogBook);

  QList<LocationLog>& cLocations = m_pcLogBook->locationList();

  // First, try to find the location
  QListIterator<LocationLog> iLog(cLocations);
  int nLocationIndex = 0;
  bool isFound = false;
  for ( ; iLog.current(); ++iLog, nLocationIndex++ ) {
    if ( (iLog.current())->getName() == cLocationName ) {
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
  catch ( ... ) {
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] New location",
                         "Out of memory when creating a new location log!");
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

  \author André Johansen.
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
  catch ( ... ) {
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] New location",
                         "Out of memory when creating a new location log!");
    return;
  }

  QList<LocationLog>& cLocations = m_pcLogBook->locationList();
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

  \author André Johansen.
*/
//*****************************************************************************

void
LocationView::deleteLocation()
{
  assert(m_pcLogBook);

  // Find and delete the location
  QList<LocationLog>& cLocationList = m_pcLogBook->locationList();
  int nCurrentItem = m_pcLocations->currentItem();
  if ( -1 == nCurrentItem )
    return;
  assert(cLocationList.count() > (unsigned)nCurrentItem);

  LocationLog* pcLog = cLocationList.at(nCurrentItem);
  assert(pcLog);

  QString cMessage;
  cMessage.sprintf("Are you sure you want to delete the location\n"
                   "`%s'?", pcLog->getName().data());
  const int nResult = QMessageBox::information(qApp->mainWidget(),
                                               "[ScubaLog] Delete location",
                                               cMessage, "&Yes", "&No");
  if ( 1 == nResult )
    return;

  m_pcLocations->removeItem(nCurrentItem);
  cLocationList.remove(nCurrentItem);
  delete pcLog;

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

  \author André Johansen.
*/
//*****************************************************************************

void
LocationView::locationNameChanged()
{
  assert(m_pcLogBook);
  assert(m_pcLocations->count() >= 1);

  int nCurrentItem = m_pcLocations->currentItem();
  assert(nCurrentItem >= 0);
  QList<LocationLog>& cLocations = m_pcLogBook->locationList();
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

  \author André Johansen.
*/
//*****************************************************************************

void
LocationView::locationDescriptionChanged()
{
  assert(m_pcLogBook);

  int nCurrentItem = m_pcLocations->currentItem();
  if ( -1 == nCurrentItem )
    return;
  QList<LocationLog>& cLocations = m_pcLogBook->locationList();
  LocationLog* pcLocation = cLocations.at(nCurrentItem);
  if ( 0 == pcLocation )
    return;
  QString cDescription(m_pcLocationDescription->text());
  pcLocation->setDescription(cDescription);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
