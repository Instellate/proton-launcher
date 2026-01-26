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

RowLayout {
    id: root
    spacing: 0

    required property string title
    required property string description
    property var iconSource: null

    Layout.leftMargin: Kirigami.Units.largeSpacing

    Kirigami.Icon {
        source: root.iconSource
        visible: root.iconSource !== null
        Layout.rightMargin: root.iconSource !== null ? Kirigami.Units.smallSpacing : 0
        implicitWidth: root.iconSource !== null ? Kirigami.Units.iconSizes.large : 0
        implicitHeight: root.iconSource !== null ? Kirigami.Units.iconSizes.large : 0
    }

    ColumnLayout {
        spacing: 0

        Controls.Label {
            text: root.title
            elide: Text.ElideRight
        }

        Controls.Label {
            text: root.description
            elide: Text.ElideRight
            color: Kirigami.Theme.disabledTextColor
        }
    }
}
