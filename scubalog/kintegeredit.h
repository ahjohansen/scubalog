//*****************************************************************************
/*!
  \file ScubaLog/widgets/kintegeredit.h
  \brief This file contains the definition of the KIntegerEdit class.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef KINTEGEREDIT_H
#define KINTEGEREDIT_H

#include <qlineedit.h>

class KIntegerValidator;


//*****************************************************************************
/*!
  \class KIntegerEdit
  \brief The KIntegerEdit class is used to edit integers.

  \author André Johansen.
*/
//*****************************************************************************

class KIntegerEdit : public QLineEdit {
  Q_OBJECT
public:
  KIntegerEdit(QWidget* pcParent = 0, const char* pzName = 0);
  KIntegerEdit(int nMin, int nDefault, int nMax,
               QWidget* pcParent = 0, const char* pzName = 0);
  ~KIntegerEdit();

  void setMinValue(int nMin);
  void setMaxValue(int nMax);

  int getValue() const;

public slots:
  void setValue(int nValue);

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
// indent-tabs-mode: nil
// End:
