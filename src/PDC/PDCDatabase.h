/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "QGCToolbox.h"
#include "PackModel.h"

#include <QSqlDatabase>
#include <QSqlQueryModel>

class PDCDatabase : public QGCTool
{
    Q_OBJECT

public:
    PDCDatabase(QGCApplication* app, QGCToolbox* toolbox);
    ~PDCDatabase();

    Q_PROPERTY(PackModel* packModel READ packModel CONSTANT)

    PackModel* packModel(void) { return _packModel; }

    // Override from QGCTool
    virtual void setToolbox(QGCToolbox* toolbox);

private:
    QSqlDatabase    _db;
    PackModel*      _packModel;
};
