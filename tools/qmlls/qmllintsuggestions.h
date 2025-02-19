// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
#ifndef QMLLINTSUGGESTIONS_H
#define QMLLINTSUGGESTIONS_H

#include "qlanguageserver.h"
#include "qqmlcodemodel.h"

#include <optional>

QT_BEGIN_NAMESPACE
namespace QmlLsp {
struct LastLintUpdate
{
    std::optional<int> version;
    std::optional<QDateTime> invalidUpdatesSince;
};

class QmlLintSuggestions : public QLanguageServerModule
{
    Q_OBJECT
public:
    QmlLintSuggestions(QLanguageServer *server, QmlLsp::QQmlCodeModel *codeModel);

    QString name() const override { return QLatin1StringView("QmlLint Suggestions"); }
public slots:
    void diagnose(const QByteArray &uri);
    void registerHandlers(QLanguageServer *server, QLanguageServerProtocol *protocol) override;
    void setupCapabilities(const QLspSpecification::InitializeParams &clientInfo,
                           QLspSpecification::InitializeResult &) override;

private:
    QMutex m_mutex;
    QHash<QByteArray, LastLintUpdate> m_lastUpdate;
    QLanguageServer *m_server;
    QmlLsp::QQmlCodeModel *m_codeModel;
};
} // namespace QmlLsp
QT_END_NAMESPACE
#endif // QMLLINTSUGGESTIONS_H
