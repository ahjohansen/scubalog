//*****************************************************************************
/*!
  \file listbox.cpp
  \brief This file contains the implementation of the ListBox class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "listbox.h"
#include <qmenu.h>
#include <qnamespace.h>
#include <QMouseEvent>
#include <QKeyEvent>


//*****************************************************************************
/*!
  Create the widget with \a pcParent as the parent widget.
*/
//*****************************************************************************

ListBox::ListBox(QWidget* pcParent)
  : Q3ListBox(pcParent),
    m_pcPopupMenu(0)
{
}


//*****************************************************************************
/*!
  Destroy the widget.  The pop-up menu will be destroyed.
*/
//*****************************************************************************

ListBox::~ListBox()
{
  delete m_pcPopupMenu;
  m_pcPopupMenu = 0;
}


//*****************************************************************************
/*!
  Check whether the widget has a pop-up menu or not.

  The pop-up menu will be created the first time getPopupMenu() is called.
*/
//*****************************************************************************

bool
ListBox::hasPopupMenu() const
{
  return 0 != m_pcPopupMenu;
}


//*****************************************************************************
/*!
  Get the pop-up menu used by the widget.

  The menu is created the first time this function is called.
*/
//*****************************************************************************

QMenu*
ListBox::getPopupMenu()
{
  if ( 0 == m_pcPopupMenu )
    m_pcPopupMenu = new QMenu(0);
  return m_pcPopupMenu;
}


//*****************************************************************************
/*!
  Handle mouse press events, with event information taken from \a pcEvent.

  If the right mouse button is pressed, and a menu is available,
  it will be shown at the current mouse-position.  In that case, the signal
  aboutToShowPopup() will be emitted first, so the user can change the
  menu to reflect current state.

  All other events are sent to the base-class.
*/
//*****************************************************************************

void
ListBox::mousePressEvent(QMouseEvent* pcEvent)
{
  if ( m_pcPopupMenu && Qt::RightButton == pcEvent->button() ) {
    int nItem = findItem(pcEvent->pos().y());
    if ( -1 != nItem ) {
      setCurrentItem(nItem);
    }
    emit aboutToShowPopup(m_pcPopupMenu);
    m_pcPopupMenu->popup(mapToGlobal(pcEvent->pos()));
  }
  else {
    Q3ListBox::mousePressEvent(pcEvent);
  }
}


//*****************************************************************************
/*!
  Handle key press events, with event information taken from \a pcEvent.

  If the menu key or the esc key is pressed, and a menu is available,
  it will be shown at the current item if any, else at the top-left of the
  widget.  In that case, the signal aboutToShowPopup() will be emitted first,
  so the user can change the menu to reflect current state.

  All other events are sent to the base-class.
*/
//*****************************************************************************

void
ListBox::keyPressEvent(QKeyEvent* pcEvent)
{
  if ( m_pcPopupMenu && Qt::NoModifier == pcEvent->modifiers() &&
       (Qt::Key_Menu == pcEvent->key() || Qt::Key_Escape  == pcEvent->key()) ) {
    QPoint cPos = mapToGlobal(pos());
    cPos.setX(cPos.x() + 2);
    int nCurrentY = 2;
    const int nCurrentItem = currentItem();
    if ( -1 != nCurrentItem ) {
      // ### Find new solution
      //itemYPos(nCurrentItem, &nCurrentY);
    }
    cPos.setY(cPos.y() + nCurrentY);
    emit aboutToShowPopup(m_pcPopupMenu);
    m_pcPopupMenu->popup(cPos);
  }
  else {
    Q3ListBox::keyPressEvent(pcEvent);
  }
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
