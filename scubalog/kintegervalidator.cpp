//*****************************************************************************
/*!
  \file kintegervalidator.cpp
  \brief This file contains the implementation of the KIntegerValidator class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include "kintegervalidator.h"

#include <assert.h>
#include <qwidget.h>

//*****************************************************************************
/*!
  Initialise the validator. The validator will accept integer with
  \a nMin as the minimum value and \a nMax as the maximum value.
  If the integer is invalid, \a nDefault will be used insted.

  \sa fixup().

  \author André Johansen.
*/
//*****************************************************************************

KIntegerValidator::KIntegerValidator(int nMin, int nDefault, int nMax,
                                     QWidget* pcParent, const char* pzName)
  : QIntValidator(nMin, nMax, pcParent, pzName)
{
  assert((nMin <= nMax) && (nMin <= nDefault) && (nMax >= nDefault));

  m_nDefault = nDefault;
}


//*****************************************************************************
/*!
  Destroy the validator.
*/
//*****************************************************************************

KIntegerValidator::~KIntegerValidator()
{
}


//*****************************************************************************
/*!
  Fix the integer contained in \a cInput to be valid according to
  this validators requirements.

  If the input contains an invalid string (i.e. non-integer),
  the default value will be used; if it contains an integer below
  the minimum allowed, the minimum will be used; if it contains an
  integer above the maximum allowed, the maximum will be used;
  else it will be left unchanged.
*/
//*****************************************************************************

void
KIntegerValidator::fixup(QString& cInput)
{
  bool isOk;
  long int nCurrent = cInput.toLong(&isOk);
  if ( false == isOk )
    cInput.setNum(m_nDefault);
  else if ( nCurrent < bottom() )
    cInput.setNum(bottom());
  else if ( nCurrent > top() )
    cInput.setNum(top());
}


//*****************************************************************************
/*!
  Set the minimum acceptable value to \a nMin.
  If the maximum value is currently smaller than the minimum value, it
  will be set to the minimum value as well.

  \sa setMaxValue(), setRange().

  \author André Johansen.
*/
//*****************************************************************************

void
KIntegerValidator::setMinValue(int nMin)
{
  int nMax = top();
  if ( nMax < nMin )
    nMax = nMin;
  setRange(nMin, nMax);
}


//*****************************************************************************
/*!
  Set the maximum acceptable value to \a nMax.
  If the minimum value is currently larger than the maximum value, it
  will be set to the maximum value as well.

  \sa setMinValue(), setRange().

  \author André Johansen.
*/
//*****************************************************************************

void
KIntegerValidator::setMaxValue(int nMax)
{
  int nMin = bottom();
  if ( nMax < nMin )
    nMin = nMax;
  setRange(nMin, nMax);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
