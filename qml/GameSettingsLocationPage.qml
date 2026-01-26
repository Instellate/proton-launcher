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

import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    required property GameInfo game

    FormCard.FormHeader {
        title: i18nc("@title", "Location")
    }

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Prefix Location")
            description: root.game.prefixLocation
            onClicked: Qt.openUrlExternally(new URL("file://" + root.game.prefixLocation))
        }

        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Executable Location")
            description: root.game.executableLocation
            onClicked: Qt.openUrlExternally(new URL("file://" + root.game.executableLocation))
        }
    }
}
