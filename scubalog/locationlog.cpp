//*****************************************************************************
/*!
  \file locationlog.cpp
  \brief This file contains the implementation of the LocationLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
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
*/
//*****************************************************************************

LocationLog::LocationLog()
{
}


//*****************************************************************************
/*!
  Destroy the log.
*/
//*****************************************************************************

LocationLog::~LocationLog()
{
}


//*****************************************************************************
/*!
  Get the location name.
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
// coding: utf-8
// End:
