//*****************************************************************************
/*!
  \file logview.cpp
  \brief This file contains the implementation of LogView.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "logview.h"
#include "logbook.h"
#include "divelist.h"
#include "logbook.h"
#include "ktimeedit.h"
#include "kdateedit.h"
#include "kintegeredit.h"
#include "debug.h"

#include <kapplication.h>
#include <klocale.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qtextdocument.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <new>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

//*****************************************************************************
/*!
  Initialise the log view with \a pcParent as the parent widget
  and \a pzName as the widget name.
*/
//*****************************************************************************

LogView::LogView(QWidget* pcParent)
  : QWidget(pcParent),
    m_pcLogBook(0),
    m_pcCurrentLog(0),
    m_pcDiveNumber(0),
    m_pcDiveDate(0),
    m_pcDiveStart(0),
    m_pcDiveTime(0),
    m_pcGasType(0),
    m_pcAirTemp(0),
    m_pcWaterTemp(0),
    m_pcLocation(0),
    m_pcEditLocation(0),
    m_pcPlanSelector(0),
    m_pcBottomTime(0),
    m_pcMaxDepth(0),
    m_pcBuddy(0),
    m_pcDiveTypeSelector(0),
    m_pcDescription(0),
    m_pcPreviousLog(0),
    m_pcNextLog(0)
{
  QLabel* pcDiveNumLabel = new QLabel(this);
  pcDiveNumLabel->setText(i18n("Dive &number:"));
  pcDiveNumLabel->setMinimumSize(pcDiveNumLabel->sizeHint());

  m_pcDiveNumber = new KIntegerEdit(1, 1, INT_MAX, this);
  m_pcDiveNumber->setText("1");
  m_pcDiveNumber->setMinimumSize(m_pcDiveNumber->sizeHint());
  pcDiveNumLabel->setBuddy(m_pcDiveNumber);
  connect(m_pcDiveNumber, SIGNAL(integerChanged(int)),
          SLOT(diveNumberChanged(int)));

  QLabel* pcDiveDateLabel = new QLabel(this);
  pcDiveDateLabel->setText(i18n("Dive &date:"));
  pcDiveDateLabel->setMinimumSize(pcDiveDateLabel->sizeHint());

  m_pcDiveDate = new KDateEdit(this);
  m_pcDiveDate->setMinimumSize(m_pcDiveDate->sizeHint());
  pcDiveDateLabel->setBuddy(m_pcDiveDate);
  connect(m_pcDiveDate, SIGNAL(dateChanged(QDate)),
          SLOT(diveDateChanged(QDate)));

  QLabel* pcDiveStartLabel = new QLabel(this);
  pcDiveStartLabel->setText(i18n("Dive &start:"));
  pcDiveStartLabel->setMinimumSize(pcDiveStartLabel->sizeHint());

  m_pcDiveStart = new KTimeEdit(this);
  m_pcDiveStart->setMinimumSize(m_pcDiveStart->sizeHint());
  pcDiveStartLabel->setBuddy(m_pcDiveStart);
  connect(m_pcDiveStart, SIGNAL(timeChanged(QTime)),
          SLOT(diveStartChanged(QTime)));

  QLabel* pcDiveTimeLabel = new QLabel(this);
  pcDiveTimeLabel->setText(i18n("Dive &time:"));
  pcDiveTimeLabel->setMinimumSize(pcDiveTimeLabel->sizeHint());

  m_pcDiveTime = new KTimeEdit(this);
  m_pcDiveTime->setMinimumSize(m_pcDiveTime->sizeHint());
  pcDiveTimeLabel->setBuddy(m_pcDiveTime);
  connect(m_pcDiveTime, SIGNAL(timeChanged(QTime)),
          SLOT(diveTimeChanged(QTime)));

  QLabel* pcGasLabel = new QLabel(this);
  pcGasLabel->setText(i18n("&Gas:"));
  pcGasLabel->setMinimumSize(pcGasLabel->sizeHint());

  m_pcGasType = new QLineEdit(this);
  m_pcGasType->setMinimumSize(m_pcGasType->sizeHint());
  pcGasLabel->setBuddy(m_pcGasType);
  connect(m_pcGasType, SIGNAL(textChanged(const QString&)),
          SLOT(gasTypeChanged(const QString&)));

  QLabel* pcAirTempLabel = new QLabel(this);
  pcAirTempLabel->setText(i18n("&Air temperature:"));
  pcAirTempLabel->setMinimumSize(pcAirTempLabel->sizeHint());

  m_pcAirTemp = new KIntegerEdit(-100, 20, 100, this);
  m_pcAirTemp->setMinimumSize(m_pcAirTemp->sizeHint());
  pcAirTempLabel->setBuddy(m_pcAirTemp);
  connect(m_pcAirTemp, SIGNAL(integerChanged(int)),
          SLOT(airTemperatureChanged(int)));

  QLabel* pcPlanLabel = new QLabel(this);
  pcPlanLabel->setText(i18n("&Plan type:"));
  pcPlanLabel->setMinimumSize(pcPlanLabel->sizeHint());

  m_pcPlanSelector = new QComboBox(this);
  m_pcPlanSelector->setEditable(true);
  m_pcPlanSelector->insertItem(0, i18n("Single level"));
  m_pcPlanSelector->insertItem(1, i18n("Multi level"));
  pcPlanLabel->setBuddy(m_pcPlanSelector);

  QLabel* pcBottomTimeLabel = new QLabel(this);
  pcBottomTimeLabel->setText(i18n("&Bottom time:"));
  pcBottomTimeLabel->setMinimumSize(pcBottomTimeLabel->sizeHint());

  m_pcBottomTime = new KTimeEdit(this);
  m_pcBottomTime->setMinimumSize(m_pcBottomTime->sizeHint());
  pcBottomTimeLabel->setBuddy(m_pcBottomTime);
  connect(m_pcBottomTime, SIGNAL(timeChanged(QTime)),
          SLOT(bottomTimeChanged(QTime)));

  QLabel* pcMaxDepthLabel = new QLabel(this);
  pcMaxDepthLabel->setText(i18n("&Max depth:"));
  pcMaxDepthLabel->setMinimumSize(pcMaxDepthLabel->sizeHint());

  m_pcMaxDepth = new QLineEdit(this);
  m_pcMaxDepth->setMinimumSize(m_pcMaxDepth->sizeHint());
  pcMaxDepthLabel->setBuddy(m_pcMaxDepth);
  connect(m_pcMaxDepth, SIGNAL(textChanged(const QString&)),
          SLOT(maxDepthChanged(const QString&)));

  QLabel* pcBuddyLabel = new QLabel(this);
  pcBuddyLabel->setText(i18n("B&uddy:"));
  pcBuddyLabel->setMinimumSize(pcBuddyLabel->sizeHint());

  m_pcBuddy = new QLineEdit(this);
  m_pcBuddy->setMinimumSize(m_pcBuddy->sizeHint());
  pcBuddyLabel->setBuddy(m_pcBuddy);
  connect(m_pcBuddy, SIGNAL(textChanged(const QString&)),
          SLOT(buddyChanged(const QString&)));

  QLabel* pcDiveTypeLabel = new QLabel(this);
  pcDiveTypeLabel->setText(i18n("Dive t&ype:"));
  pcDiveTypeLabel->setMinimumSize(pcDiveTypeLabel->sizeHint());

  m_pcDiveTypeSelector = new QComboBox(this);
  m_pcDiveTypeSelector->setEditable(true);
  m_pcDiveTypeSelector->insertItem(0, i18n("Nature"));
  m_pcDiveTypeSelector->insertItem(1, i18n("Deep"));
  m_pcDiveTypeSelector->insertItem(2, i18n("Wreck"));
  m_pcDiveTypeSelector->insertItem(3, i18n("Night"));
  m_pcDiveTypeSelector->insertItem(4, i18n("Cave"));
  m_pcDiveTypeSelector->insertItem(5, i18n("Ice"));
  m_pcDiveTypeSelector->insertItem(6, i18n("Current"));
  m_pcDiveTypeSelector->insertItem(7, i18n("Navigation"));
  m_pcDiveTypeSelector->insertItem(8, i18n("Rescue"));
  m_pcDiveTypeSelector->insertItem(9, i18n("Course"));
  m_pcDiveTypeSelector->setMinimumSize(m_pcDiveTypeSelector->sizeHint());
  connect(m_pcDiveTypeSelector, SIGNAL(activated(const QString&)),
          SLOT(diveTypeChanged(const QString&)));
  pcDiveTypeLabel->setBuddy(m_pcDiveTypeSelector);

  QLabel* pcWaterTempLabel = new QLabel(this);
  pcWaterTempLabel->setText(i18n("Min &water temp:"));
  pcWaterTempLabel->setMinimumSize(pcWaterTempLabel->sizeHint());

  QLabel* pcLocationLabel = new QLabel(this);
  pcLocationLabel->setText(i18n("L&ocation:"));
  pcLocationLabel->setMinimumSize(pcLocationLabel->sizeHint());

  QWidget* pcLocationLine = new QWidget(this);

  m_pcLocation = new QLineEdit(pcLocationLine);
  m_pcLocation->setMinimumSize(m_pcLocation->sizeHint());
  pcLocationLabel->setBuddy(m_pcLocation);
  connect(m_pcLocation, SIGNAL(textChanged(const QString&)),
          SLOT(locationChanged(const QString&)));

  m_pcEditLocation = new QPushButton(pcLocationLine);
  m_pcEditLocation->setText(i18n("Edit"));
  m_pcEditLocation->setMinimumSize(m_pcEditLocation->sizeHint());
  connect(m_pcEditLocation, SIGNAL(clicked()),
          SLOT(editLocation()));

  m_pcWaterTemp = new KIntegerEdit(this);
  m_pcWaterTemp->setMinimumSize(m_pcWaterTemp->sizeHint());
  pcWaterTempLabel->setBuddy(m_pcWaterTemp);
  connect(m_pcWaterTemp, SIGNAL(integerChanged(int)),
          SLOT(waterTemperatureChanged(int)));

  QLabel* pcDescriptionLabel = new QLabel(this);
  pcDescriptionLabel->setText(i18n("Dive des&cription:"));
  pcDescriptionLabel->setMinimumSize(pcDescriptionLabel->sizeHint());

  m_pcDescription = new QTextEdit(this);
  connect(m_pcDescription,  SIGNAL(textChanged()),
          SLOT(diveDescriptionChanged()));
  pcDescriptionLabel->setBuddy(m_pcDescription);

  m_pcPreviousLog = new QPushButton(this);
  m_pcPreviousLog->setText(i18n("Previous log"));
  connect(m_pcPreviousLog, SIGNAL(clicked()),
          SLOT(gotoPreviousLog()));

  m_pcNextLog = new QPushButton(this);
  m_pcNextLog->setText(i18n("Next log"));
  connect(m_pcNextLog, SIGNAL(clicked()),
          SLOT(gotoNextLog()));

  QSize cNavSize(m_pcPreviousLog->sizeHint());
  if ( m_pcNextLog->sizeHint().width() > cNavSize.width() )
    cNavSize.setWidth(m_pcNextLog->sizeHint().width());
  m_pcPreviousLog->setMinimumSize(cNavSize);
  m_pcNextLog->setMinimumSize(cNavSize);


  //
  // Geometry management.
  //

  QVBoxLayout* pcDVTopLayout     = new QVBoxLayout(this);
  QGridLayout* pcUpperLayout     = new QGridLayout();
  QGridLayout* pcNavigatorLayout = new QGridLayout();
  QHBoxLayout* pcLocationLayout  = new QHBoxLayout(pcLocationLine);
  pcDVTopLayout->addLayout(pcUpperLayout);
  pcUpperLayout->setColumnStretch(1, 1);
  pcUpperLayout->setColumnStretch(3, 1);
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
  pcUpperLayout->addWidget(pcLocationLine, 6, 1, 1, 3);
  pcDVTopLayout->addWidget(pcDescriptionLabel);
  pcDVTopLayout->addWidget(m_pcDescription, 10);
  pcDVTopLayout->addLayout(pcNavigatorLayout);
  pcNavigatorLayout->addWidget(m_pcPreviousLog, 0, 0);
  pcNavigatorLayout->setColumnStretch(1, 1);
  pcNavigatorLayout->addWidget(m_pcNextLog,     0, 2);

  pcDVTopLayout->activate();

  viewLog(0);
}


//*****************************************************************************
/*!
  Destroy the log view. All resources will be freed.

  Notice that this class is not the owner of the log-list, thus it will
  not be freed here.
*/
//*****************************************************************************

LogView::~LogView()
{
}


//*****************************************************************************
/*!
  Set the log book to \a pcLogBook.

  This function will update the view. If a null-pointer is passed,
  the view will be cleared and the widgets will be disabled.
*/
//*****************************************************************************

void
LogView::setLogBook(LogBook* pcLogBook)
{
  m_pcLogBook = pcLogBook;
  viewLog(0);
}


//*****************************************************************************
/*!
  Create a new log entry. The log view will be displayed.

  The signal newLog() will be emitted.
*/
//*****************************************************************************

void
LogView::newLog()
{
  assert(m_pcLogBook);

  try {
    DiveList& cDiveList = m_pcLogBook->diveList();
    DiveLog* pcLog = new DiveLog();
    DiveLog* pcLast = cDiveList.last();
    int nNumber = ( pcLast ? pcLast->logNumber() + 1 : 1 );
    pcLog->setLogNumber(nNumber);
    cDiveList.append(pcLog);
    viewLog(pcLog);
    emit newLog(pcLog);
  }
  catch ( std::bad_alloc ) {
    QMessageBox::warning(QApplication::topLevelWidgets().at(0),
                         i18n("[ScubaLog] New dive log"),
                         i18n("Out of memory when creating a new dive log!"));
  }
}


//*****************************************************************************
/*!
  Show the log \a pcLog. If a null-pointer is passed, the view is cleared.

  Notice that this function will not make the view visible if it is not,
  that should be done after this function is called.
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
    m_pcAirTemp->setValue((int)pcLog->airTemperature());
    m_pcAirTemp->setEnabled(true);
    m_pcLocation->setText(pcLog->diveLocation());
    m_pcLocation->setEnabled(true);
    m_pcEditLocation->setEnabled(true);
    m_pcPlanSelector->setCurrentIndex(pcLog->planType());
    m_pcPlanSelector->setEnabled(true);
    m_pcBottomTime->setTime(pcLog->bottomTime());
    m_pcBottomTime->setEnabled(true);
    m_pcMaxDepth->setText(cDepth);
    m_pcMaxDepth->setEnabled(true);
    m_pcBuddy->setText(pcLog->buddyName());
    m_pcBuddy->setEnabled(true);
    QString cDiveType(pcLog->diveType());
    int nDiveTypeIndex = m_pcDiveTypeSelector->findText(cDiveType);
    if ( nDiveTypeIndex != -1 ) {
      m_pcDiveTypeSelector->setCurrentIndex(nDiveTypeIndex);
    }
    else {
      m_pcDiveTypeSelector->insertItem(INT_MAX, cDiveType);
      m_pcDiveTypeSelector->setCurrentIndex(m_pcDiveTypeSelector->count()-1);
    }
    m_pcDiveTypeSelector->setEnabled(true);
    m_pcWaterTemp->setValue((int)pcLog->waterTemperature());
    m_pcWaterTemp->setEnabled(true);
    m_pcDescription->setText(pcLog->diveDescription());
    m_pcDescription->setEnabled(true);

    // Check if a previous and next log exists
    bool isPreviousAvailable = false;
    bool isNextAvailable = false;
    if ( m_pcLogBook ) {
      DiveList& cDiveList = m_pcLogBook->diveList();
      QListIterator<DiveLog*> iLog(cDiveList);
      while ( iLog.hasNext() ) {
        DiveLog* pcCurrent = iLog.next();
        if ( pcCurrent == m_pcCurrentLog ) {
          if ( iLog.hasPrevious() )
            isPreviousAvailable = true;
          if ( iLog.hasNext() )
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
    m_pcPlanSelector->setCurrentIndex(DiveLog::e_SingleLevel);
    m_pcPlanSelector->setEnabled(false);
    m_pcBottomTime->setTime(cTime);
    m_pcBottomTime->setEnabled(false);
    m_pcMaxDepth->setText("");
    m_pcMaxDepth->setEnabled(false);
    m_pcBuddy->setText("");
    m_pcBuddy->setEnabled(false);
    m_pcDiveTypeSelector->setCurrentIndex(0);
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
  The gas type changed to \a cGasType. Update the log.
*/
//*****************************************************************************

void
LogView::gasTypeChanged(const QString& cGasType)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setGasType(cGasType);
}


//*****************************************************************************
/*!
  The bottom time changed to \a cTime. Update the log.
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
  The location changed to \a cLocation. Update the log.
*/
//*****************************************************************************

void
LogView::locationChanged(const QString& cLocation)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setDiveLocation(cLocation);
}


//*****************************************************************************
/*!
  The maximum depth changed to \a cDepth. Update the log.
*/
//*****************************************************************************

void
LogView::maxDepthChanged(const QString& cDepth)
{
  float vDepth = (float)atof(cDepth.toAscii());
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setMaxDepth(vDepth);
}


//*****************************************************************************
/*!
  The buddy changed to \a cBuddy. Update the log.
*/
//*****************************************************************************

void
LogView::buddyChanged(const QString& cBuddy)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setBuddyName(cBuddy);
}


//*****************************************************************************
/*!
  The dive type changed to \a cDiveType. Update the log.
*/
//*****************************************************************************

void
LogView::diveTypeChanged(const QString& cDiveType)
{
  if ( m_pcCurrentLog )
    m_pcCurrentLog->setDiveType(cDiveType);
}


//*****************************************************************************
/*!
  The dive description changed. Update the log.
*/
//*****************************************************************************

void
LogView::diveDescriptionChanged()
{
  if ( m_pcCurrentLog ) {
    QString cDescription(m_pcDescription->document()->toPlainText());
    m_pcCurrentLog->setDiveDescription(cDescription);
  }
}


//*****************************************************************************
/*!
  Goto the previous log in the log book.
*/
//*****************************************************************************

void
LogView::gotoPreviousLog()
{
  assert(m_pcLogBook);

  DiveLog* pcPrevious = 0;
  const DiveList& cDiveList = m_pcLogBook->diveList();
  QListIterator<DiveLog*> iLog(cDiveList);
  while ( iLog.hasNext() ) {
    DiveLog* pcCurrent = iLog.next();
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
*/
//*****************************************************************************

void
LogView::gotoNextLog()
{
  assert(m_pcLogBook);

  DiveLog* pcNext = 0;
  const DiveList& cDiveList = m_pcLogBook->diveList();
  QListIterator<DiveLog*> iLog(cDiveList);
  while ( iLog.hasNext() ) {
    DiveLog* pcCurrent = iLog.next();
    if ( pcCurrent == m_pcCurrentLog ) {
      if ( iLog.hasNext() ) {
        pcNext = iLog.next();
      }
      break;
    }
  }

  if ( pcNext )
    viewLog(pcNext);
}


//*****************************************************************************
/*!
  The log \a pcLog is about to be deleted, make sure it is not used anymore.
*/
//*****************************************************************************

void
LogView::deletingLog(const DiveLog* pcLog)
{
  if ( m_pcCurrentLog && pcLog == m_pcCurrentLog ) {
    DiveLog* pcNewLog = 0;
    if ( m_pcLogBook ) {
      const DiveList& cDiveList = m_pcLogBook->diveList();
      QListIterator<DiveLog*> iLog(cDiveList);
      while ( iLog.hasNext() ) {
        DiveLog* pcCurrentLog = iLog.next();
        if ( pcCurrentLog == pcLog ) {
          if ( iLog.hasNext() )
            pcNewLog = iLog.next();
          else if ( iLog.hasPrevious() ) {
            pcNewLog = iLog.previous();
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
// coding: utf-8
// End:
