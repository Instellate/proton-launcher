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

import org.kde.kirigami as Kirigami
import QtQuick.Controls as Controls
import QtQuick

Kirigami.Page {
    title: i18nc("@title", "Recently Played Games")

    Controls.Label {
        visible: GameManager.games.length == 0
        text: i18nc("recently-no-played-games", "You haven't played any games yet")
        font.pixelSize: Kirigami.Units.gridUnit

        anchors.centerIn: parent
    }
}
