//*****************************************************************************
/*!
  \file integerdialog.h
  \brief This file contains the definition of the IntegerDialog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef INTEGERDIALOG_H
#define INTEGERDIALOG_H

#include <qdialog.h>


class QString;
class QLabel;
class QPushButton;
class KIntegerEdit;


//*****************************************************************************
/*!
  \class IntegerDialog
  \brief The IntegerDialog class is used to prompt the user for an integer.

  This dialog is used to ask a user about an integer, which can be forced
  to stay within a user specified range by calling setMinValue() and
  setMaxValue().

  The default integer to be shown can be set with setValue().

  A text to be shown above the editor can be set with setLabel().

  The function getValue() can be used at any time to query the currently
  entered integer.

  \sa KIntegerEdit.

  \author André Johansen
*/
//*****************************************************************************

class IntegerDialog : public QDialog {
  Q_OBJECT
public:
  IntegerDialog(QWidget* pcParent = 0,
                bool isModal = false, Qt::WFlags nFlags = 0);
  virtual ~IntegerDialog();

  void setValue(int nValue);
  void setMinValue(int nMinValue);
  void setMaxValue(int nMaxValue);

  void setText(const QString& cText);

  int getValue() const;

protected:
  void updateGeometry();

  //! The label.
  QLabel*       m_pcText;
  //! The integer editor line.
  KIntegerEdit* m_pcEditor;
  //! The ok button.
  QPushButton*  m_pcOk;
  //! The cancel button.
  QPushButton*  m_pcCancel;
};


#endif // INTEGERDIALOG_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
