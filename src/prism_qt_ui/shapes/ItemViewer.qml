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


Rectangle {
    color: "lightgray"
    clip: true
    property Component contentItemComponent
    property bool interactive: true
    property alias contentItem : draggable
    onWidthChanged: resetZoom()
    onHeightChanged: resetZoom()

    function resetZoom()
    {
        //console.log("cam1.width",cam.width, "cam.parent.width",cam.parent.width)
        draggable.ox = draggable.width/2
        draggable.oy = draggable.height/2
        draggable.dx =0
        draggable.dy =0

        let h_scale = draggable.parent.height / draggable.height
        let w_scale = draggable.parent.width / draggable.width

        if(h_scale* draggable.height <= draggable.parent.height+0.1 &&  h_scale * draggable.width <=draggable.parent.width + 0.1)
            draggable.scale = h_scale
        else
            draggable.scale = w_scale
    }

    Item{
        id:draggable
        anchors.centerIn: parent
        transformOrigin: Item.Center
        property real scale:0.1
        property real ox : width/2
        property real oy : height/2
        property real dx :0
        property real dy :0
        transform: [
            Scale{
                origin.x:  draggable.ox
                origin.y:  draggable.oy
                xScale: draggable.scale
                yScale: draggable.scale
            },
            Translate {
                x:draggable.dx
                y:draggable.dy
            }

        ]
        Loader{
            anchors.fill: parent
            sourceComponent: contentItemComponent
        }
    }
    MouseArea{
        enabled: interactive
        anchors.fill: parent
        property bool dragging : false
        property real startX : 0
        property real startY : 0
        onPressed: {
            dragging = true
            startX = mouse.x
            startY = mouse.y
        }
        onReleased:{
            dragging = false
        }
        onPositionChanged: {
            if(dragging)
            {
                draggable.dx += mouse.x - startX
                draggable.dy += mouse.y - startY
                startX = mouse.x
                startY = mouse.y
            }
        }

        onWheel: {
            if(wheel.angleDelta.y>0) {
                var p = mapToItem(draggable,wheel.x,wheel.y)
                var pre_ox =draggable.ox
                var pre_oy =draggable.oy
                var pre_scale = draggable.scale
                draggable.ox = p.x
                draggable.oy = p.y
                draggable.scale +=0.1
                draggable.dx  -= (p.x - pre_ox) * (1 - pre_scale)
                draggable.dy  -= (p.y - pre_oy) * (1 - pre_scale)
            }
            else{
                if(draggable.scale < 0.11)
                    return;
                p = mapToItem(draggable,wheel.x,wheel.y)
                pre_ox =draggable.ox
                pre_oy =draggable.oy
                pre_scale = draggable.scale
                draggable.ox = p.x
                draggable.oy = p.y
                draggable.scale -=0.1
                draggable.dx  -= (p.x - pre_ox) * (1 - pre_scale)
                draggable.dy  -= (p.y - pre_oy) * (1 - pre_scale)
            }
        }
    }
}
