/*
 * Copyright 2005-2018  Thomas Baumgart <tbaumgart@kde.org>
 * Copyright 2017-2018  Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MYMONEYOBJECT_P_H
#define MYMONEYOBJECT_P_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QString>
#include <QDomElement>

// ----------------------------------------------------------------------------
// Project Includes

class MyMoneyObjectPrivate
{
public:
  MyMoneyObjectPrivate()
  {
  }

  virtual ~MyMoneyObjectPrivate()
  {
  }

  void setId(const QString& id)
  {
    m_id = id;
  }

  /**
   * This method writes out the members contained in this object.
   */
  void writeBaseXML(QDomDocument& document, QDomElement& el) const
  {
    Q_UNUSED(document);

    el.setAttribute(QStringLiteral("id"), m_id);
  }

  QString m_id;
};

#endif
