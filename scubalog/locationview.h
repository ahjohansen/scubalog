//*****************************************************************************
/*!
  \file locationview.h
  \brief This file contains the definition of the LocationView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef LOCATIONVIEW_H
#define LOCATIONVIEW_H

#include <qwidget.h>

class QPopupMenu;
class QListBox;
class QPushButton;
class QLineEdit;
class QMultiLineEdit;
class ListBox;
class LogBook;
class LocationLog;


//*****************************************************************************
/*!
  \class LocationView
  \brief The LocationView class is used to edit information about locations.

  This view provides a way to store information about the different
  locations.

  Currently, only text is supported, but inline pictures will
  be supported in the future.

  Notice that this class does not take ownership of the location logs,
  that responsibility belongs to the LogBook class
  (although this class might create and delete logs upon request
  from the user).

  \author André Johansen
*/
//*****************************************************************************

class LocationView : public QWidget {
  Q_OBJECT
public:
  LocationView(QWidget* pcParent, const char* pzName);
  virtual ~LocationView();

  void setLogBook(LogBook* pcLogBook);

public slots:
  void editLocation(const QString& cLocationName);

private slots:
  void locationSelected(int nLocationIndex);
  void editLocationName(int nLocationIndex);
  void editCurrentLocationName();
  void newLocation();
  void deleteLocation();
  void locationNameChanged();
  void locationDescriptionChanged();
  void prepareLocationsMenu(QPopupMenu* pcMenu);

private:
  //! The location selector.
  ListBox*        m_pcLocations;
  //! The `new location' button.
  QPushButton*    m_pcNewLocation;
  //! The `delete location' button.
  QPushButton*    m_pcDeleteLocation;
  //! The name of the location.
  QLineEdit*      m_pcLocationName;
  //! The location description.
  QMultiLineEdit* m_pcLocationDescription;
  //! The current logbook.
  LogBook*        m_pcLogBook;
};

#endif // LOCATIONVIEW_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
