//*****************************************************************************
/*!
  \file ktimevalidator.h
  \brief This file contains the definition of the KTimeValidator class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef KTIMEVALIDATOR_H
#define KTIMEVALIDATOR_H

#include <qdatetime.h>
#include <qvalidator.h>

class QString;

//*****************************************************************************
/*!
  \class KTimeValidator
  \brief The KTimeValidator is used to validate time input.

  This validator is used to validate a time as the user types it.
  Only 24-hour format is accepted.

  If the widget is left with an invalid time, a default time will
  be used.
*/
//*****************************************************************************

class KTimeValidator : public QValidator {
  Q_OBJECT
public:
  KTimeValidator(QTime cMin, QTime cDefault, QTime cMax,
                 QWidget* pcParent = 0, const char* pzName = 0);
  virtual ~KTimeValidator();

  virtual State validate(QString& cInput, int& nCursorPos) const;
  virtual void fixup(QString& cInput);

  virtual void setRange(QTime cFirst, QTime cDefault, QTime cLast);
  //! Get the first allowed time for this validator.
  QTime bottom() const { return m_cFirst; }
  //! Get the last allowed time for this validator.
  QTime top() const { return m_cLast; }
  //! Get the default time for this validator.
  QTime defaultTime() const { return m_cDefault; }

  bool isValidTime(int nDay, int nMonth, int nYear) const;

protected:
  //! The first time allowed.
  QTime m_cFirst;
  //! The last time allowed.
  QTime m_cLast;
  //! The default time.
  QTime m_cDefault;
};


#endif // KTIMEVALIDATOR_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
