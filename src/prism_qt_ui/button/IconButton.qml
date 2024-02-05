import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtGraphicalEffects 1.0
import "../shapes"
import prismCpp 1.0
import prism_qt_ui 1.0

Item {
    id: btn
    property color color: "white"
    property color hoveredColor: "white"
    property url icon: ""
    property alias ma :ma
    property alias mipmap: svg.mipmap
    property alias isEnable: ma.enabled
    property alias containsMouse: ma.containsMouse
    property alias cursorShape: ma.cursorShape
    signal clicked(var mouse)

    SvgIcon {
        id: svg
        anchors.fill: parent
        svgPath: btn.icon
        color: ma.containsMouse ? btn.hoveredColor : btn.color
    }

    //Image {
    //    id: img
    //    source: btn.icon
    //    anchors.fill: parent
    //}
    //ColorOverlay {
    //    anchors.fill: img
    //    source: img
    //    color: ma.containsMouse ? btn.hoveredColor : btn.color
    //}
    MouseArea {
        anchors.fill: svg
        id: ma
        enabled: btn.enabled
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            btn.clicked(mouse)
        }
    }
}
