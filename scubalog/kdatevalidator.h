// $Id$
//
// This code is copyrighted by (c) André Johansen 1999.
// The class is published under the GPL, version 2.
//

#ifndef KDATEVALIDATOR_H
#define KDATEVALIDATOR_H

#include <qdatetime.h>
#include <qvalidator.h>

class QString;


/*!
  \class KDateValidator
  \brief The KDateValidator is used to validate date input.

  This validator is used to validate a date as the user types it.
  Only this form is accepted: "16.1.1999".

  If the widget is left with an invalid date, a default date will
  be used.
*/

class KDateValidator : public QValidator {
  Q_OBJECT
public:
  KDateValidator(QDate cMin, QDate cDefault, QDate cMax,
		 QWidget* pcParent = 0, const char* pzName = 0);
  virtual ~KDateValidator();

#if QT_VERSION >= 200
  virtual State validate(QString& cInput, int& nCursorPos) const;
#else  // Qt 1
  virtual State validate(QString& cInput, int& nCursorPos);
#endif // QT_VERSION
  virtual void fixup(QString& cInput);

  virtual void setRange(QDate cFirst, QDate cDefault, QDate cLast);
  //! Get the first allowed date for this validator.
  QDate bottom() const { return m_cFirst; }
  //! Get the last allowed date for this validator.
  QDate top() const { return m_cLast; }
  //! Get the default date for this validator.
  QDate defaultDate() const { return m_cDefault; }

  bool isValidDate(int nDay, int nMonth, int nYear) const;
  static int daysInMonth(int nMonth, int nYear);
  static QDate convertToDate(const QString& cDateText);


protected:
  int daysInMonth(int nMonth) const;
  State validateDay(int nDay) const;
  State validateMonth(int nMonth) const;
  State validateYear(int nYear) const;

  //! The first date allowed.
  QDate m_cFirst;
  //! The last date allowed.
  QDate m_cLast;
  //! The default date.
  QDate m_cDefault;
};


#endif // KDATEVALIDATOR_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
