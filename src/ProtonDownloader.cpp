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

#include <KTar>
#include <QCoro/QCoroNetwork>
#include <QNetworkReply>

#include "GameManager.h"

#define PROTON_GE_RELEASE_URL                                                                      \
    QStringLiteral("https://api.github.com/repos/GloriousEggroll/proton-ge-custom/releases")

using QNetworkReplyPtr = QSharedPointer<QNetworkReply>;

QCoro::Task<> ProtonDownloader::getProtonGeVersionCoro() {
    const QNetworkRequest request(QUrl(PROTON_GE_RELEASE_URL));

    const QNetworkReplyPtr reply(co_await this->_networkManager->get(request));
    const QByteArray bytes = reply->readAll();

    const QJsonDocument document = QJsonDocument::fromJson(bytes);
    const QJsonObject latestVersion = document.array().first().toObject();

    const QString tagName = latestVersion[QStringLiteral("tag_name")].toString();

    const QStringList protonInstallations = GameManager::getProtonInstallations().keys();
    if (!protonInstallations.contains(tagName)) {
        Q_EMIT foundProtonGeVersion(tagName, true);
        this->_assetsUrl = latestVersion[QStringLiteral("assets_url")].toString();
    } else {
        Q_EMIT foundProtonGeVersion(tagName, false);
    }

    co_return;
}

QCoro::Task<> ProtonDownloader::downloadProtonGeCoro() {
    this->_processing = true;
    Q_EMIT processingChanged();

    const QNetworkRequest request{QUrl(this->_assetsUrl)};

    const QNetworkReplyPtr reply{co_await this->_networkManager->get(request)};
    const QByteArray bytes = reply->readAll();

    const QJsonDocument document = QJsonDocument::fromJson(bytes);

    const QJsonArray assets = document.array();
    QString downloadUrlStr;
    QString assetName;
    QString digest;

    for (const QJsonValueConstRef assetValue: assets) {
        const QJsonObject assetObj = assetValue.toObject();

        const QString possibleUrl = assetObj[QStringLiteral("browser_download_url")].toString();
        if (possibleUrl.endsWith(QStringLiteral(".tar.gz"))) {
            downloadUrlStr = possibleUrl;
            assetName = assetObj[QStringLiteral("name")].toString();
            digest = assetObj[QStringLiteral("digest")].toString();
            break;
        }
    }

    if (downloadUrlStr.isEmpty()) {
        co_return;
    }

    QSharedPointer<QCryptographicHash> hasher{new QCryptographicHash(QCryptographicHash::Sha256)};

    const QDir tmpLocation(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    const QString fileLocation = tmpLocation.filePath(assetName);
    QSharedPointer<QFile> file(new QFile(tmpLocation.filePath(assetName)));

    if (!file->open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        qFatal() << "Cannot open file" << tmpLocation.filePath(assetName);
    }

    const QNetworkRequest downloadRequest{QUrl(downloadUrlStr)};

    QNetworkReply *downloadReply = this->_networkManager->get(downloadRequest);

    connect(downloadReply,
            &QNetworkReply::downloadProgress,
            this,
            &ProtonDownloader::onDownloadProgress);

    connect(downloadReply, &QNetworkReply::readyRead, [downloadReply, file, hasher] {
        const QByteArray partialBytes = downloadReply->readAll();
        file->write(partialBytes);
        hasher->addData(partialBytes);
    });

    connect(downloadReply,
            &QNetworkReply::finished,
            [downloadReply, file, fileLocation, hasher, digest, this] {
                const QString hasherResult =
                        QStringLiteral("sha256:") + QString::fromUtf8(hasher->result().toHex());
                if (hasherResult != digest) {
                    qFatal() << "Expected digest" << digest << "but got" << hasherResult
                             << "instead";
                }

                file->flush();
                file->close();

                downloadReply->deleteLater();
                this->_amountDownloaded = 0;
                this->_totalAmount = 0;

                Q_EMIT totalAmountChanged();
                Q_EMIT amountDownloadedChanged();

                QThreadPool::globalInstance()->start(
                        [this, fileLocation] { extractProtonGe(fileLocation); });
            });

    co_return;
}

void ProtonDownloader::extractProtonGe(const QString &fileLocation) {
    this->_extracting = true;
    Q_EMIT extractingChanged();

    KTar tar(fileLocation, QStringLiteral("application/gzip"));
    if (!tar.open(QIODevice::ReadOnly)) {
        qFatal() << "Cannot open" << fileLocation << "for reading";
    }

    const KArchiveDirectory *archiveDirectory = tar.directory();

    QDir steam = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    steam.cd(QStringLiteral(".local/share/Steam"));

    if (!steam.exists(QStringLiteral("compatibilitytools.d"))) {
        steam.mkdir(QStringLiteral("compatibilitytools.d"));
    }

    const QDir compatTools = steam.filePath(QStringLiteral("compatibilitytools.d"));
    if (!archiveDirectory->copyTo(compatTools.path())) {
        qFatal() << "Couldn't copy archive to" << compatTools;
    }

    tar.close();

    QFile archiveFile(fileLocation);
    if (!archiveFile.remove()) {
        qWarning() << "Couldn't remove ProtonGE archive at" << fileLocation;
    }

    this->_extracting = false;
    this->_processing = false;

    Q_EMIT extractingChanged();
    Q_EMIT processingChanged();
}

ProtonDownloader::ProtonDownloader(QObject *parent) : QObject(parent) {
    this->_networkManager = new QNetworkAccessManager(this);
    this->_amountDownloaded = 0;
    this->_totalAmount = 0;
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

void ProtonDownloader::downloadProtonGe() {
    downloadProtonGeCoro();
}

void ProtonDownloader::onDownloadProgress(const qint64 bytesSent, const qint64 bytesTotal) {
    this->_totalAmount = bytesTotal;
    this->_amountDownloaded = bytesSent;

    Q_EMIT totalAmountChanged();
    Q_EMIT amountDownloadedChanged();
}
