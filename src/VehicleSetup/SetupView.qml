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
import QtQuick.Layouts  1.2

import QGroundControl                       1.0
import QGroundControl.Palette               1.0
import QGroundControl.Controls              1.0
import QGroundControl.ScreenTools           1.0

QGCView {
    id:         _qgcView
    viewPanel:  panel
    z:          QGroundControl.zOrderTopMost

    readonly property real _defaultTextHeight: ScreenTools.defaultFontPixelHeight
    readonly property real _defaultTextWidth:  ScreenTools.defaultFontPixelWidth
    readonly property real _margins:            _defaultTextWidth / 2
    readonly property real _buttonWidth:       _defaultTextWidth * 18

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    ExclusiveGroup { id: setupButtonGroup }

    QGCViewPanel {
        id:             panel
        anchors.fill:   parent

        QGCFlickable {
            id:                 buttonScroll
            anchors.margins:    _margins
            anchors.left:       parent.left
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            width:              buttonColumn.width
            contentHeight:      buttonColumn.height
            flickableDirection: Flickable.VerticalFlick
            clip:               true

            ColumnLayout {
                id:         buttonColumn
                spacing:    _defaultTextHeight / 2

                SubMenuButton {
                    id:                 packButton
                    imageResource:      "/qmlimages/VehicleSummaryIcon.png"
                    setupIndicator:     false
                    checked:            true
                    exclusiveGroup:     setupButtonGroup
                    text:               qsTr("Packs")
                    Layout.fillWidth:   true

                    onClicked: showSummaryPanel()
                }

                SubMenuButton {
                    id:                 dogButton
                    imageResource:      "/qmlimages/FirmwareUpgradeIcon.png"
                    setupIndicator:     false
                    exclusiveGroup:     setupButtonGroup
                    text:               qsTr("Dogs")
                    Layout.fillWidth:   true

                    onClicked: showFirmwarePanel()
                }

                SubMenuButton {
                    id:                 photoButton
                    exclusiveGroup:     setupButtonGroup
                    setupIndicator:     false
                    text:               qsTr("Photos")
                    Layout.fillWidth:   true

                    onClicked:      showPX4FlowPanel()
                }
            }
        }

        Loader {
            id:                 panelLoader
            anchors.margins:    _margins
            anchors.left:       buttonScroll.right
            anchors.right:      parent.right
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            source:             "qrc:/PDC/PackEditor.qml"
        }
    }
}
