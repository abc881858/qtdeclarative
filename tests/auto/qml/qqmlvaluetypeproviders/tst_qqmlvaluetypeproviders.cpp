// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <qtest.h>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QDebug>
#include <QScopedPointer>
#include <private/qqmlglobal_p.h>
#include <private/qquickvaluetypes_p.h>
#include <QtQuickTestUtils/private/qmlutils_p.h>
#include "testtypes.h"

QT_BEGIN_NAMESPACE
extern int qt_defaultDpi(void);
QT_END_NAMESPACE

// There is some overlap between the qqmllanguage and qqmlvaluetypes
// test here, but it needs to be separate to ensure that no QML plugins
// are loaded prior to these tests, which could contaminate the type
// system with more providers.

class tst_qqmlvaluetypeproviders : public QQmlDataTest
{
    Q_OBJECT
public:
    tst_qqmlvaluetypeproviders() : QQmlDataTest(QT_QMLTEST_DATADIR) {}

private slots:
    void initTestCase() override;

    void qtqmlValueTypes();   // This test function _must_ be the first test function run.
    void qtquickValueTypes();
    void comparisonSemantics();
    void cppIntegration();
    void jsObjectConversion();
    void invokableFunctions();
    void userType();
    void changedSignal();
    void structured();
};

void tst_qqmlvaluetypeproviders::initTestCase()
{
    QQmlDataTest::initTestCase();
    registerTypes();
}

void tst_qqmlvaluetypeproviders::qtqmlValueTypes()
{
    QQmlEngine e;
    QQmlComponent component(&e, testFileUrl("qtqmlValueTypes.qml"));
    QVERIFY2(!component.isError(), qPrintable(component.errorString()));
    QVERIFY(component.errors().isEmpty());
    QScopedPointer<QObject> object(component.create());
    QVERIFY(object != nullptr);
    QVERIFY(object->property("qtqmlTypeSuccess").toBool());
    QVERIFY(object->property("qtquickTypeSuccess").toBool());
}

void tst_qqmlvaluetypeproviders::qtquickValueTypes()
{
    QQmlEngine e;
    QQmlComponent component(&e, testFileUrl("qtquickValueTypes.qml"));
    QVERIFY(!component.isError());
    QVERIFY(component.errors().isEmpty());
    QScopedPointer<QObject> object(component.create());
    QVERIFY(object != nullptr);
    QVERIFY(object->property("qtqmlTypeSuccess").toBool());
    QVERIFY(object->property("qtquickTypeSuccess").toBool());
}

void tst_qqmlvaluetypeproviders::comparisonSemantics()
{
    QQmlEngine e;
    QQmlComponent component(&e, testFileUrl("comparisonSemantics.qml"));
    QVERIFY(!component.isError());
    QVERIFY(component.errors().isEmpty());
    QScopedPointer<QObject> object(component.create());
    QVERIFY(object != nullptr);
    QVERIFY(object->property("comparisonSuccess").toBool());
}

void tst_qqmlvaluetypeproviders::cppIntegration()
{
    QQmlEngine e;
    QQmlComponent component(&e, testFileUrl("cppIntegration.qml"));
    QVERIFY(!component.isError());
    QVERIFY(component.errors().isEmpty());
    QScopedPointer<QObject> object(component.create());
    QVERIFY(object != nullptr);

    // ensure accessing / comparing / assigning cpp-defined props
    // and qml-defined props works in QML.
    QVERIFY(object->property("success").toBool());

    // ensure types match
    QCOMPARE(object->property("g").userType(), object->property("rectf").userType());
    QCOMPARE(object->property("p").userType(), object->property("pointf").userType());
    QCOMPARE(object->property("z").userType(), object->property("sizef").userType());
    QCOMPARE(object->property("v2").userType(), object->property("vector2").userType());
    QCOMPARE(object->property("v3").userType(), object->property("vector").userType());
    QCOMPARE(object->property("v4").userType(), object->property("vector4").userType());
    QCOMPARE(object->property("q").userType(), object->property("quaternion").userType());
    QCOMPARE(object->property("m").userType(), object->property("matrix").userType());
    QCOMPARE(object->property("c").userType(), object->property("color").userType());
    QCOMPARE(object->property("f").userType(), object->property("font").userType());

    // ensure values match
    QCOMPARE(object->property("g").value<QRectF>(), object->property("rectf").value<QRectF>());
    QCOMPARE(object->property("p").value<QPointF>(), object->property("pointf").value<QPointF>());
    QCOMPARE(object->property("z").value<QSizeF>(), object->property("sizef").value<QSizeF>());
    QCOMPARE(object->property("v2").value<QVector2D>(), object->property("vector2").value<QVector2D>());
    QCOMPARE(object->property("v3").value<QVector3D>(), object->property("vector").value<QVector3D>());
    QCOMPARE(object->property("v4").value<QVector4D>(), object->property("vector4").value<QVector4D>());
    QCOMPARE(object->property("q").value<QQuaternion>(), object->property("quaternion").value<QQuaternion>());
    QCOMPARE(object->property("m").value<QMatrix4x4>(), object->property("matrix").value<QMatrix4x4>());
    QCOMPARE(object->property("c").value<QColor>(), object->property("color").value<QColor>());
    QCOMPARE(object->property("f").value<QFont>(), object->property("font").value<QFont>());
}

void tst_qqmlvaluetypeproviders::jsObjectConversion()
{
    QQmlEngine e;
    QQmlComponent component(&e, testFileUrl("jsObjectConversion.qml"));
    QVERIFY(!component.isError());
    QVERIFY(component.errors().isEmpty());
    QScopedPointer<QObject> object(component.create());
    QVERIFY(object != nullptr);
    QVERIFY(object->property("qtquickTypeSuccess").toBool());
}

void tst_qqmlvaluetypeproviders::invokableFunctions()
{
    QQmlEngine e;
    QQmlComponent component(&e, testFileUrl("invokableFunctions.qml"));
    QVERIFY(!component.isError());
    QVERIFY(component.errors().isEmpty());
    QScopedPointer<QObject> object(component.create());
    QVERIFY(object != nullptr);
    QVERIFY(object->property("complete").toBool());
    QVERIFY(object->property("success").toBool());
}

namespace {

// A value-type class to export to QML
class TestValue
{
public:
    TestValue() : m_p1(0), m_p2(0.0) {}
    TestValue(int p1, double p2) : m_p1(p1), m_p2(p2) {}
    TestValue(const TestValue &other) : m_p1(other.m_p1), m_p2(other.m_p2) {}
    ~TestValue() {}

    TestValue &operator=(const TestValue &other) { m_p1 = other.m_p1; m_p2 = other.m_p2; return *this; }

    int property1() const { return m_p1; }
    void setProperty1(int p1) { m_p1 = p1; }

    double property2() const { return m_p2; }
    void setProperty2(double p2) { m_p2 = p2; }

    bool operator==(const TestValue &other) const { return (m_p1 == other.m_p1) && (m_p2 == other.m_p2); }
    bool operator!=(const TestValue &other) const { return !operator==(other); }

    bool operator<(const TestValue &other) const { if (m_p1 < other.m_p1) return true; return m_p2 < other.m_p2; }

private:
    int m_p1;
    double m_p2;
};

}

Q_DECLARE_METATYPE(TestValue);

namespace {

class TestValueType
{
    TestValue v;
    Q_GADGET
    Q_PROPERTY(int property1 READ property1 WRITE setProperty1)
    Q_PROPERTY(double property2 READ property2 WRITE setProperty2)
public:
    Q_INVOKABLE QString toString() const { return QString::number(property1()) + QLatin1Char(',') + QString::number(property2()); }

    int property1() const { return v.property1(); }
    void setProperty1(int p1) { v.setProperty1(p1); }

    double property2() const { return v.property2(); }
    void setProperty2(double p2) { v.setProperty2(p2); }
};

class TestValueExporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TestValue testValue READ testValue WRITE setTestValue)
    QML_NAMED_ELEMENT(TestValueExporter)
public:
    TestValue testValue() const { return m_testValue; }
    void setTestValue(const TestValue &v) { m_testValue = v; }

    Q_INVOKABLE TestValue getTestValue() const { return TestValue(333, 666.999); }

private:
    TestValue m_testValue;
};

}

void tst_qqmlvaluetypeproviders::userType()
{
    qmlRegisterExtendedType<TestValue, TestValueType>("Test", 1, 0, "test_value");
    qmlRegisterTypesAndRevisions<TestValueExporter>("Test", 1);
    Q_ASSERT(qMetaTypeId<TestValue>() >= QMetaType::User);

    TestValueExporter exporter;

    QQmlEngine e;

    QQmlComponent component(&e, testFileUrl("userType.qml"));
    QScopedPointer<QObject> obj(component.createWithInitialProperties({{"testValueExporter", QVariant::fromValue(&exporter)}}));
    QVERIFY(obj != nullptr);
    QCOMPARE(obj->property("success").toBool(), true);
}

void tst_qqmlvaluetypeproviders::changedSignal()
{
    QQmlEngine e;
    QQmlComponent component(&e, testFileUrl("changedSignal.qml"));
    QVERIFY(!component.isError());
    QVERIFY(component.errors().isEmpty());
    QScopedPointer<QObject> object(component.create());
    QVERIFY(object != nullptr);
    QVERIFY(object->property("complete").toBool());
    QVERIFY(object->property("success").toBool());
}

void tst_qqmlvaluetypeproviders::structured()
{
    QQmlEngine e;
    const QUrl url = testFileUrl("structuredValueTypes.qml");
    QQmlComponent component(&e, url);
    QVERIFY2(!component.isError(), qPrintable(component.errorString()));

    const char *warnings[] = {
        "Could not find any constructor for value type ConstructibleValueType to call"
            " with value [object Object]",
        "Could not find any constructor for value type ConstructibleValueType to call"
            " with value QVariant(QJSValue, )",
        "Could not convert [object Object] to double for property y",
        "Could not find any constructor for value type ConstructibleValueType to call"
            " with value [object Object]",
        "Could not find any constructor for value type ConstructibleValueType to call"
            " with value QVariant(QVariantMap, QMap())",
        "Could not convert array value at position 5 from QVariantMap to ConstructibleValueType",
        "Could not find any constructor for value type ConstructibleValueType to call"
            " with value [object Object]",
        "Could not find any constructor for value type ConstructibleValueType to call"
            " with value QVariant(QJSValue, )"
    };

    for (const auto warning : warnings)
        QTest::ignoreMessage(QtWarningMsg, warning);

    QTest::ignoreMessage(QtWarningMsg, qPrintable(
                             url.toString()  + QStringLiteral(":36: Error: Cannot assign QJSValue "
                                                              "to ConstructibleValueType")));

    QTest::ignoreMessage(QtWarningMsg, qPrintable(
                             url.toString()  + QStringLiteral(":14:5: Unable to assign QJSValue "
                                                              "to ConstructibleValueType")));

    QScopedPointer<QObject> o(component.create());
    QVERIFY2(!o.isNull(), qPrintable(component.errorString()));

    QCOMPARE(o->property("p").value<QPointF>(), QPointF(7, 77));
    QCOMPARE(o->property("s").value<QSizeF>(), QSizeF(7, 77));
    QCOMPARE(o->property("r").value<QRectF>(), QRectF(5, 55, 7, 77));

    QCOMPARE(o->property("p2").value<QPointF>(), QPointF(4, 5));
    QCOMPARE(o->property("s2").value<QSizeF>(), QSizeF(7, 8));
    QCOMPARE(o->property("r2").value<QRectF>(), QRectF(9, 10, 11, 12));

    QCOMPARE(o->property("c1").value<ConstructibleValueType>(), ConstructibleValueType(5));
    QCOMPARE(o->property("c2").value<ConstructibleValueType>(), ConstructibleValueType(0));
    QCOMPARE(o->property("c3").value<ConstructibleValueType>(), ConstructibleValueType(99));
    QCOMPARE(o->property("c4").value<ConstructibleValueType>(), ConstructibleValueType(0));

    QCOMPARE(o->property("ps").value<QList<QPointF>>(),
             QList<QPointF>({QPointF(1, 2), QPointF(3, 4), QPointF(55, 0)}));
    QCOMPARE(o->property("ss").value<QList<QSizeF>>(),
             QList<QSizeF>({QSizeF(5, 6), QSizeF(7, 8), QSizeF(-1, 99)}));
    QCOMPARE(o->property("cs").value<QList<ConstructibleValueType>>(),
             QList<ConstructibleValueType>({1, 2, 3, 4, 5, 0}));

    StructuredValueType b1;
    b1.setI(10);
    b1.setC(14);
    b1.setP(QPointF(1, 44));
    QCOMPARE(o->property("b1").value<StructuredValueType>(), b1);

    StructuredValueType b2;
    b2.setI(11);
    b2.setC(15);
    b2.setP(QPointF(4, 0));
    QCOMPARE(o->property("b2").value<StructuredValueType>(), b2);


    QList<StructuredValueType> bb(3);
    bb[0].setI(21);
    bb[1].setC(22);
    bb[2].setP(QPointF(199, 222));
    QCOMPARE(o->property("bb").value<QList<StructuredValueType>>(), bb);

    MyTypeObject *t = qobject_cast<MyTypeObject *>(o.data());
    QVERIFY(t);

    QCOMPARE(t->constructible(), ConstructibleValueType(47));

    StructuredValueType structured;
    structured.setI(11);
    structured.setC(12);
    structured.setP(QPointF(7, 8));
    QCOMPARE(t->structured(), structured);
}

QTEST_MAIN(tst_qqmlvaluetypeproviders)

#include "tst_qqmlvaluetypeproviders.moc"
