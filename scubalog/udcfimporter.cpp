//#define DEBUG_ON
#include "debug.h"

#include "udcfimporter.h"
#include "logbook.h"
#include "locationlog.h"
#include "equipmentlog.h"
#include "divelist.h"
#include "divelog.h"

#include <KLocalizedString>
#include <QMessageBox>
#include <QApplication>
#include <QXmlStreamReader>
#include <QFile>

namespace
{

  static QString getXmlNameLower(QXmlStreamReader& xml)
  {
    return xml.name().toString().toLower();
  }

}


LogBook*
UDCFImporter::importLogBook(const QString& filename) const
{
  // Open the file
  QFile file(filename);
  if ( !file.open(QIODevice::ReadOnly) )
  {
    QString message;
    message = QString(i18n("Couldn't open file"))
      + "\n`" + filename + "'";
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] Read log book"),
                         message);
    return NULL;
  }


  LogBook* logbook = new LogBook();

  // Parse the XML
  QXmlStreamReader xml(&file);
  while ( xml.readNextStartElement() ) {
    if ( getXmlNameLower(xml) == "profile" &&
         xml.attributes().value("UDCF") == "1" ) {
      DBG(("found 'profile', reading logbook data ...\n"));
      readUDCF(logbook, xml);
    }
    else {
      xml.raiseError("Not an UDCF version 1 file.");
    }
  }

  DBG(("-- done reading ...\n"));
  if ( xml.hasError() ) {
    printf("ERROR: %s:%d: %s\n",
           filename.toUtf8().data(),
           (int)xml.lineNumber(),
           xml.errorString().toUtf8().data());
    //    ... // do error handling
    //delete logbook;
    //return NULL;
  }
  file.close();

  return logbook;
}


void UDCFImporter::readUDCF(LogBook* logbook,
                            QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() && getXmlNameLower(xml) == "profile");

  DBG(("Reading UDCF profile data\n"));

  while ( xml.readNextStartElement() ) {

    const QString element_name = getXmlNameLower(xml);
    DBG(("- Found section '%s'\n",
         element_name.toUtf8().data()));

    // Validate units; only metric supported
    if ( element_name == "units" ) {
      QString text = xml.readElementText();
      if ( text != "Metric" ) {
        DBG(("-- Found unsupported units = %s\n", text.toUtf8().data()));
        xml.raiseError("Unsupported units.");
      }
    }

    // Skip the device element with child nodes
    else if ( element_name == "device" ) {
      DBG(("-- Skipping 'device' section\n"));
      xml.skipCurrentElement();
    }

    else if ( element_name == "program" ) {
      DBG(("- Found 'program' section\n"));
      while ( xml.readNextStartElement() ) {
        const QString program_element_name = getXmlNameLower(xml);

        if ( program_element_name == "personal" ) {
          DBG(("-- Found 'personal' section\n"));
          readPersonalInfo(logbook, xml);
        }
        else if ( program_element_name == "locations" ) {
          DBG(("-- Found 'locations' section\n"));
          readLocations(logbook, xml);
        }
        else if ( program_element_name == "equipment" ) {
          DBG(("-- Found 'equipment' section\n"));
          readEquipment(logbook, xml);
        }
        else {
          xml.raiseError("Unsupported element.");
        }
      }
    }

    else if ( element_name == "repgroup" ) {
      DBG(("- Found 'repgroup' section\n"));
      readDiveLogs(logbook, xml);
    }

    else {
      DBG(("- Skipping unknown section %s\n",
           element_name.toUtf8().data()));
      xml.skipCurrentElement();
    }

  }

  DBG(("- Done reading UDCF data\n"));
}


/**
 * Read personal information from XML stream \a xml into \a logbook.
 */

void UDCFImporter::readPersonalInfo(LogBook* logbook,
                                    QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() &&
           xml.name().toString().toLower() == "personal");

  DBG(("-- Reading personal info\n"));

  while ( xml.readNextStartElement() ) {
    const QString element_name = getXmlNameLower(xml);
    QString text = xml.readElementText();
    DBG(("--- Element %s\n", element_name.toUtf8().data()));
    if ( element_name == "name" ) {
      logbook->setDiverName(text);
    }
    else if ( element_name == "mail" ) {
      logbook->setEmailAddress(text);
    }
    else if ( element_name == "url" ) {
      logbook->setWwwUrl(text);
    }
    else if ( element_name == "comments" ) {
      logbook->setComments(text);
    }
    else {
      xml.raiseError("Unsupported element.");
    }
  }

  DBG(("--- Done reading personal info\n"));
}

void UDCFImporter::readLocations(LogBook*          logbook,
                                 QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() &&
           xml.name().toString().toLower() == "locations");

  DBG(("-- Reading locations:\n"));

  while ( xml.readNextStartElement() ) {
    QString element_name;
    element_name = getXmlNameLower(xml);
    if ( element_name == "location" ) {
      LocationLog* location = new LocationLog();
      // Read location data
      while ( xml.readNextStartElement() ) {
        element_name = getXmlNameLower(xml);
        QString text = xml.readElementText();
        // DBG(("-- Element %s => %s\n",
        //      element_name.toUtf8().data(),
        //      text.toUtf8().data()));
        if ( element_name == "name" ) {
          location->setName(text);
        }
        else if ( element_name == "description" ) {
          location->setDescription(text);
        }
        else {
          xml.raiseError("Unsupported element.");
        }
      }
      DBG(("--- Found location \"%s\"\n",
           location->getName().toUtf8().data()));
      logbook->locationList().append(location);
    }
    else {
      xml.raiseError("Unsupported element.");
    }
  }

  DBG(("--- Found %d locations\n", logbook->locationList().size()));
}


void UDCFImporter::readEquipment(LogBook*          logbook,
                                 QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() &&
           xml.name().toString().toLower() == "equipment");

  DBG(("-- Reading equipment\n"));

  while ( xml.readNextStartElement() ) {
    QString element_name;
    element_name = getXmlNameLower(xml);
    if ( element_name == "piece" ) {
      EquipmentLog* equipment = new EquipmentLog();
      // Read equipment data
      while ( xml.readNextStartElement() ) {
        element_name = getXmlNameLower(xml);
        if ( element_name == "type" ) {
          QString text = xml.readElementText();
          equipment->setType(text);
        }
        else if ( element_name == "name" ) {
          QString text = xml.readElementText();
          equipment->setName(text);
        }
        else if ( element_name == "serial" ) {
          QString text = xml.readElementText();
          equipment->setSerialNumber(text);
        }
        else if ( element_name == "require" ) {
          QString text = xml.readElementText();
          equipment->setServiceRequirements(text);
        }
        else if ( element_name == "service" ) {
          readEquipmentHistory(equipment, xml);
        }
        else {
          printf("WARNING: unhandled element: %s\n",
                 xml.name().toUtf8().data());
        }
      }

      DBG(("--- Equipment type '%s', '%s'\n",
           equipment->type().toUtf8().data(),
           equipment->name().toUtf8().data()));
      logbook->equipmentLog().append(equipment);
    }
    else {
      QString text = xml.readElementText();
      printf("WARNING: unhandled element: %s = '%s'\n",
             xml.name().toUtf8().data(),
             text.toUtf8().data());
    }
  }

  DBG(("--- Found %d equipment items\n",
       logbook->equipmentLog().size()));
}


void
UDCFImporter::readEquipmentHistory(EquipmentLog*     equipment,
                                   QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() &&
           xml.name().toString().toLower() == "service");

  EquipmentHistoryEntry* history = new EquipmentHistoryEntry();
  while ( xml.readNextStartElement() ) {
    QString element_name = getXmlNameLower(xml);
    if ( element_name == "date" ) {
      QDate date = readDate(xml);
      history->setDate(date);
    }
    else if ( element_name == "comment" ) {
      history->setComment(xml.readElementText());
    }
    else {
      xml.raiseError("Unsupported element.");
      delete history;
      history = nullptr;
    }
  }

  if ( history ) {
    DBG(("---- Found equipment history\n"));
    equipment->history().append(history);
  }
}


QDate UDCFImporter::readDate(QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() &&
           xml.name().toString().toLower() == "date");

  int year  = 0;
  int month = 0;
  int day   = 0;

  while ( xml.readNextStartElement() ) {
    const QString element_name = getXmlNameLower(xml);
    const QString text = xml.readElementText();
    if ( element_name == "year" ) {
      year = text.toInt();
    }
    else if ( element_name == "month" ) {
      month = text.toInt();
    }
    else if ( element_name == "day" ) {
      day = text.toInt();
    }
    else {
      xml.raiseError("Unsupported element.");
    }
  }

  QDate date(year, month, day);
  if ( !date.isValid() ) {
    DBG(("Warning, invalid date year=%d month=%d day=%d\n",
         year, month, day));
    // xml.raiseError("Invalid date.");
  }

  return date;
}


QTime UDCFImporter::readTime(QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() &&
           xml.name().toString().toLower() == "time");

  int hour   = -1;
  int minute = -1;

  while ( xml.readNextStartElement() ) {
    const QString element_name = getXmlNameLower(xml);
    const QString text = xml.readElementText();
    if ( element_name == "hour" ) {
      hour = text.toInt();
    }
    else if ( element_name == "minute" ) {
      minute = text.toInt();
    }
    else {
      xml.raiseError("Unsupported element.");
    }
  }

  QTime time(hour, minute);
  if ( !time.isValid() ) {
    xml.raiseError("Invalid time.");
  }
  return time;
}


void UDCFImporter::readDiveLogs(LogBook* logbook, QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() &&
           xml.name().toString().toLower() == "repgroup");

  QString element_name;
  QString element_text;

  DBG(("-- Reading dive logs ...\n"));

  while ( xml.readNextStartElement() ) {
    element_name = getXmlNameLower(xml);

    if ( element_name == "dive" ) {
      DiveLog* divelog = new DiveLog();
      // Read dive log data
      while ( xml.readNextStartElement() ) {
        element_name = getXmlNameLower(xml);
        if ( element_name == "place" ) {
          const QString text = xml.readElementText();
          divelog->setDiveLocation(text);
        }
        else if ( element_name == "date" ) {
          QDate date = readDate(xml);
          divelog->setDiveDate(date);
        }
        else if ( element_name == "time" ) {
          QTime time = readTime(xml);
          divelog->setDiveStart(time);
        }
        else if ( element_name == "surfaceinterval" ) {
          QString interval = xml.readElementText();
          DBG(("---- Surface interval: %s\n",
               interval.toUtf8().data()));
        }
        else if ( element_name == "temperature" ) {
          element_text = xml.readElementText();
          divelog->setWaterTemperature(element_text.toFloat());
        }
        else if ( element_name == "density" ) {
          element_text = xml.readElementText();
          DBG(("---- Density: %s\n", element_text.toUtf8().data()));
        }
        else if ( element_name == "altitude" ) {
          element_text = xml.readElementText();
          DBG(("---- Altitude: %s\n", element_text.toUtf8().data()));
        }
        else if ( element_name == "gases" ) {
          readGas(divelog, xml);
        }
        else if ( element_name == "program" ) {
          while ( xml.readNextStartElement() ) {
            element_name = getXmlNameLower(xml);
            if ( element_name == "scubalog" ) {
              while ( xml.readNextStartElement() ) {
                element_name = getXmlNameLower(xml);
                if ( element_name == "number" ) {
                  element_text = xml.readElementText();
                  divelog->setLogNumber(element_text.toInt());
                }
                else if ( element_name == "divetime" ) {
                  element_text = xml.readElementText();
                  QTime time(0, element_text.toInt());
                  divelog->setDiveTime(time);
                }
                else if ( element_name == "bottomtime" ) {
                  element_text = xml.readElementText();
                  QTime time(0, element_text.toInt());
                  divelog->setBottomTime(time);
                }
                else if ( element_name == "airtemp" ) {
                  element_text = xml.readElementText();
                  float t = element_text.toFloat();
                  divelog->setAirTemperature(t);
                }
                else if ( element_name == "plantype" ) {
                  element_text = xml.readElementText();
                  int p = element_text.toInt();
                  DiveLog::PlanType_e pt =
                    (p == 0 ? DiveLog::e_SingleLevel : DiveLog::e_MultiLevel);
                  divelog->setPlanType(pt);
                }
                else if ( element_name == "buddy" ) {
                  element_text = xml.readElementText();
                  divelog->setBuddyName(element_text);
                }
                else if ( element_name == "type" ) {
                  element_text = xml.readElementText();
                  divelog->setDiveType(element_text);
                }
                else if ( element_name == "description" ) {
                  element_text = xml.readElementText();
                  divelog->setDiveDescription(element_text);
                }
                else {
                  xml.raiseError("Unsupported element.");
                }
              }
            }
          }
        }
        else if ( element_name == "timedepthmode" ) {
          xml.skipCurrentElement();
        }
        else if ( element_name == "samples" ) {
          // Read the five samples (T=time, D=depth)
          // - 1: T=0 D=0m
          // - 2: T=0 D=max
          // - 3: T=bottom-time D=max
          // - 4: T=divetime - bottom-time D=3m
          // - 5: T=divetime D=0m
          // Store max-depth from D2, bottom-time from T3, dive-time from T5
          int sample = 1;
          int time;
          float depth;
          while ( xml.readNextStartElement() ) {
            element_name = getXmlNameLower(xml);
            if ( element_name == "switch" ) {
              xml.skipCurrentElement();
            }
            else if ( element_name == "t" ) {
              element_text = xml.readElementText();
              time = element_text.toInt();
            }
            else if ( element_name == "d" ) {
              element_text = xml.readElementText();
              depth = element_text.toFloat();
              if ( sample == 2 ) {
                divelog->setMaxDepth(depth);
              }
              else if ( sample == 3 ) {
                int h = time / 60;
                int m = time - (h * 60);
                divelog->setBottomTime(QTime(h, m));
              }
              else if ( sample == 5 ) {
                int h = time / 60;
                int m = time - (h * 60);
                divelog->setDiveTime(QTime(h, m));
              }
              ++sample;
            }
          }
        }
        else {
          xml.raiseError("Unsupported element.");
        }
      }

      DBG(("--- Found dive log #%d at '%s'\n",
           divelog->logNumber(),
           divelog->diveLocation().toUtf8().data()));
      logbook->diveList().append(divelog);
    }
    else {
      xml.raiseError("Unsupported element.");
    }
  }

  DBG(("--- Found %d dive logs\n",
       logbook->diveList().size()));
}


void UDCFImporter::readGas(DiveLog* divelog, QXmlStreamReader& xml) const
{
  Q_ASSERT(xml.isStartElement() &&
           xml.name().toString().toLower() == "gases");

  DBG(("--- Reading gas type ...\n"));

  while ( xml.readNextStartElement() ) {
    QString element_name;
    element_name = getXmlNameLower(xml);
    if ( element_name == "mix" ) {
      while ( xml.readNextStartElement() ) {
        QString mix_element_name;
        mix_element_name = getXmlNameLower(xml);
        if ( mix_element_name == "mixname" ) {
          divelog->setGasType(xml.readElementText());
        }
        else if ( mix_element_name == "o2" ||
                  mix_element_name == "n2" ||
                  mix_element_name == "he" ) {
          xml.skipCurrentElement();
        }
      }
    }
    else {
      xml.raiseError("Unsupported element.");
    }
  }
}
