// $Id$
//
// This code is copyrighted by (c) André Johansen 1999.
// The class is published under the GPL, version 2.
//


#ifndef KINTEGERVALIDATOR_H
#define KINTEGERVALIDATOR_H

#include <qvalidator.h>

class QString;


/*!
  \class KIntegerValidator
  \brief The KIntegerValidator extends QIntValidator.

  This class has a default value that will be used whenever the widget
  is left with an invalid value.
*/

class KIntegerValidator : public QIntValidator {
  Q_OBJECT
public:
  KIntegerValidator(int nMin, int nDefault, int nMax,
		    QWidget* pcParent = 0, const char* pzName = 0);
  ~KIntegerValidator();

  void fixup(QString& cInput);

  //! Get the default value for this validator.
  int defaultValue() const { return m_nDefault; }

protected:
  //! The value that will be used when the integer is left invalid.
  int m_nDefault;
};


#endif // KINTEGERVALIDATOR_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
