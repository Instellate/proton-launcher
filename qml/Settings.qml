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
import QtQml
import QtQuick
import org.kde.ki18n
import org.kde.kirigamiaddons.settings as KSettings
import org.kde.kirigamiaddons.formcard as FormCard

KSettings.ConfigurationView {
    id: root

    readonly property Component general: FormCard.FormCardPage {
        id: general

        FormCard.FormHeader {
            title: KI18n.i18nc("@title:section", "Game Launch")
        }

        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                model: proxyModel
                text: KI18n.i18nc("@label", "Default Proton version")
                textRole: "name"
                valueRole: "value"

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
                onActivated: {
                    Config.defaultProtonVersion = currentValue;
                }
            }

            FormCard.FormTextFieldDelegate {
                label: KI18n.i18nc("@label", "Default Launch Arguments")
                placeholderText: KI18n.i18n("%command% will be substituted with the run command")
                text: Config.defaultLaunchArguments

                onTextChanged: Config.defaultLaunchArguments = text
            }

            FormCard.FormButtonDelegate {
                id: protonGeVersion

                property var currentVersion: null

                description: {
                    if (downloader.error !== null) {
                        return downloader.error;
                    }

                    if (downloader.totalAmount !== 0) {
                        return KI18n.i18nc("@action:description:Amount downloaded", "%1 out of %2 downloaded", Qt.locale().formattedDataSize(downloader.amountDownloaded), Qt.locale().formattedDataSize(downloader.totalAmount));
                    }

                    if (downloader.extracting) {
                        return KI18n.i18nc("@action:description:Extracting ProtonGE", "Extracting files from archive");
                    }

                    if (currentVersion !== null) {
                        return KI18n.i18nc("@action:description", "Already on latest version %1", currentVersion);
                    }

                    return "";
                }
                enabled: !downloader.processing
                text: KI18n.i18nc("@action:button", "Download latest ProtonGE version")

                onClicked: downloader.getProtonGeVersion()
            }

            ProtonDownloader {
                id: downloader

                onFoundProtonGeVersion: function (version, isOutdated) {
                    if (isOutdated) {
                        protonGeVersion.currentVersion = null;

                        downloadProtonGe.protonGeVersion = version;
                        downloadProtonGe.open();
                    } else {
                        protonGeVersion.currentVersion = version;
                    }
                }
            }

            DownloadProtonDialog {
                id: downloadProtonGe

                parent: general
                protonGeVersion: null

                onAccepted: {
                    downloader.downloadProtonGe();
                }
            }
        }

        FormCard.FormHeader {
            title: KI18n.i18nc("@title:section", "Proton Installations")
            
        }

        FormCard.FormCard {
            FormCard.FormFolderDelegate {
                id: addProtonInstallation

                label: KI18n.i18nc("@action:add-proton-installation", "Add a new folder containing Proton installations")
                currentFolder: StandardPaths.writableLocation(StandardPaths.HomeLocation)

                onAccepted: {
                    const fileUrl = new URL(addProtonInstallation.selectedFolder);
                    if (fileUrl.protocol === "file:") {
                        Config.protonInstallationPaths.push(fileUrl.pathname);
                    }
                }
            }

            FormCard.FormSectionText {
                visible: Config.protonInstallationPaths.length > 0
                text: KI18n.i18nc("@action:remove-proton-installation", "Press a path to remove it")
            }

            Repeater {
                model: Config.protonInstallationPaths

                FormCard.FormButtonDelegate {
                    required property string modelData
                    required property int index

                    text: modelData
                    onClicked: Config.protonInstallationPaths.splice(index, 1)
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

    modules: [
        KSettings.ConfigurationModule {
            moduleId: "General"
            page: () => root.general
            text: KI18n.i18nc("@action:button", "General")
        }
    ]
}
