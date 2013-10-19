//*****************************************************************************
/*!
  \file integerdialog.cpp
  \brief This file contains the implementation of IntegerDialog.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#include <assert.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <QGridLayout>
#include <kapplication.h>
#include <klocale.h>
#include "kintegeredit.h"
#include "integerdialog.h"
#include <algorithm>


//*****************************************************************************
/*!
  Create the dialog object with \a pcParent as parent widget and
  \a pzName as widget name. If \a isModal is `true', the dialog
  will be modal. The flags \a nFlags can be used to customize the dialog.

  The dialog will by default accept all possible integers. Use setMinValue()
  and setMaxValue() to restrict the range accepted.

  To get a label above the integer editor, use setText().
*/
//*****************************************************************************

IntegerDialog::IntegerDialog(QWidget* pcParent,
                             bool isModal, Qt::WFlags nFlags)
  : QDialog(pcParent, nFlags),
    m_pcText(0),
    m_pcEditor(0),
    m_pcOk(0),
    m_pcCancel(0)
{
  setModal(isModal);

  m_pcText = new QLabel(this);
  m_pcText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  //m_pcText->setAutoResize(true);

  m_pcEditor = new KIntegerEdit(this);
  m_pcEditor->setFocus();
  m_pcEditor->setMinimumSize(m_pcEditor->sizeHint());

  m_pcOk = new QPushButton(this);
  m_pcOk->setText(i18n("&Ok"));
  m_pcOk->setDefault(true);
  connect(m_pcOk, SIGNAL(clicked()), SLOT(accept()));

  m_pcCancel = new QPushButton(this);
  m_pcCancel->setText(i18n("&Cancel"));
  connect(m_pcCancel, SIGNAL(clicked()), SLOT(reject()));

  QSize cButtonSize(m_pcCancel->sizeHint());
  if ( m_pcOk->sizeHint().width() > cButtonSize.width() )
    cButtonSize.setWidth(m_pcOk->sizeHint().width());
  m_pcOk->setMinimumSize(cButtonSize);
  m_pcCancel->setMinimumSize(cButtonSize);

  QGridLayout* pcLayout = new QGridLayout(this);
  pcLayout->setColumnStretch(1, 1);
  pcLayout->setRowStretch(0, 1);
  pcLayout->addWidget(m_pcText,   0, 0, 0, 2);
  pcLayout->addWidget(m_pcEditor, 1, 1, 0, 2);
  pcLayout->addWidget(m_pcOk,     2, 0);
  pcLayout->addWidget(m_pcCancel, 2, 2);
  pcLayout->activate();

  updateGeometry();
}


//*****************************************************************************
/*!
  Destroy the dialog.
*/
//*****************************************************************************

IntegerDialog::~IntegerDialog()
{
  m_pcText   = 0;
  m_pcEditor = 0;
  m_pcOk     = 0;
  m_pcCancel = 0;
}


//*****************************************************************************
/*!
  Set the value to be shown to \a nValue.

  \sa KIntegerEdit::setValue().
*/
//*****************************************************************************

void
IntegerDialog::setValue(int nValue)
{
  m_pcEditor->setValue(nValue);
}


//*****************************************************************************
/*!
  Set the minimum acceptable value to \a nMinValue.

  \sa KIntegerEdit::setMinValue().
*/
//*****************************************************************************

void
IntegerDialog::setMinValue(int nMinValue)
{
  m_pcEditor->setMinValue(nMinValue);
}


//*****************************************************************************
/*!
  Set the maximum acceptable value to \a nMaxValue.

  \sa KIntegerEdit::setMaxValue().
*/
//*****************************************************************************

void
IntegerDialog::setMaxValue(int nMaxValue)
{
  m_pcEditor->setMaxValue(nMaxValue);
}


//*****************************************************************************
/*!
  Get the current value from the integer editor.
*/
//*****************************************************************************

int
IntegerDialog::getValue() const
{
  return m_pcEditor->getValue();
}


//*****************************************************************************
/*!
  Set the text to be shown above the editor to \a cText.
*/
//*****************************************************************************

void
IntegerDialog::setText(const QString& cText)
{
  m_pcText->setText(cText);
  m_pcText->setMinimumSize(m_pcText->sizeHint());
  updateGeometry();
}


//*****************************************************************************
/*!
  Update the geometry.
*/
//*****************************************************************************

void
IntegerDialog::updateGeometry()
{
  int nMinWidth  = std::max(m_pcEditor->minimumSize().width(),
                            m_pcOk->minimumSize().width() +
                            m_pcCancel->minimumSize().width());
  int nMinHeight = (m_pcText->minimumSize().width() +
                    m_pcEditor->minimumSize().height() +
                    m_pcOk->minimumSize().height());
  setMinimumSize(nMinWidth, nMinHeight);
  resize(nMinWidth, nMinHeight);
}


// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
