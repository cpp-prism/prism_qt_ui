import QtQml 2.15
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4 as Q1
import QtQuick.Controls.Styles 1.4
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import ".."
import "../shapes"
//import prism_qt_ui 1.0
import prismCpp 1.0

Q1.Menu {
    id: menu
    property real width: 200
    property real height: 200
    property real redius: 5
    property real shadowThickness: 5
    property real xOffset: 0
    property real yOffset: 0
    property var popwin
    default property Component content
    function setPopupSize() {}
    onWidthChanged: {
        __contentItem.width = width
    }
    onHeightChanged: {
        __contentItem.height = menu.height
    }

    __xOffset: xOffset
    __yOffset: yOffset
    function prismTimer() {
        return Qt.createQmlObject("import QtQuick 2.0; Timer {}", menu)
    }
    function delay(delayTime, cb) {
        var timer = prismTimer()
        timer.interval = delayTime
        timer.repeat = false
        timer.triggered.connect(cb)
        timer.start()
    }
    onAboutToShow: {
        popwin = MenuHelper.setMenuStyle(menu, menu.shadowThickness, false)
        delay(50, function () {
            if (popwin)
                popwin.opacity = 1.0
        })
    }
    style: MenuStyle {
        frame: ColumnLayout {
            id: root
            spacing: 0
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.margins: menu.shadowThickness
                CusCornersRec {
                    id: effectSrc
                    visible: false
                    anchors.fill: parent
                    color: "#33808080"
                    radius: menu.redius * 3
                    topLeftCorner: true
                    bottomLeftCorner: true
                    topRightCorner: true
                    bottomRightCorner: true
                }
                FastBlur {
                    transparentBorder: true
                    anchors.fill: parent
                    radius: menu.shadowThickness * 3
                    source: effectSrc
                }
                CusCornersRec {
                    color: "white"
                    anchors.fill: parent
                    anchors.margins: menu.shadowThickness
                    radius: menu.redius
                    topLeftCorner: true
                    bottomLeftCorner: true
                    topRightCorner: true
                    bottomRightCorner: true
                }
                Loader {
                    id: loader
                    anchors.fill: parent
                    anchors.margins: menu.shadowThickness * 2
                    sourceComponent: content
                    onLoaded: {
                        menu.__contentItem.height = menu.height + shadowThickness * 2 + 2
                        menu.__contentItem.width = menu.width + shadowThickness * 2
                    }
                }
            }
        }
    }
}
