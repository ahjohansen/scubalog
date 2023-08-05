//*****************************************************************************
/*!
  \file divelog.cpp
  \brief This file contains the implementation of the DiveLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Hübert Johansen
*/
//*****************************************************************************

#include "divelog.h"
#include "chunkio.h"
#include <KLocalizedString>
#include <qdatastream.h>
#include <qdatetime.h>
#include <qstring.h>


//*****************************************************************************
/*!
  Initialise an empty log.
*/
//*****************************************************************************

DiveLog::DiveLog()
  : m_nLogNumber(0),
    m_cDiveDate(QDate::currentDate()),
    m_cDiveStart(QTime()),
    m_cLocation(""),
    m_cBuddyName(""),
    m_vMaxDepth(0.0F),
    m_cDiveTime(QTime()),
    m_cBottomTime(QTime()),
    m_cGasType("Air"),
    m_nNumLitresUsed(0),
    m_vAirTemperature(0.0F),
    m_vSurfaceTemperature(0.0F),
    m_vWaterTemperature(0.0F),
    m_ePlanType(e_SingleLevel),
    m_cDiveType(i18n("Nature")),
    m_cDiveDescription("")
{
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


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
