//*****************************************************************************
/*!
  \file dateitem.h
  \brief This file contains the definition of the DateItem class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef DATEITEM_H
#define DATEITEM_H

#include <q3table.h>

//*****************************************************************************
/*!
  \class DateItem
  \brief The DateItem class is used to show dates in a QTable widget.

  \author André Johansen
*/
//*****************************************************************************

class DateItem : public Q3TableItem
{
public:
  //! Initialise the object.
  DateItem(Q3Table* i_pcTable, EditType eEditType, const QString& i_cText)
    : Q3TableItem(i_pcTable, eEditType, i_cText)
  {
    setReplaceable(false);
  }

  void setContentFromEditor(QWidget* pcEditor);
};

#endif // DATEITEM_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
