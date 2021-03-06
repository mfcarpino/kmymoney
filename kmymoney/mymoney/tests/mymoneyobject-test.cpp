/*
 * Copyright 2005-2011  Thomas Baumgart <tbaumgart@kde.org>
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

#include "mymoneyobject-test.h"

#include <QtTest>
#include <QDomDocument>
#include <QDomElement>

#include "mymoneyobject_p.h"
#include "mymoneyexception.h"
#include "mymoneyaccount.h"

class TestMyMoneyObjectPrivate : public MyMoneyObjectPrivate
{
public:
  TestMyMoneyObjectPrivate()
  {
  }
};
class TestMyMoneyObject : public MyMoneyObject
{
  Q_DECLARE_PRIVATE(TestMyMoneyObject)
public:
  TestMyMoneyObject() : MyMoneyObject(*new MyMoneyObjectPrivate) {}
  TestMyMoneyObject & operator=(TestMyMoneyObject other);
  friend void swap(TestMyMoneyObject& first, TestMyMoneyObject& second);
  ~TestMyMoneyObject(){}
  TestMyMoneyObject(const QDomElement& node, const bool forceId = true) :
      MyMoneyObject(*new MyMoneyObjectPrivate, node, forceId) {}
  bool hasReferenceTo(const QString&) const final override {
    return false;
  }
  void writeXML(QDomDocument&, QDomElement&) const final override {}
};

void swap(TestMyMoneyObject& first, TestMyMoneyObject& second)
{
  using std::swap;
  swap(first.d_ptr, second.d_ptr);
}

TestMyMoneyObject & TestMyMoneyObject::operator=(TestMyMoneyObject other)
{
  swap(*this, other);
  return *this;
}

QTEST_GUILESS_MAIN(MyMoneyObjectTest)

void MyMoneyObjectTest::testEmptyConstructor()
{
  MyMoneyAccount a;
  QVERIFY(a.id().isEmpty());
}

void MyMoneyObjectTest::testConstructor()
{
  MyMoneyAccount a(QString("thb"), MyMoneyAccount());

  QVERIFY(!a.id().isEmpty());
  QVERIFY(a.id() == QString("thb"));
}

void MyMoneyObjectTest::testClearId()
{
  MyMoneyAccount a(QString("thb"), MyMoneyAccount());

  QVERIFY(!a.id().isEmpty());
  a.clearId();
  QVERIFY(a.id().isEmpty());
}

void MyMoneyObjectTest::testCopyConstructor()
{
  MyMoneyAccount a(QString("thb"), MyMoneyAccount());
  MyMoneyAccount b(a);

  QVERIFY(a.MyMoneyObject::operator==(b));
}

void MyMoneyObjectTest::testAssignmentConstructor()
{
  MyMoneyAccount a(QString("thb"), MyMoneyAccount());
  MyMoneyAccount b = a;

  QVERIFY(a.MyMoneyObject::operator==(b));
}

void MyMoneyObjectTest::testEquality()
{
  MyMoneyAccount a(QString("thb"), MyMoneyAccount());
  MyMoneyAccount b(QString("thb"), MyMoneyAccount());
  MyMoneyAccount c(QString("ace"), MyMoneyAccount());

  QVERIFY(a.MyMoneyObject::operator==(b));
  QVERIFY(!(a.MyMoneyObject::operator==(c)));
}

void MyMoneyObjectTest::testReadXML()
{
  TestMyMoneyObject t;

  QString ref_ok = QString(
                     "<!DOCTYPE TEST>\n"
                     "<TRANSACTION-CONTAINER>\n"
                     " <MYMONEYOBJECT id=\"T000000000000000001\" >\n"
                     " </MYMONEYOBJECT>\n"
                     "</TRANSACTION-CONTAINER>\n"
                   );

  QString ref_false1 = QString(
                         "<!DOCTYPE TEST>\n"
                         "<TRANSACTION-CONTAINER>\n"
                         " <MYMONEYOBJECT id=\"\" >\n"
                         " </MYMONEYOBJECT>\n"
                         "</TRANSACTION-CONTAINER>\n"
                       );

  QString ref_false2 = QString(
                         "<!DOCTYPE TEST>\n"
                         "<TRANSACTION-CONTAINER>\n"
                         " <MYMONEYOBJECT >\n"
                         " </MYMONEYOBJECT>\n"
                         "</TRANSACTION-CONTAINER>\n"
                       );

  QDomDocument doc;
  QDomElement node;

  // id="" but required
  doc.setContent(ref_false1);
  node = doc.documentElement().firstChild().toElement();

  try {
    t = TestMyMoneyObject(node);
    QFAIL("Missing expected exception");
  } catch (const MyMoneyException &) {
  }

  // id attribute missing but required
  doc.setContent(ref_false2);
  node = doc.documentElement().firstChild().toElement();

  try {
    t = TestMyMoneyObject(node);
    QFAIL("Missing expected exception");
  } catch (const MyMoneyException &) {
  }

  // id present
  doc.setContent(ref_ok);
  node = doc.documentElement().firstChild().toElement();

  try {
    t = TestMyMoneyObject(node);
    QVERIFY(t.id() == "T000000000000000001");
  } catch (const MyMoneyException &) {
    QFAIL("Unexpected exception");
  }

  // id="" but not required
  doc.setContent(ref_false1);
  node = doc.documentElement().firstChild().toElement();

  try {
    t = TestMyMoneyObject(node, false);
    QVERIFY(t.id().isEmpty());
  } catch (const MyMoneyException &) {
    QFAIL("Unexpected exception");
  }
}
