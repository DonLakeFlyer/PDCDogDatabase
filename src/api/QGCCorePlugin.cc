/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "QGCOptions.h"
#include "QmlComponentInfo.h"
#include "FactMetaData.h"
#include "SettingsManager.h"
#include "AppMessages.h"
#include "QmlObjectListModel.h"
#include "QGCLoggingCategory.h"

#include <QtQml>
#include <QQmlEngine>

/// @file
///     @brief Core Plugin Interface for QGroundControl - Default Implementation
///     @author Gus Grubba <mavlink@grubba.com>

class QGCCorePlugin_p
{
public:
    QGCCorePlugin_p()
        : pGeneral                  (nullptr)
        , pOfflineMaps              (nullptr)
        , pConsole                  (nullptr)
        , defaultOptions            (nullptr)
        , valuesPageWidgetInfo      (nullptr)
        , cameraPageWidgetInfo      (nullptr)
        , videoPageWidgetInfo       (nullptr)
        , healthPageWidgetInfo      (nullptr)
        , vibrationPageWidgetInfo   (nullptr)
    {
    }

    ~QGCCorePlugin_p()
    {
        if(pGeneral)
            delete pGeneral;
        if(pOfflineMaps)
            delete pOfflineMaps;
        if(pConsole)
            delete pConsole;
        if(defaultOptions)
            delete defaultOptions;
    }

    QmlComponentInfo* pGeneral;
    QmlComponentInfo* pOfflineMaps;
    QmlComponentInfo* pConsole;
    QVariantList settingsList;
    QGCOptions*  defaultOptions;

    QmlComponentInfo*   valuesPageWidgetInfo;
    QmlComponentInfo*   cameraPageWidgetInfo;
    QmlComponentInfo*   videoPageWidgetInfo;
    QmlComponentInfo*   healthPageWidgetInfo;
    QmlComponentInfo*   vibrationPageWidgetInfo;
    QVariantList        instrumentPageWidgetList;

    QmlObjectListModel _emptyCustomMapItems;
};

QGCCorePlugin::~QGCCorePlugin()
{
    if(_p) {
        delete _p;
    }
}

QGCCorePlugin::QGCCorePlugin(QGCApplication *app, QGCToolbox* toolbox)
    : QGCTool(app, toolbox)
    , _showTouchAreas(false)
    , _showAdvancedUI(true)
{
    _p = new QGCCorePlugin_p;
}

void QGCCorePlugin::setToolbox(QGCToolbox *toolbox)
{
    QGCTool::setToolbox(toolbox);
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    qmlRegisterUncreatableType<QGCCorePlugin>("QGroundControl.QGCCorePlugin", 1, 0, "QGCCorePlugin", "Reference only");
    qmlRegisterUncreatableType<QGCOptions>("QGroundControl.QGCOptions",       1, 0, "QGCOptions",    "Reference only");
}

QVariantList &QGCCorePlugin::settingsPages()
{
    if(!_p->pGeneral) {
        _p->pGeneral = new QmlComponentInfo(tr("General"),
            QUrl::fromUserInput("qrc:/qml/GeneralSettings.qml"),
            QUrl::fromUserInput("qrc:/res/gear-white.svg"));
        _p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pGeneral)));
        _p->pOfflineMaps = new QmlComponentInfo(tr("Offline Maps"),
            QUrl::fromUserInput("qrc:/qml/OfflineMap.qml"),
            QUrl::fromUserInput("qrc:/res/waves.svg"));
        _p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pOfflineMaps)));
        _p->pConsole = new QmlComponentInfo(tr("Console"),
            QUrl::fromUserInput("qrc:/qml/QGroundControl/Controls/AppMessages.qml"));
        _p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pConsole)));
    }
    return _p->settingsList;
}

QVariantList& QGCCorePlugin::instrumentPages(void)
{
    if (!_p->valuesPageWidgetInfo) {
        _p->valuesPageWidgetInfo    = new QmlComponentInfo(tr("Values"),    QUrl::fromUserInput("qrc:/qml/ValuePageWidget.qml"));
        _p->cameraPageWidgetInfo    = new QmlComponentInfo(tr("Camera"),    QUrl::fromUserInput("qrc:/qml/CameraPageWidget.qml"));
#if defined(QGC_GST_STREAMING)
        _p->videoPageWidgetInfo     = new QmlComponentInfo(tr("Video Stream"), QUrl::fromUserInput("qrc:/qml/VideoPageWidget.qml"));
#endif
        _p->healthPageWidgetInfo    = new QmlComponentInfo(tr("Health"),    QUrl::fromUserInput("qrc:/qml/HealthPageWidget.qml"));
        _p->vibrationPageWidgetInfo = new QmlComponentInfo(tr("Vibration"), QUrl::fromUserInput("qrc:/qml/VibrationPageWidget.qml"));

        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->valuesPageWidgetInfo));
        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->cameraPageWidgetInfo));
#if defined(QGC_GST_STREAMING)
        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->videoPageWidgetInfo));
#endif
        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->healthPageWidgetInfo));
        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->vibrationPageWidgetInfo));
    }
    return _p->instrumentPageWidgetList;
}

int QGCCorePlugin::defaultSettings()
{
    return 0;
}

QGCOptions* QGCCorePlugin::options()
{
    if(!_p->defaultOptions) {
        _p->defaultOptions = new QGCOptions();
    }
    return _p->defaultOptions;
}

bool QGCCorePlugin::overrideSettingsGroupVisibility(QString name)
{
    Q_UNUSED(name);

    // Always show all
    return true;
}

bool QGCCorePlugin::adjustSettingMetaData(const QString& settingsGroup, FactMetaData& metaData)
{
    if (settingsGroup != AppSettings::settingsGroup) {
        // All changes refer to AppSettings
        return true;
    }

    //-- Default Palette
    if (metaData.name() == AppSettings::indoorPaletteName) {
        QVariant outdoorPalette;
#if defined (__mobile__)
        outdoorPalette = 0;
#else
        outdoorPalette = 1;
#endif
        metaData.setRawDefaultValue(outdoorPalette);
        return true;
#if defined(__ios__)
    } else if (metaData.name() == AppSettings::savePathName) {
        QString appName = qgcApp()->applicationName();
        QDir rootDir = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        metaData.setRawDefaultValue(rootDir.filePath(appName));
        return false;
#endif
    }
    return true; // Show setting in ui
}

void QGCCorePlugin::setShowTouchAreas(bool show)
{
    if (show != _showTouchAreas) {
        _showTouchAreas = show;
        emit showTouchAreasChanged(show);
    }
}

void QGCCorePlugin::setShowAdvancedUI(bool show)
{
    if (show != _showAdvancedUI) {
        _showAdvancedUI = show;
        emit showAdvancedUIChanged(show);
    }
}

void QGCCorePlugin::paletteOverride(QString colorName, QGCPalette::PaletteColorInfo_t& colorInfo)
{
    Q_UNUSED(colorName);
    Q_UNUSED(colorInfo);
}

QString QGCCorePlugin::showAdvancedUIMessage(void) const
{
    return tr("WARNING: You are about to enter Advanced Mode. "
              "If used incorrectly, this may cause your vehicle to malfunction thus voiding your warranty. "
              "You should do so only if instructed by customer support. "
              "Are you sure you want to enable Advanced Mode?");
}

void QGCCorePlugin::valuesWidgetDefaultSettings(QStringList& largeValues, QStringList& smallValues)
{
    Q_UNUSED(smallValues);
    largeValues << "Vehicle.altitudeRelative" << "Vehicle.groundSpeed" << "Vehicle.flightTime";
}

QQmlApplicationEngine* QGCCorePlugin::createRootWindow(QObject *parent)
{
    QQmlApplicationEngine* pEngine = new QQmlApplicationEngine(parent);
    pEngine->addImportPath("qrc:/qml");
    pEngine->rootContext()->setContextProperty("debugMessageModel", AppMessages::getModel());
    pEngine->load(QUrl(QStringLiteral("qrc:/qml/MainWindowNative.qml")));
    return pEngine;
}

QmlObjectListModel* QGCCorePlugin::customMapItems(void)
{
    return &_p->_emptyCustomMapItems;
}

QString QGCCorePlugin::stableVersionCheckFileUrl(void) const
{
#ifdef QGC_CUSTOM_BUILD
    // Custom builds must override to turn on and provide their own location
    return QString();
#else
    return QString("https://s3-us-west-2.amazonaws.com/qgroundcontrol/latest/QGC.version.txt");
#endif
}
