//*****************************************************************************
/*!
  \file locationlog.cpp
  \brief This file contains the implementation of the LocationLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <assert.h>
#include <qstring.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include <kapp.h>
#include "chunkio.h"
#include "locationlog.h"


//*****************************************************************************
/*!
  Initialize the log.

  By default, both the name and description are empty.

  \author André Johansen.
*/
//*****************************************************************************

LocationLog::LocationLog()
{
}


//*****************************************************************************
/*!
  Destroy the log.

  \author André Johansen.
*/
//*****************************************************************************

LocationLog::~LocationLog()
{
}


//*****************************************************************************
/*!
  Get the location name.

  \author André Johansen.
*/
//*****************************************************************************

QString
LocationLog::getName() const
{
  return m_cName;
}


//*****************************************************************************
/*!
  Set the name of the location to \a cName.

  \author André Johansen.
*/
//*****************************************************************************

void
LocationLog::setName(const QString& cName)
{
  m_cName = cName;
  m_cName.detach();
}


//*****************************************************************************
/*!
  Get the description of the location.

  \author André Johansen.
*/
//*****************************************************************************

QString
LocationLog::getDescription() const
{
  return m_cDescription;
}


//*****************************************************************************
/*!
  Set the description of the location to \a cDescription.

  \author André Johansen.
*/
//*****************************************************************************

void
LocationLog::setDescription(const QString& cDescription)
{
  m_cDescription = cDescription;
  m_cDescription.detach();
}


//*****************************************************************************
/*!
  Read a log from the stream \a cStream into \a cLog.
  Returns a reference to the stream.
  On error, the exception IOException is thrown.

  \author André Johansen.
*/
//*****************************************************************************

QDataStream&
operator >>(QDataStream& cStream, LocationLog& cLog)
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
    cText.sprintf(i18n("Unknown location log chunk version %d!"),
                  nChunkVersion);
    throw IOException(cText);
  }

  // Read the body of the data
  cStream >> cLog.m_cName
          >> cLog.m_cDescription;

  // Ensure we're at the correct position in the stream
  const int nNextChunkPos = nPos + nChunkSize - sizeof(unsigned int);
  if ( nNextChunkPos != cDevice.at() ) {
    QString cText;
    cText.sprintf(i18n("Unexpected position after reading location log!\n"
                       "Current position is %d; expected %d..."),
                  cDevice.at(), nNextChunkPos);
    throw IOException(cText);
  }

  return cStream;
}


//*****************************************************************************
/*!
  Write the log \a cLog to the stream \a cStream.
  Returns a reference to the stream.
  On error, the exception IOException is thrown.

  \author André Johansen.
*/
//*****************************************************************************

QDataStream&
operator <<(QDataStream& cStream, const LocationLog& cLog)
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.at();

  // Calculate the chunk size
  unsigned int nChunkSize =
    3 * sizeof(unsigned int)
    + sizeof(unsigned int) + cLog.m_cName.size()
    + sizeof(unsigned int) + cLog.m_cDescription.size();

  // Write the header
  unsigned int nChunkVersion = (unsigned int)LocationLog::e_ChunkVersion;
  cStream << MAKE_CHUNK_ID('S', 'L', 'L', 'L')
          << nChunkSize
          << nChunkVersion;

  // Write the body
  cStream << cLog.m_cName
          << cLog.m_cDescription;

  // Ensure we're at the correct position in the stream
  const int nNextChunkPos = nPos + nChunkSize;
  if ( nNextChunkPos != cDevice.at() ) {
    QString cText;
    cText.sprintf(i18n("Unexpected position after writing location log!\n"
                       "Current position is %d; expected %d..."),
                  cDevice.at(), nNextChunkPos);
    throw IOException(cText);
  }

  return cStream;
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
