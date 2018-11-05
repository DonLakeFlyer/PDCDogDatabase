/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#ifndef AppSettings_H
#define AppSettings_H

#include "SettingsGroup.h"
#include "SettingsFact.h"

class AppSettings : public SettingsGroup
{
    Q_OBJECT
    
public:
    AppSettings(QObject* parent = NULL);

    Q_PROPERTY(Fact* appFontPointSize                   READ appFontPointSize                   CONSTANT)
    Q_PROPERTY(Fact* indoorPalette                      READ indoorPalette                      CONSTANT)
    Q_PROPERTY(Fact* savePath                           READ savePath                           CONSTANT)
    Q_PROPERTY(Fact* mapboxToken                        READ mapboxToken                        CONSTANT)
    Q_PROPERTY(Fact* esriToken                          READ esriToken                          CONSTANT)

    Q_PROPERTY(QString logSavePath          READ logSavePath        NOTIFY savePathsChanged)
    Q_PROPERTY(QString crashSavePath        READ crashSavePath      NOTIFY savePathsChanged)

    Fact* appFontPointSize                  (void);
    Fact* indoorPalette                     (void);
    Fact* savePath                          (void);
    Fact* mapboxToken                       (void);
    Fact* esriToken                         (void);

    QString logSavePath         (void);
    QString crashSavePath         (void);

    static const char* name;
    static const char* settingsGroup;

    static const char* appFontPointSizeName;
    static const char* indoorPaletteName;
    static const char* savePathName;
    static const char* mapboxTokenName;
    static const char* esriTokenName;
    static const char* logFileExtension;

    // Child directories of savePath for specific file types
    static const char* logDirectory;
    static const char* crashDirectory;

signals:
    void savePathsChanged(void);

private slots:
    void _indoorPaletteChanged(void);
    void _checkSavePathDirectories(void);

private:
    SettingsFact* _appFontPointSizeFact;
    SettingsFact* _indoorPaletteFact;
    SettingsFact* _savePathFact;
    SettingsFact* _mapboxTokenFact;
    SettingsFact* _esriTokenFact;
};

#endif
