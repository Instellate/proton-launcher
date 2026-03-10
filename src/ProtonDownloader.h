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

#pragma once

#include <QCoro/Task>
#include <QNetworkAccessManager>
#include <QObject>
#include <QtQmlIntegration>

class ProtonDownloader : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool processing MEMBER _processing NOTIFY processingChanged)
    Q_PROPERTY(bool extracting MEMBER _extracting NOTIFY extractingChanged)
    Q_PROPERTY(qint64 amountDownloaded MEMBER _amountDownloaded NOTIFY amountDownloadedChanged)
    Q_PROPERTY(qint64 totalAmount MEMBER _totalAmount NOTIFY totalAmountChanged)
    QML_ELEMENT

    bool _processing = false;
    bool _extracting = false;
    qint64 _amountDownloaded;
    qint64 _totalAmount;

    QString _assetsUrl;
    QNetworkAccessManager *_networkManager;

    QCoro::Task<> getProtonGeVersionCoro();

    QCoro::Task<> downloadProtonGeCoro();

    void extractProtonGe(const QString &fileLocation);

public:
    explicit ProtonDownloader(QObject *parent = nullptr);

    Q_INVOKABLE void getProtonGeVersion();
    Q_INVOKABLE void downloadProtonGe();

Q_SIGNALS:
    void processingChanged();
    void amountDownloadedChanged();
    void totalAmountChanged();
    void extractingChanged();

    void foundProtonGeVersion(QString version, bool outdated);

private Q_SLOTS:
    void onDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
};
