//*****************************************************************************
/*!
  \file kintegeredit.cpp
  \brief This file contains the implementation of the KIntegerEdit class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include "kintegeredit.h"
#include "kintegervalidator.h"

#include <assert.h>
#include <limits.h>

//*****************************************************************************
/*!
  Initialise this integer edit widget.

  This widget will accept all valid integers.

  The parent widget is \a pcParent, the name is \a pzName.

  \author André Johansen.
*/
//*****************************************************************************

KIntegerEdit::KIntegerEdit(QWidget* pcParent, const char* pzName)
  : QLineEdit(pcParent, pzName),
    m_pcValidator(0),
    m_nInteger(0)
{
  m_pcValidator = new KIntegerValidator(INT_MIN, 0, INT_MAX);
  setValidator(m_pcValidator);
}


//*****************************************************************************
/*!
  Initialise this integer edit widget.

  This widget will accept integers between \a nMin and \a nMax.
  If the widget is left with an invalid integer, \a nDefault will be used.

  The parent widget is \a pcParent, the name is \a pzName.

  \author André Johansen.
*/
//*****************************************************************************

KIntegerEdit::KIntegerEdit(int nMin, int nDefault, int nMax,
                           QWidget* pcParent, const char* pzName)
  : QLineEdit(pcParent, pzName),
    m_pcValidator(0),
    m_nInteger(0)
{
  m_pcValidator = new KIntegerValidator(nMin, nDefault, nMax);
  setValidator(m_pcValidator);
}


//*****************************************************************************
/*!
  Destroy the widget.

  \author André Johansen.
*/
//*****************************************************************************

KIntegerEdit::~KIntegerEdit()
{
  delete m_pcValidator;
  m_pcValidator = 0;
}


//*****************************************************************************
/*!
  Set the value to \a nValue.
  If the input is invalid according to the specifications for this widget,
  the default will be used.

  \author André Johansen.
*/
//*****************************************************************************

void
KIntegerEdit::setValue(int nValue)
{
  QString cText;
  cText.setNum(nValue);
  m_pcValidator->fixup(cText);
  setText(cText);
}


//*****************************************************************************
/*!
  Set the minimum acceptable integer to \a nMin.

  \sa setMaxValue(), KIntegerValidator::setMinValue().

  \author André Johansen.
*/
//*****************************************************************************

void
KIntegerEdit::setMinValue(int nMin)
{
  KIntegerValidator* pcValidator =
    dynamic_cast<KIntegerValidator*>(const_cast<QValidator*>(validator()));
  pcValidator->setMinValue(nMin);
}


//*****************************************************************************
/*!
  Set the maximum acceptable integer to \a nMax.

  \sa setMinValue(), KIntegerValidator::setMaxValue().

  \author André Johansen.
*/
//*****************************************************************************

void
KIntegerEdit::setMaxValue(int nMax)
{
  KIntegerValidator* pcValidator =
    dynamic_cast<KIntegerValidator*>(const_cast<QValidator*>(validator()));
  pcValidator->setMaxValue(nMax);
}


//*****************************************************************************
/*!
  Get the current integer.

  \author André Johansen.
*/
//*****************************************************************************

int
KIntegerEdit::getValue() const
{
  QString cText(text());
  int nNumber = cText.toInt();
  return nNumber;
}


//*****************************************************************************
/*!
  Ensure the value is valid when leaving the widget.
  If the number changed, emit the integerChanged() signal.

  \author André Johansen.
*/
//*****************************************************************************

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
// indent-tabs-mode: nil
// End:
