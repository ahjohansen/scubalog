//*****************************************************************************
/*!
  \file scubalogproject.h
  \brief This file contains the definition of the ScubaLogProject class.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef SCUBALOGPROJECT_H
#define SCUBALOGPROJECT_H

#include "importer.h"
#include "exporter.h"
#include "chunkio.h"

class QDataStream;
class LogBook;
class DiveLog;
class LocationLog;
class EquipmentLog;
class EquipmentHistoryEntry;

//*****************************************************************************
/*!
  \class ScubaLogProject
  \brief The ScubaLogProject class is an importer/exporter for the native
  ScubaLog file format.

  This file format should be obsoleted soon.

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

  \author André Johansen
*/
//*****************************************************************************

class ScubaLogProject
  : public Importer,
    public Exporter
{
public:
  virtual ~ScubaLogProject() {}

  //! This function is not implemented, and will always return 0.
  virtual DiveLog* importLog(const QString& /*cName*/) const
  {
    return 0;
  }

  virtual LogBook* importLogBook(const QString& cName) const;

  //! This function is not implemented, and will always return false.
  virtual bool exportLog(const DiveLog& /*cLog*/,
                         const QString& /*cName*/) const { return false; }
  virtual bool exportLogBook(const LogBook& cLogBook,
                             const QString& cName) const;

private:
  void readPersonalInformation(QDataStream& cStream,
                               LogBook&     cLogBook) const;
  void writePersonalInformation(QDataStream&   cStream,
                                const LogBook& cLogBook) const;

  void readDiveLog(QDataStream& cStream,
                   DiveLog&     cLog) const;
  void writeDiveLog(QDataStream&   cStream,
                    const DiveLog& cLog) const;

  void readLocationLog(QDataStream& cStream,
                       LocationLog& cLog) const;
  void writeLocationLog(QDataStream& cStream,
                        const LocationLog& cLog) const;

  void readEquipmentLog(QDataStream&  cStream,
                        EquipmentLog& cLog) const;
  void writeEquipmentLog(QDataStream&        cStream,
                         const EquipmentLog& cLog) const;

  void readEquipmentHistoryEntry(QDataStream&           cStream,
                                 EquipmentHistoryEntry& cEntry) const;
  void writeEquipmentHistoryEntry(QDataStream& cStream,
                                  const EquipmentHistoryEntry& cEntry) const;
};

#endif // SCUBALOGPROJECT_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
