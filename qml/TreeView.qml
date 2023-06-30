import QtQuick.Controls 1.4
import QtQuick 2.15
import OpcUaModel 1.0
import ConnectServer 1.0
Rectangle {

    property bool sucsValue: false
    color: "grey"
    TreeView {
        id: tree
        anchors.fill: parent
        implicitHeight: parent.height
        implicitWidth: parent.width
        backgroundVisible: true
        TableViewColumn {
            title: "BrowseName"
            role: "fileName"
            width: 300
        }
        TableViewColumn {
            title: "Permissions"
            role: "filePermissions"
            width: 100
        }

        model:  null

        onModelChanged: {
            console.log(tree.model);
            console.log(columnCount);
        }
//        Component.onCompleted: {
//            for (var i = 0; i < header.count; ++i) {
//                header.section(i).resizeMode = HeaderView.ResizeToContents;
//            }
//        }
    }



    Connections {
        target: connectServer // The instance of MyClass

        function onSucsChanged() {
            sucsValue = connectServer.sucs;

            console.log("sucsValue:", sucsValue);
            tree.model =connectServer.returnModel();
            // React to the change in sucs property
        }
    }

}

