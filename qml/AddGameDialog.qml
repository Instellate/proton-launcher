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

import QtCore
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: root
    title: i18nc("@title:window", "Add Game")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    FormCard.FormTextFieldDelegate {
        id: nameField
        label: i18nc("@label:textbox", "Name")
        onAccepted: executableLocationField.forceActiveFocus()
    }

    FormCard.FormFileDelegate {
        id: executableLocationField
        label: i18nc("@label:file-location", "Game Executable")
        currentFolder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
        onAccepted: moveGameField.forceActiveFocus()
        acceptLabel: i18nc("@label:select-file", "Select")
    }

    FormCard.FormSwitchDelegate {
        id: moveGameField
        checked: false
        text: i18nc("@label:checkbox", "Move game folder to library")
    }

    onAccepted: {
        GameManager.addGame(nameField.text, executableLocationField.selectedFile, moveGameField.checked);
    }
}
