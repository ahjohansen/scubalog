//*****************************************************************************
/*!
  \file equipmentlog.cpp
  \brief This file contains the implementation of the EquipmentLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "equipmentlog.h"

//*****************************************************************************
/*!
  Create a log.
*/
//*****************************************************************************

EquipmentLog::EquipmentLog()
{
}


//*****************************************************************************
/*!
  Destroy the log.
*/
//*****************************************************************************

EquipmentLog::~EquipmentLog()
{
  // Delete the history entries
  while ( !m_cHistory.isEmpty() ) {
    delete m_cHistory.takeFirst();
  }
}


//*****************************************************************************
/*!
  Create an history entry.
*/
//*****************************************************************************

EquipmentHistoryEntry::EquipmentHistoryEntry()
{
}


//*****************************************************************************
/*!
  Destroy the history entry.
*/
//*****************************************************************************

EquipmentHistoryEntry::~EquipmentHistoryEntry()
{
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
