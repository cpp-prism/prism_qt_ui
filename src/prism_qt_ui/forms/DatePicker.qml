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
import prism_qt_ui_private 1.0
import QtQml 2.12

import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12

DatePickerCpp{
    id:control
    implicitHeight: 30
    implicitWidth: 100
    //Layout.preferredHeight: 30
    //Layout.preferredWidth: 100
    property var maximumDate
    property var minimumDate
    Q1.TextField{
        anchors.fill: parent
        id:tb_calandar
        readOnly: true
        text: Qt.formatDate(control.selectedTime,"yyyy-MM-dd")
    }
    MouseArea{
        anchors.fill: parent
        onClicked:
        {
            pop_calendar.xOffset = -mouseX
            pop_calendar.yOffset = -mouseY + control.height
            pop_calendar.popup()

        }
    }
    DesktopPopup{
        id:pop_calendar
        width: 300
        height: 300
        //yOffset:tb_calandar.height
        //closePolicy: Popup.CloseOnEscape|Popup.CloseOnPressOutside
        Q1.Calendar{
            maximumDate: control.maximumDate
            minimumDate: control.minimumDate
            anchors.fill: parent
            selectedDate: control.selectedTime
            onReleased: function(d){
                d.setHours(0)
                control.setSelectedTime(d)
                pop_calendar.close()
            }

        }
    }

}
