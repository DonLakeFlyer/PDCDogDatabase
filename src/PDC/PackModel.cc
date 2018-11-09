/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "PackModel.h"

#include <QDebug>
#include <QQmlEngine>
#include <QtSql>

const int PackModel::ObjectRole = Qt::UserRole;
const int PackModel::TextRole = Qt::UserRole + 1;

PackModel::PackModel(QObject* parent)
    : QSqlTableModel(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    qmlRegisterUncreatableType<PackModel>("QGroundControl.PDCDatabase", 1, 0, "PackModel", "Reference only");

    setTable("Packs");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    setHeaderData(0, Qt::Horizontal, tr("Name"));
}

PackModel::~PackModel()
{
    
}

QHash<int, QByteArray> PackModel::roleNames() const
{
   QHash<int, QByteArray> roles;
   // record() returns an empty QSqlRecord
   for (int i = 0; i < this->record().count(); i ++) {
       roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toLower().toUtf8());
   }
   return roles;
}

QVariant PackModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    if (index.isValid()) {
        if (role < Qt::UserRole) {
            value = QSqlQueryModel::data(index, role);
        } else {
            int columnIdx = role - Qt::UserRole - 1;
            QModelIndex modelIndex = this->index(index.row(), columnIdx);
            value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
        }
    }
    return value;
}

QString PackModel::addPack(QString name)
{
    QSqlRecord newRecord = record();
    newRecord.setValue("Name", name);

    if (!insertRecord(rowCount(), newRecord)) {
        return "Add failed";
    }

    submitAll();

    return QString();
}

void PackModel::deletePack(int index)
{
    if (!removeRows(index, 1)) {
        qDebug() << "removeRows failed";
    }
    submitAll();
}
