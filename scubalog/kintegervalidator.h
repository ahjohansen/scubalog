//*****************************************************************************
/*!
  \file kintegervalidator.h
  \brief This file contains the definition of the KIntegerValidator class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef KINTEGERVALIDATOR_H
#define KINTEGERVALIDATOR_H

#include <qvalidator.h>

class QString;


//*****************************************************************************
/*!
  \class KIntegerValidator
  \brief The KIntegerValidator extends QIntValidator.

  This class has a default value that will be used whenever the widget
  is left with an invalid value.

  \author André Johansen.
*/
//*****************************************************************************

class KIntegerValidator : public QIntValidator {
  Q_OBJECT
public:
  KIntegerValidator(int nMin, int nDefault, int nMax,
                    QWidget* pcParent = 0, const char* pzName = 0);
  ~KIntegerValidator();

  void setMinValue(int nMin);
  void setMaxValue(int nMax);

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
// indent-tabs-mode: nil
// End:
