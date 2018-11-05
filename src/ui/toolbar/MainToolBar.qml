/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick              2.3
import QtQuick.Layouts      1.2
import QtQuick.Controls     1.2

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.Palette               1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Controllers           1.0

Rectangle {
    id:         toolBar
    color:      qgcPal.globalTheme === QGCPalette.Light ? Qt.rgba(1,1,1,0.8) : Qt.rgba(0,0,0,0.75)

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    signal showSettingsView
    signal showSetupView
    signal showPlanView
    signal showFlyView
    signal showAnalyzeView
    signal armVehicle
    signal disarmVehicle
    signal vtolTransitionToFwdFlight
    signal vtolTransitionToMRFlight

    function checkSettingsButton() {
        settingsButton.checked = true
    }

    function checkSetupButton() {
        setupButton.checked = true
    }

    function checkPlanButton() {
        planButton.checked = true
    }

    function checkFlyButton() {
        flyButton.checked = true
    }

    function checkAnalyzeButton() {
        analyzeButton.checked = true
    }

    Component.onCompleted: {
        //-- TODO: Get this from the actual state
        flyButton.checked = true
    }

    // Prevent all clicks from going through to lower layers
    DeadMouseArea {
        anchors.fill: parent
    }

    /// Bottom single pixel divider
    Rectangle {
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        height:         1
        color:          "black"
        visible:        qgcPal.globalTheme === QGCPalette.Light
    }

    RowLayout {
        anchors.bottomMargin:   1
        anchors.rightMargin:    ScreenTools.defaultFontPixelWidth / 2
        anchors.fill:           parent
        spacing:                ScreenTools.defaultFontPixelWidth * 2

        //---------------------------------------------
        // Toolbar Row
        Row {
            id:                 viewRow
            Layout.fillHeight:  true
            spacing:            ScreenTools.defaultFontPixelWidth / 2

            ExclusiveGroup { id: mainActionGroup }

            QGCToolBarButton {
                id:                 settingsButton
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                exclusiveGroup:     mainActionGroup
                source:             "/res/QGCLogoWhite"
                logo:               true
                onClicked:          toolBar.showSettingsView()
                visible:            !QGroundControl.corePlugin.options.combineSettingsAndSetup
            }

            QGCToolBarButton {
                id:                 setupButton
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                exclusiveGroup:     mainActionGroup
                source:             "/qmlimages/Gears.svg"
                onClicked:          toolBar.showSetupView()
            }

            QGCToolBarButton {
                id:                 planButton
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                exclusiveGroup:     mainActionGroup
                source:             "/qmlimages/Plan.svg"
                onClicked:          toolBar.showPlanView()
            }

            QGCToolBarButton {
                id:                 flyButton
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                exclusiveGroup:     mainActionGroup
                source:             "/qmlimages/PaperPlane.svg"
                onClicked:          toolBar.showFlyView()
            }

            QGCToolBarButton {
                id:                 analyzeButton
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                exclusiveGroup:     mainActionGroup
                source:             "/qmlimages/Analyze.svg"
                visible:            !ScreenTools.isMobile && QGroundControl.corePlugin.showAdvancedUI
                onClicked:          toolBar.showAnalyzeView()
            }
        }
    }
}
