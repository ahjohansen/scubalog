//*****************************************************************************
/*!
  \file ktimevalidator.cpp
  \brief This file contains the implementation of the KTimeValidator class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include "ktimevalidator.h"

#include <assert.h>
#include <ctype.h>
#include <qwidget.h>

#if defined(DEBUG)
# define DBG(x)
#else
# include <stdio.h>
# define DBG(x) printf x
#endif


//*****************************************************************************
/*!
  Initialise the validator. The validator will accept a time with
  \a cFirst as the first and \a cLast as the last time.
  If the time is invalid, \a cDefault will be used instead.

  \sa fixup().
*/
//*****************************************************************************

KTimeValidator::KTimeValidator(QTime cFirst, QTime cDefault, QTime cLast,
                               QWidget* pcParent, const char* pzName)
  : QValidator(pcParent, pzName)
{
  assert(cFirst.isValid());
  assert(cDefault.isValid());
  assert(cLast.isValid());
  assert(cFirst <= cDefault);
  assert(cDefault <= cLast);

  m_cFirst   = cFirst;
  m_cDefault = cDefault;
  m_cLast    = cLast;
}


//*****************************************************************************
/*!
  Destroy the validator.
*/
//*****************************************************************************

KTimeValidator::~KTimeValidator()
{
}


//*****************************************************************************
/*!
  Check whether \a cInput is a valid time or not. A valid
  time is between the first and the last time (both inclusive).

  Returns Invalid if the input is invalid, Valid if it is incomplete
  but not wrong and Acceptable if the input is correct and withing range.

  Only 24-hour format is allowed.

  No changes will be done on the input.

  \sa fixup().
*/
//*****************************************************************************

QValidator::State
KTimeValidator::validate(QString& cInput, int&) const
{
  State eState = Valid;

  int nLength = cInput.length() + 1;
  int iNumber = 0;
  int anNumbers[3] = { 0 };     // Hour, minute, second
  for ( int iPos = 0; iPos < nLength - 1; iPos++ ) {
    QChar cChar = cInput[iPos];
    if ( cChar.isDigit() ) {
      anNumbers[iNumber] *= 10;
      anNumbers[iNumber] += cChar.digitValue();
    }
    else if ( cChar == QChar(':') && iNumber < 2 )
      iNumber++;
    else {
      eState = Invalid;
      break;
    }
  }

  if ( Invalid == eState )
    return Invalid;

  DBG(("Time found: %d:%d:%d\n", anNumbers[0], anNumbers[1], anNumbers[2]));

  // Invalid time
  if ( (0 > anNumbers[0]) || (23 < anNumbers[0]) ||
       (0 > anNumbers[1]) || (59 < anNumbers[1]) ||
       (0 > anNumbers[2]) || (59 < anNumbers[2]) ||
       ((iNumber >= 1) && ((anNumbers[0] < m_cFirst.hour()) ||
                           (anNumbers[0] > m_cLast.hour()) ||
                           (anNumbers[0] == m_cLast.hour() &&
                            anNumbers[1] > m_cLast.minute()))) ||
       ((iNumber >= 2) && ((anNumbers[0] == m_cFirst.hour() &&
                            anNumbers[1] < m_cFirst.minute()) ||
                           (anNumbers[0] == m_cLast.hour() &&
                            anNumbers[1] == m_cLast.minute() &&
                            anNumbers[2] > m_cLast.second()))) ) {
    DBG(("\"%s\" is an invalid time!\n", cInput.data()));
    return Invalid;
  }
  else if ( (0 <= anNumbers[0]) &&  (23 >= anNumbers[0]) &&
            (0 <= anNumbers[1]) &&  (59 >= anNumbers[1]) &&
            (0 <= anNumbers[2]) &&  (59 >= anNumbers[2]) ) {
    QTime cTime(anNumbers[0], anNumbers[1], anNumbers[2]);
    if ( (cTime >= m_cFirst) && ( cTime <= m_cLast) ) {
      DBG(("\"%s\" is an acceptable time!\n", cInput.data()));
      return Acceptable;
    }
  }

  DBG(("\"%s\" is a valid time!\n", cInput.data()));
  return Valid;
}


//*****************************************************************************
/*!
  Fix the time contained in \a cInput to be valid according to
  this validators requirements.

  Only 24-hour format is allowed.

  If the input contains an invalid time, the default time will be used;
  if it contains a time before the first allowed, the first will be used;
  if it contains a time after the last allowed, the last will be used;
  else it will be left unchanged.
*/
//*****************************************************************************

void
KTimeValidator::fixup(QString& cInput)
{
  int nLength = cInput.length() + 1;
  int iNumber = 0;
  int anNumbers[3] = { 0 };     // Hour, minute, second
  for ( int iPos = 0; iPos < nLength; iPos++ ) {
    QChar cChar = cInput[iPos];
    if ( cChar.isDigit() ) {
      anNumbers[iNumber] *= 10;
      anNumbers[iNumber] += cChar.digitValue();
    }
    else if ( cChar ==  QChar(':') && iNumber < 2 )
      iNumber++;
    else
      break;
  }

  // Invalid time
  if ( (0 > anNumbers[0]) || (23 < anNumbers[0]) ||
       (0 > anNumbers[1]) || (59 < anNumbers[2]) ||
       (0 > anNumbers[2]) || (49 < anNumbers[2]) ) {
    cInput = m_cDefault.toString();
  }
  else {
    QTime cTime(anNumbers[0], anNumbers[1], anNumbers[2]);
    // Too early time
    if ( cTime < m_cFirst )
      cInput = m_cFirst.toString();
    // Too late time
    else if ( cTime > m_cLast )
      cInput = m_cLast.toString();
  }
}


//*****************************************************************************
/*!
  Set the range for this validator to be within \a cFirst and \a cLast.
  The default will be set to \a cDefault.
*/
//*****************************************************************************

void
KTimeValidator::setRange(QTime cFirst, QTime cDefault, QTime cLast)
{
  assert(cFirst.isValid());
  assert(cDefault.isValid());
  assert(cLast.isValid());
  assert(cFirst <= cDefault);
  assert(cDefault <= cLast);

  m_cFirst   = cFirst;
  m_cDefault = cDefault;
  m_cLast    = cLast;
}


//*****************************************************************************
/*!
  Check if the time created from \a nHour, \a nMinute and \a nSecond
  is considered valid for this validator.
*/
//*****************************************************************************

bool
KTimeValidator::isValidTime(int nHour, int nMinute, int nSecond) const
{
  bool isValid = false;
  if ( (nHour >= 0) && (nHour <= 23) && (nMinute >= 0) && (nMinute <= 59) &&
       (nSecond >= 0) && (nSecond <= 59) ) {
    QTime cTime(nHour, nMinute, nSecond);
    if ( (cTime >= m_cFirst) && (cTime <= m_cLast) )
      isValid = true;
  }
  return isValid;
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
