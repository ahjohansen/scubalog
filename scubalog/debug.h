/*!
  \file ScubaLog/debug.h
  \brief This file contains debugging helpers.

  $Id$

  \par Copyright:
  André Johansen.
*/

#ifndef DEBUG_H
#define DEBUG_H

/*!
  \def DEBUG_ON

  This define is used to turn on debugging.
*/

/*!
  \def DBG(x)

  This macro is used to print debug information when DEBUG_ON is defined.
  When not, this macro is a noop.

  Notice that you need an extra pair of parenthesis around the print
  statement.

  \example
  DBG(("Testing the DBG() macro at line %d!\n", __LINE__));
  \endexample
*/

#if defined(DEBUG_ON)
# include <stdio.h>
# define DBG(x) do { printf x; } while (0);
#else
# define DBG(x)
#endif


#endif // DEBUG_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// End:
