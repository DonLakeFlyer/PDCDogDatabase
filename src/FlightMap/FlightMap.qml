/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.3
import QtQuick.Controls 1.2
import QtLocation       5.3
import QtPositioning    5.3
import QtQuick.Dialogs  1.2

import QGroundControl                       1.0
import QGroundControl.FactSystem            1.0
import QGroundControl.Controls              1.0
import QGroundControl.FlightMap             1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.QGCPositionManager    1.0

Map {
    id: _map

    zoomLevel:                  QGroundControl.flightMapZoom
    center:                     QGroundControl.flightMapPosition
    //-- Qt 5.9 has rotation gesture enabled by default. Here we limit the possible gestures.
    gesture.acceptedGestures:   MapGestureArea.PinchGesture | MapGestureArea.PanGesture | MapGestureArea.FlickGesture
    gesture.flickDeceleration:  3000
    plugin:                     Plugin { name: "QGroundControl" }

    property string mapName:                        'defaultMap'
    property bool   isSatelliteMap:                 activeMapType.name.indexOf("Satellite") > -1 || activeMapType.name.indexOf("Hybrid") > -1
    property var    gcsPosition:                    QGroundControl.qgcPositionManger.gcsPosition
    property bool   userPanned:                     false   ///< true: the user has manually panned the map
    property bool   allowGCSLocationCenter:         false   ///< true: map will center/zoom to gcs location one time
    property bool   firstGCSPositionReceived:       false   ///< true: first gcs position update was responded to
    property var    qgcView

    readonly property real  maxZoomLevel: 20

    function setVisibleRegion(region) {
        // TODO: Is this still necessary with Qt 5.11?
        // This works around a bug on Qt where if you set a visibleRegion and then the user moves or zooms the map
        // and then you set the same visibleRegion the map will not move/scale appropriately since it thinks there
        // is nothing to do.
        _map.visibleRegion = QtPositioning.rectangle(QtPositioning.coordinate(0, 0), QtPositioning.coordinate(0, 0))
        _map.visibleRegion = region
    }

    function centerToSpecifiedLocation() {
        qgcView.showDialog(specifyMapPositionDialog, qsTr("Specify Position"), qgcView.showDialogDefaultWidth, StandardButton.Close)
    }

    Component {
        id: specifyMapPositionDialog

        EditPositionDialog {
            coordinate:             center
            onCoordinateChanged:    center = coordinate
        }
    }

    ExclusiveGroup { id: mapTypeGroup }

    // Center map to gcs location
    onGcsPositionChanged: {
        if (gcsPosition.isValid && allowGCSLocationCenter && !firstGCSPositionReceived) {
            firstGCSPositionReceived = true
            center = gcsPosition
            zoomLevel = QGroundControl.flightMapInitialZoom
        }
    }

    // We track whether the user has panned or not to correctly handle automatic map positioning
    Connections {
        target: gesture

        onPanFinished:      userPanned = true
        onFlickFinished:    userPanned = true
    }

    function updateActiveMapType() {
        var settings =  QGroundControl.settingsManager.flightMapSettings
        var fullMapName = settings.mapProvider.enumStringValue + " " + settings.mapType.enumStringValue
        for (var i = 0; i < _map.supportedMapTypes.length; i++) {
            if (fullMapName === _map.supportedMapTypes[i].name) {
                _map.activeMapType = _map.supportedMapTypes[i]
                return
            }
        }
    }

    Component.onCompleted: {
        updateActiveMapType()
    }

    Connections {
        target:             QGroundControl.settingsManager.flightMapSettings.mapType
        onRawValueChanged:  updateActiveMapType()
    }

    Connections {
        target:             QGroundControl.settingsManager.flightMapSettings.mapProvider
        onRawValueChanged:  updateActiveMapType()
    }

    /// Ground Station location
    MapQuickItem {
        anchorPoint.x:  sourceItem.width / 2
        anchorPoint.y:  sourceItem.height / 2
        visible:        gcsPosition.isValid
        coordinate:     gcsPosition

        sourceItem: Image {
            source:         "/res/QGCLogoFull"
            mipmap:         true
            antialiasing:   true
            fillMode:       Image.PreserveAspectFit
            height:         ScreenTools.defaultFontPixelHeight * 1.75
            sourceSize.height: height
        }
    }
} // Map
