//*****************************************************************************
/*!
  \file chunkio.h
  \brief This file contains some helpers for the I/O system.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef CHUNKIO_H
#define CHUNKIO_H

#include <qstring.h>


//*****************************************************************************
/*!
  \def MAKE_CHUNK_ID(a,b,c,d)

  Create a chunk-id (unsigned 32 bit integer) from the arguments
  \a a, \a b, \a c and \a d.
*/
//*****************************************************************************

#define MAKE_CHUNK_ID(a,b,c,d) \
        ((unsigned int)((a)<<24 | (b)<<16 | (c)<<8 | (d)))


//*****************************************************************************
/*!
  \class IOException
  \brief The exception class that is thrown when the I/O system fails.

  \author André Johansen
*/
//*****************************************************************************

class IOException {
public:
  //! Initialise the exceptions object with \a cExplanation as explanation.
  IOException(const QString& cExplanation) : m_cExplanation(cExplanation) {}
  //! Get the explanation for the exception.
  QString explanation() const { return m_cExplanation; }

private:
  //! The exception explanation.
  QString m_cExplanation;
};

#endif // CHUNKIO_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
