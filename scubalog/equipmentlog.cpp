/*!
  \file ScubaLog/equipmentlog.cpp
  \brief This file contains the implementation of the EquipmentLog class.

  This file is part of Scuba Log, a dive logging application for KDE.
  Scuba Log is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/

#include <qstring.h>
#include <qdatetime.h>
#include <qlist.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include "chunkio.h"
#include "equipmentlog.h"


/*!
  Create a log.
*/

EquipmentLog::EquipmentLog()
{
}


/*!
  Destroy the log.
*/

EquipmentLog::~EquipmentLog()
{
  // Delete the history entries
  for ( EquipmentHistoryEntry* pcEntry = m_cHistory.first();
	pcEntry; pcEntry = m_cHistory.next() ) {
    delete pcEntry;
  }
}


/*!
  Read a log from the stream \a cStream into \a cLog.
  Returns a reference to the stream.
  On error, the exception IOException is thrown.
*/

QDataStream&
operator >>(QDataStream& cStream, EquipmentLog& cLog)
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.at();

  // Read rest of header
  unsigned int nChunkSize;
  unsigned int nChunkVersion;
  cStream >> nChunkSize
          >> nChunkVersion;
  if ( 1 != nChunkVersion ) {
    QString cText;
    cText.sprintf("Unknown equipment log chunk version %d!", nChunkVersion);
    throw IOException(cText);
  }

  // Read the body of the data
  cStream >> cLog.m_cType
	  >> cLog.m_cName
	  >> cLog.m_cSerial
	  >> cLog.m_cServiceRequirements;

  // Read the history entries
  unsigned int nNumEntries;
  cStream >> nNumEntries;
  for ( unsigned int iEntry = 0; iEntry < nNumEntries; iEntry++ ) {
    EquipmentHistoryEntry* pcEntry = new EquipmentHistoryEntry();
    cStream >> *pcEntry;
    cLog.m_cHistory.append(pcEntry);
  }

  // Ensure we're at the correct position in the stream
  const int nNextChunkPos = nPos + nChunkSize - sizeof(unsigned int);
  if ( nNextChunkPos != cDevice.at() ) {
    QString cText;
    cText.sprintf("Unexpected position after reading equipment log!\n"
		  "Current position is %d; expected %d...",
		  cDevice.at(), nNextChunkPos);
    throw IOException(cText);
  }

  return cStream;
}


/*!
  Write the log \a cLog to the stream \a cStream.
  Returns a reference to the stream.
  On error, the exception IOException is thrown.
*/

QDataStream&
operator <<(QDataStream& cStream, const EquipmentLog& cLog)
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.at();

  QListIterator<EquipmentHistoryEntry> iHistoryEntry(cLog.m_cHistory);

  // Calculate the chunk size
  unsigned int nChunkSize =
    3 * sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int) + cLog.m_cType.size()
    + sizeof(unsigned int) + cLog.m_cName.size()
    + sizeof(unsigned int) + cLog.m_cSerial.size()
    + sizeof(unsigned int) + cLog.m_cServiceRequirements.size();
  for ( ; iHistoryEntry.current(); ++iHistoryEntry )
    nChunkSize += 2 * sizeof(unsigned int) +
      iHistoryEntry.current()->comment().size();

  // Write the header
  unsigned int nChunkVersion = (unsigned int)EquipmentLog::e_ChunkVersion;
  cStream << MAKE_CHUNK_ID('S', 'L', 'E', 'L')
	  << nChunkSize
	  << nChunkVersion;

  // Write the body
  cStream << cLog.m_cType
	  << cLog.m_cName
	  << cLog.m_cSerial
	  << cLog.m_cServiceRequirements
	  << cLog.m_cHistory.count();

  // Write the history entries
  iHistoryEntry.toFirst();
  for ( ; iHistoryEntry.current(); ++iHistoryEntry ) {
    const EquipmentHistoryEntry* pcEntry = iHistoryEntry.current();
    cStream << *pcEntry;
  }

  // Ensure we're at the correct position in the stream
  const int nNextChunkPos = nPos + nChunkSize;
  if ( nNextChunkPos != cDevice.at() ) {
    QString cText;
    cText.sprintf("Unexpected position after writing equipment log!\n"
		  "Current position is %d; expected %d...",
		  cDevice.at(), nNextChunkPos);
    throw IOException(cText);
  }

  return cStream;
}



/*!
  Create an history entry.
*/

EquipmentHistoryEntry::EquipmentHistoryEntry()
{
}


/*!
  Destroy the history entry.
*/

EquipmentHistoryEntry::~EquipmentHistoryEntry()
{
}

/*!
  Read an entry from the stream \a cStream into \a cEntry.

  Returns a reference to the stream.

  This function needs better error-handling. Use exceptions!
*/

QDataStream&
operator >>(QDataStream& cStream, EquipmentHistoryEntry& cEntry)
{
  cStream >> cEntry.m_cDate
	  >> cEntry.m_cComment;

  return cStream;
}


/*!
  Write the entry \a cEntry to the stream \a cStream.

  Returns a reference to the stream.
*/

QDataStream&
operator <<(QDataStream& cStream, const EquipmentHistoryEntry& cEntry)
{
  cStream << cEntry.m_cDate
	  << cEntry.m_cComment;

  return cStream;
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
