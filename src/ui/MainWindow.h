/****************************************************************************
 *
 *   (c) 2009-2018 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/**
 * @file
 *   @brief Definition of class MainWindow
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#pragma once

#ifdef __mobile__
#error Should not be include in mobile build
#endif

#include <QMainWindow>
#include <QStatusBar>
#include <QStackedWidget>
#include <QSettings>
#include <QList>
#include <QTimer>
#include <QVBoxLayout>

#include "ui_MainWindow.h"

/**
 * @brief Main Application Window
 *
 **/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// @brief Returns the MainWindow singleton. Will not create the MainWindow if it has not already
    ///         been created.
    static MainWindow* instance(void);

    /// @brief Deletes the MainWindow singleton
    void deleteInstance(void);

    /// @brief Creates the MainWindow singleton. Should only be called once by QGCApplication.
    static MainWindow* _create();

    ~MainWindow();

    // Called from MainWindow.qml when the user accepts the window close dialog
    void _reallyClose(void);

    /// @return Root qml object of main window QML
    QObject* rootQmlObject(void);

public slots:
    void closeEvent(QCloseEvent* event);

    /** @brief Update the window name */
    void configureWindowName();

signals:
    /** Emitted when any value changes from any source */
    void valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec);
    void reallyClose(void);

    // Used for unit tests to know when the main window closes
    void mainWindowClosed(void);

protected:
    void connectCommonActions();

    void loadSettings();
    void storeSettings();

    QSettings settings;

#ifdef QGC_MOUSE_ENABLED_WIN
    /** @brief 3d Mouse support (WIN only) */
    Mouse3DInput* mouseInput;               ///< 3dConnexion 3dMouse SDK
    Mouse6dofInput* mouse;                  ///< Implementation for 3dMouse input
#endif // QGC_MOUSE_ENABLED_WIN

    QTimer windowNameUpdateTimer;

private slots:
    void _closeWindow(void) { close(); }
    void _showAdvancedUIChanged(bool advanced);

private:
    /// Constructor is private since all creation should be through MainWindow::_create
    MainWindow();

    void _openUrl(const QString& url, const QString& errorMessage);

    QMap<QString, QAction*>         _mapName2Action;

    void _storeCurrentViewState(void);
    void _loadCurrentViewState(void);

    QVBoxLayout*            _centralLayout;
    Ui::MainWindow          _ui;

    bool    _forceClose;

    QString _getWindowGeometryKey();
};

