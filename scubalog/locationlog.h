//*****************************************************************************
/*!
  \file locationlog.h
  \brief This file contains the definition of the LocationLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#ifndef LOCATIONLOG_H
#define LOCATIONLOG_H

#include <qstring.h>

//*****************************************************************************
/*!
  \class LocationLog
  \brief The LocationLog class is used to log information about a location.

  \author André Johansen.
*/
//*****************************************************************************

class LocationLog
{
public:
  LocationLog();
  ~LocationLog();

  QString getName() const;
  void setName(const QString& cName);
  QString getDescription() const;
  void setDescription(const QString& cDescription);

private:
  //! The name of the location.
  QString m_cName;
  //! The description of the location.
  QString m_cDescription;
};


#endif // LOCATIONLOG_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
