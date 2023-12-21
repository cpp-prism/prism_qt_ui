import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12
import prismCpp 1.0
import prism_qt_ui 1.0

BorderLessWindow_Win_cpp {
    id: window
    property real exceed_pixels: Math.round(8 / Screen.devicePixelRatio)
    property url contentUrl

    visible: false
    opacity: 0
    title: qsTr("prism")
    color: "transparent"
    flags: Qt.Window | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint
           | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint
    minimumWidth: 1030
    minimumHeight: 670

    Component.onCompleted: {
        width = minimumWidth
        height = minimumHeight
        x = Screen.width / 2 - width / 2
        y = Screen.height / 2 - height / 2
        CppUtility.enableWindowBorderless(window)
        JsEx.delay(window, 500, function () {
            window.opacity = 1
            window.visible = true
            window.visibility = Window.Windowed
        })
    }

    onVisibilityChanged: {
        //todo  如果是windows7 basic 应该不需要这个逻辑,待测试

        // tricks for maximized window on Microsoft Windows, if not do so, the window will exceed the monitor with about 8 pixels.
        var is_maximized = window.visibility === Window.Maximized
        window_rootLoader.anchors.margins = is_maximized ? exceed_pixels : 0
    }
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
