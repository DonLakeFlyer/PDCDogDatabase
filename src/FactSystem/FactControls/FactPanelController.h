/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#ifndef FactPanelController_H
#define FactPanelController_H

/// @file
///     @author Don Gagne <don@thegagnes.com>

#include <QObject>
#include <QQuickItem>

#include "QGCLoggingCategory.h"
#include "Fact.h"

Q_DECLARE_LOGGING_CATEGORY(FactPanelControllerLog)

/// FactPanelController is used in combination with the FactPanel Qml control for handling
/// missing Facts from C++ code.
class FactPanelController : public QObject
{
    Q_OBJECT

public:
    /// @param standaloneUnitTesting true: being run without factPanel, false: normal operation, factPanel is required
    FactPanelController(bool standaloneUnitTesting = false);

    Q_PROPERTY(QQuickItem* factPanel READ factPanel WRITE setFactPanel)

    QQuickItem* factPanel(void);
    void setFactPanel(QQuickItem* panel);

private slots:
    void _checkForMissingFactPanel(void);

private:
    void _notifyPanelErrorMsg(const QString& errorMsg);
    void _showInternalError(const QString& errorMsg);

    QQuickItem*         _factPanel;
};

#endif
