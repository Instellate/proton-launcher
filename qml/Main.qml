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
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import org.kde.kirigamiaddons.formcard as FormCard

StatefulApp.StatefulWindow {
    id: root

    width: 800
    height: 600

    windowName: i18nc("@title:window", "Proton Launcher")

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: [
            Kirigami.Action {
                fromQAction: root.application.action("options_configure")
            },
            Kirigami.Action {
                fromQAction: root.application.action("options_configure_keybinding")
            },
            Kirigami.Action {
                separator: true
            },
            Kirigami.Action {
                text: i18n("About Proton Launcher")
                icon.name: "help-about"
                onTriggered: {
                    root.pageStack.pushDialogLayer(aboutPage, {
                        width: root.width
                    }, {
                        width: Kirigami.Units.gridUnit * 30,
                        height: Kirigami.Units.gridUnit * 30,
                        title: i18nc("@title:window", "About Proton Launcher")
                    });
                }
            },
            Kirigami.Action {
                fromQAction: root.application.action("file_quit")
            }
        ]
    }

    application: ProtonLauncherApp {
        configurationView: Settings {}
    }

    pageStack.initialPage: [Qt.resolvedUrl("Library.qml"), Qt.resolvedUrl("Recent.qml")]
    pageStack.defaultColumnWidth: 14 * Kirigami.Units.gridUnit

    Component {
        id: aboutPage

        FormCard.AboutPage {
            donateUrl: "http://github.com/sponsors/Instellate"
            getInvolvedUrl: "https://github.com/Instellate/proton-launcher"
        }
    }

    onClosing: {
        GameManager.saveSettings();
    }
}
