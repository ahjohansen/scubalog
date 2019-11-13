//*****************************************************************************
/*!
  \file personalinfoview.cpp
  \brief This file contains the implementation of the PersonalInfoView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include "personalinfoview.h"
#include "logbook.h"
#include "divelist.h"
#include <klocale.h>
#include <kapplication.h>
#include <qlayout.h>
#include <QTextEdit>
#include <qlineedit.h>
#include <qlabel.h>
#include <QGridLayout>
#include <QVBoxLayout>


//*****************************************************************************
/*!
  Create the personal info view class with \a pcParent as parent widget.
*/
//*****************************************************************************

PersonalInfoView::PersonalInfoView(QWidget* pcParent)
  : QWidget(pcParent),
    m_pcName(0),
    m_pcEmailAddress(0),
    m_pcWwwUrl(0),
    m_pcLoggedDiveTime(0),
    m_pcComments(0),
    m_pcLogBook(0)
{
  //
  // Create GUI elements
  //

  QLabel* pcNameLabel = new QLabel(this);
  pcNameLabel->setText(i18n("&Name:"));
  pcNameLabel->setMinimumSize(pcNameLabel->sizeHint());

  m_pcName = new QLineEdit(this);
  m_pcName->setMinimumSize(m_pcName->sizeHint());
  connect(m_pcName, SIGNAL(textChanged(const QString&)),
          SLOT(diverNameChanged(const QString&)));
  pcNameLabel->setBuddy(m_pcName);

  QLabel* pcEmailLabel = new QLabel(this);
  pcEmailLabel->setText(i18n("&Email address:"));
  pcEmailLabel->setMinimumSize(pcEmailLabel->sizeHint());

  m_pcEmailAddress = new QLineEdit(this);
  m_pcEmailAddress->setMinimumSize(m_pcEmailAddress->sizeHint());
  connect(m_pcEmailAddress, SIGNAL(textChanged(const QString&)),
          SLOT(emailAddressChanged(const QString&)));
  pcEmailLabel->setBuddy(m_pcEmailAddress);

  QLabel* pcWwwUrlLabel = new QLabel(this);
  pcWwwUrlLabel->setText(i18n("&WWW URL:"));
  pcWwwUrlLabel->setMinimumSize(pcWwwUrlLabel->sizeHint());

  m_pcWwwUrl = new QLineEdit(this);
  m_pcWwwUrl->setMinimumSize(m_pcWwwUrl->sizeHint());
  connect(m_pcWwwUrl, SIGNAL(textChanged(const QString&)),
          SLOT(wwwUrlChanged(const QString&)));
  pcWwwUrlLabel->setBuddy(m_pcWwwUrl);

  m_pcLoggedDiveTime = new QLabel(this);
  m_pcLoggedDiveTime->setText(i18n("Total logged dive time: 000h 00min"));
  m_pcLoggedDiveTime->setMinimumSize(m_pcLoggedDiveTime->sizeHint());

  QLabel* pcCommentsLabel = new QLabel(this);
  pcCommentsLabel->setText(i18n("&Comments:"));
  pcCommentsLabel->setMinimumSize(pcCommentsLabel->sizeHint());

  m_pcComments = new QTextEdit(this);
  connect(m_pcComments, SIGNAL(textChanged()), SLOT(commentsChanged()));
  pcCommentsLabel->setBuddy(m_pcComments);


  setLogBook(0);


  //
  // Geometry management using layout engines
  //

  QVBoxLayout* pcDVTopLayout = new QVBoxLayout(this);
  QGridLayout* pcUpperLayout = new QGridLayout();
  pcDVTopLayout->addLayout(pcUpperLayout);
  pcUpperLayout->setColumnStretch(1, 1);
  pcUpperLayout->setColumnStretch(3, 1);
  pcUpperLayout->addWidget(pcNameLabel,       0, 0);
  pcUpperLayout->addWidget(m_pcName,          0, 1);
  pcUpperLayout->addWidget(pcEmailLabel,      1, 0);
  pcUpperLayout->addWidget(m_pcEmailAddress,  1, 1);
  pcUpperLayout->addWidget(pcWwwUrlLabel,     1, 2);
  pcUpperLayout->addWidget(m_pcWwwUrl,        1, 3);
  pcUpperLayout->addWidget(m_pcLoggedDiveTime, 2, 0, 1, 2);
  pcDVTopLayout->addWidget(pcCommentsLabel);
  pcDVTopLayout->addWidget(m_pcComments, 10);
  pcDVTopLayout->activate();
}


//*****************************************************************************
/*!
  Destroy the view.
*/
//*****************************************************************************

PersonalInfoView::~PersonalInfoView()
{
}


//*****************************************************************************
/*!
  Set the log book to \a pcLogBook. The view will be updated.
  If a null-pointer is passed, the view will be cleared.
*/
//*****************************************************************************

void
PersonalInfoView::setLogBook(LogBook* pcLogBook)
{
  m_pcLogBook = pcLogBook;

  if ( pcLogBook ) {
    m_pcName->setText(pcLogBook->diverName());
    m_pcName->setEnabled(true);
    m_pcEmailAddress->setText(pcLogBook->emailAddress());
    m_pcEmailAddress->setEnabled(true);
    m_pcWwwUrl->setText(pcLogBook->wwwUrl());
    m_pcWwwUrl->setEnabled(true);
    updateLoggedDiveTime();
    m_pcComments->setText(pcLogBook->comments());
    m_pcComments->setEnabled(true);
  }
  else {
    m_pcName->setText("");
    m_pcName->setEnabled(false);
    m_pcEmailAddress->setText("");
    m_pcEmailAddress->setEnabled(false);
    m_pcWwwUrl->setText("");
    m_pcWwwUrl->setEnabled(false);
    m_pcComments->setText("");
    m_pcComments->setEnabled(false);
  }
}


//*****************************************************************************
/*!
  The diver name was changed to \a cName.
  Update the current log book, if any.
*/
//*****************************************************************************

void
PersonalInfoView::diverNameChanged(const QString& cName)
{
  if ( m_pcLogBook )
    m_pcLogBook->setDiverName(cName);
}


//*****************************************************************************
/*!
  The email address was changed to \a cEmailAddress.
  Update the current log book, if any.
*/
//*****************************************************************************

void
PersonalInfoView::emailAddressChanged(const QString& cEmailAddress)
{
  if ( m_pcLogBook )
    m_pcLogBook->setEmailAddress(cEmailAddress);
}


//*****************************************************************************
/*!
  The www url was changed to \a cWwwUrl.
  Update the current log book, if any.
*/
//*****************************************************************************

void
PersonalInfoView::wwwUrlChanged(const QString& cWwwUrl)
{
  if ( m_pcLogBook )
    m_pcLogBook->setWwwUrl(cWwwUrl);
}


//*****************************************************************************
/*!
  Update the logged dive time.

  This is done by going through all the logged dives in the current logbook,
  summing the dive time.
*/
//*****************************************************************************

void
PersonalInfoView::updateLoggedDiveTime()
{
  QString cLoggedTimeText(i18n("0h 0min"));
  if ( m_pcLogBook ) {
    unsigned int nNumMins = 0;
    DiveList& cDiveList = m_pcLogBook->diveList();
    QListIterator<DiveLog*> iLog(cDiveList);
    while ( iLog.hasNext() ) {
      DiveLog* pcLog = iLog.next();
      nNumMins += pcLog->diveTime().hour() * 60 + pcLog->diveTime().minute();
    }
    cLoggedTimeText =
      QString(i18n("Total logged dive time: %1h %2min"))
      .arg(nNumMins/60)
      .arg(nNumMins%60);
  }
  m_pcLoggedDiveTime->setText(cLoggedTimeText);
}


//*****************************************************************************
/*!
  The comments was changed.
  Update the current log book, if any.
*/
//*****************************************************************************

void
PersonalInfoView::commentsChanged()
{
  if ( m_pcLogBook )
    m_pcLogBook->setComments(m_pcComments->document()->toPlainText());
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
