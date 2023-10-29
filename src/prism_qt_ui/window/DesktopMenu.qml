import QtQml 2.15
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4 as Q1
import QtQuick.Controls.Styles 1.4
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import ".."
import "../shapes"
import prismCpp 1.0
import QtQml 2.12

import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12

Q1.Menu {
    id: menu
    __xOffset: xOffset
    __yOffset: yOffset
    property real iconLM: 0
    property real iconRM: 10
    property real iconWidth: 20
    property real iconHeight: 20
    property color iconColor: Style.black50
    property real iconOpacity: 1.0
    property real labelWidth: 0
    property real popupWidth: __contentItem.width
    property real itemsHeight: 30
    property real shadowThickness: 5
    property real redius: shadowThickness
    property real xOffset: 0
    property real yOffset: 0
    property var popwin
    property var splist: []
    property bool showIcon: false
    property font font: Style.h7
    property bool iconMipmap: false
    property bool isOpen: false
    property Component itemsComponent: RowLayout {
        id: itemComponentRoot
        property var menuItem: menu.items[svg.parent.parent.data.index]
        enabled: menuItem.enabled
        SvgIcon {
            id: svg
            visible: showIcon
            width: itemComponentRoot.menuItem.iconWidth
            height: itemComponentRoot.menuItem.iconHeight
            Layout.preferredWidth: width
            Layout.preferredHeight: height
            Layout.rightMargin: itemComponentRoot.menuItem.iconRM
            Layout.leftMargin: itemComponentRoot.menuItem.iconLM
            svgPath: parent.parent.data.iconSource
            color: itemComponentRoot.menuItem.iconColor
            opacity: itemComponentRoot.menuItem.iconOpacity
            mipmap: itemComponentRoot.menuItem.iconMipmap
            //Component.onCompleted: {
            //    console.log("==================")
            //    console.log(tex.text)
            //    console.log(itemComponentRoot.menuItem.text)
            //    console.log(itemComponentRoot.menuItem.iconRM)
            //    console.log(itemComponentRoot.menuItem.iconLM)
            //    console.log(itemComponentRoot.menuItem.iconWidth)
            //    console.log(itemComponentRoot.menuItem.iconMipmap)
            //    console.log(itemComponentRoot.menuItem.iconColor)
            //}
        }

        Text {
            id: tex
            text: parent.parent.data.text
            font: menu.font
            color:  parent.enabled? Style.black80 :Style.gray80
        }
    }

    function drawLine() {
        JsEx.delay(menu, 50, function () {
            menu.splist.forEach(function (item) {
                item.drawLine()
            })
            if (popwin)
                popwin.opacity = 1.0
        })
    }
    onAboutToHide: {
        splist = []
        isOpen = false
    }

    onAboutToShow: {
        isOpen = true
        popwin = MenuHelper.setMenuStyle(menu, menu.shadowThickness, true)

    }
    style: MenuStyle {
        submenuOverlap: menu.redius * 2
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
            }
        }

        itemDelegate.background: ColumnLayout {
            spacing: 0
            height: menu.itemsHeight + (styleData.index === 0 ? menu.redius * 2 + 2.5 : 0)
                    + (styleData.index === menu.items.length - 1 ? menu.redius * 2 + 2.5 : 0)
            x: -1

            Item {
                Layout.preferredHeight: styleData.index === 0 ? menu.redius * 2 + 2.5 : 0
            }
            Rectangle {
                id: bg
                color: styleData.selected ? "#F2F2F2" : "transparent"
                Layout.leftMargin: menu.shadowThickness * 2
                Layout.rightMargin: menu.shadowThickness * 2 - 2
                Layout.fillWidth: true
                Layout.preferredHeight: menu.itemsHeight
            }
            Item {
                Layout.preferredHeight: styleData.index === menu.items.length
                                        - 1 ? menu.redius * 2 + 2.5 : 0
            }
        }

        itemDelegate.submenuIndicator: ColumnLayout {
            spacing: 0
            x: -menu.redius
            width: submenuIndicator.width + menu.redius
            height: menu.itemsHeight + (styleData.index === 0 ? menu.redius * 2 + 2.5 : 0)
                    + (styleData.index === menu.items.length - 1 ? menu.redius * 2 + 2.5 : 0)
            Item {
                Layout.preferredHeight: styleData.index === 0 ? menu.redius * 2 + 2.5 : 0
            }
            Item {
                id: submenuIndicator
                width: 10
                Layout.preferredHeight: menu.itemsHeight
                SvgIcon {
                    width: 7
                    height: 10
                    color: Style.black
                    mipmap: true
                    anchors.centerIn: parent
                    svgPath: "qrc:/prism_qt_ui/svg/play-arrow.svg"
                }
            }
            Item {
                Layout.preferredHeight: styleData.index === menu.items.length
                                        - 1 ? menu.redius * 2 + 2.5 : 0
            }
        }
        itemDelegate.shortcut: ColumnLayout {
            spacing: 0
            x: -menu.redius
            width: tb.width + menu.redius
            height: menu.itemsHeight + (styleData.index === 0 ? menu.redius * 2 + 2.5 : 0)
                    + (styleData.index === menu.items.length - 1 ? menu.redius * 2 + 2.5 : 0)
            Item {
                Layout.preferredHeight: styleData.index === 0 ? menu.redius * 2 + 2.5 : 0
            }
            Item {
                Layout.preferredHeight: menu.itemsHeight
                Layout.fillWidth: true
                Text {
                    id: tb
                    text: styleData.shortcut
                    opacity: 0.5
                    font: menu.font

                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            Item {
                Layout.preferredHeight: styleData.index === menu.items.length
                                        - 1 ? menu.redius * 2 + 2.5 : 0
            }
        }

        itemDelegate.label: ColumnLayout {
            spacing: 0
            id: itemRoot
            x: menu.redius
            width: labelWidth > 0 ? labelWidth : loader.width + 30

            height: menu.itemsHeight + (styleData.index === 0 ? menu.redius * 2 + 2.5 : 0)
                    + (styleData.index === menu.items.length - 1 ? menu.redius * 2 + 2.5 : 0)
            Item {
                Layout.preferredHeight: styleData.index === 0 ? menu.redius * 2 + 2.5 : 0
            }
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: menu.itemsHeight
                Loader {
                    id: loader
                    anchors.verticalCenter: parent.verticalCenter
                    property var data: styleData
                    sourceComponent: itemsComponent
                }
            }
            Item {
                Layout.preferredHeight: styleData.index === menu.items.length
                                        - 1 ? menu.redius * 2 + 2.5 : 0
            }
        }

        separator: ColumnLayout {
            id: s
            spacing: 0
            height: 4
            width: menu.popupWidth

            Component.onCompleted: {
                splist.push(s)
            }
            function drawLine() {
                Qt.createQmlObject(
                            "import QtQuick 2.15;import QtQuick.Layouts 1.15;  Rectangle{ Layout.alignment: Qt.AlignCenter; color: \"#F2F2F2\"; height:1;width: menu.popupWidth - (shadowThickness*2 ) * 2 ;} ",
                            s)
            }
        }
        scrollIndicator: RowLayout {
            visible: false
            height: 0
            Rectangle {
                color: "#33808080"
                visible: styleData.scrollerDirection !== Qt.NoArrow
                Layout.fillWidth: true
                Layout.preferredHeight: 0
                Layout.alignment: Layout.Center
                Text {
                    text: "^"
                    rotation: styleData.scrollerDirection === Qt.DownArrow ? 180 : 0
                    anchors.centerIn: parent
                }
            }
        }
    }
}
