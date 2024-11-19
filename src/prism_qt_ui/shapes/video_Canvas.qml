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
    id: root_marsk
    anchors.fill: parent

    property int view_scale: 1
    property int roi_x: 0
    property int roi_y: 0
    property int roi_width: 1
    property int roi_height: 1
    property color strokeColor : "blue"

    property alias rect_roi: rect_roi
    property alias canvas: canvas

    property int frameWidth: 100
    property int frameHeight: 100
    signal drawCompleted(int x,int y, int width, int height);

    z: 99
    Rectangle {
        id: rect_roi
        z: 100
        property int frameWidth: parent.frameWidth
        property int frameHeight: parent.frameHeight
        property bool isFillWidth: {
            if ((frameHeight * root_marsk.width / frameWidth) <= root_marsk.height)
                return true
            return false
        }
        property int roi_x: root_marsk.roi_x
        property int roi_y: root_marsk.roi_y
        property int roi_width: root_marsk.roi_width
        property int roi_height: root_marsk.roi_height

        onRoi_xChanged: {
            if (roi_x !== root_marsk.roi_x)
                root_marsk.roi_xChanged(roi_x)
        }
        onRoi_yChanged: {
            if (roi_y !== root_marsk.roi_y)
                root_marsk.roi_yChanged(roi_y)
        }
        onRoi_widthChanged: {
            if (roi_width !== root_marsk.roi_width)
                root_marsk.roi_widthChanged(roi_width)
        }
        onRoi_heightChanged: {
            if (roi_height !== root_marsk.roi_height)
                root_marsk.roi_heightChanged(roi_height)
        }

        property int pixel_control_width: (isFillWidth ? root_marsk.width : frameWidth
                                                         * root_marsk.height / frameHeight)
        property int pixel_control_height: (!isFillWidth ? root_marsk.height : frameHeight
                                                           * root_marsk.width / frameWidth)
        property real x_scale_factor: pixel_control_width / frameWidth
        property real y_scale_factor: pixel_control_height / frameHeight

        width: roi_width * x_scale_factor

        height: roi_height * y_scale_factor
        x: (root_marsk.width - pixel_control_width) / 2 + roi_x * x_scale_factor
        y: (root_marsk.height - pixel_control_height) / 2 + roi_y * y_scale_factor

        color: "transparent"


        Canvas {
            id: canvas
            anchors.fill: parent
            focus: true

            property point startPoint: Qt.point(0, 0)
            property point endPoint: Qt.point(0, 0)
            property bool isDrawing: false
            property var last_x;
            property var last_y;
            property var last_w;
            property var last_h;
            property var cross_mousex
            property var cross_mousey

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                    ctx.strokeStyle = root_marsk.strokeColor
                if (isDrawing) {
                    if(endPoint.x < 0)
                        var endx = 0
                    else if(endPoint.x > width)
                        endx= width
                    else
                        endx = endPoint.x

                    if(endPoint.y <0)
                        var endy = 0
                    else if(endPoint.y> height)
                        endy = height
                    else
                        endy = endPoint.y


                    last_x = Math.min(startPoint.x, endx)
                    last_y = Math.min(startPoint.y, endy)
                    last_w = Math.abs(startPoint.x - endx)
                    last_h = Math.abs(startPoint.y - endy)


                    ctx.strokeRect(last_x, last_y, last_w, last_h)
                }

                //ctx.beginPath()
                //ctx.lineWidth = 1
                //ctx.moveTo(cross_mousex,0)
                //ctx.lineTo(cross_mousex,canvas.height)

                //ctx.moveTo(0,cross_mousey)
                //ctx.lineTo(canvas.width,cross_mousey)

                //ctx.stroke()

            }

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    canvas.startPoint = Qt.point(mouse.x, mouse.y)
                    canvas.endPoint = Qt.point(mouse.x, mouse.y)
                    canvas.isDrawing = true
                    canvas.requestPaint()
                }

                onPositionChanged: {
                    if (canvas.isDrawing) {
                        canvas.endPoint = Qt.point(mouse.x, mouse.y)
                        canvas.requestPaint()
                    }
                }

                onReleased: {
                    canvas.isDrawing = false
                    canvas.requestPaint()
                    drawCompleted(canvas.last_x * root_marsk.frameWidth / width,
                                  canvas.last_y * root_marsk.frameHeight / height,
                                  canvas.last_w * root_marsk.frameWidth / width,
                                  canvas.last_h * root_marsk.frameHeight / height)
                }
            }
        }
    }
}
