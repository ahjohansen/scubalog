//*****************************************************************************
/*!
  \file listbox.h
  \brief This file contains the definition of the ListBox class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef LISTBOX_H
#define LISTBOX_H

#include <qlistbox.h>

class QPopupMenu;

//*****************************************************************************
/*!
  \class ListBox
  \brief The ListBox class is used to show a list of items.

  The class extends the QListBox widget in Qt by providing the possibility
  to have a pop-up menu related to the widget.

  By default, the menu is accessed with right mouse-button, the menu key or
  the esc key.

  When the pop-up menu is about to be shown, the signal aboutToShowPopup() is
  emitted, with the pop-up menu as an argument.

  When showing the menu, is will be positioned at the current mouse-position
  if accessed with the right mouse button, at the current item if accessed with
  a key, or at the top-left corner if accessed with a key when no item is
  current.

  Notice that the pop-up menu is owned by this widget, it can be changed by
  users, but not deleted.

  \author André Johansen.
*/
//*****************************************************************************

class ListBox : public QListBox {
  Q_OBJECT

public:
  ListBox(QWidget* pcParent = 0, const char* pzWidgetName = 0);
  ~ListBox();

  bool hasPopupMenu() const;
  QPopupMenu* getPopupMenu();

protected:
  virtual void mousePressEvent(QMouseEvent* pcEvent);
  virtual void keyPressEvent(QKeyEvent* pcEvent);

private:
  //! The pop-up menu used by the widget, if any.
  QPopupMenu* m_pcPopupMenu;

signals:
  //! Emitted just before the pop-up menu \a pcMenu is shown.
  void aboutToShowPopup(QPopupMenu* pcMenu);
};

#endif // LISTBOX_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
