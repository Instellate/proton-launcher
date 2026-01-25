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

#include <QObject>
#include <QtQmlIntegration>

class QSqlQuery;

class GameInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString executableLocation READ executableLocation WRITE setExecutableLocation NOTIFY executableLocationChanged)
    Q_PROPERTY(QString prefixLocation READ prefixLocation WRITE setPrefixLocation NOTIFY prefixLocationChanged)
    Q_PROPERTY(QVariant bannerLocation READ bannerLocation WRITE setBannerLocation NOTIFY bannerLocationChanged)
    Q_PROPERTY(qint64 playTime READ playTime WRITE setPlayTime NOTIFY playTimeChanged)
    Q_PROPERTY(QDateTime lastPlayed READ lastPlayed WRITE setLastPlayed NOTIFY lastPlayedChanged)
    QML_ELEMENT

    QString _id;
    QString _name;
    QString _executableLocation;
    QString _prefixLocation;
    QVariant _bannerLocation;
    QString _launchArguments;
    qint64 _playTime = 0;
    QDateTime _lastPlayed;

    friend class GameManager;

    explicit GameInfo(QObject *parent, const QSqlQuery &query);
public:
    explicit GameInfo(QObject *parent = nullptr) : QObject(parent) {
    }

    explicit GameInfo(QObject *parent, const QString &id);

    QString id() const;
    QString name() const;
    QString executableLocation() const;
    QString prefixLocation() const;
    QVariant bannerLocation() const;
    QString launchArguments() const;
    qint64 playTime() const;
    QDateTime lastPlayed() const;

    void setId(const QString &newId);
    void setName(const QString &newName);
    void setExecutableLocation(const QString &location);
    void setPrefixLocation(const QString &location);
    void setBannerLocation(const QVariant &location);
    void setLaunchArguments(const QString &arguments);
    void setPlayTime(qint64 time);
    void setLastPlayed(const QDateTime &date);

Q_SIGNALS:
    void idChanged();
    void nameChanged();
    void executableLocationChanged();
    void prefixLocationChanged();
    void bannerLocationChanged();
    void launchArgumentsChanged();
    void playTimeChanged();
    void lastPlayedChanged();
};
