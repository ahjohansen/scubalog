//*****************************************************************************
/*!
  \file ScubaLog/widgets/ktabcontrol.h
  \brief This file contains the definition of the KTabControl class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef KTABCONTROL_H
#define KTABCONTROL_H

#include <ktabctl.h>


//*****************************************************************************
/*!
  \class KTabControl
  \brief The KTabControl class is used to control pages with a tabbar.

  This class is provided to work around some of the missing features of
  KTabCtl, which this class inherits. The best thing would probably be
  to write this class from scratch, as a replacement for KTabCtl.

  \author André Johansen.
*/  
//*****************************************************************************

class KTabControl : public KTabCtl {
public:
  KTabControl(QWidget* pcParent = 0, const char* pzName = 0);
  virtual ~KTabControl();

  void showPage(QWidget* pcPage);
  void showPage(int nPageNumber);

  QWidget* page(int nPageNumber) const;
};

#endif // KTABCONTROL_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
