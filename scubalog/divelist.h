//*****************************************************************************
/*!
  \file divelist.h
  \brief This file contains the definition of the DiveList class.

  This file is part of Scuba Log, a dive logging application for KDE.
  Scuba Log is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen 1999.
*/
//*****************************************************************************

#ifndef DIVELIST_H
#define DIVELIST_H

#include <qlist.h>
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
    setAutoDelete(true);
  }

protected:
  //! Compare the two items \a pcLog1 and \a pcLog2 in the list.
  virtual int compareItems(DiveLog* pcLog1, DiveLog* pcLog2) {
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
