# Copyright (C) 2026 Instellate
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 4.1)
project(proton-launcher VERSION "0.0.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMSetupVersion)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX PROTON_LAUNCHER
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/proton-launcher-version.h)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Quick
    Gui
    QuickControls2
    Widgets
    Sql
)

find_package(KF6 REQUIRED COMPONENTS
    Kirigami
    KirigamiAddons
    I18n
    CoreAddons
    QQC2DesktopStyle
    IconThemes
    Config
    Crash
)

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.formcard REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.settings REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.delegates REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.statefulapp REQUIRED)

ki18n_install(po)

qt_add_resources(PROTON_LAUNCHER_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

add_executable(proton-launcher)

ecm_add_qml_module(proton-launcher URI xyz.instellate.protonLauncher)

target_sources(proton-launcher PRIVATE 
    src/main.cpp 
    src/ProtonLauncherApp.cpp src/ProtonLauncherApp.h
    src/Database.cpp src/Database.h
    src/GameInfo.cpp src/GameInfo.h
    src/GameManager.cpp src/GameManager.cpp
    ${PROTON_LAUNCHER_RESOURCES}
)

ecm_target_qml_sources(proton-launcher
    SOURCES
        qml/Main.qml
        qml/Library.qml
        qml/Game.qml
        qml/GameInfoCard.qml
        qml/GameSettings.qml
        qml/GameSettingsGeneralPage.qml
        qml/AddGameDialog.qml
        qml/Recent.qml
        qml/Settings.qml
)

target_link_libraries(proton-launcher
    PRIVATE
    Qt::Core
    Qt::Quick
    Qt::Gui
    Qt::QuickControls2
    Qt::Widgets
    Qt::Sql
    KF6::I18n
    KF6::CoreAddons
    KF6::IconThemes
    KF6::ConfigGui
    KF6::Crash
    KirigamiAddonsStatefulApp
)

target_include_directories(proton-launcher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_options(proton-launcher PRIVATE -Wall -Wextra)

install(TARGETS proton-launcher ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS xyz.instellate.proton-launcher.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES 
    icons/xyz.instellate.proton-launcher.svg 
    DESTINATION ${KDE_INSTALL_FULL_DATADIR}/icons/hicolor/scalable/apps/)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
