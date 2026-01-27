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
#include <QThreadPool>
#include <chrono>

#include "GameManager.h"
#include "build/config.h"

#define ID_COLUMN QStringLiteral("id")
#define NAME_COLUMN QStringLiteral("name")
#define EXECUTABLE_LOCATION_COLUMN QStringLiteral("executable_location")
#define PREFIX_LOCATION_COLUMN QStringLiteral("prefix_location")
#define BANNER_LOCATION_COLUMN QStringLiteral("banner_location")
#define LAUNCH_ARGUMENTS_COLUMN QStringLiteral("launch_arguments")
#define PROTON_PATH_COLUMN QStringLiteral("proton_path")
#define PLAY_TIME_COLUMN QStringLiteral("play_time")
#define LAST_PLAYED_COLUMN QStringLiteral("last_played")
#define ICON_LOCATION_COLUMN QStringLiteral("icon_location")

GameInfo::GameInfo(QObject *parent, const QSqlQuery &query) : QObject(parent) {
    this->_id = query.value(ID_COLUMN).toString();
    this->_name = query.value(NAME_COLUMN).toString();
    this->_executableLocation = query.value(EXECUTABLE_LOCATION_COLUMN).toString();
    this->_prefixLocation = query.value(PREFIX_LOCATION_COLUMN).toString();
    this->_bannerLocation = query.value(BANNER_LOCATION_COLUMN);
    this->_launchArguments = query.value(LAUNCH_ARGUMENTS_COLUMN);
    this->_protonPath = query.value(PROTON_PATH_COLUMN);
    this->_playTime = query.value(PLAY_TIME_COLUMN).toLongLong();
    this->_lastPlayed = query.value(LAST_PLAYED_COLUMN);
    this->_iconLocation = query.value(ICON_LOCATION_COLUMN);
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
    this->_lastPlayed = query.value(LAST_PLAYED_COLUMN);
    this->_iconLocation = query.value(ICON_LOCATION_COLUMN);
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

QVariant GameInfo::lastPlayed() const {
    return this->_lastPlayed;
}

QVariant GameInfo::iconLocation() const {
    return this->_iconLocation;
}

QString GameInfo::consoleLog() const {
    return this->_consoleLog;
}

bool GameInfo::isRunning() const {
    return this->_gameProcess != nullptr;
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

void GameInfo::setBannerLocation(const QVariant &banner) {
    if (!banner.isNull() && banner.metaType() != QMetaType::fromType<QUrl>()) {
        qFatal() << "Expected string when setting banner location";
    }

    QDir gallery = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    gallery = gallery.filePath(QStringLiteral(".proton-launcher"));
    gallery = gallery.filePath(QStringLiteral("gallery"));

    if (!gallery.exists(this->_id)) {
        gallery.mkpath(this->_id);
    }
    gallery.cd(this->_id);

    const QStringList oldBanners = gallery.entryList({QStringLiteral("banner.*")});
    for (const QString &oldBanner: oldBanners) {
        QFile file(oldBanner);
        file.remove();
    }

    if (banner.isNull()) {
        this->_bannerLocation = QVariant();
        this->_updated = true;
        Q_EMIT bannerLocationChanged();
        return;
    }

    const QFileInfo fileInfo(banner.toUrl().toLocalFile());
    const QString galleryPath = gallery.filePath(QStringLiteral("banner.") + fileInfo.suffix());
    QFile::copy(fileInfo.absoluteFilePath(), galleryPath);

    this->_bannerLocation = galleryPath;
    this->_updated = true;

    Q_EMIT bannerLocationChanged();
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

void GameInfo::setLastPlayed(const QVariant &date) {
    if (!date.isNull() && date.metaType() != QMetaType::fromType<QDateTime>()) {
        qFatal() << "Expected string when setting launch arguments";
    }

    this->_lastPlayed = date;
    this->_updated = true;

    Q_EMIT lastPlayedChanged();
}

void GameInfo::setIconLocation(const QVariant &icon) {
    if (!icon.isNull() && icon.metaType() != QMetaType::fromType<QUrl>()) {
        qFatal() << "Expected string when setting launch arguments";
    }

    QDir gallery = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    gallery = gallery.filePath(QStringLiteral(".proton-launcher"));
    gallery = gallery.filePath(QStringLiteral("gallery"));

    if (!gallery.exists(this->_id)) {
        gallery.mkpath(this->_id);
    }
    gallery.cd(this->_id);

    const QStringList oldIcons = gallery.entryList({QStringLiteral("icon.*")});
    for (const QString &oldIcon: oldIcons) {
        QFile file(oldIcon);
        file.remove();
    }

    if (icon.isNull()) {
        this->_iconLocation = QVariant();
        this->_updated = true;
        Q_EMIT iconLocationChanged();
        return;
    }

    const QFileInfo fileInfo(icon.toUrl().toLocalFile());
    const QString galleryPath = gallery.filePath(QStringLiteral("icon.") + fileInfo.suffix());
    QFile::copy(fileInfo.absoluteFilePath(), galleryPath);

    this->_iconLocation = galleryPath;
    this->_updated = true;
    Q_EMIT iconLocationChanged();
}

void GameInfo::start() {
    if (this->_gameProcess) {
        qFatal() << "Game already running"; // TODO: Not crash
    }

    this->_consoleLog = QString();

    QDir steamDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    steamDir.cd(QStringLiteral(".steam"));
    steamDir.cd(QStringLiteral("steam"));

    const QString bashLocation = QStandardPaths::findExecutable(QStringLiteral("bash"));
    qDebug() << "Bash location:" << bashLocation;

    this->_gameProcess = new QProcess(this);
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("STEAM_COMPAT_DATA_PATH"), this->_prefixLocation);
    environment.insert(QStringLiteral("STEAM_COMPAT_CLIENT_INSTALL_PATH"), steamDir.path());
    environment.insert(QStringLiteral("SteamAppId"), this->_id);

    QString protonExecutable;
    if (!this->_protonPath.isNull()) {
        protonExecutable = this->_protonPath.toString();
    } else if (!Config::defaultProtonVersion().isEmpty()) {
        protonExecutable = this->_protonPath.toString();
    } else {
        protonExecutable = GameManager::getProtonInstallations().first().toString();
    }

    const QString executable = this->_executableLocation;

    QString command = QStringLiteral("\"${1}\" waitforexitandrun \"${2}\"");
    qDebug() << "Command:" << command;

    if (!this->_launchArguments.isNull()) {
        command = this->_launchArguments.toString().replace(QStringLiteral("%command%"), command);
    }

    QStringList arguments;
    arguments << QStringLiteral("-c") << command << QStringLiteral("_") << protonExecutable
              << executable;

    connect(this->_gameProcess, &QProcess::finished, this, &GameInfo::gameProcessFinished);
    connect(this->_gameProcess,
            &QProcess::readyReadStandardOutput,
            this,
            &GameInfo::readChannelAvailable);
    connect(this->_gameProcess,
            &QProcess::readyReadStandardError,
            this,
            &GameInfo::readChannelAvailable);

    this->_gameProcess->setProcessEnvironment(environment);
    this->_gameProcess->start(bashLocation, arguments);
    qInfo() << "Started game" << this->_name << "with process id"
            << this->_gameProcess->processId();
    Q_EMIT isRunningChanged();

    this->setLastPlayed(QDateTime::currentDateTime());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void GameInfo::stop() {
    QThreadPool::globalInstance()->start([this] {
        QDir protonPath = this->_gameProcess->arguments().at(3);
        protonPath.cdUp();
        protonPath.cd(QStringLiteral("files"));
        protonPath.cd(QStringLiteral("bin"));

        const QDir prefixPath = this->_prefixLocation;

        QProcess wineServerProcess;
        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
        environment.insert(
                QStringLiteral("WINEPREFIX"), prefixPath.filePath(QStringLiteral("pfx")));

        QStringList arguments;
        arguments << QStringLiteral("-k");

        QProcess process;
        process.setProgram(protonPath.filePath(QStringLiteral("wineserver")));
        process.setArguments(arguments);
        process.setProcessEnvironment(environment);
        process.start();

        process.waitForFinished();
    });
}

void GameInfo::gameProcessFinished() {
    const QString output = QString::fromUtf8(this->_gameProcess->readAllStandardOutput());
    const QString error = QString::fromUtf8(this->_gameProcess->readAllStandardError());

    this->_consoleLog += output;
    this->_consoleLog += error;
    Q_EMIT consoleLogChanged();

    delete this->_gameProcess;
    this->_gameProcess = nullptr;
    Q_EMIT isRunningChanged();

    // _lastPlayed gets updated to now when the game starts
    // So when the game stops we can get the time played by just subtracting current time from last
    // played
    const auto timePlayed = std::chrono::duration_cast<std::chrono::seconds>(
            QDateTime::currentDateTime() - this->_lastPlayed.toDateTime());
    this->setPlayTime(timePlayed.count() + this->_playTime);
    qDebug() << "Play time:" << timePlayed;
}

void GameInfo::readChannelAvailable() {
    if (!this->_gameProcess) {
        qWarning() << "Got null game process";
        return;
    }

    const QString output = QString::fromUtf8(this->_gameProcess->readAllStandardOutput());
    const QString error = QString::fromUtf8(this->_gameProcess->readAllStandardError());

    this->_consoleLog += output;
    this->_consoleLog += error;
    Q_EMIT consoleLogChanged();
}
