//*****************************************************************************
/*!
  \file dateitem.h
  \brief This file contains the definition of the DateItem class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Hübert Johansen
*/
//*****************************************************************************

#ifndef DATEITEM_H
#define DATEITEM_H

#include <QTableWidgetItem>


//*****************************************************************************
/*!
  \class DateItem
  \brief The DateItem class is used to show dates in a QTableWidget.

  \author André Hübert Johansen
*/
//*****************************************************************************

class DateItem : public QTableWidgetItem
{
public:
  //! Initialise the object.
  DateItem(const QString& text)
    : QTableWidgetItem(text)
  {
  }

  virtual void setData(int role, const QVariant& value);
};

#endif // DATEITEM_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
