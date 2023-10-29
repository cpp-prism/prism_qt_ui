import QtQuick 2.0
import QtQuick.Window 2.12
import prismCpp 1.0
import QtQuick.Layouts 1.12

Window {
    id: window
    visible: true
    title: qsTr("prism")
    color: "white"
    flags: Qt.Window | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint
           | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint
    minimumWidth: 1030
    minimumHeight: 670

    ColumnLayout {
        anchors.fill: parent
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            color: "pink"
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
