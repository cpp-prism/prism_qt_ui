import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtQuick.Effects


Item {
    id: root
    property color color: "lightgray"
    property url svgPath: value
    property alias mipmap: img.mipmap
    property alias padding: img.anchors.margins
    Image {
        id: img
        source: root.svgPath
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        visible: false
    }
    MultiEffect {
        anchors.fill: img
        source: img

        colorization: 1.0        // 开启上色效果
        colorizationColor: root.color
    }
}
