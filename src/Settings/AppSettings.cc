/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AppSettings.h"
#include "QGCPalette.h"
#include "QGCApplication.h"

#include <QQmlEngine>
#include <QtQml>
#include <QStandardPaths>

const char* AppSettings::name =                                         "App";
const char* AppSettings::settingsGroup =                                ""; // settings are in root group

const char* AppSettings::appFontPointSizeName =                         "BaseDeviceFontPointSize";
const char* AppSettings::indoorPaletteName =                            "StyleIsDark";
const char* AppSettings::savePathName =                                 "SavePath";
const char* AppSettings::mapboxTokenName =                              "MapboxToken";
const char* AppSettings::esriTokenName =                                "EsriToken";

const char* AppSettings::logDirectory =             "Logs";
const char* AppSettings::crashDirectory =           "CrashLogs";

AppSettings::AppSettings(QObject* parent)
    : SettingsGroup                         (name, settingsGroup, parent)
    , _appFontPointSizeFact                 (NULL)
    , _indoorPaletteFact                    (NULL)
    , _savePathFact                         (NULL)
    , _mapboxTokenFact                      (NULL)
    , _esriTokenFact                        (NULL)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    qmlRegisterUncreatableType<AppSettings>("QGroundControl.SettingsManager", 1, 0, "AppSettings", "Reference only");
    QGCPalette::setGlobalTheme(indoorPalette()->rawValue().toBool() ? QGCPalette::Dark : QGCPalette::Light);

    // Instantiate savePath so we can check for override and setup default path if needed

    SettingsFact* savePathFact = qobject_cast<SettingsFact*>(savePath());
    QString appName = qgcApp()->applicationName();
    if (savePathFact->rawValue().toString().isEmpty() && _nameToMetaDataMap[savePathName]->rawDefaultValue().toString().isEmpty()) {
#ifdef __mobile__
#ifdef __ios__
        QDir rootDir = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
#else
        QDir rootDir = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
#endif
        savePathFact->setVisible(false);
#else
        QDir rootDir = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
#endif
        savePathFact->setRawValue(rootDir.filePath(appName));
    }

    connect(savePathFact, &Fact::rawValueChanged, this, &AppSettings::savePathsChanged);
    connect(savePathFact, &Fact::rawValueChanged, this, &AppSettings::_checkSavePathDirectories);

    _checkSavePathDirectories();
}

void AppSettings::_checkSavePathDirectories(void)
{
    QDir savePathDir(savePath()->rawValue().toString());
    if (!savePathDir.exists()) {
        QDir().mkpath(savePathDir.absolutePath());
    }
    if (savePathDir.exists()) {
        savePathDir.mkdir(logDirectory);
        savePathDir.mkdir(crashDirectory);
    }
}

Fact* AppSettings::appFontPointSize(void)
{
    if (!_appFontPointSizeFact) {
        _appFontPointSizeFact = _createSettingsFact(appFontPointSizeName);
    }

    return _appFontPointSizeFact;
}

Fact* AppSettings::indoorPalette(void)
{
    if (!_indoorPaletteFact) {
        _indoorPaletteFact = _createSettingsFact(indoorPaletteName);
        connect(_indoorPaletteFact, &Fact::rawValueChanged, this, &AppSettings::_indoorPaletteChanged);
    }

    return _indoorPaletteFact;
}

void AppSettings::_indoorPaletteChanged(void)
{
    qgcApp()->_loadCurrentStyleSheet();
    QGCPalette::setGlobalTheme(indoorPalette()->rawValue().toBool() ? QGCPalette::Dark : QGCPalette::Light);
}

Fact* AppSettings::savePath(void)
{
    if (!_savePathFact) {
        _savePathFact = _createSettingsFact(savePathName);
    }

    return _savePathFact;
}

QString AppSettings::logSavePath(void)
{
    QString path = savePath()->rawValue().toString();
    if (!path.isEmpty() && QDir(path).exists()) {
        QDir dir(path);
        return dir.filePath(logDirectory);
    }

    return QString();
}
QString AppSettings::crashSavePath(void)
{
    QString path = savePath()->rawValue().toString();
    if (!path.isEmpty() && QDir(path).exists()) {
        QDir dir(path);
        return dir.filePath(crashDirectory);
    }

    return QString();
}

Fact* AppSettings::mapboxToken(void)
{
    if (!_mapboxTokenFact) {
        _mapboxTokenFact = _createSettingsFact(mapboxTokenName);
    }

    return _mapboxTokenFact;
}

Fact* AppSettings::esriToken(void)
{
    if (!_esriTokenFact) {
        _esriTokenFact = _createSettingsFact(esriTokenName);
    }

    return _esriTokenFact;
}
