//*****************************************************************************
/*!
  \file kcelleditview.cpp
  \brief This file contains the implementation of KCellEditView.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <stdio.h>
#include <assert.h>
#include <qheader.h>
#include <qpen.h>
#include <qpainter.h>
#include <qkeycode.h>
#include <qfontinfo.h>
#include <qlineedit.h>
#include <qtableview.h>
#include <qscrollbar.h>
#include "kcelledit.h"
#include "kcelleditview.h"



//*****************************************************************************
/*!
  \class _KCellEdit
  \brief Internal use only.

  \author André Johansen.
*/
//*****************************************************************************

class _KCellEdit : public KCellEdit {
  friend class KCellEditView;
public:
  _KCellEdit(int nNumCols, QWidget* pcParent, const char* pzName)
    : KCellEdit(nNumCols, pcParent, pzName) {
  }
  using QTableView::horizontalScrollBar;
  using QTableView::verticalScrollBar;
private:
  _KCellEdit(const _KCellEdit&);
  _KCellEdit& operator =(const _KCellEdit&);
};


//*****************************************************************************
/*!
  Create a view with \a nNumCols columns, \a pcParent as parent widget
  and \a pzName as widget name.

  Initially, the view will have no rows.
*/
//*****************************************************************************

KCellEditView::KCellEditView(int nNumCols, QWidget* pcParent,
                             const char* pzName)
  : QFrame(pcParent, pzName), m_pcHeader(0), m_pcCellEdit(0),
    m_nNumCols(nNumCols)
{
  assert(nNumCols >= 1);

  setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

  QSize cSize(sizeHint());

  m_pcHeader = new QHeader(nNumCols, this, "cellEditHeader");
  m_pcHeader->setGeometry(2, 2, cSize.width() - 4,
                          m_pcHeader->sizeHint().height());
  m_pcHeader->setMovingEnabled(false);

  _KCellEdit* pcCellEdit = new _KCellEdit(nNumCols, this, "cellEdit");
  m_pcCellEdit = pcCellEdit;
  m_pcCellEdit->setGeometry(2, m_pcHeader->height() + 2,
                            sizeHint().width() - 4,
                            sizeHint().height() - 
                            m_pcHeader->sizeHint().height() - 4);
  m_pcCellEdit->connect(m_pcHeader, SIGNAL(sizeChange(int, int, int)),
                        SLOT(setColumnWidth(int, int, int)));

  const QScrollBar* pcScrollBar = pcCellEdit->horizontalScrollBar();
  m_pcHeader->connect(pcScrollBar, SIGNAL(valueChanged(int)),
                      SLOT(setOffset(int)));
  m_pcHeader->connect(pcScrollBar, SIGNAL(sliderMoved(int)),
                      SLOT(setOffset(int)));

  for ( int iCol = 0; iCol < nNumCols; iCol++ )
    m_pcCellEdit->setColumnWidth(iCol, 0, m_pcHeader->cellSize(iCol));

  connect(pcCellEdit, SIGNAL(textChanged(int, int, const QString&)),
          SIGNAL(textChanged(int, int, const QString&)));
}


//*****************************************************************************
/*!
  Destroy the view.
*/
//*****************************************************************************

KCellEditView::~KCellEditView()
{
}


//*****************************************************************************
/*!
  Clear the view. All texts will be cleared, and the number of rows set to 1.

  \sa KCellEdit::clear().
*/
//*****************************************************************************

void
KCellEditView::clear()
{
  m_pcCellEdit->clear();
}


//*****************************************************************************
/*!
  Set the name of column \a nCol to \a cName.

  \sa QHeader::setLabel().
*/
//*****************************************************************************

void
KCellEditView::setColumnName(int nCol, const QString& cName)
{
  assert(nCol >= 0 && nCol < m_nNumCols);
  m_pcHeader->setLabel(nCol, cName);
}


//*****************************************************************************
/*!
  Set the editor to be used in column \a nCol to \a pcEditor.

  \sa KCellEdit::setColEditor().
*/
//*****************************************************************************

void
KCellEditView::setColEditor(int nCol, QLineEdit* pcEditor)
{
  m_pcCellEdit->setColEditor(nCol, pcEditor);
}


//*****************************************************************************
/*!
  Set the text in the cell (\a nRow, \a nCol) to \a cText.
  If \a bUpdateCell is `true', the cell will be repainted.

  \sa KCellEdit::setCellText().
*/
//*****************************************************************************

void
KCellEditView::setCellText(int nRow, int nCol, const QString& cText,
                           bool bUpdateCell)
{
  m_pcCellEdit->setCellText(nRow, nCol, cText, bUpdateCell);
}


//*****************************************************************************
/*!
  Handle widget resize. The event information is in \a pcEvent.

  This function makes sure the header occupies the entire width of
  the view by resizing the last column.

  \sa KCellEdit::setColumnWidth().
*/
//*****************************************************************************

void
KCellEditView::resizeEvent(QResizeEvent* pcEvent)
{
  assert(pcEvent);
  assert(m_pcHeader);
  assert(m_pcCellEdit);

  int nLastColWidth = width() - 4;
  for ( int iCol = 0; iCol < m_nNumCols - 1; iCol++ )
    nLastColWidth -= m_pcHeader->cellSize(iCol);
  nLastColWidth = QMAX(50, nLastColWidth);
  m_pcHeader->setCellSize(m_nNumCols - 1, nLastColWidth);
  m_pcCellEdit->setColumnWidth(m_nNumCols - 1, 0, nLastColWidth);
  m_pcHeader->resize(width() - 4, m_pcHeader->sizeHint().height());
  m_pcCellEdit->resize(width() - 4, height()-m_pcHeader->height() - 4);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
