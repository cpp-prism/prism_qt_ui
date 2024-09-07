import QtQuick 2.0
import QtQuick.Window 2.12
import prismCpp 1.0
import QtQuick.Layouts 1.12
import ".."

Window {
    id: window
    visible: true
    title: qsTr("网难云音乐")
    color: "white"
    flags: Qt.Window | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint
           | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint

    property url contentUrl

    //minimumWidth: 1030
    //minimumHeight: 670
    onVisibilityChanged: {
        CppUtility.enableWindowBorderless(window)
    }
    Component.onCompleted: {
        CppUtility.enableWindowBorderless(window)
    }
    Rectangle{
        color: window.color
        anchors.fill: parent
        //anchors.margins: window.visibility === Window.Windowed? frameBorderWidth:0
        LiveLoader {
            id: window_rootLoader
            anchors.fill: parent
            source: contentUrl
        }
    }

}
