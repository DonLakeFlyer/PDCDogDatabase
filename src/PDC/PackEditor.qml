import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts  1.2
import QtQuick.Dialogs  1.2

import QGroundControl               1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.PDCDatabase   1.0


Rectangle {
    color: "red"//qgcPal.window

    property PackModel _packModel: QGroundControl.pdcDatabase.packModel

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    Row {
        anchors.fill: parent

        TableView {
            id:     packTable
            model:  _packModel

            TableViewColumn {
                role: "name"
                title: "Name"
                width: 100
            }
        }

        Column {
            spacing: ScreenTools.defaultFontPixelHeight

            QGCButton {
                text:       "Add"
                onClicked:  _qgcView.showDialog(addPackDialog, "Add Pack", _qgcView.showDialogDefaultWidth, StandardButton.Cancel | StandardButton.Ok)
            }

            QGCButton {
                text: "Delete"
                onClicked: _packModel.deletePack(packTable.currentRow)
            }
        }
    }

    Component {
        id: addPackDialog

        QGCViewDialog {

            function accept() {
                errorLabel.text = _packModel.addPack(packNameField.text)
                if (errorLabel.text === "") {
                    hideDialog()
                }
            }

            Column {
                QGCLabel { text: "Pack Name" }

                QGCTextField {
                    id: packNameField
                }

                QGCLabel {
                    id:     errorLabel
                    color:  qgcPal.warningText
                }
            }

        }
    }
}
