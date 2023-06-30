import QtQuick.Layouts 1.3
import Qt.labs.platform 1.1
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import OpcUaModel 1.0
import ConnectServer 1.0
GridLayout
{
    id: connectionGridLayout

    columns: 3
    rowSpacing: 6
    columnSpacing: 10
    implicitWidth: parent.width
    implicitHeight: label1.height

    Label {
        id: label1
        Layout.fillWidth: true
        text: "Select OPC UA Backend:"
        font.pixelSize: 14
        font.family: "Ubuntu"
        color: "#ffffff"
    }
    ComboBox{
        displayText:"open62541"
        //model: [ "open62541" ]
        model: [ "" ]
        Layout.fillWidth: true
        font.pixelSize: 14
        font.family: "Ubuntu"
        background: Rectangle
        {
            color:"#ffffff"
            radius:3
        }
        delegate: Text {
            width: parent.width
            height: 30
            text: modelData
            color: "#ffffff"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
    Label {
        text: "    "
        width: 100
    }
    Label {
        id: label2
        Layout.fillWidth: true
        text: "Select host to discover:"
        font.pixelSize: 14
        font.family: "Ubuntu"
        color: "#ffffff"
    }

    TextField {
        text: "opc.tcp://localhost:48010"
        placeholderText: qsTr("opc.tcp://localhost:48010")
        Layout.fillWidth: true
        implicitWidth: parent.width*0.6
        font.pixelSize: 14
        font.family: "Ubuntu"
        enabled: true
        focus: true
        background: Rectangle
        {
            color:"#ffffff"
            radius:3
        }
    }

    Button {
        id:control_server
        text: "Find Servers"
        Layout.fillWidth: true
        contentItem: Text {
            text: control_server.text
            font: control_server.font
            opacity: enabled ? 1.0 : 0.3
            color: control_server.down || control_server.hovered ? "#ffffff" : "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            implicitWidth: 50
            implicitHeight: 14
            opacity: enabled ? 1 : 0.3
            color: control_server.down || control_server.hovered ? "#21be2b" : "#708090"
            border.width: 1
            radius: 2
        }
        onClicked: connectServer.findServers()
    }

    Label {
        id: label3
        Layout.fillWidth: true
        text: "Select host to discover:"
        font.pixelSize: 14
        font.family: "Ubuntu"
        color: "#ffffff"
    }



    ComboBox{
        displayText:"Server Name"
        //model: [ "open62541" ]
        enabled: false
        model: [ "" ]
        Layout.fillWidth: true
        font.pixelSize: 14
        font.family: "Ubuntu"
        background: Rectangle
        {
            color:"#ffffff"
            radius:3
        }
        delegate: Text {
            width: parent.width
            height: 30
            text: modelData
            color: "#ffffff"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Button {
        id:control_select_server
        text: "Get EndPoints"
        Layout.fillWidth: true
        enabled: true

        contentItem: Text {
            text: control_select_server.text
            font: control_select_server.font
            opacity: enabled ? 1.0 : 0.3
            color: control_select_server.down || control_select_server.hovered ? "#ffffff" : "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            implicitWidth:50
            implicitHeight: 14
            opacity: enabled ? 1 : 0.3
            color: control_select_server.down || control_select_server.hovered ? "#21be2b" : "#708090"
            border.width: 1
            radius: 2
        }
        onClicked: connectServer.getEndpoint()
    }

    Label {
        id: label4
        Layout.fillWidth: true
        text: "Select host to discover:"
        font.pixelSize: 14
        font.family: "Ubuntu"
        color: "#ffffff"
    }



    ComboBox{
        displayText:"EndPoint Name"
        //model: [ "open62541" ]
        enabled: false
        model: [ "" ]
        Layout.fillWidth: true
        font.pixelSize: 14
        font.family: "Ubuntu"
        background: Rectangle
        {
            color:"#ffffff"
            radius:3
        }
        delegate: Text {
            width: parent.width
            height: 30
            text: modelData
            color: "#ffffff"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Button {
        id:control_select_endpoint
        text: "Connect"
        Layout.fillWidth: true
        enabled: true
        contentItem: Text {
            text: control_select_endpoint.text
            font: control_select_endpoint.font
            opacity: enabled ? 1.0 : 0.3
            color: control_select_endpoint.down || control_select_endpoint.hovered ? "#ffffff" : "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            implicitWidth:50
            implicitHeight: 14
            opacity: enabled ? 1 : 0.3
            color: control_select_endpoint.down || control_select_endpoint.hovered ? "#21be2b" : "#708090"
            border.width: 1
            radius: 2
        }
        onClicked: connectServer.connectToServer()
    }

}


