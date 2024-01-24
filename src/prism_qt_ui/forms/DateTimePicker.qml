import QtQml 2.15
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4 as Q1
import QtQuick.Controls.Styles 1.4
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import ".."
import "../shapes"
import "../window"
import prismCpp 1.0
import prism_qt_ui_private 1.0
import QtQml 2.12

import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml.Models 2.15
DatePickerCpp{
    id:control
    implicitHeight: 25
    implicitWidth: 150
    //Layout.preferredHeight: 30
    //Layout.preferredWidth: 100
    property var maximumDate
    property var minimumDate

    //Component.onCompleted: {
    //    if(!selectedTime.isValid || selectedTime.isNull)
    //        setSelectedTime(new Date())
    //}

    Q1.TextField{
        anchors.fill: parent
        id:tb_calandar
        readOnly: true
        text: Qt.formatDateTime(control.selectedTime,"yyyy-MM-dd HH:mm:ss")
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            //pop_calendar.xOffset = -mouseX - pop_calendar.shadowThickness
            //pop_calendar.yOffset = -mouseY + control.height - pop_calendar.shadowThickness
            //pop_calendar.popup()


            pop_calendar.y = control.height
            pop_calendar.open()
        }
    }
    //DesktopPopup{
    DefaultPopup{
        id:pop_calendar
        width: 450
        height: 330
        closePolicy: Popup.CloseOnEscape|Popup.CloseOnPressOutside
        property int cacheRows :998
        property int rowHeight :30

        Rectangle{
            anchors.fill: parent
            RowLayout{
                id:lo_poplayout
                anchors.fill: parent
                spacing: 0

                property var currentDateTime //: control.selectedTime //当弹出popup选择时间，如果不存在，则以时间为准
                property int  preMaxDay : 31
                property int  maxDay : 31
                Component.onCompleted: {
                    currentDateTime = control.selectedTime //当弹出popup选择时间，如果不存在，则以时间为准
                }

                function calculateDaysInMonth(year, month) {
                    // Check if the year is a leap year
                    var isLeapYear = (year % 4 === 0 && year % 100 !== 0) || (year % 400 === 0);

                    // Define the number of days for each month
                    var daysInMonth = [31, isLeapYear ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];

                    // Return the number of days for the specified month
                    return daysInMonth[month - 1];
                }

                function calValue(index,ls_year){
                    var base = (index - ls_year.count/2)
                    if(ls_year.type === 1) //year
                    {
                        var result =  base + lo_poplayout.currentDateTime.getFullYear()
                    }
                    else if(ls_year.type === 2) //month
                    {
                        result =  (base + 1 + lo_poplayout.currentDateTime.getMonth())%12
                        if(result<=0)
                            result +=12
                    }
                    else if(ls_year.type === 3) //day
                    {
                        var days = lo_poplayout.calculateDaysInMonth(control.selectedTime.getYear(),
                                                                     control.selectedTime.getMonth() + 1 )
                        //var days = calculateDaysInMonth(lo_poplayout.currentDateTime.getYear(),
                        //                                lo_poplayout.currentDateTime.getMonth() + 1 )
                        result =  (base+ lo_poplayout.currentDateTime.getDate())%days

                        if(result<=0)
                            result +=days
                    }
                    else if(ls_year.type === 4) // hour
                    {
                        result =  (base +  lo_poplayout.currentDateTime.getHours())%24
                        if(result<0)
                            result +=24
                    }
                    else if(ls_year.type === 5) // minutues
                    {
                        result =  (base  +  lo_poplayout.currentDateTime.getMinutes())%60
                        if(result<0)
                            result +=60
                    }
                    else if(ls_year.type === 6) // second
                    {
                        result =  (base  +  lo_poplayout.currentDateTime.getSeconds())%60
                        if(result<0)
                            result +=60
                    }
                    return result ;
                }

                function calText(index,ls_year){
                    var result = lo_poplayout.calValue(index,ls_year)
                    return  (result < 10 ? "0":"") + result
                }
                property Component comp_listview: Component{
                    ListView {
                        property int type :parent.type
                        id:ls_year
                        clip: true
                        spacing:15
                        model:
                        {
                            return pop_calendar.cacheRows
                            //if(ls_year.type === 1)
                            //    return pop_calendar.cacheRows
                            //else if(ls_year.type === 2)
                            //    return 12*2+4
                            //else if(ls_year.type === 3)
                            //    return 31*2+4
                            //else if(ls_year.type === 4)
                            //    return  24*2+4
                            //else if(ls_year.type === 5)
                            //    return 60*2+4
                            //else if(ls_year.type === 6)
                            //    return 60*2+4
                        }
                        orientation: ListView.Vertical
                        preferredHighlightBegin: height / 2 - ls_year.spacing
                        preferredHighlightEnd: height / 2 + lo_poplayout.spacing
                        currentIndex : count / 2
                        highlightRangeMode: ListView.StrictlyEnforceRange

                        property bool localDay  : control.refreshDate
                        property bool localMonth  : control.refreshMonth
                        onLocalMonthChanged: {
                            //if(localMonth !== control.refreshMonth)
                            {
                                if(ls_year.type === 2 && control.selectedTime && lo_poplayout.currentDateTime)
                                {
                                    //ls_year.highlightMoveDuration = 0
                                    ls_year.currentIndex = ls_year.count/2 + (control.selectedTime.getMonth() - lo_poplayout.currentDateTime.getMonth())
                                    //ls_year.highlightMoveDuration = -1
                                }
                            }
                        }
                        onLocalDayChanged: {
                            //if(localDay !== control.refreshDate)
                            {
                                if(ls_year.type === 3 && control.selectedTime && lo_poplayout.currentDateTime)
                                {
                                    //ls_year.highlightMoveDuration = 0
                                    ls_year.currentIndex = ls_year.count/2 + (control.selectedTime.getDate() - lo_poplayout.currentDateTime.getDate())
                                    //ls_year.highlightMoveDuration = -1
                                }
                            }
                        }


                        Component.onCompleted: {
                            ls_year.highlightMoveDuration = 0
                            currentIndex = count / 2
                            ls_year.highlightMoveDuration =-1

                        }
                        onCurrentIndexChanged: {
                            if(currentIndex===-1)
                                return
                            if(!lo_poplayout.currentDateTime)
                                return
                            var tmp = control.selectedTime
                            if(ls_year.type === 1) //year
                            {
                                lo_poplayout.preMaxDay =  lo_poplayout.calculateDaysInMonth(tmp.getFullYear(),tmp.getMonth()+1)
                                var year = lo_poplayout.calValue(currentIndex,ls_year)
                                lo_poplayout.maxDay =  lo_poplayout.calculateDaysInMonth(year,tmp.getMonth()+1)
                                //var delta = lo_poplayout.maxDay - lo_poplayout.preMaxDay
                                //tmp.setDate(lo_poplayout.preMaxDay - lo_poplayout.maxDay )
                                tmp.setYear(year)

                                control.forceRefreshMonth()
                            }
                            else if(ls_year.type === 2) //month
                            {
                                lo_poplayout.preMaxDay =  lo_poplayout.calculateDaysInMonth(tmp.getFullYear(),tmp.getMonth()+1)
                                var month = lo_poplayout.calValue(currentIndex,ls_year) - 1
                                lo_poplayout.maxDay =  lo_poplayout.calculateDaysInMonth(tmp.getFullYear(),month+1)
                                //console.log("lo_poplayout.preMaxDay : ",lo_poplayout.preMaxDay)
                                //console.log("lo_poplayout.maxDay : ",lo_poplayout.maxDay)
                                //if(lo_poplayout.maxDay < lo_poplayout.preMaxDay)
                                //{
                                //    tmp.setDate(lo_poplayout.preMaxDay - lo_poplayout.maxDay )
                                //    tmp.setMonth(month)
                                //}
                                //else if(lo_poplayout.maxDay > lo_poplayout.preMaxDay)
                                //{
                                //    tmp.setMonth(month)
                                //    tmp.setDate(lo_poplayout.maxDay )
                                //}
                                //else
                                tmp.setMonth(month)
                                control.forceRefreshData()
                            }
                            else if(ls_year.type === 3) //day
                            {
                                tmp.setDate(lo_poplayout.calValue(currentIndex,ls_year ))
                            }
                            else if(ls_year.type === 4) //hour
                                tmp.setHours(lo_poplayout.calValue(currentIndex,ls_year))
                            else if(ls_year.type === 5) //minutes
                                tmp.setMinutes(lo_poplayout.calValue(currentIndex,ls_year))
                            else if(ls_year.type === 6) //seconds
                                tmp.setSeconds(lo_poplayout.calValue(currentIndex,ls_year))

                            control.setSelectedTime(tmp)

                            //JsEx.delay(lo_poplayout,100,function()
                            //{
                            //    ls_year.highlightMoveDuration =0
                            //    if(ls_year.type === 2)
                            //        ls_year.currentIndex = ls_year.count/2 + (control.selectedTime.getMonth() - lo_poplayout.currentDateTime.getMonth())
                            //    else if(ls_year.type === 3)
                            //        ls_year.currentIndex = ls_year.count/2 + (control.selectedTime.getDate() - lo_poplayout.currentDateTime.getDate())
                            //    else if(ls_year.type === 4)
                            //        ls_year.currentIndex = ls_year.count/2 + (control.selectedTime.getHours() - lo_poplayout.currentDateTime.getHours())
                            //    else if(ls_year.type === 5)
                            //        ls_year.currentIndex = ls_year.count/2 + (control.selectedTime.getMinutes() - lo_poplayout.currentDateTime.getMinutes())
                            //    else if(ls_year.type === 6)
                            //        ls_year.currentIndex = ls_year.count/2 + (control.selectedTime.getSeconds() - lo_poplayout.currentDateTime.getSeconds())
                            //    ls_year.highlightMoveDuration = -1


                            //    control.forceRefreshData()
                            //    control.forceRefreshMonth()
                            //})
                               // control.forceRefreshData()
                               // control.forceRefreshMonth()

                        }

                        delegate: Item {
                            width: ListView.view.width
                            height: pop_calendar.rowHeight
                            Rectangle{
                                id:delegateRoot
                                property bool isSelected: parent.ListView.isCurrentItem
                                anchors.centerIn: parent
                                //color: parent.ListView.isCurrentItem ? "red" : "steelblue"
                                width: parent.width
                                height: pop_calendar.rowHeight
                                color: "transparent"
                                Text {
                                    font.bold: true
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    text:   {
                                        var result =  lo_poplayout.currentDateTime?((control.refreshDate?"":"")+ lo_poplayout.calText(model.index,ls_year)):""
                                        return result
                                    }
                                    onTextChanged: {
                                        //var backup_index = ls_year.currentIndex
                                        //ls_year.currentIndex = -1
                                        //ls_year.currentIndex = backup_index
                                    }

                                    MouseArea{
                                        anchors.fill: parent
                                        onClicked: {
                                            ls_year.currentIndex = model.index
                                        }
                                    }
                                }
                                scale: {
                                    var result = 2.5 - 1 * Math.abs(model.index - ls_year.currentIndex)
                                    if(result <1)
                                        result = 1
                                    return result
                                }
                                Behavior on scale {
                                    NumberAnimation { duration: 100
                                    }
                                }
                            }
                        }
                    }
                }
                //倒序的原因，让年月先设置值，日期才算得准
                layoutDirection: ListView.RightToLeft
                Loader{
                    property int type : 6 // year1 ...2 ...3 ...4 ...5 second6
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    sourceComponent: lo_poplayout.comp_listview
                }
                Loader{
                    property int type : 5 // year1 ...2 ...3 ...4 ...5 second6
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    sourceComponent: lo_poplayout.comp_listview
                }
                Loader{
                    property int type : 4 // year1 ...2 ...3 ...4 ...5 second6
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    sourceComponent: lo_poplayout.comp_listview
                }
                Item {
                    Layout.preferredWidth: 30
                }
                Loader{
                    property int type : 3 // year1 ...2 ...3 ...4 ...5 second6
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    sourceComponent: lo_poplayout.comp_listview
                }
                Loader{
                    property int type : 2 // year1 ...2 ...3 ...4 ...5 second6
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    sourceComponent: lo_poplayout.comp_listview
                }
                Loader{
                    property int type : 1 // year1 ...2 ...3 ...4 ...5 second6
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/5
                    sourceComponent: lo_poplayout.comp_listview
                }
            }
        }

    }

}
