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
import org.kde.kirigamiaddons.settings as KSettings
import org.kde.kirigamiaddons.formcard as FormCard

KSettings.ConfigurationView {
    id: root

    modules: [
        KSettings.ConfigurationModule {
            moduleId: "General"
            page: () => root.general
            text: i18nc("@action:button", "General")
        }
    ]

    readonly property Component general: FormCard.FormCardPage {
        id: general

        FormCard.FormHeader {
            title: i18nc("@title:section", "Game Launch")
        }

        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                text: i18nc("@label", "Default Proton version")
                textRole: "name"
                valueRole: "value"
                model: proxyModel

                onActivated: {
                    Config.defaultProtonVersion = currentValue;
                }

                Component.onCompleted: {
                    const protonInstalls = GameManager.getProtonInstallations();

                    for (const name in protonInstalls) {
                        const path = protonInstalls[name];
                        protonVersionsModel.append({
                            name,
                            value: path
                        });

                        if (path === Config.defaultProtonVersion) {
                            this.currentValue = path;
                        }
                    }
                }
            }

            FormCard.FormTextFieldDelegate {
                label: i18nc("@label", "Default Launch Arguments")
                placeholderText: i18n("%command% will be substituted with the run command")
                text: Config.defaultLaunchArguments

                onTextChanged: Config.defaultLaunchArguments = text
            }

            FormCard.FormButtonDelegate {
                id: protonGeVersion

                property var currentVersion: null

                text: i18nc("@action:button", "Check ProtonGE Version")
                onClicked: downloader.getProtonGeVersion()
                enabled: !downloader.processing

                description: currentVersion !== null ? i18nc("@action:description", "Current latest ProtonGE version: %1", currentVersion) : null
            }

            ProtonDownloader {
                id: downloader

                onFoundProtonGeVersion: function (version, isOutdated) {
                    console.log(version);

                    if (isOutdated) {
                        downloadProtonGe.protonGeVersion = version;
                        downloadProtonGe.open();
                    } else {
                        protonGeVersion.currentVersion = version;
                    }
                }
            }

            DownloadProtonGeDialog {
                id: downloadProtonGe

                protonGeVersion: null
                parent: general

                onAccepted: {
                    console.log("User wants to download latest version");
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
    }
}
