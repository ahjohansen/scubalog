/*!
  \file scubalog.h
  \brief This file contains the definition of the main() function.

  This file is part of Scuba Log, a dive logging application for KDE.
  Scuba Log is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/

#include <kapp.h>
#include "scubalog.h"


/*!
  \relates ScubaLog

  The entry point for the application.

  \author André Johansen.
*/

int
main(int nArgumentCount, char** apzArguments)
{
  KApplication cApplication(nArgumentCount, apzArguments, "ScubaLog");
  ScubaLog* pcMainGUI = new ScubaLog("scubaLog");
  cApplication.setMainWidget(pcMainGUI);
  pcMainGUI->show();
  int nReturnValue = cApplication.exec();
  //delete pcMainGUI;
  return nReturnValue;
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
