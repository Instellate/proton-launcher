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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import "utils.js" as Utils

Kirigami.Page {
    id: root

    title: game.name
    padding: 0

    required property GameInfo game
    readonly property double bannerLength: 17.5

    actions: [
        Kirigami.Action {
            text: i18nc("@action:settings", "Settings")
            icon.name: "configure"
            onTriggered: settings.open()
        }
    ]

    ColumnLayout {
        anchors.fill: parent

        spacing: Kirigami.Units.largeSpacing

        Image {
            source: "file://" + game.bannerLocation
            fillMode: Image.PreserveAspectCrop
            clip: true
            smooth: true

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * root.bannerLength
            sourceClipRect.height: Kirigami.Units.gridUnit * root.bannerLength
        }

        RowLayout {
            spacing: Kirigami.Units.gridUnit

            GameInfoCard {
                title: i18nc("@label", "Last Played")
                description: !root.game.lastPlayed ? i18n("Never played") : Qt.formatDateTime(root.game.lastPlayed, "MMM, d, yyyy")
                iconSource: "clock"
            }

            GameInfoCard {
                title: i18nc("@label", "Amount played")
                description: Utils.formatDuration(root.game.playTime)
            }
        }

        ColumnLayout {
            spacing: Kirigami.Units.mediumSpacing

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: Kirigami.Units.gridUnit
            Layout.rightMargin: Kirigami.Units.gridUnit
            Layout.bottomMargin: Kirigami.Units.gridUnit

            Controls.Label {
                text: i18nc("@label", "Console")
            }

            Controls.Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Flickable {
                    clip: true
                    anchors.fill: parent

                    Controls.ScrollBar.vertical: Controls.ScrollBar {}
                    Controls.ScrollBar.horizontal: Controls.ScrollBar {}

                    contentWidth: consoleText.width
                    contentHeight: consoleText.height
                    contentY: contentHeight - height

                    property bool atBottom: contentY >= (contentHeight - height - 1)
                    property bool lessThan: contentHeight <= height + 1

                    onContentHeightChanged: {
                        if (lessThan) {
                            contentY = contentHeight - height;
                        } else if (atBottom) {
                            contentY = contentHeight - height;
                        }
                    }

                    Controls.Label {
                        id: consoleText
                        text: root.game.consoleLog
                    }
                }
            }
        }
    }

    Controls.Button {
        y: Kirigami.Units.gridUnit * root.bannerLength - height - Kirigami.Units.gridUnit * 0.5
        x: Kirigami.Units.gridUnit * 0.5

        text: i18nc("@action", "Play")
        icon.source: "media-playback-start"
        enabled: !game.isRunning

        onClicked: {
            game.start();
        }
    }

    Controls.Button {
        y: Kirigami.Units.gridUnit * root.bannerLength - height - Kirigami.Units.gridUnit * 0.5
        x: Kirigami.Units.gridUnit * 0.5

        text: i18nc("@action", "Stop")
        icon.source: "gtk-stop"
        visible: game.isRunning

        onClicked: {
            game.stop();
        }
    }

    GameSettings {
        id: settings

        game: root.game
        window: applicationWindow().applicationWindow
    }
}
