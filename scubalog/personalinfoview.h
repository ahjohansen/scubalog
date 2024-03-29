//*****************************************************************************
/*!
  \file personalinfoview.h
  \brief This file contains the definition of the PersonalInfoView class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef PERSONALINFOVIEW_H
#define PERSONALINFOVIEW_H

#include <qwidget.h>

class QLabel;
class QLineEdit;
class QTextEdit;
class LogBook;


//*****************************************************************************
/*!
  \class PersonalInfoView
  \brief The PersonalInfoView is used to view personal information.

  A log book contains some personal information about the diver,
  like name and email address.

  This class does not take ownership of the log book.

  \author André Johansen
*/
//*****************************************************************************

class PersonalInfoView : public QWidget {
  Q_OBJECT
public:
  PersonalInfoView(QWidget* pcParent);
  ~PersonalInfoView();

  void setLogBook(LogBook* pcLogBook);

public slots:
  void updateLoggedDiveTime();

private slots:
  void diverNameChanged(const QString& cName);
  void emailAddressChanged(const QString& cEmailAddress);
  void wwwUrlChanged(const QString& cWwwUrl);
  void commentsChanged();

private:
  //! Disabled copy constructor.
  PersonalInfoView(const PersonalInfoView&);
  //! Disabled assignment operator.
  PersonalInfoView& operator =(const PersonalInfoView&);

  //! The name editor.
  QLineEdit*      m_pcName;
  //! The email address editor.
  QLineEdit*      m_pcEmailAddress;
  //! The WWW URL editor.
  QLineEdit*      m_pcWwwUrl;
  //! The logged dive time label.
  QLabel*         m_pcLoggedDiveTime;
  //! The comments editor.
  QTextEdit*      m_pcComments;
  //! The log book being edited.
  LogBook*        m_pcLogBook;
};


#endif // PERSONALINFOVIEW_H


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
