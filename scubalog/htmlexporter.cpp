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

  // Export the log index
  QString cLogFileName(cDirName);
  cLogFileName += "/";
  cLogFileName += "logbook.html";
  QFile cLogFile(cLogFileName);
  bool isLFOpen = cLogFile.open(IO_WriteOnly);
  if ( false == isLFOpen ) {
    QString cMessage;
    cMessage = "Couldn't open file for output\n(`" + cLogFileName + "')";
    errorMessage(cMessage);
    return false;
  }
  QTextStream cLogStream(&cLogFile);
  cLogStream << "<HTML>"
             << "<HEAD>\n"
             << "<TITLE>"
             << cLogBook.diverName()
             << " -- log index"
             << "</TITLE>\n"
             << "</HEAD>\n"
             << "<BODY>\n";
  for ( iLog.toFirst(); iLog.current(); ++iLog ) {
    const DiveLog* pcCurrentLog = iLog.current();
    QString cLogNumber;
    cLogNumber.setNum(pcCurrentLog->logNumber());
    cLogStream << "<A HREF=\""
               << cLogNumber
               << ".html\">"
               << cLogNumber
               << ".</A> "
               << pcCurrentLog->diveLocation()
               << "<BR>\n";
  }
  cLogStream << "\n"
             << "<HR>\n"
             << "Dive log exported from "
             << "<A HREF=\"http://www.stud.ifi.uio.no/~andrej/scubalog/\">"
             << "ScubaLog</A> "
             << cCurrentDate.toString()
             << "\n"
             << "</BODY>\n"
             << "</HTML>\n";

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
            << pcCurrentLog->diveLocation()
            << "<BR>\n"
            << "<B>Buddy:</B> "
            << pcCurrentLog->buddyName()
            << "<BR>\n"
            << "<B>Maximum depth:</B> "
            << pcCurrentLog->maxDepth()
            << "<BR>\n"
            << "<B>Dive time:</B> "
            << pcCurrentLog->diveTime().toString()
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
