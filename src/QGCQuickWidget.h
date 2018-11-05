/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#ifndef QGCQuickWidget_H
#define QGCQuickWidget_H

#include <QQuickWidget>

/// @file
///     @brief Subclass of QQuickWidget which injects Facts and the Palette object into
///             the QML context.
///
///     @author Don Gagne <don@thegagnes.com>

class QGCQuickWidget : public QQuickWidget {
    Q_OBJECT
    
public:
    QGCQuickWidget(QWidget* parent = NULL);
    
    /// Sets the QML into the control. Will display errors message box if error occurs loading source.
    ///     @return true: source loaded, false: source not loaded, errors occurred
    bool setSource(const QUrl& qmlUrl);
};

#endif
