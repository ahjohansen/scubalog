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
#include "scubalog.h"


//*****************************************************************************
/*!
  \relates ScubaLog

  The entry point for the application.

  \author André Johansen.
*/
//*****************************************************************************

int
main(int nArgumentCount, char** apzArguments)
{
  try {
    // Initialize the application
    KApplication cApplication(nArgumentCount, apzArguments, "scubalog");

    const char* pzLogBook = 0;

    // Validate arguments
    if ( nArgumentCount > 2 ) {
      QMessageBox::information(0, i18n("[ScubaLog] Invalid argument"),
                               i18n("ScubaLog encountered an invalid "
                                    "argument.\n"
                                    "ScubaLog can either be started with no "
                                    "arguments,\n"
                                    "or optionally with one argument giving "
                                    "the filename of a log book."));
    }
    else if ( 2 == nArgumentCount )
      pzLogBook = apzArguments[1];

    ScubaLog* pcMainGUI = new ScubaLog("scubaLog", pzLogBook);
    cApplication.setMainWidget(pcMainGUI);
    pcMainGUI->show();
    int nReturnValue = cApplication.exec();
    pcMainGUI->saveConfig();
    delete pcMainGUI;
    return nReturnValue;
  }
  catch ( std::bad_alloc ) {
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
