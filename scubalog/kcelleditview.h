//*****************************************************************************
/*!
  \file ScubaLog/widgets/kcelleditview.h
  \brief This file contains the definition of KCellEditView.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef KCELLEDITVIEW_H
#define KCELLEDITVIEW_H

#include <qstring.h>
#include <qframe.h>

class QLineEdit;
class QHeader;
class KCellEdit;


//*****************************************************************************
/*!
  \class KCellEditView
  \brief The KCellEditView class is used to edit cells.

  \author André Johansen.
*/
//*****************************************************************************

class KCellEditView : public QFrame {
  Q_OBJECT
public:
  KCellEditView(int nNumCols, QWidget* pcParent = 0, const char* pzName = 0);
  virtual ~KCellEditView();

  void setColumnName(int nCol, const QString& cName);
  void setColEditor(int nCol, QLineEdit* pcEditor);
  void setCellText(int nRow, int nCol, const QString& cText, bool bUpdateCell);
  virtual void clear();

protected:
  virtual void resizeEvent(QResizeEvent* pcEvent);

private:
  //! The header widget.
  QHeader*   m_pcHeader;
  //! The tablew view.
  KCellEdit* m_pcCellEdit;
  //! The number of columns.
  int        m_nNumCols;

signals:
  /*!
    This signal is emitted when the cell (\a nRow, \a nCol) has changed
    text to \a cText.
  */
  void textChanged(int nRow, int nCol, const QString& cText);
};

#endif // KCELLEDITVIEW_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
