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

#include <KStatusNotifierItem>
#include <QAction>
#include <QMenu>
#include <QObject>
#include <QQmlApplicationEngine>

class SystemTray : public QObject {
    Q_OBJECT

    QQmlApplicationEngine *_qmlEngine;

    KStatusNotifierItem *_notifierItem;
    QMenu *_notifierItemMenu;

    QAction *_recentAction;
    QAction *_gamesSeperator = nullptr;
    QList<QAction *> _gameActions;

public:
    explicit SystemTray(QQmlApplicationEngine *engine, QObject *parent = nullptr);

private Q_SLOTS:
    void recentTriggered() const;

    void gameTriggered(const class GameInfo *game) const;

    void activateRequested() const;

    void gamesChanged();

private:
    class GameManager *getGameManager() const;
};
