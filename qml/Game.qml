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
                description: "Aug 23, 2024"
                iconSource: "clock"
            }

            GameInfoCard {
                title: i18nc("@label", "Amount played")
                description: "7.7 hours"
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
            }
        }
    }

    Controls.Button {
        y: Kirigami.Units.gridUnit * root.bannerLength - height - Kirigami.Units.gridUnit * 0.5
        x: Kirigami.Units.gridUnit * 0.5

        text: i18nc("@action", "Play")
        icon.source: "media-playback-start"
    }

    GameSettings {
        id: settings

        game: root.game
        window: applicationWindow().applicationWindow
    }
}
