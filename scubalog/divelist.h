//*****************************************************************************
/*!
  \file divelist.h
  \brief This file contains the definition of the DiveList class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef DIVELIST_H
#define DIVELIST_H

#include <qlist.h>
#include "debug.h"
#include "divelog.h"


//*****************************************************************************
/*!
  \class DiveList
  \brief The DiveList class is the list used to hold the dive logs.

  This class is basically a wrapper around a QList.

  \author André Johansen.
*/
//*****************************************************************************

class DiveList : public QList<DiveLog> {
public:
  //! Initialize the list.
  DiveList() : QList<DiveLog>::QList<DiveLog>() {
    DBG(("DiveList::DiveList()\n"));
    setAutoDelete(true);
  }

protected:
  //! Compare the two items \a pcItem1 and \a pcItem2 in the list.
  virtual int compareItems(QCollection::Item pcItem1,
                           QCollection::Item pcItem2)
  {
    DiveLog* pcLog1 = static_cast<DiveLog*>(pcItem1);
    DiveLog* pcLog2 = static_cast<DiveLog*>(pcItem2);
    DBG(("compareItems(): %d and %d => %d\n",
         pcLog1->logNumber(), pcLog2->logNumber(),
         pcLog1->logNumber() - pcLog2->logNumber()));
    return ( pcLog1->logNumber() - pcLog2->logNumber() );
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
// End:
