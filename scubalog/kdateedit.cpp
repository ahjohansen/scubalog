//*****************************************************************************
/*!
  \file kdateedit.cpp
  \brief This file contains the implementation of the KDateEdit class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include "kdateedit.h"
#include "kdatevalidator.h"

#include <string.h>
#include <stdio.h>

//*****************************************************************************
/*!
  Create a widget with \a pcParent as parent widget and \a pzName as
  the widget name.
  The valid date for this widget is 1.1.1900 to today, with today
  as the default.
*/
//*****************************************************************************

KDateEdit::KDateEdit(QWidget* pcParent, const char* pzName)
  : QLineEdit(pcParent, pzName)
{
  m_cDate = QDate::currentDate();
  setText(m_cDate.toString());
  m_pcValidator = new KDateValidator(QDate(1900,1,1), m_cDate, m_cDate);
  setValidator(m_pcValidator);
}


//*****************************************************************************
/*!
  Create a widget with \a pcParent as parent widget and \a pzName as
  the widget name.
  The valid date for this widget is from \a cFirst to \a cLast (inclusive),
  with \a cDefault as the default.
*/
//*****************************************************************************

KDateEdit::KDateEdit(QDate cFirst, QDate cDefault, QDate cLast,
                     QWidget* pcParent, const char* pzName)
  : QLineEdit(pcParent, pzName)
{
  m_cDate = cDefault;
  setText(m_cDate.toString());
  m_pcValidator = new KDateValidator(cFirst, cDefault, cLast);
  setValidator(m_pcValidator);
}


//*****************************************************************************
/*!
  Destroy the widget.
*/
//*****************************************************************************

KDateEdit::~KDateEdit()
{
  delete m_pcValidator;
  m_pcValidator = 0;
}


//*****************************************************************************
/*!
  Get the current date.
*/
//*****************************************************************************

QDate
KDateEdit::date() const
{
  return m_cDate;
}


//*****************************************************************************
/*!
  Set the current date to \a cDate.
  No validation is done on the date given.

  If the date is different from the old one, dateChanged() is emitted.
*/
//*****************************************************************************

void
KDateEdit::setDate(QDate cDate)
{
  setText(cDate.toString());
  if ( cDate != m_cDate ) {
    m_cDate = cDate;
    dateChanged(cDate);
  }
}


//*****************************************************************************
/*!
  Extract a date from \a cText.

  The date must be in this format: "16.1.1999".

  If a date couldn't be created, the validator will be used to fix it.
*/
//*****************************************************************************

QDate
KDateEdit::convertToDate(const QString& cText)
{
  QDate cDate;
  int nDay   = 0;
  int nMonth = 0;
  int nYear  = 0;

  if ( (3 == sscanf(cText, "%d.%d.%d", &nDay, &nMonth, &nYear)) &&
       (m_pcValidator->isValidDate(nDay, nMonth, nYear)) )
    cDate = QDate(nYear, nMonth, nDay);
  else {
    cDate = m_pcValidator->defaultDate();
  }

  return cDate;
}


//*****************************************************************************
/*!
  Handle focus out event.

  The current text in the editor will be converted to a date, and if
  valid will be used as the new date. The widget will be updated with
  the date in "Sat Jan 16 1999" format.
*/
//*****************************************************************************

void
KDateEdit::focusOutEvent(QFocusEvent*)
{
  QDate cNewDate = convertToDate(text());
  if ( cNewDate.isValid() && (cNewDate != m_cDate) ) {
    m_cDate = cNewDate;
    emit dateChanged(cNewDate);
  }
  setText(m_cDate.toString());
}


//*****************************************************************************
/*!
  Handle focus in event.

  The current date will be converted to a format on the form "16.1.1999",
  and the widget will be updated.
*/
//*****************************************************************************

void
KDateEdit::focusInEvent(QFocusEvent*)
{
  QString cNewText;
  cNewText.sprintf("%d.%d.%d", m_cDate.day(), m_cDate.month(), m_cDate.year());
  setText(cNewText);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
