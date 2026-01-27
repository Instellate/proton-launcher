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

#include "GameInfo.h"

class GameManager final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<GameInfo *> games READ games NOTIFY gamesChanged)
    Q_PROPERTY(QVariant currentGameRunning READ currentGameRunning NOTIFY currentGameRunningChanged)
    QML_ELEMENT
    QML_SINGLETON

    QList<GameInfo *> _games;
    GameInfo *_currentGameRunning = nullptr;
    QProcess *_gameProcess = nullptr;

public:
    explicit GameManager(QObject *parent = nullptr);

    ~GameManager() override;

    [[nodiscard]] QList<GameInfo *> games() const;
    [[nodiscard]] QVariant currentGameRunning() const;

    Q_INVOKABLE void addGame(const QString &name, const QUrl &executableLocation, bool moveGame);

    Q_INVOKABLE static QVariantMap getProtonInstallations();

    Q_INVOKABLE static void saveSettings();

Q_SIGNALS:
    void gamesChanged();

    void currentGameRunningChanged();
};
