//*****************************************************************************
/*!
  \file importer.h
  \brief This file contains the import interface for ScubaLog.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef IMPORTER_H
#define IMPORTER_H

#include <new>

class DiveLog;
class LogBook;
class QString;

//*****************************************************************************
/*!
  \class Importer
  \brief The Importer class is an interface for importer classes.

  Importer classes are used to read log book data into ScubaLog.

  \author André Johansen
*/
//*****************************************************************************

class Importer
{
public:
  //! Destructor.  Frees all internal resources.
  virtual ~Importer() {}

  //! Import a log from the file \a cFileName.
  //! Returns 0 on failure.
  virtual DiveLog* importLog(const QString& cFileName) const = 0;
  //! Import a logbook from \a cName, which might be a file or a directory,
  //! depending on the importer.
  //! Returns 0 on failure.
  virtual LogBook* importLogBook(const QString& cDirName) const = 0;
};

#endif // IMPORTER_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
