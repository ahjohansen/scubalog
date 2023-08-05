//*****************************************************************************
/*!
  \file dateitem.cpp
  \brief This file contains the implementation of the DateItem class.

  \par Copyright:
  André Hübert Johansen
*/
//*****************************************************************************

#include "dateitem.h"
#include "kdatevalidator.h"
#include <qdatetime.h>
#include <qstring.h>

//*****************************************************************************
/*!
  Set the data for this item for role \a role with value \a value.
  On edit role, copy the date into the item if valid.

  \sa QDate::fromString(), QDate::toString().
*/
//*****************************************************************************

void
DateItem::setData(int role, const QVariant& value)
{
  if ( role == Qt::EditRole ) {
    const QString cText = value.toString();
    const QDate cDate = KDateValidator::convertToDate(cText);
    if ( !cDate.isValid() )
      return;
    const QString cItemText = cDate.toString();
    setText(cItemText);
  }
  else {
    QTableWidgetItem::setData(role, value);
  }
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
