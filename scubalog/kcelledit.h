/*!
  \file ScubaLog/widgets/kcelledit.h
  \brief This file contains the definition of KCellEdit.

  $Id$

  \par Copyright:
  André Johansen.
*/

#ifndef KCELLEDIT_H
#define KCELLEDIT_H

#include <qstring.h>
#include <qtableview.h>

class QLineEdit;

class KCellEdit : public QTableView {
  Q_OBJECT
public:
  KCellEdit(int nNumCols, QWidget* pcParent = 0, const char* pzName = 0);
  virtual ~KCellEdit();

  virtual bool eventFilter(QObject* pcReceiver, QEvent* pcEvent);

  void setColEditor(int nCol, QLineEdit* pcEditor);
  void setCellText(int nRow, int nCol, const QString& cText, bool bUpdateCell);
  virtual void clear();

public slots:
  virtual void setColumnWidth(int nColumn, int nOldWidth, int nNewWidth);

protected:
  virtual int cellWidth(int nColumn);
  virtual void mousePressEvent(QMouseEvent* pcEvent);
  virtual void mouseDoubleClickEvent(QMouseEvent* pcEvent);
  virtual void keyPressEvent(QKeyEvent* pcEvent);
  virtual void paintCell(QPainter* pcPainter, int nRow, int nCol);
  virtual void addRow();

protected slots:
  virtual void horizontalScroll(int nNewX);
  virtual void verticalScroll(int nNewY);

private slots:
  virtual int totalWidth();
  void editCell();
  void finishEdit(bool bKeepResult);

private:
  //! Forward declare class to avoid STL inclusion here.
  class StringContainer;

  //! The current active row, -1 if none.
  int              m_nActiveRow;
  //! The current active column, -1 if none.
  int              m_nActiveCol;
  //! The column widths.
  int*             m_panColWidths;
  //! Set to `true' when editing.
  bool             m_isEditing;
  //! The editor widgets.
  QLineEdit**      m_papcEditors;
  //! The cell strings.
  StringContainer* m_pcCellStrings;

signals:
  /*!
    This signal is emitted when the cell (\a nRow, \a nCol) has changed
    text to \a cText.
  */
  void textChanged(int nRow, int nCol, const QString& cText);
};

#endif // KCELLEDIT_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
