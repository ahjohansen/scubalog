//*****************************************************************************
/*!
  \file ScubaLog/logbook.h
  \brief This file contains the definition of the LogBook class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef LOGBOOK_H
#define LOGBOOK_H

#include <qstring.h>

class DiveList;
class EquipmentLog;
class LocationLog;
template <class T> class QList<T>;


//*****************************************************************************
/*!
  \class LogBook
  \brief The LogBook class is a dive log book.

  A log book contains a dive log list, personal information and
  an equipment list with history.

  A log book is saved to a file using a QDataStream. A chunk-format is
  used to avoid future problems and help in backwards compatibility.
  Each chunk has an header with theese three fields; an identifier,
  chunk size (including the header) and chunk version. A log book file
  consists of a file header and different chunks.

  The file header:
  \arg U32   An identifier containing the characters "SLLB" 
  \arg U32   The size of the file including the header
  \arg U32   The file format version (current version is 1)

  The personal information chunk:
  \arg U32   An identifier containing the characters "SLPI"
  \arg U32   The size of the chunk including the header
  \arg U32   The chunk format version (current version is 1)
  \arg U8[]  The name of the diver (zero-terminated)
  \arg U8[]  The email address of the diver (zero-terminated)
  \arg U8[]  An www url (zero-terminated)
  \arg U8[]  Comments (zero-terminated)

  The dive log chunk:
  \arg U32   An identifier containing the characters "SLDL"
  \arg U32   The size of the chunk including the header
  \arg U32   The chunk format version (current version is 1)
  \arg U32   The dive number
  \arg U32   The date of the dive (Julian format)
  \arg U32   The time on the day the dive started (in milliseconds)
  \arg U8[]  The location of the dive (zero-terminated)
  \arg U8[]  The name of the dive-buddy (zero-terminated)
  \arg Float The maximum depth (meters)
  \arg U32   The length of the dive (in milliseconds)
  \arg U32   The bottom time (in milliseconds)
  \arg U8[]  The gas type (zero-terminated)
  \arg U32   Surface Air Consuption (in litres)
  \arg Float Air temperature (in degrees Celsius)
  \arg Float Water surface temperature (in degrees Celsius)
  \arg Float The minimum water temperature (in degrees Celsius)
  \arg U8    Plan type
  \arg U8[]  Dive type (zero-terminated)
  \arg U8[]  Dive description (zero-terminated)

  The equipment chunk:
  \arg U32   An identifier containing the characters "SLEL"
  \arg U32   The size of the chunk including the header
  \arg U32   The chunk format version (current version is 1)
  \arg U8[]  The type of the equipment (zero-terminated)
  \arg U8[]  The name of the equipment (zero-terminated)
  \arg U8[]  The serial number of the equipment (zero-terminated)
  \arg U8[]  The service requirements of the equipment (zero-terminated)
  \arg U32   The number of history entries
  \arg U32   The date of entry (Julian format)
  \arg U8[]  The history event (zero-terminated)

  \author André Johansen.
*/
//*****************************************************************************

class LogBook {
public:
  LogBook();
  ~LogBook();

  bool readLogBook(const QString& cFileName);
  bool saveLogBook(const QString& cFileName);

  QString diverName() const { return m_cDiverName; }
  QString emailAddress() const { return m_cEmailAddress; }
  QString wwwUrl() const { return m_cWwwUrl; }
  QString comments() const { return m_cComments; }
  DiveList& diveList();
  const DiveList& diveList() const;
  QList<LocationLog>& locationList() const { return *m_pcLocations; }
  QList<EquipmentLog>& equipmentLog() const { return *m_pcEquipment; }

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
// End:
