/*!
  \file widget/kcelledit.cpp
  \brief This file contains the implementation of KCellEdit

  $Id$

  \par Copyright:
  André Johansen.
*/

#include <stdio.h>
#include <assert.h>
#include <vector>
#include <qpen.h>
#include <qpainter.h>
#include <qkeycode.h>
#include <qfontinfo.h>
#include <qlineedit.h>
#include <qscrollbar.h>
#include "kcelledit.h"
#include "kcelledit.moc"


class KCellEdit::StringContainer : public std::vector<QString> {};

/*!
  Create a view with \a nNumCols columns, \a pcParent as parent widget
  and \a pzName as widget name.

  Initially, the view will have no rows.
*/

KCellEdit::KCellEdit(int nNumCols, QWidget* pcParent, const char* pzName)
  : QTableView(pcParent, pzName), m_panColWidths(0), m_papcEditors(0),
    m_pcCellStrings(0)
{
  assert(nNumCols >= 1);

  m_nActiveRow = m_nActiveCol = -1;
  m_isEditing = false;

  m_panColWidths = new int[nNumCols];
  m_papcEditors = new QLineEdit*[nNumCols];
  for ( int iCol = 0; iCol < nNumCols; iCol++ ) {
    m_panColWidths[iCol] = 50;
    m_papcEditors[iCol] = 0;
  }

  m_pcCellStrings = new StringContainer();

  const QScrollBar* pcHScrollBar = horizontalScrollBar();
  connect(pcHScrollBar, SIGNAL(valueChanged(int)),
	  SLOT(horizontalScroll(int)));
  connect(pcHScrollBar, SIGNAL(sliderMoved(int)),
	  SLOT(horizontalScroll(int)));
  const QScrollBar* pcVScrollBar = verticalScrollBar();
  connect(pcVScrollBar, SIGNAL(valueChanged(int)),
	  SLOT(verticalScroll(int)));
  connect(pcVScrollBar, SIGNAL(sliderMoved(int)),
	  SLOT(verticalScroll(int)));

  // Set cell size
  setCellWidth(0);
  QFontInfo cFontInfo(font());
  setCellHeight(cFontInfo.pointSize() + 10);

  setBackgroundColor(QColor(white));
  setTableFlags(Tbl_autoScrollBars | Tbl_smoothScrolling);
  setNumCols(nNumCols);
  setFocusPolicy(StrongFocus);
}


/*!
  Destroy the widget and delete all allocated resources.

  Notice that the cell editors obtained with setColEditor() will
  be destoyed with this widget.
*/

KCellEdit::~KCellEdit()
{
  delete m_panColWidths;
  delete m_papcEditors;
  delete m_pcCellStrings;
}


/*!
  Clear the view. All the saved texts will be cleared.

  Notice that the number of rows will be set to 0;
*/

void
KCellEdit::clear()
{
  m_pcCellStrings->erase(m_pcCellStrings->begin(), m_pcCellStrings->end());
  m_nActiveRow = m_nActiveCol = -1;
  setNumRows(0);
}


/*!
  Add a row to the view. The new cells will start out as empty.
*/

void
KCellEdit::addRow()
{
  setNumRows(numRows()+1);
  while ( m_pcCellStrings->size() < (unsigned)numRows()*numCols() )
    m_pcCellStrings->push_back(QString());
}


/*!
  Set the editor to be used for the column \a nCol to \a pcEditor.
  If that column already had an editor, it is deleted.

  The editor must be of type QLineEdit or a derived class.

  This widget will take ownership of the editor.
*/

void
KCellEdit::setColEditor(int nCol, QLineEdit* pcEditor)
{
  assert(nCol >= 0 && nCol < numCols());
  assert(pcEditor);
  pcEditor->hide();
  pcEditor->recreate(this, 0, QPoint());
  if ( m_papcEditors[nCol] )
    delete m_papcEditors[nCol];
  m_papcEditors[nCol] = pcEditor;
}


/*!
 Set the string for the cell at (\a nRow, \a nCol) to be \a cCellText.
 If \a bUpdateCell is `true', the cell will be redrawn, if not,
 it should be so later on. This can be useful if you want to set the text
 for several cells in one go and have them all updated at the end by
 calling repaint().

 Notice that this call might generate new rows -- if the row is not available,
 all missing rows will be inserted as empty.
*/

void
KCellEdit::setCellText(int nRow, int nCol, const QString& cText,
		       bool bUpdateCell)
{
  assert(nRow >= 0 && nCol >= 0 && nCol < numCols());

  // Insert missing cells
  int nMissingTextCells = (nRow+1)*numCols() - m_pcCellStrings->size();
  while ( nMissingTextCells-- > 0 )
    m_pcCellStrings->push_back(QString());

  (*m_pcCellStrings)[nRow*numCols()+nCol] = cText;
#if QT_VERSION < 200
  (*m_pcCellStrings)[nRow*numCols()+nCol].detach();
#endif // QT_VERSION

  if ( nRow >= numRows() ) {
    if ( false == bUpdateCell )
      setAutoUpdate(false);
    setNumRows(nRow+1);
    if ( false == bUpdateCell )
      setAutoUpdate(true);
  }

  if ( bUpdateCell )
    updateCell(nRow, nCol);
}


/*!
  Handle keypress events. The event information is taken from \a pcEvent.

  The keys handled (all others will be ignored):
  \arg Cursor left - move one column left, or to the last column in the
  previous row if positioned at the first column.
  \arg Cursor right - move one column right, or to the first column in the
  next row if positioned at the last column.
  \arg Cursor up - move one row up.
  \arg Cursor down - move one row down.
  \arg Enter and return - try to edit the current cell.

  Rows will be added as needed.
*/

void
KCellEdit::keyPressEvent(QKeyEvent* pcEvent)
{
  assert(0 != pcEvent);
  assert((-1 == m_nActiveRow && -1 == m_nActiveCol) ||
	 (0 <= m_nActiveRow && numRows() > m_nActiveRow &&
	  0 <= m_nActiveCol && numCols() > m_nActiveCol));

  const int nOldRow = m_nActiveRow;
  const int nOldCol = m_nActiveCol;

  if ( m_isEditing || pcEvent->state() ) {
    pcEvent->ignore();
    return;
  }

  switch ( pcEvent->key() ) {
  case Key_Left:
    // Go to the first cell
    if ( -1 == m_nActiveRow && -1 == m_nActiveCol ) {
      if ( 0 == numRows() )
	addRow();
      m_nActiveRow = m_nActiveCol = 0;
    }
    // Move one column left
    else if ( m_nActiveCol >= 1 )
      m_nActiveCol--;
    // Move to last column one row up
    else if ( m_nActiveRow >= 1 ) {
      m_nActiveCol = numCols() - 1;
      m_nActiveRow--;
    }
    break;

  case Key_Right:
    // Go to the first cell
    if ( -1 == m_nActiveRow && -1 == m_nActiveCol ) {
      if ( 0 == numRows() )
	addRow();
      m_nActiveRow = m_nActiveCol = 0;
    }
    // Move one column right
    else if ( m_nActiveCol < numCols() - 1 )
      m_nActiveCol++;
    // Move to first column one row down
    else {
      while ( m_nActiveRow + 1 >= numRows() )
	addRow();
      m_nActiveCol = 0;
      m_nActiveRow++;
    }
    break;

  case Key_Up:
    // Go to the first cell
    if ( -1 == m_nActiveRow && -1 == m_nActiveCol ) {
      if ( 0 == numRows() )
	addRow();
      m_nActiveRow = m_nActiveCol = 0;
    }
    // Move one row up
    else if ( m_nActiveRow >= 1 )
      m_nActiveRow--;
    break;

  case Key_Down:
    // Go to the first cell
    if ( -1 == m_nActiveRow && -1 == m_nActiveCol ) {
      if ( 0 == numRows() )
	addRow();
      m_nActiveRow = m_nActiveCol = 0;
    }
    // Move one row down
    else {
      while ( m_nActiveRow + 1 >= numRows() )
	addRow();
      m_nActiveCol = 0;
      m_nActiveRow++;
    }
    break;

  case Key_Enter:
  case Key_Return:
    if ( 0 == numRows() )
      addRow();
    editCell();
    break;

  default:
    // Let another widget get the keypress
    pcEvent->ignore();
    break;
  }

  // Try to make current cell fully visible

  
  // Update cell markings
  if ( m_nActiveRow != nOldRow || m_nActiveCol != nOldCol ) {
    updateCell(nOldRow, nOldCol);
    updateCell(m_nActiveRow, m_nActiveCol);
  }
}


/*!
  Handle mouse double click events.
  The event information is taken from \a pcEvent.

  If the left mousebutton is doubleclicked, the current cell will be put
  in edit mode if possible.
*/

void
KCellEdit::mouseDoubleClickEvent(QMouseEvent* pcEvent)
{
  assert(pcEvent);

  const int nOldRow = m_nActiveRow;
  const int nOldCol = m_nActiveCol;
  const int nNewRow = findRow(pcEvent->y());
  const int nNewCol = findCol(pcEvent->x());

  if ( LeftButton == pcEvent->button() &&
       nNewRow >= 0 && nNewRow < numRows() &&
       nNewCol >= 0 && nNewCol < numCols() ) {
    m_nActiveRow = nNewRow;
    m_nActiveCol = nNewCol;

    if ( nOldRow != nNewRow || nOldCol != nNewCol )
      updateCell(nOldRow, nOldCol);

    editCell();
  }
}


/*!
  Handle mouse press events.
  The event information is taken from \a pcEvent.

  If the left mousebutton is clicked, the cell under the cursor will be
  the new current cell.
*/

void
KCellEdit::mousePressEvent(QMouseEvent* pcEvent)
{
  assert(pcEvent);

  const int nOldRow = m_nActiveRow;
  const int nOldCol = m_nActiveCol;
  const int nNewRow = findRow(pcEvent->y());
  const int nNewCol = findCol(pcEvent->x());

  if ( m_isEditing )
    finishEdit(false);

  if ( LeftButton == pcEvent->button() &&
       nNewRow >= 0 && nNewRow < numRows() &&
       nNewCol >= 0 && nNewCol < numCols() ) {
    m_nActiveRow = nNewRow;
    m_nActiveCol = nNewCol;
  }

  if ( nOldRow != m_nActiveRow || nOldCol != m_nActiveCol ) {
    updateCell(nOldRow, nOldCol);
    updateCell(m_nActiveRow, m_nActiveCol);
  }
}


/*!
  Edit the current cell.
*/

void
KCellEdit::editCell()
{
  assert(false == m_isEditing);

  QLineEdit* pcEditor = m_papcEditors[m_nActiveCol];
  if ( pcEditor ) {
    int nX, nY;
    rowYPos(m_nActiveRow, &nY);
    colXPos(m_nActiveCol, &nX);
    pcEditor->installEventFilter(this);
    pcEditor->setGeometry(nX, nY, m_panColWidths[m_nActiveCol],
			  cellHeight());
    QString cText;
    if ( (unsigned)m_nActiveRow*numCols()+m_nActiveCol <
	 m_pcCellStrings->size() )
      cText = (*m_pcCellStrings)[m_nActiveRow*numCols()+m_nActiveCol];
    pcEditor->setText(cText);
    pcEditor->setFocus();
    pcEditor->show();
    m_isEditing = true;
  }
}


/*!
  Finish editing the cell.
  Keep the result if \a bKeepResult is `true'.
*/

void
KCellEdit::finishEdit(bool bKeepResult)
{
  QLineEdit* pcEditor = m_papcEditors[m_nActiveCol];
  assert(pcEditor);
  if ( bKeepResult ) {
    QString cText(pcEditor->text());
    setCellText(m_nActiveRow, m_nActiveCol, cText, false);
    emit textChanged(m_nActiveRow, m_nActiveCol, cText);
  }
  pcEditor->hide();
  m_isEditing = false;
  setFocus();
  updateCell(m_nActiveRow, m_nActiveCol);
}


/*!
  Paint the cell (\a nRow, \a nCol) using the open painter \a pcPainter.
*/

void
KCellEdit::paintCell(QPainter* pcPainter, int nRow, int nCol)
{
  assert(0 != pcPainter &&
	 nRow >= 0 && nRow < numRows() && nCol >= 0 && nCol < numCols());
  int nW = cellWidth(nCol);
  int nH = cellHeight(nRow);

  // Clear cell to white
  pcPainter->fillRect(0, 0, nW, nH, QColor(white));

  // Draw text
  QString cText;
  if ( (unsigned)nRow*numCols()+nCol < m_pcCellStrings->size() )
    cText = (*m_pcCellStrings)[nRow*numCols()+nCol];
  pcPainter->drawText(4, 4, nW - 8, nH - 7, 0, cText);

  // Draw a box around the active cell if widget has focus
  if ( hasFocus() && m_nActiveRow == nRow && m_nActiveCol == nCol ) {
    QPen cPen(black, 3);
    pcPainter->setPen(cPen);
    pcPainter->drawRect(1, 1, nW-2, nH-2);
  }
}


/*!
  Set the width of the column \a nColumn to \a nNewWidth.
  The old width was \a nOldWidth.
*/

void
KCellEdit::setColumnWidth(int nColumn, int nOldWidth, int nNewWidth)
{
  m_panColWidths[nColumn] = nNewWidth;
  updateTableSize();
  repaint();
  if ( m_isEditing ) {
    int nX, nY;
    QLineEdit* pcEditor = m_papcEditors[m_nActiveCol];
    assert(pcEditor);
    rowYPos(m_nActiveRow, &nY);
    colXPos(m_nActiveCol, &nX);
    pcEditor->setGeometry(nX, nY, m_panColWidths[m_nActiveCol],
			  pcEditor->height());
  }
}


/*!
  Get the width of the cell \a nColumn.
*/

int
KCellEdit::cellWidth(int nColumn)
{
  assert(nColumn >= 0 && nColumn < numCols());
  assert(m_panColWidths);
  return m_panColWidths[nColumn];
}


/*!
  Get the total width of the view.
*/

int
KCellEdit::totalWidth()
{
  assert(m_panColWidths);
  int nWidth = 0;
  const int nNumCols = numCols();
  for ( int iCol = 0; iCol < nNumCols; iCol++ )
    nWidth += m_panColWidths[iCol];
  return nWidth;
}


/*!
  Filter eventes. The receiver of the event \a pcEvent is \a pcReceiver.

  Theese keys are filtered out:
  \arg Return and enter - finish editing.
  \arg Tab - end editing, move to next cell if any.
  \arg Esc - cancel editing.
*/

bool
KCellEdit::eventFilter(QObject* pcReceiver, QEvent* pcEvent)
{
  bool isEventEaten = false;

  QLineEdit* pcEditor = m_papcEditors[m_nActiveCol];

#if QT_VERSION >= 200
  if ( pcReceiver == pcEditor && pcEvent->type() == QEvent::KeyPress ) {
#else  // Qt 1
  if ( pcReceiver == pcEditor && pcEvent->type() == Event_KeyPress ) {
#endif // QT_VERSION
    QKeyEvent* pcKeyEvent = (QKeyEvent*)pcEvent;
    switch ( pcKeyEvent->key() ) {
    case Key_Return:
    case Key_Enter:
      // End editing, save result
      finishEdit(true);
      isEventEaten = true;
      break;

    case Key_Tab:
      // End editing, save result, edit next if possible
      finishEdit(true);
      if ( m_nActiveCol + 1 < numCols() ) {
	updateCell(m_nActiveRow, m_nActiveCol);
	m_nActiveCol++;
	editCell();
      }
      else if ( m_nActiveRow + 1 < numRows() ) {
	updateCell(m_nActiveRow, m_nActiveCol);
	m_nActiveCol = 0;
	m_nActiveRow++;
	editCell();
      }
      isEventEaten = true;
      break;

    case Key_Escape:
      // Cancel editing
      finishEdit(false);
      isEventEaten = true;
      break;
    }
  }

  return isEventEaten;
}


/*!
  The view has been scrolled to \a nNewX in the horizontal direction.
  Update the GUI.
*/

void
KCellEdit::horizontalScroll(int nNewX)
{
  if ( m_isEditing ) {
    int nX, nY;
    rowYPos(m_nActiveRow, &nY);
    colXPos(m_nActiveCol, &nX);
    QLineEdit* pcEditor = m_papcEditors[m_nActiveCol];
    pcEditor->move(nX, nY);
  }
}


/*!
  The view has been scrolled to \a nNewY in the vertical direction.
  Update the GUI.
*/

void
KCellEdit::verticalScroll(int nNewY)
{
  if ( m_isEditing ) {
    int nX, nY;
    rowYPos(m_nActiveRow, &nY);
    colXPos(m_nActiveCol, &nX);
    QLineEdit* pcEditor = m_papcEditors[m_nActiveCol];
    pcEditor->move(nX, nY);
  }
}

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
