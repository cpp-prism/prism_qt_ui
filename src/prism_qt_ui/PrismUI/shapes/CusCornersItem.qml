import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12
import PrismUI 1.0
import PrismCpp 1.0

import QtQml 2.15
import QtQuick.Effects

Item{
    property alias radius :mask_reec.radius
    property alias topLeftCorner: mask_reec.topLeftCorner
    property alias topRightCorner: mask_reec.topRightCorner
    property alias bottomRightCorner:  mask_reec.bottomRightCorner
    property alias bottomLeftCorner:  mask_reec.bottomLeftCorner
    property alias sourceComponent:sourceItem.sourceComponent
    Loader {
        id: sourceItem
        source: ""

        //anchors.centerIn: parent
        anchors.fill: parent
        visible: false
    }

    MultiEffect {
        source: sourceItem
        anchors.fill: sourceItem
        maskEnabled: true
        maskSource: mask
    }

    Item {
        id: mask
        width: sourceItem.width
        height: sourceItem.height
        layer.enabled: true
        layer.smooth: true
        visible: false

        CusCornersRec {
            id:mask_reec
            width: sourceItem.width
            height: sourceItem.height
            radius: 20
            color: "black"
        }
    }

}
