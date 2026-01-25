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
#include <QSqlQuery>

GameManager::GameManager(QObject *parent) : QObject(parent) {
    auto database = QSqlDatabase::database();
    QSqlQuery query(QStringLiteral("SELECT * FROM games"));

    while (query.next()) {
        this->_games.emplace_back(new GameInfo(this, query));
    }
}

QList<GameInfo *> GameManager::games() {
    return this->_games;
}

void GameManager::addGame(
        const QString &name, const QUrl &executableLocation, const bool moveGame) {
    QFileInfo executableFile(executableLocation.toLocalFile());
    if (!executableFile.exists()) {
        qFatal() << "File" << executableLocation << "doesn't exist";
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
    query.bindValue(QStringLiteral(":exec"), executableFile.absolutePath());
    query.bindValue(QStringLiteral(":prefix"), prefixPath.absolutePath());
    query.exec();

    auto gameInfo = new GameInfo(this);
    gameInfo->_id = gameId;
    gameInfo->_name = name;
    gameInfo->_executableLocation = executableFile.absolutePath();
    gameInfo->_prefixLocation = prefixPath.absolutePath();

    this->_games.emplace_back(gameInfo);
    gamesChanged();
}
