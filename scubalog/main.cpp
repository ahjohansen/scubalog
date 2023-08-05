//*****************************************************************************
/*!
  \file main.cpp
  \brief This file contains the definition of the main() function.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Hübert Johansen
*/
//*****************************************************************************

#include "scubalog.h"
#include "config.h"

#include <kaboutdata.h>
#include <klocalizedstring.h>
#include <QCommandLineParser>
#include <qmessagebox.h>
#include <QApplication>
#include <iostream>
#include <new>

//*****************************************************************************
/*!
  \relates ScubaLog

  The entry point for the application.
*/
//*****************************************************************************

int
main(int nArgumentCount, char** apzArguments)
{
  int nReturnValue = 5;

  try {
    // Initialise the application
    QApplication cApplication(nArgumentCount, apzArguments);
    KLocalizedString::setApplicationDomain("scubalog");

    // Setup the about dialogue box.
    KAboutData aboutData("scubalog",
                         i18n("ScubaLog"),
                         VERSION,
                         i18n("ScubaLog is a scuba dive logging program,\n"
                              "written for the K Desktop Environment."),
                         KAboutLicense::GPL,
                         i18n("(c) 1999-2023, André Hübert Johansen"),
                         "",
                         "https://github.com/ahjohansen/scubalog",
                         "andrejoh@gmail.com");
    aboutData.addAuthor(i18n("André Hübert Johansen"),
                        i18n("Author"),
                        "andrejoh@gmail.com",
                        "https://github.com/ahjohansen/scubalog");
    aboutData.addAuthor(i18n("Jordi Cantón"),
                        i18n("Porting to KDE v2"),
                        "JordiNitrox@virtual-sub.org",
                        "http://www.virtual-sub.org");
    KAboutData::setApplicationData(aboutData);

    const char* pzLogBook = 0;

    // Command line handling.
    QCommandLineParser parser;
    parser.addPositionalArgument("file", i18n("ScubaLog file to open"));
    parser.process(cApplication);
    const QStringList args = parser.positionalArguments();
    if ( args.count() ) {
      pzLogBook = args.at(0).toUtf8().constData();
    }

    ScubaLog* pcMainGUI = new ScubaLog(pzLogBook);
    pcMainGUI->show();
    nReturnValue = cApplication.exec();
    pcMainGUI->saveConfig();
    delete pcMainGUI;
  }
  catch ( std::bad_alloc& ) {
    std::cerr << i18n("Out of memory -- exiting ScubaLog...\n").toStdString();
    throw;
  }
  catch ( ... ) {
    std::cerr << i18n("Caught unhandled exception -- exiting ScubaLog...\n").toStdString();
    throw;
  }

  return nReturnValue;
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
