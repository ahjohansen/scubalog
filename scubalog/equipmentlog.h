//*****************************************************************************
/*!
  \file ScubaLog/equipmentlog.h
  \brief This file contains the definition of the EquipmentLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef EQUIPMENTLOG_H
#define EQUIPMENTLOG_H

#include <qstring.h>
#include <qdatetime.h>
#include <qlist.h>

class QDataStream;


//*****************************************************************************
/*!
  \class EquipmentHistoryEntry
  \brief The EquipmentHistoryEntry is used to hold equipment history events.

  Each history entry consists of a date and a comment explaining what
  happened.

  History entries should include events like date of purchase, major
  service occasions, failures and other special incidents.

  \author André Johansen.
*/
//*****************************************************************************

class EquipmentHistoryEntry {
public:
  friend QDataStream& operator >>(QDataStream&, EquipmentHistoryEntry&);
  friend QDataStream& operator <<(QDataStream&, const EquipmentHistoryEntry&);

  EquipmentHistoryEntry();
  ~EquipmentHistoryEntry();

  //! Get the date for the history entry.
  const QDate date() const { return m_cDate; }
  //! Set the date for the history entry.
  void setDate(const QDate cDate) { m_cDate = cDate; }
  //! Get the comment for the history entry.
  const QString comment() const { return m_cComment; }
  //! Set the comment for the history entry.
  void setComment(const QString& cComment) { m_cComment = cComment; }

private:
  //! The date of the event.
  QDate   m_cDate;
  //! An explanation of the event.
  QString m_cComment;
};


//*****************************************************************************
/*!
  \class EquipmentLog
  \brief The EquipmentLog class is used to hold information about equipment.

  Each log holds the name, serial number, service type and history of one
  piece of equipment.

  \author André Johansen.
*/
//*****************************************************************************

class EquipmentLog {
public:
  friend QDataStream& operator >>(QDataStream&, EquipmentLog&);
  friend QDataStream& operator <<(QDataStream&, const EquipmentLog&);

  enum {
    //! The current version of the chunk.
    e_ChunkVersion = 1
  };

  EquipmentLog();
  ~EquipmentLog();

  //! Get the name for this item.
  QString name() const { return m_cName; }
  //! Set the name for this item to \a cName.
  void setName(const QString& cName) { m_cName = cName; }
  //! Get the item type.
  QString type() const { return m_cType; }
  //! Set the item type to \a cType.
  void setType(const QString& cType ) { m_cType = cType; }
  //! Get the serial number.
  QString serialNumber() const { return m_cSerial; }
  //! Set the serial number to \a cSerial.
  void setSerialNumber(const QString& cSerial ) { m_cSerial = cSerial; }
  //! Get the service requirements.
  QString serviceRequirements() const { return m_cServiceRequirements; }
  //! Set the service requirements to \a cServiceRequirements.
  void setServiceRequirements(const QString& cServiceRequirements ) {
    m_cServiceRequirements = cServiceRequirements;
  }
  //! Get the history for this equipment item.
  QList<EquipmentHistoryEntry>& history() { return m_cHistory; }

private:
  //! The type of equipment (i.e. mask, regulators).
  QString m_cType;
  //! The name and brand.
  QString m_cName;
  //! The serial number, if any.
  QString m_cSerial;
  //! The service requirements.
  QString m_cServiceRequirements;
  //! The history.
  QList<EquipmentHistoryEntry> m_cHistory;
};

#endif // EQUIPMENTLOG_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
