import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import prism_qt_ui 1.0
import prismCpp 1.0

import QtGraphicalEffects 1.0

Item {
    property color color: "lightgray"
    property url svgPath: value
    property alias mipmap: img.mipmap
    property alias padding: img.anchors.margins
    id: root
    Image {
        id: img
        source: root.svgPath
        anchors.fill: parent
    }
    ColorOverlay {
        anchors.fill: img
        source: img
        color: root.color
    }
}
