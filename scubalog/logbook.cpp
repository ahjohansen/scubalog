//*****************************************************************************
/*!
  \file ScubaLog/logbook.cpp
  \brief This file contains the implementation of the LogBook class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <qapplication.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include "debug.h"
#include "chunkio.h"
#include "divelist.h"
#include "locationlog.h"
#include "equipmentlog.h"
#include "logbook.h"


//*****************************************************************************
/*!
  Create a new log book.

  \author André Johansen.
*/
//*****************************************************************************

LogBook::LogBook()
  : m_pcDiveList(0),
    m_pcLocations(0),
    m_pcEquipment(0)
{
  m_pcDiveList  = new DiveList();
  m_pcLocations = new QList<LocationLog>();
  m_pcEquipment = new QList<EquipmentLog>();
}


//*****************************************************************************
/*!
  Destroy the log book.

  \author André Johansen.
*/
//*****************************************************************************

LogBook::~LogBook()
{
  delete m_pcDiveList;
  m_pcDiveList  = 0;
  delete m_pcLocations;
  m_pcLocations = 0;
  delete m_pcEquipment;
  m_pcEquipment = 0;
}


//*****************************************************************************
/*!
  Read a log book from the file \a cFileName.

  Returns `true' if ok, else `false', in which case the log book might be
  invalid and should be deleted.

  The error-checking should be much better!

  \author André Johansen.
*/
//*****************************************************************************

bool
LogBook::readLogBook(const QString& cFileName)
{
  QFile cFile(cFileName);
  if ( false == cFile.open(IO_ReadOnly) ) {
    QString cMessage("Couldn't open file\n`" + cFileName + "'!");
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Read log book",
                         cMessage);
    return false;
  }
  QDataStream cStream(&cFile);

  unsigned int nChunkId;
  unsigned int nChunkSize;
  unsigned int nChunkVersion;

  // Ensure the file is valid
  cStream >> nChunkId >> nChunkSize >> nChunkVersion;
  if ( (MAKE_CHUNK_ID('S', 'L', 'L', 'B') != nChunkId) ||
       (nChunkSize != cFile.size()) ||
       (1 != nChunkVersion) ) {
    QString cMessage("Couldn't read log book from\n`" + cFileName + "'.\n"
                     "Unknown file format -- probably not a log book!");
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Read log book",
                         cMessage);
    return false;
  }

  // Read all chunks
  while ( false == cStream.eof() ) {
    // Read a chunk header
    cStream >> nChunkId;

    // Read personal information
    if ( MAKE_CHUNK_ID('S', 'L', 'P', 'I') == nChunkId ) {
      cStream >> nChunkSize >> nChunkVersion;
      if ( 1 == nChunkVersion ) {
        cStream >> m_cDiverName
                >> m_cEmailAddress
                >> m_cWwwUrl
                >> m_cComments;
      }
      else
        return false;
    }

    // Read a dive log
    else if ( MAKE_CHUNK_ID('S', 'L', 'D', 'L') == nChunkId ) {
      int nPos = cFile.at();
      DiveLog* pcLog = 0;
      try {
        pcLog = new DiveLog();
        cStream >> *pcLog;
      }
      catch ( IOException& cException) {
        QString cText("Error while reading log book from\n`" + cFileName +
                      "':\n" + cException.explanation());
        QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Read log book",
                             cText);
        cFile.at(nPos);
        cStream >> nChunkSize >> nChunkVersion;
        cFile.at( nPos + nChunkSize - sizeof(unsigned int) );
        DBG(("Position of next chunk=%d\n", cFile.at()));
        delete pcLog;
        continue;
      }
      if ( pcLog ) {
        DBG(("Read log %d\n", pcLog->logNumber()));
        m_pcDiveList->inSort(pcLog);
      }
    }

    // Read a location log
    else if ( MAKE_CHUNK_ID('S', 'L', 'L', 'L') == nChunkId ) {
      int nPos = cFile.at();
      LocationLog* pcLog = 0;
      try {
        pcLog = new LocationLog();
        cStream >> *pcLog;
      }
      catch ( IOException& cException) {
        QString cText("Error while reading log book from\n`" + cFileName +
                      "':\n" + cException.explanation());
        QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Read log book",
                             cText);
        cFile.at(nPos);
        cStream >> nChunkSize >> nChunkVersion;
        cFile.at( nPos + nChunkSize - sizeof(unsigned int) );
        DBG(("Position of next chunk=%d\n", cFile.at()));
        delete pcLog;
        continue;
      }
      if ( pcLog ) {
        DBG(("Read location log for `%s'\n", pcLog->getName().data()));
        m_pcLocations->inSort(pcLog);
      }
    }

    // Read an equipment entry
    else if ( MAKE_CHUNK_ID('S', 'L', 'E', 'L') == nChunkId ) {
      int nPos = cFile.at();
      EquipmentLog* pcLog = 0;
      try {
        pcLog = new EquipmentLog();
        cStream >> *pcLog;
      }
      catch ( IOException& cException) {
        QString cText("Error while reading log book from\n`" + cFileName +
                      "':\n" + cException.explanation());
        QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Read log book",
                             cText);
        cFile.at(nPos);
        cStream >> nChunkSize >> nChunkVersion;
        cFile.at( nPos + nChunkSize - sizeof(unsigned int) );
        DBG(("Position of next chunk=%d\n", cFile.at()));
        delete pcLog;
        continue;
      }
      m_pcEquipment->append(pcLog);
    }

    // Skip unknown chunk
    else {
      cStream >> nChunkSize >> nChunkVersion;
      cFile.at( cFile.at() + nChunkSize - 3 * sizeof(unsigned int) );
    }
  }

  bool isOk = IO_Ok == cFile.status();
  if ( false == isOk ) {
    QString cMessage("Error reading from file\n`" + cFileName + "'!");
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Read log book",
                         cMessage);
  }

  return isOk;
}


//*****************************************************************************
/*!
  Save the log book to the file \a cFileName.

  Returns `true' if ok, else `false'. Only I/O errors like permission denied
  and out of space should result in errors.

  The error-checking should be much better!

  \author André Johansen.
*/
//*****************************************************************************

bool
LogBook::saveLogBook(const QString& cFileName)
{
  QFile cFile(cFileName);
  if ( false == cFile.open(IO_WriteOnly) ) {
    QString cMessage("Couldn't open file\n`" + cFileName + "'.\n"
                     "Ensure that you have write permission and\n"
                     "that there is enough free space on the media!");
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Write log book",
                         cMessage);
    return false;
  }
  QDataStream cStream(&cFile);

  unsigned int nChunkSize;
  unsigned int nChunkVersion;

  try {
    // Write file header (size must be updated later)
    nChunkSize    = 0;
    nChunkVersion = 1;
    cStream << MAKE_CHUNK_ID('S', 'L', 'L', 'B')
            << nChunkSize
            << nChunkVersion;

    // Write personal information
    nChunkSize =
      3 * sizeof(unsigned int)
      + sizeof(unsigned int) + m_cDiverName.size()
      + sizeof(unsigned int) + m_cEmailAddress.size()
      + sizeof(unsigned int) + m_cWwwUrl.size()
      + sizeof(unsigned int) + m_cComments.size();
    nChunkVersion = 1;
    cStream << MAKE_CHUNK_ID('S', 'L', 'P', 'I')
            << nChunkSize
            << nChunkVersion
            << m_cDiverName
            << m_cEmailAddress
            << m_cWwwUrl
            << m_cComments;

    // Write all the dive logs
    const DiveLog* pcLog = m_pcDiveList->first();
    for ( ; pcLog; pcLog = m_pcDiveList->next() ) {
      cStream << *pcLog;
    }

    // Write all the location logs
    const LocationLog* pcLocationLog = m_pcLocations->first();
    for ( ; pcLocationLog; pcLocationLog = m_pcLocations->next() ) {
      cStream << *pcLocationLog;
    }

    // Write all the equipment entries
    const EquipmentLog* pcEquipment;
    for ( pcEquipment = m_pcEquipment->first(); pcEquipment;
          pcEquipment = m_pcEquipment->next() ) {
      cStream << *pcEquipment;
    }
  }
  catch ( IOException& cException ) {
    QString cText("Error while writing log book to\n`" + cFileName + "':\n" +
                  cException.explanation() + "\n"
                  "Ensure there is enough free space on the media!");
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Write log book",
                         cText);
    QFile::remove(cFileName);
    return false;
  }

  // Update file size
  cFile.at(4);
  nChunkSize = cFile.size();
  cStream << nChunkSize;

  bool isOk = IO_Ok == cFile.status();
  cFile.close();
  if ( false == isOk ) {
    QFile::remove(cFileName);
    QString cMessage("Error writing to file\n`" + cFileName + "'!");
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Write log book",
                         cMessage);
  }


  return isOk;
}


//*****************************************************************************
/*!
  Get the divelist.

  \author André Johansen.
*/
//*****************************************************************************

DiveList&
LogBook::diveList()
{
  return *m_pcDiveList;
}


//*****************************************************************************
/*!
  Get the divelist.

  \author André Johansen.
*/
//*****************************************************************************

const DiveList&
LogBook::diveList() const
{
  return *m_pcDiveList;
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
