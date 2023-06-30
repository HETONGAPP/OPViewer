
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.1
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
ApplicationWindow {
    id: window
    visible: true
    width: 850
    height: 600
    minimumWidth: 850
    minimumHeight: 600
    title: "OPC UA Viewer"
    color: "#333333"

    MenuBar {
        id: menuBar

        Menu { title: qsTr("File")
            Action {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }

        Menu { title: qsTr("Help")
            Action {
                text: qsTr("About")
                onTriggered: {
                    var dialog = Qt.createQmlObject('import QtQuick.Dialogs 1.3;
                    MessageDialog { text: "OPViewer Version-1.0.0"; icon: StandardIcon.Information; }'
                    , window);
                    dialog.open();
                }
            }
        }

        delegate: MenuBarItem {
            id: menuBarItem

            contentItem: Text {
                text: menuBarItem.text
                //font: menuBarItem.font
                font.family: "Ubuntu"
                font.pixelSize: 16
                font.bold: true
                opacity: enabled ? 1.0 : 0.3
                color: menuBarItem.highlighted ? "#ffffff" : "#21be2b"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                implicitWidth: 40
                implicitHeight: 40
                opacity: enabled ? 1 : 0.3
                color: menuBarItem.highlighted ? "#778899" : "transparent"
            }
        }

        background: Rectangle {
            implicitWidth: 40
            implicitHeight: 40
            color: "#333333"

            Rectangle {
                color: "#333333"
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
            }
        }
    }
    Seperator
    {
        anchors.top: menuBar.bottom
    }
    ColumnLayout
    {
        anchors.top: menuBar.bottom
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.right: parent.right
        anchors.rightMargin: 6
        height: window.height - menuBar.height-6
        spacing: 6
        ConnectBar
        {
           id: _connectBar
        }
        Seperator{}
        TreeView
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        Seperator{}
        Label
        {
            text: "Log"
            color: "#ffffff"
            font.family: "Ubuntu"
            font.pixelSize: 12
        }
        Seperator{}

        ConsoleLog{
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        Seperator{}
    }
}



