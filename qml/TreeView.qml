 import QtQuick.Controls 1.4
import QtQuick 2.15


Rectangle {


    color: "grey"
    TreeView {
        implicitHeight: parent.height
        implicitWidth: parent.width
        backgroundVisible: false

        TableViewColumn {
            title: "Name"
            role: "fileName"
            width: 300
        }
        TableViewColumn {
            title: "Permissions"
            role: "filePermissions"
            width: 100
        }
//        model: fileSystemModel
    }
}

