/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "QGCLoggingCategory.h"
#include "QGCToolbox.h"
#include "AppSettings.h"
#include "UnitsSettings.h"
#include "FlightMapSettings.h"
#include <QVariantList>

/// Provides access to all app settings
class SettingsManager : public QGCTool
{
    Q_OBJECT
    
public:
    SettingsManager(QGCApplication* app, QGCToolbox* toolbox);

    Q_PROPERTY(QObject* appSettings         READ appSettings            CONSTANT)
    Q_PROPERTY(QObject* unitsSettings       READ unitsSettings          CONSTANT)
    Q_PROPERTY(QObject* flightMapSettings   READ flightMapSettings      CONSTANT)

    // Override from QGCTool
    virtual void setToolbox(QGCToolbox *toolbox);

    AppSettings*            appSettings         (void) { return _appSettings; }
    UnitsSettings*          unitsSettings       (void) { return _unitsSettings; }
    FlightMapSettings*      flightMapSettings   (void) { return _flightMapSettings; }

private:
    AppSettings*            _appSettings;
    UnitsSettings*          _unitsSettings;
    FlightMapSettings*      _flightMapSettings;
};
