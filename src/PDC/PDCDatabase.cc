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
    : QGCTool(app, toolbox)
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("PDC.db");
    if (!_db.open()) {
        qWarning() << "Open failed" << _db.lastError().driverText() << _db.lastError().databaseText();
    }
    _packModel = new PackModel(this);
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
