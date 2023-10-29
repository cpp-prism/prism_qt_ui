import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12
import prism_qt_ui 1.0
import prismCpp 1.0

import QtQml 2.15
import QtGraphicalEffects 1.0

Image {
    id: img
    //anchors.fill: parent
    //source: imgSource
    property bool rounded: true
    property bool adapt: true
    property real radius: 10

    layer.enabled: rounded
    layer.effect: OpacityMask {
        maskSource: Item {
            width: img.width
            height: img.height
            Rectangle {
                anchors.centerIn: parent
                width: img.adapt ? img.width : Math.min(img.width, img.height)
                height: img.adapt ? img.height : width
                radius: img.radius
            }
        }
    }
}
