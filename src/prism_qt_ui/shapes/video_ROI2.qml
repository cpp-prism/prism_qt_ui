import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.4 as Q1
import QtQuick.Controls.Styles 1.4
import prismCpp 1.0
import prism_qt_ui 1.0
import prism_qt_ui_private 1.0

    Video_roi_cpp {
        id:root_marsk
        anchors.fill: parent

        property int view_scale :1
        //property int roi_x:0
        //property int roi_y:0
        //property int roi_width:1
        //property int roi_height:1

        property bool disableDragAndResize: false
        property alias rect_roi :rect_roi

        property int frameWidth : 100
        property int frameHeight :  100

        property color color: "transparent"
        property color borderColor: "blue"
        property int borderWidth: 1

        signal clicked(var x,var y)
        signal deleteKeydown(var e)
        signal deleteKeyup(var e)
        signal leftKeydown(var e)
        signal leftKeyup(var e)
        signal rightKeydown(var e)
        signal rightKeyup(var e)

        property alias mouseArea_drag:ma_drag

        z:99
        Rectangle {
            id:rect_roi
            z:100

            color: root_marsk.color
            border.color: root_marsk.borderColor
            border.width: root_marsk.borderWidth
            property int frameWidth : parent.frameWidth
            property int frameHeight :  parent.frameHeight
            property bool  isFillWidth:{
                if((frameHeight * root_marsk.width/frameWidth) <= root_marsk.height)
                    return true
                return false
            }
            property int roi_x:root_marsk.roi_x
            property int roi_y:root_marsk.roi_y
            property int roi_width:root_marsk.roi_width
            property int roi_height:root_marsk.roi_height

            onRoi_xChanged:  root_marsk.setRoi_x(rect_roi.roi_x)
            onRoi_yChanged:  root_marsk.setRoi_y(roi_y)
            onRoi_widthChanged:  root_marsk.setRoi_width(roi_width)
            onRoi_heightChanged:  root_marsk.setRoi_height(roi_height)



            property int pixel_control_width:  (isFillWidth? root_marsk.width : frameWidth * root_marsk.height/frameHeight)
            property int pixel_control_height: (!isFillWidth? root_marsk.height : frameHeight * root_marsk.width/frameWidth)
            property real x_scale_factor :pixel_control_width /frameWidth
            property real y_scale_factor :pixel_control_height /frameHeight

            width: roi_width * x_scale_factor

            height: roi_height * y_scale_factor
            x: (root_marsk.width - pixel_control_width)/2  + roi_x * x_scale_factor
            y:(root_marsk.height - pixel_control_height)/2 + roi_y * y_scale_factor


            property int guard_radius :  4 * view_scale
        }
        //drag
        MouseArea{
            id:ma_drag
            visible: rect_roi.visible
            anchors.fill : rect_roi
            property int roi_x
            property int roi_y
            property point point_pressed
            drag.threshold: 0
            cursorShape: Qt.SizeAllCursor


            focus: true
            Keys.enabled: true
            Keys.onPressed: function(e) {
                if(e.key === Qt.Key_Delete
                        ||e.key === Qt.Key_Backspace)
                {
                    root_marsk.deleteKeydown(e)
                }
                if(e.key === Qt.Key_Left)
                {
                    root_marsk.leftKeydown(e)
                }
                if(e.key === Qt.Key_Right)
                {
                    root_marsk.rightKeydown(e)
                }
            }
            Keys.onReleased: function(e) {
                if(e.key === Qt.Key_Delete
                        ||e.key === Qt.Key_Backspace)
                {
                    root_marsk.deleteKeyup(e)
                }
                if(e.key === Qt.Key_Left)
                {
                    root_marsk.leftKeyup(e)
                }
                if(e.key === Qt.Key_Right)
                {
                    root_marsk.rightKeyup(e)
                }
            }
            onClicked: function(e){
                ma_drag.forceActiveFocus()
               root_marsk.clicked(e.x/rect_roi.x_scale_factor, e.y/rect_roi.y_scale_factor)
            }
            onPressed: {
                CppUtility.setCursor(cursorShape)
                roi_x = rect_roi.roi_x
                roi_y = rect_roi.roi_y
                point_pressed = CppUtility.getMousePos()
            }
            function updatePosition(){
                if(pressed && !disableDragAndResize )
                {
                    var point_moving = CppUtility.getMousePos()
                    rect_roi.roi_x  = roi_x + view_scale * (point_moving.x - point_pressed.x) / rect_roi.x_scale_factor
                    rect_roi.roi_y  = roi_y + view_scale * (point_moving.y - point_pressed.y) / rect_roi.y_scale_factor

                    if(rect_roi.roi_x <0)
                    {
                        rect_roi.roi_x = 0
                    }
                    if(rect_roi.roi_x > rect_roi.frameWidth- rect_roi.roi_width)
                    {
                        rect_roi.roi_x = rect_roi.frameWidth- rect_roi.roi_width
                    }

                    if(rect_roi.roi_y <0)
                        rect_roi.roi_y = 0
                    if(rect_roi.roi_y > rect_roi.frameHeight- rect_roi.roi_height)
                        rect_roi.roi_y = rect_roi.frameHeight- rect_roi.roi_height

                }
            }
            onMouseXChanged: updatePosition()
            onMouseYChanged: updatePosition()
            onReleased: {
                CppUtility.restoreCursor()
            }

        }

        //左上
        Rectangle{
            visible: rect_roi.visible && !disableDragAndResize
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.left: rect_roi.left
            anchors.top: rect_roi.top
            anchors.margins: -rect_roi.guard_radius

            color: borderColor
            MouseArea{
                anchors.fill : parent
                property int roi_x
                property int roi_y
                property int roi_width
                property int roi_height
                property point point_pressed
                property point map2frame;
                property point frame_global;
                cursorShape: Qt.SizeFDiagCursor
                onPressed: {
                    if(rect_roi.isFillWidth)
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius,rect_roi.guard_radius - (root_marsk.height- rect_roi.pixel_control_height)/2)
                    else
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius- (root_marsk.width- rect_roi.pixel_control_width)/2,rect_roi.guard_radius)

                    frame_global = CppUtility.getMousePos()

                    frame_global.x -= map2frame.x / view_scale
                    frame_global.y -= map2frame.y / view_scale

                    CppUtility.setCursor(cursorShape)
                    roi_x = rect_roi.roi_x
                    roi_y = rect_roi.roi_y
                    roi_width = rect_roi.roi_width
                    roi_height = rect_roi.roi_height
                    point_pressed = CppUtility.getMousePos()
                }
                function updatePosition(){
                    if(pressed  && !disableDragAndResize)
                    {
                        var point_moving = CppUtility.getMousePos()
                        if(point_moving.x < frame_global.x)
                            point_moving.x = frame_global.x
                        if((point_moving.x - frame_global.x)* view_scale > rect_roi.frameWidth * rect_roi.x_scale_factor)
                            point_moving.x = (rect_roi.frameWidth * rect_roi.x_scale_factor) / view_scale + frame_global.x
                        rect_roi.roi_x  = roi_x +  view_scale * (point_moving.x - point_pressed.x) / rect_roi.x_scale_factor
                        rect_roi.roi_width  = roi_width -  view_scale * (point_moving.x - point_pressed.x)/rect_roi.y_scale_factor
                        if(rect_roi.roi_width<0)
                        {
                            rect_roi.roi_width *= -1
                            rect_roi.roi_x  =  + roi_x + (view_scale *(point_moving.x - point_pressed.x )-rect_roi.width) / rect_roi.x_scale_factor
                        }

                        if(point_moving.y < frame_global.y)
                            point_moving.y = frame_global.y
                        if((point_moving.y - frame_global.y)*view_scale > rect_roi.frameHeight * rect_roi.y_scale_factor)
                            point_moving.y = (rect_roi.frameHeight * rect_roi.y_scale_factor) / view_scale +  frame_global.y
                        rect_roi.roi_y  = roi_y + view_scale *(point_moving.y - point_pressed.y) / rect_roi.y_scale_factor
                        rect_roi.roi_height  = roi_height - view_scale *(point_moving.y - point_pressed.y)/rect_roi.y_scale_factor
                        if(rect_roi.roi_height<0)
                        {
                            rect_roi.roi_height *= -1
                            rect_roi.roi_y  = roi_y + (view_scale *(point_moving.y - point_pressed.y )- rect_roi.height) / rect_roi.y_scale_factor

                        }
                    }
                }
                onMouseXChanged: updatePosition()
                onMouseYChanged: updatePosition()
                onReleased: {
                    CppUtility.restoreCursor()
                }

            }

        }
        //左中
        Rectangle{
            visible: rect_roi.visible && !disableDragAndResize
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.left: rect_roi.left
            anchors.verticalCenter: rect_roi.verticalCenter
            anchors.margins: -rect_roi.guard_radius

            color: borderColor

            MouseArea{
                anchors.fill : parent
                property int roi_x
                property int roi_y
                property int roi_width
                property int roi_height
                property point point_pressed
                property point map2frame;
                property point frame_global;
                cursorShape: Qt.SizeHorCursor
                onPressed: {
                    if(rect_roi.isFillWidth)
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius,rect_roi.guard_radius - (root_marsk.height- rect_roi.pixel_control_height)/2)
                    else
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius- (root_marsk.width- rect_roi.pixel_control_width)/2,rect_roi.guard_radius)

                    frame_global = CppUtility.getMousePos()

                    frame_global.x -= map2frame.x/view_scale
                    frame_global.y -= map2frame.y/view_scale

                    CppUtility.setCursor(cursorShape)
                    roi_x = rect_roi.roi_x
                    roi_y = rect_roi.roi_y
                    roi_width = rect_roi.roi_width
                    roi_height = rect_roi.roi_height
                    point_pressed = CppUtility.getMousePos()
                }
                function updatePosition(){
                    if(pressed  && !disableDragAndResize)
                    {
                        var point_moving = CppUtility.getMousePos()
                        if(point_moving.x < frame_global.x)
                            point_moving.x = frame_global.x
                        if((point_moving.x - frame_global.x)*view_scale > rect_roi.frameWidth * rect_roi.x_scale_factor)
                            point_moving.x = (rect_roi.frameWidth * rect_roi.x_scale_factor) / view_scale + frame_global.x
                        rect_roi.roi_x  = roi_x +  view_scale * (point_moving.x - point_pressed.x) / rect_roi.x_scale_factor
                        rect_roi.roi_width  = roi_width -  view_scale * (point_moving.x - point_pressed.x)/rect_roi.y_scale_factor
                        if(rect_roi.roi_width<0)
                        {
                            rect_roi.roi_width *= -1
                            rect_roi.roi_x  =  + roi_x + (view_scale *(point_moving.x - point_pressed.x )-rect_roi.width) / rect_roi.x_scale_factor
                        }

                    }
                }
                onMouseXChanged: updatePosition()
                onMouseYChanged: updatePosition()
                onReleased: {
                    CppUtility.restoreCursor()
                }

            }

        }
        //左下
        Rectangle{
            visible: rect_roi.visible && !disableDragAndResize
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.left: rect_roi.left
            anchors.bottom: rect_roi.bottom
            anchors.margins: -rect_roi.guard_radius

            color: borderColor
            MouseArea{
                anchors.fill : parent
                property int roi_x
                property int roi_y
                property int roi_width
                property int roi_height
                property point point_pressed
                property point map2frame;
                property point frame_global;
                cursorShape: Qt.SizeBDiagCursor
                onPressed: {
                    if(rect_roi.isFillWidth)
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius,rect_roi.guard_radius - (root_marsk.height- rect_roi.pixel_control_height)/2)
                    else
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius- (root_marsk.width- rect_roi.pixel_control_width)/2,rect_roi.guard_radius)

                    frame_global = CppUtility.getMousePos()

                    frame_global.x -= map2frame.x/view_scale
                    frame_global.y -= map2frame.y/view_scale

                    CppUtility.setCursor(cursorShape)
                    roi_x = rect_roi.roi_x
                    roi_y = rect_roi.roi_y
                    roi_width = rect_roi.roi_width
                    roi_height = rect_roi.roi_height
                    point_pressed = CppUtility.getMousePos()
                }
                function updatePosition(){
                    if(pressed  && !disableDragAndResize)
                    {
                        var point_moving = CppUtility.getMousePos()
                        if(point_moving.x < frame_global.x)
                            point_moving.x = frame_global.x
                        if((point_moving.x - frame_global.x)*view_scale > rect_roi.frameWidth * rect_roi.x_scale_factor)
                            point_moving.x = (rect_roi.frameWidth * rect_roi.x_scale_factor)/view_scale + frame_global.x
                        rect_roi.roi_x  = roi_x +  view_scale * (point_moving.x - point_pressed.x) / rect_roi.x_scale_factor
                        rect_roi.roi_width  = roi_width -  view_scale * (point_moving.x - point_pressed.x)/rect_roi.y_scale_factor
                        if(rect_roi.roi_width<0)
                        {
                            rect_roi.roi_width *= -1
                            rect_roi.roi_x  =  + roi_x + (view_scale *(point_moving.x - point_pressed.x )-rect_roi.width)/ rect_roi.x_scale_factor
                        }

                        if(point_moving.y < frame_global.y)
                            point_moving.y = frame_global.y
                        if((point_moving.y - frame_global.y) * view_scale > rect_roi.frameHeight * rect_roi.y_scale_factor)
                            point_moving.y =(rect_roi.frameHeight * rect_roi.y_scale_factor) /view_scale + frame_global.y
                        rect_roi.roi_height  =  roi_height + view_scale *(point_moving.y - point_pressed.y) / rect_roi.y_scale_factor
                        rect_roi.roi_y  = roi_y /*- (point_moving.y - point_pressed.y)/rect_roi.y_scale_factor*/
                        if(rect_roi.roi_height<0)
                        {
                            rect_roi.roi_height *= -1
                            rect_roi.roi_y  = roi_y + (/*point_moving.y - point_pressed.y*/ - rect_roi.height) / rect_roi.y_scale_factor

                        }
                    }
                }
                onMouseXChanged: updatePosition()
                onMouseYChanged: updatePosition()
                onReleased: {
                    CppUtility.restoreCursor()
                }

            }

        }
        //右上
        Rectangle{
            visible: rect_roi.visible && !disableDragAndResize
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.right: rect_roi.right
            anchors.top: rect_roi.top
            anchors.margins: -rect_roi.guard_radius

            color: borderColor
            MouseArea{
                anchors.fill : parent
                property int roi_x
                property int roi_y
                property int roi_width
                property int roi_height
                property point point_pressed
                property point map2frame;
                property point frame_global;
                cursorShape: Qt.SizeBDiagCursor
                onPressed: {
                    if(rect_roi.isFillWidth)
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius,rect_roi.guard_radius - (root_marsk.height- rect_roi.pixel_control_height)/2)
                    else
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius- (root_marsk.width- rect_roi.pixel_control_width)/2,rect_roi.guard_radius)

                    frame_global = CppUtility.getMousePos()

                    frame_global.x -= map2frame.x / view_scale
                    frame_global.y -= map2frame.y / view_scale

                    CppUtility.setCursor(cursorShape)
                    roi_x = rect_roi.roi_x
                    roi_y = rect_roi.roi_y
                    roi_width = rect_roi.roi_width
                    roi_height = rect_roi.roi_height
                    point_pressed = CppUtility.getMousePos()
                }
                function updatePosition(){
                    if(pressed  && !disableDragAndResize)
                    {
                        var point_moving = CppUtility.getMousePos()
                        if(point_moving.x < frame_global.x)
                            point_moving.x = frame_global.x
                        if((point_moving.x - frame_global.x)* view_scale >  rect_roi.frameWidth * rect_roi.x_scale_factor)
                            point_moving.x = (rect_roi.frameWidth * rect_roi.x_scale_factor) / view_scale + frame_global.x
                        rect_roi.roi_x  = roi_x /*+ (point_moving.x - point_pressed.x) / rect_roi.x_scale_factor*/
                        rect_roi.roi_width  = roi_width +  view_scale * (point_moving.x - point_pressed.x)/rect_roi.y_scale_factor
                        if(rect_roi.roi_width<0)
                        {
                            rect_roi.roi_width *= -1
                            rect_roi.roi_x  =  + roi_x + (/*point_moving.x - point_pressed.x*/ -rect_roi.width) / rect_roi.x_scale_factor
                        }

                        if(point_moving.y < frame_global.y)
                            point_moving.y = frame_global.y
                        if((point_moving.y - frame_global.y)*view_scale > rect_roi.frameHeight * rect_roi.y_scale_factor)
                            point_moving.y = (rect_roi.frameHeight * rect_roi.y_scale_factor) / view_scale + frame_global.y
                        rect_roi.roi_y  = roi_y + view_scale *(point_moving.y - point_pressed.y) / rect_roi.y_scale_factor
                        rect_roi.roi_height  = roi_height - view_scale *(point_moving.y - point_pressed.y)/rect_roi.y_scale_factor
                        if(rect_roi.roi_height<0)
                        {
                            rect_roi.roi_height *= -1
                            rect_roi.roi_y  = roi_y + (view_scale *(point_moving.y - point_pressed.y)- rect_roi.height) / rect_roi.y_scale_factor

                        }
                    }
                }
                onMouseXChanged: updatePosition()
                onMouseYChanged: updatePosition()
                onReleased: {
                    CppUtility.restoreCursor()
                }

            }

        }
        //右中
        Rectangle{
            visible: rect_roi.visible && !disableDragAndResize
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.right: rect_roi.right
            anchors.verticalCenter: rect_roi.verticalCenter
            anchors.margins: -rect_roi.guard_radius

            color: borderColor
            MouseArea{
                anchors.fill : parent
                property int roi_x
                property int roi_y
                property int roi_width
                property int roi_height
                property point point_pressed
                property point map2frame;
                property point frame_global;
                cursorShape: Qt.SizeHorCursor
                onPressed: {
                    if(rect_roi.isFillWidth)
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius,rect_roi.guard_radius - (root_marsk.height- rect_roi.pixel_control_height)/2)
                    else
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius- (root_marsk.width- rect_roi.pixel_control_width)/2,rect_roi.guard_radius)

                    frame_global = CppUtility.getMousePos()

                    frame_global.x -= map2frame.x / view_scale
                    frame_global.y -= map2frame.y / view_scale

                    CppUtility.setCursor(cursorShape)
                    roi_x = rect_roi.roi_x
                    roi_y = rect_roi.roi_y
                    roi_width = rect_roi.roi_width
                    roi_height = rect_roi.roi_height
                    point_pressed = CppUtility.getMousePos()
                }
                function updatePosition(){
                    if(pressed  && !disableDragAndResize)
                    {
                        var point_moving = CppUtility.getMousePos()
                        if(point_moving.x < frame_global.x)
                            point_moving.x = frame_global.x
                        if((point_moving.x - frame_global.x)* view_scale > rect_roi.frameWidth * rect_roi.x_scale_factor)
                            point_moving.x = (rect_roi.frameWidth * rect_roi.x_scale_factor) /view_scale + frame_global.x
                        rect_roi.roi_x  = roi_x /*+ (point_moving.x - point_pressed.x) / rect_roi.x_scale_factor*/
                        rect_roi.roi_width  = roi_width +  view_scale * (point_moving.x - point_pressed.x)/rect_roi.y_scale_factor
                        if(rect_roi.roi_width<0)
                        {
                            rect_roi.roi_width *= -1
                            rect_roi.roi_x  =  + roi_x + (/*point_moving.x - point_pressed.x*/ -rect_roi.width) / rect_roi.x_scale_factor
                        }

                    }
                }
                onMouseXChanged: updatePosition()
                onMouseYChanged: updatePosition()
                onReleased: {
                    CppUtility.restoreCursor()
                }

            }

        }
        //右下
        Rectangle{
            visible: rect_roi.visible && !disableDragAndResize
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.right: rect_roi.right
            anchors.bottom: rect_roi.bottom
            anchors.margins: -rect_roi.guard_radius

            color: borderColor
            MouseArea{
                anchors.fill : parent
                property int roi_x
                property int roi_y
                property int roi_width
                property int roi_height
                property point point_pressed
                property point map2frame;
                property point frame_global;
                cursorShape: Qt.SizeFDiagCursor
                onPressed: {
                    if(rect_roi.isFillWidth)
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius,rect_roi.guard_radius - (root_marsk.height- rect_roi.pixel_control_height)/2)
                    else
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius- (root_marsk.width- rect_roi.pixel_control_width)/2,rect_roi.guard_radius)

                    frame_global = CppUtility.getMousePos()

                    frame_global.x -= map2frame.x/view_scale
                    frame_global.y -= map2frame.y/view_scale

                    CppUtility.setCursor(cursorShape)
                    roi_x = rect_roi.roi_x
                    roi_y = rect_roi.roi_y
                    roi_width = rect_roi.roi_width
                    roi_height = rect_roi.roi_height
                    point_pressed = CppUtility.getMousePos()
                }
                function updatePosition(){
                    if(pressed  && !disableDragAndResize)
                    {
                        var point_moving = CppUtility.getMousePos()
                        if(point_moving.x < frame_global.x)
                            point_moving.x = frame_global.x
                        if((point_moving.x - frame_global.x)*view_scale > rect_roi.frameWidth * rect_roi.x_scale_factor)
                            point_moving.x = (rect_roi.frameWidth * rect_roi.x_scale_factor)/view_scale + frame_global.x
                        rect_roi.roi_x  = roi_x /*+ (point_moving.x - point_pressed.x) / rect_roi.x_scale_factor*/
                        rect_roi.roi_width  = roi_width +  view_scale * (point_moving.x - point_pressed.x)/rect_roi.y_scale_factor
                        if(rect_roi.roi_width<0)
                        {
                            rect_roi.roi_width *= -1
                            rect_roi.roi_x  =  + roi_x + (/*point_moving.x - point_pressed.x*/ -rect_roi.width) / rect_roi.x_scale_factor
                        }

                        if(point_moving.y < frame_global.y)
                            point_moving.y = frame_global.y
                        if((point_moving.y - frame_global.y) *view_scale > rect_roi.frameHeight * rect_roi.y_scale_factor)
                            point_moving.y = (rect_roi.frameHeight * rect_roi.y_scale_factor) / view_scale + frame_global.y
                        rect_roi.roi_y  = roi_y /*+ (point_moving.y - point_pressed.y) / rect_roi.y_scale_factor*/
                        rect_roi.roi_height  = roi_height + view_scale *(point_moving.y - point_pressed.y)/rect_roi.y_scale_factor
                        if(rect_roi.roi_height<0)
                        {
                            rect_roi.roi_height *= -1
                            rect_roi.roi_y  = roi_y + (/*point_moving.y - point_pressed.y*/ - rect_roi.height) / rect_roi.y_scale_factor

                        }
                    }
                }
                onMouseXChanged: updatePosition()
                onMouseYChanged: updatePosition()
                onReleased: {
                    CppUtility.restoreCursor()
                }

            }

        }
        //中上
        Rectangle{
            visible: rect_roi.visible && !disableDragAndResize
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.top: rect_roi.top
            anchors.horizontalCenter: rect_roi.horizontalCenter
            anchors.margins: -rect_roi.guard_radius

            color: borderColor
            MouseArea{
                anchors.fill : parent
                property int roi_x
                property int roi_y
                property int roi_width
                property int roi_height
                property point point_pressed
                property point map2frame;
                property point frame_global;
                cursorShape: Qt.SizeVerCursor
                onPressed: {
                    if(rect_roi.isFillWidth)
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius,rect_roi.guard_radius - (root_marsk.height- rect_roi.pixel_control_height)/2)
                    else
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius- (root_marsk.width- rect_roi.pixel_control_width)/2,rect_roi.guard_radius)

                    frame_global = CppUtility.getMousePos()

                    frame_global.x -= map2frame.x/view_scale
                    frame_global.y -= map2frame.y/view_scale

                    CppUtility.setCursor(cursorShape)
                    roi_x = rect_roi.roi_x
                    roi_y = rect_roi.roi_y
                    roi_width = rect_roi.roi_width
                    roi_height = rect_roi.roi_height
                    point_pressed = CppUtility.getMousePos()
                }
                function updatePosition(){
                    if(pressed  && !disableDragAndResize)
                    {
                        var point_moving = CppUtility.getMousePos()

                        if(point_moving.y < frame_global.y)
                            point_moving.y = frame_global.y
                        if((point_moving.y - frame_global.y)* view_scale > rect_roi.frameHeight * rect_roi.y_scale_factor)
                            point_moving.y = (rect_roi.frameHeight * rect_roi.y_scale_factor)/view_scale + frame_global.y
                        rect_roi.roi_y  = roi_y +  view_scale * (point_moving.y - point_pressed.y) / rect_roi.y_scale_factor
                        rect_roi.roi_height  = roi_height -  view_scale * (point_moving.y - point_pressed.y)/rect_roi.y_scale_factor
                        if(rect_roi.roi_height<0)
                        {
                            rect_roi.roi_height *= -1
                            rect_roi.roi_y  = roi_y + (view_scale *(point_moving.y - point_pressed.y) - rect_roi.height)/ rect_roi.y_scale_factor

                        }
                    }
                }
                onMouseXChanged: updatePosition()
                onMouseYChanged: updatePosition()
                onReleased: {
                    CppUtility.restoreCursor()
                }

            }

        }
        //中下
        Rectangle{
            visible: rect_roi.visible && !disableDragAndResize
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.bottom: rect_roi.bottom
            anchors.horizontalCenter: rect_roi.horizontalCenter
            anchors.margins: -rect_roi.guard_radius

            color: borderColor
            MouseArea{
                anchors.fill : parent
                property int roi_x
                property int roi_y
                property int roi_width
                property int roi_height
                property point point_pressed
                property point map2frame;
                property point frame_global;
                cursorShape: Qt.SizeVerCursor
                onPressed: {
                    if(rect_roi.isFillWidth)
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius,rect_roi.guard_radius - (root_marsk.height- rect_roi.pixel_control_height)/2)
                    else
                        map2frame = mapToItem(root_marsk,rect_roi.guard_radius- (root_marsk.width- rect_roi.pixel_control_width)/2,rect_roi.guard_radius)

                    frame_global = CppUtility.getMousePos()

                    frame_global.x -= map2frame.x/view_scale
                    frame_global.y -= map2frame.y/view_scale

                    CppUtility.setCursor(cursorShape)
                    roi_x = rect_roi.roi_x
                    roi_y = rect_roi.roi_y
                    roi_width = rect_roi.roi_width
                    roi_height = rect_roi.roi_height
                    point_pressed = CppUtility.getMousePos()
                }
                function updatePosition(){
                    if(pressed  && !disableDragAndResize)
                    {
                        var point_moving = CppUtility.getMousePos()

                        if(point_moving.y < frame_global.y)
                            point_moving.y = frame_global.y

                        if((point_moving.y - frame_global.y)*view_scale >  rect_roi.frameHeight * rect_roi.y_scale_factor)
                            point_moving.y =   (rect_roi.frameHeight * rect_roi.y_scale_factor)/view_scale + frame_global.y
                        rect_roi.roi_height  = roi_height +  view_scale * (point_moving.y - point_pressed.y) / rect_roi.y_scale_factor
                        rect_roi.roi_y  = roi_y /*- (point_moving.y - point_pressed.y)/rect_roi.y_scale_factor*/
                        if(rect_roi.roi_height<0)
                        {
                            rect_roi.roi_height *= -1
                            rect_roi.roi_y  = roi_y + (/*point_moving.y - point_pressed.y*/ - rect_roi.height) / rect_roi.y_scale_factor

                        }
                    }
                }
                onMouseXChanged: updatePosition()
                onMouseYChanged: updatePosition()
                onReleased: {
                    CppUtility.restoreCursor()
                }

            }

        }


    }
