import Qt.labs.platform 1.0
//import QtQuick.Dialogs 1.0
//import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml 2.12
import QtGraphicalEffects 1.0
import prismCpp 1.0
import prism_qt_ui 1.0
import "../shapes"

MouseArea {
    id: emptyableIconButton
    Layout.preferredHeight: 20
    Layout.preferredWidth: 20
    Layout.leftMargin: 5
    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

    property real borderWidth: 1
    property string icon: "qrc:/prismFramework/Svg/heart.svg"
    property bool mipmap: true
    property color borderColor: "lightgray"
    property color fillColor: "red"

    signal clicked(var mouse)

    SvgIcon {
        id: icon_border
        anchors.fill: emptyableIconButton
        mipmap: emptyableIconButton.mipmap
        svgPath: EventsHub.getQmlPath(emptyableIconButton.icon)
        color: emptyableIconButton.borderColor
    }
    SvgIcon {
        anchors.fill: icon_border
        anchors.margins: emptyableIconButton.borderWidth
        color: emptyableIconButton.fillColor
        mipmap: emptyableIconButton.mipmap
        svgPath: EventsHub.getQmlPath(emptyableIconButton.icon)
    }

    MouseArea {
        id: ma
        anchors.fill: icon_border
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            emptyableIconButton.clicked(mouse)
        }
    }
}
