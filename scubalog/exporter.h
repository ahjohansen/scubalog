//*****************************************************************************
/*!
  \file exporter.h
  \brief This file contains the export interface for ScubaLog.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
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

  \author André Johansen.
*/
//*****************************************************************************

class Exporter {
public:
  Exporter();
  virtual ~Exporter();

  //! Export the log \a cLog to the file \a cFileName.
  virtual bool exportLog(const DiveLog& cLog,
                         const QString& cFileName) const = 0;
  //! Export the logbook \a cLogBook to the directory \a cDirName.
  virtual bool exportLogBook(const LogBook& cLogBook,
                             const QString& cDirName) const = 0;
};

#endif // EXPORTER_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
