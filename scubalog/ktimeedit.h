//*****************************************************************************
/*!
  \file ktimeedit.h
  \brief This file contains the definition of the KTimeEdit class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef KTIMEEDIT_H
#define KTIMEEDIT_H

#include <qlineedit.h>
#include <qdatetime.h>

class KTimeValidator;


//*****************************************************************************
/*!
  \class KTimeEdit
  \brief The KTimeEdit class is a time input widget.

  This class provides a simple widget to write a time. The time will
  be shown in 24 hour-mode (23:04:58).

  \author André Johansen
*/
//*****************************************************************************

class KTimeEdit : public QLineEdit {
  Q_OBJECT
public:
  KTimeEdit(QWidget* pcParent = 0, const char* pzName = 0);
  KTimeEdit(QTime cFirst, QTime cDefault, QTime cLast,
            QWidget* pcParent = 0, const char* pzName = 0);
  virtual ~KTimeEdit();

  QTime time() const;

public slots:
  void setTime(QTime cTime);

protected:
  virtual QTime convertToTime(const QString& cText);
  virtual void focusOutEvent(QFocusEvent*);

private:
  //! The current selected time.
  QTime m_cTime;
  //! The validator for the input.
  KTimeValidator* m_pcValidator;

signals:
  void timeChanged(QTime cNewTime);
};


#endif // KTIMEEDIT_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
