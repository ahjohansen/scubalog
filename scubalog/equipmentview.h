//*****************************************************************************
/*!
  \file equipmentview.h
  \brief This file contains the definition of the EquipmentView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef EQUIPMENTVIEW_H
#define EQUIPMENTVIEW_H

#include <qwidget.h>

class QString;
class Q3ListBox;
class QPushButton;
class QLineEdit;
class LogBook;
class Q3Table;
class QMenu;

//*****************************************************************************
/*!
  \class EquipmentView
  \brief The EquipmentView class is used to edit equipment related parts.

  A log book may contain descriptions of the divers equipment, together
  with a history, service requirements etc.

  \author André Johansen
*/
//*****************************************************************************

class EquipmentView : public QWidget {
  Q_OBJECT
public:
  EquipmentView(QWidget* pcParent);
  virtual ~EquipmentView();

  void setLogBook(LogBook* pcLogBook);

private slots:
  void newItem();
  void deleteItem();
  void changeItemName();
  void editItemName(int nItemNumber);
  void itemSelected(int nItemNumber);
  void itemTypeChanged(const QString& cType);
  void itemSerialChanged(const QString& cSerial);
  void itemServiceChanged(const QString& cService);
  void logEntryChanged(int nRow, int nCol);
  void newLogEntry();
  void deleteLogEntry();
  void moveCurrentUp();
  void moveCurrentDown();
  void showLogEntryMenu(int i_nRow, int i_nCol, const QPoint& i_cPos);

private:
  void createLogEntryMenu();

private:
  //! The log book.
  LogBook*       m_pcLogBook;
  //! The equipment list view.
  Q3ListBox*      m_pcItemView;
  //! The new item button.
  QPushButton*   m_pcNew;
  //! The delete item button.
  QPushButton*   m_pcDelete;
  //! The move item up button.
  QPushButton*   m_pcMoveUp;
  //! The move item down button.
  QPushButton*   m_pcMoveDown;
  //! The item name editor.
  QLineEdit*     m_pcItemName;
  //! The type editor.
  QLineEdit*     m_pcType;
  //! The serial number editor.
  QLineEdit*     m_pcSerial;
  //! The service editor.
  QLineEdit*     m_pcService;
  //! The log edit view.
  Q3Table*       m_pcLogView;
  //! The context menu used in the log for an entry.
  QMenu*         m_pcLogEntryMenu;
};

#endif // EQUIPMENTVIEW_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
