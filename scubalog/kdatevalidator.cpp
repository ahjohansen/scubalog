//*****************************************************************************
/*!
  \file kdatevalidator.cpp
  \brief This file contains the implementation of the KDateValidator class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "kdatevalidator.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <qwidget.h>

#if defined(DEBUG)
# define DBG(x)
#else
# include <stdio.h>
# define DBG(x) printf x
#endif


//*****************************************************************************
/*!
  Initialise the validator. The validator will accept a date with
  \a cFirst as the first and \a cLast as the last date.
  If the date is invalid, \a cDefault will be used instead.

  \sa fixup().
*/
//*****************************************************************************

KDateValidator::KDateValidator(QDate cFirst, QDate cDefault, QDate cLast,
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

KDateValidator::~KDateValidator()
{
}


//*****************************************************************************
/*!
  Check whether \a cInput is a valid date or not. A valid
  date is between the first and the last date (both inclusive).

  Returns Invalid if the input is invalid, Valid if it is incomplete
  but not wrong and Acceptable if the input is correct and withing range.

  The only allowed format is "16.1.1999".

  No changes will be done on the input.

  \sa fixup().
*/
//*****************************************************************************

QValidator::State
KDateValidator::validate(QString& cInput, int&) const
{
  State eState = Valid;

  int nLength = cInput.length() + 1;
  int iNumber = 0;
  int anNumbers[3] = { 0 };     // Day, month, year
  for ( int iPos = 0; iPos < nLength - 1; iPos++ ) {
    QChar cChar = cInput[iPos];
    if ( cChar.isDigit() ) {
      anNumbers[iNumber] *= 10;
      anNumbers[iNumber] += cChar.digitValue();
    }
    else if ( cChar.isPunct() && iNumber < 2 )
      iNumber++;
    else {
      eState = Invalid;
      break;
    }
  }

  if ( Invalid == eState )
    return Invalid;

  DBG(("Date found: %d.%d.%d\n", anNumbers[0], anNumbers[1], anNumbers[2]));

  // Invalid date
  if ( (31 < anNumbers[0]) ||
       ((iNumber >= 1) && ((1 > anNumbers[0]) || (12 < anNumbers[1])) ||
        ((anNumbers[1] >= 1) && (daysInMonth(anNumbers[1]) < anNumbers[0]))) ||
       ((iNumber == 2) && ((1 > anNumbers[1]) ||
                           (daysInMonth(anNumbers[1]) < anNumbers[0]) ||
                           (m_cLast.year() < anNumbers[2]) ||
                           ((m_cFirst.year()-anNumbers[2] < 10) &&
                            anNumbers[2] < m_cFirst.year()) ||
                           (anNumbers[2] >= m_cFirst.year() &&
                            anNumbers[2] <= m_cLast.year() &&
                            false == isValidDate(anNumbers[0], anNumbers[1],
                                                 anNumbers[2])))) ) {
    DBG(("\"%s\" is an invalid date!\n", cInput.data()));
    return Invalid;
  }
  else if ( (1 <= anNumbers[0]) &&  (31 >= anNumbers[0]) &&
            (1 <= anNumbers[1]) &&  (12 >= anNumbers[1]) &&
            (1752 <= anNumbers[2]) &&  (8000 >= anNumbers[2]) &&
            (daysInMonth(anNumbers[1], anNumbers[2]) >= anNumbers[0]) ) {
    QDate cDate(anNumbers[2], anNumbers[1], anNumbers[0]);
    if ( (cDate >= m_cFirst) && ( cDate <= m_cLast) ) {
      DBG(("\"%s\" is an acceptable date!\n", cInput.data()));
      return Acceptable;
    }
  }

  DBG(("\"%s\" is a valid date!\n", cInput.data()));
  return Valid;
}


//*****************************************************************************
/*!
  Fix the date contained in \a cInput to be valid according to
  this validators requirements.

  The only allowed format is "16.1.1999".

  If the input contains an invalid date, the default date will be used;
  if it contains a date before the first allowed, the first will be used;
  if it contains a date after the last allowed, the last will be used;
  else it will be left unchanged.
*/
//*****************************************************************************

void
KDateValidator::fixup(QString& cInput)
{
  int nLength = cInput.length() + 1;
  int iNumber = 0;
  int anNumbers[3] = { 0 };     // Day, month, year
  for ( int iPos = 0; iPos < nLength; iPos++ ) {
    QChar cChar = cInput[iPos];
    if ( cChar.isDigit() ) {
      anNumbers[iNumber] *= 10;
      anNumbers[iNumber] += cChar.digitValue();
    }
    else if ( cChar.isPunct() && iNumber < 2 )
      iNumber++;
    else
      break;
  }

  // Invalid date
  if ( (   1 > anNumbers[0]) || (  31 < anNumbers[0]) ||
       (   1 > anNumbers[1]) || (  12 < anNumbers[2]) ||
       (1752 > anNumbers[2]) || (8000 < anNumbers[2]) ||
       (daysInMonth(anNumbers[1], anNumbers[2]) > anNumbers[0]) ) {
    cInput = m_cDefault.toString();
  }
  else {
    QDate cDate(anNumbers[2], anNumbers[1], anNumbers[0]);
    // Too early date
    if ( cDate < m_cFirst )
      cInput = m_cFirst.toString();
    // Too late date
    else if ( cDate > m_cLast )
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
KDateValidator::setRange(QDate cFirst, QDate cDefault, QDate cLast)
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
  Check if the date created from \a nDay, \a nMonth and \a nYear
  is considered valid for this validator.
*/
//*****************************************************************************

bool
KDateValidator::isValidDate(int nDay, int nMonth, int nYear) const
{
  bool isValid = false;
  if ( (nDay >= 1) && (nDay <= 31) && (nMonth >= 1) && (nMonth <= 12) &&
       (nYear >= m_cFirst.year()) && (nYear <= m_cLast.year()) &&
       (daysInMonth(nMonth, nYear) >= nDay) ) {
    QDate cDate(nYear, nMonth, nDay);
    if ( (cDate >= m_cFirst) && (cDate <= m_cLast) )
      isValid = true;
  }
  return isValid;
}


//*****************************************************************************
/*!
  Get the number of days in month number \a nMonth.
  This function does no checking on leapyears, the number of months in
  february is returned as 29.
*/
//*****************************************************************************

int
KDateValidator::daysInMonth(int nMonth) const
{
  const int anMonthDays[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  assert((nMonth >= 1) && (nMonth <= 12));
  return anMonthDays[nMonth-1];
}


//*****************************************************************************
/*!
  Get the number of days in month number \a nMonth in the year \a nYear.
  This function takes leapyears into account.

  The month must be between 1 and 12, the year must be between 1752 and 8000.
*/
//*****************************************************************************

int
KDateValidator::daysInMonth(int nMonth, int nYear)
{
  const int anMonthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  assert((nMonth >= 1) && (nMonth <= 12));
  assert((nYear >= 1752) && (nYear < 8000));
  if ( (2 == nMonth) &&
       (nYear % 4 == 0 && nYear % 100 != 0 || nYear % 400 == 0) )
    return 29;
  else
    return anMonthDays[nMonth-1];
}


//*****************************************************************************
/*!
  Extract a date from \a cDateText.

  The date must be in this format: "16.1.1999".

  If a date couldn't be created, a null-date will be returned.

  \sa QDate::isNull().
*/
//*****************************************************************************

QDate
KDateValidator::convertToDate(const QString& cDateText)
{
  QDate cDate;
  int nDay   = 0;
  int nMonth = 0;
  int nYear  = 0;

  if ( (3 == sscanf(cDateText, "%d.%d.%d", &nDay, &nMonth, &nYear)) &&
       (QDate::isValid(nYear, nMonth, nDay)) )
    cDate = QDate(nYear, nMonth, nDay);

  return cDate;
}



// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
