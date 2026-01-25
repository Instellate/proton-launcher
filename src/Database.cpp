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

#include "Database.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QStandardPaths>
#include <QString>

void Database::initialize() {
    QDir genericConfig = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if (!genericConfig.exists(QStringLiteral("proton-launcher"))) {
        genericConfig.mkdir(QStringLiteral("proton-launcher"));
    }

    genericConfig.cd(QStringLiteral("proton-launcher"));
    QString databasePath = genericConfig.filePath(QStringLiteral("proton-launcher.db"));
    auto database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));

    database.setDatabaseName(databasePath);

    if (!database.open()) {
        qFatal() << "Cannot open database at" << databasePath;
    }

    QSqlQuery(QStringLiteral("CREATE TABLE IF NOT EXISTS migrations(id TEXT PRIMARY KEY)")).exec();

    QSet<QString> set;
    QSqlQuery existingMigrations(QStringLiteral("SELECT id FROM migrations"));
    while (existingMigrations.next()) {
        QString id = existingMigrations.value(QStringLiteral("id")).toString();
        set.insert(id);
    }

    QDir migrationsDir(QStringLiteral(":/resources/migrations"));
    QFileInfoList migrations = migrationsDir.entryInfoList();
    for (const QFileInfo &migration: migrations) {
        QString migrationName = migration.fileName();

        if (set.contains(migrationName)) {
            continue;
        }

        QFile migrationFile{migration.absoluteFilePath()};
        if (!migrationFile.open(QIODeviceBase::ReadOnly)) {
            qFatal() << "Cannot open migration file" << migration.absoluteFilePath();
        }

        QString migrationContent = QString::fromUtf8(migrationFile.readAll());
        for (const QString &migrationString: migrationContent.split(QStringLiteral(";"))) {
            QSqlQuery migrationQuery(migrationString);
            if (!migrationQuery.exec()) {
                qDebug() << migrationQuery.lastError();
            }
        }

        QSqlQuery addMigrationQuery;
        addMigrationQuery.prepare(QStringLiteral("INSERT INTO migrations(id) VALUES (?)"));
        addMigrationQuery.addBindValue(migrationName);
        if (!addMigrationQuery.exec()) {
            qDebug() << addMigrationQuery.lastError();
        }
        qDebug() << "Added migration" << migrationName;
    }
}
