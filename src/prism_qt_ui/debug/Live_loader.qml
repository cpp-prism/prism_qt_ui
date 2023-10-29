import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQml 2.15
import prismCpp 1.0

Loader {
    id: rootLoader
    anchors.fill: parent
    Button {
        width: 100
        height: 30
        focusPolicy: Qt.NoFocus
        visible: CppUtility.enableHotReload()
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: "hot reload"
        z: 100
        opacity: 10
        onClicked: {
            var url = rootLoader.source
            rootLoader.source = ""
            CppUtility.clearQmlCache()
            rootLoader.source = url
        }
    }
}
