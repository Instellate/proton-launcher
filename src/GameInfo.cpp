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

#include "GameInfo.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#define ID_COLUMN QStringLiteral("id")
#define NAME_COLUMN QStringLiteral("name")
#define EXECUTABLE_LOCATION_COLUMN QStringLiteral("executable_location")
#define PREFIX_LOCATION_COLUMN QStringLiteral("prefix_location")
#define BANNER_LOCATION_COLUMN QStringLiteral("banner_location")
#define PLAY_TIME_COLUMN QStringLiteral("play_time")
#define LAST_PLAYED_COLUMN QStringLiteral("last_played")

GameInfo::GameInfo(QObject *parent, const QSqlQuery &query) : QObject(parent) {
    this->_id = query.value(ID_COLUMN).toString();
    this->_name = query.value(NAME_COLUMN).toString();
    this->_executableLocation = query.value(EXECUTABLE_LOCATION_COLUMN).toString();
    this->_prefixLocation = query.value(PREFIX_LOCATION_COLUMN).toString();
    this->_bannerLocation = query.value(BANNER_LOCATION_COLUMN).toString();
    this->_playTime = query.value(PLAY_TIME_COLUMN).toLongLong();
    this->_lastPlayed = query.value(LAST_PLAYED_COLUMN).toDateTime();
}

GameInfo::GameInfo(QObject *parent, const QString &id) : QObject(parent) {
    auto database = QSqlDatabase::database();

    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT * FROM games WHERE id = ?"));
    query.addBindValue(id);

    if (!query.next()) {
        qFatal() << "Couldn't get game with id" << id;
    }

    this->_id = query.value(ID_COLUMN).toString();
    this->_name = query.value(NAME_COLUMN).toString();
    this->_executableLocation = query.value(EXECUTABLE_LOCATION_COLUMN).toString();
    this->_prefixLocation = query.value(PREFIX_LOCATION_COLUMN).toString();
    this->_bannerLocation = query.value(BANNER_LOCATION_COLUMN).toString();
    this->_playTime = query.value(PLAY_TIME_COLUMN).toLongLong();
    this->_lastPlayed = query.value(LAST_PLAYED_COLUMN).toDateTime();
}

QString GameInfo::id() const {
    return this->_id;
}

QString GameInfo::name() const {
    return this->_name;
}

QString GameInfo::executableLocation() const {
    return this->_executableLocation;
}

QString GameInfo::prefixLocation() const {
    return this->_prefixLocation;
}

QVariant GameInfo::bannerLocation() const {
    return this->_bannerLocation;
}

QString GameInfo::launchArguments() const {
    return this->_launchArguments;
}

qint64 GameInfo::playTime() const {
    return this->_playTime;
}

QDateTime GameInfo::lastPlayed() const {
    return this->_lastPlayed;
}

void GameInfo::setId(const QString &newId) {
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE games SET id = :new_id WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->_id);
    query.bindValue(QStringLiteral(":new_id"), newId);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    this->_id = newId;
    idChanged();
}
void GameInfo::setName(const QString &newName) {
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE games SET name = :value WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->_id);
    query.bindValue(QStringLiteral(":value"), newName);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    this->_name = newName;
    nameChanged();
}

void GameInfo::setExecutableLocation(const QString &location) {
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE games SET executable_location = :value WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->_id);
    query.bindValue(QStringLiteral(":value"), location);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    this->_executableLocation = location;
    executableLocationChanged();
}

void GameInfo::setPrefixLocation(const QString &location) {
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE games SET prefix_location = :value WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->_id);
    query.bindValue(QStringLiteral(":value"), location);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    this->_prefixLocation = location;
    prefixLocationChanged();
}

void GameInfo::setBannerLocation(const QVariant &location) {
    if (!location.isNull() && location.metaType() != QMetaType::fromType<QString>()) {
        qFatal() << "Expected string when setting banner location";
    }

    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE games SET banner_location = :value WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->_id);
    query.bindValue(QStringLiteral(":value"), location);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    this->_bannerLocation = location;
    bannerLocationChanged();
}
void GameInfo::setLaunchArguments(const QString &arguments) {
    this->_launchArguments = arguments;
    launchArgumentsChanged();
}

void GameInfo::setPlayTime(const qint64 time) {
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE games SET last_played = :value WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->_id);
    query.bindValue(QStringLiteral(":value"), time);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    this->_playTime = time;
    playTimeChanged();
}

void GameInfo::setLastPlayed(const QDateTime &date) {
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE games SET play_time = :value WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->_id);
    query.bindValue(QStringLiteral(":value"), date);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    this->_lastPlayed = date;
    lastPlayedChanged();
}
