//*****************************************************************************
/*!
  \file dateitem.h
  \brief This file contains the definition of the DateItem class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  Andr� Johansen.
*/
//*****************************************************************************

#ifndef DATEITEM_H
#define DATEITEM_H
#endif // DATEITEM_H

#include <qtable.h>

//*****************************************************************************
/*!
  \class DateItem
  \brief The DateItem class is used to show dates in a QTable widget.

  \author Andr� Johansen.
*/
//*****************************************************************************

class DateItem : public QTableItem
{
public:
  //! Initialise the object.
  DateItem(QTable* i_pcTable, EditType eEditType, const QString& i_cText)
    : QTableItem(i_pcTable, eEditType, i_cText)
  {
    setReplaceable(false);
  }

  void setContentFromEditor(QWidget* pcEditor);
};


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End: