/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "FactPanelController.h"
#include "QGCApplication.h"

#include <QQmlEngine>

/// @file
///     @author Don Gagne <don@thegagnes.com>

QGC_LOGGING_CATEGORY(FactPanelControllerLog, "FactPanelControllerLog")

FactPanelController::FactPanelController(bool standaloneUnitTesting)
    : _factPanel(NULL)
{
    if (!standaloneUnitTesting) {
        // Do a delayed check for the _factPanel finally being set correctly from Qml
        QTimer::singleShot(1000, this, &FactPanelController::_checkForMissingFactPanel);
    }
}

QQuickItem* FactPanelController::factPanel(void)
{
    return _factPanel;
}

void FactPanelController::setFactPanel(QQuickItem* panel)
{
    // Once we finally have the _factPanel member set, send any
    // missing fact notices that were waiting to go out

    _factPanel = panel;
}

void FactPanelController::_notifyPanelErrorMsg(const QString& errorMsg)
{
    if (_factPanel) {
        QVariant returnedValue;

        QMetaObject::invokeMethod(_factPanel,
                                  "showError",
                                  Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, errorMsg));
    }
}

void FactPanelController::_checkForMissingFactPanel(void)
{
    if (!_factPanel) {
        _showInternalError(tr("Incorrect FactPanel Qml implementation. FactPanelController used without passing in factPanel."));
    }
}

void FactPanelController::_showInternalError(const QString& errorMsg)
{
    _notifyPanelErrorMsg(tr("Internal Error: %1").arg(errorMsg));
    qCWarning(FactPanelControllerLog) << "Internal Error" << errorMsg;
    qgcApp()->showMessage(errorMsg);
}
