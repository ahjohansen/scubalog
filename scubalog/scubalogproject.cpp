//*****************************************************************************
/*!
  \file scubalogproject.cpp
  \brief This file contains the implementation of the ScubaLogProject class.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "scubalogproject.h"
#include "logbook.h"
#include "equipmentlog.h"
#include "locationlog.h"
#include "divelist.h"
#include "chunkio.h"
#include "debug.h"

#include <kapplication.h>
#include <klocale.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <algorithm>

bool operator < (const LocationLog& l, const LocationLog& r)
{
  return QString::compare(l.getName(), r.getName());
}


//*****************************************************************************
/*!
  Read a log book from the file \a cFileName.

  Returns the log book if ok, else 0.

  Notice that out-of-memory exceptions (std::bad_alloc)
  should be caught from the outside!
*/
//*****************************************************************************

LogBook*
ScubaLogProject::importLogBook(const QString& cFileName) const
{
  QFile cFile(cFileName);
  if ( false == cFile.open(QIODevice::ReadOnly) ) {
    QString cMessage;
    cMessage = QString(i18n("Couldn't open file"))
      + "\n`" + cFileName + "'!";
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] Read log book"),
                         cMessage);
    return 0;
  }
  QDataStream cStream(&cFile);
  cStream.setVersion(1);
  unsigned int nFileSize = cFile.size();

  unsigned int nChunkId;
  unsigned int nChunkSize;
  unsigned int nChunkVersion;

  // Ensure the file is valid
  cStream >> nChunkId >> nChunkSize >> nChunkVersion;
  if ( (MAKE_CHUNK_ID('S', 'L', 'L', 'B') != nChunkId) ||
       (nChunkSize != cFile.size()) ||
       (1 != nChunkVersion) ) {
    QString cMessage;
    cMessage = QString(i18n("Couldn't read log book from"))
      + "\n`" + cFileName + "'.\n"
      + i18n("Unknown file format -- probably not a log book!");
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] Read log book"),
                         cMessage);
    return 0;
  }

  LogBook* pcLogBook = new LogBook();

  // Read all chunks
  while ( !cStream.atEnd() ) {
    // Read a chunk header
    cStream >> nChunkId;

    // Read personal information
    if ( MAKE_CHUNK_ID('S', 'L', 'P', 'I') == nChunkId ) {
      try {
        readPersonalInformation(cStream, *pcLogBook);
      }
      catch ( IOException& ) {
        delete pcLogBook;
        return 0;
      }
    }

    // Read a dive log
    else if ( MAKE_CHUNK_ID('S', 'L', 'D', 'L') == nChunkId ) {
      int nPos = cFile.pos();
      DiveLog* pcLog = new DiveLog();
      try {
        readDiveLog(cStream, *pcLog);
      }
      catch ( IOException& cException ) {
        QString cText;
        cText = QString(i18n("Error while reading log book from"))
          + "\n`" + cFileName + "':\n" + cException.explanation();
        QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                             i18n("[ScubaLog] Read log book"), cText);
        cFile.seek(nPos);
        cStream >> nChunkSize >> nChunkVersion;
        cFile.seek(nPos + nChunkSize - sizeof(unsigned int));
        DBG(("Position of next chunk=%d\n", cFile.pos()));
        delete pcLog;
        continue;
      }
      DBG(("Read log %d (%s)\n",
           pcLog->logNumber(),
           pcLog->diveLocation().toUtf8().constData()));
      DiveList& cDiveList = pcLogBook->diveList();
      cDiveList.append(pcLog);
    }

    // Read a location log
    else if ( MAKE_CHUNK_ID('S', 'L', 'L', 'L') == nChunkId ) {
      int nPos = cFile.pos();
      LocationLog* pcLog = new LocationLog();
      try {
        readLocationLog(cStream, *pcLog);
      }
      catch ( IOException& cException) {
        QString cText;
        cText = QString(i18n("Error while reading log book from"))
          + "\n`" + cFileName + "':\n" + cException.explanation();
        QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                             i18n("[ScubaLog] Read log book"), cText);
        cFile.seek(nPos);
        cStream >> nChunkSize >> nChunkVersion;
        cFile.seek(nPos + nChunkSize - sizeof(unsigned int));
        DBG(("Position of next chunk=%d\n", cFile.pos()));
        delete pcLog;
        continue;
      }
      DBG(("Read location log for \"%s\"\n",
           pcLog->getName().toUtf8().constData()));
      QList<LocationLog*>& cLocationList = pcLogBook->locationList();
      cLocationList.append(pcLog);
    }

    // Read an equipment entry
    else if ( MAKE_CHUNK_ID('S', 'L', 'E', 'L') == nChunkId ) {
      int nPos = cFile.pos();
      EquipmentLog* pcLog = new EquipmentLog();
      try {
        readEquipmentLog(cStream, *pcLog);
      }
      catch ( IOException& cException) {
        QString cText;
        cText = QString(i18n("Error while reading log book from"))
          + "\n`" + cFileName + "':\n" + cException.explanation();
        QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                             i18n("[ScubaLog] Read log book"), cText);
        cFile.seek(nPos);
        cStream >> nChunkSize >> nChunkVersion;
        cFile.seek(nPos + nChunkSize - sizeof(unsigned int));
        DBG(("Position of next chunk=%d\n", cFile.pos()));
        delete pcLog;
        continue;
      }
      pcLogBook->equipmentLog().append(pcLog);
    }

    // Skip unknown chunk
    else {
      cStream >> nChunkSize >> nChunkVersion;
      const int nOldPos = cFile.pos();
      const int nNewPos = nOldPos + nChunkSize - 3 * sizeof(unsigned int);
      if ( nNewPos <= nOldPos || nNewPos < 0 ||
           (unsigned)nNewPos >= nFileSize ) {
        QString cText;
        cText = QString(i18n("Can't read further, aborting load of log-book"));
        QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                             i18n("[ScubaLog] Read log book"), cText);
        break;
      }
      cFile.seek(nNewPos);
    }
  }

  if ( cFile.error() != QFile::NoError ) {
    QString cMessage;
    cMessage = QString(i18n("Error reading from file"))
      + "\n`" + cFileName + "'!";
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] Read log book"),
                         cMessage);
    delete pcLogBook;
    pcLogBook = 0;
  }

  // Sort the lists.
  DiveList& cDiveList = pcLogBook->diveList();
  cDiveList.sort();
  QList<LocationLog*>& cLocationList = pcLogBook->locationList();
  std::sort(cLocationList.begin(), cLocationList.end());

  return pcLogBook;
}


//*****************************************************************************
/*!
  Save the log book \a cLogBook to the file \a cFileName.

  Returns `true' if ok, else `false'. Only I/O errors like permission denied
  and out of space should result in errors.
*/
//*****************************************************************************

bool
ScubaLogProject::exportLogBook(const LogBook& cLogBook,
                               const QString& cFileName) const
{
  QFile cFile(cFileName);
  if ( false == cFile.open(QIODevice::WriteOnly) ) {
    QString cMessage;
    cMessage = QString(i18n("Couldn't open file"))
      + "\n`" + cFileName + "'.\n"
      + i18n("Ensure that you have write permission and\n"
             "that there is enough free space on the media!");
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] Write log book"),
                         cMessage);
    return false;
  }
  QDataStream cStream(&cFile);
  cStream.setVersion(1);

  unsigned int nChunkSize;
  unsigned int nChunkVersion;

  try {
    // Write file header (size must be updated later)
    nChunkSize    = 0;
    nChunkVersion = 1;
    cStream << MAKE_CHUNK_ID('S', 'L', 'L', 'B')
            << nChunkSize
            << nChunkVersion;

    writePersonalInformation(cStream, cLogBook);

    // Write all the dive logs
    const DiveList& cDiveList = cLogBook.diveList();
    QListIterator<DiveLog*> iDiveLog(cDiveList);
    while ( iDiveLog.hasNext() ) {
      const DiveLog* pcLog = iDiveLog.next();
      writeDiveLog(cStream, *pcLog);
    }

    // Write all the location logs
    const QList<LocationLog*>& cLocationList = cLogBook.locationList();
    QListIterator<LocationLog*> iLocationLog(cLocationList);
    while ( iLocationLog.hasNext() ) {
      const LocationLog* pcLocationLog = iLocationLog.next();
      writeLocationLog(cStream, *pcLocationLog);
    }

    // Write all the equipment entries
    const QList<EquipmentLog*>& cEquipmentList = cLogBook.equipmentLog();
    QListIterator<EquipmentLog*> iEquipmentLog(cEquipmentList);
    while ( iEquipmentLog.hasNext() ) {
      const EquipmentLog* pcEquipment = iEquipmentLog.next();
      writeEquipmentLog(cStream, *pcEquipment);
    }
  }
  catch ( IOException& cException ) {
    QString cText;
    cText = QString(i18n("Error while writing log book to"))
      + "\n`" + cFileName + "':\n" + cException.explanation() + "\n"
      + i18n("Ensure there is enough free space on the media!");
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] Write log book"),
                         cText);
    QFile::remove(cFileName);
    return false;
  }

  // Update file size
  cFile.seek(4);
  nChunkSize = cFile.size();
  cStream << nChunkSize;

  const bool isOk = (cFile.error() == QFile::NoError);
  cFile.close();
  if ( !isOk ) {
    QFile::remove(cFileName);
    QString cMessage;
    cMessage = QString(i18n("Error writing to file"))
      + "\n`" + cFileName + "'!";
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] Write log book"),
                         cMessage);
  }

  return isOk;
}


//*****************************************************************************
/*!
  Read personal information from the stream \a cStream to \a cLogBook.

  The chunk id has already been read from the stream when this function is
  called.

  \exceptions IOException is thrown on input errors.
*/
//*****************************************************************************

void
ScubaLogProject::readPersonalInformation(QDataStream& cStream,
                                         LogBook&     cLogBook) const
{
  unsigned int nChunkSize;
  unsigned int nChunkVersion;

  cStream >> nChunkSize >> nChunkVersion;
  if ( 1 == nChunkVersion ) {
    QString cDiverName;
    QString cEmailAddress;
    QString cWwwUrl;
    QString cComments;
    cStream >> cDiverName
            >> cEmailAddress
            >> cWwwUrl
            >> cComments;
    cLogBook.setDiverName(cDiverName);
    cLogBook.setEmailAddress(cEmailAddress);
    cLogBook.setWwwUrl(cWwwUrl);
    cLogBook.setComments(cComments);
  }
  else {
    QString cMessage;
    cMessage = QString(i18n("Invalid personal information chunk version"));
    throw IOException(cMessage);
  }
}


//*****************************************************************************
/*!
  Write personal information from \a cLogBook to the stream \a cStream.
*/
//*****************************************************************************

void
ScubaLogProject::writePersonalInformation(QDataStream&   cStream,
                                          const LogBook& cLogBook) const
{
  // Write personal information
  const unsigned int nChunkSize =
    3 * sizeof(unsigned int)
    + sizeof(unsigned int) + cLogBook.diverName().length()
    + sizeof(unsigned int) + cLogBook.emailAddress().length()
    + sizeof(unsigned int) + cLogBook.wwwUrl().length()
    + sizeof(unsigned int) + cLogBook.comments().length();
  const unsigned int nChunkVersion = 1;
  cStream << MAKE_CHUNK_ID('S', 'L', 'P', 'I')
          << nChunkSize
          << nChunkVersion
          << cLogBook.diverName()
          << cLogBook.emailAddress()
          << cLogBook.wwwUrl()
          << cLogBook.comments();
}


//*****************************************************************************
/*!
  Read a dive log from the stream \a cStream to \a cLog.

  \exception IOException is thrown on input errors.
*/
//*****************************************************************************

void
ScubaLogProject::readDiveLog(QDataStream& cStream,
                             DiveLog&     cLog) const
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.pos();

  // Read rest of header
  unsigned int nChunkSize;
  unsigned int nChunkVersion;
  cStream >> nChunkSize
          >> nChunkVersion;
  if ( 1 != nChunkVersion ) {
    QString cText;
    cText.sprintf(i18n("Unknown dive log chunk version %d!").toLatin1(),
                  nChunkVersion);
    throw IOException(cText);
  }

  int           nLogNumber;
  QDate         cDiveDate;
  QTime         cDiveStart;
  QString       cLocation;
  QString       cBuddyName;
  float         vMaxDepth;
  QTime         cDiveTime;
  QTime         cBottomTime;
  QString       cGasType;
  int           nNumLitresUsed;
  float         vAirTemperature;
  float         vSurfaceTemperature;
  float         vWaterTemperature;
  unsigned char nPlanType;
  QString       cDiveType;
  QString       cDescription;
  cStream >> nLogNumber
          >> cDiveDate
          >> cDiveStart
          >> cLocation
          >> cBuddyName
          >> vMaxDepth
          >> cDiveTime
          >> cBottomTime
          >> cGasType
          >> nNumLitresUsed
          >> vAirTemperature
          >> vSurfaceTemperature
          >> vWaterTemperature
          >> nPlanType
          >> cDiveType
          >> cDescription;

  cLog.setLogNumber(nLogNumber);
  cLog.setDiveDate(cDiveDate);
  cLog.setDiveStart(cDiveStart);
  cLog.setDiveLocation(cLocation);
  cLog.setBuddyName(cBuddyName);
  cLog.setMaxDepth(vMaxDepth);
  cLog.setDiveTime(cDiveTime);
  cLog.setBottomTime(cBottomTime);
  cLog.setGasType(cGasType);
  cLog.setSurfaceAirConsumption(nNumLitresUsed);
  cLog.setAirTemperature(vAirTemperature);
  cLog.setWaterSurfaceTemperature(vSurfaceTemperature);
  cLog.setWaterTemperature(vWaterTemperature);
  cLog.setPlanType((DiveLog::PlanType_e)nPlanType);
  cLog.setDiveType(cDiveType);
  cLog.setDiveDescription(cDescription);

  // Ensure we're at the correct position in the stream
  const unsigned int nNextChunkPos = nPos + nChunkSize - sizeof(unsigned int);
  if ( nNextChunkPos != cDevice.pos() ) {
    QString cText;
    cText.sprintf(i18n("Unexpected position after reading dive log!\n"
                       "Current position is %d; expected %d...").toLatin1(),
                  cDevice.pos(), nNextChunkPos);
    throw IOException(cText);
  }
}


//*****************************************************************************
/*!
  Write the dive log \a cLog to the stream \a cStream.
  On error, the exception IOException is thrown.
*/
//*****************************************************************************

void
ScubaLogProject::writeDiveLog(QDataStream&   cStream,
                              const DiveLog& cLog) const
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.pos();

  const unsigned int nChunkVersion = 1;
  const unsigned int nChunkSize =
    3 * sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int) + cLog.diveLocation().length()
    + sizeof(unsigned int) + cLog.buddyName().length()
    + sizeof(float)
    + sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int) + cLog.gasType().length()
    + sizeof(unsigned int)
    + sizeof(float)
    + sizeof(float)
    + sizeof(float)
    + sizeof(unsigned char)
    + sizeof(unsigned int) + cLog.diveType().length()
    + sizeof(unsigned int) + cLog.diveDescription().length();
  const unsigned char nPlanType = (unsigned char)cLog.planType();
  cStream << MAKE_CHUNK_ID('S', 'L', 'D', 'L')
          << nChunkSize
          << nChunkVersion
          << cLog.logNumber()
          << cLog.diveDate()
          << cLog.diveStart()
          << cLog.diveLocation()
          << cLog.buddyName()
          << cLog.maxDepth()
          << cLog.diveTime()
          << cLog.bottomTime()
          << cLog.gasType()
          << cLog.surfaceAirConsuption()
          << cLog.airTemperature()
          << cLog.waterSurfaceTemperature()
          << cLog.waterTemperature()
          << nPlanType
          << cLog.diveType()
          << cLog.diveDescription();

  // Ensure we're at the correct position in the stream
  const unsigned int nNextChunkPos = nPos + nChunkSize;
  if ( nNextChunkPos != cDevice.pos() ) {
    QString cText;
    cText.sprintf(i18n("Unexpected position after writing dive log!\n"
                       "Current position is %d; expected %d...").toLatin1(),
                  cDevice.pos(), nNextChunkPos);
    throw IOException(cText);
  }
}


//*****************************************************************************
/*!
  Read a location log from the stream \a cStream into \a cLog.

  \exception IOException is thrown on error.
*/
//*****************************************************************************

void
ScubaLogProject::readLocationLog(QDataStream& cStream, LocationLog& cLog) const
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.pos();

  // Read rest of header
  unsigned int nChunkSize;
  unsigned int nChunkVersion;
  cStream >> nChunkSize
          >> nChunkVersion;
  if ( 1 != nChunkVersion ) {
    QString cText;
    cText.sprintf(i18n("Unknown location log chunk version %d!").toLatin1(),
                  nChunkVersion);
    throw IOException(cText);
  }

  // Read the body of the data
  QString cName;
  QString cDescription;
  cStream >> cName
          >> cDescription;
  cLog.setName(cName);
  cLog.setDescription(cDescription);

  // Ensure we're at the correct position in the stream
  const unsigned int nNextChunkPos = nPos + nChunkSize - sizeof(unsigned int);
  if ( nNextChunkPos != cDevice.pos() ) {
    QString cText;
    cText.sprintf(i18n("Unexpected position after reading location log!\n"
                       "Current position is %d; expected %d...").toLatin1(),
                  cDevice.pos(), nNextChunkPos);
    throw IOException(cText);
  }
}


//*****************************************************************************
/*!
  Write the location log \a cLog to the stream \a cStream.
  On error, the exception IOException is thrown.
*/
//*****************************************************************************

void
ScubaLogProject::writeLocationLog(QDataStream&       cStream,
                                  const LocationLog& cLog) const
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.pos();

  // Calculate the chunk size
  unsigned int nChunkSize =
    3 * sizeof(unsigned int)
    + sizeof(unsigned int) + cLog.getName().length()
    + sizeof(unsigned int) + cLog.getDescription().length();

  // Write the header
  const unsigned int nChunkVersion = 1;
  cStream << MAKE_CHUNK_ID('S', 'L', 'L', 'L')
          << nChunkSize
          << nChunkVersion;

  // Write the body
  cStream << cLog.getName()
          << cLog.getDescription();

  // Ensure we're at the correct position in the stream
  const unsigned int nNextChunkPos = nPos + nChunkSize;
  if ( nNextChunkPos != cDevice.pos() ) {
    QString cText;
    cText.sprintf(i18n("Unexpected position after writing location log!\n"
                       "Current position is %d; expected %d...").toLatin1(),
                  cDevice.pos(), nNextChunkPos);
    throw IOException(cText);
  }
}


//*****************************************************************************
/*!
  Read an equipment log from the stream \a cStream into \a cLog.
  On error, the exception IOException is thrown.
  Notice that the OOM exception (std::bad_alloc) must be handled
  on the outside too!
*/
//*****************************************************************************

void
ScubaLogProject::readEquipmentLog(QDataStream&  cStream,
                                  EquipmentLog& cLog) const
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.pos();

  // Read rest of header
  unsigned int nChunkSize;
  unsigned int nChunkVersion;
  cStream >> nChunkSize
          >> nChunkVersion;
  if ( 1 != nChunkVersion ) {
    QString cText;
    cText.sprintf(i18n("Unknown equipment log chunk version %d!").toLatin1(),
                  nChunkVersion);
    throw IOException(cText);
  }

  // Read the body of the data
  QString cType;
  QString cName;
  QString cSerial;
  QString cServiceReq;
  cStream >> cType
          >> cName
          >> cSerial
          >> cServiceReq;
  cLog.setName(cName);
  cLog.setType(cType);
  cLog.setSerialNumber(cSerial);
  cLog.setServiceRequirements(cServiceReq);

  // Read the history entries
  unsigned int nNumEntries;
  cStream >> nNumEntries;
  for ( unsigned int iEntry = 0; iEntry < nNumEntries; iEntry++ ) {
    EquipmentHistoryEntry* pcEntry = new EquipmentHistoryEntry();
    try {
      readEquipmentHistoryEntry(cStream, *pcEntry);
    }
    catch ( ... ) {
      delete pcEntry;
      throw;
    }
    cLog.history().append(pcEntry);
  }

  // Ensure we're at the correct position in the stream
  const unsigned int nNextChunkPos = nPos + nChunkSize - sizeof(unsigned int);
  if ( nNextChunkPos != cDevice.pos() ) {
    QString cText;
    cText.sprintf(i18n("Unexpected position after reading equipment log!\n"
                       "Current position is %d; expected %d...").toLatin1(),
                  cDevice.pos(), nNextChunkPos);
    throw IOException(cText);
  }
}


//*****************************************************************************
/*!
  Write the log \a cLog to the stream \a cStream.
  Returns a reference to the stream.
  On error, the exception IOException is thrown.
*/
//*****************************************************************************

void
ScubaLogProject::writeEquipmentLog(QDataStream&        cStream,
                                   const EquipmentLog& cLog) const
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.pos();

  QListIterator<EquipmentHistoryEntry*> iHistoryEntry(cLog.history());

  // Calculate the chunk size
  unsigned int nChunkSize =
    3 * sizeof(unsigned int)
    + sizeof(unsigned int) + cLog.type().length()
    + sizeof(unsigned int) + cLog.name().length()
    + sizeof(unsigned int) + cLog.serialNumber().length()
    + sizeof(unsigned int) + cLog.serviceRequirements().length()
    + sizeof(unsigned int);
  for ( ; iHistoryEntry.hasNext(); ) {
    const EquipmentHistoryEntry* pcEntry = iHistoryEntry.next();
    nChunkSize += 2 * sizeof(unsigned int) + pcEntry->comment().length();
  }

  // Write the header
  unsigned int nChunkVersion = 1;
  cStream << MAKE_CHUNK_ID('S', 'L', 'E', 'L')
          << nChunkSize
          << nChunkVersion;

  // Write the body
  cStream << cLog.type()
          << cLog.name()
          << cLog.serialNumber()
          << cLog.serviceRequirements()
          << cLog.history().count();

  // Write the history entries
  iHistoryEntry = cLog.history();
  for ( ; iHistoryEntry.hasNext(); ) {
    const EquipmentHistoryEntry* pcEntry = iHistoryEntry.next();
    writeEquipmentHistoryEntry(cStream, *pcEntry);
  }

  // Ensure we're at the correct position in the stream
  const unsigned int nNextChunkPos = nPos + nChunkSize;
  if ( nNextChunkPos != cDevice.pos() ) {
    QString cText;
    cText.sprintf(i18n("Unexpected position after writing equipment log!\n"
                       "Current position is %d; expected %d...").toLatin1(),
                  cDevice.pos(), nNextChunkPos);
    throw IOException(cText);
  }
}


//*****************************************************************************
/*!
  Read an equipment history entry from the stream \a cStream into \a cEntry.

  This function needs better error-handling. Use exceptions!
*/
//*****************************************************************************

void
ScubaLogProject::readEquipmentHistoryEntry(QDataStream& cStream,
                                           EquipmentHistoryEntry& cEntry) const
{
  QDate cDate;
  QString cComment;
  cStream >> cDate
          >> cComment;
  cEntry.setDate(cDate);
  cEntry.setComment(cComment);
}


//*****************************************************************************
/*!
  Write the equipment history entry \a cEntry to the stream \a cStream.
*/
//*****************************************************************************

void
ScubaLogProject::writeEquipmentHistoryEntry(QDataStream& cStream,
                                            const EquipmentHistoryEntry& cEntry) const
{
  cStream << cEntry.date()
          << cEntry.comment();
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
