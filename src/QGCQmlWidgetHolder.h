/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "ui_QGCQmlWidgetHolder.h"

namespace Ui {
class QGCQmlWidgetHolder;
}

/// This is used to create widgets which are implemented in QML.

class QGCQmlWidgetHolder : public QWidget
{
    Q_OBJECT

public:
    // This has a title and action since the base class is QGCDockWidget. In order to use this
    // control as a normal QWidget, not a doc widget just pass in:
    //      title = QString()
    //      action = NULL
    explicit QGCQmlWidgetHolder(const QString& title, QAction* action, QWidget *parent = Q_NULLPTR);
    ~QGCQmlWidgetHolder();

    /// Get Root Context
    QQmlContext* getRootContext(void);

    /// Get Root Object
    QQuickItem* getRootObject(void);

    /// Get QML Engine
    QQmlEngine*	getEngine();

    /// Sets the QML into the control. Will display errors message box if error occurs loading source.
    /// @return true: source loaded, false: source not loaded, errors occurred
    bool setSource(const QUrl& qmlUrl);

    void setContextPropertyObject(const QString& name, QObject* object);

    /// Sets the resize mode for the QQuickWidget container
    void setResizeMode(QQuickWidget::ResizeMode resizeMode);

private:
    Ui::QGCQmlWidgetHolder _ui;
};
