/***************************************************************************
                          udcfexporter.h  -  description
                             -------------------
    begin                : Tue Sep 11 2001
    copyright            : (C) 2001 by Jordi Canton
    email                : JordiNitrox@virtual-sub.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//*****************************************************************************
/*!
  \file udcfexporter.h
  \brief This file contains the definition of the UDCFExporter Class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  Jordi Cantón
*/
//*****************************************************************************

#ifndef UDCFEXPORTER_H
#define UDCFEXPORTER_H

#include "exporter.h"

class DiveLog;
class LogBook;
class QString;
class QDomDocument;
class QDomElement;


//*****************************************************************************
/*!
  \class UDCFExporter
  \brief The UDCFExporter class is used to export to UDCF format.

  \author Jordi Canton.
*/
//*****************************************************************************

class UDCFExporter : public Exporter  {
public: 
	UDCFExporter();
	virtual ~UDCFExporter();
   //! Export the log \a cLog to the file \a cFileName.
  virtual bool exportLog(const DiveLog& cLog,
                         const QString& cFileName) const;
  //! Export the logbook \a cLogBook to the file \a cFileName.
  virtual bool exportLogBook(const LogBook& cLogBook,
                             const QString& cFileName) const;
private:
  //!displays an error message
  void errorMessage(const QString& cMessage) const;
  //!build the head of the document
  void buildHead(QDomDocument& doc) const;
  //!build the personal info
  void buildPersInfo(QDomDocument& doc,QDomElement& ,const LogBook& cLogBook) const;
  //!build the location logs
  void buildLocationLogs(QDomDocument& doc,QDomElement& ,const LogBook& cLogBook) const;
  //!build the equipment log
  void buildEquipmentLogs(QDomDocument& doc,QDomElement& ,const LogBook& cLogBook) const;
  //!build the profiles
  void buildDiveLogs(QDomDocument& doc,const LogBook& cLogBook) const;


};

#endif
