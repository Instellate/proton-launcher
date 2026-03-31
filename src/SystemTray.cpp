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

#include "SystemTray.h"

#include <KLocalizedString>
#include <QIcon>

#include "GameManager.h"

SystemTray::SystemTray(QQmlApplicationEngine *engine, QObject *parent) : QObject(parent) {
    this->_qmlEngine = engine;
    this->_notifierItemMenu = new QMenu();

    GameManager *gameManager = getGameManager();
    connect(gameManager, &GameManager::gamesChanged, this, &SystemTray::gamesChanged);

    QList<const GameInfo *> games = gameManager->games().toList<QList<const GameInfo *>>();

    qsizetype count = 0;
    for (const GameInfo *game : games) {
        if (++count >= 5) {
            break;
        }

        QAction *gameAction = new QAction(game->name(), this);

        connect(gameAction, &QAction::triggered, [this, game] { gameTriggered(game); });
        this->_gameActions.emplace_back(gameAction);
    }
    this->_notifierItemMenu->addActions(this->_gameActions);

    if (count > 0) {
        this->_gamesSeperator = this->_notifierItemMenu->addSeparator();
    }

    this->_recentAction = new QAction(
            i18nc("@system-tray:action:Opens recent games page", "Recent"),
            this->_notifierItemMenu);
    this->_recentAction->setIcon(QIcon::fromTheme(QStringLiteral("appointment-new")));
    this->_notifierItemMenu->addAction(this->_recentAction);

    connect(this->_recentAction, &QAction::triggered, this, &SystemTray::recentTriggered);

    this->_notifierItem =
            new KStatusNotifierItem(QStringLiteral("xyz.instellate.proton-launcher"), this);
    this->_notifierItem->setIconByName(QStringLiteral("xyz.instellate.proton-launcher"));
    this->_notifierItem->setContextMenu(this->_notifierItemMenu);
    this->_notifierItem->setStatus(KStatusNotifierItem::Active);

    connect(this->_notifierItem,
            &KStatusNotifierItem::activateRequested,
            this,
            &SystemTray::activateRequested);
}

void SystemTray::recentTriggered() const {
    QObject *rootObject = this->_qmlEngine->rootObjects().first();

    QMetaObject::invokeMethod(rootObject, "openRecent");
}

void SystemTray::gameTriggered(const GameInfo *game) const {
    QObject *rootObject = this->_qmlEngine->rootObjects().first();

    QMetaObject::invokeMethod(rootObject, "openGame", QVariant::fromValue(game));
}

void SystemTray::activateRequested() const {
    QObject *rootObject = this->_qmlEngine->rootObjects().first();

    QMetaObject::invokeMethod(rootObject, "showNormal");
    QMetaObject::invokeMethod(rootObject, "raise");
    QMetaObject::invokeMethod(rootObject, "requestActivate");
}

void SystemTray::gamesChanged() {
    GameManager *gameManager = getGameManager();

    for (QAction *action : this->_gameActions) {
        this->_notifierItemMenu->removeAction(action);
        delete action;
    }
    this->_gameActions.clear();

    if (this->_gamesSeperator) {
        this->_notifierItemMenu->removeAction(this->_gamesSeperator);
        this->_gamesSeperator = nullptr;
    }

    QList<const GameInfo *> games = gameManager->games().toList<QList<const GameInfo *>>();
    if (games.size() <= 0) {
        return;
    }

    qsizetype count = 0;
    for (const GameInfo *game : games) {
        if (++count >= 5) {
            break;
        }

        QAction *gameAction = new QAction(game->name(), this);

        connect(gameAction, &QAction::triggered, [this, game] { gameTriggered(game); });
        this->_gameActions.emplace_back(gameAction);
    }

    this->_notifierItemMenu->insertActions(this->_recentAction, this->_gameActions);
    this->_gamesSeperator = this->_notifierItemMenu->insertSeparator(this->_recentAction);
}

GameManager *SystemTray::getGameManager() const {
    return this->_qmlEngine->singletonInstance<GameManager *>(
            QStringLiteral("xyz.instellate.protonLauncher"), QStringLiteral("GameManager"));
}
