//*****************************************************************************
/*!
  \file divelist.h
  \brief This file contains the definition of the DiveList class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef DIVELIST_H
#define DIVELIST_H

#include "debug.h"
#include "divelog.h"
#include <qlist.h>
#include <QtAlgorithms>


//! Compare the two items \a pcItem1 and \a pcItem2 in the list.
static bool compareDiveLogItems(const DiveLog* pcLog1, const DiveLog* pcLog2)
{
  DBG(("compareDiveLogItems(): %d and %d => %s\n",
       pcLog1->logNumber(), pcLog2->logNumber(),
       pcLog1->logNumber() < pcLog2->logNumber() ? "less" : "!less"));
  return pcLog1->logNumber() < pcLog2->logNumber();
}


//*****************************************************************************
/*!
  \class DiveList
  \brief The DiveList class is the list used to hold the dive logs.

  This class is basically a wrapper around a QList.

  \author André Johansen
*/
//*****************************************************************************

class DiveList : public QList<DiveLog*> {
public:
  //! Initialise the list.
  DiveList() : QList<DiveLog*>() {
    DBG(("DiveList::DiveList()\n"));
  }

  void sort()
  {
    qSort(this->begin(), this->end(), compareDiveLogItems);
  }

private:
  //! Disabled copy constructor.
  DiveList(const DiveList&);
  //! Disabled assignment operator.
  DiveList& operator =(const DiveList&);
};

#endif // DIVELIST_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
