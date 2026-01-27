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

import QtQml
import QtCore
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    id: root

    required property GameInfo game

    FormCard.FormHeader {
        title: i18nc("@title:section", "Metadata")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            label: i18nc("@label", "Name")
            text: root.game.name

            onTextChanged: {
                if (this.text == root.game.name) {
                    return;
                }
                root.game.name = this.text;
            }
        }

        FormCard.FormFileDelegate {
            label: i18nc("@label", "Game Banner")
            currentFolder: root.game.bannerLocation === null ? StandardPaths.writableLocation(StandardPaths.PicturesLocation) : new URL("file://" + root.game.bannerLocation)

            onAccepted: {
                root.game.bannerLocation = this.selectedFile.toString().replace("file://", "");
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:section", "Launch")
    }

    FormCard.FormCard {
        FormCard.FormComboBoxDelegate {
            text: i18nc("@label", "Proton Version")
            textRole: "name"
            valueRole: "value"
            model: proxyModel

            onActivated: {
                if (root.game.protonPath === this.currentValue) {
                    return;
                }

                root.game.protonPath = this.currentValue;
            }
        }

        FormCard.FormTextFieldDelegate {
            label: i18nc("@label", "Launch Arguments")
            placeholderText: i18n("%command% will be substituded for the run command")
            text: root.game.launchArguments

            onTextChanged: {
                if (this.text === root.game.launchArguments) {
                    return;
                }

                if (this.text.trim() === "") {
                    root.game.launchArguments = null;
                } else {
                    root.game.launchArguments = this.text;
                }
            }
        }
    }

    ListModel {
        id: protonVersionsModel
    }

    SortFilterProxyModel {
        id: proxyModel
        model: protonVersionsModel
        sorters: [
            RoleSorter {
                roleName: "name"
            }
        ]
    }

    Component.onCompleted: {
        const protonInstalls = GameManager.getProtonInstallations();

        for (const name in protonInstalls) {
            const path = protonInstalls[name];
            protonVersionsModel.append({
                name,
                value: path
            });

            if (path === root.game.protonPath) {
                this.currentValue = path;
            }
        }
    }
}
