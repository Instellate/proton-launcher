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
import org.kde.kirigamiaddons.components as Addons
import "utils.js" as Utils

Kirigami.ScrollablePage {
    id: root
    title: i18nc("@title", "Library")

    property Component addGameComponent: Qt.createComponent("xyz.instellate.protonLauncher", "AddGameDialog")
    property var addGame: addGameComponent.createObject(root)

    actions: [
        Kirigami.Action {
            id: openRecent

            text: i18nc("@action:opren-recent", "Recent")
            icon.name: "appointment-new"
            checkable: true
            checked: true
            onTriggered: {
                itemView.currentIndex = -1;
                Utils.openPage(Qt.resolvedUrl("Recent.qml"));
            }
        },
        Kirigami.Action {
            text: i18nc("@action:add-game", "Add Game")
            icon.name: "list-add"
            onTriggered: root.addGame.open()
        }
    ]

    contentItem: ListView {
        id: itemView
        // reuseItems: true

        model: proxyModel

        delegate: MouseArea {
            id: itemDelegate

            required property GameInfo game
            required property int index

            implicitHeight: innerDelegate.implicitHeight
            implicitWidth: innerDelegate.implicitWidth
            width: innerDelegate.width

            acceptedButtons: Qt.RightButton

            Delegates.RoundedItemDelegate {
                id: innerDelegate

                highlighted: ListView.isCurrentItem
                gridView: itemDelegate.GridView
                listView: itemDelegate.ListView

                contentItem: RowLayout {
                    spacing: Kirigami.Units.largeSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing

                    Addons.Avatar {
                        source: itemDelegate.game.iconLocation ? "file://" + itemDelegate.game.iconLocation : null // qmllint disable
                        imageMode: Addons.Avatar.AdaptiveImageOrInitals
                        initialsMode: Addons.Avatar.UseInitials

                        name: itemDelegate.game.name
                        implicitWidth: Kirigami.Units.iconSizes.small
                        implicitHeight: Kirigami.Units.iconSizes.small
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft
                        text: itemDelegate.game.name
                    }
                }

                onClicked: {
                    if (itemDelegate.ListView.view.currentIndex === itemDelegate.index) {
                        return;
                    }

                    itemDelegate.ListView.view.currentIndex = itemDelegate.index;
                    Utils.openPage(Qt.resolvedUrl("Game.qml"), {
                        game: itemDelegate.game
                    });
                }
            }

            GameContextMenu {
                id: contextMenu
                game: itemDelegate.game
            }

            onClicked: contextMenu.open()
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

    Connections {
        target: applicationWindow().pageStack

        function onPagePushed(page) {
            openRecent.checked = page instanceof Recent;
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
