//*****************************************************************************
/*!
  \file logbook.cpp
  \brief This file contains the implementation of the LogBook class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
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
  Get the divelist.
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
// coding: utf-8
// End:
