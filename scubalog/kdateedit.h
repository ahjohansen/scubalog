/*!
  \file ScubaLog/widgets/kdateedit.h
  \brief This file contains the definition of KDateEdit.

  $Id$

  \par Copyright: André Johansen.
*/


#ifndef KDATEEDIT_H
#define KDATEEDIT_H

#include <qlineedit.h>
#include <qdatetime.h>

class KDateValidator;


/*!
  \class KDateEdit
  \brief The KDateEdit class is a date input widget.

  This class provides a simple widget to write a date. The date will
  be shown as "Sat Jan 16 1999". When typing a date, a dotted numerical
  version is used: "16.1.1999".

  \author André Johansen <andrej@ifi.uio.no>.
*/

class KDateEdit : public QLineEdit {
  Q_OBJECT
public:
  KDateEdit(QWidget* pcParent = 0, const char* pzName = 0);
  KDateEdit(QDate cFirst, QDate cDefault, QDate cLast,
	    QWidget* pcParent = 0, const char* pzName = 0);
  virtual ~KDateEdit();

  QDate date() const;

public slots:
  void setDate(QDate cDate);

protected:
  virtual QDate convertToDate(const QString& cText);
  virtual void focusOutEvent(QFocusEvent*);
  virtual void focusInEvent(QFocusEvent*);

private:
  //! The current selected date.
  QDate m_cDate;
  //! The validator for the input.
  KDateValidator* m_pcValidator;

signals:
  void dateChanged(QDate cNewDate);
};


#endif // KDATEEDIT_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
