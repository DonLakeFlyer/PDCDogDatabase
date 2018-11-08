 /****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "FactSystem.h"
#include "QGCImageProvider.h"
#include "QGCMapEngineManager.h"
#include "PositionManager.h"
#include "QGCCorePlugin.h"
#include "QGCOptions.h"
#include "SettingsManager.h"
#include "QGCApplication.h"
#include "PDCDatabase.h"

#if defined(QGC_CUSTOM_BUILD)
#include CUSTOMHEADER
#endif

QGCToolbox::QGCToolbox(QGCApplication* app)
    : _factSystem           (NULL)
    , _imageProvider        (NULL)
    , _mapEngineManager     (NULL)
    , _qgcPositionManager   (NULL)
    , _corePlugin           (NULL)
    , _settingsManager      (NULL)
    , _pdcDatabase          (NULL)
{
    // SettingsManager must be first so settings are available to any subsequent tools
    _settingsManager =          new SettingsManager(app, this);

    //-- Scan and load plugins
    _scanAndLoadPlugins(app);
    _factSystem =           new FactSystem          (app, this);
    _imageProvider =        new QGCImageProvider    (app, this);
    _mapEngineManager =     new QGCMapEngineManager (app, this);
    _qgcPositionManager =   new QGCPositionManager  (app, this);
    _pdcDatabase =          new PDCDatabase         (app, this);
}

void QGCToolbox::setChildToolboxes(void)
{
    // SettingsManager must be first so settings are available to any subsequent tools
    _settingsManager->setToolbox(this);

    _corePlugin->setToolbox(this);
    _factSystem->setToolbox(this);
    _imageProvider->setToolbox(this);
    _mapEngineManager->setToolbox(this);
    _qgcPositionManager->setToolbox(this);
    _pdcDatabase->setToolbox(this);
}

void QGCToolbox::_scanAndLoadPlugins(QGCApplication* app)
{
#if defined (QGC_CUSTOM_BUILD)
    //-- Create custom plugin (Static)
    _corePlugin = (QGCCorePlugin*) new CUSTOMCLASS(app, app->toolbox());
    if(_corePlugin) {
        return;
    }
#endif
    //-- No plugins found, use default instance
    _corePlugin = new QGCCorePlugin(app, app->toolbox());
}

QGCTool::QGCTool(QGCApplication* app, QGCToolbox* toolbox)
    : QObject(toolbox)
    , _app(app)
    , _toolbox(NULL)
{
}

void QGCTool::setToolbox(QGCToolbox* toolbox)
{
    _toolbox = toolbox;
}
