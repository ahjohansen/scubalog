// $Id$
//
// This code is copyrighted by (c) André Johansen 1999.
// This code is distributed under the GPL, version 2.
//

#ifndef KINTEGEREDIT_H
#define KINTEGEREDIT_H

#include <qlineedit.h>

class KIntegerValidator;


/*!
  \class KIntegerEdit
  \brief The KIntegerEdit class is used to edit integers.
*/

class KIntegerEdit : public QLineEdit {
  Q_OBJECT
public:
  KIntegerEdit(QWidget* pcParent = 0, const char* pzName = 0);
  KIntegerEdit(int nMin, int nDefault, int nMax,
	       QWidget* pcParent = 0, const char* pzName = 0);
  ~KIntegerEdit();

public slots:
  void setNumber(int nNumber);

protected:
  void focusOutEvent(QFocusEvent* pcEvent);

  //! The validator used for this widget.
  KIntegerValidator* m_pcValidator;

private:
  //! Disabled copy constructor.
  KIntegerEdit(const KIntegerEdit&);
  //! Disabled assignment operator.
  KIntegerEdit& operator =(KIntegerEdit&);

  //! The current integer.
  int m_nInteger;

signals:
  //! Emitted whenever the integer changes.
  void integerChanged(int nNew);
};


#endif // KINTEGEREDIT_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
