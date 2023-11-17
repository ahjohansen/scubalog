/**
 * \file udcfimporter.h
 * \brief This file contains the definition of UDCFImporter.
 * 
 * \par Copyright:
 * André Hübert Johansen
 */

#ifndef UDCFIMPORTER_H
#define UDCFIMPORTER_H

#include "importer.h"
#include <QDate>
#include <QTime>


class EquipmentLog;
class DiveLog;
class QXmlStreamReader;


class UDCFImporter : public Importer
{
public:
  //! Destructor.  Frees all internal resources.
  virtual ~UDCFImporter() {}

  //! Import a log from the file \a cFileName.
  //! Returns 0 on failure.
  virtual DiveLog* importLog(const QString& cFileName) const { return nullptr; }
  //! Import a logbook from \a cName, which might be a file or a directory,
  //! depending on the importer.
  //! Returns 0 on failure.
  virtual LogBook* importLogBook(const QString& cDirName) const;


private:
  void readUDCF(LogBook* logbook, QXmlStreamReader& xml) const;
  void readPersonalInfo(LogBook* logbook, QXmlStreamReader& xml) const;
  void readLocations(LogBook* logbook, QXmlStreamReader& xml) const;
  void readEquipment(LogBook* logbook, QXmlStreamReader& xml) const;
  void readEquipmentHistory(EquipmentLog* equipment, QXmlStreamReader& xml) const;
  void readDiveLogs(LogBook* logbook, QXmlStreamReader& xml) const;
  void readGas(DiveLog* divelog, QXmlStreamReader& xml) const;

  QDate readDate(QXmlStreamReader& xml) const;
  QTime readTime(QXmlStreamReader& xml) const;
};

#endif // UDCFIMPORTER_H
