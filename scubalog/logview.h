//*****************************************************************************
/*!
  \file ScubaLog/logview.h
  \brief This file contains the definition of the LogView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  Andr� Johansen.
*/
//*****************************************************************************

#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <qwidget.h>
#include <qdatetime.h>

class DiveLog;
class LogBook;
class QComboBox;
class QLineEdit;
class QMultiLineEdit;
class QPushButton;
class KIntegerEdit;
class KDateEdit;
class KTimeEdit;


//*****************************************************************************
/*!
  \class LogView
  \brief The LogView class handles editing a log.

  This class is the one viewed when editing a log.

  \author Andr� Johansen.
*/
//*****************************************************************************

class LogView : public QWidget {
  Q_OBJECT
public:
  LogView(QWidget* pcParent = 0, const char* pzName = 0);
  virtual ~LogView();

  void setLogBook(LogBook* pcLogBook);

public slots:
  void viewLog(DiveLog* pcLog);
  void newLog();
  void deletingLog(const DiveLog* pcLog);

private slots:
  void diveNumberChanged(int nNumber);
  void diveDateChanged(QDate cDate);
  void diveStartChanged(QTime cStart);
  void diveTimeChanged(QTime cTime);
  void gasTypeChanged(const char* pzGasType);
  void bottomTimeChanged(QTime cTime);
  void airTemperatureChanged(int nTemperature);
  void waterTemperatureChanged(int nTemperature);
  void locationChanged(const char* pzLocation);
  void maxDepthChanged(const char* pzDepth);
  void buddyChanged(const char* pzDepth);
  void diveTypeChanged(const char* pzDiveType);
  void diveDescriptionChanged();
  void gotoPreviousLog();
  void gotoNextLog();
  void editLocation();

private:
  //! The current log book.
  LogBook*      m_pcLogBook;
  //! The current dive log.
  DiveLog*      m_pcCurrentLog;

  //! The current dive number.
  KIntegerEdit* m_pcDiveNumber;
  //! The dive date.
  KDateEdit*    m_pcDiveDate;
  //! The dive start time.
  KTimeEdit*    m_pcDiveStart;
  //! The dive length.
  KTimeEdit*    m_pcDiveTime;
  //! The gas type.
  QLineEdit*    m_pcGasType;
  //! The air temperature.
  KIntegerEdit* m_pcAirTemp;
  //! The water temperature.
  KIntegerEdit* m_pcWaterTemp;
  //! The location.
  QLineEdit*    m_pcLocation;
  //! Edit the current location.
  QPushButton*  m_pcEditLocation;
  //! The plan type (single- or multi-level).
  QComboBox*    m_pcPlanSelector;
  //! The bottom time.
  KTimeEdit*    m_pcBottomTime;
  //! The maximum depth (should be changed with a distance widget).
  QLineEdit*    m_pcMaxDepth;
  //! The buddy.
  QLineEdit*    m_pcBuddy;
  //! The dive type selector.
  QComboBox*    m_pcDiveTypeSelector;
  //! The dive description editor.
  QMultiLineEdit* m_pcDescription;
  //! Move to previous log.
  QPushButton*    m_pcPreviousLog;
  //! Move to next log.
  QPushButton*    m_pcNextLog;

signals:
  void newLog(DiveLog* pcLog);
  void editLocation(const QString& cLocationName);
};


#endif // LOGVIEW_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
