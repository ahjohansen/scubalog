//*****************************************************************************
/*!
  \file locationlog.cpp
  \brief This file contains the implementation of the LocationLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include "locationlog.h"
#include <qstring.h>
#include <assert.h>
#include <stdio.h>

//*****************************************************************************
/*!
  Initialise the log.

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
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
