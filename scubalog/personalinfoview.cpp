//*****************************************************************************
/*!
  \file ScubaLog/personalinfoview.cpp
  \brief This file contains the implementation of the PersonalInfoView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  $Id$

  \par Copyright:
  André Johansen.
*/
//*****************************************************************************

#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilinedit.h>
#include <qlayout.h>
#include "divelist.h"
#include "logbook.h"
#include "personalinfoview.h"
#include "personalinfoview.moc"


//*****************************************************************************
/*!
  Create the personal info view class with \a pcParent as parent widget
  and \a pzName as widget name.

  \author André Johansen.
*/
//*****************************************************************************

PersonalInfoView::PersonalInfoView(QWidget* pcParent, const char* pzName)
  : QWidget(pcParent, pzName),
    m_pcName(0), m_pcEmailAddress(0), m_pcWwwUrl(0), m_pcComments(0),
    m_pcLogBook(0)
{
  //
  // Create GUI elements
  //

  QLabel* pcNameLabel = new QLabel(this, "diverNameText");
  pcNameLabel->setText("&Name:");
  pcNameLabel->setMinimumSize(pcNameLabel->sizeHint());

  m_pcName = new QLineEdit(this, "diverName");
  m_pcName->setMinimumSize(m_pcName->sizeHint());
  connect(m_pcName, SIGNAL(textChanged(const char*)),
          SLOT(diverNameChanged(const char*)));
  pcNameLabel->setBuddy(m_pcName);

  QLabel* pcEmailLabel = new QLabel(this, "emailText");
  pcEmailLabel->setText("&Email address:");
  pcEmailLabel->setMinimumSize(pcEmailLabel->sizeHint());

  m_pcEmailAddress = new QLineEdit(this, "emailAddress");
  m_pcEmailAddress->setMinimumSize(m_pcEmailAddress->sizeHint());
  connect(m_pcEmailAddress, SIGNAL(textChanged(const char*)),
          SLOT(emailAddressChanged(const char*)));
  pcEmailLabel->setBuddy(m_pcEmailAddress);

  QLabel* pcWwwUrlLabel = new QLabel(this, "wwwUrlText");
  pcWwwUrlLabel->setText("&WWW URL:");
  pcWwwUrlLabel->setMinimumSize(pcWwwUrlLabel->sizeHint());

  m_pcWwwUrl = new QLineEdit(this, "wwwUrl");
  m_pcWwwUrl->setMinimumSize(m_pcWwwUrl->sizeHint());
  connect(m_pcWwwUrl, SIGNAL(textChanged(const char*)),
          SLOT(wwwUrlChanged(const char*)));
  pcWwwUrlLabel->setBuddy(m_pcWwwUrl);

  m_pcLoggedDiveTime = new QLabel(this, "LoggedDiveTime");
  m_pcLoggedDiveTime->setText("Total logged dive time: 000h 00min");
  m_pcLoggedDiveTime->setMinimumSize(m_pcLoggedDiveTime->sizeHint());

  QLabel* pcCommentsLabel = new QLabel(this, "commentsText");
  pcCommentsLabel->setText("&Comments:");
  pcCommentsLabel->setMinimumSize(pcCommentsLabel->sizeHint());

  m_pcComments = new QMultiLineEdit(this, "comments");
  connect(m_pcComments, SIGNAL(textChanged()), SLOT(commentsChanged()));
  pcCommentsLabel->setBuddy(m_pcComments);


  setLogBook(0);


  //
  // Geometry management using layout engines
  //

  QVBoxLayout* pcDVTopLayout = new QVBoxLayout(this, 5);
  QGridLayout* pcUpperLayout = new QGridLayout(3, 4);
  pcDVTopLayout->addLayout(pcUpperLayout);
  pcUpperLayout->setColStretch(1, 1);
  pcUpperLayout->setColStretch(3, 1);
  pcUpperLayout->addWidget(pcNameLabel,       0, 0);
  pcUpperLayout->addWidget(m_pcName,          0, 1);
  pcUpperLayout->addWidget(pcEmailLabel,      1, 0);
  pcUpperLayout->addWidget(m_pcEmailAddress,  1, 1);
  pcUpperLayout->addWidget(pcWwwUrlLabel,     1, 2);
  pcUpperLayout->addWidget(m_pcWwwUrl,        1, 3);
  pcUpperLayout->addMultiCellWidget(m_pcLoggedDiveTime, 2, 2, 0, 1);
  pcDVTopLayout->addWidget(pcCommentsLabel);
  pcDVTopLayout->addWidget(m_pcComments, 10);
  pcDVTopLayout->activate();
}


//*****************************************************************************
/*!
  Destroy the view.

  \author André Johansen.
*/
//*****************************************************************************

PersonalInfoView::~PersonalInfoView()
{
}


//*****************************************************************************
/*!
  Set the log book to \a pcLogBook. The view will be updated.
  If a null-pointer is passed, the view will be cleared.

  \author André Johansen.
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
  The diver name was changed to \a pzName.
  Update the current log book, if any.
*/
//*****************************************************************************

void
PersonalInfoView::diverNameChanged(const char* pzName)
{
  if ( m_pcLogBook )
    m_pcLogBook->setDiverName(pzName);
}


//*****************************************************************************
/*!
  The email address was changed to \a pzEmailAddress.
  Update the current log book, if any.
*/
//*****************************************************************************

void
PersonalInfoView::emailAddressChanged(const char* pzEmailAddress)
{
  if ( m_pcLogBook )
    m_pcLogBook->setEmailAddress(pzEmailAddress);
}


//*****************************************************************************
/*!
  The www url was changed to \a pzWwwUrl.
  Update the current log book, if any.
*/
//*****************************************************************************

void
PersonalInfoView::wwwUrlChanged(const char* pzWwwUrl)
{
  if ( m_pcLogBook )
    m_pcLogBook->setWwwUrl(pzWwwUrl);
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
  QString cLoggedTimeText("0h 0min");
  if ( m_pcLogBook ) {
    unsigned int nNumMins = 0;
    DiveList& cDiveList = m_pcLogBook->diveList();
    for ( DiveLog* pcLog = cDiveList.first(); pcLog; pcLog = cDiveList.next() )
      nNumMins += pcLog->diveTime().hour() * 60 + pcLog->diveTime().minute();

    cLoggedTimeText.sprintf("Total logged dive time: %dh %dmin",
                            nNumMins/60, nNumMins%60);
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
    m_pcLogBook->setComments(m_pcComments->text());
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
