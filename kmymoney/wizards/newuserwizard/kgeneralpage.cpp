/***************************************************************************
                             kgeneralpage.cpp
                             -------------------
    begin                : Sat Feb 18 2006
    copyright            : (C) 2006 Thomas Baumgart
    email                : Thomas Baumgart <ipwizard@users.sourceforge.net>
                           (C) 2017 by Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kgeneralpage.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QPushButton>

// ----------------------------------------------------------------------------
// KDE Includes

#include <KLineEdit>
#include <KLocalizedString>
#include <KMessageBox>

// ----------------------------------------------------------------------------
// Project Includes

#include "userinfo.h"
#include "ui_userinfo.h"

#include "knewuserwizard.h"
#include "knewuserwizard_p.h"

#include "mymoneycontact.h"
#include "kcurrencypage.h"

class KMyMoneyWizardPage;

namespace NewUserWizard
{
  class GeneralPagePrivate : public WizardPagePrivate<Wizard>
  {
    Q_DISABLE_COPY(GeneralPagePrivate)

  public:
    GeneralPagePrivate(QObject* parent) :
      WizardPagePrivate<Wizard>(parent),
      m_contact(nullptr)
    {
    }

    MyMoneyContact *m_contact;
  };

  GeneralPage::GeneralPage(Wizard* wizard) :
    UserInfo(wizard),
    WizardPage<Wizard>(*new GeneralPagePrivate(wizard), stepCount++, this, wizard)
  {
    Q_D(GeneralPage);
    d->m_contact = new MyMoneyContact(this);
    ui->m_userNameEdit->setFocus();

    ui->m_loadAddressButton->setEnabled(d->m_contact->ownerExists());
    connect(ui->m_loadAddressButton, &QAbstractButton::clicked, this, &GeneralPage::slotLoadFromAddressBook);
  }

  GeneralPage::~GeneralPage()
  {
  }

  void GeneralPage::slotLoadFromAddressBook()
  {
    Q_D(GeneralPage);
    ui->m_userNameEdit->setText(d->m_contact->ownerFullName());
    ui->m_emailEdit->setText(d->m_contact->ownerEmail());
    if (ui->m_emailEdit->text().isEmpty()) {
      KMessageBox::sorry(this, i18n("Unable to load data, because no contact has been associated with the owner of the standard address book."), i18n("Address book import"));
      return;
    }
    ui->m_loadAddressButton->setEnabled(false);
    connect(d->m_contact, &MyMoneyContact::contactFetched, this, &GeneralPage::slotContactFetched);
    d->m_contact->fetchContact(ui->m_emailEdit->text());
  }

  void GeneralPage::slotContactFetched(const ContactData &identity)
  {
    ui->m_loadAddressButton->setEnabled(true);
    if (identity.email.isEmpty())
      return;
    ui->m_telephoneEdit->setText(identity.phoneNumber);
    QString sep;
    if (!identity.country.isEmpty() && !identity.region.isEmpty())
      sep = " / ";
    ui->m_countyEdit->setText(QString("%1%2%3").arg(identity.country, sep, identity.region));
    ui->m_postcodeEdit->setText(identity.postalCode);
    ui->m_townEdit->setText(identity.locality);
    ui->m_streetEdit->setText(identity.street);
  }

  KMyMoneyWizardPage* GeneralPage::nextPage() const
  {
    Q_D(const GeneralPage);
    return d->m_wizard->d_func()->m_currencyPage;
  }

}
