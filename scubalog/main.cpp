//*****************************************************************************
/*!
  \file main.cpp
  \brief This file contains the definition of the main() function.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <new>
#include <iostream>
#include <qmessagebox.h>
#include <kapp.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include "config.h"
#include "scubalog.h"

static const char *description =
	I18N_NOOP("ScubaLog is a scuba dive logging program,\nwritten for the K Desktop Environment.");
static KCmdLineOptions options[] =
{
  { "+[File]", I18N_NOOP("ScubaLog file to open"), 0 },
  { 0, 0, 0 }
};

//*****************************************************************************
/*!
  \relates ScubaLog

  The entry point for the application.

  \author André Johansen, Jordi Cantón.
*/
//*****************************************************************************

int
main(int nArgumentCount, char** apzArguments)
{

  KAboutData aboutData( "scubalog", I18N_NOOP("ScubaLog"),
                        VERSION, description, KAboutData::License_GPL,
                        "(c) 2000-2001, André Johansen, Jordi Cantón", 0,
                        "http://www.virtual-sub.org/scubalog", "scubalogbug@virtual-sub.org");
  aboutData.addAuthor("Andre Johansen","Creator of the original ScubaLog", "");
  aboutData.addAuthor("Jordi Canton","Porting to KDE2",
                      "JordiNitrox@virtual-sub.org",
                      "http://www.virtual-sub.org");
  KCmdLineArgs::init( nArgumentCount, apzArguments, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options );

  try {
    // Initialize the application
    KApplication cApplication;

    const char* pzLogBook = 0;

    // Validate arguments
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    if ( args->count() )
      pzLogBook = args->arg(0);

    ScubaLog* pcMainGUI = new ScubaLog("scubalog", pzLogBook);
    cApplication.setMainWidget(pcMainGUI);
    pcMainGUI->show();
    int nReturnValue = cApplication.exec();
    pcMainGUI->saveConfig();
    delete pcMainGUI;
    return nReturnValue;
  }
  catch ( std::bad_alloc& ) {
    std::cerr << i18n("Out of memory -- exiting ScubaLog...\n");
    throw;
  }
  catch ( ... ) {
    std::cerr << i18n("Caught unhandled exception -- exiting ScubaLog...\n");
    throw;
  }
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
