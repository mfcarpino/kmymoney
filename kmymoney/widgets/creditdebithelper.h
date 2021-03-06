/***************************************************************************
                               creditdebithelper.h
                             -------------------
    copyright            : (C) 2016 by Thomas Baumgart <tbaumgart@kde.org>
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

#ifndef CREDITDEBITHELPER_H
#define CREDITDEBITHELPER_H

#include <qobjectdefs.h>

#include "kmm_widgets_export.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <qobject.h>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

class AmountEdit;
class MyMoneyMoney;

class CreditDebitHelperPrivate;
class KMM_WIDGETS_EXPORT CreditDebitHelper : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(CreditDebitHelper)

public:
  explicit CreditDebitHelper(QObject* parent, AmountEdit* credit, AmountEdit* debit);
  ~CreditDebitHelper();

  /**
   * Retruns the value of the widget that is filled.
   * A credit is retruned as negative, a debit as positive value.
   */
  MyMoneyMoney value() const;

  /**
   * Loads the widgets with the @a value passed. If
   * @a value is negative it is loaded into the credit
   * widget, otherwise into the debit widget.
   */
  void setValue(const MyMoneyMoney& value);

  /**
   * This method returns true if at least one
   * of the two widgets is filled with text.
   * It returns false if both widgets are empty.
   */
  bool haveValue() const;

Q_SIGNALS:
  void valueChanged();

private Q_SLOTS:
  void creditChanged();
  void debitChanged();

private:
  CreditDebitHelperPrivate * const d_ptr;
  Q_DECLARE_PRIVATE(CreditDebitHelper)
};

#endif
