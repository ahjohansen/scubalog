// $Id$
//
// This code is copyrighted by (c) André Johansen 1999.
// This code is distributed under the GPL, version 2.
//

#include <assert.h>
#include "kintegervalidator.h"
#include "kintegervalidator.moc"


/*!
  Initialize the validator. The validator will accept integer with
  \a nMin as the minimum value and \a nMax as the maximum value.
  If the integer is invalid, \a nDefault will be used insted.

  \sa fixup().
*/

KIntegerValidator::KIntegerValidator(int nMin, int nDefault, int nMax,
				     QWidget* pcParent, const char* pzName)
  : QIntValidator(nMin, nMax, pcParent, pzName)
{
  assert((nMin <= nMax) && (nMin <= nDefault) && (nMax >= nDefault));

  m_nDefault = nDefault;
}


/*!
  Destroy the validator.
*/

KIntegerValidator::~KIntegerValidator()
{
}


/*!
  Fix the integer contained in \a cInput to be valid according to
  this validators requirements.

  If the input contains an invalid string (i.e. non-integer),
  the default value will be used; if it contains an integer below
  the minimum allowed, the minimum will be used; if it contains an
  integer above the maximum allowed, the maximum will be used;
  else it will be left unchanged.
*/

void
KIntegerValidator::fixup(QString& cInput)
{
#if QT_VERSION < 200
  cInput.detach();
#endif
  bool isOk;
  long int nCurrent = cInput.toLong(&isOk);
  if ( false == isOk )
    cInput.setNum(m_nDefault);
  else if ( nCurrent < bottom() )
    cInput.setNum(bottom());
  else if ( nCurrent > top() )
    cInput.setNum(top());
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
