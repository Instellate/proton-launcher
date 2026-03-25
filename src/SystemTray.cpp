//
// Created by instellate on 25/03/2026.
//

#include "SystemTray.h"

#include <KLocalizedString>
#include <QCoreApplication>
#include <QIcon>

#include "GameManager.h"

SystemTray::SystemTray(QQmlApplicationEngine *engine, QObject *parent) : QObject(parent) {
    this->_qmlEngine = engine;
    this->_notifierItemMenu = new QMenu();

    const auto *gameManager = this->_qmlEngine->singletonInstance<const GameManager *>(
            QStringLiteral("xyz.instellate.protonLauncher"), QStringLiteral("GameManager"));
    QList<GameInfo *> games = gameManager->games();

    qsizetype count = 0;
    for (const GameInfo *game: games) {
        if (++count >= 5) {
            break;
        }

        // ReSharper disable once CppDFAMemoryLeak
        QAction *gameAction = new QAction(game->name(), this);

        this->_notifierItemMenu->addAction(gameAction);
        connect(gameAction, &QAction::triggered, [this, game] { gameTriggered(game); });
    }

    this->_notifierItemMenu->addSeparator();

    this->_recentAction = new QAction(
            i18nc("@system-tray:action:Opens recent games page", "Recent"),
            this->_notifierItemMenu);
    this->_recentAction->setIcon(QIcon::fromTheme(QStringLiteral("appointment-new")));
    this->_notifierItemMenu->addAction(this->_recentAction);

    connect(this->_recentAction, &QAction::triggered, this, &SystemTray::recentTriggered);

    this->_notifierItem =
            new KStatusNotifierItem(QStringLiteral("xyz.instellate.protonLauncher"), this);
    this->_notifierItem->setContextMenu(this->_notifierItemMenu);
    connect(this->_notifierItem,
            &KStatusNotifierItem::activateRequested,
            this,
            &SystemTray::activateRequested);
}

void SystemTray::recentTriggered() const {
    QObject *rootObject = this->_qmlEngine->rootObjects().first();

    const QMetaObject *metaObject = rootObject->metaObject();
    metaObject->invokeMethod(rootObject, "openRecent");
}

void SystemTray::gameTriggered(const GameInfo *game) const {
    QObject *rootObject = this->_qmlEngine->rootObjects().first();

    const QMetaObject *metaObject = rootObject->metaObject();
    metaObject->invokeMethod(rootObject, "openGame", QVariant::fromValue(game));
}

void SystemTray::activateRequested() const {
    QObject *rootObject = this->_qmlEngine->rootObjects().first();

    const QMetaObject *metaObject = rootObject->metaObject();
    metaObject->invokeMethod(rootObject, "showNormal");
    metaObject->invokeMethod(rootObject, "raise");
    metaObject->invokeMethod(rootObject, "requestActivate");
}
