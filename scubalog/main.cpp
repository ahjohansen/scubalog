//*****************************************************************************
/*!
  \file main.cpp
  \brief This file contains the definition of the main() function.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "scubalog.h"
#include "config.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapp.h>
#include <qmessagebox.h>
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
  // Setup the about dialogue box.
  KAboutData aboutData("scubalog",
                       I18N_NOOP("ScubaLog"),
                       VERSION,
                       I18N_NOOP("ScubaLog is a scuba dive logging program,\n"
                                 "written for the K Desktop Environment."),
                       KAboutData::License_GPL,
                       "(c) 1999-2002, André Johansen",
                       0,
                       "http://home.tiscali.no/andrej/scubalog",
                       "andrejoh@c2i.net");
  aboutData.addAuthor("André Johansen",
                      "Author",
                      "andrejoh@c2i.net",
                      "http://home.tiscali.no/andrej/scubalog/");
  aboutData.addAuthor("Jordi Cantón",
                      "Porting to KDE v2",
                      "JordiNitrox@virtual-sub.org",
                      "http://www.virtual-sub.org");

  // Setup command line handling.
  KCmdLineOptions options[] = {
    { "+[File]", I18N_NOOP("ScubaLog file to open"), 0 },
    { 0, 0, 0 }
  };
  KCmdLineArgs::init( nArgumentCount, apzArguments, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options );

  int nReturnValue = 5;

  try {
    // Initialise the application
    KApplication cApplication;

    const char* pzLogBook = 0;

    // Validate arguments
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    if ( args->count() )
      pzLogBook = args->arg(0);

    ScubaLog* pcMainGUI = new ScubaLog("scubalog", pzLogBook);
    cApplication.setMainWidget(pcMainGUI);
    pcMainGUI->show();
    nReturnValue = cApplication.exec();
    pcMainGUI->saveConfig();
    delete pcMainGUI;
  }
  catch ( std::bad_alloc& ) {
    std::cerr << i18n("Out of memory -- exiting ScubaLog...\n");
    throw;
  }
  catch ( ... ) {
    std::cerr << i18n("Caught unhandled exception -- exiting ScubaLog...\n");
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
