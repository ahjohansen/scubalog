//*****************************************************************************
/*!
  \file listbox.cpp
  \brief This file contains the implementation of the ListBox class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <qpopupmenu.h>
#include <qkeycode.h>
#include "listbox.h"


//*****************************************************************************
/*!
  Create the widget with \a pcParent as the parent widget and \a pzWidgetName
  as the widget name.

  \author André Johansen.
*/
//*****************************************************************************

ListBox::ListBox(QWidget* pcParent, const char* pzWidgetName)
  : QListBox(pcParent, pzWidgetName),
    m_pcPopupMenu(0)
{
}


//*****************************************************************************
/*!
  Destroy the widget.  The pop-up menu will be destroyed.

  \author André Johansen.
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

  \author André Johansen.
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

  \author André Johansen.
*/
//*****************************************************************************

QPopupMenu*
ListBox::getPopupMenu()
{
  if ( 0 == m_pcPopupMenu )
    m_pcPopupMenu = new QPopupMenu(0, "popup");
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

  \author André Johansen.
*/
//*****************************************************************************

void
ListBox::mousePressEvent(QMouseEvent* pcEvent)
{
  if ( m_pcPopupMenu && RightButton == pcEvent->button() ) {
    int nItem = findItem(pcEvent->pos().y());
    if ( -1 != nItem ) {
      setCurrentItem(nItem);
    }
    emit aboutToShowPopup(m_pcPopupMenu);
    m_pcPopupMenu->popup(mapToGlobal(pcEvent->pos()));
  }
  else {
    QListBox::mousePressEvent(pcEvent);
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

  \author André Johansen.
*/
//*****************************************************************************

void
ListBox::keyPressEvent(QKeyEvent* pcEvent)
{
  if ( m_pcPopupMenu && 0 == pcEvent->state() &&
       (Key_Menu == pcEvent->key() || Key_Escape  == pcEvent->key()) ) {
    QPoint cPos = mapToGlobal(pos());
    cPos.setX(cPos.x() + 2);
    int nCurrentY = 2;
    const int nCurrentItem = currentItem();
    if ( -1 != nCurrentItem ) {
      itemYPos(nCurrentItem, &nCurrentY);
    }
    cPos.setY(cPos.y() + nCurrentY);
    emit aboutToShowPopup(m_pcPopupMenu);
    m_pcPopupMenu->popup(cPos);
  }
  else {
    QListBox::keyPressEvent(pcEvent);
  }
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
