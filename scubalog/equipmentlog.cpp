//*****************************************************************************
/*!
  \file equipmentlog.cpp
  \brief This file contains the implementation of the EquipmentLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  Andr� Johansen.
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
  for ( EquipmentHistoryEntry* pcEntry = m_cHistory.first();
        pcEntry; pcEntry = m_cHistory.next() ) {
    delete pcEntry;
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
// End:
