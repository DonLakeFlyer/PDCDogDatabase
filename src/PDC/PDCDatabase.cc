/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "PDCDatabase.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QQmlEngine>
#include <QtQml>

PDCDatabase::PDCDatabase(QGCApplication *app, QGCToolbox* toolbox)
    : QGCTool   (app, toolbox)
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("PDC.db");
    if (!_db.open()) {
        qWarning() << "Open failed" << _db.lastError().driverText() << _db.lastError().databaseText();
    }

    for (const QString& table: _db.tables()) {
        qDebug() << table;
    }

    QSqlQuery query("SELECT * FROM Packs");
    int idName = query.record().indexOf("Name");
    while (query.next())
    {
       QString name = query.value(idName).toString();
       qDebug() << name;
    }
}

PDCDatabase::~PDCDatabase()
{
    _db.close();
}

void PDCDatabase::setToolbox(QGCToolbox *toolbox)
{
    QGCTool::setToolbox(toolbox);

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    qmlRegisterUncreatableType<PDCDatabase>("QGroundControl.PDCDatabase", 1, 0, "PDCDatabase", "Reference only");
}
