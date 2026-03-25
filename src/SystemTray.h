//
// Created by instellate on 25/03/2026.
//

#pragma once

#include <KStatusNotifierItem>
#include <QAction>
#include <QMenu>
#include <QObject>
#include <QQmlApplicationEngine>

class GameInfo;

class SystemTray : public QObject {
    Q_OBJECT

    QQmlApplicationEngine *_qmlEngine;

    KStatusNotifierItem *_notifierItem;
    QMenu *_notifierItemMenu;

    QAction *_quitAction;
    QAction *_recentAction;

public:
    explicit SystemTray(QQmlApplicationEngine *engine, QObject *parent = nullptr);

private Q_SLOT:

    void recentTriggered() const;

    void gameTriggered(const GameInfo *game) const;

    void activateRequested() const;
};
