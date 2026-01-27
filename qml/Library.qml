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

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

Kirigami.Page {
    id: root
    title: i18nc("@title", "Library")

    property Component addGameComponent: Qt.createComponent("xyz.instellate.protonLauncher", "AddGameDialog")
    property var addGame: addGameComponent.createObject(root)

    function openPage(page: Component, properties) {
        if (applicationWindow().pageStack.depth > 1) {
            applicationWindow().pageStack.pop();
        }
        applicationWindow().pageStack.push(page, properties);
    }

    actions: [
        Kirigami.Action {
            text: i18nc("@action:add-game", "Add Game")
            icon.name: "list-add"
            onTriggered: root.addGame.open()
        },
        Kirigami.Action {
            id: openRecent
            text: i18nc("@action:opren-recent", "Recent")
            icon.name: "appointment-new"
            checkable: true
            checked: true
            onTriggered: {
                itemView.currentIndex = -1;
                openRecent.checked = true;
                root.openPage(Qt.createComponent("xyz.instellate.protonLauncher", "Recent"));
            }
        }
    ]

    contentItem: ListView {
        id: itemView
        reuseItems: true

        model: proxyModel

        delegate: Delegates.RoundedItemDelegate {
            id: itemDelegate
            highlighted: ListView.isCurrentItem

            required property GameInfo game
            required property int index

            contentItem: RowLayout {
                Controls.Label {
                    text: itemDelegate.game.name
                }
            }

            onClicked: {
                if (ListView.view.currentIndex === index) {
                    return;
                }

                openRecent.checked = false;
                ListView.view.currentIndex = index;
                root.openPage(Qt.createComponent("xyz.instellate.protonLauncher", "Game"), {
                    game
                });
            }
        }

        Component.onCompleted: {
            itemView.currentIndex = -1;
        }
    }

    // Resets the Delegate values on the form
    Connections {
        target: root.addGame

        function onAccepted() {
            root.addGame.destroy();
            root.addGame = root.addGameComponent.createObject(root);
        }

        function onClosed() {
            root.addGame.destroy();
            root.addGame = root.addGameComponent.createObject(root);
        }
    }

    ListModel {
        id: itemModel
    }

    SortFilterProxyModel {
        id: proxyModel

        model: itemModel
        sorters: [
            RoleSorter {
                roleName: "name"
            }
        ]
    }

    Connections {
        target: GameManager

        function onGamesChanged() {
            itemModel.clear();
            for (const game of GameManager.games) {
                itemModel.append({
                    game,
                    name: game.name
                });
            }
        }
    }

    Component.onCompleted: {
        for (const game of GameManager.games) {
            itemModel.append({
                game,
                name: game.name
            });
        }
    }
}
