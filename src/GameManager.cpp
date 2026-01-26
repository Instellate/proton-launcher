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

#include "GameManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <qobject.h>

QDir getPrefixPath() {
    const QString storageLocation = QStringLiteral(".proton-launcher");
    const QString games = QStringLiteral("games");
    const QString prefixes = QStringLiteral("prefixes");

    QDir prefixPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    prefixPath.cd(storageLocation);
    prefixPath.cd(prefixes);
    return prefixes;
}

GameManager::GameManager(QObject *parent) : QObject(parent) {
    auto database = QSqlDatabase::database();
    QSqlQuery query(QStringLiteral("SELECT * FROM games ORDER BY last_played DESC"));

    while (query.next()) {
        this->_games.emplace_back(new GameInfo(this, query));
    }

    gamesChanged();
}

GameManager::~GameManager() {
    for (const GameInfo *game: this->_games) {
        if (!game->_updated) {
            continue;
        }
        qDebug() << "Updating game" << game->_name;

        QSqlQuery query;
        query.prepare(QStringLiteral(
                "UPDATE games "
                "SET name = :name,"
                "executable_location = :exec,"
                "prefix_location = :prefix,"
                "banner_location = :banner,"
                "launch_arguments = :args,"
                "proton_path = :proton,"
                "play_time  = :play_time,"
                "last_played = :last_played "
                "WHERE id = :id"));

        query.bindValue(QStringLiteral(":id"), game->_id);
        query.bindValue(QStringLiteral(":name"), game->_name);
        query.bindValue(QStringLiteral(":exec"), game->_executableLocation);
        query.bindValue(QStringLiteral(":prefix"), game->_prefixLocation);
        query.bindValue(QStringLiteral(":banner"), game->_bannerLocation);
        query.bindValue(QStringLiteral(":args"), game->_launchArguments);
        query.bindValue(QStringLiteral(":proton"), game->_protonPath);
        query.bindValue(QStringLiteral(":play_time"), game->_playTime);
        query.bindValue(QStringLiteral(":last_played"), game->_lastPlayed);

        query.exec();
        qDebug() << query.lastError();
    }
}

QList<GameInfo *> GameManager::games() const {
    return this->_games;
}

QVariant GameManager::currentGameRunning() const {
    if (this->_currentGameRunning) {
        return this->_currentGameRunning->_id;
    }

    return {};
}

void GameManager::startGame(GameInfo *info) {
    if (this->_currentGameRunning) {
        qFatal() << "Game already running"; // TODO: Not crash
    }

    if (!info) {
        qFatal() << "Got null game info";
    }
    info->_consoleLog = QString();

    QDir steamDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    steamDir.cd(QStringLiteral(".steam"));
    steamDir.cd(QStringLiteral("steam"));

    const QString bashLocation = QStandardPaths::findExecutable(QStringLiteral("bash"));
    qDebug() << "Bash location:" << bashLocation;

    this->_gameProcess = new QProcess(this);
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("STEAM_COMPAT_DATA_PATH"), info->_prefixLocation);
    environment.insert(QStringLiteral("STEAM_COMPAT_CLIENT_INSTALL_PATH"), steamDir.path());
    environment.insert(QStringLiteral("SteamAppId"), info->_id);

    QString command;
    if (!info->_protonPath.isNull()) {
        command = info->_protonPath.toString();
    } else {
        command = getProtonInstallations().first().toString();
    }

    QString exec = info->_executableLocation;
    exec.replace(QStringLiteral("'"), QStringLiteral("''"));
    command += QStringLiteral(" waitforexitandrun '") + exec + QStringLiteral("'");
    qDebug() << "Command:" << command;

    if (!info->_launchArguments.isNull()) {
        command = info->_launchArguments.toString().replace(QStringLiteral("%command%"), command);
    }

    QStringList arguments;
    arguments << QStringLiteral("-c") << command;

    connect(this->_gameProcess, &QProcess::finished, this, &GameManager::gameProcessFinished);
    connect(this->_gameProcess,
            &QProcess::readyReadStandardOutput,
            this,
            &GameManager::readChannelAvailable);
    connect(this->_gameProcess,
            &QProcess::readyReadStandardError,
            this,
            &GameManager::readChannelAvailable);

    this->_gameProcess->setProcessEnvironment(environment);
    this->_gameProcess->start(bashLocation, arguments);
    qInfo() << "Started game" << info->_name;

    this->_currentGameRunning = info;

    Q_EMIT currentGameRunningChanged();

    info->setLastPlayed(QDateTime::currentDateTime());
}

void GameManager::stopGame() {
    if (!this->_gameProcess) {
        return;
    }

    this->_gameProcess->close();
}

void GameManager::addGame(
        const QString &name, const QUrl &executableLocation, const bool moveGame) {
    QFileInfo executableFile(executableLocation.toLocalFile());
    if (!executableFile.exists()) {
        qFatal() << "File" << executableLocation << "doesn't exist";
    }

    if (!executableFile.isFile()) {
        qFatal() << "Didn't get a file";
    }

    const QString storageLocation = QStringLiteral(".proton-launcher");
    const QString games = QStringLiteral("games");
    const QString prefixes = QStringLiteral("prefixes");

    QDir protonPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    if (!protonPath.exists(storageLocation)) {
        if (!protonPath.mkdir(storageLocation)) {
            qFatal() << "Couldn't create" << protonPath.filePath(storageLocation);
        }
    }
    protonPath.cd(storageLocation);

    if (moveGame) {
        if (!protonPath.exists(games)) {
            if (!protonPath.mkdir(games)) {
                qFatal() << "Couldn't create" << protonPath.filePath(games);
            }
        }

        const QDir executableDir = executableFile.dir();
        QDir gamesPath = protonPath.filePath(games);

        QDir dir;
        dir.rename(executableDir.absolutePath(), gamesPath.filePath(name));
        gamesPath.cd(name);
        executableFile = QFileInfo(gamesPath.filePath(executableFile.fileName()));
    }

    const QString gameId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QDir prefixPath = protonPath.filePath(prefixes);

    if (!prefixPath.mkpath(gameId)) {
        qFatal() << "Couldn't create directory" << prefixPath.filePath(gameId);
    }
    prefixPath.cd(gameId);

    auto database = QSqlDatabase::database();
    QSqlQuery query;
    query.prepare(QStringLiteral(
            "INSERT INTO games(id, name, executable_location, prefix_location)"
            "VALUES (:id, :name, :exec, :prefix)"));
    query.bindValue(QStringLiteral(":id"), gameId);
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":exec"), executableFile.absoluteFilePath());
    query.bindValue(QStringLiteral(":prefix"), prefixPath.absolutePath());
    query.exec();

    auto gameInfo = new GameInfo(this);
    gameInfo->_id = gameId;
    gameInfo->_name = name;
    gameInfo->_executableLocation = executableFile.absoluteFilePath();
    gameInfo->_prefixLocation = prefixPath.absolutePath();

    this->_games.emplace_back(gameInfo);
    gamesChanged();
}

QVariantMap GameManager::getProtonInstallations() {
    QVariantMap proton;

    QDir steam = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    steam.cd(QStringLiteral(".steam"));
    steam.cd(QStringLiteral("steam"));

    QDir compatTools = steam.filePath(QStringLiteral("compatibilitytools.d"));
    for (const QFileInfo &possibleInstall: compatTools.entryInfoList(QDir::Dirs)) {
        QFileInfo protonFile(QDir(possibleInstall.filePath()).filePath(QStringLiteral("proton")));

        if (!protonFile.exists() || !protonFile.isExecutable()) {
            continue;
        }

        proton.insert(possibleInstall.fileName(), protonFile.filePath());
    }

    QDir steamCommon = steam.filePath(QStringLiteral("steamapps"));
    steamCommon.cd(QStringLiteral("common"));

    for (const QFileInfo &possibleInstall: steamCommon.entryInfoList(QDir::Dirs)) {
        QFileInfo protonFile(QDir(possibleInstall.filePath()).filePath(QStringLiteral("proton")));
        if (!protonFile.exists() || !protonFile.isExecutable()) {
            continue;
        }

        proton.insert(possibleInstall.fileName(), protonFile.filePath());
    }

    return proton;
}

void GameManager::gameProcessFinished() {
    qDebug() << "Game process finished";

    const QString output = QString::fromUtf8(this->_gameProcess->readAllStandardOutput());
    const QString error = QString::fromUtf8(this->_gameProcess->readAllStandardError());

    this->_currentGameRunning->_consoleLog += output;
    this->_currentGameRunning->_consoleLog += error;
    Q_EMIT this->_currentGameRunning->consoleLogChanged();

    delete this->_gameProcess;
    this->_gameProcess = nullptr;
    this->_currentGameRunning = nullptr;
    Q_EMIT currentGameRunningChanged();
}

void GameManager::readChannelAvailable() {
    if (!this->_gameProcess) {
        qWarning() << "Got null game process";
        return;
    }

    const QString output = QString::fromUtf8(this->_gameProcess->readAllStandardOutput());
    const QString error = QString::fromUtf8(this->_gameProcess->readAllStandardError());

    this->_currentGameRunning->_consoleLog += output;
    this->_currentGameRunning->_consoleLog += error;
    Q_EMIT this->_currentGameRunning->consoleLogChanged();
}
