/***************************************************************************
                          simpleledgerview.cpp
                             -------------------
    begin                : Sat Aug 8 2015
    copyright            : (C) 2015 by Thomas Baumgart
    email                : Thomas Baumgart <tbaumgart@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "simpleledgerview.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QTabBar>

// ----------------------------------------------------------------------------
// KDE Includes


// ----------------------------------------------------------------------------
// Project Includes

#include "kmymoneyviewbase_p.h"
#include "ledgerviewpage.h"
#include "models.h"
#include "accountsmodel.h"
#include "kmymoneyaccountcombo.h"
#include "ui_simpleledgerview.h"
#include "icons/icons.h"
#include "mymoneyfile.h"
#include "mymoneyaccount.h"
#include "mymoneyenums.h"
#include "modelenums.h"

using namespace Icons;

class SimpleLedgerViewPrivate : public KMyMoneyViewBasePrivate
{
  Q_DECLARE_PUBLIC(SimpleLedgerView)

public:
  explicit SimpleLedgerViewPrivate(SimpleLedgerView* qq)
  : q_ptr(qq)
  , ui(new Ui_SimpleLedgerView)
  , accountsModel(nullptr)
  , newTabWidget(nullptr)
  , lastIdx(-1)
  , inModelUpdate(false)
  , m_needLoad(true)
  {}

  ~SimpleLedgerViewPrivate()
  {
    delete ui;
  }

  void init()
  {
    Q_Q(SimpleLedgerView);
    m_needLoad = false;
    ui->setupUi(q);
    ui->ledgerTab->setTabIcon(0, Icons::get(Icon::ListAdd));
    ui->ledgerTab->setTabText(0, QString());
    newTabWidget = ui->ledgerTab->widget(0);

    accountsModel= new AccountNamesFilterProxyModel(q);

    // remove close button from new page
    QTabBar* bar = ui->ledgerTab->findChild<QTabBar*>();
    if(bar) {
      QTabBar::ButtonPosition closeSide = (QTabBar::ButtonPosition)q->style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, newTabWidget);
      QWidget *w = bar->tabButton(0, closeSide);
      bar->setTabButton(0, closeSide, 0);
      w->deleteLater();
      q->connect(bar, SIGNAL(tabMoved(int,int)), q, SLOT(checkTabOrder(int,int)));
    }

    q->connect(ui->accountCombo, SIGNAL(accountSelected(QString)), q, SLOT(openNewLedger(QString)));
    q->connect(ui->ledgerTab, &QTabWidget::currentChanged, q, &SimpleLedgerView::tabSelected);
    q->connect(Models::instance(), &Models::modelsLoaded, q, &SimpleLedgerView::updateModels);
    q->connect(ui->ledgerTab, &QTabWidget::tabCloseRequested, q, &SimpleLedgerView::closeLedger);

    accountsModel->addAccountGroup(QVector<eMyMoney::Account::Type> {eMyMoney::Account::Type::Asset, eMyMoney::Account::Type::Liability, eMyMoney::Account::Type::Equity});

    accountsModel->setHideEquityAccounts(false);
    auto const model = Models::instance()->accountsModel();
    accountsModel->setSourceModel(model);
    accountsModel->setSourceColumns(model->getColumns());
    accountsModel->sort((int)eAccountsModel::Column::Account);
    ui->accountCombo->setModel(accountsModel);

    q->tabSelected(0);
    q->updateModels();
    q->openFavoriteLedgers();
  }

  SimpleLedgerView*             q_ptr;
  Ui_SimpleLedgerView*          ui;
  AccountNamesFilterProxyModel* accountsModel;
  QWidget*                      newTabWidget;
  int                           lastIdx;
  bool                          inModelUpdate;
  bool                          m_needLoad;
};


SimpleLedgerView::SimpleLedgerView(QWidget *parent) :
    KMyMoneyViewBase(*new SimpleLedgerViewPrivate(this), parent)
{
}

SimpleLedgerView::~SimpleLedgerView()
{
}

void SimpleLedgerView::openNewLedger(QString accountId)
{
  Q_D(SimpleLedgerView);
  if(d->inModelUpdate || accountId.isEmpty())
    return;

  LedgerViewPage* view = 0;
  // check if ledger is already opened
  for(int idx=0; idx < d->ui->ledgerTab->count()-1; ++idx) {
    view = qobject_cast<LedgerViewPage*>(d->ui->ledgerTab->widget(idx));
    if(view) {
      if(accountId == view->accountId()) {
        d->ui->ledgerTab->setCurrentIndex(idx);
        return;
      }
    }
  }

  // need a new tab, we insert it before the rightmost one
  QModelIndex index = Models::instance()->accountsModel()->accountById(accountId);
  if(index.isValid()) {

    // create new ledger view page
    MyMoneyAccount acc = Models::instance()->accountsModel()->data(index, (int)eAccountsModel::Role::Account).value<MyMoneyAccount>();
    view = new LedgerViewPage(this);
    view->setShowEntryForNewTransaction();
    view->setAccount(acc);

    /// @todo setup current global setting for form visibility
    // view->showTransactionForm(...);

    // insert new ledger view page in tab view
    int newIdx = d->ui->ledgerTab->insertTab(d->ui->ledgerTab->count()-1, view, acc.name());
    d->ui->ledgerTab->setCurrentIndex(d->ui->ledgerTab->count()-1);
    d->ui->ledgerTab->setCurrentIndex(newIdx);
  }
}

void SimpleLedgerView::tabSelected(int idx)
{
  Q_D(SimpleLedgerView);
  // qDebug() << "tabSelected" << idx << (d->ui->ledgerTab->count()-1);
  if(idx != (d->ui->ledgerTab->count()-1)) {
    d->lastIdx = idx;
  }
}

void SimpleLedgerView::updateModels()
{
  Q_D(SimpleLedgerView);
  d->inModelUpdate = true;
  // d->ui->accountCombo->
  d->ui->accountCombo->expandAll();
  d->ui->accountCombo->setSelected(MyMoneyFile::instance()->asset().id());
  d->inModelUpdate = false;
}

void SimpleLedgerView::closeLedger(int idx)
{
  Q_D(SimpleLedgerView);
  // don't react on the close request for the new ledger function
  if(idx != (d->ui->ledgerTab->count()-1)) {
    d->ui->ledgerTab->removeTab(idx);
  }
}

void SimpleLedgerView::checkTabOrder(int from, int to)
{
  Q_D(SimpleLedgerView);
  if(d->inModelUpdate)
    return;

  QTabBar* bar = d->ui->ledgerTab->findChild<QTabBar*>();
  if(bar) {
    const int rightMostIdx = d->ui->ledgerTab->count()-1;

    if(from == rightMostIdx) {
      // someone tries to move the new account tab away from the rightmost position
      d->inModelUpdate = true;
      bar->moveTab(to, from);
      d->inModelUpdate = false;
    }
  }
}

void SimpleLedgerView::showTransactionForm(bool show)
{
  emit showForms(show);
}

void SimpleLedgerView::closeLedgers()
{
  Q_D(SimpleLedgerView);
  if (d->m_needLoad)
    return;
  auto tabCount = d->ui->ledgerTab->count();
  // check that we have a least one tab that can be closed
  if(tabCount > 1) {
    // we keep the tab with the selector open at all times
    // which is located in the right most position
    --tabCount;
    do {
      --tabCount;
      closeLedger(tabCount);
    } while(tabCount > 0);
  }
}

void SimpleLedgerView::openFavoriteLedgers()
{
  Q_D(SimpleLedgerView);
  if (d->m_needLoad)
    return;

  AccountsModel* model = Models::instance()->accountsModel();
  QModelIndex start = model->index(0, 0);
  QModelIndexList indexes = model->match(start, (int)eAccountsModel::Role::Favorite, QVariant(true), -1, Qt::MatchRecursive);

  // indexes now has a list of favorite accounts but two entries for each.
  // that doesn't matter here, since openNewLedger() can handle duplicates
  Q_FOREACH(QModelIndex index, indexes) {
    openNewLedger(model->data(index, (int)eAccountsModel::Role::ID).toString());
  }
  d->ui->ledgerTab->setCurrentIndex(0);
}

void SimpleLedgerView::showEvent(QShowEvent* event)
{
  Q_D(SimpleLedgerView);
  if (d->m_needLoad)
    d->init();

  // don't forget base class implementation
  QWidget::showEvent(event);
}
