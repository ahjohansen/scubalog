//*****************************************************************************
/*!
  \file ScubaLog/divelog.cpp
  \brief This file contains the implementation of the DiveLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <qstring.h>
#include <qdatetime.h>
#include <qdatastream.h>
#include "chunkio.h"
#include "divelog.h"


//*****************************************************************************
/*!
  Initialize an empty log.
*/
//*****************************************************************************

DiveLog::DiveLog()
{
  m_nLogNumber          = 0;
  m_cDiveDate           = QDate::currentDate();
  m_cDiveStart          = QTime();
  m_cLocation           = "";
  m_cBuddyName          = "";
  m_vMaxDepth           = 0.0F;
  m_cDiveTime           = QTime();
  m_cBottomTime         = QTime();
  m_cGasType            = "Air";
  m_nNumLitresUsed      = 0;
  m_vAirTemperature     = 0.0F;
  m_vSurfaceTemperature = 0.0F;
  m_vWaterTemperature   = 0.0F;
  m_ePlanType           = e_SingleLevel;
  m_cDiveType           = "Nature";
  m_cDiveDescription    = "";
}


//*****************************************************************************
/*!
  Destroy the dive log. All resources will be freed.
*/
//*****************************************************************************

DiveLog::~DiveLog()
{
#if !defined(NDEBUG)
  m_nLogNumber = 0xDEADBEEF;
#endif // NDEBUG
}


//*****************************************************************************
/*!
  Read a log from the stream \a cStream into \a cLog.
  Returns a reference to the stream.
  On error, the exception IOException is thrown.
*/
//*****************************************************************************

QDataStream&
operator >>(QDataStream& cStream, DiveLog& cLog)
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
    cText.sprintf("Unknown dive log chunk version %d!", nChunkVersion);
    throw IOException(cText);
  }

  unsigned char nPlanType;
  cStream >> cLog.m_nLogNumber
          >> cLog.m_cDiveDate
          >> cLog.m_cDiveStart
          >> cLog.m_cLocation
          >> cLog.m_cBuddyName
          >> cLog.m_vMaxDepth
          >> cLog.m_cDiveTime
          >> cLog.m_cBottomTime
          >> cLog.m_cGasType
          >> cLog.m_nNumLitresUsed
          >> cLog.m_vAirTemperature
          >> cLog.m_vSurfaceTemperature
          >> cLog.m_vWaterTemperature
          >> nPlanType
          >> cLog.m_cDiveType
          >> cLog.m_cDiveDescription;

  cLog.setPlanType((DiveLog::PlanType_e)nPlanType);

  // Ensure we're at the correct position in the stream
  const int nNextChunkPos = nPos + nChunkSize - sizeof(unsigned int);
  if ( nNextChunkPos != cDevice.at() ) {
    QString cText;
    cText.sprintf("Unexpected position after reading dive log!\n"
                  "Current position is %d; expected %d...",
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
*/
//*****************************************************************************

QDataStream&
operator <<(QDataStream& cStream, const DiveLog& cLog)
{
  // Save current IO position for checking at end
  const QIODevice& cDevice = *cStream.device();
  const int nPos = cDevice.at();

  const unsigned int nChunkVersion = 1;
  const unsigned int nChunkSize =
    3 * sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int) + cLog.m_cLocation.size()
    + sizeof(unsigned int) + cLog.m_cBuddyName.size()
    + sizeof(float)
    + sizeof(unsigned int)
    + sizeof(unsigned int)
    + sizeof(unsigned int) + cLog.m_cGasType.size()
    + sizeof(unsigned int)
    + sizeof(float)
    + sizeof(float)
    + sizeof(float)
    + sizeof(unsigned char)
    + sizeof(unsigned int) + cLog.m_cDiveType.size()
    + sizeof(unsigned int) + cLog.m_cDiveDescription.size();
  const unsigned char nPlanType = (unsigned char)cLog.m_ePlanType;
  cStream << MAKE_CHUNK_ID('S', 'L', 'D', 'L')
          << nChunkSize
          << nChunkVersion
          << cLog.m_nLogNumber
          << cLog.m_cDiveDate
          << cLog.m_cDiveStart
          << cLog.m_cLocation
          << cLog.m_cBuddyName
          << cLog.m_vMaxDepth
          << cLog.m_cDiveTime
          << cLog.m_cBottomTime
          << cLog.m_cGasType
          << cLog.m_nNumLitresUsed
          << cLog.m_vAirTemperature
          << cLog.m_vSurfaceTemperature
          << cLog.m_vWaterTemperature
          << nPlanType
          << cLog.m_cDiveType
          << cLog.m_cDiveDescription;


  // Ensure we're at the correct position in the stream
  const int nNextChunkPos = nPos + nChunkSize;
  if ( nNextChunkPos != cDevice.at() ) {
    QString cText;
    cText.sprintf("Unexpected position after writing dive log!\n"
                  "Current position is %d; expected %d...",
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
