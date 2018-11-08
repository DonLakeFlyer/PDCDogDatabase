/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/// @file
///     @author Don Gagne <don@thegagnes.com>

#include "QGroundControlQmlGlobal.h"

#include <QSettings>
#include <QLineF>
#include <QPointF>

static const char* kQmlGlobalKeyName = "QGCQml";

const char* QGroundControlQmlGlobal::_flightMapPositionSettingsGroup =          "FlightMapPosition";
const char* QGroundControlQmlGlobal::_flightMapPositionLatitudeSettingsKey =    "Latitude";
const char* QGroundControlQmlGlobal::_flightMapPositionLongitudeSettingsKey =   "Longitude";
const char* QGroundControlQmlGlobal::_flightMapZoomSettingsKey =                "FlightMapZoom";

QGeoCoordinate   QGroundControlQmlGlobal::_coord = QGeoCoordinate(0.0,0.0);
double           QGroundControlQmlGlobal::_zoom = 2;
QGroundControlQmlGlobal::QGroundControlQmlGlobal(QGCApplication* app, QGCToolbox* toolbox)
    : QGCTool               (app, toolbox)
    , _flightMapInitialZoom (17.0)
    , _mapEngineManager     (NULL)
    , _qgcPositionManager   (NULL)
    , _corePlugin           (NULL)
    , _settingsManager      (NULL)
    , _pdcDatabase          (NULL)
{
    // We clear the parent on this object since we run into shutdown problems caused by hybrid qml app. Instead we let it leak on shutdown.
    setParent(NULL);
    // Load last coordinates and zoom from config file
    QSettings settings;
    settings.beginGroup(_flightMapPositionSettingsGroup);
    _coord.setLatitude(settings.value(_flightMapPositionLatitudeSettingsKey,    _coord.latitude()).toDouble());
    _coord.setLongitude(settings.value(_flightMapPositionLongitudeSettingsKey,  _coord.longitude()).toDouble());
    _zoom = settings.value(_flightMapZoomSettingsKey, _zoom).toDouble();
}

QGroundControlQmlGlobal::~QGroundControlQmlGlobal()
{
    // Save last coordinates and zoom to config file
    QSettings settings;
    settings.beginGroup(_flightMapPositionSettingsGroup);
    settings.setValue(_flightMapPositionLatitudeSettingsKey, _coord.latitude());
    settings.setValue(_flightMapPositionLongitudeSettingsKey, _coord.longitude());
    settings.setValue(_flightMapZoomSettingsKey, _zoom);
}

void QGroundControlQmlGlobal::setToolbox(QGCToolbox* toolbox)
{
    QGCTool::setToolbox(toolbox);

    _mapEngineManager       = toolbox->mapEngineManager();
    _qgcPositionManager     = toolbox->qgcPositionManager();
    _corePlugin             = toolbox->corePlugin();
    _settingsManager        = toolbox->settingsManager();
    _pdcDatabase            = toolbox->pdcDatabase();
}

void QGroundControlQmlGlobal::saveGlobalSetting (const QString& key, const QString& value)
{
    QSettings settings;
    settings.beginGroup(kQmlGlobalKeyName);
    settings.setValue(key, value);
}

QString QGroundControlQmlGlobal::loadGlobalSetting (const QString& key, const QString& defaultValue)
{
    QSettings settings;
    settings.beginGroup(kQmlGlobalKeyName);
    return settings.value(key, defaultValue).toString();
}

void QGroundControlQmlGlobal::saveBoolGlobalSetting (const QString& key, bool value)
{
    QSettings settings;
    settings.beginGroup(kQmlGlobalKeyName);
    settings.setValue(key, value);
}

bool QGroundControlQmlGlobal::loadBoolGlobalSetting (const QString& key, bool defaultValue)
{
    QSettings settings;
    settings.beginGroup(kQmlGlobalKeyName);
    return settings.value(key, defaultValue).toBool();
}

bool QGroundControlQmlGlobal::linesIntersect(QPointF line1A, QPointF line1B, QPointF line2A, QPointF line2B)
{
    QPointF intersectPoint;

    return QLineF(line1A, line1B).intersect(QLineF(line2A, line2B), &intersectPoint) == QLineF::BoundedIntersection &&
            intersectPoint != line1A && intersectPoint != line1B;
}

void QGroundControlQmlGlobal::setFlightMapPosition(QGeoCoordinate& coordinate)
{
    if (coordinate != flightMapPosition()) {
        _coord.setLatitude(coordinate.latitude());
        _coord.setLongitude(coordinate.longitude());

        emit flightMapPositionChanged(coordinate);
    }
}

void QGroundControlQmlGlobal::setFlightMapZoom(double zoom)
{
    if (zoom != flightMapZoom()) {
        _zoom = zoom;
        emit flightMapZoomChanged(zoom);
    }
}
