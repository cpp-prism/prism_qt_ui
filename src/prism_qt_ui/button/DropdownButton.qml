import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.4 as Q1
import prismCpp 1.0
import prism_qt_ui 1.0
Rectangle{
    id:root
    property alias font : tb.font
    color: contextMenu ?(contextMenu.isOpen? Style.lightgray100 :btn_menu_file.containsMouse? Style.lightgray40 :"transparent"):"transparent"
    default property var  contextMenu
    property alias text: tb.text
    onContextMenuChanged: {
        contextMenu.showIcon= true
        contextMenu.shadowThickness= 5
    }

    Text {
        id:tb
        anchors.fill: parent
        font: Style.h7
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    MouseArea{
        id:btn_menu_file
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            if(contextMenu)
            {
                contextMenu.xOffset=   -btn_menu_file.mouseX -4
                contextMenu.yOffset=  -btn_menu_file.mouseY + btn_menu_file.height - 5
            }
            contextMenu.popup()
        }
        //DesktopMenu {
        //    id: contextMenu
        //    showIcon: true
        //    shadowThickness: 5
        //    xOffset:   -btn_menu_file.mouseX -4
        //    yOffset:  -btn_menu_file.mouseY + btn_menu_file.height - 8


        //}
    }
}
