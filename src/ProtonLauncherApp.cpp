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

#include "ProtonLauncherApp.h"

#include <KLocalization>
#include <QtLogging>

ProtonLauncherApp::ProtonLauncherApp(QObject *parent) : AbstractKirigamiApplication(parent) {
    setupActions();
}

void ProtonLauncherApp::setupActions() {
    AbstractKirigamiApplication::setupActions();
    QAction *aboutKde = action(QStringLiteral("open_about_kde_page"));
    mainCollection()->removeAction(aboutKde);
}

