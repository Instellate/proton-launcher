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
#define LAUNCH_ARGUMENTS_COLUMN QStringLiteral("launch_arguments")
#define PROTON_PATH_COLUMN QStringLiteral("proton_path")
#define PLAY_TIME_COLUMN QStringLiteral("play_time")
#define LAST_PLAYED_COLUMN QStringLiteral("last_played")

GameInfo::GameInfo(QObject *parent, const QSqlQuery &query) : QObject(parent) {
    this->_id = query.value(ID_COLUMN).toString();
    this->_name = query.value(NAME_COLUMN).toString();
    this->_executableLocation = query.value(EXECUTABLE_LOCATION_COLUMN).toString();
    this->_prefixLocation = query.value(PREFIX_LOCATION_COLUMN).toString();
    this->_bannerLocation = query.value(BANNER_LOCATION_COLUMN);
    this->_launchArguments = query.value(LAUNCH_ARGUMENTS_COLUMN);
    this->_protonPath = query.value(PROTON_PATH_COLUMN);
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
    this->_bannerLocation = query.value(BANNER_LOCATION_COLUMN);
    this->_launchArguments = query.value(LAUNCH_ARGUMENTS_COLUMN);
    this->_protonPath = query.value(PROTON_PATH_COLUMN);
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

QVariant GameInfo::launchArguments() const {
    return this->_launchArguments;
}
QVariant GameInfo::protonPath() const {
    return this->_protonPath;
}

qint64 GameInfo::playTime() const {
    return this->_playTime;
}

QDateTime GameInfo::lastPlayed() const {
    return this->_lastPlayed;
}

QString GameInfo::consoleLog() const {
    return this->_consoleLog;
}

void GameInfo::setName(const QString &newName) {
    this->_name = newName;
    this->_updated = true;

    Q_EMIT nameChanged();
}

void GameInfo::setExecutableLocation(const QString &location) {
    this->_executableLocation = location;
    this->_updated = true;

    Q_EMIT executableLocationChanged();
}

void GameInfo::setPrefixLocation(const QString &location) {
    this->_prefixLocation = location;
    this->_updated = true;

    Q_EMIT prefixLocationChanged();
}

void GameInfo::setBannerLocation(const QVariant &location) {
    if (!location.isNull() && location.metaType() != QMetaType::fromType<QString>()) {
        qFatal() << "Expected string when setting banner location";
    }

    this->_bannerLocation = location;
    this->_updated = true;

    bannerLocationChanged();
}
void GameInfo::setLaunchArguments(const QVariant &arguments) {
    if (!arguments.isNull() && arguments.metaType() != QMetaType::fromType<QString>()) {
        qFatal() << "Expected string when setting launch arguments";
    }

    this->_launchArguments = arguments;
    this->_updated = true;

    Q_EMIT launchArgumentsChanged();
}

void GameInfo::setProtonPath(const QVariant &path) {
    if (!path.isNull() && path.metaType() != QMetaType::fromType<QString>()) {
        qFatal() << "Expected string when setting launch arguments";
    }

    this->_protonPath = path;
    this->_updated = true;

    Q_EMIT protonPathChanged();
}

void GameInfo::setPlayTime(const qint64 time) {
    this->_playTime = time;
    this->_updated = true;

    Q_EMIT playTimeChanged();
}

void GameInfo::setLastPlayed(const QDateTime &date) {
    this->_lastPlayed = date;
    this->_updated = true;

    Q_EMIT lastPlayedChanged();
}
