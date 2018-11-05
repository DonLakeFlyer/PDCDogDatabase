/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/**
 * @file
 *   @brief Implementation of class QGCApplication
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#include <QFile>
#include <QFlags>
#include <QPixmap>
#include <QDesktopWidget>
#include <QPainter>
#include <QStyleFactory>
#include <QAction>
#include <QStringListModel>
#include <QRegularExpression>
#include <QSettings>

#ifdef QGC_ENABLE_BLUETOOTH
#include <QBluetoothLocalDevice>
#endif

#include <QDebug>

#include "QGC.h"
#include "QGCApplication.h"
#include "CmdLineOptParser.h"
#include "QGCTemporaryFile.h"
#include "QGCPalette.h"
#include "QGCMapPalette.h"
#include "QGCLoggingCategory.h"
#include "ScreenToolsController.h"
#include "QGCFileDialogController.h"
#include "QmlObjectListModel.h"
#include "QGCGeoBoundingCube.h"
#include "QGroundControlQmlGlobal.h"
#include "CoordinateVector.h"
#include "AppMessages.h"
#include "SimulatedPosition.h"
#include "PositionManager.h"
#include "SettingsManager.h"
#include "QGCCorePlugin.h"
#include "EditPositionDialogController.h"
#include "FactValueSliderListModel.h"
#include "QGCFileDownload.h"

#ifndef __mobile__
#include "QGCQFileDialog.h"
#include "QGCMessageBox.h"
#include "MainWindow.h"
#endif

#ifdef Q_OS_LINUX
#ifndef __mobile__
#include <unistd.h>
#include <sys/types.h>
#endif
#endif

#include "QGCMapEngine.h"

QGCApplication* QGCApplication::_app = nullptr;

const char* QGCApplication::_deleteAllSettingsKey           = "DeleteAllSettingsNextBoot";
const char* QGCApplication::_settingsVersionKey             = "SettingsVersion";

const char* QGCApplication::_darkStyleFile          = ":/res/styles/style-dark.css";
const char* QGCApplication::_lightStyleFile         = ":/res/styles/style-light.css";

// Qml Singleton factories

static QObject* screenToolsControllerSingletonFactory(QQmlEngine*, QJSEngine*)
{
    ScreenToolsController* screenToolsController = new ScreenToolsController;
    return screenToolsController;
}

static QObject* qgroundcontrolQmlGlobalSingletonFactory(QQmlEngine*, QJSEngine*)
{
    // We create this object as a QGCTool even though it isn't in the toolbox
    QGroundControlQmlGlobal* qmlGlobal = new QGroundControlQmlGlobal(qgcApp(), qgcApp()->toolbox());
    qmlGlobal->setToolbox(qgcApp()->toolbox());

    return qmlGlobal;
}

QGCApplication::QGCApplication(int &argc, char* argv[], bool unitTesting)
#ifdef __mobile__
    : QGuiApplication           (argc, argv)
    , _qmlAppEngine             (nullptr)
#else
    : QApplication              (argc, argv)
#endif
    , _runningUnitTests         (unitTesting)
    , _logOutput                (false)
    , _fakeMobile               (false)
    , _settingsUpgraded         (false)
    , _majorVersion             (0)
    , _minorVersion             (0)
    , _buildVersion             (0)
    , _currentVersionDownload   (nullptr)
    , _toolbox                  (nullptr)
    , _bluetoothAvailable       (false)
{
    _app = this;


    QLocale locale = QLocale::system();
    //-- Some forced locales for testing
    //QLocale locale = QLocale(QLocale::German);
    //QLocale locale = QLocale(QLocale::French);
    //QLocale locale = QLocale(QLocale::Chinese);
#if defined (__macos__)
    locale = QLocale(locale.name());
#endif
    //-- Our localization
    if(_QGCTranslator.load(locale, "qgc_", "", ":/localization"))
        _app->installTranslator(&_QGCTranslator);

    // This prevents usage of QQuickWidget to fail since it doesn't support native widget siblings
#ifndef __android__
    setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
#endif

    // Setup for network proxy support
    QNetworkProxyFactory::setUseSystemConfiguration(true);

#ifdef Q_OS_LINUX
#ifndef __mobile__
    if (!_runningUnitTests) {
        if (getuid() == 0) {
            QMessageBox msgBox;
            msgBox.setInformativeText(tr("You are running %1 as root. "
                                         "You should not do this since it will cause other issues with %1. "
                                         "%1 will now exit. "
                                         "If you are having serial port issues on Ubuntu, execute the following commands to fix most issues:\n"
                                         "sudo usermod -a -G dialout $USER\n"
                                         "sudo apt-get remove modemmanager").arg(qgcApp()->applicationName()));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            _exit(0);
        }

        // Determine if we have the correct permissions to access USB serial devices
        QFile permFile("/etc/group");
        if(permFile.open(QIODevice::ReadOnly)) {
            while(!permFile.atEnd()) {
                QString line = permFile.readLine();
                if (line.contains("dialout") && !line.contains(getenv("USER"))) {
                    QMessageBox msgBox;
                    msgBox.setInformativeText("The current user does not have the correct permissions to access serial devices. "
                                              "You should also remove modemmanager since it also interferes. "
                                              "If you are using Ubuntu, execute the following commands to fix these issues:\n"
                                              "sudo usermod -a -G dialout $USER\n"
                                              "sudo apt-get remove modemmanager");
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setDefaultButton(QMessageBox::Ok);
                    msgBox.exec();
                    break;
                }
            }
            permFile.close();
        }
    }
#endif
#endif

    // Parse command line options

    bool fClearSettingsOptions = false; // Clear stored settings
    bool logging = false;               // Turn on logging
    QString loggingOptions;

    CmdLineOpt_t rgCmdLineOptions[] = {
        { "--clear-settings",   &fClearSettingsOptions, nullptr },
        { "--logging",          &logging,               &loggingOptions },
        { "--fake-mobile",      &_fakeMobile,           nullptr },
        { "--log-output",       &_logOutput,            nullptr },
        // Add additional command line option flags here
    };

    ParseCmdLineOptions(argc, argv, rgCmdLineOptions, sizeof(rgCmdLineOptions)/sizeof(rgCmdLineOptions[0]), false);

    // Set application information
    if (_runningUnitTests) {
        // We don't want unit tests to use the same QSettings space as the normal app. So we tweak the app
        // name. Also we want to run unit tests with clean settings every time.
        setApplicationName(QString("%1_unittest").arg(QGC_APPLICATION_NAME));
    } else {
        setApplicationName(QGC_APPLICATION_NAME);
    }
    setOrganizationName(QGC_ORG_NAME);
    setOrganizationDomain(QGC_ORG_DOMAIN);

    this->setApplicationVersion(QString(GIT_VERSION));

    // Set settings format
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;
    qDebug() << "Settings location" << settings.fileName() << "Is writable?:" << settings.isWritable();

#ifdef UNITTEST_BUILD
    if (!settings.isWritable()) {
        qWarning() << "Setings location is not writable";
    }
#endif
    // The setting will delete all settings on this boot
    fClearSettingsOptions |= settings.contains(_deleteAllSettingsKey);

    if (_runningUnitTests) {
        // Unit tests run with clean settings
        fClearSettingsOptions = true;
    }

    if (fClearSettingsOptions) {
        // User requested settings to be cleared on command line
        settings.clear();
    } else {
        // Determine if upgrade message for settings version bump is required. Check and clear must happen before toolbox is started since
        // that will write some settings.
        if (settings.contains(_settingsVersionKey)) {
            if (settings.value(_settingsVersionKey).toInt() != QGC_SETTINGS_VERSION) {
                settings.clear();
                _settingsUpgraded = true;
            }
        } else if (settings.allKeys().count()) {
            // Settings version key is missing and there are settings. This is an upgrade scenario.
            settings.clear();
            _settingsUpgraded = true;
        }
    }
    settings.setValue(_settingsVersionKey, QGC_SETTINGS_VERSION);

    // Set up our logging filters
    QGCLoggingCategoryRegister::instance()->setFilterRulesFromSettings(loggingOptions);

    _toolbox = new QGCToolbox(this);
    _toolbox->setChildToolboxes();

    _checkForNewVersion();
}

void QGCApplication::_shutdown(void)
{
    // This code is specifically not in the destructor since the application object may not be available in the destructor.
    // This cause problems for deleting object like settings which are in the toolbox which may have qml references. By
    // moving them here and having main.cc call this prior to deleting the app object we make sure app object is still
    // around while these things are shutting down.
#ifndef __mobile__
    MainWindow* mainWindow = MainWindow::instance();
    if (mainWindow) {
        delete mainWindow;
    }
#endif
    delete _toolbox;
}

QGCApplication::~QGCApplication()
{
    // Place shutdown code in _shutdown
    _app = nullptr;
}

void QGCApplication::_initCommon(void)
{
    static const char* kRefOnly         = "Reference only";
    static const char* kQGCControllers  = "QGroundControl.Controllers";

    QSettings settings;

    // Register our Qml objects

    qmlRegisterType<QGCPalette>     ("QGroundControl.Palette", 1, 0, "QGCPalette");
    qmlRegisterType<QGCMapPalette>  ("QGroundControl.Palette", 1, 0, "QGCMapPalette");

    qmlRegisterUncreatableType<CoordinateVector>    ("QGroundControl",                      1, 0, "CoordinateVector",           kRefOnly);
    qmlRegisterUncreatableType<QmlObjectListModel>  ("QGroundControl",                      1, 0, "QmlObjectListModel",         kRefOnly);

    qmlRegisterUncreatableType<QGCPositionManager>  ("QGroundControl.QGCPositionManager",   1, 0, "QGCPositionManager",         kRefOnly);
    qmlRegisterUncreatableType<FactValueSliderListModel>("QGroundControl.FactControls",     1, 0, "FactValueSliderListModel",   kRefOnly);

    qmlRegisterUncreatableType<QGCGeoBoundingCube>  ("QGroundControl.FlightMap",            1, 0, "QGCGeoBoundingCube",         kRefOnly);

    qmlRegisterType<ScreenToolsController>          (kQGCControllers,                       1, 0, "ScreenToolsController");
    qmlRegisterType<QGCFileDialogController>        (kQGCControllers,                       1, 0, "QGCFileDialogController");
    qmlRegisterType<EditPositionDialogController>   (kQGCControllers,                       1, 0, "EditPositionDialogController");

    // Register Qml Singletons
    qmlRegisterSingletonType<QGroundControlQmlGlobal>   ("QGroundControl",                          1, 0, "QGroundControl",         qgroundcontrolQmlGlobalSingletonFactory);
    qmlRegisterSingletonType<ScreenToolsController>     ("QGroundControl.ScreenToolsController",    1, 0, "ScreenToolsController",  screenToolsControllerSingletonFactory);
}

bool QGCApplication::_initForNormalAppBoot(void)
{
    QSettings settings;

    _loadCurrentStyleSheet();

    // Exit main application when last window is closed
    connect(this, &QGCApplication::lastWindowClosed, this, QGCApplication::quit);

#ifdef __mobile__
    _qmlAppEngine = toolbox()->corePlugin()->createRootWindow(this);
#else
    // Start the user interface
    MainWindow* mainWindow = MainWindow::_create();
    Q_CHECK_PTR(mainWindow);
#endif

    if (_settingsUpgraded) {
        showMessage(tr("The format for QGroundControl saved settings has been modified. "
                    "Your saved settings have been reset to defaults."));
    }

    if (getQGCMapEngine()->wasCacheReset()) {
        showMessage(tr("The Offline Map Cache database has been upgraded. "
                    "Your old map cache sets have been reset."));
    }

    settings.sync();
    return true;
}

bool QGCApplication::_initForUnitTests(void)
{
    return true;
}

void QGCApplication::deleteAllSettingsNextBoot(void)
{
    QSettings settings;
    settings.setValue(_deleteAllSettingsKey, true);
}

void QGCApplication::clearDeleteAllSettingsNextBoot(void)
{
    QSettings settings;
    settings.remove(_deleteAllSettingsKey);
}

/// @brief Returns the QGCApplication object singleton.
QGCApplication* qgcApp(void)
{
    return QGCApplication::_app;
}

void QGCApplication::informationMessageBoxOnMainThread(const QString& title, const QString& msg)
{
    Q_UNUSED(title);
    showMessage(msg);
}

void QGCApplication::warningMessageBoxOnMainThread(const QString& title, const QString& msg)
{
#ifdef __mobile__
    Q_UNUSED(title)
    showMessage(msg);
#else
    QGCMessageBox::warning(title, msg);
#endif
}

void QGCApplication::criticalMessageBoxOnMainThread(const QString& title, const QString& msg)
{
#ifdef __mobile__
    Q_UNUSED(title)
    showMessage(msg);
#else
    QGCMessageBox::critical(title, msg);
#endif
}

void QGCApplication::_loadCurrentStyleSheet(void)
{
#ifndef __mobile__
    bool success = true;
    QString styles;

    // The dark style sheet is the master. Any other selected style sheet just overrides
    // the colors of the master sheet.
    QFile masterStyleSheet(_darkStyleFile);
    if (masterStyleSheet.open(QIODevice::ReadOnly | QIODevice::Text)) {
        styles = masterStyleSheet.readAll();
    } else {
        qDebug() << "Unable to load master dark style sheet";
        success = false;
    }

    if (success && !_toolbox->settingsManager()->appSettings()->indoorPalette()->rawValue().toBool()) {
        // Load the slave light stylesheet.
        QFile styleSheet(_lightStyleFile);
        if (styleSheet.open(QIODevice::ReadOnly | QIODevice::Text)) {
            styles += styleSheet.readAll();
        } else {
            qWarning() << "Unable to load slave light sheet:";
            success = false;
        }
    }

    setStyleSheet(styles);

    if (!success) {
        // Fall back to plastique if we can't load our own
        setStyle("plastique");
    }
#endif
}

QObject* QGCApplication::_rootQmlObject()
{
#ifdef __mobile__
    if(_qmlAppEngine && _qmlAppEngine->rootObjects().size())
        return _qmlAppEngine->rootObjects()[0];
    return nullptr;
#else
    MainWindow * mainWindow = MainWindow::instance();
    if (mainWindow) {
        return mainWindow->rootQmlObject();
    } else if (runningUnitTests()){
        // Unit test can run without a main window
        return nullptr;
    } else {
        qWarning() << "Why is MainWindow missing?";
        return nullptr;
    }
#endif
}


void QGCApplication::showMessage(const QString& message)
{
    // PreArm messages are handled by Vehicle and shown in Map
    if (message.startsWith(QStringLiteral("PreArm")) || message.startsWith(QStringLiteral("preflight"), Qt::CaseInsensitive)) {
        return;
    }

    QObject* rootQmlObject = _rootQmlObject();

    if (rootQmlObject) {
        QVariant varReturn;
        QVariant varMessage = QVariant::fromValue(message);

        QMetaObject::invokeMethod(_rootQmlObject(), "showMessage", Q_RETURN_ARG(QVariant, varReturn), Q_ARG(QVariant, varMessage));
#ifndef __mobile__
    } else if (runningUnitTests()){
        // Unit test can run without a main window which will lead to no root qml object. Use QGCMessageBox instead
        QGCMessageBox::information("Unit Test", message);
#endif
    } else {
        qWarning() << "Internal error";
    }
}

void QGCApplication::showSetupView(void)
{
    if(_rootQmlObject()) {
        QMetaObject::invokeMethod(_rootQmlObject(), "showSetupView");
    }
}

void QGCApplication::qmlAttemptWindowClose(void)
{
    if(_rootQmlObject()) {
        QMetaObject::invokeMethod(_rootQmlObject(), "attemptWindowClose");
    }
}

bool QGCApplication::isInternetAvailable()
{
    return getQGCMapEngine()->isInternetActive();
}

void QGCApplication::_checkForNewVersion(void)
{
#ifndef __mobile__
    if (!_runningUnitTests) {
        if (_parseVersionText(applicationVersion(), _majorVersion, _minorVersion, _buildVersion)) {
            QString versionCheckFile = toolbox()->corePlugin()->stableVersionCheckFileUrl();
            if (!versionCheckFile.isEmpty()) {
                _currentVersionDownload = new QGCFileDownload(this);
                connect(_currentVersionDownload, &QGCFileDownload::downloadFinished, this, &QGCApplication::_currentVersionDownloadFinished);
                connect(_currentVersionDownload, &QGCFileDownload::error, this, &QGCApplication::_currentVersionDownloadError);
                _currentVersionDownload->download(versionCheckFile);
            }
        }
    }
#endif
}

void QGCApplication::_currentVersionDownloadFinished(QString remoteFile, QString localFile)
{
    Q_UNUSED(remoteFile);

#ifdef __mobile__
    Q_UNUSED(localFile);
#else
    QFile versionFile(localFile);
    if (versionFile.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&versionFile);
        QString version = textStream.readLine();

        qDebug() << version;

        int majorVersion, minorVersion, buildVersion;
        if (_parseVersionText(version, majorVersion, minorVersion, buildVersion)) {
            if (_majorVersion < majorVersion ||
                    (_majorVersion == majorVersion && _minorVersion < minorVersion) ||
                    (_majorVersion == majorVersion && _minorVersion == minorVersion && _buildVersion < buildVersion)) {
                QGCMessageBox::information(tr("New Version Available"), tr("There is a newer version of %1 available. You can download it from %2.").arg(applicationName()).arg(toolbox()->corePlugin()->stableDownloadLocation()));
            }
        }
    }

    _currentVersionDownload->deleteLater();
#endif
}

void QGCApplication::_currentVersionDownloadError(QString errorMsg)
{
    Q_UNUSED(errorMsg);
    _currentVersionDownload->deleteLater();
}

bool QGCApplication::_parseVersionText(const QString& versionString, int& majorVersion, int& minorVersion, int& buildVersion)
{
    QRegularExpression regExp("v(\\d+)\\.(\\d+)\\.(\\d+)");
    QRegularExpressionMatch match = regExp.match(versionString);
    if (match.hasMatch() && match.lastCapturedIndex() == 3) {
        majorVersion = match.captured(1).toInt();
        minorVersion = match.captured(2).toInt();
        buildVersion = match.captured(3).toInt();
        return true;
    }

    return false;
}
