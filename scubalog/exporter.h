//*****************************************************************************
/*!
  \file exporter.h
  \brief This file contains the export interface for ScubaLog.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef EXPORTER_H
#define EXPORTER_H

class DiveLog;
class LogBook;
class QString;

//*****************************************************************************
/*!
  \class Exporter
  \brief The Exporter class is an interface for exporter classes.

  \author André Johansen
*/
//*****************************************************************************

class Exporter
{
public:
  //! Destroy the object.
  virtual ~Exporter() {}

  //! Export the log \a cLog to the file \a cName.
  virtual bool exportLog(const DiveLog& cLog,
                         const QString& cName) const = 0;
  //! Export the logbook \a cLogBook to  \a cName.
  virtual bool exportLogBook(const LogBook& cLogBook,
                             const QString& cName) const = 0;
};

#endif // EXPORTER_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
