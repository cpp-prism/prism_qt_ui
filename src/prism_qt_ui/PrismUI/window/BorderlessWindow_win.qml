import QtQuick.Controls
import QtQuick.Layouts
import QtQuick
import QtQuick.Window
import PrismCpp 1.0
import PrismUI 1.0

BorderLessWindowWincpp {
    id: window
    property real exceed_pixels: Math.round(8 / Screen.devicePixelRatio)
    property var contentUrl
    property alias showReloadButton: window_rootLoader.showButton
    property alias debugBtn: window_rootLoader.debugBtn


    opacity: 0
    visible: true
    title: qsTr("prism")
    color: "transparent"
    x:9999
    flags: Qt.Window | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint
           | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint | Qt.FramelessWindowHint

    Component.onCompleted: {


        x = Screen.width / 2 - width / 2
        y = Screen.height / 2 - height / 2
        CppUtility.enableWindowBorderless(window)
        JsEx.delay(window, 1, function () {
            window.opacity = 1
            //window.visible = true
            //window.visibility = Window.Windowed
        })
    }

    onVisibilityChanged: {
        //todo  如果是windows7 basic 应该不需要这个逻辑,待测试
        //请使用win32控制窗口状态，不要和qt的showNormal等方法混合使用,否则会抽风
        var is_maximized = window.visibility == Window.Maximized
        // tricks for maximized window on Microsoft Windows, if not do so, the window will exceed the monitor with about 8 pixels.
        window_rootLoader.anchors.margins  = is_maximized ? (exceed_pixels): 0
    }
    Item{
        anchors.fill: parent
        LiveLoader {
            id: window_rootLoader

            anchors.fill: parent
            source: contentUrl
        }
    }

}
