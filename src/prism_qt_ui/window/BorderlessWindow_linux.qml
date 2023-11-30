import QtQuick 2.0
import QtQuick.Window 2.12
import prismCpp 1.0
import QtQuick.Layouts 1.12
import prismCpp 1.0
import prism_qt_ui 1.0
import dv.viewmodels 1.0

Window {
    id: window
    visible: true
    title: qsTr("prism")
    color: "white"
    //flags: Qt.Window | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint
    //       | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint |Qt.FramelessWindowHint
    minimumWidth: 1030
    minimumHeight: 670

    Rectangle{
        color: window.color
        anchors.fill: parent
        LiveLoader {
            id: window_rootLoader
            anchors.fill: parent
            source: contentUrl
        }
    }
}
