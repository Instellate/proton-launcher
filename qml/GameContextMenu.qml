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
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Addons
import "utils.js" as Utils

Item {
    id: root

    required property GameInfo game

    Component {
        id: contextMenu

        Addons.ConvergentContextMenu {
            Kirigami.Action {
                text: i18nc("@action", "Play")
                icon.source: "media-playback-start"

                visible: !root.game.isRunning
                onTriggered: root.game.start()
            }

            Kirigami.Action {
                text: i18nc("@action", "Stop")
                icon.source: "dialog-cancel"

                visible: root.game.isRunning
                onTriggered: root.game.stop()
            }

            Kirigami.Action {
                text: i18nc("@action", "Configure")
                icon.source: "configure"

                onTriggered: settings.open()
            }

            Kirigami.Action {
                text: i18nc("@action:inmenu", "Delete")
                icon.source: "delete"

                onTriggered: deleteDialog.open()
            }
        }
    }

    GameSettings {
        id: settings

        game: root.game
        window: applicationWindow().applicationWindow
    }

    FormCard.FormCardDialog {
        id: deleteDialog
        title: i18nc("@title:window", "Delete Game")

        FormCard.FormHeader {
            title: i18nc("@action:description", "Are you sure you want to remove this game?")
        }

        FormCard.FormCheckDelegate {
            id: removeGameFolder
            text: i18nc("@action:label", "Remove the game folder as well")

            checked: false
        }

        standardButtons: Controls.Dialog.Yes | Controls.Dialog.Cancel

        onAccepted: {
            const lastPage = applicationWindow().pageStack.lastItem;
            if (lastPage instanceof Game && lastPage.game.id === root.game.id) {
                Utils.openPage(Qt.resolvedUrl("Recent.qml"));
            }

            GameManager.removeGame(root.game.id, removeGameFolder.checked);
        }
    }

    function open() {
        const item = contextMenu.createObject(Controls.Overlay.overlay);
        item.popup();
    }
}
