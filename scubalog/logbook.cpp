//*****************************************************************************
/*!
  \file logbook.cpp
  \brief This file contains the implementation of the LogBook class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <qfile.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include <kapp.h>
#include <klocale.h>
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

  Notice that out-of-memory exceptions (std::bad_alloc)
  should be caught from the outside!

  \author André Johansen.
*/
//*****************************************************************************

bool
LogBook::readLogBook(const QString& cFileName) throw(std::bad_alloc)
{
  QFile cFile(cFileName);
  if ( false == cFile.open(IO_ReadOnly) ) {
    QString cMessage;
    cMessage = QString(i18n("Couldn't open file"))
      + "\n`" + cFileName + "'!";
    QMessageBox::warning(qApp->mainWidget(), i18n("[ScubaLog] Read log book"),
                         cMessage);
    return false;
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
    QMessageBox::warning(qApp->mainWidget(), i18n("[ScubaLog] Read log book"),
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
        QString cText;
        cText = QString(i18n("Error while reading log book from"))
          + "\n`" + cFileName + "':\n" + cException.explanation();
        QMessageBox::warning(qApp->mainWidget(),
                             i18n("[ScubaLog] Read log book"), cText);
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
        QString cText;
        cText = QString(i18n("Error while reading log book from"))
          + "\n`" + cFileName + "':\n" + cException.explanation();
        QMessageBox::warning(qApp->mainWidget(),
                             i18n("[ScubaLog] Read log book"), cText);
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
        QString cText;
        cText = QString(i18n("Error while reading log book from"))
          + "\n`" + cFileName + "':\n" + cException.explanation();
        QMessageBox::warning(qApp->mainWidget(),
                             i18n("[ScubaLog] Read log book"), cText);
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
      const int nOldPos = cFile.at();
      const int nNewPos = nOldPos + nChunkSize - 3 * sizeof(unsigned int);
      if ( nNewPos <= nOldPos || nNewPos < 0 ||
           (unsigned)nNewPos >= nFileSize ) {
        QString cText;
        cText = QString(i18n("Can't read further, aborting load of log-book"));
        QMessageBox::warning(qApp->mainWidget(),
                             i18n("[ScubaLog] Read log book"), cText);
        break;
      }
      cFile.at(nNewPos);
    }
  }

  bool isOk = IO_Ok == cFile.status();
  if ( false == isOk ) {
    QString cMessage;
    cMessage = QString(i18n("Error reading from file"))
      + "\n`" + cFileName + "'!";
    QMessageBox::warning(qApp->mainWidget(), i18n("[ScubaLog] Read log book"),
                         cMessage);
  }

  return isOk;
}


//*****************************************************************************
/*!
  Save the log book to the file \a cFileName.

  Returns `true' if ok, else `false'. Only I/O errors like permission denied
  and out of space should result in errors.

  \author André Johansen.
*/
//*****************************************************************************

bool
LogBook::saveLogBook(const QString& cFileName) throw()
{
  QFile cFile(cFileName);
  if ( false == cFile.open(IO_WriteOnly) ) {
    QString cMessage;
    cMessage = QString(i18n("Couldn't open file"))
      + "\n`" + cFileName + "'.\n"
      + i18n("Ensure that you have write permission and\n"
             "that there is enough free space on the media!");
    QMessageBox::warning(qApp->mainWidget(), i18n("[ScubaLog] Write log book"),
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

    // Write personal information
    nChunkSize =
      3 * sizeof(unsigned int)
      + sizeof(unsigned int) + m_cDiverName.length()
      + sizeof(unsigned int) + m_cEmailAddress.length()
      + sizeof(unsigned int) + m_cWwwUrl.length()
      + sizeof(unsigned int) + m_cComments.length();
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
    QString cText;
    cText = QString(i18n("Error while writing log book to"))
      + "\n`" + cFileName + "':\n" + cException.explanation() + "\n"
      + i18n("Ensure there is enough free space on the media!");
    QMessageBox::warning(qApp->mainWidget(), i18n("[ScubaLog] Write log book"),
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
    QString cMessage;
    cMessage = QString(i18n("Error writing to file"))
      + "\n`" + cFileName + "'!";
    QMessageBox::warning(qApp->mainWidget(), i18n("[ScubaLog] Write log book"),
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

DiveList&
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
