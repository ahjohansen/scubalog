//*****************************************************************************
/*!
  \file ScubaLog/htmlexporter.cpp
  \brief This file contains the implementation of HTMLExporter.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <limits.h>
#include <assert.h>
#include <qlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qregexp.h>
#include "debug.h"
#include "divelist.h"
#include "divelog.h"
#include "locationlog.h"
#include "logbook.h"
#include "htmlexporter.h"


//*****************************************************************************
/*!
  Initialize the exporter object.

  \author André Johansen.
*/
//*****************************************************************************

HTMLExporter::HTMLExporter()
{
}


//*****************************************************************************
/*!
  Destroy the exporter object.

  \author André Johansen.
*/
//*****************************************************************************

HTMLExporter::~HTMLExporter()
{
}


//*****************************************************************************
/*!
  Export the log \a cLog to the file \a cFileName.
  Returns `true' if successful, else `false'.

  \warning This function is not yet implemented.

  \author André Johansen.
*/
//*****************************************************************************

bool
HTMLExporter::exportLog(const DiveLog& /*cLog*/,
                        const QString& /*cFileName*/) const
{
  return false;
}


//*****************************************************************************
/*!
  Export the logbook \a cLogBook to the directory \a cDirName.
  Returns `true' on success, `false' on failure.

  \author André Johansen.
*/
//*****************************************************************************

bool
HTMLExporter::exportLogBook(const LogBook& cLogBook,
                            const QString& cDirName) const
{
  QDir cOutputDir(cDirName);
  if ( false == cOutputDir.exists() ) {
    if ( false == cOutputDir.mkdir(cDirName) ) {
      QString cMessage;
      cMessage = "Couldn't create output directory\n`" + cDirName + "'";
      errorMessage(cMessage);
    }
  }

  const QDate cCurrentDate(QDate::currentDate());
  const DiveList& cDiveList = cLogBook.diveList();
  QListIterator<DiveLog> iLog(cDiveList);

  bool isIndexOk = exportIndex(cLogBook, cDirName);
  if ( false == isIndexOk )
    return false;

  bool isLocationsOk = exportLocations(cLogBook, cDirName);
  if ( false == isLocationsOk )
    return false;


  //
  // Export the dive logs
  //
  for ( iLog.toFirst(); iLog.current(); ++iLog ) {
    const DiveLog* pcCurrentLog = iLog.current();
    QString cLogNumber;
    cLogNumber.setNum(pcCurrentLog->logNumber());
    QString cFileName(cDirName);
    cFileName += "/";
    cFileName += cLogNumber;
    cFileName += ".html";
    QFile cFile(cFileName);
    bool isOpen = cFile.open(IO_WriteOnly);
    if ( false == isOpen ) {
      QString cMessage;
      cMessage = "Couldn't open file for output\n(`" + cFileName + "')";
      errorMessage(cMessage);
      return false;
    }

    // Process the dive description
    QString cDescription(pcCurrentLog->diveDescription());
    cDescription.detach();
    createLinks(cDescription);
    createParagraphs(cDescription);

    // Create location text
    QString cLocationName;
    bool isLocationLogged =
      hasLocation(cLogBook, pcCurrentLog->diveLocation());
    if ( isLocationLogged ) {
      cLocationName += "<A HREF=\""
        + getLocationExportName(pcCurrentLog->diveLocation())
        + "\">" + pcCurrentLog->diveLocation() + "</A>";        
    }
    else {
      cLocationName = pcCurrentLog->diveLocation();
    }

    // Create dive time text
    const QTime cDiveTime(pcCurrentLog->diveTime());
    QString cDiveTimeText;
    if ( cDiveTime.hour() ) {
      QString cHours;
      cHours.setNum(cDiveTime.hour());
      cDiveTimeText += cHours + " hour";
      if ( cDiveTime.hour() != 1 )
        cDiveTimeText += "s";
      cDiveTimeText += " ";
    }
    if ( cDiveTime.minute() ) {
      QString cMinutes;
      cMinutes.setNum(cDiveTime.minute());
      cDiveTimeText += cMinutes + " minute";
      if ( cDiveTime.minute() != 1 )
        cDiveTimeText += "s";
      cDiveTimeText += " ";
    }
    if ( cDiveTime.second() ) {
      QString cSeconds;
      cSeconds.setNum(cDiveTime.second());
      cDiveTimeText += cSeconds + " second";
      if ( cDiveTime.second() != 1 )
        cDiveTimeText += "s";
      cDiveTimeText += " ";
    }

    // Output
    QTextStream cStream(&cFile);
    cStream << "<HTML>"
            << "<HEAD>\n"
            << "<TITLE>"
            << cLogBook.diverName()
            << " -- Log "
            << pcCurrentLog->logNumber()
            << "</TITLE>\n"
            << "</HEAD>\n"
            << "<BODY>\n"
            << "<B>Log number:</B> "
            << pcCurrentLog->logNumber()
            << "<BR>\n"
            << "<B>Date:</B> "
            << pcCurrentLog->diveDate().toString()
            << " "
            << pcCurrentLog->diveStart().toString()
            << "<BR>\n"
            << "<B>Location:</B> "
            << cLocationName
            << "<BR>\n"
            << "<B>Buddy:</B> "
            << pcCurrentLog->buddyName()
            << "<BR>\n"
            << "<B>Maximum depth:</B> "
            << pcCurrentLog->maxDepth()
            << "m<BR>\n"
            << "<B>Dive time:</B> "
            << cDiveTimeText
            << "<BR>\n"
            << "<P>\n"
            << cDescription
            << "\n<HR>\n";
    if ( false == iLog.atFirst() ) {
      QListIterator<DiveLog> iPreviousLog = iLog;
      --iPreviousLog;
      cStream << "<A HREF=\""
              << (iPreviousLog.current())->logNumber()
              << ".html\">Previous log</A> ";
    }
    if ( false == iLog.atLast() ) {
      QListIterator<DiveLog> iNextLog = iLog;
      ++iNextLog;
      cStream << "<A HREF=\""
              << (iNextLog.current())->logNumber()
              << ".html\">Next log</A> ";
    }
    cStream << "<A HREF=\"logbook.html\">Index</A>\n"
            << "<P>\n"
            << "Dive log exported from "
            << "<A HREF=\"http://www.stud.ifi.uio.no/~andrej/scubalog/\">"
            << "ScubaLog</A> "
            << cCurrentDate.toString()
            << "\n"
            << "</BODY>\n"
            << "</HTML>\n";

    // Ensure output was successful
    if ( IO_Ok != cFile.status() ) {
      QString cMessage;
      cMessage = "Error outputting log\n(`" + cFileName + "')";
      errorMessage(cMessage);
      cFile.remove();
      return false;
    }

  }

  return true;
}


//*****************************************************************************
/*!
  Export the dive log and location log index for the log book \a cLogBook
  to the directory \a cDirName.

  The output file will be called `logbook.html'.

  Returns `true' on success and `false' on failure.

  \author André Johansen.
*/
//*****************************************************************************

bool
HTMLExporter::exportIndex(const LogBook& cLogBook,
                          const QString& cDirName) const
{
  const QDate cCurrentDate(QDate::currentDate());
  const DiveList& cDiveList = cLogBook.diveList();
  QListIterator<DiveLog> iLog(cDiveList);

  const QString cFileName(cDirName + "/logbook.html");
  QFile cFile(cFileName);
  bool isOpen = cFile.open(IO_WriteOnly);
  if ( false == isOpen ) {
    const QString cMessage("Couldn't open file for output\n(`" +
                           cFileName + "')");
    errorMessage(cMessage);
    return false;
  }
  QTextStream cStream(&cFile);
  cStream << "<HTML>"
          << "<HEAD>\n"
          << "<TITLE>"
          << cLogBook.diverName()
          << " -- log book index"
          << "</TITLE>\n"
          << "</HEAD>\n"
          << "<BODY>\n"
          << "<H1>Dive logs</H1>\n";
  for ( iLog.toFirst(); iLog.current(); ++iLog ) {
    const DiveLog* pcCurrentLog = iLog.current();
    QString cLogNumber;
    cLogNumber.setNum(pcCurrentLog->logNumber());
    cStream << "<A HREF=\""
            << cLogNumber
            << ".html\">"
            << cLogNumber
            << ".</A> "
            << pcCurrentLog->diveLocation()
            << "<BR>\n";
  }
  cStream << "<H1>Location logs</H1>\n";
  QListIterator<LocationLog> iLocation(cLogBook.locationList());
  for ( ; iLocation.current(); ++iLocation ) {
    const LocationLog* pcCurrentLog = iLocation.current();
    cStream << "<A HREF=\""
            << getLocationExportName(pcCurrentLog->getName())
            << "\">"
            << pcCurrentLog->getName()
            << "</A><BR>\n";
  }
  cStream << "<HR>\n"
          << "Dive log exported from "
          << "<A HREF=\"http://www.stud.ifi.uio.no/~andrej/scubalog/\">"
          << "ScubaLog</A> "
          << cCurrentDate.toString()
          << "\n"
          << "</BODY>\n"
          << "</HTML>\n";


  bool isOk = IO_Ok == cFile.status();
  cFile.close();
  if ( false == isOk ) {
    QFile::remove(cFileName);
    QString cMessage("Error writing to file\n`" + cFileName + "'!");
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Write log index",
                         cMessage);
  }

  return isOk;
}


//*****************************************************************************
/*!
  Export the location logs for the log book \a cLogBook
  to the directory \a cDirName.

  Returns `true' on success and `false' on failure.

  \author André Johansen.
*/
//*****************************************************************************

bool
HTMLExporter::exportLocations(const LogBook& cLogBook,
                              const QString& cDirName) const
{
  const QDate cCurrentDate(QDate::currentDate());
  const QList<LocationLog>& cLocationList = cLogBook.locationList();
  QListIterator<LocationLog> iLog(cLocationList);

  for ( ; iLog.current(); ++iLog ) {
    const LocationLog* pcLog = iLog.current();
    assert(pcLog);
    const QString cLocationName(pcLog->getName());
    QString cFileName(cDirName + "/");
    cFileName += getLocationExportName(cLocationName);
    QFile cFile(cFileName);
    bool isOpen = cFile.open(IO_WriteOnly);
    if ( false == isOpen ) {
      const QString cMessage("Couldn't open file for output\n(`" +
                             cFileName + "')");
      errorMessage(cMessage);
      return false;
    }

    QString cDescription(pcLog->getDescription());
    cDescription.detach();
    createParagraphs(cDescription);
    createLinks(cDescription);

    QTextStream cStream(&cFile);
    cStream << "<HTML>"
            << "<HEAD>\n"
            << "<TITLE>"
            << cLogBook.diverName()
            << " -- "
            << cLocationName
            << "</TITLE>\n"
            << "</HEAD>\n"
            << "<BODY>\n"
            << "<B>Location:</B> "
            << cLocationName
            << "<BR>\n"
            << "<P>\n"
            << cDescription
            << "\n<HR>\n";
    if ( false == iLog.atFirst() ) {
      QListIterator<LocationLog> iPreviousLog = iLog;
      --iPreviousLog;
      cStream << "<A HREF=\""
              << getLocationExportName((iPreviousLog.current())->getName())
              << "\">Previous location</A> ";
    }
    if ( false == iLog.atLast() ) {
      QListIterator<LocationLog> iNextLog = iLog;
      ++iNextLog;
      cStream << "<A HREF=\""
              << getLocationExportName((iNextLog.current())->getName())
              << "\">Next location</A> ";
    }
    cStream << "<A HREF=\"logbook.html\">Index</A>\n"
            << "<P>\n"
            << "Dive log exported from "
            << "<A HREF=\"http://www.stud.ifi.uio.no/~andrej/scubalog/\">"
            << "ScubaLog</A> "
            << cCurrentDate.toString()
            << "\n"
            << "</BODY>\n"
            << "</HTML>\n";

    // Ensure output was successful
    if ( IO_Ok != cFile.status() ) {
      QString cMessage;
      cMessage = "Error outputting log\n(`" + cFileName + "')";
      errorMessage(cMessage);
      cFile.remove();
      return false;
    }
  }

  return true;
}


//*****************************************************************************
/*!
  Create a filename suitable for the location name \a cLocationName.

  \author André Johansen.
*/
//*****************************************************************************

QString
HTMLExporter::getLocationExportName(const QString& cLocationName) const
{
  QString cExportName(cLocationName);
  cExportName.detach();
  QRegExp cExpression("[\\s!\"#\\c$%%&'()\\c*\\c+,-./:<=>?@"
                      "\\^`{|}~\\c[\\x5d]");
  assert(cExpression.isValid());
  cExportName.replace(cExpression, "_");
  cExportName += ".html";
  cExportName = cExportName.lower();
  DBG(("Export name for `%s' is `%s'\n", cLocationName.data(),
       cExportName.data()));
  return cExportName;
}


//*****************************************************************************
/*!
  Search the location logs in the log book \a cLogBook for a location
  with the name \a cLocationName.

  If one is found, return `true', else return `false'.

  \author André Johansen.
*/
//*****************************************************************************

bool
HTMLExporter::hasLocation(const LogBook& cLogBook,
                          const QString& cLocationName) const
{
  QListIterator<LocationLog> iLog(cLogBook.locationList());
  bool isFound = false;
  for ( ; iLog.current(); ++iLog ) {
    const LocationLog* pcLog = iLog.current();
    if ( pcLog->getName() == cLocationName ) {
      isFound = true;
      break;
    }
  }
  return isFound;
}


//*****************************************************************************
/*!
  Create paragraphs in the output by replacing double newlines in \a cText
  with an HTML paragraph marker.

  \author André Johansen.
*/
//*****************************************************************************

void
HTMLExporter::createParagraphs(QString& cText) const
{
  cText.replace(QRegExp("\n\n"), "\n<P>\n");
}


//*****************************************************************************
/*!
  Search for http URLs in the input \a cText and create real links.
  The link-text will be the link itself.

  \author André Johansen.
*/
//*****************************************************************************

void
HTMLExporter::createLinks(QString& cText) const
{
  const QRegExp cWS("\\s");
  bool isDone = false;
  int nSearchPos = 0;
  do {
    int nStartPos = cText.find("http://", nSearchPos);
    if ( -1 == nStartPos ) {
      isDone = true;
      continue;
    }
    int nEndPos = cText.find(cWS, nStartPos);
    if ( -1 == nEndPos )
      nEndPos = cText.length();
    int nLength = nEndPos - nStartPos;

    const QString cAddress = cText.mid(nStartPos, nLength);
    DBG(("Found URL `%s' starting at %d, length %d\n",
         cAddress.data(), nStartPos, nLength));

    QString cLink;
    cLink = "<A HREF=\"" + cAddress + "\">" + cAddress + "</A>";

    cText.replace(nStartPos, nLength, cLink);
    nSearchPos = nStartPos + cLink.length();
  } while ( false == isDone );
}


//*****************************************************************************
/*!
  Display the error-message \a cMessage.

  \author André Johansen.
*/
//*****************************************************************************

void
HTMLExporter::errorMessage(const QString& cMessage) const
{
  QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] Output error",
                       cMessage);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
