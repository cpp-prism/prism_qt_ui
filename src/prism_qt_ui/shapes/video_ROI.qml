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

    Item {
        id:root_marsk
        anchors.fill: parent

        property int view_scale :1
        property int roi_x_b :0
        property int roi_y_b :0
        property int roi_width_b :0
        property int roi_height_b :0

        property alias rect_roi :rect_roi

        property real frameWidth : 100
        property real frameHeight :  100

        //property int roi_x_b: root._3d_cam_setting ? Bind.create(root._3d_cam_setting,"roi_info.offsetX"):0
        //property int roi_y_b: root._3d_cam_setting ? Bind.create(root._3d_cam_setting,"roi_info.offsetY"):0
        //property int roi_width_b: root._3d_cam_setting ? Bind.create(root._3d_cam_setting,"roi_info.width"):0
        //property int roi_height_b: root._3d_cam_setting ? Bind.create(root._3d_cam_setting,"roi_info.height"):0

        //onRoi_xChanged: {
        //    if(root._3d_cam_setting && rect_roi.roi_x !== root_marsk.roi_x_b )
        //    {
        //        root._3d_cam_setting.set("roi_info.offsetX",rect_roi.roi_x)
        //        //console.log("roi_x:",roi_x)
        //    }
        //}
        //onRoi_yChanged: {
        //    if(root._3d_cam_setting && rect_roi.roi_y !== root_marsk.roi_y_b)
        //    {
        //        root._3d_cam_setting.set("roi_info.offsetY",rect_roi.roi_y)
        //        //console.log("roi_y" , roi_y)
        //    }
        //}
        //onRoi_widthChanged: {
        //    if(root._3d_cam_setting && rect_roi.roi_width !== root_marsk.roi_width_b && rect_roi.roi_width >=0)
        //    {
        //        root._3d_cam_setting.set("roi_info.width",rect_roi.roi_width)
        //        //console.log("roi_width: ",roi_width)
        //    }
        //}
        //onRoi_heightChanged: {
        //    if(root._3d_cam_setting && rect_roi.roi_height !== root_marsk.roi_height_b && rect_roi.roi_height >= 0)
        //    {
        //        root._3d_cam_setting.set("roi_info.height",rect_roi.roi_height)
        //        //console.log("roi_height:",roi_height)
        //    }
        //}

        //visible:
        //{
        //    var showroi = root._3d_cam_setting ? Bind.create(root._3d_cam_setting,"show_ROI"):0
        //    return fram_rvm && fram_rvm !== null && fram_rvm.get("width")!==1 && fram_rvm.get("height")!==1 && showroi
        //}



        //onRoi_x_bChanged: {
        //    if(rect_roi.roi_x!==roi_x_b)
        //        rect_roi.roi_x = roi_x_b
        //}
        //onRoi_y_bChanged: {
        //    if(rect_roi.roi_y!==roi_y_b)
        //        rect_roi.roi_y = roi_y_b
        //}
        //onRoi_width_bChanged: {
        //    if(rect_roi.roi_width!==roi_width_b && roi_width_b !==0)
        //        rect_roi.roi_width = roi_width_b
        //}
        //onRoi_height_bChanged: {
        //    if(rect_roi.roi_height!==roi_height_b && roi_height_b !== 0)
        //        rect_roi.roi_height = roi_height_b
        //}

       //inner

        signal roi_xChanged(var i);
        signal roi_yChanged(var i);
        signal roi_widthChanged(var i);
        signal roi_heightChanged(var i);


        z:99
        Rectangle {
            id:rect_roi
            z:100
            property real frameWidth : parent.frameWidth
            property real frameHeight :  parent.frameHeight
            property bool  isFillWidth:{
                if((frameHeight * root_marsk.width/frameWidth) <= root_marsk.height)
                    return true
                return false
            }
            property int roi_x:0
            property int roi_y:0
            property int roi_width:1
            property int roi_height:1

            onRoi_xChanged: {
                root_marsk.roi_xChanged(roi_x)
            }
            onRoi_yChanged: {
                root_marsk.roi_yChanged(roi_y)
            }
            onRoi_widthChanged: {
                root_marsk.roi_widthChanged(roi_width)
            }
            onRoi_heightChanged: {
                root_marsk.roi_heightChanged(roi_height)
            }



            property int pixel_control_width:  (isFillWidth? root_marsk.width : frameWidth * root_marsk.height/frameHeight)
            property int pixel_control_height: (!isFillWidth? root_marsk.height : frameHeight * root_marsk.width/frameWidth)
            property real x_scale_factor :pixel_control_width /frameWidth
            property real y_scale_factor :pixel_control_height /frameHeight

            width: roi_width * x_scale_factor

            height: roi_height * y_scale_factor
            x: (root_marsk.width - pixel_control_width)/2  + roi_x * x_scale_factor
            y:(root_marsk.height - pixel_control_height)/2 + roi_y * y_scale_factor

            color: "red"
            opacity: 0.2

            property int guard_radius :  4 * view_scale
        }
        //drag
        MouseArea{
            visible: rect_roi.visible
            anchors.fill : rect_roi
            property int roi_x
            property int roi_y
            property point point_pressed
            drag.threshold: 0
            cursorShape: Qt.SizeAllCursor
            onPressed: {
                CppUtility.setCursor(cursorShape)
                roi_x = rect_roi.roi_x
                roi_y = rect_roi.roi_y
                point_pressed = CppUtility.getMousePos()
            }
            function updatePosition(){
                if(pressed )
                {
                    var point_moving = CppUtility.getMousePos()
                    rect_roi.roi_x  = roi_x + view_scale * (point_moving.x - point_pressed.x) / rect_roi.x_scale_factor
                    rect_roi.roi_y  = roi_y + view_scale * (point_moving.y - point_pressed.y) / rect_roi.y_scale_factor

                    if(rect_roi.roi_x <0)
                        rect_roi.roi_x = 0
                    if(rect_roi.roi_x > rect_roi.frameWidth- rect_roi.roi_width)
                        rect_roi.roi_x = rect_roi.frameWidth- rect_roi.roi_width

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
            visible: rect_roi.visible
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.left: rect_roi.left
            anchors.top: rect_roi.top
            anchors.margins: -rect_roi.guard_radius

            color: "green"
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
                    if(pressed )
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
            visible: rect_roi.visible
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.left: rect_roi.left
            anchors.verticalCenter: rect_roi.verticalCenter
            anchors.margins: -rect_roi.guard_radius

            color: "green"

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
                    if(pressed )
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
            visible: rect_roi.visible
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.left: rect_roi.left
            anchors.bottom: rect_roi.bottom
            anchors.margins: -rect_roi.guard_radius

            color: "green"
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
                    if(pressed )
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
            visible: rect_roi.visible
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.right: rect_roi.right
            anchors.top: rect_roi.top
            anchors.margins: -rect_roi.guard_radius

            color: "green"
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
                    if(pressed )
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
            visible: rect_roi.visible
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.right: rect_roi.right
            anchors.verticalCenter: rect_roi.verticalCenter
            anchors.margins: -rect_roi.guard_radius

            color: "green"
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
                    if(pressed )
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
            visible: rect_roi.visible
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.right: rect_roi.right
            anchors.bottom: rect_roi.bottom
            anchors.margins: -rect_roi.guard_radius

            color: "green"
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
                    if(pressed )
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
            visible: rect_roi.visible
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.top: rect_roi.top
            anchors.horizontalCenter: rect_roi.horizontalCenter
            anchors.margins: -rect_roi.guard_radius

            color: "green"
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
                    if(pressed )
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
            visible: rect_roi.visible
            height: rect_roi.guard_radius*2
            width: rect_roi.guard_radius*2
            radius: rect_roi.guard_radius

            anchors.bottom: rect_roi.bottom
            anchors.horizontalCenter: rect_roi.horizontalCenter
            anchors.margins: -rect_roi.guard_radius

            color: "green"
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
                    if(pressed )
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
