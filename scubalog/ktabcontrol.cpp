//*****************************************************************************
/*!
  \file ktabcontrol.cpp
  \brief This file contains the implementation of the KTabControl class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "ktabcontrol.h"


//*****************************************************************************
/*!
  Create a new tab control widget with \a pcParent as the parent widget
  and \a pzName as the widget name.
*/
//*****************************************************************************

KTabControl::KTabControl(QWidget* pcParent, const char* pzName)
  : KTabCtl(pcParent, pzName)
{
}


//*****************************************************************************
/*!
  Destroy the widget.
*/
//*****************************************************************************

KTabControl::~KTabControl()
{
}


//*****************************************************************************
/*!
  Show the page \a pcPage. If the page is not controlled by this widget,
  or a null-pointer is passed, nothing happens.
*/
//*****************************************************************************

void
KTabControl::showPage(QWidget* pcPage)
{
  if ( pcPage ) {
    int nPageNumber = pages.find(pcPage);
    if ( -1 != nPageNumber ) {
      showTab(nPageNumber);
      tabs->setCurrentTab(nPageNumber);
    }
  }
}


//*****************************************************************************
/*!
  Show the page with index \a nPageNumber.
  If the page is not available, or an invalid index is passed, nothing happens.

  The index starts at zero.
*/
//*****************************************************************************

void
KTabControl::showPage(int nPageNumber)
{
  if ( (nPageNumber >= 0) && ((unsigned)nPageNumber < pages.size()) ) {
    showTab(nPageNumber);
    tabs->setCurrentTab(nPageNumber);
  }
}


//*****************************************************************************
/*!
  Get the page with index \a nPageNumber. The index starts at 0.
  If an invalid number is given, 0 is returned.
*/
//*****************************************************************************

QWidget*
KTabControl::page(int nPageNumber) const
{
  QWidget* pcPage = 0;
  if ( (nPageNumber >= 0) && ((unsigned)nPageNumber < pages.size()) )
    pcPage = pages[nPageNumber];
  return pcPage;
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
