/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QtSql>

class PackModel : public QSqlTableModel
{
    Q_OBJECT

public:
    PackModel(QObject* parent = NULL);
    ~PackModel();

    Q_INVOKABLE QString addPack(QString name);
    Q_INVOKABLE void deletePack(int index);

    // Overrides from QSqlQueryModel
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    static const int ObjectRole;
    static const int TextRole;
};
