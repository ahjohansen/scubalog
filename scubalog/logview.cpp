//*****************************************************************************
/*!
  \file ScubaLog/logview.cpp
  \brief This file contains the implementation of LogView.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <new>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qmultilinedit.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include "debug.h"
#include "kintegeredit.h"
#include "kdateedit.h"
#include "ktimeedit.h"
#include "divelist.h"
#include "logview.h"
#include "logview.moc"


//*****************************************************************************
/*!
  Initialize the log view with \a pcParent as the parent widget
  and \a pzName as the widget name.

  \author André Johansen.
*/
//*****************************************************************************

LogView::LogView(QWidget* pcParent, const char* pzName)
  : QWidget(pcParent, pzName), m_pcDiveLogList(0), m_pcCurrentLog(0)
{
  QLabel* pcDiveNumLabel = new QLabel(this, "diveNumberText");
  pcDiveNumLabel->setText("Dive &number:");
  pcDiveNumLabel->setMinimumSize(pcDiveNumLabel->sizeHint());

  m_pcDiveNumber = new KIntegerEdit(1, 1, INT_MAX, this, "diveNumber");
  m_pcDiveNumber->setText("1");
  m_pcDiveNumber->setMinimumSize(m_pcDiveNumber->sizeHint());
  pcDiveNumLabel->setBuddy(m_pcDiveNumber);
  connect(m_pcDiveNumber, SIGNAL(integerChanged(int)),
          SLOT(diveNumberChanged(int)));

  QLabel* pcDiveDateLabel = new QLabel(this, "diveDateText");
  pcDiveDateLabel->setText("Dive &date:");
  pcDiveDateLabel->setMinimumSize(pcDiveDateLabel->sizeHint());

  m_pcDiveDate = new KDateEdit(this, "diveDate");
  m_pcDiveDate->setMinimumSize(m_pcDiveDate->sizeHint());
  pcDiveDateLabel->setBuddy(m_pcDiveDate);
  connect(m_pcDiveDate, SIGNAL(dateChanged(QDate)),
          SLOT(diveDateChanged(QDate)));

  QLabel* pcDiveStartLabel = new QLabel(this, "diveStartText");
  pcDiveStartLabel->setText("Dive &start:");
  pcDiveStartLabel->setMinimumSize(pcDiveStartLabel->sizeHint());

  m_pcDiveStart = new KTimeEdit(this, "diveStart");
  m_pcDiveStart->setMinimumSize(m_pcDiveStart->sizeHint());
  pcDiveStartLabel->setBuddy(m_pcDiveStart);
  connect(m_pcDiveStart, SIGNAL(timeChanged(QTime)),
          SLOT(diveStartChanged(QTime)));

  QLabel* pcDiveTimeLabel = new QLabel(this, "diveTimeText");
  pcDiveTimeLabel->setText("Dive &time:");
  pcDiveTimeLabel->setMinimumSize(pcDiveTimeLabel->sizeHint());

  m_pcDiveTime = new KTimeEdit(this, "diveTime");
  m_pcDiveTime->setMinimumSize(m_pcDiveTime->sizeHint());
  pcDiveTimeLabel->setBuddy(m_pcDiveTime);
  connect(m_pcDiveTime, SIGNAL(timeChanged(QTime)),
          SLOT(diveTimeChanged(QTime)));

  QLabel* pcGasLabel = new QLabel(this, "gasText");
  pcGasLabel->setText("&Gas:");
  pcGasLabel->setMinimumSize(pcGasLabel->sizeHint());

  m_pcGasType = new QLineEdit(this, "gasType");
  m_pcGasType->setMinimumSize(m_pcGasType->sizeHint());
  pcGasLabel->setBuddy(m_pcGasType);
  connect(m_pcGasType, SIGNAL(textChanged(const char*)),
          SLOT(gasTypeChanged(const char*)));

  QLabel* pcAirTempLabel = new QLabel(this, "airTempText");
  pcAirTempLabel->setText("&Air temperature:");
  pcAirTempLabel->setMinimumSize(pcAirTempLabel->sizeHint());

  m_pcAirTemp = new KIntegerEdit(-100, 20, 100, this, "airTemperature");
  m_pcAirTemp->setMinimumSize(m_pcAirTemp->sizeHint());
  pcAirTempLabel->setBuddy(m_pcAirTemp);
  connect(m_pcAirTemp, SIGNAL(integerChanged(int)),
          SLOT(airTemperatureChanged(int)));

  QLabel* pcLocationLabel = new QLabel(this, "locationText");
  pcLocationLabel->setText("L&ocation:");
  pcLocationLabel->setMinimumSize(pcLocationLabel->sizeHint());

  QWidget* pcLocationLine = new QWidget(this, "locationEdit");

  m_pcLocation = new QLineEdit(pcLocationLine, "location");
  m_pcLocation->setMinimumSize(m_pcLocation->sizeHint());
  pcLocationLabel->setBuddy(m_pcLocation);
  connect(m_pcLocation, SIGNAL(textChanged(const char*)),
          SLOT(locationChanged(const char*)));

  m_pcEditLocation = new QPushButton(pcLocationLine, "editLocation");
  m_pcEditLocation->setText("Edit");
  m_pcEditLocation->setMinimumSize(m_pcEditLocation->sizeHint());
  connect(m_pcEditLocation, SIGNAL(clicked()),
          SLOT(editLocation()));

  QLabel* pcPlanLabel = new QLabel(this, "planText");
  pcPlanLabel->setText("&Plan type:");
  pcPlanLabel->setMinimumSize(pcPlanLabel->sizeHint());

  m_pcPlanSelector = new QComboBox(true, this, "planSelector");
  m_pcPlanSelector->insertItem("Single level");
  m_pcPlanSelector->insertItem("Multi level");
  pcPlanLabel->setBuddy(m_pcPlanSelector);

  QLabel* pcBottomTimeLabel = new QLabel(this, "bottomTimeText");
  pcBottomTimeLabel->setText("&Bottom time:");
  pcBottomTimeLabel->setMinimumSize(pcBottomTimeLabel->sizeHint());

  m_pcBottomTime = new KTimeEdit(this, "bottomTime");
  m_pcBottomTime->setMinimumSize(m_pcBottomTime->sizeHint());
  pcBottomTimeLabel->setBuddy(m_pcBottomTime);
  connect(m_pcBottomTime, SIGNAL(timeChanged(QTime)),
          SLOT(bottomTimeChanged(QTime)));

  QLabel* pcMaxDepthLabel = new QLabel(this, "maxDepthText");
  pcMaxDepthLabel->setText("&Max depth:");
  pcMaxDepthLabel->setMinimumSize(pcMaxDepthLabel->sizeHint());

  m_pcMaxDepth = new QLineEdit(this, "maxDepth");
  m_pcMaxDepth->setMinimumSize(m_pcMaxDepth->sizeHint());
  pcMaxDepthLabel->setBuddy(m_pcMaxDepth);
  connect(m_pcMaxDepth, SIGNAL(textChanged(const char*)),
          SLOT(maxDepthChanged(const char*)));

  QLabel* pcBuddyLabel = new QLabel(this, "buddyText");
  pcBuddyLabel->setText("B&uddy:");
  pcBuddyLabel->setMinimumSize(pcBuddyLabel->sizeHint());

  m_pcBuddy = new QLineEdit(this, "buddy");
  m_pcBuddy->setMinimumSize(m_pcBuddy->sizeHint());
  pcBuddyLabel->setBuddy(m_pcBuddy);
  connect(m_pcBuddy, SIGNAL(textChanged(const char*)),
          SLOT(buddyChanged(const char*)));

  QLabel* pcDiveTypeLabel = new QLabel(this, "diveTypeText");
  pcDiveTypeLabel->setText("Dive t&ype:");
  pcDiveTypeLabel->setMinimumSize(pcDiveTypeLabel->sizeHint());

  m_pcDiveTypeSelector = new QComboBox(true, this, "diveTypeSelector");
  m_pcDiveTypeSelector->insertItem("Nature");
  m_pcDiveTypeSelector->insertItem("Deep");
  m_pcDiveTypeSelector->insertItem("Wreck");
  m_pcDiveTypeSelector->insertItem("Night");
  m_pcDiveTypeSelector->insertItem("Cave");
  m_pcDiveTypeSelector->insertItem("Ice");
  m_pcDiveTypeSelector->insertItem("Current");
  m_pcDiveTypeSelector->insertItem("Navigation");
  m_pcDiveTypeSelector->insertItem("Rescue");
  m_pcDiveTypeSelector->insertItem("Course");
  m_pcDiveTypeSelector->setMinimumSize(m_pcDiveTypeSelector->sizeHint());
  connect(m_pcDiveTypeSelector, SIGNAL(activated(const char*)),
          SLOT(diveTypeChanged(const char*)));
  pcDiveTypeLabel->setBuddy(m_pcDiveTypeSelector);

  QLabel* pcWaterTempLabel = new QLabel(this, "waterTempText");
  pcWaterTempLabel->setText("Min &water temp:");
  pcWaterTempLabel->setMinimumSize(pcWaterTempLabel->sizeHint());

  m_pcWaterTemp = new KIntegerEdit(this, "waterTemperature");
  m_pcWaterTemp->setMinimumSize(m_pcWaterTemp->sizeHint());
  pcWaterTempLabel->setBuddy(m_pcWaterTemp);
  connect(m_pcWaterTemp, SIGNAL(integerChanged(int)),
          SLOT(waterTemperatureChanged(int)));

  QLabel* pcDescriptionLabel = new QLabel(this, "descriptionText");
  pcDescriptionLabel->setText("Dive des&cription:");
  pcDescriptionLabel->setMinimumSize(pcDescriptionLabel->sizeHint());

  m_pcDescription = new QMultiLineEdit(this, "description");
  connect(m_pcDescription,  SIGNAL(textChanged()),
          SLOT(diveDescriptionChanged()));
  pcDescriptionLabel->setBuddy(m_pcDescription);

  m_pcPreviousLog = new QPushButton(this, "previousLog");
  m_pcPreviousLog->setText("Previous log");
  connect(m_pcPreviousLog, SIGNAL(clicked()),
          SLOT(gotoPreviousLog()));

  m_pcNextLog = new QPushButton(this, "nextLog");
  m_pcNextLog->setText("Next log");
  connect(m_pcNextLog, SIGNAL(clicked()),
          SLOT(gotoNextLog()));

  QSize cNavSize(m_pcPreviousLog->sizeHint());
  if ( m_pcNextLog->sizeHint().width() > cNavSize.width() )
    cNavSize.setWidth(m_pcNextLog->sizeHint().width());
  m_pcPreviousLog->setMinimumSize(cNavSize);
  m_pcNextLog->setMinimumSize(cNavSize);


  //
  // Geometry management using layout engines
  //

  QVBoxLayout* pcDVTopLayout = new QVBoxLayout(this, 5);
  QGridLayout* pcUpperLayout = new QGridLayout(7, 4);
  QGridLayout* pcNavigatorLayout = new QGridLayout(1, 3);
  QHBoxLayout* pcLocationLayout = new QHBoxLayout(pcLocationLine);
  pcDVTopLayout->addLayout(pcUpperLayout);
  pcUpperLayout->setColStretch(1, 1);
  pcUpperLayout->setColStretch(3, 1);
  pcUpperLayout->addWidget(pcDiveNumLabel,       0, 0);
  pcUpperLayout->addWidget(m_pcDiveNumber,       0, 1);
  pcUpperLayout->addWidget(pcDiveDateLabel,      1, 0);
  pcUpperLayout->addWidget(m_pcDiveDate,         1, 1);
  pcUpperLayout->addWidget(pcDiveStartLabel,     2, 0);
  pcUpperLayout->addWidget(m_pcDiveStart,        2, 1);
  pcUpperLayout->addWidget(pcDiveTimeLabel,      3, 0);
  pcUpperLayout->addWidget(m_pcDiveTime,         3, 1);
  pcUpperLayout->addWidget(pcGasLabel,           4, 0);
  pcUpperLayout->addWidget(m_pcGasType,          4, 1);
  pcUpperLayout->addWidget(pcAirTempLabel,       5, 0);
  pcUpperLayout->addWidget(m_pcAirTemp,          5, 1);
  pcUpperLayout->addWidget(pcPlanLabel,          0, 2);
  pcUpperLayout->addWidget(m_pcPlanSelector,     0, 3);
  pcUpperLayout->addWidget(pcBottomTimeLabel,    1, 2);
  pcUpperLayout->addWidget(m_pcBottomTime,       1, 3);
  pcUpperLayout->addWidget(pcMaxDepthLabel,      2, 2);
  pcUpperLayout->addWidget(m_pcMaxDepth,         2, 3);
  pcUpperLayout->addWidget(pcBuddyLabel,         3, 2);
  pcUpperLayout->addWidget(m_pcBuddy,            3, 3);
  pcUpperLayout->addWidget(pcDiveTypeLabel,      4, 2);
  pcUpperLayout->addWidget(m_pcDiveTypeSelector, 4, 3);
  pcUpperLayout->addWidget(pcWaterTempLabel,     5, 2);
  pcUpperLayout->addWidget(m_pcWaterTemp,        5, 3);
  pcUpperLayout->addWidget(pcLocationLabel,      6, 0);
  pcLocationLayout->addWidget(m_pcLocation, 1);
  pcLocationLayout->addWidget(m_pcEditLocation);
  pcUpperLayout->addMultiCellWidget(pcLocationLine, 6, 6, 1, 3);
  pcDVTopLayout->addWidget(pcDescriptionLabel);
  pcDVTopLayout->addWidget(m_pcDescription, 10);
  pcDVTopLayout->addLayout(pcNavigatorLayout);
  pcNavigatorLayout->addWidget(m_pcPreviousLog, 0, 0);
  pcNavigatorLayout->setColStretch(1, 1);
  pcNavigatorLayout->addWidget(m_pcNextLog,     0, 2);

  pcDVTopLayout->activate();

  viewLog(0);
}


//*****************************************************************************
/*!
  Destroy the log view. All resources will be freed.

  Notice that this class is not the owner of the log-list, thus it will
  not be freed here.

  \author André Johansen.
*/
//*****************************************************************************

LogView::~LogView()
{
}


//*****************************************************************************
/*!
  Create a new log entry. The log view will be displayed.

  The signal newLog() will be emitted.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::newLog()
{
  assert(m_pcDiveLogList);

  try {
    DiveLog* pcLog = new DiveLog();
    DiveLog* pcLast = m_pcDiveLogList->last();
    int nNumber = ( pcLast ? pcLast->logNumber() + 1 : 1 );
    pcLog->setLogNumber(nNumber);
    m_pcDiveLogList->append(pcLog);
    viewLog(pcLog);
    emit newLog(pcLog);
  }
  catch ( std::bad_alloc ) {
    QMessageBox::warning(qApp->mainWidget(), "[ScubaLog] New dive log",
                         "Out of memory when creating a new dive log!");
  }
}


//*****************************************************************************
/*!
  Show the log \a pcLog. If a null-pointer is passed, the view is cleared.

  Notice that this function will not make the view visible if it is not,
  that should be done after this function is called.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::viewLog(DiveLog* pcLog)
{
  m_pcCurrentLog = pcLog;

  if ( pcLog ) {
    QString cDepth;
    cDepth.sprintf("%.2f", pcLog->maxDepth());
    m_pcDiveNumber->setValue(pcLog->logNumber());
    m_pcDiveNumber->setEnabled(true);
    m_pcDiveDate->setDate(pcLog->diveDate());
    m_pcDiveDate->setEnabled(true);
    m_pcDiveStart->setTime(pcLog->diveStart());
    m_pcDiveStart->setEnabled(true);
    m_pcDiveTime->setTime(pcLog->diveTime());
    m_pcDiveTime->setEnabled(true);
    m_pcGasType->setText(pcLog->gasType());
    m_pcGasType->setEnabled(true);
    m_pcAirTemp->setValue(pcLog->airTemperature());
    m_pcAirTemp->setEnabled(true);
    m_pcLocation->setText(pcLog->diveLocation());
    m_pcLocation->setEnabled(true);
    m_pcEditLocation->setEnabled(true);
    m_pcPlanSelector->setCurrentItem(pcLog->planType());
    m_pcPlanSelector->setEnabled(true);
    m_pcBottomTime->setTime(pcLog->bottomTime());
    m_pcBottomTime->setEnabled(true);
    m_pcMaxDepth->setText(cDepth);
    m_pcMaxDepth->setEnabled(true);
    m_pcBuddy->setText(pcLog->buddyName());
    m_pcBuddy->setEnabled(true);
    QString cDiveType(pcLog->diveType());
    bool bDiveTypeFound = false;
    for ( int iDiveType = 0; iDiveType < m_pcDiveTypeSelector->count();
          ++iDiveType ) {
      if ( cDiveType == m_pcDiveTypeSelector->text(iDiveType) ) {
        m_pcDiveTypeSelector->setCurrentItem(iDiveType);
        bDiveTypeFound = true;
        break;
      }
    }
    if ( false == bDiveTypeFound ) {
      m_pcDiveTypeSelector->insertItem(cDiveType);
      m_pcDiveTypeSelector->setCurrentItem(m_pcDiveTypeSelector->count()-1);
    }
    m_pcDiveTypeSelector->setEnabled(true);
    m_pcWaterTemp->setValue(pcLog->waterTemperature());
    m_pcWaterTemp->setEnabled(true);
    m_pcDescription->setText(pcLog->diveDescription());
    m_pcDescription->setEnabled(true);

    // Check if a previous and next log exists
    bool isPreviousAvailable = false;
    bool isNextAvailable = false;
    if ( m_pcDiveLogList ) {
      QListIterator<DiveLog> iLog(*m_pcDiveLogList);
      for ( ; iLog.current(); ++iLog ) {
        DiveLog* pcCurrent = iLog.current();
        if ( pcCurrent == m_pcCurrentLog ) {
          QListIterator<DiveLog> iPrevLog = iLog;
          QListIterator<DiveLog> iNextLog = iLog;
          --iPrevLog;
          ++iNextLog;
          if ( iPrevLog.current() )
            isPreviousAvailable = true;
          if ( iNextLog.current() )
            isNextAvailable = true;
          break;
        }
      }
    }
    m_pcPreviousLog->setEnabled(isPreviousAvailable);
    m_pcNextLog->setEnabled(isNextAvailable);
  }
  else {
    QDate cDate(QDate::currentDate());
    QTime cTime;
    m_pcDiveNumber->setValue(1);
    m_pcDiveNumber->setEnabled(false);
    m_pcDiveDate->setDate(cDate);
    m_pcDiveDate->setEnabled(false);
    m_pcDiveStart->setTime(cTime);
    m_pcDiveStart->setEnabled(false);
    m_pcDiveTime->setTime(cTime);
    m_pcDiveTime->setEnabled(false);
    m_pcGasType->setText("");
    m_pcGasType->setEnabled(false);
    m_pcAirTemp->setValue(30);
    m_pcAirTemp->setEnabled(false);
    m_pcLocation->setText("");
    m_pcLocation->setEnabled(false);
    m_pcEditLocation->setEnabled(false);
    m_pcPlanSelector->setCurrentItem(DiveLog::e_SingleLevel);
    m_pcPlanSelector->setEnabled(false);
    m_pcBottomTime->setTime(cTime);
    m_pcBottomTime->setEnabled(false);
    m_pcMaxDepth->setText("");
    m_pcMaxDepth->setEnabled(false);
    m_pcBuddy->setText("");
    m_pcBuddy->setEnabled(false);
    m_pcDiveTypeSelector->setCurrentItem(0);
    m_pcDiveTypeSelector->setEnabled(false);
    m_pcWaterTemp->setValue(20);
    m_pcWaterTemp->setEnabled(false);
    m_pcDescription->setText("");
    m_pcDescription->setEnabled(false);
    m_pcPreviousLog->setEnabled(false);
    m_pcNextLog->setEnabled(false);
  }
}


//*****************************************************************************
/*!
  The dive number changed to \a nNumber. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::diveNumberChanged(int nNumber)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setLogNumber(nNumber);
}


//*****************************************************************************
/*!
  The dive date changed to \a cDate. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::diveDateChanged(QDate cDate)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setDiveDate(cDate);
}


//*****************************************************************************
/*!
  The dive start changed to \a cStart. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::diveStartChanged(QTime cStart)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setDiveStart(cStart);
}


//*****************************************************************************
/*!
  The dive time changed to \a cTime. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::diveTimeChanged(QTime cTime)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setDiveTime(cTime);
}


//*****************************************************************************
/*!
  The gas type changed to \a pzGasType. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::gasTypeChanged(const char* pzGasType)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setGasType(QString(pzGasType));
}


//*****************************************************************************
/*!
  The bottom time changed to \a cTime. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::bottomTimeChanged(QTime cTime)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setBottomTime(cTime);
}


//*****************************************************************************
/*!
  The air temperature changed to \a nTemp. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::airTemperatureChanged(int nTemp)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setAirTemperature((float)nTemp);
}


//*****************************************************************************
/*!
  The water temperature changed to \a nTemp. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::waterTemperatureChanged(int nTemp)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setWaterTemperature((float)nTemp);
}


//*****************************************************************************
/*!
  The location changed to \a pzLocation. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::locationChanged(const char* pzLocation)
{
  assert(pzLocation);
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setDiveLocation(QString(pzLocation));
}


//*****************************************************************************
/*!
  The maximum depth changed to \a pzDepth. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::maxDepthChanged(const char* pzDepth)
{
  assert(pzDepth);
  float vDepth = (float)atof(pzDepth);
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setMaxDepth(vDepth);
}


//*****************************************************************************
/*!
  The buddy changed to \a pzBuddy. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::buddyChanged(const char* pzBuddy)
{
  assert(pzBuddy);
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setBuddyName(QString(pzBuddy));
}


//*****************************************************************************
/*!
  The dive type changed to \a pzDiveType. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::diveTypeChanged(const char* pzDiveType)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setDiveType(QString(pzDiveType));
}


//*****************************************************************************
/*!
  The dive description changed. Update the log.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::diveDescriptionChanged()
{
  if ( m_pcCurrentLog ) {
    QString cDescription(m_pcDescription->text());
    m_pcCurrentLog->setDiveDescription(cDescription);
  }
}


//*****************************************************************************
/*!
  Goto the previous log in the log book.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::gotoPreviousLog()
{
  assert(m_pcDiveLogList);

  DiveLog* pcPrevious = 0;
  DiveLog* pcCurrent  = 0;
  QListIterator<DiveLog> iLog(*m_pcDiveLogList);
  for ( ; iLog.current(); ++iLog ) {
    pcCurrent = iLog.current();
    if ( pcCurrent == m_pcCurrentLog )
      break;
    pcPrevious = pcCurrent;
  }

  if ( pcPrevious )
    viewLog(pcPrevious);
}


//*****************************************************************************
/*!
  Goto the next log in the log book.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::gotoNextLog()
{
  assert(m_pcDiveLogList);

  DiveLog* pcCurrent  = 0;
  DiveLog* pcNext     = 0;
  QListIterator<DiveLog> iLog(*m_pcDiveLogList);
  for ( ; iLog.current(); ++iLog ) {
    pcCurrent = iLog.current();
    if ( pcCurrent == m_pcCurrentLog ) {
      QListIterator<DiveLog> iNextLog = iLog;
      ++iNextLog;
      pcNext = iNextLog.current();
      break;
    }
  }

  if ( pcNext )
    viewLog(pcNext);
}


//*****************************************************************************
/*!
  The log \a pcLog is about to be deleted, make sure it is not used anymore.

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::deletingLog(const DiveLog* pcLog)
{
  if ( m_pcCurrentLog && pcLog == m_pcCurrentLog ) {
    DiveLog* pcNewLog = 0;
    if ( m_pcDiveLogList ) {
      QListIterator<DiveLog> iLog(*m_pcDiveLogList);
      for ( ; iLog.current(); ++iLog ) {
        DiveLog* pcCurrentLog = iLog.current();
        if ( pcCurrentLog == pcLog ) {
          QListIterator<DiveLog> iNewLog = iLog;
          ++iNewLog;
          if ( iNewLog.current() )
            pcNewLog = iNewLog.current();
          else {
            iNewLog = iLog;
            --iNewLog;
            pcNewLog = iNewLog.current();
          }
          break;
        }
      }
    }
    viewLog(pcNewLog);
  }
}


//*****************************************************************************
/*!
  Edit the current location.

  This function will emit the signal editLocation().

  \author André Johansen.
*/
//*****************************************************************************

void
LogView::editLocation()
{
  assert(m_pcCurrentLog);

  if ( false == m_pcCurrentLog->diveLocation().isNull() )
    emit editLocation(m_pcCurrentLog->diveLocation());
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
