// Copyright (C) 2016 The Qt Company Ltd.
// Copyright (C) 2016 BasysKom GmbH.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQMLVMEMETAOBJECT_P_H
#define QQMLVMEMETAOBJECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QMetaObject>
#include <QtCore/QBitArray>
#include <QtCore/QPair>
#include <QtCore/QDate>
#include <QtCore/qlist.h>
#include <QtCore/qdebug.h>

#include <private/qobject_p.h>

#include "qqmlguard_p.h"

#include <private/qqmlguardedcontextdata_p.h>
#include <private/qbipointer_p.h>

#include <private/qv4object_p.h>
#include <private/qv4value_p.h>
#include <private/qqmlpropertyvalueinterceptor_p.h>

QT_BEGIN_NAMESPACE

class QQmlVMEMetaObject;
class QQmlVMEVariantQObjectPtr : public QQmlGuard<QObject>
{
public:
    inline QQmlVMEVariantQObjectPtr();

    inline void setGuardedValue(QObject *obj, QQmlVMEMetaObject *target, int index);

    QQmlVMEMetaObject *m_target;
    int m_index;

private:
    static void objectDestroyedImpl(QQmlGuardImpl *guard);
};


class Q_QML_PRIVATE_EXPORT QQmlInterceptorMetaObject : public QDynamicMetaObjectData
{
public:
    QQmlInterceptorMetaObject(QObject *obj, const QQmlPropertyCache::ConstPtr &cache);
    ~QQmlInterceptorMetaObject() override;

    void registerInterceptor(QQmlPropertyIndex index, QQmlPropertyValueInterceptor *interceptor);

    static QQmlInterceptorMetaObject *get(QObject *obj);

    QMetaObject *toDynamicMetaObject(QObject *o) override;

    // Used by auto-tests for inspection
    QQmlPropertyCache::ConstPtr propertyCache() const { return cache; }

    bool intercepts(QQmlPropertyIndex propertyIndex) const
    {
        for (auto it = interceptors; it; it = it->m_next) {
            if (it->m_propertyIndex == propertyIndex)
                return true;
        }
        if (auto parentInterceptor = ((parent.isT1() && parent.flag()) ? static_cast<QQmlInterceptorMetaObject *>(parent.asT1()) : nullptr))
            return parentInterceptor->intercepts(propertyIndex);
        return false;
    }

    QObject *object = nullptr;
    QQmlPropertyCache::ConstPtr cache;

protected:
    int metaCall(QObject *o, QMetaObject::Call c, int id, void **a) override;
    bool intercept(QMetaObject::Call c, int id, void **a)
    {
        if (!interceptors)
            return false;

        switch (c) {
        case QMetaObject::WriteProperty:
            if (*reinterpret_cast<int*>(a[3]) & QQmlPropertyData::BypassInterceptor)
                return false;
            break;
        case QMetaObject::BindableProperty:
            break;
        default:
            return false;
        }

        return doIntercept(c, id, a);
    }

    QBiPointer<QDynamicMetaObjectData, const QMetaObject> parent;
    const QMetaObject *metaObject = nullptr;

private:
    bool doIntercept(QMetaObject::Call c, int id, void **a);
    QQmlPropertyValueInterceptor *interceptors = nullptr;
};

inline QQmlInterceptorMetaObject *QQmlInterceptorMetaObject::get(QObject *obj)
{
    if (obj) {
        if (QQmlData *data = QQmlData::get(obj)) {
            if (data->hasInterceptorMetaObject)
                return static_cast<QQmlInterceptorMetaObject *>(QObjectPrivate::get(obj)->metaObject);
        }
    }

    return nullptr;
}

class QQmlVMEMetaObjectEndpoint;
class Q_QML_PRIVATE_EXPORT QQmlVMEMetaObject : public QQmlInterceptorMetaObject
{
public:
    QQmlVMEMetaObject(QV4::ExecutionEngine *engine, QObject *obj,
                      const QQmlPropertyCache::ConstPtr &cache,
                      const QQmlRefPointer<QV4::ExecutableCompilationUnit> &qmlCompilationUnit,
                      int qmlObjectId);
    ~QQmlVMEMetaObject() override;

    bool aliasTarget(int index, QObject **target, int *coreIndex, int *valueTypeIndex) const;
    QV4::ReturnedValue vmeMethod(int index) const;
    void setVmeMethod(int index, const QV4::Value &function);
    QV4::ReturnedValue vmeProperty(int index) const;
    void setVMEProperty(int index, const QV4::Value &v);

    void connectAliasSignal(int index, bool indexInSignalRange);

    static inline QQmlVMEMetaObject *get(QObject *o);
    static QQmlVMEMetaObject *getForProperty(QObject *o, int coreIndex);
    static QQmlVMEMetaObject *getForMethod(QObject *o, int coreIndex);
    static QQmlVMEMetaObject *getForSignal(QObject *o, int coreIndex);

protected:
    int metaCall(QObject *o, QMetaObject::Call _c, int _id, void **_a) override;

public:
    QV4::ExecutionEngine *engine;
    QQmlGuardedContextData ctxt;

    inline int propOffset() const;
    inline int methodOffset() const;
    inline int signalOffset() const;
    inline int signalCount() const;

    QQmlVMEMetaObjectEndpoint *aliasEndpoints;

    QV4::WeakValue propertyAndMethodStorage;
    QV4::MemberData *propertyAndMethodStorageAsMemberData() const;

    int readPropertyAsInt(int id) const;
    bool readPropertyAsBool(int id) const;
    double readPropertyAsDouble(int id) const;
    QString readPropertyAsString(int id) const;
    QSizeF readPropertyAsSizeF(int id) const;
    QPointF readPropertyAsPointF(int id) const;
    QUrl readPropertyAsUrl(int id) const;
    QDate readPropertyAsDate(int id) const;
    QTime readPropertyAsTime(int id) const;
    QDateTime readPropertyAsDateTime(int id) const;
    QRectF readPropertyAsRectF(int id) const;
    QObject *readPropertyAsQObject(int id) const;
    QVector<QQmlGuard<QObject> > *readPropertyAsList(int id) const;

    void writeProperty(int id, int v);
    void writeProperty(int id, bool v);
    void writeProperty(int id, double v);
    void writeProperty(int id, const QString& v);

    template<typename VariantCompatible>
    void writeProperty(int id, const VariantCompatible &v)
    {
        QV4::MemberData *md = propertyAndMethodStorageAsMemberData();
        if (md) {
            QV4::Scope scope(engine);
            QV4::Scoped<QV4::MemberData>(scope, md)->set(engine, id, engine->newVariantObject(
                                                             QVariant::fromValue(v)));
        }
    }

    void writeProperty(int id, QObject *v);

    void ensureQObjectWrapper();

    void mark(QV4::MarkStack *markStack);

    void connectAlias(int aliasId);

    QV4::ReturnedValue method(int) const;

    QV4::ReturnedValue readVarProperty(int) const;
    void writeVarProperty(int, const QV4::Value &);
    QVariant readPropertyAsVariant(int) const;
    void writeProperty(int, const QVariant &);

    inline QQmlVMEMetaObject *parentVMEMetaObject() const;

    void activate(QObject *, int, void **);

    QList<QQmlVMEVariantQObjectPtr *> varObjectGuards;

    QQmlVMEVariantQObjectPtr *getQObjectGuardForProperty(int) const;


    // keep a reference to the compilation unit in order to still
    // do property access when the context has been invalidated.
    QQmlRefPointer<QV4::ExecutableCompilationUnit> compilationUnit;
    const QV4::CompiledData::Object *compiledObject;
};

QQmlVMEMetaObject *QQmlVMEMetaObject::get(QObject *obj)
{
    if (obj) {
        if (QQmlData *data = QQmlData::get(obj)) {
            if (data->hasVMEMetaObject)
                return static_cast<QQmlVMEMetaObject *>(QObjectPrivate::get(obj)->metaObject);
        }
    }

    return nullptr;
}

int QQmlVMEMetaObject::propOffset() const
{
    return cache->propertyOffset();
}

int QQmlVMEMetaObject::methodOffset() const
{
    return cache->methodOffset();
}

int QQmlVMEMetaObject::signalOffset() const
{
    return cache->signalOffset();
}

int QQmlVMEMetaObject::signalCount() const
{
    return cache->signalCount();
}

QQmlVMEMetaObject *QQmlVMEMetaObject::parentVMEMetaObject() const
{
    if (parent.isT1() && parent.flag())
        return static_cast<QQmlVMEMetaObject *>(parent.asT1());

    return nullptr;
}

QT_END_NAMESPACE

#endif // QQMLVMEMETAOBJECT_P_H
