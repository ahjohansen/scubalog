//*****************************************************************************
/*!
  \file htmlexporter.cpp
  \brief This file contains the implementation of HTMLExporter.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include <limits.h>
#include <assert.h>
#include <qlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <kapplication.h>
#include <klocale.h>
#include "debug.h"
#include "divelist.h"
#include "divelog.h"
#include "locationlog.h"
#include "logbook.h"
#include "htmlexporter.h"


//*****************************************************************************
/*!
  Initialise the exporter object.
*/
//*****************************************************************************

HTMLExporter::HTMLExporter()
{
}


//*****************************************************************************
/*!
  Destroy the exporter object.
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
      cMessage = QString(i18n("Couldn't create output directory"))
        + "\n`" + cDirName + "'";
      errorMessage(cMessage);
    }
  }

  const QDate cCurrentDate(QDate::currentDate());
  const DiveList& cDiveList = cLogBook.diveList();
  QListIterator<DiveLog*> iLog(cDiveList);

  bool isIndexOk = exportIndex(cLogBook, cDirName);
  if ( false == isIndexOk )
    return false;

  bool isLocationsOk = exportLocations(cLogBook, cDirName);
  if ( false == isLocationsOk )
    return false;


  //
  // Export the dive logs
  //
  while ( iLog.hasNext() ) {
    const DiveLog* pcCurrentLog = iLog.next();
    QString cLogNumber;
    cLogNumber.setNum(pcCurrentLog->logNumber());
    QString cFileName(cDirName);
    cFileName += "/";
    cFileName += cLogNumber;
    cFileName += ".html";
    QFile cFile(cFileName);
    bool isOpen = cFile.open(QIODevice::WriteOnly);
    if ( false == isOpen ) {
      QString cMessage;
      cMessage = QString(i18n("Couldn't open file for output"))
        + "\n(`" + cFileName + "')";
      errorMessage(cMessage);
      return false;
    }

    // Process the dive description
    QString cDescription(pcCurrentLog->diveDescription());
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
      cDiveTimeText += cHours + " ";
      if ( cDiveTime.hour() == 1 )
        cDiveTimeText += i18n("hour") + QString(" ");
      else
        cDiveTimeText += i18n("hours") + QString(" ");
    }
    if ( cDiveTime.minute() ) {
      QString cMinutes;
      cMinutes.setNum(cDiveTime.minute());
      cDiveTimeText += cMinutes + " ";
      if ( cDiveTime.minute() == 1 )
        cDiveTimeText += i18n("minute") + QString(" ");
      else
        cDiveTimeText += i18n("minutes") + QString(" ");
    }
    if ( cDiveTime.second() ) {
      QString cSeconds;
      cSeconds.setNum(cDiveTime.second());
      cDiveTimeText += cSeconds + " ";
      if ( cDiveTime.second() == 1 )
        cDiveTimeText += i18n("seconds") + QString(" ");
      else
        cDiveTimeText += i18n("second") + QString(" ");
    }

    // Output
    QTextStream cStream(&cFile);
    cStream << "<HTML>"
            << "<HEAD>\n"
            << "<TITLE>"
            << cLogBook.diverName()
            << " -- " << i18n("Log") << " "
            << pcCurrentLog->logNumber()
            << "</TITLE>\n"
            << "</HEAD>\n"
            << "<BODY>\n"
            << "<B>" << i18n("Log number") << ":</B> "
            << pcCurrentLog->logNumber()
            << "<BR>\n"
            << "<B>" << i18n("Date") << ":</B> "
            << pcCurrentLog->diveDate().toString()
            << " "
            << pcCurrentLog->diveStart().toString()
            << "<BR>\n"
            << "<B>" << i18n("Location") << ":</B> "
            << cLocationName
            << "<BR>\n"
            << "<B>" << i18n("Buddy") << ":</B> "
            << pcCurrentLog->buddyName()
            << "<BR>\n"
            << "<B>" << i18n("Maximum depth") << ":</B> "
            << pcCurrentLog->maxDepth()
            << "m<BR>\n"
            << "<B>" << i18n("Dive time") << ":</B> "
            << cDiveTimeText
            << "<BR>\n"
            << "<P>\n"
            << cDescription
            << "\n<HR>\n";
    if ( !iLog.hasPrevious() ) {
      const DiveLog* pcPreviousLog = iLog.peekPrevious();
      cStream << "<A HREF=\""
              << pcPreviousLog->logNumber()
              << ".html\">" << i18n("Previous log") << "</A> ";
    }
    if ( iLog.hasNext() ) {
      const DiveLog* pcNextLog = iLog.peekNext();
      cStream << "<A HREF=\""
              << pcNextLog->logNumber()
              << ".html\">" << i18n("Next log") << "</A> ";
    }
    cStream << "<A HREF=\"logbook.html\">" << i18n("Index") << "</A>\n"
            << "<P>\n"
            << i18n("Dive log exported from")
            << " <A HREF=\"http://home.tiscali.no/andrej/scubalog/\">"
            << "ScubaLog</A> "
            << cCurrentDate.toString()
            << "\n"
            << "</BODY>\n"
            << "</HTML>\n";

    // Ensure output was successful
    if ( cFile.error() != QFile::NoError ) {
      QString cMessage;
      cMessage = QString(i18n("Error outputting log"))
        + "\n(`" + cFileName + "')";
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
*/
//*****************************************************************************

bool
HTMLExporter::exportIndex(const LogBook& cLogBook,
                          const QString& cDirName) const
{
  const QDate cCurrentDate(QDate::currentDate());
  const DiveList& cDiveList = cLogBook.diveList();
  QListIterator<DiveLog*> iLog(cDiveList);

  const QString cFileName(cDirName + "/logbook.html");
  QFile cFile(cFileName);
  bool isOpen = cFile.open(QIODevice::WriteOnly);
  if ( false == isOpen ) {
    QString cMessage;
    cMessage = QString(i18n("Couldn't open file for output"))
      + "\n(`" + cFileName + "')";
    errorMessage(cMessage);
    return false;
  }
  QTextStream cStream(&cFile);
  cStream << "<HTML>"
          << "<HEAD>\n"
          << "<TITLE>"
          << cLogBook.diverName()
          << " -- " << i18n("log book index")
          << "</TITLE>\n"
          << "</HEAD>\n"
          << "<BODY>\n"
          << "<H1>" << i18n("Dive logs") << "</H1>\n";
  for ( ; iLog.hasNext(); ) {
    const DiveLog* pcCurrentLog = iLog.next();
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
  cStream << "<H1>" << i18n("Location logs") << "</H1>\n";
  QListIterator<LocationLog*> iLocation(cLogBook.locationList());
  for ( ; iLocation.hasNext(); ) {
    const LocationLog* pcCurrentLog = iLocation.next();
    cStream << "<A HREF=\""
            << getLocationExportName(pcCurrentLog->getName())
            << "\">"
            << pcCurrentLog->getName()
            << "</A><BR>\n";
  }
  cStream << "<HR>\n"
          << i18n("Dive log exported from")
          << " <A HREF=\"http://home.tiscali.no/andrej/scubalog/\">"
          << "ScubaLog</A> "
          << cCurrentDate.toString()
          << "\n"
          << "</BODY>\n"
          << "</HTML>\n";


  const bool isOk = (cFile.error() == QFile::NoError);
  cFile.close();
  if ( !isOk ) {
    QFile::remove(cFileName);
    QString cMessage;
    cMessage = QString(i18n("Error writing to file"))
      + "\n`" + cFileName + "'!";
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] Write log index"), cMessage);
  }

  return isOk;
}


//*****************************************************************************
/*!
  Export the location logs for the log book \a cLogBook
  to the directory \a cDirName.

  Returns `true' on success and `false' on failure.
*/
//*****************************************************************************

bool
HTMLExporter::exportLocations(const LogBook& cLogBook,
                              const QString& cDirName) const
{
  const QDate cCurrentDate(QDate::currentDate());
  const QList<LocationLog*>& cLocationList = cLogBook.locationList();
  QListIterator<LocationLog*> iLog(cLocationList);

  for ( ; iLog.hasNext(); ) {
    const LocationLog* pcLog = iLog.next();
    assert(pcLog);
    const QString cLocationName(pcLog->getName());
    QString cFileName(cDirName + "/");
    cFileName += getLocationExportName(cLocationName);
    QFile cFile(cFileName);
    bool isOpen = cFile.open(QIODevice::WriteOnly);
    if ( false == isOpen ) {
      QString cMessage;
      cMessage = QString(i18n("Couldn't open file for output"))
        + "\n(`" + cFileName + "')";
      errorMessage(cMessage);
      return false;
    }

    QString cDescription(pcLog->getDescription());
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
            << "<B>" << i18n("Location") << ":</B> "
            << cLocationName
            << "<BR>\n"
            << "<P>\n"
            << cDescription
            << "\n<HR>\n";
    if ( iLog.hasPrevious() ) {
      LocationLog* pcPreviousLog = iLog.peekPrevious();
      cStream << "<A HREF=\""
              << getLocationExportName(pcPreviousLog->getName())
              << "\">" << i18n("Previous location") << "</A> ";
    }
    if ( iLog.hasNext() ) {
      LocationLog* pcNextLog = iLog.peekNext();
      cStream << "<A HREF=\""
              << getLocationExportName(pcNextLog->getName())
              << "\">" << i18n("Next location") << "</A> ";
    }
    cStream << "<A HREF=\"logbook.html\">" << i18n("Index") << "</A>\n"
            << "<P>\n"
            << i18n("Dive log exported from")
            << " <A HREF=\"http://home.tiscali.no/andrej/scubalog/\">"
            << "ScubaLog</A> "
            << cCurrentDate.toString()
            << "\n"
            << "</BODY>\n"
            << "</HTML>\n";

    // Ensure output was successful
    if ( cFile.error() != QFile::NoError ) {
      QString cMessage;
      cMessage = QString(i18n("Error outputting log"))
        + "\n(`" + cFileName + "')";
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
*/
//*****************************************************************************

QString
HTMLExporter::getLocationExportName(const QString& cLocationName) const
{
  QString cExportName(cLocationName);
  QRegExp cExpression("[\\s!\"#\\c$%%&'()\\c*\\c+,-./:<=>?@"
                      "\\^`{|}~\\c[\\x5d]");
  assert(cExpression.isValid());
  cExportName.replace(cExpression, "_");
  cExportName += ".html";
  cExportName = cExportName.toLower();
  DBG(("Export name for `%s' is `%s'\n", cLocationName.data(),
       cExportName.data()));
  return cExportName;
}


//*****************************************************************************
/*!
  Search the location logs in the log book \a cLogBook for a location
  with the name \a cLocationName.

  If one is found, return `true', else return `false'.
*/
//*****************************************************************************

bool
HTMLExporter::hasLocation(const LogBook& cLogBook,
                          const QString& cLocationName) const
{
  QListIterator<LocationLog*> iLog(cLogBook.locationList());
  bool isFound = false;
  for ( ; iLog.hasNext(); ) {
    const LocationLog* pcLog = iLog.next();
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
*/
//*****************************************************************************

void
HTMLExporter::createLinks(QString& cText) const
{
  const QRegExp cWS("\\s");
  bool isDone = false;
  int nSearchPos = 0;
  do {
    int nStartPos = cText.indexOf("http://", nSearchPos);
    if ( -1 == nStartPos ) {
      isDone = true;
      continue;
    }
    int nEndPos = cText.indexOf(cWS, nStartPos);
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
*/
//*****************************************************************************

void
HTMLExporter::errorMessage(const QString& cMessage) const
{
  QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                       i18n("[ScubaLog] Output error"),
                       cMessage);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
