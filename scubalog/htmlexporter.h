//*****************************************************************************
/*!
  \file htmlexporter.h
  \brief This file contains the definition for the HTMLExporter class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef HTMLEXPORTER_H
#define HTMLEXPORTER_H

#include "exporter.h"


class DiveLog;
class LogBook;
class QString;


//*****************************************************************************
/*!
  \class HTMLExporter
  \brief The HTMLExporter class is used to export to HTML format.

  \author André Johansen
*/
//*****************************************************************************

class HTMLExporter : public Exporter {
public:
  HTMLExporter();
  virtual ~HTMLExporter();

  virtual bool exportLog(const DiveLog& cLog,
                         const QString& cFileName) const;
  virtual bool exportLogBook(const LogBook& cLogBook,
                             const QString& cFileName) const;

protected:
  bool exportIndex(const LogBook& cLogBook, const QString& cDirName) const;
  bool exportLocations(const LogBook& cLogBook, const QString& cDirName) const;

  bool hasLocation(const LogBook& cLogBook,
                   const QString& cLocationName) const;
  QString getLocationExportName(const QString& cLocationName) const;

  void createParagraphs(QString& cText) const;
  void createLinks(QString& cText) const;

  void errorMessage(const QString& cMessage) const;
};

#endif // HTMLEXPORTER_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
