/*!
  \file widgets/kintegeredit.cpp
  \brief This file contains the implementation of the KIntegerEdit class.

  This file is part of Scuba Log, a dive logging application for KDE.
  Scuba Log is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/

#include <limits.h>
#include <assert.h>
#include "kintegervalidator.h"
#include "kintegeredit.h"
#include "kintegeredit.moc"


/*!
  Initialize this integer edit widget.

  This widget will accept all valid integers.

  The parent widget is \a pcParent, the name is \a pzName.
*/

KIntegerEdit::KIntegerEdit(QWidget* pcParent, const char* pzName)
  : QLineEdit(pcParent, pzName), m_pcValidator(0)
{
  m_pcValidator = new KIntegerValidator(INT_MIN, 0, INT_MAX);
  setValidator(m_pcValidator);
}


/*!
  Initialize this integer edit widget.

  This widget will accept integers between \a nMin and \a nMax.
  If the widget is left with an invalid integer, \a nDefault will be used.

  The parent widget is \a pcParent, the name is \a pzName.
*/

KIntegerEdit::KIntegerEdit(int nMin, int nDefault, int nMax,
			   QWidget* pcParent, const char* pzName)
  : QLineEdit(pcParent, pzName), m_pcValidator(0)
{
  m_pcValidator = new KIntegerValidator(nMin, nDefault, nMax);
  setValidator(m_pcValidator);
}
			   

/*!
  Destroy the widget.
*/

KIntegerEdit::~KIntegerEdit()
{
  delete m_pcValidator;
  m_pcValidator = 0;
}


/*!
  Set the number to \a nNumber.
  If the input is invalid according to the specifications for this widget,
  the default will be used.
*/

void
KIntegerEdit::setNumber(int nNumber)
{
  QString cText;
  cText.sprintf("%d", nNumber);
  m_pcValidator->fixup(cText);
  setText(cText);
}


/*!
  Ensure the value is valid when leaving the widget.
  If the number changed, emit the integerChanged() signal.
*/

void
KIntegerEdit::focusOutEvent(QFocusEvent* pcEvent)
{
  assert(pcEvent);
  QString cText(text());
  m_pcValidator->fixup(cText);
  if ( cText != text() ) {
    setText(cText);
  }
  int nNumber = cText.toInt();
  if ( nNumber != m_nInteger ) {
    m_nInteger = nNumber;
    emit integerChanged(nNumber);
  }
}



// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
