/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "QGCQmlWidgetHolder.h"

#include <QQmlContext>
#include <QQmlError>

QGCQmlWidgetHolder::QGCQmlWidgetHolder(const QString& title, QAction* action, QWidget *parent) :
    QWidget(parent)
{
    _ui.setupUi(this);

    layout()->setContentsMargins(0,0,0,0);
    setResizeMode(QQuickWidget::SizeRootObjectToView);
}

QGCQmlWidgetHolder::~QGCQmlWidgetHolder()
{

}
bool QGCQmlWidgetHolder::setSource(const QUrl& qmlUrl)
{
    bool success = _ui.qmlWidget->setSource(qmlUrl);
    for (QQmlError error: _ui.qmlWidget->errors()) {
        qDebug() << "Error" << error.toString();
    }
    return success;
}

void QGCQmlWidgetHolder::setContextPropertyObject(const QString& name, QObject* object)
{
    _ui.qmlWidget->rootContext()->setContextProperty(name, object);
}

QQmlContext* QGCQmlWidgetHolder::getRootContext(void)
{
    return _ui.qmlWidget->rootContext();
}

QQuickItem* QGCQmlWidgetHolder::getRootObject(void)
{
    return _ui.qmlWidget->rootObject();
}

QQmlEngine*	QGCQmlWidgetHolder::getEngine()
{
    return _ui.qmlWidget->engine();
}


void QGCQmlWidgetHolder::setResizeMode(QQuickWidget::ResizeMode resizeMode)
{
    _ui.qmlWidget->setResizeMode(resizeMode);
}
