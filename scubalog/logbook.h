//*****************************************************************************
/*!
  \file logbook.h
  \brief This file contains the definition of the LogBook class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef LOGBOOK_H
#define LOGBOOK_H

#include <qstring.h>
#include <qlist.h>

class DiveList;
class EquipmentLog;
class LocationLog;

//*****************************************************************************
/*!
  \class LogBook
  \brief The LogBook class is a dive log book.

  A log book contains a dive log list, personal information and
  an equipment list with history.

  \author André Johansen
*/
//*****************************************************************************

class LogBook
{
public:
  LogBook();
  ~LogBook();

  //! Get the name of the diver.
  QString diverName() const { return m_cDiverName; }
  QString emailAddress() const { return m_cEmailAddress; }
  QString wwwUrl() const { return m_cWwwUrl; }
  QString comments() const { return m_cComments; }
  DiveList& diveList();
  DiveList& diveList() const;
  QList<LocationLog>& locationList() const { return *m_pcLocations; }
  QList<EquipmentLog>& equipmentLog() const { return *m_pcEquipment; }

  //! Set the name of the diver to \a cName.
  void setDiverName(const QString& cName) { m_cDiverName = cName; }
  void setEmailAddress(const QString& cAddress) { m_cEmailAddress = cAddress; }
  void setWwwUrl(const QString& cWwwUrl) { m_cWwwUrl = cWwwUrl; }
  void setComments(const QString& cComments) { m_cComments = cComments; }

private:
  //! Disabled copy constructor.
  LogBook(const LogBook&);
  //! Disabled assignment operator.
  LogBook& operator =(const LogBook&);

  //! The name of the diver.
  QString   m_cDiverName;
  //! The email address of the diver.
  QString   m_cEmailAddress;
  //! A www-url pointing to the divers homepage or divelog.
  QString   m_cWwwUrl;
  //! Other information, like comments and medical information.
  QString   m_cComments;
  //! The dive list.
  DiveList* m_pcDiveList;
  //! The location list.
  QList<LocationLog>*  m_pcLocations;
  //! The eqipment with history.
  QList<EquipmentLog>* m_pcEquipment;
};

#endif // LOGBOOK_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
