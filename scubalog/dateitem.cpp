//*****************************************************************************
/*!
  \file dateitem.cpp
  \brief This file contains the implementation of the DateItem class.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include "dateitem.h"

#include <assert.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qlineedit.h>

//*****************************************************************************
/*!
  The editor \a i_pcEditor has edited the contents of this table item.
  Copy the date into the item, if valid.

  \sa QDate::fromString(), QDate::toString().

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

void
DateItem::setContentFromEditor(QWidget* i_pcEditor)
{
  assert(i_pcEditor);

  QLineEdit* pcEditor = dynamic_cast<QLineEdit*>(i_pcEditor);
  if ( pcEditor ) {
    const QString cText = pcEditor->text();
    const QDate cDate = QDate::fromString(cText, Qt::ISODate);
    if ( false == cDate.isValid() )
      return;
    const QString cItemText = cDate.toString(Qt::ISODate);
    setText(cItemText);
  }
  else {
    QTableItem::setContentFromEditor(i_pcEditor);
  }
}



// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
