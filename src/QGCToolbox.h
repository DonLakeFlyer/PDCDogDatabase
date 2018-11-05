/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#ifndef QGCToolbox_h
#define QGCToolbox_h

#include <QObject>

class FactSystem;
class QGCMapEngineManager;
class QGCApplication;
class QGCImageProvider;
class QGCPositionManager;
class QGCCorePlugin;
class SettingsManager;

/// This is used to manage all of our top level services/tools
class QGCToolbox : public QObject {
    Q_OBJECT

public:
    QGCToolbox(QGCApplication* app);

    QGCMapEngineManager*        mapEngineManager(void)          { return _mapEngineManager; }
    QGCImageProvider*           imageProvider()                 { return _imageProvider; }
    QGCPositionManager*         qgcPositionManager(void)        { return _qgcPositionManager; }
    QGCCorePlugin*              corePlugin(void)                { return _corePlugin; }
    SettingsManager*            settingsManager(void)           { return _settingsManager; }

private:
    void setChildToolboxes(void);
    void _scanAndLoadPlugins(QGCApplication *app);

    FactSystem*                 _factSystem;
    QGCImageProvider*           _imageProvider;
    QGCMapEngineManager*        _mapEngineManager;
    QGCPositionManager*         _qgcPositionManager;
    QGCCorePlugin*              _corePlugin;
    SettingsManager*            _settingsManager;
    friend class QGCApplication;
};

/// This is the base class for all tools
class QGCTool : public QObject {
    Q_OBJECT

public:
    // All tools must be parented to the QGCToolbox and go through a two phase creation. In the constructor the toolbox
    // should only be passed to QGCTool constructor for correct parenting. It should not be referenced or set in the
    // protected member. Then in the second phase of setToolbox calls is where you can reference the toolbox.
    QGCTool(QGCApplication* app, QGCToolbox* toolbox);

    // If you override this method, you must call the base class.
    virtual void setToolbox(QGCToolbox* toolbox);

protected:
    QGCApplication* _app;
    QGCToolbox*     _toolbox;
};

#endif
