import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQml 2.15
import PrismCpp 1.0
import PrismUI 1.0

Loader {
    id: rootLoader
    property bool showButton: true
    property alias debugBtn :btn
    function updateUrl()
    {
        var url = rootLoader.source
        rootLoader.source = ""
        CppUtility.clearQmlCache()
        rootLoader.source = url
    }
    Button {
        id:btn
        width: 100
        height: 30
        focusPolicy: Qt.NoFocus
        visible: CppUtility.enableHotReload() && rootLoader.showButton
        x:parent.width/2 - width/2
        text: "hot reload"
        z: 100
        onClicked: {
            rootLoader.updateUrl()
        }
    }
}
