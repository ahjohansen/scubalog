//*****************************************************************************
/*!
  \file ktimeedit.cpp
  \brief This file contains the implementation of the KTimeEdit class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <string.h>
#include <stdio.h>
#include "ktimevalidator.h"
#include "ktimeedit.h"



//*****************************************************************************
/*!
  Create a widget with \a pcParent as parent widget and \a pzName as
  the widget name.
  The valid time for this widget is 0:00:00 to 23:59:59, with the current
  time as the default.
*/
//*****************************************************************************

KTimeEdit::KTimeEdit(QWidget* pcParent, const char* pzName)
  : QLineEdit(pcParent, pzName)
{
  m_cTime = QTime::currentTime();
  setText(m_cTime.toString());
  m_pcValidator = new KTimeValidator(QTime(), m_cTime, QTime(23,59,59));
  setValidator(m_pcValidator);
}


//*****************************************************************************
/*!
  Create a widget with \a pcParent as parent widget and \a pzName as
  the widget name.
  The valid time for this widget is from \a cFirst to \a cLast (inclusive),
  with \a cDefault as the default.
*/
//*****************************************************************************

KTimeEdit::KTimeEdit(QTime cFirst, QTime cDefault, QTime cLast,
                     QWidget* pcParent, const char* pzName)
  : QLineEdit(pcParent, pzName)
{
  m_cTime = cDefault;
  setText(m_cTime.toString());
  m_pcValidator = new KTimeValidator(cFirst, cDefault, cLast);
  setValidator(m_pcValidator);
}


//*****************************************************************************
/*!
  Destroy the widget.
*/
//*****************************************************************************

KTimeEdit::~KTimeEdit()
{
  delete m_pcValidator;
  m_pcValidator = 0;
}


//*****************************************************************************
/*!
  Get the current time from the widget (i.e. not the clock time).
*/
//*****************************************************************************

QTime
KTimeEdit::time() const
{
  return m_cTime;
}


//*****************************************************************************
/*!
  Set the current time to \a cTime.
  No validation is done on the time given.

  If the time is different from the old one, timeChanged() is emitted.
*/
//*****************************************************************************

void
KTimeEdit::setTime(QTime cTime)
{
  setText(cTime.toString());
  if ( cTime != m_cTime ) {
    m_cTime = cTime;
    timeChanged(cTime);
  }
}


//*****************************************************************************
/*!
  Extract a time from \a cText.

  The time must be in 24-hour format.

  If a time couldn't be created, the validator will be used to fix it.
*/
//*****************************************************************************

QTime
KTimeEdit::convertToTime(const QString& cText)
{
  QTime cTime;
  int nHour   = 0;
  int nMinute = 0;
  int nSecond = 0;

  if ( (3 == sscanf(cText, "%d:%d:%d", &nHour, &nMinute, &nSecond)) &&
       (m_pcValidator->isValidTime(nHour, nMinute, nSecond)) )
    cTime = QTime(nHour, nMinute, nSecond);
  else {
    cTime = m_pcValidator->defaultTime();
  }

  return cTime;
}


//*****************************************************************************
/*!
  Handle focus out event.

  The current text in the editor will be converted to a time, and if
  valid will be used as the new time. The widget will be updated.
*/
//*****************************************************************************

void
KTimeEdit::focusOutEvent(QFocusEvent*)
{
  QTime cNewTime = convertToTime(text());
  if ( cNewTime.isValid() && (cNewTime != m_cTime) &&
       m_pcValidator->isValidTime(cNewTime.hour(), cNewTime.minute(),
                                  cNewTime.second()) ) {
    m_cTime = cNewTime;
    emit timeChanged(cNewTime);
  }
  setText(m_cTime.toString());
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
