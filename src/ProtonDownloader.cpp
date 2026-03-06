// Copyright (C) 2026  Instellate
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>

#include "ProtonDownloader.h"

#include <QCoro/QCoroNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "GameManager.h"

#define PROTON_GE_RELEASE_URL                                                                      \
    QStringLiteral("https://api.github.com/repos/GloriousEggroll/proton-ge-custom/releases")

QCoro::Task<> ProtonDownloader::getProtonGeVersionCoro() {
    QNetworkAccessManager networkAccessManager;
    const QNetworkRequest request{QUrl(PROTON_GE_RELEASE_URL)};

    const QSharedPointer<QNetworkReply> reply(co_await networkAccessManager.get(request));
    const QByteArray bytes = reply->readAll();

    const QJsonDocument document = QJsonDocument::fromJson(bytes);
    const QJsonObject latestVersion = document.array().first().toObject();

    const QString tagName = latestVersion[QStringLiteral("tag_name")].toString();

    const QStringList protonInstallations = GameManager::getProtonInstallations().keys();
    if (!protonInstallations.contains(tagName)) {
        Q_EMIT foundProtonGeVersion(tagName, true);
    } else {
        Q_EMIT foundProtonGeVersion(tagName, false);
    }

    co_return;
}

ProtonDownloader::ProtonDownloader(QObject *parent) : QObject(parent) {
}

void ProtonDownloader::getProtonGeVersion() {
    this->_processing = true;
    Q_EMIT processingChanged();
    getProtonGeVersionCoro().then(
            [this] {
                this->_processing = false;
                Q_EMIT processingChanged();
            },
            [this](const std::exception &e) {
                qWarning() << "Got error when trying to check ProtonGE"
                           << QString::fromUtf8(e.what());

                this->_processing = false;
                Q_EMIT processingChanged();
            });
}
