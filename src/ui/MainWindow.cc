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
 *   @brief Implementation of class MainWindow
 *   @author Lorenz Meier <mail@qgroundcontrol.org>
 */

#include <QSettings>
#include <QNetworkInterface>
#include <QDebug>
#include <QTimer>
#include <QHostInfo>
#include <QQuickView>
#include <QDesktopWidget>
#include <QScreen>
#include <QDesktopServices>
#include <QDockWidget>
#include <QMenuBar>
#include <QDialog>
#include <QFontDatabase>
#include <QQuickItem>

#include "QGC.h"
#include "MainWindow.h"
#include "QGCApplication.h"
#include "QGCImageProvider.h"
#include "QGCCorePlugin.h"

#ifndef __mobile__
#include "AppMessages.h"
#endif

#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

static MainWindow* _instance = NULL;   ///< @brief MainWindow singleton

MainWindow* MainWindow::_create()
{
    new MainWindow();
    return _instance;
}

MainWindow* MainWindow::instance(void)
{
    return _instance;
}

void MainWindow::deleteInstance(void)
{
    delete this;
}

/// @brief Private constructor for MainWindow. MainWindow singleton is only ever created
///         by MainWindow::_create method. Hence no other code should have access to
///         constructor.
MainWindow::MainWindow()
    : _forceClose           (false)
{
    _instance = this;

    //-- Load fonts
    if(QFontDatabase::addApplicationFont(":/fonts/opensans") < 0) {
        qWarning() << "Could not load /fonts/opensans font";
    }
    if(QFontDatabase::addApplicationFont(":/fonts/opensans-demibold") < 0) {
        qWarning() << "Could not load /fonts/opensans-demibold font";
    }

    // Qt 4/5 on Ubuntu does place the native menubar correctly so on Linux we revert back to in-window menu bar.
#ifdef Q_OS_LINUX
    menuBar()->setNativeMenuBar(false);
#endif
    // Setup user interface
    loadSettings();

    _ui.setupUi(this);
    // Make sure tool bar elements all fit before changing minimum width
    setMinimumWidth(1024);
    setMinimumHeight(620);
    configureWindowName();

    // Setup central widget with a layout to hold the views
    _centralLayout = new QVBoxLayout();
    _centralLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget()->setLayout(_centralLayout);

    _mainQmlWidgetHolder = new QGCQmlWidgetHolder(QString(), NULL, this);
    _centralLayout->addWidget(_mainQmlWidgetHolder);
    _mainQmlWidgetHolder->setVisible(true);

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    _mainQmlWidgetHolder->setContextPropertyObject("controller", this);
    _mainQmlWidgetHolder->setContextPropertyObject("debugMessageModel", AppMessages::getModel());
    _mainQmlWidgetHolder->setSource(QUrl::fromUserInput("qrc:qml/MainWindowHybrid.qml"));

    // Image provider
    QQuickImageProvider* pImgProvider = dynamic_cast<QQuickImageProvider*>(qgcApp()->toolbox()->imageProvider());
    _mainQmlWidgetHolder->getEngine()->addImageProvider(QStringLiteral("QGCImages"), pImgProvider);

    // Set dock options
    setDockOptions(0);
    // Setup corners
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    // On Mobile devices, we don't want any main menus at all.
#ifdef __mobile__
    menuBar()->setNativeMenuBar(false);
#endif

    connect(qgcApp()->toolbox()->corePlugin(), &QGCCorePlugin::showAdvancedUIChanged, this, &MainWindow::_showAdvancedUIChanged);
    _showAdvancedUIChanged(qgcApp()->toolbox()->corePlugin()->showAdvancedUI());

    // Status Bar
    setStatusBar(new QStatusBar(this));
    statusBar()->setSizeGripEnabled(true);

    // Create actions
    connectCommonActions();

#ifndef __mobile__

    // Restore the window position and size
    if (settings.contains(_getWindowGeometryKey()))
    {
        restoreGeometry(settings.value(_getWindowGeometryKey()).toByteArray());
    }
    else
    {
        // Adjust the size
        QScreen* scr = QApplication::primaryScreen();
        QSize scrSize = scr->availableSize();
        if (scrSize.width() <= 1280)
        {
            resize(scrSize.width(), scrSize.height());
        }
        else
        {
            int w = scrSize.width()  > 1600 ? 1600 : scrSize.width();
            int h = scrSize.height() >  800 ?  800 : scrSize.height();
            resize(w, h);
            move((scrSize.width() - w) / 2, (scrSize.height() - h) / 2);
        }
    }
#endif

    connect(&windowNameUpdateTimer, &QTimer::timeout, this, &MainWindow::configureWindowName);
    windowNameUpdateTimer.start(15000);

    if (!qgcApp()->runningUnitTests()) {
#ifdef __mobile__
        menuBar()->hide();
#endif
        show();
    }
}

MainWindow::~MainWindow()
{
    // This needs to happen before we get into the QWidget dtor
    // otherwise  the QML engine reads freed data and tries to
    // destroy MainWindow a second time.
    delete _mainQmlWidgetHolder;
    _instance = NULL;
}

QString MainWindow::_getWindowGeometryKey()
{
    return "_geometry";
}

void MainWindow::_reallyClose(void)
{
    _forceClose = true;
    close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!_forceClose) {
        // Attempt close from within the root Qml item
        qgcApp()->qmlAttemptWindowClose();
        event->ignore();
        return;
    }

    _storeCurrentViewState();
    storeSettings();

    emit mainWindowClosed();
}

void MainWindow::loadSettings()
{
}

void MainWindow::storeSettings()
{
    settings.setValue(_getWindowGeometryKey(), saveGeometry());
}

void MainWindow::configureWindowName()
{
    setWindowTitle(qApp->applicationName() + " " + qApp->applicationVersion());
}

/**
* @brief Create all actions associated to the main window
*
**/
void MainWindow::connectCommonActions()
{
    // Connect internal actions
    connect(this, &MainWindow::reallyClose, this, &MainWindow::_reallyClose, Qt::QueuedConnection); // Queued to allow closeEvent to fully unwind before _reallyClose is called
}

void MainWindow::_openUrl(const QString& url, const QString& errorMessage)
{
    if(!QDesktopServices::openUrl(QUrl(url))) {
        qgcApp()->showMessage(QString("Could not open information in browser: %1").arg(errorMessage));
    }
}

/// Stores the state of the toolbar, status bar and widgets associated with the current view
void MainWindow::_storeCurrentViewState(void)
{
    settings.setValue(_getWindowGeometryKey(), saveGeometry());
}

QObject* MainWindow::rootQmlObject(void)
{
    return _mainQmlWidgetHolder->getRootObject();
}

void MainWindow::_showAdvancedUIChanged(bool advanced)
{
    if (advanced) {
        menuBar()->addMenu(_ui.menuFile);
        menuBar()->addMenu(_ui.menuWidgets);
    } else {
        menuBar()->clear();
    }
}
