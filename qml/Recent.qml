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
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import "utils.js" as Utils

Kirigami.ScrollablePage {
    id: root
    title: i18nc("@title", "Recently Played Games")

    Flow {
        spacing: Kirigami.Units.largeSpacing
        width: parent.width
        height: parent.height

        Repeater {
            model: proxyModel

            Kirigami.Card {
                id: card
                required property GameInfo game
                required property int index

                width: Kirigami.Units.gridUnit * 20

                banner {
                    source: "file://" + card.game.bannerLocation
                    implicitHeight: Kirigami.Units.gridUnit * 7.5
                    title: game.name
                    titleAlignment: Qt.AlignLeft | Qt.AlignBottom
                }

                onClicked: {
                    Utils.openPage(Qt.resolvedUrl("Game.qml"), {
                        game: card.game
                    });
                }

                actions: [
                    Kirigami.Action {
                        visible: !card.game.isRunning
                        text: "Start game"
                        icon.source: "media-playback-start"
                        onTriggered: card.game.start()
                    },
                    Kirigami.Action {
                        visible: card.game.isRunning
                        text: "Stop game"
                        icon.source: "gtk-stop"
                        onTriggered: card.game.stop()
                    }
                ]

                contentItem: Item {
                    implicitHeight: row.implicitHeight

                    Row {
                        id: row
                        anchors.left: parent.left
                        anchors.leftMargin: Kirigami.Units.largeSpacing

                        Column {
                            spacing: 0
                            anchors.verticalCenter: parent.verticalCenter

                            Controls.Label {
                                text: i18nc("@label", "Last  Played")
                                elide: Text.ElideRight
                            }

                            Controls.Label {
                                text: Qt.formatDateTime(card.game.lastPlayed, "MMM, d, yyyy")
                                color: Kirigami.Theme.disabledTextColor
                            }
                        }
                    }

                    Row {
                        anchors.right: parent.right
                        anchors.rightMargin: Kirigami.Units.largeSpacing

                        Rectangle {
                            implicitHeight: Kirigami.Units.iconSizes.large
                            implicitWidth: 1
                            color: Kirigami.Theme.backgroundColor
                        }

                        Column {
                            spacing: 0
                            anchors.verticalCenter: parent.verticalCenter

                            Controls.Label {
                                text: i18nc("@label", "Amount Played")
                                elide: Text.ElideRight
                            }

                            Controls.Label {
                                text: Utils.formatDuration(card.game.playTime)
                                color: Kirigami.Theme.disabledTextColor
                                anchors.right: parent.right
                            }
                        }
                    }

                    Connections {
                        target: card.game

                        function onLastPlayedChanged() {
                            itemModel.set(card.index, {
                                game: card.game,
                                lastPlayed: new Date(card.game.lastPlayed)
                            });
                        }
                    }
                }
            }
        }
    }

    Controls.Label {
        visible: itemModel.count == 0
        text: i18n("You haven't played any games yet")
        font.pixelSize: Kirigami.Units.gridUnit

        anchors.centerIn: parent
    }

    ListModel {
        id: itemModel
    }

    SortFilterProxyModel {
        id: proxyModel
        model: itemModel

        sorters: [
            RoleSorter {
                roleName: "lastPlayed"
                sortOrder: Qt.DescendingOrder
            }
        ]
    }

    Component.onCompleted: {
        for (const game of GameManager.games) {
            if (game.lastPlayed) {
                itemModel.append({
                    game,
                    lastPlayed: new Date(game.lastPlayed)
                });
            }
        }
    }

    Connections {
        target: GameManager

        function onGamesChanged() {
            itemModel.clear();
            for (const game of GameManager.games) {
                itemModel.append({
                    game,
                    lastPlayed: Number(game.lastPlayed.value)
                });
            }
        }
    }
}
