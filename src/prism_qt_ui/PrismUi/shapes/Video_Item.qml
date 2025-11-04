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
import QtQuick.Shapes 1.15

Item {
    id: root_marsk
    anchors.fill: parent

    property double view_scale: 1
    property int roi_x: 0
    property int roi_y: 0
    property int roi_width: 1
    property int roi_height: 1
    property color strokeColor : "blue"

    default property alias content: contentPlaceholder.data

    property alias rect_roi: rect_roi

    property int frameWidth: 100
    property int frameHeight: 100
    signal drawCompleted(int x,int y, int width, int height);
    signal rightPressed();
    signal rightReleased();

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


        Item {
            width: parent.width;
            height: parent.height
            id: contentPlaceholder
        }
    }
}
