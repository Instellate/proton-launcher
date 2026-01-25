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

#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedString>
#include <KConfig>
#include <KSharedConfig>
#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QString>

#include "proton-launcher-version.h"
#include "Database.h"

int main(int argc, char *argv[]) {
    KIconTheme::initTheme();
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("proton-launcher");
    QApplication::setOrganizationName(QStringLiteral("Instellate"));
    QApplication::setOrganizationDomain(QStringLiteral("instellate.xyz"));
    QApplication::setApplicationName(QStringLiteral("Proton Launcher"));
    QApplication::setDesktopFileName(QStringLiteral("xyz.instellate.proton-launcher"));

    QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("xyz.instellate.protonLauncher")));

    KAboutData about(
            QStringLiteral("proton-launcher"),
            QStringLiteral("Proton Launcher"),
            QStringLiteral(PROTON_LAUNCHER_VERSION_STRING),
            i18n("Game library to manage and run applicatios that requires Proton"),
            KAboutLicense::GPL_V3,
            i18n("© 2026 Instellate"));
    about.addAuthor(
            QStringLiteral("Instellate"),
            i18n("Maintainer"),
            QStringLiteral("instellate@instellate.xyz"),
            QStringLiteral("https://instellate.xyz"),
            QUrl(QStringLiteral("https://avatars.githubusercontent.com/u/63735043?v=4")));
    about.setBugAddress(QStringLiteral("https://github.com/Instellate/proton-launcher/issues").toUtf8());

    KAboutData::setApplicationData(about);

    Database::initialize();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext{&engine});
    engine.loadFromModule("xyz.instellate.protonLauncher", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QCoreApplication::exec();
}
